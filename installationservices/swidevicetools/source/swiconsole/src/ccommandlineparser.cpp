/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CCommandLineParser: This class provides functionality to parse the command
* line arguments and to create the preference class.
* @internalComponent
*
*/

#include <bacline.h>		// CCommandLineArguments
#include <e32cons.h>		// CConsoleBase

// User includes
#include "ccommandlineparser.h"
#include "ciohandler.h"
#include "swiconsoleconstants.h"
#include "swiutility.h"
#include "swiconsoleerrors.h"


const TInt KHexPrefixLength = 2;
const TInt8 KOptionSymbol = '/';

// Extern variable defined in main.cpp
extern CIoHandler* gIoHandler;

// Commandline Options or Arguments
//Basic Options
_LIT(KOptionInstall, 		"/i");
_LIT(KOptionUnInstall, 		"/u");
_LIT(KOptionList, 			"/d"); // Display Package Information
_LIT(KOptionHelp1,			"/h");
_LIT(KOptionHelp2,			"/?");
_LIT(KOptionVersion1, 		"/ver");
_LIT(KOptionVersion2, 		"/version");
_LIT(KOptionUid, 			"/uid");
_LIT(KOptionPakageName, 	"/pkg");
_LIT(KOptionVendorName,		"/vendor");
// Operation Mode
_LIT(KOptionNormalMode, 	"/normal");
_LIT(KOptionUnAttendedMode,	"/unattended");
_LIT(KOptionOptimalMode, 	"/optimal");
// Application Options
_LIT(KOptionLog, 			"/l");
_LIT(KOptionLogFile,		"/lf");
_LIT(KOptionVerbose, 		"/v");
// Events
_LIT(KOptionProgressInfo,	"/ep");		
_LIT(KOptionDevCert,		"/edevcert");
// Cancellable Event
_LIT(KOptionRemoveFiles,	"/cer");		
_LIT(KOptionClosingApp,		"/ces");
_LIT(KOptionCopyingFiles,	"/cec");
_LIT(KOptionOCSPCheckStart,	"/ceo");
// Installation Event
_LIT(KEventProgressFinal, 	"/iepf");
_LIT(KEventProgressUpdate, 	"/iepu");
_LIT(KEventOcspCheckEnd, 	"/ieoe");
_LIT(KEventCompletedNoMsg, 	"/iecnm");
_LIT(KEventDevCertUsed, 	"/iedev");
// Dialog Responses
_LIT(KDisplayInstall,		"/di");
_LIT(KGrantCapabilities,	"/dc");
_LIT(KDisplayLanguage,		"/dl");
_LIT(KDisplayDrive,			"/dd");
_LIT(KDisplayUpgrade,		"/dg");
_LIT(KDisplayOption,		"/do");
_LIT(KDisplayWarning,		"/dw");
_LIT(KDisplayOCSPError,		"/dOCSP");
_LIT(KMissingDependency,	"/ddepend");
_LIT(KDisplayText,			"/dt");
_LIT(KQuestionIncompatible,	"/dqi");
_LIT(KQuestionOverwrite,	"/dqo");
_LIT(KDisplayUnInstall,		"/du");
_LIT(KDisplayDependencyBreak,"/ddb");
_LIT(KDisplayAppInUse,		"/da");

// OCSP revocation check
_LIT(KOcspRevocationServer,	"/ocspserver");
_LIT(KOcspEnableCheck,		"/ocspenable");

// Embedded sis files
_LIT(KEmbeddedSis,			"/emb");
_LIT(KEmbeddedPackage, 		"/embPackage");
_LIT(KEmbeddedVendor, 		"/embVendor");

// Answers to prompt
_LIT(KAnsEnable, 			"enable");
_LIT(KAnsDisable, 			"disable");
_LIT(KAnsHide, 				"hide");
_LIT(KAnsShow, 				"show");
_LIT(KAnsPrompt, 			"prompt");
_LIT(KAnsYes,				"yes");
_LIT(KAnsNo,				"no");


