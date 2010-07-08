/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file 
 @internalComponent
 @released
*/

#include "sisexpression.h"
#include "utility.h"
#include <hal_data.h>

#define LANGUAGE_VARIABLE	L"LANGUAGE"


const SKeyword KVariables [] =
	{
		{L"MANUFACTURER",				HALData::EManufacturer},
		{L"MANUFACTURERHARDWAREREV",	HALData::EManufacturerHardwareRev},
		{L"MANUFACTURERSOFTWAREREV",	HALData::EManufacturerSoftwareRev},
		{L"MANUFACTURERSOFTWAREBUILD",	HALData::EManufacturerSoftwareBuild},
		{L"MODEL",						HALData::EModel},
		{L"MACHINEUID",					HALData::EMachineUid},
		{L"DEVICEFAMILY",				HALData::EDeviceFamily},
		{L"DEVICEFAMILYREV",			HALData::EDeviceFamilyRev},
		{L"CPU",						HALData::ECPU},
		{L"CPUARCH",					HALData::ECPUArch},
		{L"CPUABI",						HALData::ECPUABI},
		{L"CPUSPEED",					HALData::ECPUSpeed},
		//{L"SystemStartupReason",		HALData::ESystemStartupReason},
		//{L"SystemException",			HALData::ESystemException},
		{L"SYSTEMTICKPERIOD",			HALData::ESystemTickPeriod},
		{L"MEMORYRAM",					HALData::EMemoryRAM},
		{L"MEMORYRAMFREE",				HALData::EMemoryRAMFree},
		{L"MEMORYROM",					HALData::EMemoryROM},
		{L"MEMORYPAGESIZE",				HALData::EMemoryPageSize},
		//{L"PowerGood",				HALData::EPowerGood},
		//{L"PowerBatteryGood",			HALData::EPowerBatteryGood},
		{L"POWERBACKUP",				HALData::EPowerBackup},
		//{L"PowerBackupGood",			HALData::EPowerBackupGood},
		//{L"PowerExternal",				HALData::EPowerExternal},
		{L"KEYBOARD",					HALData::EKeyboard},
		{L"KEYBOARDDEVICEKEYS",			HALData::EKeyboardDeviceKeys},
		{L"KEYBOARDAPPKEYS",			HALData::EKeyboardAppKeys},
		{L"KEYBOARDCLICK",				HALData::EKeyboardClick},
		//{L"KeyboardClickState",		HALData::EKeyboardClickState},
		//{L"KeyboardClickVolume",		HALData::EKeyboardClickVolume},
		{L"KEYBOARDCLICKVOLUMEMAX",		HALData::EKeyboardClickVolumeMax},
		{L"DISPLAYXPIXELS",				HALData::EDisplayXPixels},
		{L"DISPLAYYPIXELS",				HALData::EDisplayYPixels},
		{L"DISPLAYXTWIPS",				HALData::EDisplayXTwips},
		{L"DISPLAYYTWIPS",				HALData::EDisplayYTwips},
		{L"DISPLAYCOLORS",				HALData::EDisplayColors},
		//{L"DisplayState",				HALData::EDisplayState},
		//{L"DisplayContrast",			HALData::EDisplayContrast},
		{L"DISPLAYCONTRASTMAX",			HALData::EDisplayContrastMax},
		{L"BACKLIGHT",					HALData::EBacklight},
		//{L"BacklightState",			HALData::EBacklightState},
		{L"PEN",						HALData::EPen},
		{L"PENX",						HALData::EPenX},
		{L"PENY",						HALData::EPenY},
		{L"PENDISPLAYON",				HALData::EPenDisplayOn},
		{L"PENCLICK",					HALData::EPenClick},
		//{L"PenClickState",			HALData::EPenClickState},
		//{L"PenClickVolume",			HALData::EPenClickVolume},
		{L"PENCLICKVOLUMEMAX",			HALData::EPenClickVolumeMax},
		{L"MOUSE",						HALData::EMouse},
		{L"MOUSEX",						HALData::EMouseX},
		{L"MOUSEY",						HALData::EMouseY},
		//{L"MouseState",				HALData::EMouseState},
		//{L"MouseSpeed",				HALData::EMouseSpeed},
		//{L"MouseAcceleration",		HALData::EMouseAcceleration},
		{L"MOUSEBUTTONS",				HALData::EMouseButtons},
		//{L"MouseButtonState",			HALData::EMouseButtonState},
		//{L"CaseState",					HALData::ECaseState},
		{L"CASESWITCH",					HALData::ECaseSwitch},
		//{L"CaseSwitchDisplayOn",		HALData::ECaseSwitchDisplayOn},
		//{L"CaseSwitchDisplayOff",		HALData::ECaseSwitchDisplayOff},
		{L"LEDS",						HALData::ELEDs},
		//{L"LEDmask",					HALData::ELEDmask},
		{L"INTEGRATEDPHONE",			HALData::EIntegratedPhone},
		{L"DISPLAYBRIGHTNESS",			HALData::EDisplayBrightness},
		{L"DISPLAYBRIGHTNESSMAX",		HALData::EDisplayBrightnessMax},
		{L"KEYBOARDBACKLIGHTSTATE",		HALData::EKeyboardBacklightState},
		{L"ACCESSORYPOWER",				HALData::EAccessoryPower},
		{L"SYSTEMDRIVE",				HALData::ESystemDrive},
		
		{L"FPHARDWARE",					HALData::EHardwareFloatingPoint},		
		{L"NUMHALATTRIBUTES",			HALData::ENumHalAttributes},

		{LANGUAGE_VARIABLE,				EVarLanguage},
		{L"REMOTEINSTALL",				EVarRemoteInstall},

		{NULL,							0}
	};


