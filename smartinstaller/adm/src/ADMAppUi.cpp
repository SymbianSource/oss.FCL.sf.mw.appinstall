/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     CADMAppUi implementation
*
*
*/


// INCLUDE FILES
#include <ADM.rsg>
#include <ADM.mbg>
#include <bacline.h>
#include <COEUTILS.H>
#include <mmtsy_names.h>    // Phone information
#include <hal.h>            // Hal::GetData()
#include <PathInfo.h>       // PathInfo::
#include <sysutil.h>        // SysUtil::DiskSpaceBelowCriticalLevel(), GetSWVersion()
#include <stringloader.h>   // StringLoader::LoadLC()
#include <aknquerydialog.h>
#include <aknnotewrappers.h>
#include <avkon.rsg>        // Default dialogs
#include <BAUTILS.H>
#include <rconnmon.h>
#include "ADM.hrh"
#include "ADM.pan"
#include "ADMApplication.h"
#include "ADMAppUi.h"
#include "ADMStateMachine.h"
#include "ADMXmlParser.h"
#include "ADMSisParser.h"
#include "ADMPackageInfo.h"
#include "ADMDownloadHandler.h"
#include "ADMInstallManager.h"
#include "networkstatuslistener.h"

#include "debug.h"
#include "globals.h"
#include "config.h"
#include "macros.h"
#include "utils.h"

#ifdef USE_LOGFILE
_LIT(KADMLogDir, "ADM");
_LIT(KADMLogFileName, "log.txt" );
_LIT(KADMLogFile, "?:\\logs\\ADM\\log.txt");
#endif

/**
 * Download path for dependency packages
 */
_LIT(KADMFolderPath,"?:\\system\\adm\\");

// ============================ MEMBER FUNCTIONS ===============================
void CADMAppUi::ConstructL()
	{
	LOG( "+ CADMAppUi::ConstructL()" );

	// Hide from application task list
	HideApplicationFromFSW();

	// Initialise app UI with standard value.
	BaseConstructL( ENoScreenFurniture /*CAknAppUi::EAknEnableSkin*/);
	StatusPane()->MakeVisible(EFalse);

	// Set download path
	TChar systemDrive;
	RFs::DriveToChar(RFs::GetSystemDrive(), systemDrive);
	HBufC* downloadPath = KADMFolderPath().AllocLC();
	downloadPath->Des()[0] = systemDrive;

	// Create the state machine
	iStateMachine = CStateMachine::NewL(*this, *downloadPath);

	iRfs = iEikonEnv->FsSession();

	iOviStoreRunning = CUtils::OviStoreRunning();

	// Process command line arguments
	ProcessCommandLineArgsL();

	iProgress = CProgressDialog::NewL(*this);

	// If we don't have exit reason set, we can show the progress already
	if (iExitReason == EExitNoError)
		{
		// Show progress bar with little progress already early on
		const TInt waitNoteRes = iResumingInstallation
				? R_ADM_CONTINUING_INST_TEXT
				: R_ADM_VERIFYING_SW_TEXT;
		ShowGlobalProgressL( waitNoteRes, 1, 10 );
		}

	iSwVersion = HBufC::NewL(KSysUtilVersionTextLength);

	// Initialize the telephone server
	User::LeaveIfError( iTelServer.Connect() );
	User::LeaveIfError( iTelServer.LoadPhoneModule( KMmTsyModuleName ) );
	User::LeaveIfError( iPhone.Open( iTelServer, KMmTsyPhoneName ) );

	// Get device information
	User::LeaveIfError( GetPhoneInformation() );

	// Ignore the return code. Default URL will be used if error happens (leaves if no memory)
	ReadConfigurationL();

	ReadStaticResourcesL();

	// Kick the state machine running
	iStateMachine->Start();

	CleanupStack::PopAndDestroy(downloadPath);

	LOG( "- CADMAppUi::ConstructL()" );
	}

// -----------------------------------------------------------------------------
// CADMAppUi::CADMAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CADMAppUi::CADMAppUi() :
		iExitReason(EExitNoError),
		iIAP(KInvalidIapId),
		iIsResumeRequired(ETrue)
	{
	// disable possible transition effects
	SetFullScreenApp(EFalse);
	INIT_DEBUG_LOG( KADMLogFile(), KADMLogDir, KADMLogFileName );
	LOG( "CADMAppUi::CADMAppUi()" );
	// Restore our thread priority to normal, we have been launched
	// with EPriorityBackground
//	RThread().SetPriority(EPriorityNormal);
	}

// -----------------------------------------------------------------------------
// CADMAppUi::~CADMAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CADMAppUi::~CADMAppUi()
	{
	LOG( "CADMAppUi::~CADMAppUi()" );

	// Stop the state machine
	iStateMachine->Stop();

	// Just be make sure that all global stuff is out from the screen
	CancelProgressBar();
	CancelWaitNote();

	iPhone.Close();
	iTelServer.Close();

	DELETE_IF_NONNULL( iQueryMessage            );
	DELETE_IF_NONNULL( iGlobalConfirmationQuery );
	DELETE_IF_NONNULL( iGlobalQueryObserver     );
	delete iAppIconFilename;
	delete iMainAppName;
	delete iConfigUrl;
	delete iSwVersion;
	delete iProgress;
	delete iDepFileName;
	delete iSisFileName;
	delete iStateMachine;

	CLOSE_DEBUG_LOG;
	}

