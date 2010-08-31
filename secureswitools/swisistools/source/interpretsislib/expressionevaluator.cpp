/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

#include <iostream>
#include <sstream>

// User includes
#include "sisexpression.h"
#include "expressionevaluator.h"
#include "sisstring.h"
#include "is_utils.h"
#include "errors.h"
#include "symbiantypes.h"
#include "stringutils.h"
#include "rommanager.h"
#include "configmanager.h"
#include "sisfile.h"
#include "sisregistryobject.h"
#include "logger.h"
#include "version.h"

// ExpressionResult
using namespace Utils;
ExpressionResult::ExpressionResult(const std::wstring& aString)
:   iType(EString), iString(&aString)
    {
    }


ExpressionResult::ExpressionResult(TUint32 aInt)
:   iType(EInt), iInt(aInt)
    {
    }


TUint32 ExpressionResult::IntegerValue() const
    {
	if (iType!=EInt)
	    {
		std::string error = "corrupt SIS file: expression corrupt in condition statement";
		throw InvalidSis("",error,INVALID_SIS);
    	}

    return iInt;
    }


bool ExpressionResult::BoolValue() const
    {
	return bool(IntegerValue());
    }


const std::wstring& ExpressionResult::StringValue() const
    {
	if (iType!=EString)
	    {
		std::string error = "corrupt SIS file: expression corrupt in condition statement";
		throw InvalidSis("",error,INVALID_SIS);
	    }

    return *iString;
    }


// ExpressionEvaluator

ExpressionEvaluator::ExpressionEvaluator(ExpressionEnvironment& aEnvironment)
:   iExpEnv(aEnvironment), iTempResult(false), iExpressionDepth(0)
    {
    }


void ExpressionEvaluator::Require(const void *aPointer) const
    {
	if (!aPointer)
	    {
		throw std::runtime_error("null pointer");
    	}
    }


ExpressionResult ExpressionEvaluator::Evaluate(const CSISExpression* aExpression, bool aLogInfo )
    {
	Require(aExpression);
	return Evaluate(*aExpression, aLogInfo);
    }


