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
* cconsloeuihandler.cpp
* CConoleUiHandler: This class implements all the callback functions
* exposed by MUiHandler class. This class is responsible for user
* interactions and communication with the SWI installer.
* @internalComponent
* Macros
* List of names of all supported capabilities
* For defining capability strings
*
*/


#define __INCLUDE_CAPABILITY_NAMES__

// System Header Files
#include <e32capability.h>			// Capability Strings

// User Header Files
#include "cconsoleuihandler.h"
#include "cpreferences.h"
#include "cpreferencehandler.h"
#include "swiutility.h"
#include "swiconsoleconstants.h"
#include "swiconsoleerrors.h"

// Literals
_LIT(KAnswerYes, "y");
_LIT(KAnswerNo, "n");
_LIT(KAnswerDisplay, "d");

// Constants
const TInt KCapabilityStringLength = 50;
const TInt KInputCharLen = 1;
const TInt KInputTwoCharLen = 2;

// Extern variable defined in main.cpp
extern CIoHandler* gIoHandler;


CConsoleUiHandler::CConsoleUiHandler(const CPreferenceHandler& aPreferenceHandler)
							:iPreferenceHandler(aPreferenceHandler)
	{
	}

CConsoleUiHandler* CConsoleUiHandler::NewL(const CPreferenceHandler& aPreferenceHandler)
	{
	CConsoleUiHandler* self = NewLC(aPreferenceHandler);
	CleanupStack::Pop(self);
	return self;
	}

CConsoleUiHandler* CConsoleUiHandler::NewLC(const CPreferenceHandler& aPreferenceHandler)
	{
	CConsoleUiHandler* ptrObj = new(ELeave) CConsoleUiHandler(aPreferenceHandler);
	CleanupStack::PushL(ptrObj);
	return ptrObj;
	}

	// Members from MCommonDialogs
TBool CConsoleUiHandler::DisplayTextL(
	 				const Swi::CAppInfo& 	aAppInfo,
					Swi::TFileTextOption 	aOption,
					const TDesC& 			aText)
	{
	CPreferences* curPreferences = iPreferenceHandler.GetPreferences(aAppInfo);
	TInt8 value = curPreferences->GetDialogOption(EDisplayText);

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ENotAnswered);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;

	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteLineL(R_DIALOG_TEXT_MSG, displayMode);
	gIoHandler->WriteL(R_APP_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppName(), displayMode);
	gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppVendor(), displayMode);
	gIoHandler->WriteLineL(aText, displayMode);
	gIoHandler->WriteLineL(R_LOG_DISPLAY_TEXT_MSG, EIoLogType);
	gIoHandler->WriteL(R_LOG_TEXT_TYPE_MSG, EIoLogType);

	TInt yesStringId = 0;
	TInt noStringId = 0;

	switch(aOption)
		{
		case Swi::EInstFileTextOptionContinue:
			{
			gIoHandler->WriteLineL(R_LOG_TEXT_CONTINUE_TYPE_MSG, EIoLogType);
			break;
			}
		case Swi::EInstFileTextOptionSkipOneIfNo:
			{
			gIoHandler->WriteL(R_LOG_TEXT_SKIPONE_TYPE_MSG, EIoLogType);
			if(showDialog)
				{
				gIoHandler->WriteLineL(R_TEXT_CONFIRM_FILE_SKIP_MSG, EIoConsoleType);
				}
			yesStringId = R_TEXT_PRESS_Y_TO_SKIP_FILE_MSG;
			noStringId = R_TEXT_PRESS_N_TO_INSTALL_FILE_MSG;
			break;
			}
		case Swi::EInstFileTextOptionAbortIfNo:
			{
			gIoHandler->WriteLineL(R_LOG_TEXT_ABORTIFNO_TYPE_MSG, EIoLogType);
			yesStringId = R_INSTALL_Y_TO_CONTINUE_MSG;
			noStringId = R_INSTALL_N_TO_ABORT_MSG;
			break;
			}
		case Swi::EInstFileTextOptionExitIfNo:
			{
			gIoHandler->WriteLineL(R_LOG_TEXT_EXITIFNO_TYPE_MSG, EIoLogType);
			yesStringId = R_INSTALL_Y_TO_CONTINUE_MSG;
			noStringId = R_INSTALL_N_TO_ABORT_MSG;
			break;
			}
		default:
			{
			User::Panic(KGeneralPanicString, KSwiInvalidSwitchCase);
			}
		}

	switch(value)
		{
		case EAnswerNo:
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			return EFalse;
		case EAnswerYes:
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			return ETrue;
		}

	displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

	// In Optimal mode it will continue installation or allow copying of
	// of the next file(EInstFileTextOptionSkipOneIfNo)
	if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_YES_MSG, displayMode);
		return ETrue;
		}

	// In Un-Attended mode it will throw an exception
	if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
		if(value == ENotAnswered)
			{
			User::Leave(KMissingMandatoryOption);
			}
		User::Leave(KSwiInvalidConsoleArg);
		}

	if(Swi::EInstFileTextOptionContinue == aOption)
		{
		if((value == ENotAnswered) && ESwiNormalMode == iPreferenceHandler.GetOperationMode())
			{
			gIoHandler->PauseScreenL();
			}
		return ETrue;
		}

	gIoHandler->WriteLineL(yesStringId, EIoConsoleType);
	gIoHandler->WriteLineL(noStringId, EIoConsoleType);

	TBuf<KInputCharLen> option;
	TBool retValue = EFalse;
	FOREVER
		{
		gIoHandler->ClearLine();
		gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG, EIoConsoleType);
		gIoHandler->ReadStringL(option);

		if(0 == option.CompareF(KAnswerYes))
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			retValue = ETrue;
			break;
			}

		if(0 == option.CompareF(KAnswerNo))
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			break;
			}
		gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
		}

	return retValue;
	}

void CConsoleUiHandler::DisplayErrorL(
					const Swi::CAppInfo&	aAppInfo,
					Swi::TErrorDialog 		aType,
					const TDesC& 			aParam)
	{
	gIoHandler->WriteL(KLineBreaker);
	gIoHandler->WriteL(KLineBreaker);
	gIoHandler->WriteLineL(R_DIALOG_ERROR_MSG);
	gIoHandler->WriteL(R_APP_NAME_MSG);
	gIoHandler->WriteLineL(aAppInfo.AppName());
	gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG);
	gIoHandler->WriteLineL(aAppInfo.AppVendor());
	gIoHandler->WriteL(R_ERR_MSG);

	switch(aType)
		{
		case Swi::EUiAlreadyInRom:
			gIoHandler->WriteLineL(R_ERR_ALREADY_IN_ROM_MSG);
			gIoHandler->WriteL(R_FILE_NAME_MSG);
			gIoHandler->WriteLineL(aParam);
			break;
		case Swi::EUiMissingDependency:
			gIoHandler->WriteLineL(R_ERR_MISSING_DEP_MSG);
			break;
		case Swi::EUiRequireVer:
			gIoHandler->WriteLineL(R_ERR_REQUIRE_VER_MSG);
			break;
		case Swi::EUiRequireVerOrGreater:
			gIoHandler->WriteLineL(R_ERR_REQUIRE_VER_OR_GR8_MSG);
			break;
		case Swi::EUiFileCorrupt:
			gIoHandler->WriteLineL(R_ERR_FILE_CORRUPT_MSG);
			break;
		case Swi::EUiDiskNotPresent:
			gIoHandler->WriteLineL(R_ERR_DISK_NOT_PRESENT_MSG);
			gIoHandler->WriteL(R_FILE_NAME_MSG);
			gIoHandler->WriteLineL(aParam);
			break;
		case Swi::EUiCannotRead:
			gIoHandler->WriteLineL(R_ERR_CANNOT_READ_MSG);
			break;
		case Swi::EUiCannotDelete:
			gIoHandler->WriteLineL(R_ERR_CANNOT_DELETE_MSG);
			gIoHandler->WriteL(R_FILE_NAME_MSG);
			gIoHandler->WriteLineL(aParam);
			break;
		case Swi::EUiInvalidFileName:
			gIoHandler->WriteLineL(R_ERR_INVALID_FILENAME_MSG);
			gIoHandler->WriteL(R_FILE_NAME_MSG);
			gIoHandler->WriteLineL(aParam);
			break;
		case Swi::EUiFileNotFound:
			gIoHandler->WriteLineL(R_ERR_FILE_NOT_FOUND_MSG);
			break;
		case Swi::EUiInsufficientSpaceOnDrive:
			gIoHandler->WriteLineL(R_ERR_INSUFFICIENT_DISK_SPACE_MSG);
			break;
		case Swi::EUiCapabilitiesCannotBeGranted:
			gIoHandler->WriteLineL(R_ERR_CANT_GRANT_CAPBILITY_MSG);
			break;
		case Swi::EUiUnknownFile:
			gIoHandler->WriteLineL(R_ERR_UNKNOWN_FILE_MSG);
			break;
		case Swi::EUiMissingBasePackage:
			gIoHandler->WriteLineL(R_ERR_MISSING_BASE_PKG_MSG);
			break;
		case Swi::EUiConstraintsExceeded:
			gIoHandler->WriteLineL(R_ERR_CONSTRAINTS_EXCEEDED_MSG);
			break;
		case Swi::EUiSIDViolation:
			gIoHandler->WriteLineL(R_ERR_SID_VIOLATION_MSG);
			break;
		case Swi::EUiVIDViolation:
			gIoHandler->WriteLineL(R_ERR_VID_VIOLATION_MSG);
			break;
		case Swi::EUiNoMemoryInDrive:
			gIoHandler->WriteLineL(R_ERR_NO_MEMORY_IN_DRIVE_MSG);
			break;
		case Swi::EUiUIDPackageViolation:
			gIoHandler->WriteLineL(R_ERR_PKG_UID_VIOLATION_MSG);
			break;
		case Swi::EUiOSExeViolation:
			gIoHandler->WriteLineL(R_ERR_EXE_VIOLATION_MSG);
			break;
		case Swi::EUiSIDMismatch:
			gIoHandler->WriteLineL(R_ERR_SID_MISMATACH_MSG);
			break;
		case Swi::EUiBlockingEclipsingFile:
			gIoHandler->WriteLineL(R_ERR_BLOCKING_ECLIPSING_FILE_MSG);
			gIoHandler->WriteL(R_FILE_NAME_MSG);
			gIoHandler->WriteLineL(aParam);
			break;
		default:
			{
			User::Panic(KGeneralPanicString, KSwiInvalidSwitchCase);
			}
		}

	if(ESwiNormalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->PauseScreenL();
		}

	return;
	}