// -----------------------------------------------------------------------------
// Processes the command line parameters and sets state machine initial
// state according to the parameters.
//
// Requires that the state machine has been instantiated (iStateMachine).
// -----------------------------------------------------------------------------
//
void CADMAppUi::ProcessCommandLineArgsL()
	{
	LOG( "+ ProcessCommandLineArgsL()" );
	CCommandLineArguments* args = CCommandLineArguments::NewLC();
	const TInt count = args->Count();

#ifdef FEATURE_INSTALL_RESUME
	// If ADM is launched from the menu, we don't get any command line parameters
	// Need to read the info from the resume info file, if it exists.
	if (count == 1)
		{
		CleanupStack::PopAndDestroy(args);
		const TInt ret = ReadResumeInfoL();
		if (ret != KErrNone)
			{
			ShowGlobalNoteL(EAknGlobalErrorNote, R_ADM_ERR_NO_PENDING_INSTALLATIONS_TEXT);
			ExitApp();
			}
		iResumingInstallation = ETrue;
		return;
		}
#else
	if (count == 1)
		{
		User::Leave( KErrNotSupported );
		}
#endif // FEATURE_INSTALL_RESUME

	// index to the command line args, starting at 1 (0 = executable name)
	TInt index = 1;

#ifdef DO_LOG
	for (int i = 0; i < count; i++)
		{
		LOG3( "Arg[%d]='%S'", i, &(args->Arg(i)) );
		}
#endif

	TPtrC arg = args->Arg(index++);
	TLex lex(arg);
	TUint32 protocolVerRaw = 0;
	TUint32 protocolVer = 0;
	TUint32 tmp;
	if ( lex.Val(protocolVerRaw, EDecimal) == 0)
		{
		LOG3( "(cmd) pVer(raw) = %d (%03x)", protocolVerRaw, protocolVerRaw );
		protocolVer = (protocolVerRaw & KCmdLineBitProtocolVerMask);
		}
	else
		{
		LOG( "Invalid P/BVersion, PANIC" );
		Panic(EPanicAdmCmdLineArgs);
		}

	if (protocolVer < 15)
		{
		// assume it's old version without the bit field support
		LOG2( "Using old args %d", protocolVer );

		// Following are the command line args passed in the old version of Bootstrap:
		// arg 0 - ADM.exe
		// arg 1 - Bootstrap Version
		// arg 2 - WrapperUid
		// arg 3 - IAP
		// arg 4 - Sisfilename
		// arg 5 - Depfilename (optional)

		// Get the Bootstrap version
		iBootstrapVersion = protocolVer;
		LOG2("(cmd) BVersion: %x", iBootstrapVersion);

		// Get the Wrapper uid
		TPtrC uidPtr = args->Arg(index++);
		lex.Assign(uidPtr);
		TUint32 uid = 0;
		if ( lex.Val(uid, EDecimal ) == 0)
			{
			LOG2("(cmd) Wrapper Uid: 0x%x", uid);

			iWrapperPackageUid = TUid::Uid(uid);
			}

		// See if at least the minimum options required are provided
		if (count < 5)
			{
			LOG2( "Incorrect # of arguments (%d) passed from bootstrap, PANIC", count );
			Panic(EPanicAdmCmdLineArgs);
			}

		// Get Iap
		TPtrC iapPtr = args->Arg(index++);
		lex.Assign(iapPtr);
		if ( lex.Val(iIAP, EDecimal) == 0 )
			{
			LOG2("(cmd) IAP: %d", iIAP);
			iStateMachine->SetIAP(iIAP);
			}

		switch (iBootstrapVersion)
			{
			case 1:
			case 2:
				{
				if ( count != 6 )
					{
					LOG2( "Incorrect # of arguments (%d) passed from bootstrap, PANIC", count );
					Panic(EPanicAdmCmdLineArgs);
					}
				iDepFileName = args->Arg(index++).AllocL();
				LOG2( "(cmd) DepFile: '%S'", iDepFileName);

				iSisFileName = args->Arg(index++).AllocL();
				LOG2( "(cmd) AppSis: '%S'", iSisFileName);
				break;
				}
			case 3:
				{
				iDepFileName = args->Arg(index++).AllocL();
				LOG2( "(cmd) DepFile: '%S'", iDepFileName);
				break;
				}
			case 4:
				{
				iSisFileName = args->Arg(index++).AllocL();
				LOG2( "(cmd) AppSis: '%S'", iSisFileName);
				break;
				}
			default:
				LOG2( "Invalid BVersion %d, PANIC", iBootstrapVersion );
				Panic(EPanicAdmCmdLineArgs);
				break;
			}
		}
	else if (protocolVer == 15)
		{
		LOG( "Using new args" );

		// Command line arguments, version 15 specification:
		//
		//31               9  8  7  6  5  4  3  2  1  0
		// +-------------+--+--+--+--+--+--+--+--+--+--+
		// | reserved    |  |  |  |  |  |  |           |
		// +-------------+--+--+--+--+--+--+--+--+--+--+
		//
		//      value
		// bits range description
		//  0-3  0-15 Command line parameter protocol version:
		//             0 = not supported
		//             1 = ADM.exe 1 <wrapper_uid> <iap> <depfile> <sisfile>
		//             2 = ADM.exe 2 <wrapper_uid> <iap> <depfile> <sisfile>
		//             3 = ADM.exe 3 <wrapper_uid> <iap> <depfile>
		//             4 = ADM.exe 4 <wrapper_uid> <iap> <sisfile> (this was used in beta1, beta2)
		//            15 = ADM.exe 15 [parameters as specified by other bits]
		//
		//         since
		//  bit  version type command line parameter present
		//    4      4     N  Bootstrap version number
		//    5      4     N  Wrapper UID
		//    6      4     N  IAP
		//    7      4     N  ADM launch condition code:
		//                    KCmdLineCancelAdm: ADM needs to cancel installation
		//                    KCmdLineLaunchAdm: ADM starts normally
		//    8      4     S  Full path to dependency XML file
		//    9      4     S  Full path to application SIS file
		//
		// Types:
		//  N  Natural decimal number (range 0 - 2^32-1, fits to TUint32)
		//  S  UTF-8 string, must NOT contains spaces or other whitespaces
		//
		// Command line parameters appear in the order of the bits set, i.e.
		// if bits 4, 8, 9 are set the command line parameters are:
		// ADM.exe 784 <bootstrap_version> <dep_file> <sis_file>
		//
		// If command line protocol version is less than 15, it is assumed that
		// old version command line parameter format is used.
		//

		// new version. bit fields define which parameters are present
		// these has to be processed in the right order: from LSB to MSB
		if (protocolVerRaw & KCmdLineBitBootstrapVersion)
			{
			TPtrC arg = args->Arg(index++);
			lex.Assign(arg);
			if ( lex.Val(iBootstrapVersion) == 0 )
				{
				LOG2( "(cmd) BVersion: %x", iBootstrapVersion );
				}
			else
				{
				LOG( "Invalid BVersion" );
				Panic(EPanicAdmCmdInvalidBootstrap);
				}
			}
		if (protocolVerRaw & KCmdLineBitWrapperUid)
			{
			TPtrC arg = args->Arg(index++);
			lex.Assign(arg);
			if ( lex.Val(tmp, EDecimal) == 0 )
				{
				LOG2("(cmd) Wrapper Uid: 0x%x", tmp);
				iWrapperPackageUid = TUid::Uid(tmp);
				}
			else
				{
				LOG( "Invalid wrapper package UID" );
				Panic(EPanicAdmCmdInvalidWrapperUid);
				}
			}
		if (protocolVerRaw & KCmdLineBitIAP)
			{
			TPtrC arg = args->Arg(index++);
			lex.Assign(arg);
			if ( lex.Val(iIAP, EDecimal) == 0 )
				{
				LOG2("(cmd) IAP: %d", iIAP);
				iStateMachine->SetIAP(iIAP);
				}
			else
				{
				LOG( "Invalid IAP" );
				Panic(EPanicAdmCmdInvalidIAP);
				}
			}
		if (protocolVerRaw & KCmdLineBitADMLaunchControl)
			{
			TPtrC arg = args->Arg(index++);
			lex.Assign(arg);
			if ( lex.Val(tmp, EDecimal) == 0 )
				{
				LOG2("(cmd) LState %d", tmp);
				switch (tmp)
					{
				case KCmdLineLaunchAdm:
					// this is valid, nothing needs to be done
					break;
				case KCmdLineCancelAdmNoResume:
					iIsResumeRequired = EFalse;
					// fall-through
				case KCmdLineCancelAdm:
					iStateMachine->SetStartState(CStateFactory::EStatePrepareExitWithError);
					iStateMachine->SetFailureReason(EUserCancelled);
					break;
				default:
					LOG2( "Invalid LState %d, PANIC", tmp );
					Panic(EPanicAdmCmdInvalidLaunchState2);
					break;
					} // switch
				}
			else
				{
				LOG( "Invalid LState, PANIC" );
				Panic(EPanicAdmCmdInvalidLaunchState);
				}
			}
		if (protocolVerRaw & KCmdLineBitFileDep)
			{
			iDepFileName = args->Arg(index++).AllocL();
			LOG2( "(cmd) DepFile: '%S'", iDepFileName);
			}
		if (protocolVerRaw & KCmdLineBitFileSis)
			{
			iSisFileName = args->Arg(index++).AllocL();
			LOG2( "(cmd) SisFile: '%S'", iSisFileName);
			}
		}
	else
		{
		LOG2( "Invalid PVersion %d, PANIC", protocolVer );
		Panic(EPanicAdmCmdInvalidProtocol);
		}
	CleanupStack::PopAndDestroy(args);

	LOG3( "- ProcessCommandLineArgsL(): %d->%d", count, index );
	}