//extern CConsoleBase* gConsole;

CCommandLineParser* CCommandLineParser::NewL()
	{
	CCommandLineParser* self = CCommandLineParser::NewLC();
	CleanupStack::Pop(self);
	
	return self;
	}

CCommandLineParser* CCommandLineParser::NewLC()
	{
	CCommandLineParser* self = new (ELeave) CCommandLineParser();
	CleanupStack::PushL(self);
	
	self->ConstructL();
	
	return self;
	}

void CCommandLineParser::ConstructL()
	{
	iCommandLineArguments = CCommandLineArguments::NewL();
	}

CCommandLineParser::~CCommandLineParser()
	{
	delete iCommandLineArguments;
	}

// This function Parses the command line argument and checks the
// operation type based on the first parameter supplied
// Return: Operation Type
// Leave:	KSwiInvalidConsoleArg - In case of wrong arguement supplied
//			KMissingOptionsValue - If value of the option not provided
//			KInvalidPackageUid - If invalid package uid provided
//			KInvalidDriveName - If invalid drive name provided
TOperation CCommandLineParser::ParseArgumentsL()
	{
	if(iCommandLineArguments->Count() <= 1)
		{
		return ESwiUsage;
		}

	TPtrC argumentPtr(iCommandLineArguments->Arg(1));
	
	if(0 == argumentPtr.CompareF(KOptionInstall))
		{
		return ESwiInstall;
		}

	if(0 == argumentPtr.CompareF(KOptionUnInstall))
		{
		return ESwiUnInstall;
		}
	
	if(0 == argumentPtr.CompareF(KOptionList))
		{
		return ESwiList;
		}
	
	if( 0 == argumentPtr.CompareF(KOptionVersion1) || 
		0 == argumentPtr.CompareF(KOptionVersion2))
		{
		return ESwiVersion;
		}

	if( 0 == argumentPtr.CompareF(KOptionHelp1) || 
		0 == argumentPtr.CompareF(KOptionHelp2) )
		{
		return ESwiHelp;
		}
	
	User::Leave(KSwiInvalidConsoleArg);
	
	return ESwiHelp;
	}

CPreferences* CCommandLineParser::GetPreferencesLC()
	{
	CPreferences* preferences = CPreferences::NewLC();
	
	return preferences;
	}

CPreferenceHandler* CCommandLineParser::GetPreferenceHandlerLC()
	{
	CPreferenceHandler* preferenceHandler = CPreferenceHandler::NewLC();
	
	FillPreferencesL(*preferenceHandler);
	
	gIoHandler->SetPreferenceHandler(preferenceHandler);
	
	LogCommandLineArgumentsL(*preferenceHandler);
	
	return preferenceHandler;
	}

