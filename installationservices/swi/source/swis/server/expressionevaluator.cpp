/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Definition of the CExpressionEvaluator
*
*/


/**
 @file 
*/

#include <bautils.h>
#include <hal.h> 


#include "expressionevaluator.h"
#include "sisexpression.h"
#include "sisstring.h"
#include "plan.h"
#include "application.h"
#include "userselections.h"
#include "siscontroller.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "sisproperties.h"
#include "sisproperty.h"
#include "sisregistrywritablesession.h"
#include "sisregistrywritableentry.h"
#include "cleanuputils.h"
#include "sisregistrypackage.h"
#include "sisregistrysession.h"
#include "sisregistryentry.h"
#include "checkedversion.h"
#include "sishelperclient.h"

using namespace Swi;
using namespace Swi::Sis;

// SIS Supported Language Function SISString Prefix
_LIT(KSupportedLangFuncPrefix, "\\sys\\install\\supportedlanguage\\?");

// PKG VERSION Function SISString Prefix
_LIT(KVersionFuncPrefix, "\\sys\\install\\pkgversion\\?");

// PKG VERSION Relational Operator Literal Strings
_LIT(KVersionEqualTo, "ET");
_LIT(KVersionLessThan, "LT");
_LIT(KVersionLessThanOrEqualTo, "LE");
_LIT(KVersionGreaterThan, "GT");
_LIT(KVersionGreaterThanOrEqualTo, "GE");
_LIT(KVersionNotEqual, "NE");

// TExpressionResult

TExpressionResult::TExpressionResult(const TDesC& aString)
	: iType(EString), iString(&aString)
	{
	}

TExpressionResult::TExpressionResult(TInt aInt)
	: iType(EInt), iInt(aInt)
	{
	}

TInt TExpressionResult::IntegerValueL() const
	{
	if (iType!=EInt)
		{
		User::Leave(KErrInvalidType);
		}
	return iInt;
	}
	
TBool TExpressionResult::BoolValueL() const
	{
	return TBool(IntegerValueL());		
	}
	
const TDesC& TExpressionResult::StringValueL() const
	{
	if (iType!=EString)
		{
		User::Leave(KErrInvalidType);
		}
	return *iString;
	}


// CExpression Evaluator

CExpressionEvaluator* CExpressionEvaluator::NewL(MExpressionEnvironment& aEnvironment)
	{
	CExpressionEvaluator* self = new (ELeave) CExpressionEvaluator(aEnvironment);
	return self;
	}

CExpressionEvaluator* CExpressionEvaluator::NewLC(MExpressionEnvironment& aEnvironment)
	{
	CExpressionEvaluator* self = new (ELeave) CExpressionEvaluator(aEnvironment);
	CleanupStack::PushL(self);
	return self;
	}

CExpressionEvaluator::CExpressionEvaluator(MExpressionEnvironment& aEnvironment)
	: iEnvironment(aEnvironment), iTemporaryResult(EFalse)
	{
	}

void CExpressionEvaluator::RequireL(const void *aPointer) const
	{
	if (!aPointer)
		{
		User::Leave(KErrInvalidExpression);
		}
	}

TExpressionResult CExpressionEvaluator::EvaluateL(const Sis::CExpression* aExpression)
	{
	RequireL(aExpression);
	return EvaluateL(*aExpression);
	}