// -----------------------------------------------------------------------------
// Read the information from the resume info file, if launched from the menu.
// -----------------------------------------------------------------------------
//
TInt CADMAppUi::ReadResumeInfoL()
	{
	LOG( "+ ReadResumeInfo()" );
	RArray< TPtrC > lineBuffer;
	TInt err = 0;
	HBufC* buffer = ReadConfigFile( iRfs, KADMResumeInfoFile, lineBuffer, err );

	if ( buffer && err == KErrNone )
		{
		// Read the main application name
		TPtrC namePtr = GetConfigValue( KRFieldAppName, lineBuffer, err );
		if ( err == KErrNone )
			{
			iMainAppName = namePtr.AllocL();
			LOG2( "Resuming '%S'", iMainAppName );
			}

		// Get BootstrapVersion from the resume info file
		TPtrC bootstrapVersionLine = GetConfigValue( KRFieldBootstrap, lineBuffer, err );
		if ( err == KErrNone )
			{
			TLex lex(bootstrapVersionLine);
			if ( lex.Val(iBootstrapVersion) == 0 )
				{
				LOG2( "(resume) Version: %d", iBootstrapVersion );
				}
			else
				{
				// TODO: Error: Can this ever happen?!
				//LOG( "Incorrect version in the resume info" );
				return KErrNotSupported;
				}
			}
		else
			{
			LOG2( "Failed to read version from the resume info (%d)", err );
			return err;
			}
		}
	else
		{
		LOG( "! No resume information, cannot continue" );
		// Resume information file doesn't exist
		return err;
		}

	// Get the Wrapper uid
	TPtrC uidPtr = GetConfigValue( KRFieldWrapperPkgUid, lineBuffer, err );
	if ( err == KErrNone )
		{
		TLex lex(uidPtr);
		TUint32 uid = 0;
		if ( lex.Val(uid, EDecimal ) == 0)
			{
			LOG2( "Wrapper Uid: 0x%x", uid );
			iWrapperPackageUid = TUid::Uid(uid);
			}
		else
			{
			//LOG( "Incorrect wrapper UID in resume info" );
			return KErrNotSupported;
			}
		}
	else
		{
		LOG2( "Failed to read wrapper UID from the resume info (%d)", err );
		return err;
		}

	switch (iBootstrapVersion)
		{
		case 1:
		case 2:
		case 3:
			{
			// Get AppFileName from the resume information file
			TPtrC depFileNameLine = GetConfigValue( KRFieldDepFileName, lineBuffer, err );
			if ( err == KErrNone )
				{
				iDepFileName = depFileNameLine.AllocL();

				LOG2( "(resume) DepFileName = '%S'", iDepFileName );

				if ( !BaflUtils::FileExists(iRfs, *iDepFileName) )
					{
					LOG2( "(resume) DepFile '%S' missing, cannot continue!", iDepFileName );
					return KErrNotFound;
					}
				}
			else
				{
				LOG2( "Failed to read from the resume info (%d)", err );
				return err;
				}
			}
			break;
		case 4:
		default:
			break;
		}

	switch (iBootstrapVersion)
		{
		case 1:
		case 2:
		case 4:
			{
			// Get AppFileName from the resume info file
			TPtrC appFileNameLine = GetConfigValue( KRFieldAppFileName, lineBuffer, err );
			if ( err == KErrNone )
				{
				iSisFileName = appFileNameLine.AllocL();

				LOG2( "(resume) AppSis = '%S'", iSisFileName );

				if ( !BaflUtils::FileExists(iRfs, *iSisFileName) )
					{
					LOG2( "(resume) AppSis '%S' missing, cannot continue!", iSisFileName );
					return KErrNotFound;
					}
				}
			else
				{
				LOG2( "Failed to read from the resume info (%d)", err );
				return err;
				}
			}
			break;
		case 3:
		default:
			break;
		}
	LOG2( "- ReadResumeInfo(): ret=%d", err );
	return err;
	}