TBool CConsoleUiHandler::DisplayDependencyBreakL(
					const Swi::CAppInfo& 			aAppInfo,
					const RPointerArray<TDesC>& 	aComponents)
	{
	CPreferences* curPreferences = iPreferenceHandler.GetPreferences(aAppInfo);
	TInt8 value = curPreferences->GetDialogOption(EDisplayDependencyBreak);

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ENotAnswered);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;

	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteLineL(R_DIALOG_DEPENDECY_BREAK_MSG, displayMode);
	gIoHandler->WriteL(R_APP_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppName(), displayMode);
	gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppVendor(), displayMode);
	gIoHandler->WriteLineL(R_DEPENDENCY_BREAK_MSG, displayMode);
	gIoHandler->WriteLineL(R_DEPENDED_COMPONENT_LIST_MSG, displayMode);

	for(TInt i = aComponents.Count() - 1; i >= 0; --i)
		{
		gIoHandler->WriteLineL(*aComponents[i], displayMode);
		}

	switch(value)
		{
		case EAnswerNo:
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			return EFalse;
		case EAnswerYes:
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			return ETrue;
		}

	displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

	if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_YES_MSG, displayMode);
		return ETrue;
		}
	if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
		if(value == ENotAnswered)
			{
			User::Leave(KMissingMandatoryOption);
			}
		User::Leave(KSwiInvalidConsoleArg);
		}

	gIoHandler->WriteLineL(R_UNINSTALL_Y_TO_CONTINUE_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(R_UNINSTALL_N_TO_ABORT_MSG, EIoConsoleType);

	TBuf<KInputCharLen> option;
	TBool retValue = EFalse;
	FOREVER
		{
		gIoHandler->ClearLine();
		gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG, EIoConsoleType);
		gIoHandler->ReadStringL(option);

		if(0 == option.CompareF(KAnswerYes))
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			retValue = ETrue;
			break;
			}

		if(0 == option.CompareF(KAnswerNo))
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			break;
			}
		gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
		}
	return retValue;
	}

TBool CConsoleUiHandler::DisplayApplicationsInUseL(
					const Swi::CAppInfo& 		aAppInfo,
					const RPointerArray<TDesC>&	aAppNames)
	{
	CPreferences* curPreferences = iPreferenceHandler.GetPreferences(aAppInfo);
	TInt8 value = curPreferences->GetDialogOption(EDisplayAppInUse);

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ENotAnswered);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;

	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteLineL(R_DIALOG_APP_IN_USE_MSG, displayMode);
	gIoHandler->WriteL(R_APP_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppName(), displayMode);
	gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppVendor(), displayMode);
	gIoHandler->WriteLineL(R_APP_IN_USE_MSG, displayMode);

	for(TInt i = aAppNames.Count() - 1; i >= 0; --i)
		{
		gIoHandler->WriteLineL(*aAppNames[i], displayMode);
		}

	switch(value)
		{
		case EAnswerNo:
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			return EFalse;
		case EAnswerYes:
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			return ETrue;
		}

	displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

	if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_YES_MSG, displayMode);
		return ETrue;
		}
	if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
		if(value == ENotAnswered)
			{
			User::Leave(KMissingMandatoryOption);
			}
		User::Leave(KSwiInvalidConsoleArg);
		}

	gIoHandler->WriteLineL(R_UNINSTALL_Y_TO_CONTINUE_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(R_UNINSTALL_N_TO_ABORT_MSG, EIoConsoleType);

	TBuf<KInputCharLen> option;
	TBool retValue = EFalse;
	FOREVER
		{
		gIoHandler->ClearLine();
		gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG, EIoConsoleType);
		gIoHandler->ReadStringL(option);

		if(0 == option.CompareF(KAnswerYes))
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			retValue = ETrue;
			break;
			}

		if(0 == option.CompareF(KAnswerNo))
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			break;
			}
		gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
		}
	return retValue;
	}

TBool CConsoleUiHandler::DisplayQuestionL(
					const Swi::CAppInfo&	aAppInfo,
					Swi::TQuestionDialog	aQuestion,
					const TDesC& aDes)
	{
	CPreferences* curPreferences = iPreferenceHandler.GetPreferences(aAppInfo);
	TInt8 value = ENotAnswered;
	switch(aQuestion)
		{
		case Swi::EQuestionIncompatible:
			value = curPreferences->GetDialogOption(EDisplayQuestionIncompatible);
			break;
		case Swi::EQuestionOverwriteFile:
			value = curPreferences->GetDialogOption(EDisplayQuestionOverwrite);
			break;
		default:
			{
			User::Panic(KGeneralPanicString, KSwiInvalidSwitchCase);
			}
		}

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ENotAnswered);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;

	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteLineL(R_DIALOG_QUESTION_MSG, displayMode);
	gIoHandler->WriteL(R_APP_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppName(), displayMode);
	gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppVendor(), displayMode);

	switch(aQuestion)
		{
		case Swi::EQuestionIncompatible:
			gIoHandler->WriteLineL(R_QUE_INCOMPATIBLE_MSG, displayMode);
			break;
		case Swi::EQuestionOverwriteFile:
			gIoHandler->WriteLineL(R_QUE_OVERWRITE_FILE_MSG, displayMode);
			gIoHandler->WriteL(R_FILE_NAME_MSG, displayMode);
			gIoHandler->WriteLineL(aDes, displayMode);
		}

	switch(value)
		{
		case EAnswerNo:
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			return EFalse;
		case EAnswerYes:
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			return ETrue;
		}

	displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

	if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_YES_MSG, displayMode);
		return ETrue;
		}
	if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
		if(value == ENotAnswered)
			{
			User::Leave(KMissingMandatoryOption);
			}
		User::Leave(KSwiInvalidConsoleArg);
		}

	gIoHandler->WriteLineL(R_QUE_PRESS_Y_FOR_YES_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(R_QUE_PRESS_N_FOR_NO_MSG, EIoConsoleType);

	TBuf<KInputCharLen> option;
	TBool retValue = EFalse;
	FOREVER
		{
		gIoHandler->ClearLine();
		gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG);
		gIoHandler->ReadStringL(option);

		if(0 == option.CompareF(KAnswerYes))
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			retValue = ETrue;
			break;
			}

		if(0 == option.CompareF(KAnswerNo))
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			break;
			}
		gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
		}

	return retValue;
	}

	//Members from MInstallerUiHandler
TBool CConsoleUiHandler::DisplayInstallL(
					const Swi::CAppInfo&		aAppInfo,
					const CApaMaskedBitmap* /*  aLogo*/,
					const RPointerArray<Swi::CCertificateInfo>& aCertificates)
	{
	CPreferences* curPreferences = iPreferenceHandler.GetDefaultPreference();
	TInt8 value = curPreferences->GetDialogOption(EDisplayInstall);

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ENotAnswered);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;

	TBool isFirstExecution = ETrue;
	TBool retValue = EFalse;
	FOREVER
		{
		gIoHandler->ClearConsoleL();
		gIoHandler->WriteLineL(R_DIALOG_INSTALL_MSG, displayMode);
		gIoHandler->WriteLineL(R_INSTALL_MSG, displayMode);
		DisplayAppInfoL(aAppInfo, displayMode);

		if(isFirstExecution)
			{
			displayMode = (isVerboseModeEnabled && (value != ENotAnswered))? EIoBoth : EIoLogType;
			gIoHandler->WriteL(KLineBreaker, displayMode);
			DisplayCertificateListL(aCertificates, displayMode);

			switch(value)
				{
				case EAnswerNo:
					gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
					gIoHandler->WriteLineL(R_NO_MSG, displayMode);
					return EFalse;
				case EAnswerYes:
					gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
					gIoHandler->WriteLineL(R_YES_MSG, displayMode);
					return ETrue;
				}

			displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

			if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
				{
				gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
				gIoHandler->WriteLineL(R_YES_MSG, displayMode);
				retValue = ETrue;
				break;
				}
			if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
				{
				gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
				gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
				if(value == ENotAnswered)
					{
					User::Leave(KMissingMandatoryOption);
					}
				User::Leave(KSwiInvalidConsoleArg);
				}
			isFirstExecution = EFalse;
			}

		gIoHandler->WriteLineL(R_INSTALL_Y_TO_CONTINUE_MSG, EIoConsoleType);
		gIoHandler->WriteLineL(R_INSTALL_N_TO_ABORT_MSG, EIoConsoleType);
		gIoHandler->WriteLineL(R_INSTALL_D_TO_VIEW_CERT_MSG, EIoConsoleType);

		TBuf<KInputCharLen> option;
		FOREVER
			{
			gIoHandler->ClearLine();
			gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG, EIoConsoleType);
			gIoHandler->ReadStringL(option);
			displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

			if(0 == option.CompareF(KAnswerYes))
				{
				gIoHandler->WriteL(KLineBreaker, displayMode);
				gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
				gIoHandler->WriteLineL(R_YES_MSG, displayMode);
				return ETrue;
				}

			if(0 == option.CompareF(KAnswerNo))
				{
				gIoHandler->WriteL(KLineBreaker, displayMode);
				gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
				gIoHandler->WriteLineL(R_NO_MSG, displayMode);
				return EFalse;
				}

			if(0 == option.CompareF(KAnswerDisplay))
				{
				gIoHandler->ClearConsoleL();
				DisplayCertificateListL(aCertificates, EIoConsoleType);
				gIoHandler->PauseScreenL();
				break;
				}
			gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
			} // FOREVER
		displayMode = EIoConsoleType;
		}// FOREVER

	return retValue;
	}