TExpressionResult CExpressionEvaluator::EvaluateL(const Sis::CExpression& aExpression)
	{	
	if (++iExpressionDepth > KMaxExpressionDepth)
		{
		iExpressionDepth=0;
		User::Leave(KErrExpressionToComplex);
		}

	switch (aExpression.Operator())
		{
		case EBinOpEqual:
			iTemporaryResult=EvaluateL(aExpression.Lhs()) == EvaluateL(aExpression.Rhs());	
			break;

		case EBinOpNotEqual:
			iTemporaryResult=EvaluateL(aExpression.Lhs()) != EvaluateL(aExpression.Rhs());	
 			break;

		case EBinOpGreaterThan:
			iTemporaryResult=EvaluateL(aExpression.Lhs()) > EvaluateL(aExpression.Rhs());	
 			break;

		case EBinOpLessThan:
			iTemporaryResult=EvaluateL(aExpression.Lhs()) < EvaluateL(aExpression.Rhs());	
 			break;

		case EBinOpGreaterOrEqual:
			iTemporaryResult=EvaluateL(aExpression.Lhs()) >= EvaluateL(aExpression.Rhs());	
 			break;
	
		case EBinOpLessOrEqual:
			iTemporaryResult=EvaluateL(aExpression.Lhs()) <= EvaluateL(aExpression.Rhs());	
 			break;
	
		case ELogOpAnd:
			{
			// The temporary are explicit to please CCover which chokes on the more
			// compact expression:
			// iTemporaryResult=EvaluateL(aExpression.Lhs()) && EvaluateL(aExpression.Rhs());
			TExpressionResult tmp1 = EvaluateL(aExpression.Lhs());
			TExpressionResult tmp2 = EvaluateL(aExpression.Rhs());
			iTemporaryResult = TExpressionResult(tmp1.BoolValueL() && tmp2.BoolValueL());	
			break;
			}
	
		case ELogOpOr:
		// Special case, need to evaluate Rhs only if !LHs, which can't be done
		// with operator||
			{
			// This code is verbose to please CCover which chokes on the more
			// compact expression:
			// iTemporaryResult=EvaluateL(aExpression.Lhs()).BoolValueL() ? TExpressionResult(ETrue) : 
			//										   EvaluateL(aExpression.Rhs()).BoolValueL();
			TExpressionResult tmp1 = EvaluateL(aExpression.Lhs());
			if (tmp1.BoolValueL())
				{
				iTemporaryResult = TExpressionResult(ETrue);
				}
			else
				{
				iTemporaryResult = TExpressionResult(EvaluateL(aExpression.Rhs())).BoolValueL();
				}
 			break;
			}

		case EFuncAppProperties:
			iTemporaryResult=iEnvironment.ApplicationPropertyL(EvaluateL(aExpression.Lhs()).IntegerValueL()
						,EvaluateL(aExpression.Rhs()).IntegerValueL());	
 			break;

		case EUnaryOpNot:
			iTemporaryResult=!EvaluateL(aExpression.Lhs());
 			break;
				
		case EFuncDevProperties:
			iTemporaryResult=TExpressionResult(iEnvironment.PackageL(EvaluateL(aExpression.Lhs()).IntegerValueL()));
 			break;
			
		case EFuncExists:
			RequireL(aExpression.StringValue());
			
			/**
			 * CR1125 - Add Package Versions to SIS File Conditionals
			 * 
			 * If the SISExpression SISString has been prefixed with the argument string identifier stored within
			 * KVersionFuncPrefix, the condition is considered to be a VERSION function call. The argument string
			 * is then passed to PackageVersionL() to query the SIS Registry and determine whether the version of
			 * an installed package satisfies the presented condition. 
			 */
			if(aExpression.StringValue()->Data().Left(KVersionFuncPrefix().Length()) == KVersionFuncPrefix)
				{
				iTemporaryResult=TExpressionResult(iEnvironment.PackageVersionL(aExpression.StringValue()->Data().Mid(KVersionFuncPrefix().Length())));	
				}
			else 
				{	
				if(aExpression.StringValue()->Data().Left(KSupportedLangFuncPrefix().Length()) == KSupportedLangFuncPrefix)
				    {
					iTemporaryResult=TExpressionResult(iEnvironment.DeviceLanguageL(aExpression.StringValue()->Data().Mid(KSupportedLangFuncPrefix().Length())));	
				    }
				 else
					{
					iTemporaryResult=TExpressionResult(iEnvironment.FileExistsL(aExpression.StringValue()->Data()));
					}
				}
 			break;

		case EPrimTypeString:
			RequireL(aExpression.StringValue());
			iTemporaryResult=TExpressionResult(aExpression.StringValue()->Data());
 			break;

		case EPrimTypeVariable:
			iTemporaryResult=TExpressionResult(iEnvironment.VariableL(aExpression.IntegerValue()));
 			break;

		case EPrimTypeOption:
			iTemporaryResult=TExpressionResult(iEnvironment.OptionL(aExpression.IntegerValue()));
 			break;

		case EPrimTypeNumber:
			iTemporaryResult=TExpressionResult(aExpression.IntegerValue());
 			break;
		
		default:
			iExpressionDepth=0;
			User::Leave(KErrSISExpressionUnknownOperator);
		}

	--iExpressionDepth;
	return iTemporaryResult;
	}

// CSwisExpressionEnvironment

/*static*/ CSwisExpressionEnvironment* CSwisExpressionEnvironment::NewL(const CApplication& aApplication)
	{
	CSwisExpressionEnvironment* self=NewLC(aApplication);
	CleanupStack::Pop(self);
	return self;
	}

/*static*/ CSwisExpressionEnvironment* CSwisExpressionEnvironment::NewLC(const CApplication& aApplication)
	{
	CSwisExpressionEnvironment* self=new(ELeave) CSwisExpressionEnvironment(aApplication);
	CleanupStack::PushL(self);
	return self;
	}
	