struct TExpressionFeatures
	{
	bool			iLeftNeeded;
	bool			iRightNeeded;
	bool			iStringNeeded;
	bool			iNumberNeeded;
	const wchar_t*	iName;
	};

static TExpressionFeatures expressionFeatures[] =
	{
		// Left	Right	String	Number	Name
		{true,	true,	false,	false,	L"="		},	 //EBinOpEqual
		{true,	true,	false,	false,	L"<>"		},	//EBinOpNotEqual
		{true,	true,	false,	false,	L">"		},	//EBinOpGreaterThan
		{true,	true,	false,	false,	L"<"		},	//EBinOpLessThan
		{true,	true,	false,	false,	L">="		},	//EBinOpGreaterThanOrEqual
		{true,	true,	false,	false,	L"<="		},	//EBinOpLessThanOrEqual
		{true,	true,	false,	false,	L"AND"		},	//ELogOpAnd
		{true,	true,	false,	false,	L"OR"		},	//ELogOpOr
		{false,	true,	false,	false,	L"NOT"		},	//EUnaryOpNot
		{false,	false,	true,	false,	L"exists"	},	//EFuncExists
		{true,	true,	false,	false,	L"appprop"	},	//EFuncAppProperties
		{false,	true,	false,	false,	L"package"	},	//EFuncDevProperties
		{false,	false,	true,	false,	L""			},	//EPrimTypeString
		{false,	false,	false,	true,	L"option"	},	//EPrimTypeOption
		{false,	false,	false,	true,	L""			},	//EPrimTypeVariable
		{false,	false,	false,	true,	L""			}	//EPrimTypeNumber
	};

TExpressionFeatures* findExpressionFeature (TUint32 aOperator)
	{
	if(aOperator <= CSISExpression::EOpNone ||aOperator >= CSISExpression::EOpUnknown)
		{
		return NULL;
		}
	return &expressionFeatures[aOperator-1];
	}



void CSISExpression::InsertMembers ()
	{
	InsertMember (iOperator);
	InsertMember (iInteger);
	InsertMember (iString);
	InsertMember (iLeaf);
	}


CSISExpression::CSISExpression (const CSISExpression& aExpression) :
		CStructure <CSISFieldRoot::ESISExpression> (aExpression),
		iOperator (aExpression.iOperator),
		iInteger (aExpression.iInteger),
		iString (aExpression.iString),
		iLeaf (aExpression.iLeaf)					
	{ 
	InsertMembers (); 
	}