TBool CConsoleUiHandler::DisplayGrantCapabilitiesL(
					const Swi::CAppInfo&	aAppInfo,
					const TCapabilitySet&	aCapabilitySet)
	{
	CPreferences* curPreferences = iPreferenceHandler.GetPreferences(aAppInfo);
	TInt8 value = curPreferences->GetDialogOption(EGrantCapabilities);

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ENotAnswered);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;

	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteLineL(R_DIALOG_GRANT_CAPABILITIES_MSG, displayMode);
	gIoHandler->WriteL(R_APP_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppName(), displayMode);
	gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppVendor(), displayMode);
	gIoHandler->WriteLineL(R_GRANT_USER_CAPABILITIES_MSG, displayMode);
	DisplayCapbilitiesL(aCapabilitySet, displayMode);

	switch(value)
		{
		case EAnswerNo:
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			return EFalse;
		case EAnswerYes:
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			return ETrue;
		}

	displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

	if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_YES_MSG, displayMode);
		return ETrue;
		}
	if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
		if(value == ENotAnswered)
			{
			User::Leave(KMissingMandatoryOption);
			}
		User::Leave(KSwiInvalidConsoleArg);
		}

	gIoHandler->WriteLineL(R_INSTALL_Y_TO_CONTINUE_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(R_INSTALL_N_TO_ABORT_MSG, EIoConsoleType);

	TBuf<KInputCharLen> option;
	TBool retValue = EFalse;
	FOREVER
		{
		gIoHandler->ClearLine();
		gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG, EIoConsoleType);
		gIoHandler->ReadStringL(option);

		if(0 == option.CompareF(KAnswerYes))
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			retValue = ETrue;
			break;
			}

		if(0 == option.CompareF(KAnswerNo))
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			break;
			}
		gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
		} //FOREVER
	return retValue;
	}

TInt CConsoleUiHandler::DisplayLanguageL(
					const Swi::CAppInfo&		aAppInfo,
					const RArray<TLanguage>&	aLanguages)
	{
	CPreferences* curPreferences = iPreferenceHandler.GetPreferences(aAppInfo);
	TLanguage value = curPreferences->GetLanguage();

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ELangNone);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;
	TInt langIndex = aLanguages.Find(value);
	if(KErrNotFound == langIndex)
		{
		displayMode = EIoBoth;
		}

	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteLineL(R_DIALOG_LANGUAGE_SELECTION_MSG, displayMode);
	gIoHandler->WriteL(R_APP_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppName(), displayMode);
	gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppVendor(), displayMode);
	gIoHandler->WriteLineL(R_DISPLAY_LANGUAGE_MSG, displayMode);
	DisplayLanguageListL(aLanguages, displayMode);

	// ELangTest is used to cancel the insatllation
	if(ELangTest == value)
		{
		gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
		gIoHandler->WriteLineL(R_CANCEL_MSG, displayMode);
		User::Leave(KErrCancel);
		}

	if(ELangNone != value)
		{
		if(KErrNotFound != langIndex)
			{
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(languageArr[aLanguages[langIndex]], displayMode);
			return langIndex;
			}
		gIoHandler->WriteLineL(R_LOG_INVALID_INPUT_MSG);
		}

	displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

	if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(languageArr[aLanguages[0]], displayMode);
		return 0; // By default it shall select the first language
		}
	if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
		if(value == ELangNone)
			{
			User::Leave(KMissingMandatoryOption);
			}
		User::Leave(KSwiInvalidConsoleArg);
		}

	gIoHandler->WriteLineL(R_PRESS_ESC_TO_CANCEL_MSG, EIoConsoleType);

	TBuf<KInputTwoCharLen> option;
	TInt findResult = KErrNotFound;
	FOREVER
		{
		gIoHandler->ClearLine();
		gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG, EIoConsoleType);
		gIoHandler->ReadStringL(option);

		value = SwiUtility::GetLanguageType(option);

		findResult = aLanguages.Find(value);

		if(KErrNotFound != findResult)
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(languageArr[aLanguages[findResult]], displayMode);
			break;
			}
		gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
		} // FOREVER

	return findResult;
	}

TInt CConsoleUiHandler::DisplayDriveL(
					const Swi::CAppInfo& 	aAppInfo,
					TInt64 					aSize,
					const RArray<TChar>&   	aDriveLetters,
					const RArray<TInt64>&   aDriveSpaces)
	{
	CPreferences* curPreferences = iPreferenceHandler.GetPreferences(aAppInfo);
	TChar driveLetter = curPreferences->GetSelectedDrive();

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (driveLetter == EKeyNull);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;
	TInt driveIndex = aDriveLetters.Find(driveLetter);

	if(driveLetter != EKeyNull && KErrNotFound == driveIndex)
		{
		if(driveLetter.IsLower())
			{
			driveIndex = aDriveLetters.Find(driveLetter.GetUpperCase());
			}
			else
			{
			driveIndex = aDriveLetters.Find(driveLetter.GetLowerCase());
			}
		if(KErrNotFound == driveIndex || aSize >= aDriveSpaces[driveIndex])
			{
			displayMode = EIoBoth;
			}
		}

	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteLineL(R_DIALOG_DRIVE_SELECTION_MSG, displayMode);
	gIoHandler->WriteL(R_APP_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppName(), displayMode);
	gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppVendor(), displayMode);
	gIoHandler->WriteLineL(R_DRIVE_SELECTION_MSG, displayMode);

	TInt driveCount = aDriveLetters.Count();

	for(TInt i = 0; i < driveCount; ++i)
		{
		if(aSize > aDriveSpaces[i])
			{
			continue;
			}
		TBuf<KDefaultStringLen> buffer;
		buffer.Format(_L("%c <%dKb>"), static_cast<TUint>(aDriveLetters[i]), aDriveSpaces[i]/1024);
		gIoHandler->WriteLineL(buffer, displayMode);
		}

	// This character is used to cancel installation
	if('#' == driveLetter)
		{
		gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
		gIoHandler->WriteLineL(R_CANCEL_MSG, displayMode);
		User::Leave(KErrCancel);
		}

	if(EKeyNull != driveLetter)
		{
		if(KErrNotFound != driveIndex)
			{
			if(aSize < aDriveSpaces[driveIndex])
				{
				gIoHandler->WriteL(KLineBreaker, displayMode);
				gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
				TBuf<KDefaultStringLen> buffer;
				buffer.Format(_L("%c Drive"), static_cast<TUint>(aDriveLetters[driveIndex]));
				gIoHandler->WriteLineL(buffer, displayMode);
				return driveIndex;
				}
			gIoHandler->WriteLineL(R_LOG_INSUFFICIENT_SPACE_IN_DRIVE_MSG, displayMode);
			}
		else
			{
			gIoHandler->WriteLineL(R_LOG_INVALID_INPUT_MSG, displayMode);
			}
		}

	displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

	// In Optimal mode it will return the first drive with enough space
	if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		for(TInt i = 0; i < driveCount; ++i)
			{
			if(aSize < aDriveSpaces[i])
				{
				TBuf<KDefaultStringLen> buffer;
				buffer.Format(_L("%c <%dKb> Drive"), static_cast<TUint>(aDriveLetters[i]), aDriveSpaces[i]/1024);
				gIoHandler->WriteLineL(buffer, displayMode);
				return i;
				}
			}
		// None of the disk has sufficient disk space
		gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
		gIoHandler->WriteL(R_REASON_MSG, displayMode);
		gIoHandler->WriteLineL(R_NO_DISK_SPACE_MSG, displayMode);
		User::Leave(KErrDiskFull);
		}
	// In Un-attended mode the tool shall throw an error
	if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
		if(driveLetter == EKeyNull)
			{
			User::Leave(KMissingMandatoryOption);
			}
		User::Leave(KSwiInvalidConsoleArg);
		}

	gIoHandler->WriteLineL(R_PRESS_ESC_TO_CANCEL_MSG, EIoConsoleType);

	TBuf<KInputCharLen> option;
	driveIndex = KErrNotFound;
	// In Normal mode get the input from the user. The user shall be
	// prompted until user gives the correct answer.
	FOREVER
		{
		gIoHandler->ClearLine();
		gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG, EIoConsoleType);
		gIoHandler->ReadStringL(option);
		if(option.Length() <= 0)
			{
			continue;
			}
		driveLetter = option[0];

		driveIndex = aDriveLetters.Find(driveLetter);

		if(KErrNotFound == driveIndex)
			{
			if(driveLetter.IsLower())
				{
				driveIndex = aDriveLetters.Find(driveLetter.GetUpperCase());
				}
				else
				{
				driveIndex = aDriveLetters.Find(driveLetter.GetLowerCase());
				}
			}

		if(KErrNotFound != driveIndex)
			{
			if(aSize < aDriveSpaces[driveIndex])
				{
				gIoHandler->WriteL(KLineBreaker, displayMode);
				gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
				TBuf<KDefaultStringLen> buffer;
				buffer.Format(_L("%c <%dKb> Drive"), static_cast<TUint>(aDriveLetters[driveIndex]), aDriveSpaces[driveIndex]/1024);
				gIoHandler->WriteLineL(buffer, displayMode);
				break;
				}
			}
		gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
		} // FOREVER
	return driveIndex;
	}