CSwisExpressionEnvironment::CSwisExpressionEnvironment(const CApplication& aApplication)
	: iApplication(aApplication)
	{
	}
	
TBool CSwisExpressionEnvironment::FileExistsL(const TDesC& aFileName)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	// Try getting file info to see if it exists
	TEntry entry;
	TInt err = fs.Entry(aFileName, entry);  
	fs.Close();
	return err==KErrNone ? ETrue : EFalse;
	}

TBool CSwisExpressionEnvironment::DeviceLanguageL(const TDesC& aLanguageIdString)
	{
	TPtrC parseString(aLanguageIdString);
	TBool deviceSupportedFlag;
	TInt langId = ParseLanguageId(parseString);
	if(langId == KErrNotFound)
		{
		return EFalse;
		}	
	
	//Set the iHasDeviceSupportedLanguages flag
	(const_cast <CApplication&> (iApplication)).SetDeviceSupportedLanguages(ETrue);
	
	deviceSupportedFlag = iApplication.IsDeviceMatchingLanguage(langId);
	
	if ( !deviceSupportedFlag )
		{
		RSisHelper iSisHelper = (const_cast <CApplication&> (iApplication)).GetSisHelper();
		RArray<TInt> deviceSupportedLanguages = (const_cast <CApplication&> (iApplication)).GetDeviceSupportedLanguages();
		TInt size = deviceSupportedLanguages.Count();
		RArray<TLanguage> equivalentLangArray;
		CleanupClosePushL(equivalentLangArray);
		
		for ( TInt i=0; i < size; i++)
			{
			iSisHelper.GetEquivalentLanguageListL((TLanguage)deviceSupportedLanguages[i],equivalentLangArray);
			TInt equivalentArraySize = equivalentLangArray.Count();
			for ( TInt j=0; j < equivalentArraySize; j++ )
				{
				if (equivalentLangArray[j] == langId )
					{
					(const_cast <CApplication&> (iApplication)).SetLanguageMatch(ETrue);
					(const_cast <CApplication&> (iApplication)).PopulateMatchingDeviceLanguagesL(langId);
					equivalentLangArray.Close();
					CleanupStack::Pop(&equivalentLangArray);
					return ETrue;
					}
				}
			equivalentLangArray.Close();	
			}
		CleanupStack::Pop(&equivalentLangArray);
		}
	// Set the iHasMatchingLanguages flag if there is an exact match between the 
	// languages which device supports and SUPPORTED_LANGUAGE token in PKG file.	
	if(deviceSupportedFlag)
		{
		(const_cast <CApplication&> (iApplication)).SetLanguageMatch(ETrue);
		(const_cast <CApplication&> (iApplication)).PopulateMatchingDeviceLanguagesL(langId);
		}
	return deviceSupportedFlag;
	}

TInt CSwisExpressionEnvironment::ParseLanguageId(const TDesC& aComponentString)
	{
	/** 
	 *  Convert the string into a TInt representation and check that
	 *  the string is a valid decimal value
	 */
	TLex componentLex = aComponentString;
	TInt componentValue = 0;
	
	if(componentLex.Val(componentValue) == KErrNone && componentLex.Eos())
		{
		return componentValue;
		}
	
	// Return an error if the TInt value is not parsed correctly
	return KErrNotFound;
	}

TInt CSwisExpressionEnvironment::ApplicationPropertyL(TInt aPackageUid, 
	TInt aKey)
	{
	//first of all check for this package
	if (aPackageUid==iApplication.ControllerL().Info().Uid().Uid().iUid)
		{
		const CProperties& properties=iApplication.ControllerL().Properties();
		for (TInt i=0; i < properties.Count(); ++i)
			{
			if (properties[i].Key() == aKey)
				{
				return properties[i].Value();
				}
			}
		}

	/// Check the registry as well
	RSisRegistrySession registrySession;
	User::LeaveIfError(registrySession.Connect());
	CleanupClosePushL(registrySession);
	
	TUid uid={aPackageUid};
	RSisRegistryEntry registryEntry;
	TInt error=registryEntry.Open(registrySession, uid);
	
	// There may be no package with this uid installed,
	// so not KErrNotFound should not cause us to stop processing
	if (error == KErrNotFound)
		{
		CleanupStack::PopAndDestroy(&registrySession);
		return 0;
		}
	CleanupClosePushL(registryEntry);
	
	TInt value=registryEntry.PropertyL(aKey);
	
	CleanupStack::PopAndDestroy(2, &registrySession);
	return value;
	}