void CSISExpression::Verify (const TUint32 aLanguages) const
	{
	CStructure <CSISFieldRoot::ESISExpression>::Verify (aLanguages);
	assert (iLeaf.size () < 3);
	CSISException::ThrowIf (	static_cast <TOperator> (iOperator.Value ()) > EOpUnknown,
								CSISException::EVerification,
								"illegal or unknown operator");
	}


CSISExpression& CSISExpression::operator = (const CSISExpression& aExpression)
	{
	iOperator = aExpression.iOperator;
	iInteger = aExpression.iInteger;
	iString = aExpression.iString;
	iLeaf.clear ();
	iLeaf.assign (aExpression.iLeaf.begin (), aExpression.iLeaf.end ());
	return *this;
	}


void CSISExpression::SetOperator (const TOperator aOperator, const CSISExpression& aLHS)	
	{
	switch (aOperator)
		{
	case EBinOpEqual :
	case EBinOpNotEqual :
	case EBinOpGreaterThan :
	case EBinOpLessThan :
	case EBinOpGreaterThanOrEqual :
	case EBinOpLessThanOrEqual :
	case ELogOpAnd :
	case ELogOpOr :
	case EFuncAppProperties :
		{
		AddLeaf (aLHS);
		AddLeaf ();
		break;
		}

	case EUnaryOpNot :
	case EFuncDevProperties :
		{
		AddLeaf (aLHS);
		break;
		}

	case EPrimTypeString :
	case EFuncExists :
		{
		iString.SetRequired(true);
		break;
		}

	case EPrimTypeVariable :
	case EPrimTypeOption :
	case EPrimTypeNumber :
		{
		break;
		}

	default :
		{
		assert (false);	
		}
		}
	iOperator = aOperator; 
	assert (iLeaf.size () < 3);
	}


void CSISExpression::SetLanguageComparision (const TInt32 aValue)
	{
	AddLeaf ();
	AddLeaf ();
	LHS ().SetOperator (EPrimTypeVariable);
	LHS ().SetLanguage ();
	RHS ().SetOperator (EPrimTypeNumber);
	RHS ().SetNumeric (aValue);
	iOperator =	EBinOpEqual;
	}


void CSISExpression::SetLanguage ()
	{
	iOperator =	EPrimTypeVariable;
	iInteger = EVarLanguage;
	}


void CSISExpression::SetVariable (const std::wstring& aIdentifier)
	{
	SetOperator (CSISExpression::EPrimTypeVariable);
	SetValue (static_cast <TUint32> (IdentifyUCKeyword (KVariables, aIdentifier, L"unknown variable")));
	}

void CSISExpression::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	TExpressionFeatures* features = findExpressionFeature (iOperator);
	if(NULL == features)
		{
		aStream << L"UNKNOWN";
		return;
		}
	
	if (features->iLeftNeeded)
		{
		aStream << L"(";
		if(iOperator.Value() == EFuncExists)
			{
			aStream << L"\"";
			}

		LHS().AddPackageEntry(aStream, aVerbose);
		if(iOperator.Value() == EFuncExists)
			{
			aStream << L"\"";
			}
		aStream << L")";
		}
	
	std::wstring versionStr = iString.GetString().substr(0,KFuncVersionPrefix.length());
	std::wstring supportedLanguageStr = iString.GetString().substr(0,KFuncSupportedLanguagePrefix.length());
	if((iOperator.Value() == EFuncExists) && (versionStr == KFuncVersionPrefix))
		{
		WriteVersionCondition (aStream, aVerbose);
		}
	else if ((iOperator.Value() == EFuncExists) && (supportedLanguageStr == KFuncSupportedLanguagePrefix))
		{
		WriteSupportedLanguageCondition (aStream, aVerbose);
		}
	else
		{	
		aStream << features->iName;
		}
	
	if (features->iRightNeeded)
		{
		aStream << L"(";
		if(iOperator.Value() == EFuncExists)
			{
			aStream << L"\"";
			}

		RHS().AddPackageEntry(aStream, aVerbose);
		if(iOperator.Value() == EFuncExists)
			{
			aStream << L"\"";
			}
		aStream << L")";
		}

	if (features->iStringNeeded)
		{
		if(((iOperator.Value() == EFuncExists) && (versionStr != KFuncVersionPrefix)) && ((iOperator.Value() == EFuncExists) && (supportedLanguageStr != KFuncSupportedLanguagePrefix)))
			{
			aStream << L"(";

			if(iOperator.Value() == EFuncExists)
				{
				aStream << L"\"";
				}
			
			iString.AddPackageEntry(aStream, aVerbose);
			if(iOperator.Value() == EFuncExists)
				{
				aStream << L"\"";
				}
			aStream << L")";
			}
		}
	
	if (features->iNumberNeeded)
		{
		if(iOperator.Value() == EPrimTypeVariable)
			{
			if(iInteger.Value() == EVarLanguage)
				{
				aStream << L"LANGUAGE";
				}
			else
				{
				for (int i=0; KVariables[i].iName != NULL; ++i)
					{
					if (KVariables[i].iId == iInteger)
						{
						aStream << KVariables[i].iName;
						}
					}
				}
			}
		else
			{
			aStream << iInteger;
			}
		}
	}