// -----------------------------------------------------------------------------
// Handles WindowServer events. Used to catch 'End Call' key.
// -----------------------------------------------------------------------------
//
void CADMAppUi::HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination )
	{
	if ( aEvent.Key()->iCode == EKeyPhoneEnd )
		{
		iStateMachine->HandleUserResponse( EKeyPhoneEnd );
		}
	else
		{
		CEikAppUi::HandleWsEventL( aEvent, aDestination );
		}
	}

// -----------------------------------------------------------------------------
// CADMAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CADMAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			Exit();
			break;

		default:
			Panic(EPanicAdmUi);
			break;
		}
	}

// -----------------------------------------------------------------------------
// Callback CGlobalWaitNote
//
// Called when global wait note was cancelled
// -----------------------------------------------------------------------------
//
void CADMAppUi::WaitNoteCancelled()
	{
	LOG2( "! Wait note cancelled (%d)", iStateMachine->StateIndex() );
	iStateMachine->HandleUserResponse( EAknSoftkeyCancel );
	}

// -----------------------------------------------------------------------------
// Callback CProgressDialog
//
// Called when progress dialog was cancelled
// -----------------------------------------------------------------------------
//
void CADMAppUi::ProgressDialogCancelled()
	{
	LOG2( "! Progress dialog cancelled (%d)", iStateMachine->StateIndex() );
	iStateMachine->HandleUserResponse( EAknSoftkeyCancel );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CADMAppUi::HandleGlobalQueryResponseL(const TInt aResponse)
	{
	LOG2( "+ HandleGlobalQueryResponseL(): %d", aResponse );
	iStateMachine->HandleUserResponse( aResponse );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CADMAppUi::ShowDownloadQueryL(const TInt aResourceId, const TInt aInfoResourceId)
	{
	const TInt KMaxDownloadSizeMsg = 32;

	if ( iGlobalQueryObserver )
		{
		iGlobalQueryObserver->Cancel();
		delete iGlobalQueryObserver;
		iGlobalQueryObserver = NULL;
		}
	iGlobalQueryObserver = CGlobalQueryObserver::NewL(this, EFalse);

	DELETE_IF_NONNULL( iGlobalConfirmationQuery );
	DELETE_IF_NONNULL( iQueryMessage );
	iGlobalConfirmationQuery = CAknGlobalConfirmationQuery::NewL();

	TBuf<KMaxDownloadSizeMsg> dlSizeDescr;
	PrettyPrint( iStateMachine->TotalDownloadSize(), dlSizeDescr );

	CDesCArrayFlat* strings = new (ELeave) CDesCArrayFlat(3);
	CleanupStack::PushL(strings);
	HBufC* msg = StringLoader::LoadLC( R_ADM_SW_UPDATE_REQUIRED_TEXT, iEikonEnv );
	HBufC* info;
	if (aInfoResourceId != -1)
		{
		info = StringLoader::LoadLC( aInfoResourceId, iEikonEnv );
		}
	else
		{
		info = KNullDesC().AllocLC();
		}
	strings->AppendL(*msg);
	strings->AppendL(*info);
	strings->AppendL(dlSizeDescr);
	iQueryMessage = StringLoader::LoadL( aResourceId, *strings, iEikonEnv );
	CleanupStack::PopAndDestroy(3, strings); // strings, msg, info

	// Cancel the progress dialog just before querying the user to reduce flicker
	CancelProgressBar();

	iGlobalConfirmationQuery->ShowConfirmationQueryL(
		iGlobalQueryObserver->iStatus,
		*iQueryMessage,
		0
		);
	iGlobalQueryObserver->Start();
#if 0
	const TInt KMaxDownloadSizeMsg = 32;
	TBuf<KMaxDownloadSizeMsg> dlSizeDescr;
	PrettyPrint( iDepTree->GetTotalDownloadSize(), dlSizeDescr );
	HBufC* string = StringLoader::LoadLC( aResourceId, dlSizeDescr, iEikonEnv );

	CAknQueryDialog* dlg = CAknQueryDialog::NewL();
	const TInt ret = dlg->ExecuteLD( R_AVKON_DIALOG_EMPTY_MENUBAR, *string );

	CleanupStack::PopAndDestroy(); // string

	return ret;
#endif
	}

// -----------------------------------------------------------------------------
// Shows the application launch query dialog
// @param aAppName Application name, caption, to be shown to the user
// @param aIconFilename Full path to the application icon, KNullDesC if none.
// -----------------------------------------------------------------------------
//
void CADMAppUi::ShowLaunchPromptL(const TDesC& aAppName, const TDesC& aIconFilename)
	{
	if ( iGlobalQueryObserver )
		{
		iGlobalQueryObserver->Cancel();
		delete iGlobalQueryObserver;
		iGlobalQueryObserver = NULL;
		}
	iGlobalQueryObserver = CGlobalQueryObserver::NewL(this, EFalse);

	DELETE_IF_NONNULL( iGlobalConfirmationQuery );
	DELETE_IF_NONNULL( iQueryMessage );
	iGlobalConfirmationQuery = CAknGlobalConfirmationQuery::NewL();
	iQueryMessage = StringLoader::LoadL( R_ADM_LAUNCH_APP_TEXT, aAppName, iEikonEnv );
	iGlobalConfirmationQuery->ShowConfirmationQueryL(
		iGlobalQueryObserver->iStatus,
		*iQueryMessage,
		0, // Softkeys
		0, // Animation
		aIconFilename,
		EMbmAdmAdm,      // BitmapId
		EMbmAdmAdm_mask  // MaskId
		);
	iGlobalQueryObserver->Start();
	// Cancel the progress dialog and notes after creating the note to reduce flicker
	CancelProgressBar();
	CancelWaitNote();
	}

// -----------------------------------------------------------------------------
// @param aType Global note type.
// @param aResourceId Resource ID to text to be displayed.
// -----------------------------------------------------------------------------
//
TInt CADMAppUi::ShowGlobalNoteL( const TAknGlobalNoteType aType, const TInt aResourceId )
	{
	// Allocate TBuf with constant length.
	TBuf<KMaxMsgSize> text( NULL );

	// Do we have a valid resource ID for the note?
	if (aResourceId != -1)
		{
		// Reads a resource into a descriptor.
		iEikonEnv->ReadResourceL( text, aResourceId );

		if ( iStateMachine->FailedState() == CStateFactory::EStateLast &&
				iExitReason != EExitNoError)
			{
			const TInt error = iStateMachine->FailedState() * 100 + iExitReason;
			text.Append('\n');
			text.AppendNum( error );
			}

		// Cancel any progress dialog
		CancelProgressBar();
		// Cancel any wait notes
		CancelWaitNote();

		if (aType == EAknGlobalErrorNote )
			{
			CAknQueryDialog* dlg = CAknQueryDialog::NewL( CAknQueryDialog::EErrorTone );
			return dlg->ExecuteLD( R_ADM_EXIT_QUERY, text );
			}
		else
			{
			// Create new CAknGlobalNote instance.
			CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
			globalNote->ShowNoteL( aType, text );
			CleanupStack::PopAndDestroy(globalNote);
			}
		}
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// Shows the exit note with information, how the interrupted/cancelled installation
// can be continued.
// -----------------------------------------------------------------------------
//
TInt CADMAppUi::ShowExitNoteL()
	{
	TInt ret = KErrNone;
	// The order of this table must match TExitReason order
	static const TInt errorNotes[ELastExitState] = {
/* EExitNoError           0 */  -1,
/* EUserCancelled         1 */  -1, //R_ADM_INST_CAN_BE_RESUMED_TEXT,
/* EDownloadFailed        2 */  R_ADM_ERR_DOWNLOAD_FAILED_TEXT,
/* EInsufficientMemory    3 */  R_ADM_ERR_INSTALLATION_NOT_ENOUGH_SPACE_TEXT,
/* EDeviceNotSupported    4 */  R_ADM_ERR_INSTALLATION_UNSUPPORTED_DEVICE_TEXT,
/* EInstallationFailed    5 */  R_ADM_ERR_INSTALLATION_FAILED_TEXT,
/* ERootInstallationFailed  */  -1
	};

	// First, show an error note describing what went wrong
	if (errorNotes[iExitReason] > 0)
		{
		CAknErrorNote* note = new (ELeave) CAknErrorNote( ETrue );
		HBufC* msg = StringLoader::LoadLC( errorNotes[iExitReason], iEikonEnv );
		note->ExecuteLD( *msg );
		CleanupStack::PopAndDestroy(); // msg
		}

	if (iExitReason != EDeviceNotSupported)
		{
		// Then show the "installation can be continued later" information
		// but only if the device is a supported one.
		CAknQueryDialog* dlg = CAknQueryDialog::NewL( CAknQueryDialog::EErrorTone );
		HBufC* oviMsg = NULL;
		if (iOviStoreRunning)
			{
			// TODO: Change ADM so root node would always contain the main application sis information
			HBufC* caption = iStateMachine->LaunchCaption();
			if (!caption)
				{
				caption = KNullDesC().AllocL();
				}
			oviMsg = StringLoader::LoadLC(
					R_ADM_INST_CAN_BE_RESUMED_OVI_TEXT,
					*caption,
					iEikonEnv
					);
			}
		else
			{
			oviMsg = KNullDesC().AllocLC();
			}
		HBufC* mainMsg = StringLoader::LoadLC(
				R_ADM_INST_CAN_BE_RESUMED_TEXT,
				*oviMsg,
				iEikonEnv
				);
		ret = dlg->ExecuteLD( R_ADM_EXIT_QUERY, *mainMsg );
		CleanupStack::PopAndDestroy(2, oviMsg);
		}
	return ret;
	}

// -----------------------------------------------------------------------------
// Creates a wait note displaying given resource ID.
// @param aResourceId Resource ID to be shown
// @param aCancellable ETrue, if wait note is cancellable
// @param aDetailedResourceId Additional resource ID to be shown, -1 if none
// -----------------------------------------------------------------------------
//
void CADMAppUi::ShowWaitNoteL( const TInt aResourceId,
		const TBool aCancellable,
		const TInt aDetailedResourceId
		)
	{
	TBuf<KMaxMsgSize> resourceMsg;
	CancelWaitNote();

	if (aDetailedResourceId != -1)
		{
		iEikonEnv->ReadResourceL( resourceMsg, aDetailedResourceId );
		HBufC* msg = StringLoader::LoadL( aResourceId, resourceMsg, iEikonEnv );
		resourceMsg.Copy( *msg );
		delete msg;
		}
	else
		{
		iEikonEnv->ReadResourceL( resourceMsg, aResourceId );
		}

	iWaitNote = CGlobalWaitNote::NewL(*this, aCancellable );
	iWaitNote->ShowNoteL( resourceMsg );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CADMAppUi::ShowWaitNoteNumL( const TInt aResourceId, const TInt aCurrent, const TInt aLast)
	{
	CancelWaitNote();

	CArrayFix<TInt>* vals = new (ELeave) CArrayFixFlat<TInt>(2);
	CleanupStack::PushL(vals);
	vals->AppendL(aCurrent);
	vals->AppendL(aLast);
	HBufC* msg = StringLoader::LoadLC( aResourceId, *vals, iEikonEnv );

	iWaitNote = CGlobalWaitNote::NewL( *this, ETrue );
	iWaitNote->ShowNoteL( *msg );

	CleanupStack::PopAndDestroy(2, vals);
	}

// -----------------------------------------------------------------------------
// Removes currently shown wait note.
// -----------------------------------------------------------------------------//
//
void CADMAppUi::CancelWaitNote()
	{
	if ( iWaitNote )
		{
		TRAP_IGNORE( iWaitNote->CancelNoteL() );
		delete iWaitNote;
		iWaitNote = NULL;
		}
	}

// -----------------------------------------------------------------------------
// Removes currently shown progress bar.
// -----------------------------------------------------------------------------//
//
void CADMAppUi::CancelProgressBar()
	{
	if ( iProgress )
		{
		iProgress->Cancel();
		}
	}

// -----------------------------------------------------------------------------
// ShowGlobalProgressL()
//
// aResourceId  Resource ID of the text to be shown on the progress bar
// aStartValue  Progress bar start value
// aEndValue    Progress bar end value
// aCurrent     Resource string value0
// aLast        Resource string value1
// -----------------------------------------------------------------------------
//
void CADMAppUi::ShowGlobalProgressL(const TInt aResourceId,
		const TInt aStartValue, const TInt aEndValue,
		const TInt aCurrent, const TInt aLast)
	{
	HBufC *msg = NULL;
	CArrayFix<TInt>* vals = new (ELeave) CArrayFixFlat<TInt>(2);
	CleanupStack::PushL(vals);

	if (aCurrent != -1)
		{
		vals->AppendL(aCurrent);
		vals->AppendL(aLast);

		// Read the resource
		msg = StringLoader::LoadLC( aResourceId, *vals, iEikonEnv );
		}
	else
		{
		// Read the resource
		msg = iEikonEnv->AllocReadResourceLC( aResourceId );

		// Special case: append the application name to the string
		if (aResourceId == R_ADM_CONTINUING_INST_TEXT)
			{
			if (iMainAppName && iMainAppName->Length() > 0)
				{
				HBufC* newmsg = HBufC::NewL(msg->Length() + iMainAppName->Length() + 3);
				newmsg->Des().Copy(*msg);
				newmsg->Des().Append('\n');
				newmsg->Des().Append('\'');
				newmsg->Des().Append(*iMainAppName);
				newmsg->Des().Append('\'');
				CleanupStack::PopAndDestroy(msg);
				msg = newmsg;
				CleanupStack::PushL(msg);
				}
			}
		}

	iProgress->Cancel();
	iProgress->StartL( *msg, aEndValue );
	iProgress->Update(aStartValue, aEndValue);

	CleanupStack::PopAndDestroy(2, vals); // msg, vals
	}

// -----------------------------------------------------------------------------
// Shows a proper wait note before the cleanup starts.
//
// This function MUST NOT LEAVE.
// -----------------------------------------------------------------------------
//
void CADMAppUi::HandleFailure()
	{
	LOG3( "+ CADMAppUi::HandleFailure(): %d. err=%d", iStateMachine->StateIndex(), iExitReason);
	// Disable wait note and progress bar, if any
	CancelProgressBar();
	CancelWaitNote();

	// Delete the dep file
	if ( iDepFileName )
		{
		// We need to ignore, if delete leaves as we are currently already
		// handling an error case.
		// TODO: Remove if(), DeleteFile() copes with NULL filenames.
		DeleteFile( *iDepFileName );
		// TODO: Should we 'delete iDepFileName;' and set it to NULL?
		}
	TInt err = KErrNone;
	TInt waitNoteResource;
	TInt detailedInfoResource = -1;

	switch (iExitReason)
		{
		case EDownloadFailed:
			waitNoteResource = R_ADM_DETAILED_CANCELLING_INSTALLATION_WAIT_TEXT;
			detailedInfoResource = R_ADM_ERR_CONN_LOST_TEXT;
			break;
		case EUserCancelled:
		case EInstallationFailed:
			waitNoteResource = R_ADM_CANCELLING_INSTALLATION_WAIT_TEXT;
			break;
		default:
			waitNoteResource = R_ADM_REVERTING_CHANGES_WAIT_TEXT;
			break;
		}

	// Do the cleanup depending on the state that experienced failure
	if ( iStateMachine->FailedState() >= CStateFactory::EStateDownloadChangesFile )
		{
		TRAP(err, ShowWaitNoteL( waitNoteResource, EFalse, detailedInfoResource ));
		}

	LOG2( "- CADMAppUi::HandleFailure(): %d", err );
	}

// -----------------------------------------------------------------------------
// Shows the final error message and exits application
// -----------------------------------------------------------------------------
//
void CADMAppUi::ExitApp()
	{
	LOG3( "+ CADMAppUi::ExitApp(): %d, %d", iExitReason, iAppLaunch );

	// Remove any progress bar
	CancelProgressBar();
	// Remove any wait note
	CancelWaitNote();

	// Just checking that iExitReason is in the proper range
	if ( iExitReason >= 0 && iExitReason < ELastExitState )
		{
		// If we have an additional note to display, show it. We need to
		// trap any leaves.
		TRAP_IGNORE(
				{
				if ( iSilentInstallationOk && iExitReason == EExitNoError )
					{
					// If we used non-silent installation, the "Installation complete" note
					// was already shown by S60 Installer. Don't show it here again.
					// If the user has launched the application, don't show the note
					if (!iNonSilentInstallation && !iAppLaunch)
						{
						ShowGlobalNoteL( EAknGlobalConfirmationNote, R_ADM_INSTALLATION_OK_TEXT );
						// We need to wait 350ms to show the note as the call to Exit() below would kill
						// the AppUi and also the note.
						User::After(350000);
						}
					}
				else
					{
					if (!iNonSilentInstallation)
						{
						// Show ADM again in task list. This is because we're using
						// CAknQueryDialog: we don't want that to get hidden.
						HideApplicationFromFSW(EFalse);
						// This call returns after user has dismissed the error dialog
						ShowExitNoteL();
						}
					}
				}
			);
		}
#ifdef _DEBUG
	else
		{
		LOG2( "Invalid exit reason %d", iExitReason );
		User::Invariant();
		}
#endif

	// Set the ADM icon hidden, if everything went smoothly or any resume info file exists.
	const TBool hideAdm = !( (iExitReason != EExitNoError || iExitReason == EDeviceNotSupported) && iIsResumeRequired );
	const TInt err = CUtils::HideApplicationFromMenu( KUidADMApp.iUid, hideAdm );
	LOG3( "ADM icon hidden: %d (%d)", hideAdm, err );

	RunAppShutter();
	Exit();

	LOG( "- CADMAppUi::ExitApp()" );
	}

// -----------------------------------------------------------------------------
// Delete a file pointed by aFilename.
// -----------------------------------------------------------------------------
//
void CADMAppUi::DeleteFile(const TDesC& aFilename)
	{
	iRfs.Delete(aFilename);
	}

// -----------------------------------------------------------------------------
// Pretty prints number as text string using KiB, MiB and GiB prefixes when needed.
//
// aSize    Number to pretty print. Only positive integers supported.
// aDescr   Descriptor holding the pretty printed number string.
// -----------------------------------------------------------------------------
//
void CADMAppUi::PrettyPrint(const TUint32 aSize, TDes& aDescr)
	{
	_LIT(KFmtDecimals,  "%02d");
	TLocale locale;
	TUint32 size = aSize, rem;
	TPtrC sizePrefix( iPrefixKb );

	// We don't pretty print bytes because "bytes" would a new string
	// to localize. This is just to minimize that work.
	if (aSize < 1024*1024)
		{
		size /= 1024;
		rem = (aSize * 100 / 1024) % 100;
		}
	else if (aSize < 1024*1024*1024)
		{
		size /= 1024*1024;
		rem = (aSize * 100 / (1024*1024)) % 100;
		sizePrefix.Set( iPrefixMb );
		}
	else
		{
		// TInt64 is needed, because aSize*100 will overflow on big values of aSize
		TInt64 remBig( aSize );
		remBig = ((remBig * 100) / (1024*1024*1024)) % 100;
		rem = remBig;
		size /= 1024*1024*1024;
		sizePrefix.Set( iPrefixGb );
		}

	aDescr.Num(size);
	// Add decimals only if they != .00
	if (rem)
		{
		aDescr.Append(locale.DecimalSeparator());
		aDescr.AppendFormat(KFmtDecimals, rem);
		}
//    if (aSize > 999)
//        aDescr.Insert(aDescr.Length() - 6, iLocale.ThousandsSeparator());
	aDescr.Append(sizePrefix);
}

// -----------------------------------------------------------------------------
// Gets detailed phone information and stores it internally.
//
// Returns KErrNone, if fetching of machine UID was succesful
// -----------------------------------------------------------------------------
//
TInt CADMAppUi::GetPhoneInformation()
	{
	LOG( "+ GetPhoneInformation()" );

	//TODO: Remove unnecessary information, currently MachineUid is only needed
	TInt r = HAL::Get(HALData::EMachineUid, iMachineUid);
#if _DEBUG
	HAL::Get(HALData::EManufacturer, iManufacturer);
	HAL::Get(HALData::EManufacturerHardwareRev, iHardwareRev);
	HAL::Get(HALData::EManufacturerSoftwareRev, iSoftwareRev);
	HAL::Get(HALData::EModel, iModel);
	HAL::Get(HALData::EDeviceFamily, iDeviceFamily);
	HAL::Get(HALData::EDeviceFamilyRev, iDeviceFamilyRev);
	HAL::Get(HALData::ECPUArch, iCpuArch);
	HAL::Get(HALData::ECPUABI, iCpuABI );
	HAL::Get(HALData::ECPUSpeed, iCpuSpeed );
#endif
	TPtr swVer(iSwVersion->Des());
	SysUtil::GetSWVersion(swVer);

	LOG8_2( "  machine uid = 0x%08x",       iMachineUid);
#if _DEBUG
	LOG8_2( "  model = 0x%08x",             iModel);
	LOG8_2(	"  manufacturer	= %d",			iManufacturer);
	LOG8_2(	"  hardware	rev	= %d",			iHardwareRev);
	LOG8_2(	"  software	rev	= %d",			iSoftwareRev);
	LOG8_2(	"  device family = %d",			iDeviceFamily);
	LOG8_2(	"  device family rev = %d",		iDeviceFamilyRev);
	LOG8_2(	"  CPU architecture	= 0x%08x",	iCpuArch);
	LOG8_2(	"  CPU ABI = %d",				iCpuABI);
	LOG8_2(	"  CPU speed = %d",				iCpuSpeed);
#endif
	LOG2("	Sw Ver = %S",					iSwVersion);

	LOG8_2( "- GetPhoneInformation (%d)", r);

	return r;
	}

// -----------------------------------------------------------------------------
// Returns true if the phone is roaming
// -----------------------------------------------------------------------------
//
TBool CADMAppUi::RegisteredRoaming()
	{
	LOG( "+ RegisteredRoaming()" );
	// Get registeration status
	TRequestStatus status;
	TInt ret = EFalse; // Default to not roaming

	RMobilePhone::TMobilePhoneRegistrationStatus regStatus(
		RMobilePhone::ERegistrationUnknown );
	iPhone.GetNetworkRegistrationStatus( status, regStatus );
	User::WaitForRequest( status );

	if ( RMobilePhone::ERegisteredRoaming == regStatus )
		{
		ret = ETrue;
		}

	LOG2( "- RegisteredRoaming: %d", ret );

	return ret;
	}
#if 0
// ---------------------------------------------------------------------------
// CADMAppUi::PhoneMemoryDrive
//
// @return Drive number for phone memory.
// ---------------------------------------------------------------------------
//
TInt CADMAppUi::PhoneMemoryDrive()
	{
	TInt res = 0;
	TInt err = RFs::CharToDrive( PathInfo::PhoneMemoryRootPath()[0], res );
	__ASSERT_ALWAYS( !err, Panic( EPanicAdmFileSystemAccess ) );

	return res;
	}

// ---------------------------------------------------------------------------
// CADMAppUi::MemoryCardDrive
//
// @return Drive number for memory card.
// ---------------------------------------------------------------------------
//
TInt CADMAppUi::MemoryCardDrive()
	{
	TInt res = 0;
	TInt err = RFs::CharToDrive( PathInfo::MemoryCardRootPath()[0], res );
	__ASSERT_ALWAYS( !err, Panic( EPanicAdmFileSystemAccess ) );

	return res;
	}
#endif
// ---------------------------------------------------------------------------
// CADMAppUi::ReadStaticResourcesL
//
// @return KErrNone if configuration was read correctly, error code otherwise
// ---------------------------------------------------------------------------
//
void CADMAppUi::ReadStaticResourcesL()
	{
	iEikonEnv->ReadResourceL( iPrefixKb, R_KB );
	iEikonEnv->ReadResourceL( iPrefixMb, R_MB );
	iEikonEnv->ReadResourceL( iPrefixGb, R_GB );
	}

// ---------------------------------------------------------------------------
// CADMAppUi::UsingWLAN
//
// @return ETrue, if selected bearer is WLAN
// ---------------------------------------------------------------------------
//
TBool CADMAppUi::UsingWLAN()
	{
	return BearerType() == EBearerWLAN;
	}

// ---------------------------------------------------------------------------
// CADMAppUi::CheckBearerTypeL
//
// @return Bearer type
// ---------------------------------------------------------------------------
//
TInt CADMAppUi::BearerType()
	{
	LOG2( "+ BearerType(): IAP=%d", iIAP );
	RConnectionMonitor monitor;
	TRequestStatus status = KRequestPending;
	TUint iapId(0);
	TInt bearer( 0 );
	TUint count;
	TUint ids[ 15 ];

	TRAPD( err, monitor.ConnectL() );
	if (err != KErrNone )
		{
		LOG2( "BearerType: Connect to RConnMon failed. %d", err );
		return err;
		}

	// Get the connection count
	monitor.GetConnectionCount( count, status );
	User::WaitForRequest( status );
	if ( status.Int() != KErrNone )
		{
		LOG2( "GetConnCount failed. %d", status.Int() );
		monitor.Close();
		return status.Int();
		}
#ifdef DO_LOG
	else
		{
		LOG2( "BearerType: %d connections:", count);
		}
#endif

	// Get the connection info for each connection
	TUint numSubConnections;
	TInt i;

	for ( i = 1; i <= count; i++ )
		{
		TInt ret = monitor.GetConnectionInfo( i, ids[ i-1 ], numSubConnections );
		if ( ret == KErrNone )
			{
			LOG3( "BearerType: ConnID[%d] = %d" , i, ids[ i-1 ] );
			// Get the IAP Id of the connection
			monitor.GetUintAttribute( ids[ i-1 ], 0, KIAPId, iapId, status );
			User::WaitForRequest( status );
			if ( status.Int() != KErrNone )
				{
				LOG2( "GetIapId failed. %d", status.Int() );
				// try next connection
				continue;
				}
#ifdef DO_LOG
			else
				{
				LOG3( "BearerType: IapId = %d, IAP = %d", iapId, iIAP );
				}
#endif
			// If the IAP matches with the one set in ADM
			// iIAP is initialized to KInvalidIapId
			if ( iapId == iIAP || iIAP == KInvalidIapId )
				{
#ifdef DO_LOG
				LOG2( "BearerType: Getting bearer for IapId %d", iapId );
#endif
				// Get the Bearer of the IAP Id.
				monitor.GetIntAttribute( ids[ i-1 ], 0, KBearer, bearer, status );
				User::WaitForRequest( status );
				if ( status.Int() != KErrNone )
					{
					LOG2( "BearerType: GetBearerType failed. %d", status.Int() );
					// try next connection
					continue;
					}
#ifdef DO_LOG
				else
					{
					LOG3( "BearerType: IapId %d: bearer = %d", iapId, bearer );
					}
#endif
				// we found matching IAP ID, break out of the loop
				break;
				}
			}
		else
			{
			LOG2( "GetConnInfo failed. %d", ret );
			return ret;
			}
		}
	TBuf<KConnMonMaxStringAttributeLength> iapName;

	// Get the string attrib of the IAP Id
	monitor.GetStringAttribute( ids[ i-1 ], 0, KIAPName, iapName, status );
	User::WaitForRequest( status );
	if ( status.Int() != KErrNone )
		{
		LOG2( "BearerType: GetStrAttr failed. %d", status.Int() );
		}
	else
		{
		LOG5( "* Selected IAP %d (%d), conn %d '%S'", iapId, bearer, ids[i-1], &iapName );
		}

	// Close the RConnMon object
	monitor.Close();

	LOG2( "- BearerType(): %d", bearer );

	return bearer;
	}

// ---------------------------------------------------------------------------
// CADMAppUi::ReadConfiguration
//
// @return KErrNone if configuration was read correctly, error code otherwise
// ---------------------------------------------------------------------------
//
TInt CADMAppUi::ReadConfigurationL()
	{
	TFileName configFile;
	RArray< TPtrC > lineBuffer;
	TInt ret( KErrNone );

	// Find the configuration file from the private directory
	ret = iRfs.PrivatePath(configFile);
	if (ret == KErrNone)
		{
		TBuf<2> appDrive;
		// Insert the drive of the running application
		appDrive.Copy(Application()->AppFullName().Left(2));
		configFile.Insert(0, appDrive);
		// Append the configuration file name
		configFile.Append(KConfigFile);

		LOG2( "configFile = %S", &configFile );

		HBufC* buffer = ReadConfigFile(iRfs, configFile, lineBuffer, ret );

		if ( buffer && ret == KErrNone )
			{
			TPtrC line = GetConfigValue( KCfgTagUrl, lineBuffer, ret );
			if ( ret == KErrNone )
				{
				iConfigUrl = HBufC8::NewL(line.Length());
				iConfigUrl->Des().Copy(line);
				LOG8_2( "configUrl = %S", iConfigUrl );
				}
			delete buffer;
			}
		}
		if ( !iConfigUrl )
			{
			// Error occured while reading the configuration, use default URL
			iConfigUrl = HBufC8::NewL(KDefaultDepServerUrl().Length());
			*iConfigUrl = KDefaultDepServerUrl;
			LOG8_2( "configUrl = %S (default)", iConfigUrl );
			}

	lineBuffer.Close();

	return ret;
	}

// End of File