TBool CConsoleUiHandler::DisplayUpgradeL(
					const Swi::CAppInfo& aAppInfo,
					const Swi::CAppInfo& aExistingAppInfo)
	{
	CPreferences* curPreferences = iPreferenceHandler.GetPreferences(aAppInfo);
	// Check whether the answer is already provided in the command
	// line or not. If yes then retrieve the answer
	TInt8 value = curPreferences->GetDialogOption(EDisplayUpgrade);

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ENotAnswered);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;

	// Display the dialog in the console
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteLineL(R_DIALOG_PACKAGE_UPGRADE_MSG, displayMode);
	gIoHandler->WriteLineL(R_UPGRADE_CONFIRMATION_MSG, displayMode);
	gIoHandler->WriteLineL(R_UPGRADE_EXISTING_PKG_DETAILS, displayMode);
	DisplayAppInfoL(aExistingAppInfo, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteLineL(R_UPGRADE_NEW_PKG_DETAILS, displayMode);
	DisplayAppInfoL(aAppInfo, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);

	switch(value)
		{
		case EAnswerNo:
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			return EFalse;
		case EAnswerYes:
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			return ETrue;
		}

	displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

	// In optimal mode it shall return true
	if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_YES_MSG, displayMode);
		return ETrue;
		}
	// In Un-attended mode it shall throw an error
	if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
		if(value == ENotAnswered)
			{
			User::Leave(KMissingMandatoryOption);
			}
		User::Leave(KSwiInvalidConsoleArg);
		}

	gIoHandler->WriteLineL(R_INSTALL_Y_TO_CONTINUE_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(R_INSTALL_N_TO_ABORT_MSG, EIoConsoleType);

	TBuf<KInputCharLen> option;
	TBool retValue = EFalse;
	// In Normal mode get the input from the user. The user shall be
	// prompted until he/she gives the correct answer.
	FOREVER
		{
		gIoHandler->ClearLine();
		gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG, EIoConsoleType);
		gIoHandler->ReadStringL(option);

		if(0 == option.CompareF(KAnswerYes))
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			retValue = ETrue;
			break;
			}

		if(0 == option.CompareF(KAnswerNo))
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			break;
			}
		gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
		} //FOREVER

	return retValue;
	}

TBool CConsoleUiHandler::DisplayOptionsL(
					const Swi::CAppInfo& 		aAppInfo,
					const RPointerArray<TDesC>& aOptions,
					RArray<TBool>&   			aSelections)
	{
	CPreferences* curPreferences = iPreferenceHandler.GetPreferences(aAppInfo);
	TInt8 value = curPreferences->GetDialogOption(EDisplayOption);

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ENotAnswered);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;
	TBool validOptionList = ETrue;
	const RArray<TInt>& refOption = curPreferences->GetOptionList();
	if(EAnswerYes == value)
		{
		if(KErrNone != ProcessOptionList(refOption, aSelections))
			{
			displayMode = EIoBoth;
			validOptionList = EFalse;
			}
		}

	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteLineL(R_DIALOG_OPTION_LIST_MSG, displayMode);
	gIoHandler->WriteL(R_APP_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppName(), displayMode);
	gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppVendor(), displayMode);
	gIoHandler->WriteLineL(R_OPTION_SELECT_MSG, displayMode);
	TInt optionCount = aOptions.Count();

	for(TInt i = 0; i < optionCount; i++)
		{
		TBuf<KDefaultStringLen> buffer;
		buffer.Format(_L("%d) "), i + 1);
		buffer.Append(*aOptions[i]);
		gIoHandler->WriteLineL(buffer, displayMode);
		}

	switch(value)
		{
		case EAnswerNo: // User wants to cancel the installation
			{
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_CANCEL_MSG, displayMode);
			return EFalse;
			}
		case EAnswerYes: // User input is given
			{
			if(validOptionList)
				{
				gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
				TBuf<KDefaultStringLen> buffer;
				for(TInt i = 0; i < refOption.Count(); ++i)
					{
					buffer.AppendNum(refOption[i]);
					buffer.Append(_L(" "));
					}
				gIoHandler->WriteL(buffer, displayMode);
				return ETrue;
				}
			gIoHandler->WriteLineL(R_LOG_INVALID_INPUT_MSG, displayMode);
			}
		}
	// User input is not given or invalid input

	displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

	// In optimal mode it shall not select any option
	if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_NONE_MSG, displayMode);
		SwiUtility::FillArray(aSelections, EFalse);
		return ETrue;
		}
	// In Un-attended mode it shall throw an error
	if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
		if(value == ENotAnswered)
			{
			User::Leave(KMissingMandatoryOption);
			}
		User::Leave(KSwiInvalidConsoleArg);
		}

	gIoHandler->WriteLineL(R_OPTION_ENTER_WITH_SPACES_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(R_PRESS_ESC_TO_CANCEL_MSG, EIoConsoleType);

	RArray<TInt> optionList;
	CleanupResetPushL(optionList);
	// In Normal mode get the input from the user. The user shall be
	// prompted until he/she gives the correct answer.
	FOREVER
		{
		optionList.Reset();
		gIoHandler->ClearLine();
		gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG);
		TRAPD(err, gIoHandler->ReadIntArrayL(optionList));

		if(KErrCancel == err)
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_CANCEL_MSG, displayMode);
			CleanupStack::PopAndDestroy(); // optionList;
			return EFalse;
			}

		if(KErrNone == err)
			{
			// Verify whether options entered are valid or not
			// Clear previous entrie, if any...
			SwiUtility::FillArray(aSelections, EFalse);
			if(KErrNone == ProcessOptionList(optionList, aSelections))
				{
				gIoHandler->WriteL(KLineBreaker, displayMode);
				gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
				TBuf<KDefaultStringLen> buffer;
				for(TInt i = 0; i < optionList.Count(); ++i)
					{
					buffer.AppendNum(optionList[i]);
					buffer.Append(_L(" "));
					}
				gIoHandler->WriteL(buffer, displayMode);
				break;
				}
			}
		gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
		} //FOREVER

	CleanupStack::PopAndDestroy(); // optionList;
	return ETrue;
	}

TBool CConsoleUiHandler::HandleInstallEventL(
					const Swi::CAppInfo& 	aAppInfo,
					Swi::TInstallEvent   	aEvent,
					TInt   					aValue,
					const TDesC& 			aDes)
	{
	if(Swi::EEventUpdateProgressBar == aEvent)
		{
		gIoHandler->UpdateProgressBar(aValue);
		if(EPromptProgress != iPreferenceHandler.GetProgressInfoType())
			{
			return ETrue;
			}
		}
	CPreferences* curPreferences = iPreferenceHandler.GetPreferences(aAppInfo);
	TEventType eventType = static_cast<TEventType>(EEventSetProgressBarFinalValue + aEvent);
	TInt8 value = curPreferences->GetCancellableEventOption(eventType);

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ENotAnswered);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;

	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteLineL(R_DIALOG_INSTALL_EVENT_MSG, displayMode);
	gIoHandler->WriteL(R_APP_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppName(), displayMode);
	gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppVendor(), displayMode);

	switch(eventType)
		{
		case EEventSetProgressBarFinalValue:
			{
			if(EHideProgress != iPreferenceHandler.GetProgressInfoType())
				{
				gIoHandler->SetFinalProgressValueL(aValue);
				}
			break;
			}
		case EEventUpdateProgressBar:
			{
			gIoHandler->WriteLineL(R_PROGRESS_PROMPT_MSG, displayMode);
			break;
			}
		case EEventAbortedInstall:
			{
			gIoHandler->WriteLineL(R_EVENT_INSTALL_ABORTED_MSG, displayMode);
			break;
			}
		case EEventCompletedInstall:
			{
			gIoHandler->WriteLineL(R_EVENT_INSTALL_COMPLETED_MSG, displayMode);
			return ETrue;
			}
		case EEventCompletedUnInstall:
			{
			gIoHandler->WriteLineL(R_EVENT_UNINSTALL_COMPLETED_MSG, displayMode);
			return ETrue;
			}
		case EEventAbortedUnInstall:
			{
			gIoHandler->WriteLineL(R_EVENT_UNINSTALL_ABORTED_MSG, displayMode);
			break;
			}
		case EEventCompletedNoMessage:
			{
			gIoHandler->WriteLineL(R_EVENT_COMPLETED_NO_MESSAGE_MSG, displayMode);
			break;
			}
		case EEventLeave:
			{
			gIoHandler->WriteLineL(R_EVENT_LEAVE_MSG, displayMode);
			break;
			}
		case EEventOcspCheckEnd:
			{
			gIoHandler->WriteLineL(R_EVENT_OCSP_CHECK_END_MSG, displayMode);
			break;
			}
		case EEventDevCert:
			{
			gIoHandler->WriteLineL(R_EVENT_DEVCERT_MSG, displayMode);
			gIoHandler->WriteLineL(aDes, displayMode);
			break;
			}
		default:
			{
			User::Panic(KGeneralPanicString, KSwiInvalidSwitchCase);
			}
		}
	gIoHandler->WriteLineL(R_EVENT_CONFIRM_CANCEL_MSG, displayMode);

	switch(value)
		{
		case EAnswerNo: // Installation shall not be cancelled
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			return ETrue;
		case EAnswerYes: // User wants to cancel the installation
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			return EFalse;
		}

	displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

	// In optimal mode it shall not select any option
	if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_NO_MSG, displayMode);
		return ETrue;
		}
	// In Un-attended mode it shall throw an error
	if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
		if(value == ENotAnswered)
			{
			User::Leave(KMissingMandatoryOption);
			}
		User::Leave(KSwiInvalidConsoleArg);
		}

	gIoHandler->WriteL(KLineBreaker);
	gIoHandler->WriteLineL(R_EVENT_Y_TO_CANCEL_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(R_EVENT_N_TO_CONTINUE_MSG, EIoConsoleType);

	TBuf<KInputCharLen> option;
	TBool retValue = EFalse;
	// In Normal mode get the input from the user. The prompt shall be
	// shown until user gives the correct answer.
	FOREVER
		{
		gIoHandler->ClearLine();
		gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG, EIoConsoleType);
		gIoHandler->ReadStringL(option);

		if(0 == option.CompareF(KAnswerYes))
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			break;
			}

		if(0 == option.CompareF(KAnswerNo))
			{
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			retValue = ETrue;
			break;
			}
		gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
		} //FOREVER
	return retValue;
	}