void CCommandLineParser::FillPreferencesL(CPreferenceHandler& aPreferenceHandler)
	{
	TInt argumentCount = iCommandLineArguments->Count();
	
	CPreferences* curPreference = aPreferenceHandler.GetDefaultPreference();

	for (TInt i = 1; i < argumentCount; ++i)
		{
		TInt dialogType = 0;
		TPtrC argumentPtr(iCommandLineArguments->Arg(i));
	
		if(0 == argumentPtr.CompareF(KOptionInstall))
			{
			aPreferenceHandler.iOperation = ESwiInstall;
			
			if(++i >= argumentCount)
				{
				User::Leave(KMissingOptionsValue);
				}
			argumentPtr.Set(iCommandLineArguments->Arg(i));
			if(KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KMissingOptionsValue);
				}
			aPreferenceHandler.iSisFileName.Copy(argumentPtr);
			}
		else if(0 == argumentPtr.CompareF(KOptionUnInstall))
			{
			aPreferenceHandler.iOperation = ESwiUnInstall;
			}
		else if(0 == argumentPtr.CompareF(KOptionList))
			{
			aPreferenceHandler.iOperation = ESwiList;
			}
		else if(0 == argumentPtr.CompareF(KOptionUid))
			{
			if(++i >= argumentCount)
				{
				User::Leave(KMissingOptionsValue);
				}
			argumentPtr.Set(iCommandLineArguments->Arg(i));
			if(KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KMissingOptionsValue);
				}
			
			if(argumentPtr.Length() > KIntStringLen)
				{
				User::Leave(KInvalidPackageUid);
				}
				
			TBuf<KIntStringLen> uidString(argumentPtr);
			
			if(argumentPtr[0] == '0' && (argumentPtr[1] == 'X' || argumentPtr[1] == 'x'))
				{
				uidString.Delete(0, KHexPrefixLength);
				}
			
			TLex lexObj(uidString);
			// String to integer conversion
			TUint pkgUid = 0;
			lexObj.Val(pkgUid, EHex);
			
			aPreferenceHandler.iPackageUID.iUid = pkgUid;
			}
		else if(0 == argumentPtr.CompareF(KOptionPakageName))
			{
			if(++i >= argumentCount)
				{
				User::Leave(KMissingOptionsValue);
				}
			argumentPtr.Set(iCommandLineArguments->Arg(i));
			if(KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KMissingOptionsValue);
				}

			aPreferenceHandler.iPackageName.Copy(argumentPtr);
			}
		else if(0 == argumentPtr.CompareF(KOptionVendorName))
			{
			if(++i >= argumentCount)
				{
				User::Leave(KMissingOptionsValue);
				}
			argumentPtr.Set(iCommandLineArguments->Arg(i));
			if(KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KMissingOptionsValue);
				}

			aPreferenceHandler.iVendorName.Copy(argumentPtr);
			}
		else if( 0 == argumentPtr.CompareF(KOptionVersion1) || 
				 0 == argumentPtr.CompareF(KOptionVersion2))
			{
			aPreferenceHandler.iOperation = ESwiVersion;
			}
		else if(0 == argumentPtr.CompareF(KOptionNormalMode))
			{
			aPreferenceHandler.iMode = ESwiNormalMode;
			}
		else if(0 == argumentPtr.CompareF(KOptionUnAttendedMode))
			{
			aPreferenceHandler.iMode = ESwiUnAttendedMode;
			}
		else if(0 == argumentPtr.CompareF(KOptionOptimalMode))
			{
			aPreferenceHandler.iMode = ESwiOptimalMode;
			}
		else if(0 == argumentPtr.CompareF(KOptionLog))
			{
			if( ++i >= argumentCount)
				{
				User::Leave(KMissingOptionsValue);
				}
			argumentPtr.Set(iCommandLineArguments->Arg(i));
			if(KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KMissingOptionsValue);
				}
			
			if(0 == argumentPtr.CompareF(KAnsEnable))
				{
				aPreferenceHandler.iIsLoggingEnabled = ETrue;
				}
			else if(0 == argumentPtr.CompareF(KAnsDisable))
				{
				aPreferenceHandler.iIsLoggingEnabled = EFalse;
				}
			else
				{
				User::Leave(KSwiInvalidConsoleArg);
				}
			}
		else if(0 == argumentPtr.CompareF(KOptionLogFile))
			{
			if( ++i >= argumentCount)
				{
				User::Leave(KMissingOptionsValue);
				}
			argumentPtr.Set(iCommandLineArguments->Arg(i));
			if(KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KMissingOptionsValue);
				}

			aPreferenceHandler.iLogFileName.Copy(argumentPtr);
			}
		else if(0 == argumentPtr.CompareF(KOptionVerbose))
			{
			aPreferenceHandler.iIsVerboseEnabled = ETrue;
			}
		else if(0 == argumentPtr.CompareF(KOptionProgressInfo))
			{
			if( ++i >= argumentCount)
				{
				User::Leave(KMissingOptionsValue);
				}
			
			argumentPtr.Set(iCommandLineArguments->Arg(i));
			if(KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KMissingOptionsValue);
				}
			
			if(0 == argumentPtr.CompareF(KAnsHide))
				{
				aPreferenceHandler.iProgessInfoType = EHideProgress;
				}
			else if(0 == argumentPtr.CompareF(KAnsShow))
				{
				aPreferenceHandler.iProgessInfoType = EShowProgress;
				}
			else if(0 == argumentPtr.CompareF(KAnsPrompt))
				{
				aPreferenceHandler.iProgessInfoType = EPromptProgress;
				}
			else
				{
				User::Leave(KSwiInvalidConsoleArg);
				}
			}
		else if(0 == argumentPtr.CompareF(KDisplayDrive))
			{
			if( ++i >= argumentCount)
				{
				User::Leave(KMissingOptionsValue);
				}
			argumentPtr.Set(iCommandLineArguments->Arg(i));
			if(KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KMissingOptionsValue);
				}
			
			if(1 != argumentPtr.Length() || KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KInvalidDriveName);
				}
		
			TChar driveLetter(argumentPtr[0]);
			
			if('#' != argumentPtr[0] && !driveLetter.IsAlpha())
				{
				User::Leave(KInvalidDriveName);
				}
				
			curPreference->iDrive = argumentPtr[0];
			}
		else if(0 == argumentPtr.CompareF(KDisplayLanguage))
			{
			if( ++i >= argumentCount)
				{
				User::Leave(KMissingOptionsValue);
				}
		
			argumentPtr.Set(iCommandLineArguments->Arg(i));
			if(KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KMissingOptionsValue);
				}
			
				if(KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KMissingOptionsValue);
				}
		
			curPreference->iLanguage = SwiUtility::GetLanguageType(argumentPtr);
			if(ELangNone == curPreference->iLanguage)
				{
				User::Leave(KSwiInvalidConsoleArg);
				}
			}
		else if(0 == argumentPtr.CompareF(KDisplayOption))
			{
			++i; // Get the arguement list
			
			curPreference->SetDialogOption(EDisplayOption, EAnswerYes);
			
			for(; i < argumentCount; ++i)
				{
				argumentPtr.Set(iCommandLineArguments->Arg(i));

				if(KOptionSymbol == argumentPtr[0])
					{
						--i;
						break;
					}
				
				TInt number = 0;
				TLex lexObj(argumentPtr);
				if(KErrNone != lexObj.Val(number))
					{
					User::Leave(KSwiInvalidConsoleArg);
					}
				
				// Entering negative value shall terminate the installation
				if(number < 0)
					{
					curPreference->SetDialogOption(EDisplayOption, EAnswerNo);
					}
				
				curPreference->iOptionList.Append(number);
				}
			// If no agrument is provided then it shall be assumed that
			// user wants to select no option.
			}
		else if(0 == argumentPtr.CompareF(KOcspRevocationServer))
			{
			if( ++i >= argumentCount)
				{
				User::Leave(KMissingOptionsValue);
				}
			argumentPtr.Set(iCommandLineArguments->Arg(i));
			if(KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KMissingOptionsValue);
				}

			aPreferenceHandler.iOcspServerUri.Copy(argumentPtr);
			}
		else if(0 == argumentPtr.CompareF(KOcspEnableCheck))
			{
			if( ++i >= argumentCount)
				{
				User::Leave(KMissingOptionsValue);
				}
			argumentPtr.Set(iCommandLineArguments->Arg(i));
			if(KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KMissingOptionsValue);
				}
			
			if(0 == argumentPtr.CompareF(KAnsYes))
				{
				aPreferenceHandler.iEnableOcspCheck = ETrue;
				}
			else if(0 == argumentPtr.CompareF(KAnsNo))
				{
				aPreferenceHandler.iEnableOcspCheck = EFalse;
				}
			else
				{
				User::Leave(KSwiInvalidConsoleArg);
				}
			}
		// Following  conditional statement is used to avoid code blot. 
		// Because for each options similar set of operations are executed.
		// Therefore one can club all those operations in a single block.
		//
		// In the following if statment each conditional check is preceded
		// by a increment of TInt type of variable. The conditional statement
		// and the increment is separated by comma operator. By providing
		// comma both of the statment will be executed but only the last
		// statement of the comma statement will be considered for evaluating
		// the condition... In this case result of CompareF will be checked
		// in the if statement not the dialogType value.
		else if(	(dialogType++, 0 == argumentPtr.CompareF(KDisplayInstall))		||
			(dialogType++, 0 == argumentPtr.CompareF(KGrantCapabilities))	||
			(dialogType++, 0 == argumentPtr.CompareF(KDisplayUpgrade))		||
			(dialogType++, 0 == argumentPtr.CompareF(KDisplayWarning))		||
			(dialogType++, 0 == argumentPtr.CompareF(KDisplayOCSPError))	||
			(dialogType++, 0 == argumentPtr.CompareF(KMissingDependency))	||
			(dialogType++, 0 == argumentPtr.CompareF(KDisplayText))			||
			(dialogType++, 0 == argumentPtr.CompareF(KQuestionIncompatible))||
			(dialogType++, 0 == argumentPtr.CompareF(KQuestionOverwrite))	||
			(dialogType++, 0 == argumentPtr.CompareF(KDisplayUnInstall))	||
			(dialogType++, 0 == argumentPtr.CompareF(KDisplayDependencyBreak))	||
			(dialogType++, 0 == argumentPtr.CompareF(KDisplayAppInUse))		||
			(dialogType++, 0 == argumentPtr.CompareF(KOptionDevCert))
		  )
			{
			if( ++i >= argumentCount)
				{
				User::Leave(KMissingOptionsValue);
				}
			argumentPtr.Set(iCommandLineArguments->Arg(i));
			if(KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KMissingOptionsValue);
				}
			
			TDialogType dialog = static_cast<TDialogType>(dialogType - 1);
			
			if(0 == argumentPtr.CompareF(KAnsYes))
				{
				curPreference->SetDialogOption(dialog, EAnswerYes);
				}
			else if(0 == argumentPtr.CompareF(KAnsNo))
				{
				curPreference->SetDialogOption(dialog, EAnswerNo);
				}
			else
				{
				User::Leave(KSwiInvalidConsoleArg);
				}
			
			continue;
			}
		else if((dialogType = EEventRemovingFiles, 
						0 == argumentPtr.CompareF(KOptionRemoveFiles))		||
			(dialogType = EEventShuttingDownApps, 
						0 == argumentPtr.CompareF(KOptionClosingApp))		||
			(dialogType = EEventCopyingFiles, 
						0 == argumentPtr.CompareF(KOptionCopyingFiles))		||
			(dialogType = EEventOcspCheckStart, 
						0 == argumentPtr.CompareF(KOptionOCSPCheckStart))	||
			(dialogType = EEventSetProgressBarFinalValue, 
						0 == argumentPtr.CompareF(KEventProgressFinal))		||
			(dialogType = EEventUpdateProgressBar, 
						0 == argumentPtr.CompareF(KEventProgressUpdate))	||
			(dialogType = EEventOcspCheckEnd, 
						0 == argumentPtr.CompareF(KEventOcspCheckEnd))		||
			(dialogType = EEventCompletedNoMessage, 
						0 == argumentPtr.CompareF(KEventCompletedNoMsg))	||
			(dialogType = EEventDevCert, 
						0 == argumentPtr.CompareF(KEventDevCertUsed))
		  )
			{
			if( ++i >= argumentCount)
				{
				User::Leave(KMissingOptionsValue);
				}
			
			argumentPtr.Set(iCommandLineArguments->Arg(i));
			if(KOptionSymbol == argumentPtr[0])
				{
				User::Leave(KMissingOptionsValue);
				}
			
			TEventType dialog = static_cast<TEventType>(dialogType);
			
			if(0 == argumentPtr.CompareF(KAnsYes))
				{
				curPreference->SetCancellableEventResponse(dialog, EAnswerYes);
				}
			else if(0 == argumentPtr.CompareF(KAnsNo))
				{
				curPreference->SetCancellableEventResponse(dialog, EAnswerNo);
				}
			else
				{
				User::Leave(KSwiInvalidConsoleArg);
				}
			}
		else if(0 == argumentPtr.CompareF(KEmbeddedSis))
			{
			curPreference = aPreferenceHandler.CreatePreferencesL();
			}
		else if(0 == argumentPtr.CompareF(KEmbeddedPackage))
			{
			++i;
			
			for(; i < argumentCount; ++i)
				{
				argumentPtr.Set(iCommandLineArguments->Arg(i));

				if(KOptionSymbol == argumentPtr[0])
					{
						--i;
						break;
					}
				
				curPreference->AddPackageNameL(argumentPtr);
				}
			}
		else if(0 == argumentPtr.CompareF(KEmbeddedVendor))
			{
			++i;
			
			for(; i < argumentCount; ++i)
				{
				argumentPtr.Set(iCommandLineArguments->Arg(i));

				if(KOptionSymbol == argumentPtr[0])
					{
						--i;
						break;
					}
				
				curPreference->AddVendorNameL(argumentPtr);
				}
			}
		else
			{
			User::Leave(KSwiInvalidConsoleArg);
			}
		}
	}