TInt CSwisExpressionEnvironment::PackageL(TInt aKey)
	{
	TUid puid = {aKey};
	RSisRegistryWritableSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
	TBool res = session.IsInstalledL(puid);
	
	if (res)
        {
        // If the package in the package(PUID) predicate is installed
        // AND it was  installed by a previous version of the package
        // that we are installing then pretend the package isn't present.
        //
        // In an SA upgrade containing an embedded package gated by if not package(puid)
        // the embedded package will be re-installed instead of removed.
        // For example, the following pkg statement will work even if multiple packages
        // embed the same file so long as RemoveOnlyWithLastDependent is set in swipolicy.ini
        // (0x01001235), *, *, *, {"embedded.sis"}
        // if not package(0x01001235)
        // @"embedded.sis",(0x01001235)
        // endif                
        RSisRegistryWritableEntry registryEntry;
        TUid uid = iApplication.ControllerL().Info().Uid().Uid();
        if (registryEntry.Open(session, uid) == KErrNone)
            {                    
            CleanupClosePushL(registryEntry);
            
            RPointerArray<CSisRegistryPackage> embeddedPackages;
            CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(embeddedPackages);
            registryEntry.EmbeddedPackagesL(embeddedPackages);
            TInt numEmbedded = embeddedPackages.Count();
            for (TInt i = 0; i < numEmbedded; ++i)
                {            
                if (puid == embeddedPackages[i]->Uid())
                    {
                    res = EFalse;
                    break;
                    }
                }
            CleanupStack::PopAndDestroy(2, &registryEntry);
            }
        }
	
	CleanupStack::PopAndDestroy(&session);

	return res;
	}

TBool CSwisExpressionEnvironment::OptionL(TInt aOptionNumber)
	{
	// Option numbers in the SISX file are 1 based.
	if (aOptionNumber < 1 ||  aOptionNumber > iApplication.UserSelections().Options().Count())
		{
		User::Leave(KErrInvalidExpression);
		}

	return  iApplication.UserSelections().Options()[aOptionNumber-1];
	}
	
TInt CSwisExpressionEnvironment::VariableL(TInt aVariableNumber)
	{
	if (aVariableNumber >=0 && aVariableNumber < 0x1000) // range defined to be Hal values
		{
		TInt value=0;
		HAL::Get(HAL::TAttribute(aVariableNumber), value); // ignore error
		return value;
		}
	else if (aVariableNumber == KVariableLanguage)
		{
		return iApplication.UserSelections().Language();
		}
	else if (aVariableNumber == KVariableRemoteInstall)
		{
		return 0; // deprecated
		}
		
	return 0;	
	}

TBool CSwisExpressionEnvironment::PackageVersionL(const TDesC& aArgsString)
	{
	// String Parse Variables
	TPtrC parseString(aArgsString);

	// Parse Package UID (TUid)
	TPtrC pUidStr;
	if(ExtractNextToken(pUidStr,parseString) == KErrNotFound)
		{
		return EFalse;
		}
	
	TUid packageUid = TUid::Null();
	if(ParsePackageUid(pUidStr, packageUid) == KErrNotFound)
		{
		return EFalse;
		}	
	
	// Parse Relation Operator
	TPtrC relationOp;
	if(ExtractNextToken(relationOp,parseString) == KErrNotFound)
		{
		return EFalse;
		}

	// Parse Version Major Value
	TPtrC vMajorStr;
	if(ExtractNextToken(vMajorStr,parseString) == KErrNotFound)
		{
		return EFalse;
		}
	
	TInt vMajor = ParseVersionComponent(vMajorStr);
	if(vMajor == KErrNotFound)
		{
		return EFalse;
		}	
	
	// Parse Version Minor Value
	TPtrC vMinorStr;
	if(ExtractNextToken(vMinorStr,parseString) == KErrNotFound)
		{
		return EFalse;
		}
	
	TInt vMinor = ParseVersionComponent(vMinorStr);
	if(vMinor == KErrNotFound)
		{
		return EFalse;
		}	
	
	// Parse Version Build Value
	TInt vBuild = ParseVersionComponent(parseString);
	if(vBuild == KErrNotFound)
		{
		return EFalse;
		}
	
	// Construct argument version object and check the component ranges
	TCheckedVersion argsVersion(TVersion(vMajor,vMinor,vBuild));
	
	if(!argsVersion.IsValid())
		{
		return EFalse;
		}
	
	// Create a new session to the SIS Registry 
	RSisRegistrySession registrySession;
	User::LeaveIfError(registrySession.Connect());
	CleanupClosePushL(registrySession);
	
	//Retrieve the package entry from the Registry using the package UID argument
	RSisRegistryEntry registryEntry;
	TInt error=registryEntry.Open(registrySession, packageUid);
		
	/**
	 *  If the registry reports that there is no package installed with 
	 *  the specified UID value, clean up and return EFalse
	 */
	if (error == KErrNotFound)
		{
		CleanupStack::PopAndDestroy(&registrySession);
		return EFalse;
		}
	CleanupClosePushL(registryEntry);
	
	TCheckedVersion registryVersion(registryEntry.VersionL());
		
	CleanupStack::PopAndDestroy(2, &registrySession);	// registryEntry, registrySession 
	
	if(relationOp == KVersionEqualTo)
		{// Equal To
		return registryVersion == argsVersion;
		}
	else if(relationOp == KVersionLessThan)
		{// Less Than
		return registryVersion < argsVersion;
		}
	else if(relationOp == KVersionLessThanOrEqualTo)
		{// Less Than Or Equal To
		return registryVersion <= argsVersion;
		}
	else if(relationOp == KVersionGreaterThan)
		{// Greater Than
		return registryVersion > argsVersion;
		}
	else if(relationOp == KVersionGreaterThanOrEqualTo)
		{// Greater Than Or Equal To
		return registryVersion >= argsVersion;
		}
	else if(relationOp == KVersionNotEqual)
		{// Not Equal
		return registryVersion != argsVersion;
		}
	else
		{// Relational Operator Not Recognised
		return EFalse;
		}
	}