void CConsoleUiHandler::HandleCancellableInstallEventL(
					const Swi::CAppInfo& 			aAppInfo,
					Swi::TInstallCancellableEvent 	aEvent,
					Swi::MCancelHandler&   			aCancelHandler,
					TInt 							/*aValue*/,
					const TDesC& 					/*aDes*/)
	{
	CPreferences* curPreferences = iPreferenceHandler.GetPreferences(aAppInfo);
	TInt8 value = curPreferences->GetCancellableEventOption(static_cast<TEventType>(aEvent));

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ENotAnswered);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;

	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteLineL(R_DIALOG_CANCELLABLE_EVENT_MSG, displayMode);
	gIoHandler->WriteL(R_APP_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppName(), displayMode);
	gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppVendor(), displayMode);

	switch(aEvent)
		{
		case Swi::EEventRemovingFiles:
			{
			gIoHandler->WriteLineL(R_EVENT_REMOVING_FILES_MSG, displayMode);
			break;
			}
		case Swi::EEventShuttingDownApps:
			{
			gIoHandler->WriteLineL(R_EVENT_SHUTTING_DOWN_APPS_MSG, displayMode);
			break;
			}
		case Swi::EEventCopyingFiles:
			{
			gIoHandler->WriteLineL(R_EVENT_COPYING_FILES_MSG, displayMode);
			break;
			}
		case Swi::EEventOcspCheckStart:
			{
			gIoHandler->WriteLineL(R_EVENT_OCSP_CHECK_START_MSG, displayMode);
			break;
			}
		default:
			{
			User::Panic(KGeneralPanicString, KSwiInvalidSwitchCase);
			}
		}
	gIoHandler->WriteLineL(R_EVENT_CONFIRM_CANCEL_MSG, displayMode);

	switch(value)
		{
		case EAnswerNo: // Event not canceled.
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			return;
		case EAnswerYes: // Event is cancelled by the user
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			aCancelHandler.HandleCancel();
			return ;
		}

	displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

	// In optimal mode it shall not cancel the event
	if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_NO_MSG, displayMode);
		return;
		}
	// In Un-attended mode it shall throw an error
	if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
		if(value == ENotAnswered)
			{
			User::Leave(KMissingMandatoryOption);
			}
		User::Leave(KSwiInvalidConsoleArg);
		}

	gIoHandler->WriteLineL(R_EVENT_Y_TO_CANCEL_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(R_EVENT_N_TO_CONTINUE_MSG, EIoConsoleType);

	TBuf<KInputCharLen> option;
	FOREVER
		{
		gIoHandler->ClearLine();
		gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG, EIoConsoleType);
		gIoHandler->ReadStringL(option);

		if(0 == option.CompareF(KAnswerYes))
			{// Event shall be cancelled
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			aCancelHandler.HandleCancel();
			break;
			}

		if(0 == option.CompareF(KAnswerNo))
			{// No cancellation of the event
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			break;
			}
		gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
		} //FOREVER

	return;
	}

TBool CConsoleUiHandler::DisplaySecurityWarningL(
				const Swi::CAppInfo&						aAppInfo,
				Swi::TSignatureValidationResult				aSigValidationResult,
				RPointerArray<CPKIXValidationResultBase>& 	aPkixResults,
				RPointerArray<Swi::CCertificateInfo>&  	 	aCertificates,
				TBool										aInstallAnyway)
	{
	CPreferences* curPreferences = iPreferenceHandler.GetPreferences(aAppInfo);
	TInt8 value = curPreferences->GetDialogOption(EDisplaySecurityWarning);

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ENotAnswered);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;

	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);

	TInt stringId = R_SIG_VALIDATION_SUCCEEDED_MSG;
	switch(aSigValidationResult)
		{
		case Swi::EValidationSucceeded:
			{
			stringId = R_SIG_VALIDATION_SUCCEEDED_MSG;
			break;
			}
		case Swi::ESignatureSelfSigned:
			{
			stringId = R_SIG_SELF_SIGNED_MSG;
			break;
			}
		case Swi::ENoCertificate:
			{
			stringId = R_SIG_NO_CERTIFICATE_MSG;
			break;
			}
		case Swi::ECertificateValidationError:
			{
			stringId = R_SIG_CERTIFICATE_VALIDATION_ERR_MSG;
			break;
			}
		case Swi::ESignatureNotPresent:
			{
			stringId = R_SIG_NOT_PRESENT_MSG;
			break;
			}
		case Swi::ESignatureCouldNotBeValidated:
			{
			stringId = R_SIG_COULD_NOT_BE_VALIDATED_MSG;
			break;
			}
		case Swi::ENoCodeSigningExtension:
			{
			stringId = R_SIG_NO_CODE_SIGNING_EXT_MSG;
			break;
			}
		case Swi::ENoSupportedPolicyExtension:
			{
			stringId = R_SIG_NO_SUPPORTED_POLICY_EXT_MSG;
			break;
			}
		case Swi::EMandatorySignatureMissing:
			{
			stringId = R_SIG_MISSING_MANDATORY_SIGNATURE_MSG;
			break;
			}
		default:
			{
			User::Panic(KGeneralPanicString, KSwiInvalidSwitchCase);
			}
		}

	TBool isFirstExecution = ETrue;
	TBool retValue = EFalse;
	FOREVER
		{
		gIoHandler->WriteLineL(R_DIALOG_SECURITY_WARNING_MSG, displayMode);
		gIoHandler->WriteL(R_APP_NAME_MSG, displayMode);
		gIoHandler->WriteLineL(aAppInfo.AppName(), displayMode);
		gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, displayMode);
		gIoHandler->WriteLineL(aAppInfo.AppVendor(), displayMode);
		gIoHandler->WriteLineL(stringId, displayMode);

		if(Swi::EValidationSucceeded == aSigValidationResult)
			{
			if((value == ENotAnswered) && ESwiNormalMode == iPreferenceHandler.GetOperationMode())
				{
				gIoHandler->PauseScreenL();
				}
			return ETrue;
			}


		if(!aInstallAnyway)
			{
			gIoHandler->WriteLineL(R_ERR_INST_CANNOT_CONTINUE_MSG);
			if(ESwiNormalMode == iPreferenceHandler.GetOperationMode())
				{
				gIoHandler->PauseScreenL();
				}
			break;
			}

		if(isFirstExecution)
			{
			displayMode = (isVerboseModeEnabled && (value != ENotAnswered))? EIoBoth : EIoLogType;
			gIoHandler->WriteL(KLineBreaker, displayMode);
			DisplayPkixResultsL(aPkixResults, aCertificates, displayMode);

			switch(value)
				{
				case EAnswerNo: // Installation should be cancelled by the user
					gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
					gIoHandler->WriteLineL(R_NO_MSG, displayMode);
					return EFalse;
				case EAnswerYes: // Installation shall continue;
					gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
					gIoHandler->WriteLineL(R_YES_MSG, displayMode);
					return ETrue;
				}

			displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

			// In optimal mode the installation shall continue;
			if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
				{
				gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
				gIoHandler->WriteLineL(R_YES_MSG, displayMode);
				retValue = ETrue;
				break;
				}
			// In Un-attended mode it shall throw an error
			if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
				{
				gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
				gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
				if(value == ENotAnswered)
					{
					User::Leave(KMissingMandatoryOption);
					}
				User::Leave(KSwiInvalidConsoleArg);
				}
			}

		gIoHandler->WriteLineL(R_PRESS_D_FOR_DETAILS_MSG, EIoConsoleType);
		gIoHandler->WriteLineL(R_INSTALL_Y_TO_CONTINUE_MSG, EIoConsoleType);
		gIoHandler->WriteLineL(R_INSTALL_N_TO_ABORT_MSG, EIoConsoleType);

		TBuf<KInputCharLen> option;
		FOREVER
			{
			gIoHandler->ClearLine();
			gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG, EIoConsoleType);
			gIoHandler->ReadStringL(option);
			displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

			if(0 == option.CompareF(KAnswerYes))
				{// Installation should continue
				gIoHandler->WriteL(KLineBreaker, displayMode);
				gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
				gIoHandler->WriteLineL(R_YES_MSG, displayMode);
				return ETrue;
				}

			if(0 == option.CompareF(KAnswerNo))
				{// Installation should be aborted
				gIoHandler->WriteL(KLineBreaker, displayMode);
				gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
				gIoHandler->WriteLineL(R_NO_MSG, displayMode);
				return EFalse;
				}

			if(0 == option.CompareF(KAnswerDisplay) )
				{
				gIoHandler->ClearConsoleL();
				DisplayPkixResultsL(aPkixResults, aCertificates, EIoConsoleType);
				gIoHandler->PauseScreenL();
				gIoHandler->ClearConsoleL();
				break;
				}
			gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
			} // FOREVER -- inner
		displayMode = EIoConsoleType;
		} // FOREVER -- outer

	return retValue;
	}

