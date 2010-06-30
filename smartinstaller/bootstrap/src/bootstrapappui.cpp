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
*     CBootstrapAppUi class implementation.
*
*
*/


// INCLUDE FILES
#include <aknglobalnote.h>
#include <stringloader.h>
#include <smartinstaller.rsg>
#include <apgcli.h>
#include <swi/swispubsubdefs.h>

#include "bootstrapapplication.h"
#include "bootstrapappui.h"
#include "bootstrapglobalwaitnoteobserver.h"
#include "bootstrapstatemachine.h"
#include "debug.h"
#include "globals.h"
#include "macros.h"
#include "config.h"
#include "utils.h"

#ifdef USE_LOGFILE
// Logging information
_LIT(KADMLogDir, "ADM");
_LIT(KBootstrapLogFileName, "log-boot.txt" );
_LIT(KBootstrapLogFile, "?:\\logs\\ADM\\log-boot.txt");
#endif

// -----------------------------------------------------------------------------
// CBootstrapAppUi::ConstructL()
// ConstructL of BootstrapApp.
// -----------------------------------------------------------------------------
//
void CBootstrapAppUi::ConstructL()
	{
	TRAPD(err, InitializeSIL());

	if(err != KErrNone)
		{
		LOG2( "Init failed %d", err );
		SetErrorId(EInstallationFailed);
		ExitApp(EBSStateInit);
		}
	else
		{
		iStateMachine = CStateMachine::NewL(*this,iWrapperPackageUid);
		iStateMachine->Start();
		}
	}

// -----------------------------------------------------------------------------
// CBootstrapAppUi::CBootstrapAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CBootstrapAppUi::CBootstrapAppUi():
		iAllowForeground(EFalse)
	{
	LOG ( "SmartInstaller bootstrap begins" );
	// disable possible transition effects
	SetFullScreenApp(EFalse);
	}

// -----------------------------------------------------------------------------
// CBootstrapAppUi::~CBootstrapAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CBootstrapAppUi::~CBootstrapAppUi()
	{
	LOG( "~SmartInstaller()" );

	delete iGlobalWaitNoteObserver;
	delete iGlobalWaitNote;
	delete iStateMachine;

	LOG( "SmartInstaller bootstrap ends" );
	CLOSE_DEBUG_LOG;
	}

// ---------------------------------------------------------------------------
// CBootstrapAppUi::InitializeSIL
// Enter function in EBSStateInit,
// Leaves if Connect failure to downloadmanager,SWI Launcher, Unable to identify ADM Presence
// ---------------------------------------------------------------------------
//
void CBootstrapAppUi::InitializeSIL()
	{
	LOG("+ Init()");

	// Create the log
	INIT_DEBUG_LOG( KBootstrapLogFile(), KADMLogDir, KBootstrapLogFileName );

	// Hide from application task list
	HideApplicationFromFSW();

	// Send application to background
	SendToBackgroundL();

	// Check if Ovi Store client is running
	// Logging purposes only at the moment
	iOviStoreRunning = CUtils::OviStoreRunning();
	if ( iOviStoreRunning )
		{
		LOG( "! Ovi Store running" );
		}

	// Wait for the installer to be free
	WaitForInstallCompleteL();

	// Initialise app UI with standard value.
	BaseConstructL( ENoScreenFurniture /* CAknAppUi::EAknEnableSkin */ );
	StatusPane()->MakeVisible(EFalse);

	LOG("- Init()");
	}