ExpressionResult ExpressionEvaluator::Evaluate(const CSISExpression& aExpression, bool aLogInfo)
    {
	if (++iExpressionDepth > KMaxExpressionDepth)
	    {
		iExpressionDepth=0;
		std::string error = "SIS File expression too complex\n";
		std::string x = wstring2string(iExpEnv.GetPackageName());
        //
		throw InvalidSis(x, error, SIS_NOT_SUPPORTED);
	    }

	switch (aExpression.Operator())
	    {
	case CSISExpression::EBinOpEqual:
	case CSISExpression::EBinOpNotEqual:
	case CSISExpression::EBinOpGreaterThan:
	case CSISExpression::EBinOpLessThan:
	case CSISExpression::EBinOpGreaterThanOrEqual:
	case CSISExpression::EBinOpLessThanOrEqual:
        {
		const ExpressionResult resultLeft = Evaluate( aExpression.LHS(), aLogInfo );
		const ExpressionResult resultRight = Evaluate( aExpression.RHS(), aLogInfo  );
        //
	    switch (aExpression.Operator())
	        {
	    case CSISExpression::EBinOpEqual:
            iTempResult = ( resultLeft == resultRight );
            break;
	    case CSISExpression::EBinOpNotEqual:
            iTempResult = ( resultLeft != resultRight );
            break;
	    case CSISExpression::EBinOpGreaterThan:
            iTempResult = ( resultLeft > resultRight );
            break;
	    case CSISExpression::EBinOpLessThan:
            iTempResult = ( resultLeft < resultRight );
            break;
	    case CSISExpression::EBinOpGreaterThanOrEqual:
            iTempResult = ( resultLeft >= resultRight );
            break;
	    case CSISExpression::EBinOpLessThanOrEqual:
            iTempResult = ( resultLeft <= resultRight );
            break;
            }
        //
		break;
        }
    
    case CSISExpression::ELogOpAnd:
		{
		ExpressionResult tmp1 = Evaluate(aExpression.LHS(), aLogInfo );
		ExpressionResult tmp2 = Evaluate(aExpression.RHS(), aLogInfo );
		iTempResult = ExpressionResult(tmp1.BoolValue() && tmp2.BoolValue());
		break;
		}

	case CSISExpression::ELogOpOr:
		{
		ExpressionResult tmp1 = Evaluate(aExpression.LHS(), aLogInfo );
		if (tmp1.BoolValue())
    		{
			iTempResult = ExpressionResult(true);
	    	}
		else
		    {
			iTempResult = ExpressionResult(Evaluate(aExpression.RHS(), aLogInfo)).BoolValue();
		    }
		break;
		}

	case CSISExpression::EUnaryOpNot:
		iTempResult=!Evaluate(aExpression.RHS(), aLogInfo );
		break;

	case CSISExpression::EFuncAppProperties:
        {
        const TUint32 resultLeft = Evaluate( aExpression.LHS(), aLogInfo  ).IntegerValue();
        const TUint32 resultRight = Evaluate( aExpression.RHS(), aLogInfo  ).IntegerValue();
        //
		iTempResult = iExpEnv.ApplicationProperty( resultLeft, resultRight );
		break;
        }

	case CSISExpression::EFuncDevProperties:
		iTempResult = ExpressionResult(iExpEnv.Package(Evaluate(aExpression.RHS(), aLogInfo ).IntegerValue()));
		break;

	case CSISExpression::EFuncExists:
		{
		const CSISString& pS = aExpression.String();
		if(pS.WasteOfSpace())
			{
			throw std::runtime_error("null pointer");
			}

		/**
		 * CR1125 - Add Package Versions to SIS File Conditionals
		 * 
		 * If the CSISExpression CSISString has been prefixed with the argument string identifier stored within
		 * KVersionFuncPrefix, the condition is considered to be a VERSION function call. The argument string
		 * is then passed to PackageVersion() to query the SIS Registry and determine whether the version of
		 * an installed package satisfies the presented condition. 
		 */
		if(pS.GetString().substr(0,KFuncVersionPrefix.length()) == KFuncVersionPrefix)
			{
			iTempResult = ExpressionResult(iExpEnv.PackageVersion(pS.GetString().substr(KFuncVersionPrefix.length())));
			}
		/**
		 * Support for exact and equivalent device supported languages
		 */
		else if (pS.GetString().substr(0,KFuncSupportedLanguagePrefix.length()) == KFuncSupportedLanguagePrefix)
			{
			iTempResult = ExpressionResult(iExpEnv.DeviceLanguage(pS.GetString().substr(KFuncSupportedLanguagePrefix.length())));
			}
		else
			{
			iTempResult = ExpressionResult(iExpEnv.FindFile(pS.GetString(), aLogInfo));
			}
		}
		break;

	case CSISExpression::EPrimTypeString:
		{
		const CSISString& pS = aExpression.String();
		if(pS.WasteOfSpace())
			{
			throw std::runtime_error("null pointer");
			}
		
		iTempResult = ExpressionResult(pS.GetString());
		}
		break;

	case CSISExpression::EPrimTypeVariable:
		{
        const int variableId = aExpression.IntValue();
        const int variableValue = iExpEnv.Variable( variableId, aLogInfo);
        //
		iTempResult = ExpressionResult( variableValue );
		break;
		}

	case CSISExpression::EPrimTypeOption:
		{
		iExpressionDepth=0;
		std::string error = "SIS File contains user options\n";
		std::string x = wstring2string(iExpEnv.GetPackageName());
        //
		throw InvalidSis(x, error, SIS_NOT_SUPPORTED);
		}

	case CSISExpression::EPrimTypeNumber:
		iTempResult = ExpressionResult(aExpression.IntValue());
		break;

	default:
		{
		iExpressionDepth=0;
		std::string error = "SIS File contains unknown expression\n";
		std::string x = wstring2string(iExpEnv.GetPackageName());
        //
		throw InvalidSis(x, error, SIS_NOT_SUPPORTED);
		}
    	}

	--iExpressionDepth;
	return iTempResult;
    }


// ExpressionEnvironment

ExpressionEnvironment::ExpressionEnvironment( const SisFile& aSisFile,
                                              const SisRegistry& aSisRegistry,
                                              RomManager& aRomManager,
                                              ConfigManager& aConfigManager,
                                              const std::wstring& aCDrive ) 
:   iSisFile(aSisFile), iSisRegistry(aSisRegistry), 
    iRomManager( aRomManager ), iConfigManager( aConfigManager ),
    iCDrive(aCDrive)
    {
    }


const std::wstring ExpressionEnvironment::GetPackageName()
    {
	return iSisFile.GetPackageName();
    }


