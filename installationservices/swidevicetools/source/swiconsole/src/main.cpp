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
* Console based SWI.
* @internalComponent
*
*/

#include <e32std.h>
#include <e32base.h>
#include <bacline.h>

// User headers
#include <swi/sisregistryentry.h>	// SIS Registry
#include "cleanuputils.h" 		// CleanupResetAndDestroyPushL
#include "cconsoleuihandler.h"
#include "cswiconsole.h"
#include "ccommandlineparser.h"
#include <swi/sisinstallerrors.h>
#include "swiconsoleerrors.h"

CIoHandler* gIoHandler = NULL;

LOCAL_D void DisplayVersionL()
	{
	gIoHandler->WriteLineL(R_APP_HEADER_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(R_APP_VERSION_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(R_APP_COPYRIGHT_MSG, EIoConsoleType);
	}

LOCAL_D void DisplayUsageL()
	{
	DisplayVersionL();
	gIoHandler->WriteLineL(R_APP_USAGE1_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(R_APP_HELP_OPTION_MSG, EIoConsoleType);
	gIoHandler->WriteLineL(R_APP_VERSION_OPTION_MSG, EIoConsoleType);
	}
	
LOCAL_D void DisplayHelpL()
	{
	gIoHandler->WriteToPageL(KLineBreaker);
	gIoHandler->WriteToPageL(R_HELP_MAIN_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_I_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_U_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_D_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_VER_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_UID_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_PKG_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_VENDOR_OPTION_MSG);
	gIoHandler->WriteToPageL(KLineBreaker);
	gIoHandler->WriteToPageL(R_HELP_OPERATION_MODE_MSG);
	gIoHandler->WriteToPageL(R_HELP_NORMAL_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_OPTIMAL_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_UNATTENDED_OPTION_MSG);
	gIoHandler->WriteToPageL(KLineBreaker);
	gIoHandler->WriteToPageL(R_HELP_APPLICATION_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_L_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_LF_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_V_OPTION_MSG);
	gIoHandler->WriteToPageL(KLineBreaker);
	gIoHandler->WriteToPageL(R_HELP_QUERY_RESONSES_MSG);
	gIoHandler->WriteToPageL(R_HELP_EP_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_EP_SHOW_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_EP_HIDE_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_EP_PROMPT_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_EDEVCERT_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_CER_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_CES_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_CEC_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_CEO_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_IEPF_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_IEPU_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_IEOE_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_IECNM_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_IEDEV_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DI_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DC_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DL_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DD_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DG_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DO_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DW_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DOCSP_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DDEPEND_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DT_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DQI_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DQO_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DU_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DDB_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_DA_OPTION_MSG);
	gIoHandler->WriteToPageL(KLineBreaker);
	gIoHandler->WriteToPageL(R_HELP_EMBEDDED_OPTION_MSG);
	gIoHandler->WriteToPageL(R_HELP_EMB_MSG);
	gIoHandler->WriteToPageL(R_HELP_EMB_PACKAGE_MSG);
	gIoHandler->WriteToPageL(R_HELP_EMB_VENDOR_MSG);
	}

LOCAL_D void HandleInternalErrorL(TInt aError)
	{
	switch(aError)
		{
		case KErrNotFound:
			{
			gIoHandler->WriteLineL(R_ERR_NOT_FOUND);
			break;
			}
		case KErrGeneral:
			{
			gIoHandler->WriteLineL(R_ERR_GENERAL);
			break;
			}
		case KErrCancel:
			{
			gIoHandler->WriteLineL(R_ERR_CANCEL);
			break;
			}
		case KErrNoMemory:
			{
			gIoHandler->WriteLineL(R_ERR_NO_MEMORY);
			break;
			}
		case KErrNotSupported:
			{
			gIoHandler->WriteLineL(R_ERR_NOT_SUPPORTED);
			break;
			}
		case KErrOverflow:
			{
			gIoHandler->WriteLineL(R_ERR_OVERFLOW);
			break;
			}
		case KErrAlreadyExists:
			{
			gIoHandler->WriteLineL(R_ERR_ALREADY_EXISTS);
			break;
			}
		case KErrPathNotFound:
			{
			gIoHandler->WriteLineL(R_ERR_PATH_NOT_FOUND);
			break;
			}
		case KErrInUse:
			{
			gIoHandler->WriteLineL(R_ERR_IN_USE);
			break;
			}
		case KErrServerTerminated:
			{
			gIoHandler->WriteLineL(R_ERR_SERVER_CLOSED);
			break;
			}
		case KErrServerBusy:
			{
			gIoHandler->WriteLineL(R_ERR_SERVER_BUSY);
			break;
			}
		case KErrCompletion:
			{
			gIoHandler->WriteLineL(R_ERR_COMPLETION_ERR);
			break;
			}
		case KErrCorrupt:
			{
			gIoHandler->WriteLineL(R_ERR_CORRUPT);
			break;
			}
		case KErrAccessDenied:
			{
			gIoHandler->WriteLineL(R_ERR_ACCESS_DENIED);
			break;
			}
		case KErrEof:
			{
			gIoHandler->WriteLineL(R_ERR_UNEXPECTED_EOF);
			break;
			}
		case KErrDiskFull:
			{
			gIoHandler->WriteLineL(R_ERR_DISK_FULL);
			break;
			}
		case KErrPermissionDenied:
			{
			gIoHandler->WriteLineL(R_ERR_NO_PERMISSION);
			break;
			}
		case KSwiInvalidConsoleArg:
			{
			gIoHandler->WriteLineL(R_INVALID_ARGUMENTS);
			DisplayUsageL();
			break;
			}
		case KMissingMandatoryOption:
			{
			gIoHandler->WriteLineL(R_ERR_MANDATORY_OPTION_MISSING_MSG);
			DisplayUsageL();
			break;
			}
		case KMissingOptionsValue:
			{
			gIoHandler->WriteLineL(R_ERR_OPTION_VALUE_MISSING_MSG);
			DisplayUsageL();
			break;
			}
		case KInvalidPackageUid:
			{
			gIoHandler->WriteLineL(R_ERR_INVALID_PACKAGE_UID_MSG);
			break;
			}
		case KInvalidDriveName:
			{
			gIoHandler->WriteLineL(R_ERR_INVALID_DRIVE_NAME_MSG);
			break;
			}
		case KInvalidArgument:
			{
			gIoHandler->WriteLineL(R_ERR_INVALID_ARGUMENT_MSG);
			break;
			}
		case KSwiInvalidSwitchCase:
			{
			gIoHandler->WriteLineL(R_ERR_INVALID_SWITCH_CASE_MSG);
			break;
			}
		case KSwiGeneralError:
			{
			gIoHandler->WriteLineL(R_ERR_GENERAL_ERROR_MSG);
			break;
			}
		case KErrSISFieldIdMissing:
		case KErrSISFieldLengthMissing:
		case KErrSISFieldLengthInvalid:
		case KErrSISStringInvalidLength:
		case KErrSISSignedControllerSISControllerMissing:
		case KErrSISControllerSISInfoMissing:
		case KErrSISControllerMissingPrerequisites:
		case KErrSISInfoSISUidMissing:
		case KErrSISInfoSISNamesMissing:
		case KErrSISFieldBufferTooShort:
		case KErrSISStringArrayInvalidElement:
		case KErrSISInfoSISVendorNamesMissing:
		case KErrSISInfoSISVersionMissing:
		case KErrSISControllerSISSupportedLanguagesMissing:
		case KErrSISSupportedLanguagesInvalidElement:
		case KErrSISLanguageInvalidLength:
		case KErrSISContentsSISSignedControllerMissing:
		case KErrSISContentsSISDataMissing:
		case KErrSISDataSISFileDataUnitMissing:
		case KErrSISFileDataUnitTargetMissing:
		case KErrSISFileOptionsMissing:
		case KErrSISFileDataUnitDescriptorMissing:
		case KErrSISFileDataDescriptionMissing:
		case KErrSISContentsMissing:
		case KErrSISEmbeddedControllersMissing:
		case KErrSISEmbeddedDataUnitsMissing:
		case KErrSISControllerOptionsMissing:
		case KErrSISExpressionMissing:
		case KErrSISExpressionStringValueMissing:
		case KErrSISOptionsStringMissing:
		case KErrSISFileOptionsExpressionMissing:
		case KErrSISExpressionHeadValueMissing:
		case KErrSISEmbeddedSISOptionsMissing:
		case KErrSISInfoSISUpgradeRangeMissing:
		case KErrSISDependencyMissingUid:
		case KErrSISDependencyMissingVersion:
		case KErrSISDependencyMissingNames:
		case KErrSISPrerequisitesMissingDependency:
		case KErrSISUpgradeRangeMissingVersion:
		case KErrSISUnexpectedFieldType:
		case KErrSISExpressionUnknownOperator:
		case KErrSISArrayReadError:
		case KErrSISArrayTypeMismatch:
		case KErrSISInvalidStringLength:
		case KErrSISTooDeeplyEmbedded:
			{
			gIoHandler->WriteLineL(R_ERR_CORRUPT_PACKAGE);
			break;
			}
		case KErrSISCompressionNotSupported:
			{
			gIoHandler->WriteLineL(R_ERR_COMPR_ALGO_UNSUPPORTED);
			break;
			}
		case KErrSISInvalidTargetFile:
			{
			gIoHandler->WriteLineL(R_ERR_ILLEGAL_TARGET);
			break;
			}
		case KErrSISWouldOverWrite:
			{
			gIoHandler->WriteLineL(R_ERR_WOULD_OVERWRITE);
			break;
			}
		case KErrSISNotEnoughSpaceToInstall:
			{
			gIoHandler->WriteLineL(R_ERR_NO_DISK_SPACE);
			break;
			}
		case KErrInstallerLeave:
			{
			gIoHandler->WriteLineL(R_ERR_UNEXPECTED_ERR);
			break;
			}
		case KErrPolicyFileCorrupt:
			{
			gIoHandler->WriteLineL(R_ERR_POLICY_CORRUPT);
			break;
			}
		case KErrSignatureSchemeNotSupported:
		case KErrDigestNotSupported:
			{
			gIoHandler->WriteLineL(R_ERR_SIGN_ALGO_UNSUPPORTED);
			break;
			}
		case KErrBadHash:
			{
			gIoHandler->WriteLineL(R_ERR_FILE_CORRUPT);
			break;
			}
		case KErrSecurityError:
			{
			gIoHandler->WriteLineL(R_ERR_SECURITY_ERR);
			break;
			}
		case KErrMissingBasePackage:
			{
			gIoHandler->WriteLineL(R_ERR_MISSING_BASE_PKG);
			break;
			}
		case KErrInvalidUpgrade:
			{
			gIoHandler->WriteLineL(R_ERR_INVALID_UPGRADE);
			break;
			}
		case KErrInvalidEclipsing:
			{
			gIoHandler->WriteLineL(R_ERR_INVALID_ECLIPSING);
			break;
			}
		case KErrWrongHeaderFormat:
			{
			gIoHandler->WriteLineL(R_ERR_WRONG_HEADER);
			break;
			}
		case KErrCapabilitiesMismatch:
			{
			gIoHandler->WriteLineL(R_ERR_INVALID_EXE);
			break;
			}
		case KErrInvalidType:
		case KErrInvalidExpression:
		case KErrExpressionToComplex:
			{
			gIoHandler->WriteLineL(R_ERR_INVALID_INST);
			break;
			}
		default:
			{
			gIoHandler->WriteLineL(R_ERR_UNKNOWN_ERROR);
			}
		}
	}

LOCAL_D void HandleErrorsL(TInt aError)
	{
	gIoHandler->WriteL(R_ERROR_NUMBER_MSG);
	TBuf<KDefaultStringLen> tempString;
	tempString.Format(_L(" %d"), aError);
	gIoHandler->WriteLineL(tempString);

	if(aError <= KSwiInvalidConsoleArg && aError >= KSwiGeneralError)
		{
		gIoHandler->WriteL(R_ERR_INTERNAL_MSG);
		}
	else
		{
		gIoHandler->WriteL(R_ERROR_MSG);
		}
	HandleInternalErrorL(aError);
	}

LOCAL_D void HandleCommandLineOptionsL(TOperationMode& mode)
	{
	CCommandLineParser* cmdLineParser = CCommandLineParser::NewLC();
	
	TOperation operation = cmdLineParser->ParseArgumentsL();
	
	switch(operation)
		{
		case ESwiUsage:
			{
			DisplayUsageL();
			break;
			}
		case ESwiHelp:
			{
			DisplayUsageL();
			DisplayHelpL();
			break;
			}
		case ESwiVersion:
			{
			DisplayVersionL();
			break;
			}
		case ESwiInstall:
			{
			CPreferenceHandler* preferenceHandler = cmdLineParser->GetPreferenceHandlerLC();
			CSwiConsole* consoleSwi = CSwiConsole::NewLC(*preferenceHandler);
			mode = preferenceHandler->GetOperationMode();
			consoleSwi->InstallL();
			CleanupStack::PopAndDestroy(consoleSwi);
			CleanupStack::PopAndDestroy(preferenceHandler);
			break;
			}
		case ESwiUnInstall:
			{
			CPreferenceHandler* preferenceHandler = cmdLineParser->GetPreferenceHandlerLC();
			CSwiConsole* consoleSwi = CSwiConsole::NewLC(*preferenceHandler);
			mode = preferenceHandler->GetOperationMode();
			consoleSwi->UninstallL();
			CleanupStack::PopAndDestroy(consoleSwi);
			CleanupStack::PopAndDestroy(preferenceHandler);	
			break;
			}
		case ESwiList:
			{
			CPreferenceHandler* preferenceHandler = cmdLineParser->GetPreferenceHandlerLC();
			CSwiConsole* consoleSwi = CSwiConsole::NewLC(*preferenceHandler);
			mode = preferenceHandler->GetOperationMode();
			consoleSwi->ListInstalledPackagesL();
			CleanupStack::PopAndDestroy(consoleSwi);
			CleanupStack::PopAndDestroy(preferenceHandler);
			break;
			}
		}
	
	CleanupStack::PopAndDestroy(cmdLineParser);
	}

LOCAL_D void RunSwiConsoleL()
	{
	// Creating the global instance of the IO Handler class
	gIoHandler = CIoHandler::NewLC();
	
	TOperationMode mode = ESwiNormalMode;
	
	TRAPD(err, HandleCommandLineOptionsL(mode));

	if(KErrNone != err)
		{
		// Errors thrown in HandleErrorsL should not
		// shadow the original error.
		TRAP_IGNORE(HandleErrorsL(err));
		}

	if(ESwiNormalMode == mode)
		{
		gIoHandler->PauseScreenL();
		// Normal should not return any error
		err = KErrNone;
		}

	// Deleting the global instance of IO Handler class
	CleanupStack::PopAndDestroy(gIoHandler);
	gIoHandler = NULL;
	User::Leave(err);
	}

//
// Executable's entry point
//
TInt E32Main()
	{
	__UHEAP_MARK;
	
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt err = KErrNoMemory;

	if(NULL != cleanup)
		{
		TRAP(err, RunSwiConsoleL());
		delete cleanup;
		}
	
	__UHEAP_MARKEND;
	return err;
	}