void CCommandLineParser::LogCommandLineArgumentsL(CPreferenceHandler& aPreferenceHandler)
	{
	if(!aPreferenceHandler.iIsLoggingEnabled)
		{
		return;
		}
	
	gIoHandler->InitLoggingL(aPreferenceHandler.iLogFileName);
	
	TInt argumentCount = iCommandLineArguments->Count();
	
	gIoHandler->WriteLineL(R_LOG_CMD_LINE_ARG_MSG, EIoLogType);

	for (TInt i = 0; i < argumentCount; ++i)
		{
		gIoHandler->WriteL(iCommandLineArguments->Arg(i), EIoLogType);
		gIoHandler->WriteL(_L(" "), EIoLogType);
		}
		
	gIoHandler->WriteL(KLineBreaker, EIoLogType);
	gIoHandler->WriteL(R_LOG_OPERATION_MSG, EIoLogType);
	switch(aPreferenceHandler.GetOperation())
		{
		case ESwiHelp:
			gIoHandler->WriteLineL(R_OPERATION_HELP_MSG, EIoLogType);
			break;
		case ESwiInstall:
			gIoHandler->WriteLineL(R_OPERATION_UNSTALL_MSG, EIoLogType);
			break;
		case ESwiUnInstall:
			gIoHandler->WriteLineL(R_OPERATION_UNINSTALL_MSG, EIoLogType);
			break;
		case ESwiList:
			gIoHandler->WriteLineL(R_OPERATION_LIST_MSG, EIoLogType);
			break;
		case ESwiVersion:
			gIoHandler->WriteLineL(R_OPERATION_VERSION_MSG, EIoLogType);
			break;
		}
	
	gIoHandler->WriteL(R_LOG_OPERATION_MODE_MSG, EIoLogType);
	switch(aPreferenceHandler.GetOperationMode())
		{
		case ESwiNormalMode:
			gIoHandler->WriteLineL(R_MODE_NORMAL_MSG, EIoLogType);
			break;
		case ESwiUnAttendedMode:
			gIoHandler->WriteLineL(R_MODE_UNATTENDED_MSG, EIoLogType);
			break;
		case ESwiOptimalMode:
			gIoHandler->WriteLineL(R_MODE_OPTIMAL_MSG, EIoLogType);
			break;
		}
	
	gIoHandler->WriteL(KLineBreaker, EIoLogType);
	}


//------------------------ EOF ------------------------------------------