TInt CSwisExpressionEnvironment::ExtractNextToken(TPtrC& aTokenString, TPtrC& aParseString)
	{
	TInt separatorPosition = aParseString.LocateF(',');
	
	// Check that a separator was located within the parse string
	if(separatorPosition == KErrNotFound || separatorPosition > aParseString.Length()-1)
		{
		return KErrNotFound;
		}
	
	// Set the extracted token string and remove the token from the parse string
	aTokenString.Set(aParseString.Left(separatorPosition));
	aParseString.Set(aParseString.Mid(separatorPosition+1));
	
	return KErrNone;
	}

TInt CSwisExpressionEnvironment::ParsePackageUid(const TDesC& aUidString, TUid& aUid)
	{	
	// Check that the UID string matches the format
	_LIT(KVersionUidFormat,"0x????????");
	if(aUidString.MatchF(KVersionUidFormat) != 0)
		{
		return KErrNotFound;
		}
	 	
	/** 
	 *  Convert the string into a TUint32 representation and check that
	 *  the string is a valid hexadecimal value
	 */
	TLex lexUid(aUidString.Right(8));
	TUint32 uidValue = 0;
	
	if(lexUid.Val(uidValue,EHex) == KErrNone && lexUid.Eos())
		{
		aUid.iUid = uidValue;
		return KErrNone; 
		}
	
	// Return an error if the UID value is not parsed correctly
	return KErrNotFound;
	}

TInt CSwisExpressionEnvironment::ParseVersionComponent(const TDesC& aComponentString)
	{
	/** 
	 *  Convert the string into a TInt representation and check that
	 *  the string is a valid decimal value
	 */
	TLex componentLex = aComponentString;
	TInt componentValue = 0;
	
	if(componentLex.Val(componentValue) == KErrNone && componentLex.Eos())
		{
		return componentValue;
		}
	
	// Return an error if the TInt value is not parsed correctly
	return KErrNotFound;
	}

// CSwisCreateExpressionEnvironment
/*static*/ CSwisCreateExpressionEnvironment* CSwisCreateExpressionEnvironment::NewL(const CApplication& aApplication)
	{
	CSwisCreateExpressionEnvironment* self=NewLC(aApplication);
	CleanupStack::Pop(self);
	return self;
	}

/*static*/ CSwisCreateExpressionEnvironment* CSwisCreateExpressionEnvironment::NewLC(const CApplication& aApplication)
	{
	CSwisCreateExpressionEnvironment* self=new(ELeave) CSwisCreateExpressionEnvironment(aApplication);
	CleanupStack::PushL(self);
	return self;
	}
	
CSwisCreateExpressionEnvironment::CSwisCreateExpressionEnvironment(const CApplication& aApplication)
	: CSwisExpressionEnvironment(aApplication),
	  iApplication(aApplication)
	{
	}

TBool CSwisCreateExpressionEnvironment::OptionL(TInt /* aOptionNumber */)
	{
	// Return ETrue, regardless of the aOptionNumber.
	return  1;
	}