bool ExpressionEnvironment::FindFile( const std::wstring& aFileName, bool aLogInfo )
    {
    bool fileExists = false;

    // Fixed up file name
    std::wstring fileName( aFileName );
   
    // If the file is in ROM then we really need to check with the ROM
    // manager. If its on C:, then we'll have to resort to using
    // the specified 'C' drive and hope that it helps.
    //
    // Filename length must be at least 3 characters, i.e. drive + semicolon + backslash + filename
    //
    if ( fileName.length() >= 1 && fileName[ 0 ] == L'\\' )
        {
        // Bad file name?
		if( aLogInfo )
			{
			LWARN(L"\tAssuming file path \'" << aFileName << L"\' refers to Z:" );
			}  		
        fileName = L"Z:" + fileName;
        }

    // Require for invalid file exception (also helps with debugging)
	    std::string narrowFileName = wstring2string( fileName );

    // Now continue with file, assuming we've fixed up the path or then
    // have enough characters to process
    bool startsWithDrive = StringUtils::StartsWithDrive( fileName );
    if  ( startsWithDrive )
        {
        const wchar_t drive = fileName[ 0 ];
        //
        switch( drive )
            {
        case L'z':
        case L'Z':
            {
            // File is in 'ROM' so use ROM manager
            fileExists = iRomManager.RomFileExists( fileName );
            break;
            }
        case L'c':
        case L'C':
        case L'!':
        default:
            {
            // File is on 'C:' so merge with C-drive specification and
            // use native FileExists() check
            ConvertToLocalPath( fileName, iCDrive );

            // For debugging
			narrowFileName = wstring2string( fileName );
            fileExists = FileExists( fileName );
            break;
            }
            }
        }
    else
        {
		std::string error = "corrupt SIS file: bad \'EXISTS' filename: \'" + narrowFileName + "\'";
		throw InvalidSis( "", error, INVALID_SIS );
        }
    //
	if(aLogInfo)
		{
		std::ostringstream stream;
		stream << "\tIF EXISTS(\'" << narrowFileName << "\') => " << fileExists;
		std::string msg = stream.str();
    std::wstring finalMessage = string2wstring( msg );
		LINFO( finalMessage );
		}
    //
    return fileExists;
    }


TUint32 ExpressionEnvironment::ApplicationProperty(TUint32 aPackageUid, TUint32 aKey)
    {
	// First of all, check in this package
	if (aPackageUid == iSisFile.GetPackageUid())
	    {
		const CSISProperties::SISPropertyArray& props = iSisFile.GetProperties()->Properties(); 
		for(int i = 0; i < props.size(); ++i)
			{
			if (props[i].Key() == aKey)
				{
				return props[i].Value();
				}
			}
	    }

	// Then check the registry
	const SisRegistryObject& aObj = iSisRegistry.GetRegistryObject(aPackageUid);
	const std::vector<SisRegistryProperty*>& props = aObj.GetProperties();
	for (std::vector<SisRegistryProperty*>::const_iterator curr1 = props.begin(); curr1 != props.end(); ++curr1)
		{
		if ((*curr1)->GetKey() == aKey)
			{
			return (*curr1)->GetValue();
			}
		}

	// No property
	return 0;
    }


int ExpressionEnvironment::Variable( int aVariableId, bool aLogInfo )
    {
    int result = 0;

    // For debugging
    std::string attributeName = ConfigManager::AttributeNameById( aVariableId );

    // See if the config manager has an over-ride for this property.
    // If so, we'll use that value in preference to any other.
    const bool attributeMappingExists = iConfigManager.ValueExists( (TUint32) aVariableId );
    if ( attributeMappingExists )
        {
        result = iConfigManager.ValueById( (TUint32) aVariableId );
        
		// verify whether the language input code is within the supported list in SIS
		// if not select first langauge in SIS file
		if (aVariableId == KVariableLanguage)
			{
			if (!iSisFile.IsSupportedLanguage((TUint32)result))
				{
				int firstLanguage = iSisFile.GetLanguage(); // get the first language
				if(aLogInfo)
					{
					std::ostringstream stream;
					stream << "Input language " << result << " is not supported by SIS file. Using first language " <<firstLanguage;
					std::string msg = stream.str();
				std::wstring finalMessage = string2wstring( msg );
					LWARN( finalMessage );	
					}
				result = firstLanguage;
				}
			}
        if(aLogInfo)
			{
			std::ostringstream stream;
			stream << "\tIF " << attributeName << " ... where [" << attributeName << " = " << result << "]";
			std::string msg = stream.str();
        std::wstring finalMessage = string2wstring( msg );
			LINFO( finalMessage );
			}
        }
	else if ( aVariableId == KVariableLanguage )
    	{
		if(aLogInfo)
			{
			LWARN(L"Disregarding language selection. Using ELangEnglish");
			}
		result = 1;
    	}
    else
        {
        std::string packageName = wstring2string( GetPackageName() );
        //
		std::string error = "SIS File contains HAL attributes\n";
		throw InvalidSis( packageName, error, SIS_NOT_SUPPORTED );
        }
    //
    return result;
    }


bool ExpressionEnvironment::Package(TUint32 aKey)
    {
	return iSisRegistry.IsInstalled(aKey);
    }