TBool CConsoleUiHandler::DisplayOcspResultL(
					const Swi::CAppInfo& 					aAppInfo,
					Swi::TRevocationDialogMessage 			aMessage,
					RPointerArray<TOCSPOutcome>& 			aOutcomes,
					RPointerArray<Swi::CCertificateInfo>& 	aCertificates,
					TBool 									aWarningOnly)
	{
	TInt stringId = R_SIG_VALIDATION_SUCCEEDED_MSG;
	switch(aMessage)
		{
		case Swi::EInvalidRevocationServerUrl:
			{
			stringId = R_OCSP_DLG_INVALID_SERVER_URL;
			break;
			}
		case Swi::EUnableToObtainCertificateStatus:
			{
			stringId = R_OCSP_DLG_UNABLE_TO_OBTAIN_STATUS;
			break;
			}
		case Swi::EResponseSignatureValidationFailure:
			{
			stringId = R_OCSP_DLG_VALIDATION_FAILURE;
			break;
			}
		case Swi::EInvalidRevocationServerResponse:
			{
			stringId = R_OCSP_DLG_INVALID_RESPONSE;
			break;
			}
		case Swi::EInvalidCertificateStatusInformation:
			{
			stringId = R_OCSP_DLG_INVALID_STATUS_INFORMATION;
			break;
			}
		case Swi::ECertificateStatusIsUnknown:
			{
			stringId = R_OCSP_DLG_UNKNOWN_STATUS;
			break;
			}
		case Swi::ECertificateStatusIsUnknownSelfSigned:
			{
			stringId = R_OCSP_DLG_UNKNOWN_SELFSIGNED;
			break;
			}
		case Swi::ECertificateStatusIsRevoked:
			{
			stringId = R_OCSP_DLG_REVOKED;
			break;
			}
		default:
			{
			User::Panic(KGeneralPanicString, KSwiInvalidSwitchCase);
			}
		}
	CPreferences* curPreferences = iPreferenceHandler.GetPreferences(aAppInfo);
	TInt8 value = curPreferences->GetDialogOption(EDisplayOCSPError);

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ENotAnswered);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;

	gIoHandler->WriteL(KLineBreaker);
	gIoHandler->WriteL(KLineBreaker);

	TBool isFirstExecution = ETrue;
	TBool retValue = EFalse;
	FOREVER
		{
		gIoHandler->WriteLineL(R_DIALOG_OCSP_RESULT_MSG, displayMode);
		gIoHandler->WriteL(R_APP_NAME_MSG, displayMode);
		gIoHandler->WriteLineL(aAppInfo.AppName(), displayMode);
		gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, displayMode);
		gIoHandler->WriteLineL(aAppInfo.AppVendor(), displayMode);
		gIoHandler->WriteL(R_OCSP_RESULT_MSG, displayMode);
		gIoHandler->WriteLineL(stringId, displayMode);

		if(!aWarningOnly)
			{
			gIoHandler->WriteLineL(R_ERR_INST_CANNOT_CONTINUE_MSG);
			if(ESwiNormalMode == iPreferenceHandler.GetOperationMode())
				{
				gIoHandler->PauseScreenL();
				}
			break;
			}

		if(isFirstExecution)
			{
			displayMode = (isVerboseModeEnabled && (value != ENotAnswered))? EIoBoth : EIoLogType;
			gIoHandler->WriteL(KLineBreaker, displayMode);
			DisplayOcspDetailsL(aOutcomes, aCertificates, displayMode);

			switch(value)
				{
				case EAnswerNo:  // Installation should be cancelled by the user
					gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
					gIoHandler->WriteLineL(R_NO_MSG, displayMode);
					return EFalse;
				case EAnswerYes:// Installation shall continue;
					gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
					gIoHandler->WriteLineL(R_YES_MSG, displayMode);
					return ETrue;
				}
			displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

			// In optimal mode the installation shall continue;
			if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
				{
				gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
				gIoHandler->WriteLineL(R_YES_MSG, displayMode);
				retValue = ETrue;
				break;
				}
			// In Un-attended mode it shall throw an error
			if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
				{
				gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
				gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
				if(value == ENotAnswered)
					{
					User::Leave(KMissingMandatoryOption);
					}
				User::Leave(KSwiInvalidConsoleArg);
				}
			}

		gIoHandler->WriteLineL(R_PRESS_D_FOR_DETAILS_MSG, EIoConsoleType);
		gIoHandler->WriteLineL(R_INSTALL_Y_TO_CONTINUE_MSG, EIoConsoleType);
		gIoHandler->WriteLineL(R_INSTALL_N_TO_ABORT_MSG, EIoConsoleType);

		TBuf<KInputCharLen> option;
		FOREVER
			{
			gIoHandler->ClearLine();
			gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG, EIoConsoleType);
			gIoHandler->ReadStringL(option);
			displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

			if(0 == option.CompareF(KAnswerYes))
				{// Installation should continue
				gIoHandler->WriteL(KLineBreaker, displayMode);
				gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
				gIoHandler->WriteLineL(R_YES_MSG, displayMode);
				return ETrue;
				}

			if(0 == option.CompareF(KAnswerNo))
				{// Installation should be aborted
				gIoHandler->WriteL(KLineBreaker, displayMode);
				gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
				gIoHandler->WriteLineL(R_NO_MSG, displayMode);
				return EFalse;
				}

			if(0 == option.CompareF(KAnswerDisplay) )
				{
				gIoHandler->ClearConsoleL();
				DisplayOcspDetailsL(aOutcomes, aCertificates, EIoConsoleType);
				gIoHandler->PauseScreenL();
				gIoHandler->ClearConsoleL();
				break;
				}
			gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
			} //FOREVER --inner
		displayMode = EIoConsoleType;
		}//FOREVER --outer
		return retValue;
	}

void CConsoleUiHandler::DisplayCannotOverwriteFileL(
					const Swi::CAppInfo&	aAppInfo,
					const Swi::CAppInfo&	aInstalledAppInfo,
					const TDesC&			aFileName)
	{
	gIoHandler->WriteL(KLineBreaker);
	gIoHandler->WriteL(KLineBreaker);
	gIoHandler->WriteLineL(R_DIALOG_CANNOT_OVERWRITE_FILE_MSG);
	gIoHandler->WriteL(R_APP_NAME_MSG);
	gIoHandler->WriteLineL(aAppInfo.AppName());
	gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG);
	gIoHandler->WriteLineL(aAppInfo.AppVendor());
	gIoHandler->WriteLineL(R_CANNOT_OVERWRITE_FILE_MSG);
	gIoHandler->WriteL(R_FILE_NAME_MSG);
	gIoHandler->WriteLineL(aFileName);
	gIoHandler->WriteLineL(R_FILE_BELONGS_TO_OTHER_APP_MSG);
	DisplayAppInfoL(aInstalledAppInfo, EIoBoth);
	if(ESwiNormalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->PauseScreenL();
		}
	return;
	}