// ---------------------------------------------------------------------------
// CBootstrapAppUi::WaitForInstallCompleteL
// called in EBSStateInit,
// Wait for the wrapper package to be completed. Then fetch the wrapper package UID
// ---------------------------------------------------------------------------
//
void CBootstrapAppUi::WaitForInstallCompleteL()
	{
	LOG( "+ WaitForInstallComplete()" );

	// Use publish and subscribe mechanism to
	// get notified when installer is free.
	RProperty installComplete;
	User::LeaveIfError(installComplete.Attach(KUidSystemCategory, Swi::KUidSoftwareInstallKey));

	TInt value(-1);
	TInt err(-1);
	err = installComplete.Get( KUidSystemCategory, Swi::KUidSoftwareInstallKey, value );
	if ( err != KErrNone)
		{
		LOG( "! Unable to get installer" );
		installComplete.Close();
		User::Leave(err);
		}
	else if((value & Swi::KSwisOperationMask) == Swi::ESwisNone)
		{
		LOG( "Installer is idle" );
		// Installer is idle.
		}
	else
		{
		LOG( "Waiting for installer to become idle" );
		TRequestStatus status;
		installComplete.Subscribe(status);
		User::WaitForRequest( status );
		if ( status.Int() != KErrNone )
			{
			LOG2( "Failed to subscribe: %d", status.Int() );
			installComplete.Close();
			User::Leave( status.Int() );
			}
		}

	// Get the Uid of the latest installed package
	iWrapperPackageUid = GetLatestInstalledL();

	iAllowForeground = ETrue;

	installComplete.Close();

	ShowGlobalWaitNoteL();

	SendToBackgroundL(EFalse);

	LOG( "- WaitForInstallComplete()" );
	}

// ---------------------------------------------------------------------------
// CBootstrapAppUi::GetLatestInstalledL
// Get the latest package installed in the device.
// Called in EBSStateInit on completion of wrapper installation.
// ---------------------------------------------------------------------------
//
TUint32 CBootstrapAppUi::GetLatestInstalledL()
	{
	LOG( "+ GetLatestInstalled()" );

	// Use publish and subscribe mechanism to
	// get Uid of the latest installed package.
	RProperty lastInstall;
	User::LeaveIfError(lastInstall.Attach(KUidSystemCategory, KUidSwiLatestInstallation));

	TInt value = -1;
	User::LeaveIfError(lastInstall.Get( KUidSystemCategory, KUidSwiLatestInstallation, value ));

	LOG2( "- GetLatestInstalled(): 0x%08x", value );
	return value;
	}

// ---------------------------------------------------------------------------
// CBootstrapAppUi::HandleGlobalWaitNoteCancel
// Call OnExitOfState() when the process is cancelled.
// ---------------------------------------------------------------------------
//
void CBootstrapAppUi::HandleGlobalWaitNoteCancel(const TInt& aStatus)
	{
	LOG2( "+ CancelNoteEvent(%d) ", aStatus );

	// Do the cleanup based on the state
	if (aStatus == -1)
		{
		iGlobalWaitNoteId = -1;
		//TODO: Is there a possibility of statemachine not being present here.?
		iStateMachine->HandleWaitNoteCancel();
		}

	LOG( "- CancelNoteEvent() ");
	}

// ---------------------------------------------------------------------------
// CBootstrapAppUi::SendToBackgroundL
// Called to move the app background/foreground.
// ---------------------------------------------------------------------------
//
void CBootstrapAppUi::SendToBackgroundL(TBool aBackground)
	{
	LOG2( "+ SendToBackground(%d)", aBackground );
	// We have gained the focus
	RWsSession aWsSession;
	User::LeaveIfError( aWsSession.Connect() );
	TApaTask task(aWsSession);

	// Initialise the object with the window group id of
	// our application (so that it represent our app)
	task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
	if (aBackground)
		{
		task.SendToBackground();
		}
	else
		{
		task.BringToForeground();
		}
	aWsSession.Close();
	LOG( "- SendToBackground() ");
	}

// ---------------------------------------------------------------------------
// CBootstrapAppUi::ShowGlobalWaitNoteL
// Show the Global wait note. (starts in EBSStateInit)
// ---------------------------------------------------------------------------
//
void CBootstrapAppUi::ShowGlobalWaitNoteL()
	{
	// Allocate TBuf with constant length.
	TBuf<KMaxMsgSize> text( NULL );

	// Reads a resource into a descriptor.
	CEikonEnv::Static()->ReadResource( text, R_ADM_VERIFYING_SW_TEXT );

	// Create new CAknGlobalNote instance.
	iGlobalWaitNote = CAknGlobalNote::NewL();

	iGlobalWaitNoteObserver = new (ELeave) CGlobalWaitNoteObserver(this);
	iGlobalWaitNoteObserver->iStatus = KRequestPending;
	iGlobalWaitNoteObserver->Start();

	iGlobalWaitNoteId = iGlobalWaitNote->ShowNoteL(
					iGlobalWaitNoteObserver->iStatus,
					EAknGlobalWaitNote,
					text );
	}