bool ExpressionEnvironment::PackageVersion(const std::wstring& aArgsString)
	{
	// Create a copy of the argument string to be used when parsing
	std::wstring parseString(aArgsString);

	// *** Parse 'Package UID' argument ***
	std::wstring packageUidStr;
	if(!CSISExpression::ExtractNextToken(parseString,packageUidStr))
		{
		return false;
		}
	
	// Package UID format checking
	packageUidStr[1] = tolower(packageUidStr[1]);
	if(packageUidStr.find(L"0x") != 0 || packageUidStr.length() != 10)
		{
		return false;
		}
	
	// Check and convert the wstring to a TUint32
	TUint32 packageUid;
	if(!CSISExpression::IsHexadecimal(packageUidStr.substr(2),packageUid))
		{
		return false;
		}


	// *** Parse 'Relational Operator' argument ***
	std::wstring relationOpStr;
	if(!CSISExpression::ExtractNextToken(parseString,relationOpStr))
		{
		return false;
		}


	// *** Parse 'Major Version Component' argument ***
	std::wstring vMajorStr;
	if(!CSISExpression::ExtractNextToken(parseString,vMajorStr))
		{
		return false;
		}

	// Check and convert the wstring to an TInt
	TInt vMajor;
	if(!CSISExpression::IsDecimal(vMajorStr, vMajor))
		{
		return false;
		}


	// *** Parse 'Minor Version Component' argument ***
	std::wstring vMinorStr;
	if(!CSISExpression::ExtractNextToken(parseString,vMinorStr))
		{
		return false;
		}

	// Check and convert the wstring to an TInt
	TInt vMinor;
	if(!CSISExpression::IsDecimal(vMinorStr, vMinor))
		{
		return false;
		}


	// *** Parse 'Build Version Component' argument ***
	std::wstring vBuildStr = parseString;

	// Check and convert the wstring to an TInt
	TInt vBuild;
	if(!CSISExpression::IsDecimal(vBuildStr, vBuild))
		{
		return false;
		}
	
	Version argsVersion(vMajor,vMinor,vBuild);

	// Check that the version component values lie within the valid range
	if(!argsVersion.IsValid())
		{
		return false;
		}

	// Then check the registry
	if(iSisRegistry.IsInstalled(packageUid))
		{
		const SisRegistryObject& pkgRegEntry = iSisRegistry.GetRegistryObject(packageUid);
		Version registryVersion = pkgRegEntry.GetVersion();

		// Compare the package version retrieved from the SIS Registry against the version
		// specified in the PKG condition
		if(relationOpStr == L"ET") 
			{// Equal To 
			return registryVersion == argsVersion;
			}
		else if(relationOpStr == L"LT")
			{// Less Than
			return registryVersion < argsVersion;
			}
		else if(relationOpStr == L"LE") 
			{// Less Than Or Equal To
			return registryVersion <= argsVersion;
			}
		else if(relationOpStr == L"GT")
			{// Greater Than
			return registryVersion > argsVersion;
			}
		else if(relationOpStr == L"GE") 
			{// Greater Than Or Equal To
			return registryVersion >= argsVersion;
			}
		else if(relationOpStr == L"NE")
			{// Not Equal
			return registryVersion != argsVersion;
			}
		else 
			{
			// If the operator is not recognised, return false
			return false;
			}
		}
	else
		{
		// Package UID has not been found within the SIS Registry
		return false;
		}
	}

bool ExpressionEnvironment::DeviceLanguage(const std::wstring& aArgsString)
	{
	TInt langID = 0;
	if(!CSISExpression::IsDecimal(aArgsString, langID))
		{
		return false;
		}
	std::vector<TInt>  devSuppLang = iConfigManager.GetDeviceSupportedLanguages();		
 	std::vector<int>::const_iterator end = devSuppLang.end();
	for (std::vector<int>::const_iterator curr = devSuppLang.begin(); curr != end; ++curr)
		{
			 if ( *curr == langID )
				 {
				 iConfigManager.AddMatchingSupportedLanguages(langID);
				 return true;
				 }
			 else
				 {
				  TLanguagePath equivalentLang;
				  Utils::GetEquivalentLanguageList((CSISLanguage::TLanguage)(*curr),equivalentLang);
				  for ( TInt i=0; i < KMaxDowngradeLanguages; i++)
					  {
					  if ( equivalentLang[i] != CSISLanguage::ELangNone )
						  {
						  if (equivalentLang[i] == langID )
							 {
							 iConfigManager.AddMatchingSupportedLanguages(langID);
							 return true;
							 }
						  }
					  else
						  {
						  break;
						  }
					  }
				 }
		}
	return false;
	}
 