void CSISExpression::WriteVersionCondition(std::basic_ostream<wchar_t>& aStream, bool aVerbose) const
	{
	std::wstring parseString = iString.GetString().substr(KFuncVersionPrefix.length());
	std::wstring outputArgs;

	try
		{
		// *** Parse Package UID Argument ***
		std::wstring packageUidStr;
		if(!ExtractNextToken(parseString,packageUidStr))
			{
			throw "Failed to Parse Package UID";
			}
		
		// Package UID format checking
		packageUidStr[1] = tolower(packageUidStr[1]);
		if(packageUidStr.find(L"0x") != 0 || packageUidStr.length() != 10)
			{
			throw "Invalid Package UID Format";
			}

		// Check that the UID string can be converted to Hexadecimal
		if(!IsHexadecimal(packageUidStr.substr(2)))
			{
			throw "Package UID contains non hexadecimal characters";
			}

		// Update Condition Output 
		outputArgs.append(packageUidStr);
		outputArgs.append(L",");
		
		
		// *** Parse Relational Operator ***
		std::wstring relationOpStr;
		if(!ExtractNextToken(parseString,relationOpStr))
			{
			throw "Failed to Parse the Relational Operator";
			}

		// Update the condition argument string as necessary depending on the operator code
		if(relationOpStr == L"ET") 
			{
			outputArgs.append(L"=");
			}
		else if(relationOpStr == L"LT")
			{
			outputArgs.append(L"<");
			}
		else if(relationOpStr == L"LE") 
			{
			outputArgs.append(L"<=");
			}
		else if(relationOpStr == L"GT")
			{
			outputArgs.append(L">");
			}
		else if(relationOpStr == L"GE") 
			{
			outputArgs.append(L">=");
			}
		else if(relationOpStr == L"NE")
			{
			outputArgs.append(L"<>");
			}
		else 
			{
			// If the operator is not recognised, throw an exception
			throw "Invalid Relational Operator";
			}

		// Update Condition Output
		outputArgs.append(L",");


		// *** Parse Major Version Component ***
		std::wstring vMajorStr;
		if(!ExtractNextToken(parseString,vMajorStr))
			{
			throw "Failed to parse the MAJOR version component";
			}

		// Check and convert the wstring to an integer
		TInt vMajor;
		if(!IsDecimal(vMajorStr,vMajor))
			{
			throw "MAJOR version component contains non numeric characters (0-9)";
			}

		// Check the Major component value lies within range
		if(vMajor < 0 || vMajor > 127)
			{
			throw "MAJOR version component out of range (0 - 127)";
			}

		// Update Condition Output
		outputArgs.append(vMajorStr);
		outputArgs.append(L",");


		// *** Parse Minor Version Component ***
		std::wstring vMinorStr;
		if(!ExtractNextToken(parseString,vMinorStr))
			{
			throw "Failed to parse the MINOR version component";
			}

		// Check and convert the wstring to an integer
		TInt vMinor;
		if(!IsDecimal(vMinorStr,vMinor))
			{
			throw "MINOR version component contains non numeric characters (0-9)";
			}
		
		// Check the Minor component value lies within range
		if(vMinor < 0 || vMinor > 99)
			{
			throw "MINOR version component out of range (0 - 99)";
			}

		// Update Condition Output
		outputArgs.append(vMinorStr);
		outputArgs.append(L",");


		// *** Parse Build Version Component ***
		std::wstring vBuildStr(parseString); 

		// Check and convert the wstring to an integer
		TInt vBuild;
		if(!IsDecimal(vBuildStr,vBuild))
			{
			throw "BUILD version component contains non numeric characters (0-9)";
			}

		// Check the Build component value lies within range
		if(vBuild < 0 || vBuild > 32767)
			{
			throw "BUILD version component out of range (0 - 32767)";
			}

		// Update Condition Output
		outputArgs.append(vBuildStr);

		// Output the successfully parsed version condition to the stream
		aStream << L"version(";
		aStream << outputArgs;
		aStream << L")";
		}
	catch(const char* aWarnMsg) 
		{
		// Convert Char* into a wstring 
		TInt bufferLength = strlen(aWarnMsg);
		wchar_t* buffer = new wchar_t[bufferLength+1];

		mbstowcs(buffer,aWarnMsg,bufferLength+1);
		std::wstring msgString(buffer);
		delete buffer;

		// Output the condition as an exists statement and comment warnings to the stream
		aStream << L"exists(\"";
		iString.AddPackageEntry(aStream, aVerbose);
		aStream << L"\")" << std::endl;
		aStream << L"; warning: \"VERSION\" condition output as \"EXISTS\"" << std::endl;
		aStream << L"; " << msgString;
		}
	}