// ---------------------------------------------------------------------------
// CBootstrapAppUi::StopGlobalWaitNoteL
// Stop the Global wait note, incase of any exit.
// ---------------------------------------------------------------------------
//
void CBootstrapAppUi::StopGlobalWaitNoteL()
	{
	LOG2( "+ StopWaitNote(%d)", iGlobalWaitNoteId );
	if ( iGlobalWaitNote && (iGlobalWaitNoteId >= 0) )
		{
		iGlobalWaitNote->CancelNoteL(iGlobalWaitNoteId);
		}
	iGlobalWaitNoteId = -1;

	DELETE_IF_NONNULL( iGlobalWaitNote         );
	DELETE_IF_NONNULL( iGlobalWaitNoteObserver );
	LOG( "- StopWaitNote()" );
	}

// ---------------------------------------------------------------------------
// CBootstrapAppUi::ShowNoteL
// Show the note on Exit on Error
// ---------------------------------------------------------------------------
//
void CBootstrapAppUi::ShowNoteL( const TAknGlobalNoteType& aType, const TInt& aResourceId )
	{
	// Allocate TBuf with constant length.
	TBuf<KMaxMsgSize> text;

	// Reads a resource into a descriptor.
	CEikonEnv::Static()->ReadResource( text, aResourceId );

	// Create new CAknGlobalNote instance.
	CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
	globalNote->ShowNoteL( aType , text );
	CleanupStack::PopAndDestroy(globalNote);
	}

// ---------------------------------------------------------------------------
// CBootstrapAppUi::HandleForegroundEventL
// On receiving the foreground event, send the app to background.
// ---------------------------------------------------------------------------
//
void CBootstrapAppUi::HandleForegroundEventL(TBool aForeground)
	{
	LOG3("+ HandleForegroundEvent(%d): %d", aForeground, iAllowForeground );
	CAknAppUi::HandleForegroundEventL(aForeground);

	if (aForeground && !iAllowForeground)
		{
		SendToBackgroundL();
		}
	LOG("- HandleForegroundEvent()" );
	}

// ---------------------------------------------------------------------------
// CBootstrapAppUi::ExitApp
// Handle success and failures on all cases
// Display an error notice and exit the application.
// Perform the necessary state specific cleanup.
// ---------------------------------------------------------------------------
//
void CBootstrapAppUi::ExitApp(TInt aAppState)
	{
	LOG3( "+ ExitApp(%d): %d", aAppState, iErrorId );

	TInt resourceId = ENoAppError;

	switch ( iErrorId )
		{
		case ENoAppError:
			break;
		case EInstallationFailed:
			{
			resourceId = R_INSTALLATION_FAILURE_ERR;
			}
			break;
		case EDownloadFailed:
			{
			resourceId = R_HTTP_TRANSACTION_ERR;
			}
			break;
		default:
			LOG2( "Unknown error %d, ignoring", iErrorId );
			break;
		}

	if (resourceId != 0)
		{
		// Display error message
		TRAP_IGNORE( {
			StopGlobalWaitNoteL();
			ShowNoteL(EAknGlobalErrorNote, resourceId);
			} );
		}
	else
		{
		// the final error note is displayed by ADM.
		// make a pause before removing the note to hide the "installation OK"
		// note displayed by S60 installer
		User::After(1500000);
		TRAP_IGNORE( StopGlobalWaitNoteL() );
		}

	// Stop the state machine
	iStateMachine->Stop();

	LOG( "StateMachine stopped" );

	Exit();

	LOG( "- ExitApp()" );
	}