TBool CConsoleUiHandler::DisplayMissingDependencyL(
					const Swi::CAppInfo&	aAppInfo,
					const TDesC&			aDependencyName,
					TVersion				aWantedVersionFrom,
					TVersion				aWantedVersionTo,
					TVersion  				aInstalledVersion)
	{
	CPreferences* curPreferences = iPreferenceHandler.GetPreferences(aAppInfo);
	TInt8 value = curPreferences->GetDialogOption(EDisplayMissingDependency);

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ENotAnswered);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;

	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteLineL(R_DIALOG_MISSING_DEPENDECY_MSG, displayMode);
	gIoHandler->WriteL(R_APP_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppName(), displayMode);
	gIoHandler->WriteL(R_PKG_VENDOR_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aAppInfo.AppVendor(), displayMode);
	gIoHandler->WriteLineL(R_MISSING_DEPENDENCY_MSG, displayMode);
	gIoHandler->WriteLineL(R_APP_REQUIRED_FOR_INSTALLATION_MSG, displayMode);
	gIoHandler->WriteL(R_DEPENDENCY_NAME_MSG, displayMode);
	gIoHandler->WriteLineL(aDependencyName, displayMode);
	gIoHandler->WriteLineL(R_WANTED_VERSION_RANGE_MSG, displayMode);
	gIoHandler->WriteL(R_FROM_MSG, displayMode);
	gIoHandler->WriteLineL(aWantedVersionFrom.Name(), displayMode);
	gIoHandler->WriteL(R_TO_MSG, displayMode);
	gIoHandler->WriteLineL(aWantedVersionTo.Name(), displayMode);
	gIoHandler->WriteL(R_INSTALLED_VERSION_MSG, displayMode);
	gIoHandler->WriteLineL(aInstalledVersion.Name(), displayMode);
	gIoHandler->WriteLineL(R_INSTALL_CONTINUE_CONFIRM_MSG, displayMode);

	switch(value)
		{
		case EAnswerNo:  // Installation should be cancelled by the user
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			return EFalse;
		case EAnswerYes:// Installation shall continue;
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			return ETrue;
		}

	displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

	// In optimal mode the installation shall continue;
	if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_YES_MSG, displayMode);
		return ETrue;
		}
	// In Un-attended mode it shall throw an error
	if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
		if(value == ENotAnswered)
			{
			User::Leave(KMissingMandatoryOption);
			}
		User::Leave(KSwiInvalidConsoleArg);
		}

	gIoHandler->WriteLineL(R_INSTALL_Y_TO_CONTINUE_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(R_INSTALL_N_TO_ABORT_MSG, EIoConsoleType);

	TBuf<KInputTwoCharLen> option;
	TBool retValue = EFalse;
	FOREVER
		{
		gIoHandler->ClearLine();
		gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG, EIoConsoleType);
		gIoHandler->ReadStringL(option);

		if(0 == option.CompareF(KAnswerYes))
			{// Installation should continue
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			retValue = ETrue;
			break;
			}

		if(0 == option.CompareF(KAnswerNo))
			{// Installation should be aborted
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			break;
			}
		gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
		}//FOREVER
	return retValue;
	}


	//Members from MUninstallerUiHandler
TBool CConsoleUiHandler::DisplayUninstallL(
					const Swi::CAppInfo& aAppInfo)
	{
	CPreferences* curPreferences = iPreferenceHandler.GetDefaultPreference();
	TInt8 value = curPreferences->GetDialogOption(EDisplayUnInstall);

	TBool isVerboseModeEnabled = iPreferenceHandler.IsVerboseEnabled();
	TBool showDialog = isVerboseModeEnabled || (value == ENotAnswered);

	TIoType displayMode = (showDialog)? EIoBoth : EIoLogType;

	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteL(KLineBreaker, displayMode);
	gIoHandler->WriteLineL(R_DIALOG_UNINSTALL_MSG, displayMode);
	gIoHandler->WriteLineL(R_UNINSTALL_MSG, displayMode);
	DisplayAppInfoL(aAppInfo, displayMode);

	switch(value)
		{
		case EAnswerNo:  // UnInstallation should be cancelled by the user
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			return EFalse;
		case EAnswerYes:// UnInstallation shall continue;
			gIoHandler->WriteL(R_LOG_PRESELECTED_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			return ETrue;
		}

	displayMode = (isVerboseModeEnabled)? EIoBoth : EIoLogType;

	// In optimal mode the un-installation shall continue;
	if(ESwiOptimalMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_YES_MSG, displayMode);
		return ETrue;
		}
	// In Un-attended mode it shall throw an error
	if(ESwiUnAttendedMode == iPreferenceHandler.GetOperationMode())
		{
		gIoHandler->WriteL(R_LOG_RESPONSE_NOT_PRESELECTED_MSG, displayMode);
		gIoHandler->WriteLineL(R_LOG_ABORTING_OPERATION_MSG, displayMode);
		if(value == ENotAnswered)
			{
			User::Leave(KMissingMandatoryOption);
			}
		User::Leave(KSwiInvalidConsoleArg);
		}

	TBuf<KInputCharLen> option;
	TBool retValue = EFalse;
	FOREVER
		{
		gIoHandler->ClearLine();
		gIoHandler->WriteL(R_INSTALL_ENTER_CHOICE_MSG, EIoConsoleType);
		gIoHandler->ReadStringL(option);

		if(0 == option.CompareF(KAnswerYes))
			{// Un-Installation should continue
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_YES_MSG, displayMode);
			retValue = ETrue;
			break;
			}

		if(0 == option.CompareF(KAnswerNo))
			{// Un-Installation should be aborted
			gIoHandler->WriteL(KLineBreaker, displayMode);
			gIoHandler->WriteL(R_LOG_USER_RESPONSE_MSG, displayMode);
			gIoHandler->WriteLineL(R_NO_MSG, displayMode);
			break;
			}
		gIoHandler->WriteErrorMsgL(R_ERR_INVALID_INPUT_MSG);
		}
	return retValue;
	}

TInt CConsoleUiHandler::ProcessOptionList(
							const	RArray<TInt>&	aOptionList,
							RArray<TBool>&	aSelections) const
	{
	TInt optionCount = aOptionList.Count();
	TInt selectionCount = aSelections.Count();
	if(selectionCount < optionCount)
		{
		return KErrGeneral;
		}

	for(TInt i = 0; i < optionCount; ++i)
		{
		TInt option = aOptionList[i] - 1;
		if(option >= selectionCount || option < 0)
			{
			return KErrGeneral;
			}

		aSelections[option] = ETrue;
		}

	return KErrNone;
	}


void CConsoleUiHandler::DisplayAppInfoL(
							const Swi::CAppInfo& aAppInfo,
							TIoType aIoType) const
	{
	gIoHandler->WriteL(R_INSTALL_APPLICATION_NAME, aIoType);
	gIoHandler->WriteLineL(aAppInfo.AppName(), aIoType);
	gIoHandler->WriteL(R_INSTALL_VERSION_NAME, aIoType);
	gIoHandler->WriteLineL(aAppInfo.AppVersion().Name(), aIoType);
	gIoHandler->WriteL(R_INSTALL_VENDOR_NAME, aIoType);
	gIoHandler->WriteLineL(aAppInfo.AppVendor(), aIoType);
	}

void CConsoleUiHandler::DisplayCapbilitiesL(
			const TCapabilitySet& aCapabilitySet,
			TIoType aIoType) const
	{
	for (TInt cap = ECapabilityTCB; cap < ECapability_Limit; ++cap)
		{
		if (aCapabilitySet.HasCapability(static_cast<TCapability>(cap)))
			{
			TBuf<KCapabilityStringLength> capString;
			const char* capName = CapabilityNames[cap];
			while (*capName)
				{
				capString.Append(*capName++);
				}
			gIoHandler->WriteLineL(capString, aIoType);
			}
		}
	}

void CConsoleUiHandler::DisplayLanguageListL(
			const RArray<TLanguage>& aLanguages,
			TIoType aIoType) const
	{
	for(TInt i = 0; i < aLanguages.Count(); ++i)
		{
		gIoHandler->WriteL(langNameResourceId[aLanguages[i]], aIoType);
		gIoHandler->WriteL(_L("\t: "), aIoType);
		gIoHandler->WriteLineL(languageArr[aLanguages[i]], aIoType);
		}
	}

void CConsoleUiHandler::DisplayCertificateListL(
			const RPointerArray<Swi::CCertificateInfo>& aCertificates,
			TIoType aIoType) const
	{
	if(!aCertificates.Count())
		{
		return; // No certificate to display
		}
	gIoHandler->WriteLineL(R_CERT_DETAILS_MSG, aIoType);
	for(TInt i = aCertificates.Count()-1; i >= 0; --i)
		{
		gIoHandler->WriteLineL(KStringLine, aIoType);
		DisplayCertificateL(*aCertificates[i], aIoType);
		}
	gIoHandler->WriteLineL(KStringLine, aIoType);
	}

void CConsoleUiHandler::DisplayCertificateL(
			const Swi::CCertificateInfo& aCertificate,
			TIoType aIoType) const
	{
	gIoHandler->WriteL(R_CERT_SUBJECT_NAME_MSG, aIoType);
	gIoHandler->WriteLineL(aCertificate.SubjectName(), aIoType);
	gIoHandler->WriteL(R_CERT_ISSUER_NAME_MSG, aIoType);
	gIoHandler->WriteLineL(aCertificate.IssuerName(), aIoType);
	gIoHandler->WriteL(R_CERT_VALID_FROM_MSG, aIoType);
	gIoHandler->WriteLineL(aCertificate.ValidFrom(), aIoType);
	gIoHandler->WriteL(R_CERT_VALID_TO_MSG, aIoType);
	gIoHandler->WriteLineL(aCertificate.ValidTo(), aIoType);
	}