bool CSISExpression::ExtractNextToken(std::wstring& aParseString, std::wstring& aTokenString)
	{
	TInt separatorPosition = aParseString.find(L",");

	// Check that a separator was located within the parse string
	if(separatorPosition == -1 || separatorPosition > aParseString.length()-1)
		{
		return false;
		}

	// Set the extracted token string and remove the token from the parse string
	aTokenString = aParseString.substr(0,separatorPosition);
	aParseString = aParseString.substr(separatorPosition + 1);

	return true;
	}

bool CSISExpression::IsHexadecimal(const std::wstring& aString)
	{
	TUint32 i;
	return IsHexadecimal(aString,i);
	}

bool CSISExpression::IsHexadecimal(const std::wstring& aString, TUint32& aHexValue)
	{
	if(aString.size() == 0)
		{
		return false;
		}

	std::wistringstream wStrStream(aString);

	if((wStrStream >> std::hex >> aHexValue) && wStrStream.eof())
		{
		return true;
		}

	return false;
	} 

bool CSISExpression::IsDecimal(const std::wstring& aString, TInt& aDecimalValue)
	{
	if(aString.size() == 0)
		{
		return false;
		}

	std::wistringstream wStrStream(aString.c_str());

	if((wStrStream >> std::dec >> aDecimalValue) && wStrStream.eof())
		{
		return true;
		}

	return false;
	}

void CSISExpression::WriteSupportedLanguageCondition(std::basic_ostream<wchar_t>& aStream, bool aVerbose) const
	{
	std::wstring parseString = iString.GetString().substr(KFuncSupportedLanguagePrefix.length());
	std::wstring outputArgs;
	try
		{
		// Check and convert the wstring to an integer
		TInt vLanguageId;
		if(!IsDecimal(parseString,vLanguageId))
			{
			throw "Supported_Language option contains non-numeric value";
			}
		// Output the successfully parsed version condition to the stream
		aStream << L"Supported_Language = ";
		aStream << vLanguageId;

		}
	catch(const char* aWarnMsg) 
		{
		// Convert Char* into a wstring 
		TInt bufferLength = strlen(aWarnMsg);
		wchar_t* buffer = new wchar_t[bufferLength+1];

		mbstowcs(buffer,aWarnMsg,bufferLength+1);
		std::wstring msgString(buffer);
		delete buffer;

		// Output the condition as an exists statement and comment warnings to the stream
		aStream << L"exists(\"";
		iString.AddPackageEntry(aStream, aVerbose);
		aStream << L"\")" << std::endl;
		aStream << L"; warning: \"Supported_Language\" condition output as \"EXISTS\"" << std::endl;
		aStream << L"; " << msgString;
		}
	}