void CConsoleUiHandler::DisplayPkixResultsL(
			RPointerArray<CPKIXValidationResultBase>& aPkixResults,
			RPointerArray<Swi::CCertificateInfo>& aCertificate,
			TIoType aIoType) const
	{
	__ASSERT_DEBUG(aPkixResults.Count() == aCertificate.Count(), User::Panic(KGeneralPanicString, KErrArgument));

	if(!aPkixResults.Count())
		{
		return; // No certificate to display
		}

	gIoHandler->WriteLineL(R_CERT_DETAILS_MSG, aIoType);
	for(TInt i = aPkixResults.Count()-1; i >= 0; --i)
		{
		gIoHandler->WriteLineL(KStringLine, aIoType);
		DisplayCertificateL(*aCertificate[i], aIoType);
		const TValidationError error =  aPkixResults[i]->Error().iReason;
		gIoHandler->WriteL(R_VALIDATION_RESULT_MSG, aIoType);
		switch(error)
			{
			case EValidatedOK:
				gIoHandler->WriteLineL(R_VALIDATION_OK_MSG, aIoType);
				break;
			case EChainHasNoRoot:
				gIoHandler->WriteLineL(R_VALIDATION_CHAIN_HAS_NO_ROOT_MSG, aIoType);
				break;
			case ESignatureInvalid:
				gIoHandler->WriteLineL(R_VALIDATION_INVALID_SIGNATURE_MSG, aIoType);
				break;
			case EDateOutOfRange:
				gIoHandler->WriteLineL(R_VALIDATION_DATE_OUT_OF_RANGE_MSG, aIoType);
				break;
			case ENameIsExcluded:
				gIoHandler->WriteLineL(R_VALIDATION_NAME_EXCLUDED_MSG, aIoType);
				break;
			case ENameNotPermitted:
				gIoHandler->WriteLineL(R_VALIDATION_NAME_NOT_PERMITTED_MSG, aIoType);
				break;
			case ENotCACert:
				gIoHandler->WriteLineL(R_VALIDATION_NOT_CA_CERT_MSG, aIoType);
				break;
			case ECertificateRevoked:
				gIoHandler->WriteLineL(R_VALIDATION_CERT_REVOKED_MSG, aIoType);
				break;
			case EUnrecognizedCriticalExtension:
				gIoHandler->WriteLineL(R_VALIDATION_UNRECOGNIZED_EXT_MSG, aIoType);
				break;
			case ENoBasicConstraintInCACert:
				gIoHandler->WriteLineL(R_VALIDATION_NO_CONSTRAINT_IN_CA_CERT_MSG, aIoType);
				break;
			case ENoAcceptablePolicy:
				gIoHandler->WriteLineL(R_VALIDATION_NO_ACCEPTABLE_POLICY_MSG, aIoType);
				break;
			case EPathTooLong:
				gIoHandler->WriteLineL(R_VALIDATION_PATH_TOO_LONG_MSG, aIoType);
				break;
			case ENegativePathLengthSpecified:
				gIoHandler->WriteLineL(R_VALIDATION_NEG_PATH_LEN_MSG, aIoType);
				break;
			case ENamesDontChain:
				gIoHandler->WriteLineL(R_VALIDATION_NAMES_DONT_CHAIN_MSG, aIoType);
				break;
			case ERequiredPolicyNotFound:
				gIoHandler->WriteLineL(R_VALIDATION_POLICY_NOT_FOUND_MSG, aIoType);
				break;
			case EBadKeyUsage:
				gIoHandler->WriteLineL(R_VALIDATION_BAD_KEY_USAGE_MSG, aIoType);
				break;
			case ERootCertNotSelfSigned:
				gIoHandler->WriteLineL(R_VALIDATION_ROOT_NOT_SELFSIGNED_MSG, aIoType);
				break;
			case ECriticalExtendedKeyUsage:
				gIoHandler->WriteLineL(R_VALIDATION_CRITICAL_EXT_KEY_USAGE_MSG, aIoType);
				break;
			case ECriticalCertPoliciesWithQualifiers:
				gIoHandler->WriteLineL(R_VALIDATION_CERT_POLICIES_WITH_QUALIFER_MSG, aIoType);
				break;
			case ECriticalPolicyMapping:
				gIoHandler->WriteLineL(R_VALIDATION_CRITICAL_POLICY_MAPPING_MSG, aIoType);
				break;
			case ECriticalDeviceId:
				gIoHandler->WriteLineL(R_VALIDATION_CRITICAL_DEVICE_ID_MSG, aIoType);
				break;
			case ECriticalSid:
				gIoHandler->WriteLineL(R_VALIDATION_CRITICAL_SID_MSG, aIoType);
				break;
			case ECriticalVid:
				gIoHandler->WriteLineL(R_VALIDATION_CRITICAL_VID_MSG, aIoType);
				break;
			case ECriticalCapabilities:
				gIoHandler->WriteLineL(R_VALIDATION_CRITICAL_CAPABILITIES_MSG, aIoType);
				break;
			default:
				{
				User::Panic(KGeneralPanicString, KSwiInvalidSwitchCase);
				}
			}
		}
	gIoHandler->WriteLineL(KStringLine, aIoType);
	}

void CConsoleUiHandler::DisplayOcspDetailsL(
			RPointerArray<TOCSPOutcome>& aOcspResults,
			RPointerArray<Swi::CCertificateInfo>& aCertificate,
			TIoType aIoType) const
	{
	__ASSERT_DEBUG(aOcspResults.Count() == aCertificate.Count(), User::Panic(KGeneralPanicString, KErrArgument));

	if(!aCertificate.Count())
		{
		return; // No certificate to display
		}

	gIoHandler->WriteLineL(R_CERT_DETAILS_MSG, aIoType);
	for(TInt i = aOcspResults.Count()-1; i >= 0; --i)
		{
		gIoHandler->WriteLineL(KStringLine, aIoType);
		DisplayCertificateL(*aCertificate[i], aIoType);

		gIoHandler->WriteL(R_OCSP_RESULT_MSG, aIoType);
		switch(aOcspResults[i]->iResult)
			{
			case OCSP::EGood:
				gIoHandler->WriteLineL(R_OCSP_RESULT_GOOD_MSG, aIoType);
				break;
			case OCSP::EUnknown:
				gIoHandler->WriteLineL(R_OCSP_RESULT_UNKNOWN_MSG, aIoType);
				break;
			case OCSP::ERevoked:
				gIoHandler->WriteLineL(R_OCSP_RESULT_REVOKED_MSG, aIoType);
				break;
			default:
				{
				User::Panic(KGeneralPanicString, KSwiInvalidSwitchCase);
				}
			}
		gIoHandler->WriteL(R_OCSP_ERROR_MSG, aIoType);

		switch(aOcspResults[i]->iStatus)
			{
			case OCSP::ETransportError:
				gIoHandler->WriteLineL(R_OCSP_TRANSPORT_ERR_MSG, aIoType);
				break;
			case OCSP::EClientInternalError:
				gIoHandler->WriteLineL(R_OCSP_CLIENT_INTERNAL_ERR_MSG, aIoType);
				break;
			case OCSP::ENoServerSpecified:
				gIoHandler->WriteLineL(R_OCSP_NO_SERVER_SPECFIED_MSG, aIoType);
				break;
			case OCSP::EInvalidURI:
				gIoHandler->WriteLineL(R_OCSP_INVALID_URI_MSG, aIoType);
				break;
			case OCSP::EMalformedResponse:
				gIoHandler->WriteLineL(R_OCSP_MALFORMED_RESPONSE_MSG, aIoType);
				break;
			case OCSP::EUnknownResponseType:
				gIoHandler->WriteLineL(R_OCSP_UNKNOWN_RESPONSE_MSG, aIoType);
				break;
			case OCSP::EUnknownCriticalExtension:
				gIoHandler->WriteLineL(R_OCSP_UNKNOWN_CRITICAL_EXT_MSG, aIoType);
				break;
			case OCSP::EMalformedRequest:
				gIoHandler->WriteLineL(R_OCSP_MALFORMED_REQUEST_MSG, aIoType);
				break;
			case OCSP::EServerInternalError:
				gIoHandler->WriteLineL(R_OCSP_SERVER_INTERNAL_ERR_MSG, aIoType);
				break;
			case OCSP::ETryLater:
				gIoHandler->WriteLineL(R_OCSP_SERVER_BUSY_MSG, aIoType);
				break;
			case OCSP::ESignatureRequired:
				gIoHandler->WriteLineL(R_OCSP_SIGNATURE_REQUIRED_MSG, aIoType);
				break;
			case OCSP::EClientUnauthorised:
				gIoHandler->WriteLineL(R_OCSP_CLIENT_UNAUTHORISED_MSG, aIoType);
				break;
			case OCSP::EMissingCertificates:
				gIoHandler->WriteLineL(R_OCSP_MISSING_CERTIFICATES_MSG, aIoType);
				break;
			case OCSP::EResponseSignatureValidationFailure:
				gIoHandler->WriteLineL(R_OCSP_RESPONSE_SIG_VALIDATION_ERR_MSG, aIoType);
				break;
			case OCSP::EThisUpdateTooLate:
				gIoHandler->WriteLineL(R_OCSP_UPDATE_TOO_LATE_MSG, aIoType);
				break;
			case OCSP::EThisUpdateTooEarly:
				gIoHandler->WriteLineL(R_OCSP_UPDATE_TOO_EARLY_MSG, aIoType);
				break;
			case OCSP::ENextUpdateTooEarly:
				gIoHandler->WriteLineL(R_OCSP_NEXT_UPDATE_TOO_EARLY_MSG, aIoType);
				break;
			case OCSP::ECertificateNotValidAtValidationTime:
				gIoHandler->WriteLineL(R_OCSP_INVALID_CERT_AT_VALIDATION_TIME_MSG, aIoType);
				break;
			case OCSP::ENonceMismatch:
				gIoHandler->WriteLineL(R_OCSP_NONCE_MISMATCH_MSG, aIoType);
				break;
			case OCSP::EMissingNonce:
				gIoHandler->WriteLineL(R_OCSP_MISSING_NONCE_MSG, aIoType);
				break;
			case OCSP::EValid:
				gIoHandler->WriteLineL(R_OCSP_VALID_MSG, aIoType);
				break;
			default:
				{
				User::Panic(KGeneralPanicString, KSwiInvalidSwitchCase);
				}
			}
		}
	gIoHandler->WriteLineL(KStringLine, aIoType);
	}

//------------------------ EOF ------------------------------------------
