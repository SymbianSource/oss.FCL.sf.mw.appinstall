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
*     State machine implementation
*
*
*/


#include <ADM.rsg>
#include "ADMApplication.h" // KUidADMApp
#include "ADMStateMachine.h"
#include "ADMStateMachineDebug.h"
#include "globals.h"
#include "config.h"

#include <e32std.h>
#include <sysutil.h>        // SysUtil::DiskSpaceBelowCriticalLevel()
#include <BAUTILS.H>
#include <apgcli.h>

// Use Char 0xFF to indicate no drive is selected
const TChar KNoDriveSelected = 0xFF;

// Used to force the progress bar to finish
const TInt KProgressBarFullValue = 100;

TInt CompareVersions(TVersion& version1, TVersion& version2);

// ---------------------------------------------------------------------------
// Panic
//
// Raises a panic with specific panic code.
// ---------------------------------------------------------------------------
//
LOCAL_C inline void Panic(TInt aPanicCode)
	{
	_LIT(KPanic, "SmartInst-SM");
	User::Panic( KPanic, aPanicCode );
	}

LOCAL_C inline void Panic(TInt aPanicCode, TInt aState)
	{
	_LIT(KPanic, "SmartInst-SM");
	const TInt reason = aState*100 + aPanicCode;
	User::Panic( KPanic, reason );
	}

// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
CStateMachine::CStateMachine(CADMAppUi& aAppUi) :
	CActive(EPriorityNormal),
	iAppUi(aAppUi),
	iFailedState( CStateFactory::EStateLast )
	{
	CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------------
// NewL()
// -----------------------------------------------------------------------------
//
CStateMachine* CStateMachine::NewL(CADMAppUi& aAppUi, const TDesC& aDownloadPath)
	{
	CStateMachine* self = new (ELeave) CStateMachine(aAppUi);
	CleanupStack::PushL(self);
	self->ConstructL(aDownloadPath);
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// ConstructL()
// -----------------------------------------------------------------------------
//
void CStateMachine::ConstructL(const TDesC& aDownloadPath)
	{
	// Create a local semaphore for handling the download deleting
	// This must be the first to be created and last to be destroyed.
	iDlDeletingSemaphore.CreateLocal(0);

	iXmlParser = CXmlParser::NewL();
	iSisParser = CSisParser::NewL();

#ifdef USE_LOGFILE
	iInstallManager = CInstallManager::NewL(this, iAppUi.iLog);
	iDepTree = CDepTree::NewL(iAppUi.iLog, aDownloadPath);
	iDownloadHandler = CDownloadHandler::NewL(this, iAppUi.iLog, !(iAppUi.EikonEnv()->StartedAsServerApp()));
#else
	iInstallManager = CInstallManager::NewL(this);
	iDepTree = CDepTree::NewL(aDownloadPath);
	iDownloadHandler = CDownloadHandler::NewL(this, !(iAppUi.EikonEnv()->StartedAsServerApp()));
#endif

	iStateFactory = CStateFactory::NewL(*this);
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CStateMachine::~CStateMachine()
	{
	if (IsActive())
		{
		Cancel();
		}
	delete iStateFactory;
	delete iDownloadHandler;
	delete iInstallManager;
	delete iDepTree;
	delete iSisParser;
	delete iXmlParser;
	delete iDownloadedFile;

	iDlDeletingSemaphore.Close();
	}

// -----------------------------------------------------------------------------
// Updates the displayed progress bar
// -----------------------------------------------------------------------------
//
void CStateMachine::UpdateProgressBar(TInt aStartValue, TInt aEndValue)
	{
	iAppUi.iProgress->Update(aStartValue, aEndValue);
	}

// -----------------------------------------------------------------------------
// Removes the displayed progress bar
// -----------------------------------------------------------------------------
//
void CStateMachine::StopProgressBar() const
	{
	iAppUi.iProgress->Cancel();
	}

// -----------------------------------------------------------------------------
// Sets the failure state and reason for exit
// -----------------------------------------------------------------------------
//
void CStateMachine::SetFailureReason(TExitReason aReason)
	{
	iAppUi.iExitReason = aReason;
	iFailedState = (CStateFactory::TAppState)StateIndex();
	// If the device is not supported, don't create resume info
	if (aReason == EDeviceNotSupported)
		{
		iAppUi.iIsResumeRequired = EFalse;
		}
	}

// -----------------------------------------------------------------------------
// Shows the download query dialog with a specified text.
// @param aResourceId Resource ID for the text to be shown.
// @param aInfoResourceId Resource ID for additional information text.
// -----------------------------------------------------------------------------
//
void CStateMachine::ShowDownloadQueryL(const TInt aResourceId, const TInt aInfoResourceId)
	{
	iAppUi.ShowDownloadQueryL( aResourceId, aInfoResourceId );
	}

// -----------------------------------------------------------------------------
// Shows wait note
// -----------------------------------------------------------------------------
//
void CStateMachine::ShowWaitNoteL(const TInt aResourceId)
	{
	iAppUi.ShowWaitNoteL( aResourceId, ETrue );
	}

// -----------------------------------------------------------------------------
// Shows wait note with the specified values
// -----------------------------------------------------------------------------
//
void CStateMachine::ShowInstallingWaitNoteL(const TInt aCurrent, const TInt aLast)
	{
	iAppUi.ShowWaitNoteNumL( R_ADM_INSTALLING_TEXT, aCurrent, aLast );
	}

// -----------------------------------------------------------------------------
// Shows the progress bar with the specified values
// -----------------------------------------------------------------------------
//
void CStateMachine::ShowProgressBarL(const TInt aStartValue, const TInt aEndValue, const TInt aCurrent, const TInt aLast)
	{
	iAppUi.ShowGlobalProgressL( R_ADM_DOWNLOADING_TEXT,
			aStartValue, aEndValue,
			aCurrent, aLast );
	}

// -----------------------------------------------------------------------------
// Stops the wait note
// -----------------------------------------------------------------------------
//
void CStateMachine::StopWaitNote()
	{
	iAppUi.CancelWaitNote();
	}

// -----------------------------------------------------------------------------
// Starts the state machine from the initial state.
// -----------------------------------------------------------------------------
//
void CStateMachine::Start()
	{
	LOG( "+ CStateMachine::Start()" );

	// If the initial state has been set, don't change it here
	if (!iState)
		{
		iState = &iStateFactory->GetState( CStateFactory::EStateSetup );
		}

	iStatus = KRequestPending;
	SetActive();
	// send signal that this request has completed
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

// -----------------------------------------------------------------------------
// Stops the state machine
// -----------------------------------------------------------------------------
//
void CStateMachine::Stop()
	{
	LOG( "+ CStateMachine::Stop()" );
	Cancel();
	iDepTree->RemoveDownloadedFiles( FsSession() );
	// Log statistics. Prevent divide-by-zero.
	if (iDownloadTime == 0)
		iDownloadTime = 1;
	LOG4( "Total %d kB in %.1f s, avg %.2f kB/s",
			iCumulativeDownloaded/1024,
			iDownloadTime/1000000.0,
			iCumulativeDownloaded*1000000.0 / iDownloadTime / 1024.0);
	}

// -----------------------------------------------------------------------------
// Returns current state ID
// -----------------------------------------------------------------------------
//
TInt CStateMachine::StateIndex() const
	{
	return iStateFactory->StateIndex(iState);
	}

// -----------------------------------------------------------------------------
// Request the next state we want the application to go to
// -----------------------------------------------------------------------------
//
void CStateMachine::SetState(const CStateFactory::TAppState aState)
	{
	SetState( iStateFactory->GetState(aState) );
	}

// -----------------------------------------------------------------------------
// Set the starting state of the state machine, but do not invoke state change
// -----------------------------------------------------------------------------
//
void CStateMachine::SetStartState(const CStateFactory::TAppState aState)
	{
	iState = &iStateFactory->GetState( aState );
	}

// -----------------------------------------------------------------------------
// Request the next state we want the application to go to
// -----------------------------------------------------------------------------
//
void CStateMachine::SetState(TState& aState)
	{
	LOG3( "+ SetState(): %d -> %d", iStateFactory->StateIndex(iState), iStateFactory->StateIndex(&aState) );

	iState->Exit();
	iState = &aState;
	SetActive();
	// send signal that this request has completed
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);

	//LOG2( "- SetState(): now in %d", iStateFactory->StateIndex(iState) );
	}

// -----------------------------------------------------------------------------
// Active object heart
// -----------------------------------------------------------------------------
//
void CStateMachine::RunL()
	{
	//LOG2( "+ CStateEngine::RunL(): state=%d", iStateFactory->StateIndex(iState) );
	iState->Enter();
	//LOG2( "- CStateEngine::RunL(): %d", iStateFactory->StateIndex(iState) );
	}

// -----------------------------------------------------------------------------
// Handles the leaves from RunL().
//
// SPECIFIC CASES NOT DONE YET:
// -
// -----------------------------------------------------------------------------
//
TInt CStateMachine::RunError(const TInt aError)
	{
	LOG3("+ SM::RunError(): %d, %d", StateIndex(), aError );

	TExitReason err = EInstallationFailed;

	switch ( aError )
		{
		case KLeaveExit:
			{
			// TODO: Ugly hack: we need to return KLeaveExit as RunError()
			// gets called when application exit is called within RunL().
			//LOG( "Exit() left" );
			return KLeaveExit;
			}
		case SwiUI::KSWInstErrUserCancel:
			{
			LOG( "User cancelled installation" );
			// User cancelled the installation
			err = EUserCancelled;
			}
			break;
		case SwiUI::KSWInstErrInsufficientMemory:
			{
			LOG( "Installer left: not enough space" );
			err = EInsufficientMemory;
			}
			break;
		case SwiUI::KSWInstErrMissingDependency:
			{
			LOG( "Installer left: missing dependency." );
			//err = EInstallationFailed;
			}
			break;
		case SwiUI::KSWInstErrFileCorrupted:
		case SwiUI::KSWInstErrPackageNotSupported:
		case SwiUI::KSWInstErrGeneralError:
		case SwiUI::KSWInstErrNoRights:
		case SwiUI::KSWInstErrNetworkFailure:
		case SwiUI::KSWInstErrAccessDenied:
		case SwiUI::KSWInstUpgradeError:
			{
			LOG2( "Installer left, err=%d", aError );
			//err = EInstallationFailed;
			}
			break;
		case KErrServerTerminated:
			{
			LOG( "Server process terminated, cannot continue." );
			}
			break;
		case EUnsupportedDevice:
			{
			LOG( "MachineId not present in supported devices list" );
			err = EDeviceNotSupported;
			}
			break;
		case KErrAlreadyExists:
			{
			// Download already exists
			LOG( "Existing download" );
			err = EDownloadFailed;
			}
			break;
		default:
			{
			LOG2( "Uncategorized leave %d, exiting", aError );
			}
			break;
		}

	LOG2( "Exiting due to failure %d", err );
	SetFailureReason( err );
	SetState( CStateFactory::EStatePrepareExitWithError );

	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CStateMachine::DoCancel()
	{
	LOG( "CStateMachine::DoCancel()" );
	iDownloadHandler->CancelAllDownloads();
	iInstallManager->CancelAll();
	}

// -----------------------------------------------------------------------------
// Sets the internet access point identifier for download handler
// -----------------------------------------------------------------------------
//
void CStateMachine::SetIAP(TUint32 aIAP)
	{
	iDownloadHandler->SetIAP(aIAP);
	}

// -----------------------------------------------------------------------------
// Handles the UI event from AppUi, filters it and passes it to states.
// -----------------------------------------------------------------------------
//
void CStateMachine::HandleUserResponse(TInt aStatus)
	{
	switch (aStatus)
		{
		case EAknSoftkeyYes:
			iState->UserAccepted();
			break;
		case EAknSoftkeyNo:
			iState->UserRejected();
			break;
		case EAknSoftkeyCancel:
		case EKeyPhoneEnd:
			// fall-through
		default:
			iState->UserCancelled();
			break;
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CStateMachine::HandleInstallCompletedL(TInt aStatus)
	{
	LOG4( "HandleInstallCompletedL(%d): %d. '%S'", aStatus, StateIndex(), iCurrentPackage->GetSisPackageName() );
	// TODO: Can iCurrentPackage ever be null? In case of root package installation??

	iState->InstallCompleted( aStatus );
	}

// -----------------------------------------------------------------------------
// Called, if IAP has changed
// -----------------------------------------------------------------------------
//
void CStateMachine::HandleIapChanged(const TUint32 aIAP)
	{
	iAppUi.iIAP = aIAP;
	SetIAP(aIAP);
	}

// -----------------------------------------------------------------------------
// Called when download has finished.
// -----------------------------------------------------------------------------
//
void CStateMachine::HandleHttpFetchCompleted(
		const TDesC& aDlFilename,
		const TInt /* aDownloadType */,
		const TInt32 aBytesDownloaded,
		const TReal32 aAvgDlSpeed, // bytes/second
		const TInt64 aDlTime       // microseconds
		)
	{
	delete iDownloadedFile;
	iDownloadedFile = NULL;
	iDownloadedFile = aDlFilename.Alloc();
	iDownloadSize = aBytesDownloaded;
	// Indicate that we've downloaded this amount already to keep progress bar happy
	iCumulativeDownloaded += aBytesDownloaded;
	// Calculate cumulative average download speed
	iDownloadTime += aDlTime;

#ifdef DEBUG_ADM
	TReal32 avgDlSpeed = iCumulativeDownloaded*1000000.0 / iDownloadTime;

#ifdef _DEBUG
	TBuf<32> buf;
	TReal r = avgDlSpeed;
	TRealFormat fmt(3, 2);
	fmt.iType = KRealFormatFixed; // | KDoNotUseTriads;
	buf.AppendNum(r/1024.0, fmt);
	buf.Append(' ');
	buf.Append(iAppUi.iPrefixKb);
	buf.Append('/');
	buf.Append('s');
	iAppUi.EikonEnv()->InfoMsg(buf);
#endif

	LOG8( "DL %7d/%7d (%.1f s, %3.02f kB/s) (total: %d kB, %.1f s, avg %.02f kB/s) OK",
			aBytesDownloaded, aBytesDownloaded, aDlTime/1000000.0, aAvgDlSpeed/1024.0,
			iCumulativeDownloaded/1024, iDownloadTime/1000000.0, avgDlSpeed/1024.0 );
#endif
	iState->DownloadCompleted(aBytesDownloaded);
	}

// -----------------------------------------------------------------------------
// Called when download manager has retrieved a packet from the network.
// Required for updating the progress bar.
// -----------------------------------------------------------------------------
//
void CStateMachine::HandleHttpFetchInProgress(const TInt32 aAlreadyDownloaded, const TInt32 aDownloadSize, const TReal32 aAvgDlSpeed)
	{
	iState->DownloadInProgress(aAlreadyDownloaded, aDownloadSize, aAvgDlSpeed);
	}

// -----------------------------------------------------------------------------
// Called when download manager encounters a problem while downloading
// -----------------------------------------------------------------------------
//
void CStateMachine::HandleHttpFetchFailure(
		const TDesC& aDlFilename,
		const TInt aGlobalErrorId,
		const TInt aErrorId,
		const TInt aFetchType
		)
	{
	if (aFetchType)
		{
		LOG5( "+ HandleHttpFetchFailure(): %d. %d, %d '%S'", StateIndex(), aGlobalErrorId, aErrorId, &aDlFilename );
		}
	delete iDownloadedFile;
	iDownloadedFile = NULL;
	iDownloadedFile = aDlFilename.Alloc();
	// TODO: Filter the errors here and invoke proper callbacks in states
	iState->DownloadFailed();
	LOG( "- HandleHttpFetchFailure()" );
	}

// -----------------------------------------------------------------------------
// Signals the semaphore that download deleting is in progress (and the installation
// can safely continue).
// -----------------------------------------------------------------------------
//
void CStateMachine::HandleDownloadDeleting()
	{
	iDlDeletingSemaphore.Signal();
	}

// =============================================================================

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TState& CStateFactory::GetState(const TAppState aState) const
	{
	__ASSERT_DEBUG( aState < EStateLast, Panic(EPanicStateIndexOutOfBounds) );
	return *iStates[aState];
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CStateFactory::StateIndex(const TState* aState) const
	{
	TInt state;
	for (state = 0; state < EStateLast; state++)
		{
		if (iStates[state] == aState)
			{
			return state;
			}
		}
	// We end up here, if the state is not set, which may happen, if this
	// is called before CStateMachine::Start() or CStateMachine::SetState().
	return KUnknownState;
	}

// -----------------------------------------------------------------------------
// Creates a new CStateFactory.
// @return Newly created CStateFactory.
// -----------------------------------------------------------------------------
//
CStateFactory* CStateFactory::NewL(CStateMachine& aStateMachine)
	{
	CStateFactory* self = new (ELeave) CStateFactory(aStateMachine);
	CleanupStack::PushL(self);

	// Create the states
	self->iStates[EStateSetup]                      = new (ELeave) TStateSetup(*self);
	self->iStates[EStateSetupParsing]               = new (ELeave) TStateSetupParsing(*self);
	self->iStates[EStateBuildDepTree]               = new (ELeave) TStateBuildDepTree(*self);
	self->iStates[EStateDownloadDepFile]            = new (ELeave) TStateDownloadDepFile(*self);
	self->iStates[EStateParseDepFile]               = new (ELeave) TStateParseDepFile(*self);
	self->iStates[EStateDownloadChangesFile]        = new (ELeave) TStateDownloadChangesFile(*self);
	self->iStates[EStateParseChangesFile]           = new (ELeave) TStateParseChangesFile(*self);
	self->iStates[EStateBuildFetchList]             = new (ELeave) TStateBuildFetchList(*self);
	self->iStates[EStateVerifyAvailableDiskSpace]   = new (ELeave) TStateVerifyAvailableDiskSpace(*self);
	self->iStates[EStateConfirmDownload]            = new (ELeave) TStateConfirmDownload(*self);
	self->iStates[EStateConfirmRoamingDownload]     = new (ELeave) TStateConfirmRoamingDownload(*self);
	self->iStates[EStateStartDependencyDownload]    = new (ELeave) TStateStartDependencyDownload(*self);
	self->iStates[EStateDownloadDependency]         = new (ELeave) TStateDownloadDependency(*self);
	self->iStates[EStateInstallDependency]          = new (ELeave) TStateInstallDependency(*self);
	self->iStates[EStateInstallAppSis]              = new (ELeave) TStateInstallAppSis(*self);
	self->iStates[EStateUninstallDependency]        = new (ELeave) TStateUninstallDependency(*self);
	self->iStates[EStatePrepareExitWithError]       = new (ELeave) TStatePrepareExitWithError(*self);
	self->iStates[EStateLaunchApp]                  = new (ELeave) TStateLaunchApp(*self);
	self->iStates[EStateExit]                       = new (ELeave) TStateExit(*self);

	CleanupStack::Pop();

	return self;
	}

// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
CStateFactory::CStateFactory(CStateMachine& aStateMachine) :
	iStateMachine(aStateMachine)
	{
	}

// -----------------------------------------------------------------------------
// Default destructor
// -----------------------------------------------------------------------------
//
CStateFactory::~CStateFactory()
	{
	// Deletes all state objects
	iStates.DeleteAll();
	}

// =============================================================================

TState::TState(const CStateFactory& aFactory) :
	iFactory(aFactory),
	iStateMachine(aFactory.StateMachine())
	{
	}

void TState::PanicInState(TStatePanic aPanic) const
	{
	Panic(aPanic, iFactory.StateIndex(this));
	}

// Change of states
void TState::Enter() const
	{
	}

void TState::Exit() const
	{
	}

// Events from UI
void TState::UserAccepted()
	{
	LOG_FUNC
	}

// -----------------------------------------------------------------------------
// Implements the default user cancel behaviour. If needed, overwrite in the
// state implementation.
// -----------------------------------------------------------------------------
//
void TState::UserCancelled()
	{
	LOG_FUNC
	iStateMachine.SetFailureReason(EUserCancelled);
	iStateMachine.SetState( CStateFactory::EStatePrepareExitWithError );
	}

// -----------------------------------------------------------------------------
// Implements the default user reject behaviour. If needed, overwrite in the
// state implementation.
// -----------------------------------------------------------------------------
//
void TState::UserRejected()
	{
	UserCancelled();
	}

// Events from DownloadManager
void TState::DownloadInProgress(const TInt /* aAlreadyDownloaded */, const TInt /* aDownloadSize */, const TReal32 /* aAvgDlSpeed */)
	{
	//LOG_FUNC
	}

void TState::DownloadCompleted(const TInt /* aBytesDownloaded */)
	{
	LOG_FUNC
	}

 void TState::DownloadFailed()
	{
	LOG_FUNC
	}

void TState::DownloadFailed(const CStateFactory::TAppState aState)
	{
	const EDownloadError errId = iStateMachine.iDownloadHandler->GetDownloadError();
	SLOG3( "+ DownloadFailed(): %d. err=%d", iStateMachine.StateIndex(), errId );

	if ( (errId == EDlErrDownloadFailure ) && ( iStateMachine.iRestartAttempt < KDownloadRestartRetries) )
		{
		iStateMachine.iRestartAttempt++;
		SLOG2( "Download restart, attempt %d", iStateMachine.iRestartAttempt );
		iStateMachine.SetState( aState );
		}
	else
		{
		SLOG3( "Aborting download. (%d; %d)", iStateMachine.iRestartAttempt, errId );
//      iStateMachine.iRestartAttempt = 0;
		iStateMachine.SetFailureReason(EDownloadFailed);
		iStateMachine.SetState( CStateFactory::EStatePrepareExitWithError );
		}
	}

void TState::DownloadCancelled()
	{
	LOG_FUNC
	iStateMachine.SetFailureReason(EUserCancelled);
	iStateMachine.SetState( CStateFactory::EStatePrepareExitWithError );
	}

// Events from InstallManager
void TState::InstallInProgress()
	{
	LOG_FUNC
	}

void TState::InstallCompleted(TInt /* aStatus */)
	{
	LOG_FUNC
	}

void TState::InstallationCancelled()
	{
	LOG_FUNC
	}

// =============================================================================
TStateSetup::TStateSetup(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	LOG_FUNC
	}

void TStateSetup::Enter() const
	{
	LOG_FUNC

	// TODO: Check if we are roaming and proceed according to that

	iStateMachine.SetState( CStateFactory::EStateSetupParsing );
	}

TStateSetupParsing::TStateSetupParsing(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateSetupParsing::Enter() const
	{
	const TInt mUid = iStateMachine.MachineUid();
	if ( mUid )
		{
		iStateMachine.iDepTree->SetMachineId( mUid );
		}
	else
		{
		PanicInState( EPanicStateInvalidMachineUid );
		}

	// Get the drive where wrapper package was installed to
	const TBool driveStatus = iStateMachine.iInstallManager->GetPackageInstalledDriveL(
			iStateMachine.WrapperPackageUid(),
			iStateMachine.iWrapperInstalledDrive);


#ifdef __WINS__
	iStateMachine.iWrapperInstalledDrive = 'C';
#else
	if ( !driveStatus )
		{
		// Read the wrapper installed drive from resume info file.
		// TODO: What if user has tried to install on a removable drive and it's not available during the menu launch?
		}
#endif

	SLOG3( "Installer pkg on %c: (%d)", (char)iStateMachine.iWrapperInstalledDrive, driveStatus );

	// Cannot combine this with drive status. User might be trying to reinstall an app, already present in the device!
	TVersion version, minVersion = KMinBootstrapSisVersion;
	if ( iStateMachine.iInstallManager->GetPackageVersionInfoL(KUidBootstrap, version) )
		{
		// Compare installed and min required Bootstrap versions
		if ( CompareVersions(minVersion, version) == EGreaterFirstVersion )
			{
			iStateMachine.SetFailureReason( EInvalidBootstrapVersion );
			iStateMachine.SetState( CStateFactory::EStatePrepareExitWithError );
			return;
			}
		}
	else
		{
		// Launched from the menu grid
		}

	switch ( iStateMachine.BootstrapVersion() )
		{
		case 1:
		case 2:
		case 3:
			{
			// Parses the dependency file included withing the wrapper package.
			SLOG2( "* Parsing '%S'", iStateMachine.DepFilename() );
			iStateMachine.iXmlParser->GetDepContentsL(
					*iStateMachine.DepFilename(),
					iStateMachine.iCurrentPackage,
					iStateMachine.iDepTree
				);
			iStateMachine.SetState( CStateFactory::EStateBuildDepTree );
			}
			break;
		case 4:
			{
			// Parses the application.sis file included in the wrapper package.
			SLOG2( "* Parsing '%S'", iStateMachine.SisFilename() );
			iStateMachine.iSisParser->GetDepContentsL(
					*iStateMachine.SisFilename(),
					iStateMachine.iCurrentPackage,
					iStateMachine.iDepTree
				);
			iStateMachine.SetState( CStateFactory::EStateBuildDepTree );
			}
			break;
		default:
			{
			SLOG2( "Invalid bootstrap v%d", iStateMachine.BootstrapVersion() );
			iStateMachine.SetFailureReason( EInvalidBootstrapVersion );
			iStateMachine.SetState( CStateFactory::EStatePrepareExitWithError );
			}
			break;
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TStateParseDepFile::TStateParseDepFile(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateParseDepFile::Enter() const
	{
	SLOG2( "+ ParseDepFileL(): '%S'", iStateMachine.iDownloadedFile );
	iStateMachine.iXmlParser->GetDepContentsL(
			*iStateMachine.iDownloadedFile,
			iStateMachine.iCurrentPackage,
			iStateMachine.iDepTree
		);
	iStateMachine.SetState( CStateFactory::EStateBuildDepTree );
	SLOG2( "- ParseDepFileL(): '%S'", iStateMachine.iDownloadedFile );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TStateParseChangesFile::TStateParseChangesFile(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateParseChangesFile::Enter() const
	{
	SLOG2("+ ParseChangesFileL(): '%S'", iStateMachine.iDownloadedFile);
	// Parse the provided changes file.
	// the parameter aChangesFileName will be invalid after the call
	iStateMachine.iXmlParser->GetChangesInfoL(
			*iStateMachine.iDownloadedFile,
			iStateMachine.iCurrentPackage,
			iStateMachine.iDepTree
		);
	iStateMachine.SetState( CStateFactory::EStateDownloadDepFile );
	SLOG2( "- ParseChangesFileL(): '%S'", iStateMachine.iDownloadedFile );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TStateBuildFetchList::TStateBuildFetchList(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateBuildFetchList::Enter() const
	{
	// Construct the list of dependent packages
	SLOG( "Building dependency list" );
	iStateMachine.iDepTree->ConstructFetchListL();

	// Remove all downloaded dep files
	SLOG( "Removing downloaded files" );
	iStateMachine.iDepTree->RemoveDownloadedFiles( iStateMachine.FsSession() );

	// Check if cycle exists
	if ( iStateMachine.iDepTree->IsCyclePresent() )
		{
		// TODO: Inform user about configuration failure, exit gracefully
		SLOG( "! Failure: cycle exists in the graph." );
		iStateMachine.SetFailureReason( EInstallationFailed );
		iStateMachine.SetState( CStateFactory::EStatePrepareExitWithError );
		}
	iStateMachine.SetState( CStateFactory::EStateVerifyAvailableDiskSpace );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TStateVerifyAvailableDiskSpace::TStateVerifyAvailableDiskSpace(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateVerifyAvailableDiskSpace::Enter() const
	{
	// MaxDownloadSize is subtracted from available System drive space.

	// In case of silent installation of main app, if there is no space available in the drive selected by the user for wrapper,
	// normal installation to follow.

	// Set the drive info
	if ( iStateMachine.iDepTree->SetDriveInfo() )
		{
		// With Ovi Store prompt always
		const TInt dlThreshold = iStateMachine.OviStoreRunning()
				? 0
				: KDownloadPromptThreshold;
		const TInt dlSize = iStateMachine.iDepTree->GetTotalDownloadSize();

		SLOG3( "Download size %d (%d)", dlSize, dlThreshold );

#ifdef DEBUG_ADM
		// Print all the drives set. To be removed after testing.
		CPackageInfo *node = NULL;
		while( (node = iStateMachine.iDepTree->GetNextSortedNode()) != NULL )
			{
			SLOG5( "PkgUid 0x%08X: InstDrive %c:, prio %d, size %d",
					node->GetPackageUid(), (char)node->GetInstallDrive(),
					node->GetDrivePriority(), node->GetInstallSize() );
			}
#endif
		// Show user how much to download and confirm, if he/she wants to do that
		// if we have dependencies to download at all
		if ( dlSize > 0 )
			{
			// Finalize the progress bar
			iStateMachine.UpdateProgressBar( KProgressBarFullValue, KProgressBarFullValue );

			// We need to download dependencies
			if ( iStateMachine.UsingWLAN() && !iStateMachine.OviStoreRunning() )
				{
				// If using WLAN, start the dependency download immediately without prompting the user
				iStateMachine.SetState( CStateFactory::EStateStartDependencyDownload );
				}
			else if ( iStateMachine.RegisteredRoaming() )
				{
				// If the device is roaming, always prompt the user
				iStateMachine.SetState( CStateFactory::EStateConfirmRoamingDownload );
				}
			else if ( dlSize > dlThreshold )
				{
				// If not using WLAN and the download threshold has exceeded, prompt the user
				iStateMachine.SetState( CStateFactory::EStateConfirmDownload );
				}
			else
				{
				// Otherwise, continue the download automatically without prompting the user
				iStateMachine.SetState( CStateFactory::EStateStartDependencyDownload );
				}
			}
		else
			{
			SLOG( "Nothing to download, dependencies OK." );
			// Keep the next progress bar update happy
			iStateMachine.iProgressMaxValue = KProgressBarFullValue;
			// Finalize the progress bar
			iStateMachine.UpdateProgressBar( KProgressBarFullValue*95/100, KProgressBarFullValue );
			iStateMachine.SetState( CStateFactory::EStateInstallAppSis );
			}
		}
	else
		{
		iStateMachine.SetFailureReason( EInsufficientMemory );
		iStateMachine.SetState( CStateFactory::EStatePrepareExitWithError );
		}
	}

// -----------------------------------------------------------------------------
// Builds the dependency tree. This function controls the state machine.
// -----------------------------------------------------------------------------
//
TStateBuildDepTree::TStateBuildDepTree(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateBuildDepTree::Enter() const
	{
	SLOG2( "+ BuildDepTreeL(%d)", iStateMachine.iNodesProcessed );

	// Update progress bar
	iStateMachine.iNodesProcessed++;

	iStateMachine.iCurrentPackage = iStateMachine.iDepTree->GetNextNode();
	CPackageInfo* package = iStateMachine.iCurrentPackage;

	if ( package == NULL )
		{
		SLOG( "Last package processed" );
		// Do the final progress bar update
		iStateMachine.UpdateProgressBar( KProgressBarFullValue*95/100, KProgressBarFullValue );

		iStateMachine.SetState( CStateFactory::EStateBuildFetchList );
		}
	else
		{
		SLOG3( "* Processing 0x%08X (%d)", package->GetPackageUid(), package->GetPackageStatus() );

		iStateMachine.UpdateProgressBar( iStateMachine.iNodesProcessed,
				iStateMachine.iNodesProcessed+2 );

		// Call the install class api to validate and set the package status
		TUid uid = TUid::Uid( package->GetPackageUid() );
		TVersion version, test;

		test = package->GetPackageVersion();

		if (iStateMachine.iInstallManager->GetPackageVersionInfoL(uid, version))
			{
			// Compare installed and required verions
			// to find if upgrade is required.
			if ( (CompareVersions(test, version) ) == EGreaterFirstVersion)
				{
				SLOG8( "Package 0x%08X: upgrade %d.%02d.%d -> %d.%02d.%d",
						uid.iUid,
						version.iMajor, version.iMinor, version.iBuild,
						test.iMajor, test.iMinor, test.iBuild
						);
				package->SetPackageStatus(EPackageToBeUpgraded);
				}
			else
				{
				SLOG5( "Package 0x%08X: v%d.%02d.%d already installed", uid.iUid,
						version.iMajor, version.iMinor, version.iBuild
						);
				package->SetPackageStatus(EPackageInstalled);
				}
			}
		else
			{
			SLOG5( "Package 0x%08X: v%d.%02d.%d to be downloaded", uid.iUid,
					test.iMajor, test.iMinor, test.iBuild
					);
			package->SetPackageStatus(EPackageToBeFetched);
			}

		iStateMachine.SetState( CStateFactory::EStateDownloadChangesFile );
		}
	SLOG2( "- BuildDepTreeL(%d)", iStateMachine.iNodesProcessed );
	}

// -----------------------------------------------------------------------------
// Requests a download for the dependecy sis package _dep.xml file.
// -----------------------------------------------------------------------------
//
TStateDownloadDepFile::TStateDownloadDepFile(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateDownloadDepFile::Enter() const
	{
	//Here fetch the dep file of the current package.
	SLOG2( "+ FetchDepFileL(): 0x%08X", iStateMachine.iCurrentPackage->GetPackageUid() );

	// TODO:Here the download Url should never be null]
	// as its mandatory to specify the Url in changes file.
	TPtrC8 urlPtr(NULL, 0);
	if (iStateMachine.iCurrentPackage->GetDownloadUrlL() != NULL)
		{
		SLOG8_2( "DepPkg URL: '%S'", iStateMachine.iCurrentPackage->GetDownloadUrlL() );
		urlPtr.Set(iStateMachine.iCurrentPackage->GetDownloadUrlL()->Des() );
		}
	else
		{
		SLOG8_2( "DepPkg URL: '%S' (default)", iStateMachine.ConfigUrl() );
		urlPtr.Set( *iStateMachine.ConfigUrl() );
		}

	User::LeaveIfNull( iStateMachine.iCurrentPackage->GetDepFileName() );

	SLOG2( "Downloading DEP: '%S'", iStateMachine.iCurrentPackage->GetDepFileName() );

	iStateMachine.iDownloadHandler->StartDownloadL(
		urlPtr,
		*iStateMachine.iCurrentPackage->GetDepFileName(),
		iStateMachine.iDepTree->GetDownloadPath(),
		1);
	// We don't request a new state after this. When download completes
	// succesfully, a new state change will be requested.
	}

void TStateDownloadDepFile::DownloadCompleted(const TInt /* aBytesDownloaded */)
	{
	iStateMachine.iRestartAttempt = 0;
	iStateMachine.SetState( CStateFactory::EStateParseDepFile );
	}

void TStateDownloadDepFile::DownloadFailed()
	{
	LOG_FUNC
	TState::DownloadFailed( CStateFactory::EStateDownloadDepFile );
	}

// -----------------------------------------------------------------------------
// Requests a download for the _changes.xml file.
// -----------------------------------------------------------------------------
//
TStateDownloadChangesFile::TStateDownloadChangesFile(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateDownloadChangesFile::Enter() const
	{
	// See if the URL is provided as part of the
	// dep file, otherwise use the default one.
	TPtrC8 urlPtr(NULL, 0);
	if ( iStateMachine.iCurrentPackage->GetURL() != NULL )
		{
		SLOG8_2( "Changes URL: '%S'", iStateMachine.iCurrentPackage->GetURL() );
		urlPtr.Set(iStateMachine.iCurrentPackage->GetURL()->Des());
		}
	else
		{
		SLOG8_2( "Changes URL: '%S' (default)", iStateMachine.ConfigUrl() );
		urlPtr.Set( *iStateMachine.ConfigUrl() );
		}

		//TODO: The only reason why this is back here :
		//Changes file name for Root is not set when root is parsed. Fix it.
		iStateMachine.iCurrentPackage->SetChangesFileNameL();
	SLOG2( "Downloading CHANGES: '%S'", iStateMachine.iCurrentPackage->GetChangesFileName() );

	// Download the changes file
	iStateMachine.iDownloadHandler->StartDownloadL(
		urlPtr,
		*iStateMachine.iCurrentPackage->GetChangesFileName(),
		iStateMachine.iDepTree->GetDownloadPath(),
		0);
	// We don't request a new state after this. When download completes
	// succesfully, a new state change will be requested by DownloadCompleted()
	}

void TStateDownloadChangesFile::DownloadCompleted(const TInt /*aBytesDownloaded*/)
	{
	LOG_FUNC

	iStateMachine.iNodesProcessed++;
	iStateMachine.UpdateProgressBar( iStateMachine.iNodesProcessed, iStateMachine.iNodesProcessed+2 );

	iStateMachine.iRestartAttempt = 0;
	iStateMachine.SetState( CStateFactory::EStateParseChangesFile );
	}

void TStateDownloadChangesFile::DownloadFailed()
	{
	LOG_FUNC
	TState::DownloadFailed( CStateFactory::EStateDownloadChangesFile );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TStateStartDependencyDownload::TStateStartDependencyDownload(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateStartDependencyDownload::Enter() const
	{
	const TUint32 totalDlSize = iStateMachine.iDepTree->GetTotalDownloadSize();
	SLOG2( "Starting to download %d bytes", totalDlSize );

	// Remove all downloads before proceeding
	iStateMachine.iDownloadHandler->CancelAllDownloads();

	// Start progress bar from beginning
	iStateMachine.iCounterCurrent = 1;
	iStateMachine.iCounterMax = iStateMachine.iDepTree->CountDownloadNodes();

	// reset to zero for download and install of packages.
	iStateMachine.iDownloadSize = 0;
	iStateMachine.iNodesProcessed = 0;

	// Start the dependency download!
	iStateMachine.SetState( CStateFactory::EStateDownloadDependency );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TStateDownloadDependency::TStateDownloadDependency(const CStateFactory& aFactory) :
	TState(aFactory),
	iRetryDownload(EFalse)
	{
	}

void TStateDownloadDependency::Enter() const
	{
	SLOG3( "+ FetchPackageL(%d): retry=%d", iStateMachine.iNodesProcessed, iRetryDownload );

	CPackageInfo* fetchPackage = NULL;

	if ( iRetryDownload )
		{
		fetchPackage = iStateMachine.iDepTree->GetCurrentFetchNode();
		}
	else
		{
		iStateMachine.iNodesProcessed++;
		// always ensure that iCurrentPackage points to the correct one
		fetchPackage = iStateMachine.iDepTree->GetNextFetchNode();
		}
	iStateMachine.iCurrentPackage = fetchPackage;

	if ( !fetchPackage )
		{
/*
		//end of list - we're finished
		SLOG2( "Uninstalling wrapper 0x%08X", iStateMachine.WrapperPackageUid().iUid );
		TInt err = iStateMachine.iInstallManager->SilentUnInstallPackage(
			iStateMachine.WrapperPackageUid() );
		SLOG2( "Uninstall status: %d", err);
*/
		// Wrapper has been uninstalled, ready to exit/launch
		iStateMachine.SetState( CStateFactory::EStateExit );
		return;
		}
/*
	// Attempting to fix 'Network connection lost!' errors on 3G. Does not help.
	SLOG( "Deleting all downloads" );
	iStateMachine.iDownloadHandler->CancelAllDownloads();
*/
	switch ( fetchPackage->GetPackageStatus() )
		{
		case EPackageRootToBeInstalled:
			{
			//SLOG( "* Installing AppSis" );

			iStateMachine.SetState( CStateFactory::EStateInstallAppSis );
			}
			break;
		case EPackageToBeFetched:
		case EPackageToBeUpgraded:
			{
			SLOG2( "* Package 0x%08X to be Fetched/Upgraded", fetchPackage->GetPackageUid() );

			// Initialize progress bar
			//ShowGlobalProgressL(R_ADM_DOWNLOAD_WAIT_TEXT, 0, KMaxTInt);

			if ( iStateMachine.iCounterMax > 0 )
				{
				iStateMachine.StopWaitNote();
				// prevent progress bar from starting from previous dl size
				iStateMachine.iProgressCurrentValue = 0;
				iStateMachine.iCumulativeDepDownloaded = 0;
				iStateMachine.iProgressMaxValue = fetchPackage->GetDownloadSize();
				iStateMachine.ShowProgressBarL(
						0,
						iStateMachine.iProgressMaxValue,
						iStateMachine.iCounterCurrent,
						iStateMachine.iCounterMax
						);
				}

			// TODO:Here the download Url should never be null,
			// as its mandatory to specify the Url in changes file.
			TPtrC8 urlPtr(NULL, 0);
			if (fetchPackage->GetDownloadUrlL() != NULL )
				{
				SLOG8_2( "Package URL: '%S'", fetchPackage->GetDownloadUrlL());
				urlPtr.Set(fetchPackage->GetDownloadUrlL()->Des());
				}
			else
				{
				SLOG8_2( "Package URL: '%S' (default)", iStateMachine.ConfigUrl() );
				urlPtr.Set(*iStateMachine.ConfigUrl());
				}

			User::LeaveIfNull( fetchPackage->GetSisPackageName() );

			SLOG2( "Downloading: '%S'", fetchPackage->GetSisPackageName() );
			iStateMachine.iDownloadHandler->StartDownloadL(
				urlPtr,
				*fetchPackage->GetSisPackageName(),
				iStateMachine.iDepTree->GetDownloadPath(),
				2);
			}
			break;
		case EPackageInstalled:
			{
			SLOG2( "* Package 0x%08X installed", fetchPackage->GetPackageUid());
			//FetchPackageL();
			iStateMachine.SetState( CStateFactory::EStateDownloadDependency );
			}
			break;
		default:
			{
			// TODO: Better error handling
			Panic( EPanicStateFetchStatus );
			}
			break;
		} // switch
	SLOG2( "- FetchPackageL(%d)", iStateMachine.iNodesProcessed );
	}

void TStateDownloadDependency::DownloadCompleted(const TInt aBytesDownloaded)
	{
	iRetryDownload = EFalse;
	// Indicate that we've downloaded this amount already to keep progress bar happy
	iStateMachine.iCumulativeDepDownloaded += aBytesDownloaded;
	iStateMachine.iDownloadSize = iStateMachine.iCumulativeDepDownloaded;
	iStateMachine.SetState( CStateFactory::EStateInstallDependency );
	}

void TStateDownloadDependency::DownloadInProgress(const TInt aAlreadyDownloaded, const TInt aDownloadSize, const TReal32 aAvgDlSpeed)
	{
	static TInt32 prevAlready = 0;
	// Prevents log and event flooding
	if (aAlreadyDownloaded != prevAlready)
		{
		prevAlready = aAlreadyDownloaded;

		iStateMachine.iDownloadSize = aAlreadyDownloaded + iStateMachine.iCumulativeDepDownloaded;
		iStateMachine.iProgressCurrentValue =
				iStateMachine.iCumulativeDepDownloaded + aAlreadyDownloaded;
		if (aAvgDlSpeed > 0)
			{
			SLOG4( "DL %7d/%7d (%3.02f kB/s)", aAlreadyDownloaded, aDownloadSize, aAvgDlSpeed/1024 );
			}
		else
			{
			SLOG3( "DL %7d/%7d", aAlreadyDownloaded, aDownloadSize );
			}
		// Update the progress bar
		iStateMachine.UpdateProgressBar( iStateMachine.iProgressCurrentValue,
				iStateMachine.iProgressMaxValue );
		}
	}

void TStateDownloadDependency::DownloadFailed()
	{
	LOG_FUNC
	iRetryDownload = ETrue;
	TState::DownloadFailed( CStateFactory::EStateDownloadDependency );
	}

// -----------------------------------------------------------------------------
// Uninstalls installed dependencies one at the time
// -----------------------------------------------------------------------------
//
TStateUninstallDependency::TStateUninstallDependency(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateUninstallDependency::Enter() const
	{
	CPackageInfo* node = iStateMachine.iDepTree->GetPreviousFetchNode();
	// Uninstall all the fetched and installed dependencies
	if (node)
		{
		// TODO: Can the package present check be moved to Install handler?. Below is an async function though.
		if ( (node->GetPackageStatus() == EPackageFetchedInstalled) &&
			( iStateMachine.iInstallManager->IsPackagePresentL(TUid::Uid(node->GetPackageUid())) ) )
			{
			SLOG2( "Uninstalling: 0x%x", node->GetPackageUid() );

			iStateMachine.iInstallManager->
				SilentUninstallPackageAsync( TUid::Uid(node->GetPackageUid()) );
			// the next event will be triggered by InstallCompleted() below
			}
		else
			{
			SLOG2( "Skipping: 0x%x", node->GetPackageUid() );
			// Process the next package
			iStateMachine.SetState( CStateFactory::EStateUninstallDependency );
			}
		}
	else
		{
		SLOG( "About to uninstall wrapper" );
		// We've processed the dependency tree, uninstall the wrapper
		// Failure reason is set before calling EPrepareForExitWithError
		iStateMachine.SetState( CStateFactory::EStateExit );
		}
	}

// This is actually handling the UNinstallation completed event
void TStateUninstallDependency::InstallCompleted(TInt aStatus)
	{
	SLOG2( "UninstallDependencyCompleted(): %d", aStatus );
	// Try to uninstall next dependency, without minding the status code
	iStateMachine.SetState( CStateFactory::EStateUninstallDependency );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TStateLaunchApp::TStateLaunchApp(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateLaunchApp::Enter() const
	{
	RArray<TUid> appSids;
	TBool launchStatus = ETrue;
	// Get the SID list from installer
	TUid uid = TUid::Uid( iStateMachine.iCurrentPackage->GetPackageUid() );
	const TBool status = iStateMachine.iInstallManager->GetPackageSidsL(uid, appSids);

	if ( status )
		{
#ifdef DEBUG_ADM
		for (TInt i = 0; i < appSids.Count(); i++)
			{
			SLOG3( "SID[%d]=0x%08X", i, appSids[i] );
			}
#endif
		TApaAppInfo appInfo;
		RApaLsSession lsSession;

		// Connect to Apparc
		if ( !lsSession.Connect() )
			{
			CleanupClosePushL( lsSession );

			for (TInt i = 0; i < appSids.Count(); i++)
				{
				TInt ret = lsSession.GetAppInfo( appInfo, appSids[i] );
				// Get the app Info of the Sid.
				if ( ret == KErrNone )
					{
					HBufC* iconFilename = NULL;
					// Get the application icon, if any. It's used in the launch dialog.
					ret = lsSession.GetAppIcon( appSids[i], iconFilename );
					if (ret == KErrNone)
						{
						// We have a launchable exe and icon: get the executable name and caption
						iStateMachine.iLaunchName = appInfo.iFullName;
						iStateMachine.iLaunchCaption = appInfo.iCaption;
						iStateMachine.iLaunchIconFilename.Copy( *iconFilename );
						SLOG5( "LaunchExe %d '%S' '%S' '%S'", i, &iStateMachine.iLaunchName, &iStateMachine.iLaunchCaption, &iStateMachine.iLaunchIconFilename );

						delete iconFilename;
						break;
						}
					// The .exe didn't have an icon, try next .exe.
					// We're assuming that only exe's having an icon are the ones we should launch
					} // if
				} // for
			CleanupStack::PopAndDestroy( &lsSession );
			} //if ( !lsSession.Connect() )
		else
			{
			launchStatus = EFalse;
			}
		} // if( status )
	else
		{
		launchStatus = EFalse;
		}

	appSids.Close();

	if ( launchStatus && iStateMachine.iLaunchName.Length() )
		{
		iStateMachine.SetAppLaunch(ETrue);
		iStateMachine.iAppUi.ShowLaunchPromptL( iStateMachine.iLaunchCaption, iStateMachine.iLaunchIconFilename );
		}
	else
		{
		// No executable to launch!
		iStateMachine.SetState( CStateFactory::EStateExit );
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void TStateLaunchApp::UserAccepted()
	{
	RApaLsSession lsSession;

	// Connect to Apparc
	User::LeaveIfError( lsSession.Connect() );
	CleanupClosePushL( lsSession );

	CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
	cmdLine->SetExecutableNameL( iStateMachine.iLaunchName );
	cmdLine->SetCommandL( EApaCommandRun );

	SLOG3( "Launching %S '%S'", &iStateMachine.iLaunchCaption, &iStateMachine.iLaunchName );
	// Launch the application
	// TODO: Better error handling
	User::LeaveIfError( lsSession.StartApp( *cmdLine ) );

	CleanupStack::PopAndDestroy( 2, &lsSession ); // cmdLine, lsSession

	iStateMachine.SetState( CStateFactory::EStateExit );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void TStateLaunchApp::UserRejected()
	{
	SLOG( "User denied application launch" );
	iStateMachine.SetAppLaunch(EFalse);
	iStateMachine.SetState( CStateFactory::EStateExit );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TStateExit::TStateExit(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateExit::Enter() const
	{
	SLOG2( "Exit (%d)", iStateMachine.FailedState() );

#ifdef FEATURE_INSTALL_RESUME
	// FailedState is checked to differentiate a completion of successful menu launch from failure cases.
	// Also uninstall the wrapper, when the resume information file is not required
	// (in case of Bootstrap failure mode - KCmdLineCancelAdmNoResume)
	if ( ( iStateMachine.FailedState() == CStateFactory::EStateLast ) ||
			 ( iStateMachine.FailedState() == KUnknownState ) ||
			 ( !iStateMachine.ResumeRequired() )
			)
		{
		// Do the final cleanup here: remove the wrapper package, which
		// will also remove the bootstrap
		SLOG2( "Uninstalling wrapper 0x%08X", iStateMachine.WrapperPackageUid().iUid );
		const TInt err = iStateMachine.iInstallManager->SilentUnInstallPackage(
				iStateMachine.WrapperPackageUid() );
		if (err != KErrNone)
			{
			SLOG2( "! Uninstall failed (%d)", err );
			}

		// Remove resume info files
		RemoveResumeFiles();
		}
	else
		{
		// Remove only the bootstrap.
		// Wrapper is retained for Menu launch.
		// It is possible that bootstrap is already removed in previous unsuccessful menu launch
		if ( iStateMachine.iInstallManager->IsPackagePresentL( KUidBootstrap ) )
			{
			SLOG2( "Uninstalling bootstrap 0x%08X", KUidBootstrap );
			const TInt err = iStateMachine.iInstallManager->SilentUnInstallPackage( KUidBootstrap );
			// ignore any errors
			if (err != KErrNone)
				{
				SLOG2( "! Uninstall failed (%d)", err );
				}
			}
		}
#else
	// Do the final cleanup here: remove the wrapper package
	SLOG2( "Uninstalling wrapper 0x%08X", iStateMachine.WrapperPackageUid().iUid );
	const TInt err = iStateMachine.iInstallManager->SilentUnInstallPackage(
			iStateMachine.WrapperPackageUid() );
	if (err != KErrNone)
		{
		SLOG2( "! Uninstall failed (%d)", err );
		}
#endif // FEATURE_INSTALL_RESUME

#ifdef FEATURE_REPORTING
	// Report only the specified cases
	switch ( iStateMachine.ExitReason() )
		{
	case EExitNoError:
	case EUserCancelled:
//	case EDownloadFailed:
	case EInsufficientMemory:
	case EDeviceNotSupported:
	case EInstallationFailed:
	case EInvalidBootstrapVersion:
	case ERootInstallationFailed:
		Report();
		break;
	default:
		break;
		}
#else
	// We can exit the application
	// The default failure reason is "No Error"
	iStateMachine.iAppUi.ExitApp();

	// Above call does not return
#endif
	}

// -----------------------------------------------------------------------------
// RemoveResumeFiles()
// On successful completion of the installation process, remove the resume info file.
// -----------------------------------------------------------------------------
//
void TStateExit::RemoveResumeFiles() const
	{
	if ( BaflUtils::FileExists(iStateMachine.FsSession(), KADMResumeInfoFile) )
		{
		RArray< TPtrC > lineBuffer;
		TInt ret = 0;
		HBufC* buffer = ReadConfigFile( iStateMachine.FsSession(), KADMResumeInfoFile, lineBuffer, ret );

		if ( buffer && ret == KErrNone )
			{
			// Get AppFileName from the config file
			TPtrC appFileNameLine = GetConfigValue( KRFieldAppFileName, lineBuffer, ret );
			if ( ret == KErrNone )
				{
				SLOG2( "AppFileName = '%S'", &appFileNameLine );

				if ( BaflUtils::FileExists(iStateMachine.FsSession(), appFileNameLine) )
					{
					const TInt err = iStateMachine.FsSession().Delete( appFileNameLine );
					SLOG2( "AppFile has been deleted (%d)", err);
					}
				}
			else
				{
				SLOG2( "Failed to read value from resume info (%d)", ret );
				}
			delete buffer;
			}
		ret = iStateMachine.FsSession().Delete( KADMResumeInfoFile );
		SLOG2( "Resume info has been deleted (%d)", ret );

		// TODO: Need to remove the copied dep file as well
		}
	}
#ifdef FEATURE_REPORTING
// -----------------------------------------------------------------------------
// Creates a report about the installation and sends that to the server
// -----------------------------------------------------------------------------
//
void TStateExit::Report() const
	{
	_LIT8(KFmtVersion, "%d.%02d.%d");
	_LIT(KOK, "OK");
	TBuf8<KMaxFileName> url;
	url.Copy( *iStateMachine.ConfigUrl() );

	// -----------------------------------------------------------------------------
	// The base report URL is:
	// http://server/root/x.yy.z/reason/flags/OK
	//
	// where
	//
	// http://server/root/     ConfigUrl()
	// x.yy.z                  ADM version formatted as %d.%02d.%d
	// reason                  iExitReason
	// flags                   Flags defined below in Report Version 1
	// OK                      Non-existing file
	//
	// -----------------------------------------------------------------------------
	// Report Version 1
	//
	//31        11 10  9  8  7  6  5  4  3  2  1  0
	// +-------+--+--+--+--+--+--+--+--+--+--+--+--+
	// |reservd|  |  |                 |           |
	// +-------+--+--+--+--+--+--+--+--+--+--+--+--+
	//
	//      value
	// bits range  description
	//  0-3  0-15  Report version
	//
	// Other bits for version 1
	//
	//      value
	// bits range  description
	//  4-9  0-63  Number of packages downloaded
	//   10   1    Ovi Store running
	//   11   1    Application launched
	// -----------------------------------------------------------------------------
	//

	// Get the ADM version
	TVersion version;
	iStateMachine.iInstallManager->GetPackageVersionInfoL(KUidADMApp, version);

#if 0
	const TUint32 KReportVersion = 1;
	TUint32 flags;
	flags  = KReportVersion                                        << 0;  // 4 bits (0-15)
	flags |= (iStateMachine.iDepTree->CountDownloadNodes() & 0x3f) << 4;  // 6 bits (0-63)
	flags |= iStateMachine.OviStoreRunning()                       << 10; // 1 bit
	flags |= iStateMachine.AppLaunch()                             << 11; // 1 bit
	SLOG2( "Exit: 0x%08x", flags );
#endif

	url.AppendFormat(KFmtVersion, version.iMajor, version.iMinor, version.iBuild);
	url.Append('/');
	url.AppendNum( iStateMachine.ExitReason() );
	url.Append('/');
	url.AppendNum( iStateMachine.OviStoreRunning() );
	url.Append('/');
	url.AppendNum( iStateMachine.ResumingInstallation() );
	url.Append('/');
#if 0
	url.AppendNumFixedWidthUC(flags, EHex, 8);
	url.Append('/');
	url.AppendNum( iStateMachine.OviStoreRunning() );
	url.Append('/');
	url.AppendNum( iStateMachine.AppLaunch() );
	url.Append('/');
#endif
	iStateMachine.iDownloadHandler->StartDownloadL(
		url,
		KOK,
		iStateMachine.iDepTree->GetDownloadPath(),
		0);
	}

// -----------------------------------------------------------------------------
// Ignore "in progress" event calls.
// -----------------------------------------------------------------------------
//
void TStateExit::DownloadInProgress(const TInt /* aAlreadyDownloaded */, const TInt /* aDownloadSize */, const TReal32 /* aAvgDlSpeed */)
	{
	}

// -----------------------------------------------------------------------------
// Transfer control to ExitApp().
// -----------------------------------------------------------------------------
//
void TStateExit::DownloadCompleted(const TInt /* aBytesDownloaded */)
	{
	iStateMachine.iAppUi.ExitApp();
	}

// -----------------------------------------------------------------------------
// Transfer control to ExitApp().
// -----------------------------------------------------------------------------
//
void TStateExit::DownloadFailed()
	{
	iStateMachine.iAppUi.ExitApp();
	}

// -----------------------------------------------------------------------------
// Transfer control to ExitApp().
// -----------------------------------------------------------------------------
//
void TStateExit::DownloadCancelled()
	{
	iStateMachine.iAppUi.ExitApp();
	}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TStatePrepareExitWithError::TStatePrepareExitWithError(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStatePrepareExitWithError::Enter() const
	{
	// Show the user a wait note, if needed
	iStateMachine.iAppUi.HandleFailure();

	// TODO: FIX THIS! DlMgr requires some time for its AOs to do the cleanup.
	User::After(1000000);
	iStateMachine.iDownloadHandler->CancelAllDownloads();
	iStateMachine.iDepTree->RemoveDownloadedFiles( iStateMachine.FsSession() );

#ifdef FEATURE_INSTALL_RESUME
	// Backup the resume info for future launch
	if ( iStateMachine.ResumeRequired() )
		{
		// Create the resume information
		if ( CreateInstallationResumeInfo() == KErrNone )
			{
			// Dependencies are not uninstalled if launched from Menu
			iStateMachine.SetState( CStateFactory::EStateExit );
			return;
			}
		}
	// If no resume info required or CreateInstallationResumeInfo fails.
	iStateMachine.SetState( CStateFactory::EStateUninstallDependency );
#else
	// Trigger the dependency uninstallation
	iStateMachine.SetState( CStateFactory::EStateUninstallDependency );
#endif // FEATURE_INSTALL_RESUME
	}

// -----------------------------------------------------------------------------
// CreateInstallationResumeInfo
// Backup the necessary information to continue the installation later i.e.
// from the application menu grid.
// -----------------------------------------------------------------------------
//
#define CHECK(a) {    \
	if (a != KErrNone) \
		goto failed;   \
	pos++;             \
	}

TInt TStatePrepareExitWithError::CreateInstallationResumeInfo() const
	{
	SLOG( "+ CreateInstallationResumeInfo()" );

	// This is increment in each CHECK() macro
	TInt pos = 0;

	// Step 0: Backup the required files to ADM Private folder.
	// (Also the names are written to resume info file)
	TRAPD( err, BackupFilesL() );
	CHECK(err);

	RFile outFile;
	CPackageInfo* node = NULL;

	TInt numberOfNodes = iStateMachine.iDepTree->NodesCount();
	// Skip the main application
	if (numberOfNodes > 0)
		{
		numberOfNodes--;
		}

	err = outFile.Open( iStateMachine.FsSession(), KADMResumeInfoFile, EFileWrite | EFileStreamText | EFileShareAny );
	CHECK(err);

	// Step 1: Write the application name
	node = iStateMachine.iDepTree->GetRootNode();
	SLOG3( "Saving state for '%S' (%d)", node->GetPackageName(), numberOfNodes );
	err = WriteToConfigFile( outFile, KRFieldAppName, *node->GetPackageName() );
	CHECK(err);

	// Step 2: Write the depPkg UIDs to resume info file.
	// A new resume info file is already created as part of the "BackupFilesL()"
	// If user presses cancel before the fetchlist is constructed, then this loop will not execute.
	for (TInt i = 0; i < numberOfNodes; i++)
		{
		node =  iStateMachine.iDepTree->GetFetchNode( i );
		SLOG2( "Adding info: 0x%08X", node->GetPackageUid() );

		err = WriteIntToConfigFile( outFile, KRFieldDepPkgUid, node->GetPackageUid() );
		CHECK(err);
		}

#if 0
	node = iStateMachine.iDepTree->GetRootNode();
	SLOG2( "Adding main: 0x%08X", node->GetPackageUid() );

	// Step 3: Write the Root pkg UID to resume info file.
	// TODO: Check whether this is really needed.
	err = WriteIntToConfigFile( outFile, KRootPkgUid, node->GetPackageUid() );
	CHECK(err);
#endif

	// Step 4: Write the BootstrapVersion to resume info file.
	err = WriteIntToConfigFile( outFile, KRFieldBootstrap, iStateMachine.BootstrapVersion() );
	CHECK(err);

	// Step 5: Write the Wrapper installed drive to resume info file.
	err = WriteIntToConfigFile( outFile, KRFieldWrapperDrive, iStateMachine.iWrapperInstalledDrive );
	CHECK(err);

	// Step 6: Write the Wrapper Package UID.
	err = WriteIntToConfigFile( outFile, KRFieldWrapperPkgUid, iStateMachine.WrapperPackageUid().iUid );

	if (err != KErrNone)
		{
failed:
		SLOG3( "Resume info creation failed (%d: %d)", pos, err );
		// Remove the resume info file in error situation as it might not contain
		// all the information needed to continue the installation
		const TInt deleteStatus = iStateMachine.FsSession().Delete(KADMResumeInfoFile);
		SLOG2( "Resume info has been removed (%d)", deleteStatus );
		}

	if (pos)
		{
		outFile.Close();
		}

	SLOG2( "- CreateInstallationResumeInfo(): %d", err );

	return err;
	}

// -----------------------------------------------------------------------------
// BackupFiles
// In case of failure during the complete installation process,
// backup the required files for future launch.
// -----------------------------------------------------------------------------
//
void TStatePrepareExitWithError::BackupFilesL() const
	{
#if 0
	HBufC* destFileName = NULL;
	TInt err = KErrNone, pos = 0;

	// Backup the main application sis file to a folder. - Copy to ADM Private folder?!
	TParsePtrC appFileName( *iStateMachine.SisFilename() );

	destFileName = HBufC::NewLC( appFileName.NameAndExt().Length() + KADMPrivatePath().Length() );
	TPtr nameBufPtr = destFileName->Des();

	nameBufPtr.Append( KADMPrivatePath );
	nameBufPtr.Append( appFileName.NameAndExt() );

	// TODO: Do the file copying in AO
	CFileMan* fileMan = CFileMan::NewL( iStateMachine.FsSession() );
	CleanupStack::PushL(fileMan);
	err = fileMan->Copy( *iStateMachine.SisFilename(), *destFileName );
	CleanupStack::PopAndDestroy(fileMan);

	if (err != KErrNone)
		{
failed:
		CleanupStack::PopAndDestroy(destFileName);
		SLOG3( "Failed to create backup (%d: %d)", pos, err );
		User::Leave(err);
		}

	// Backup the info. The file is not present yet.
	RFile outFile;
	err = outFile.Replace( iStateMachine.FsSession(), KADMResumeInfoFile, EFileWrite | EFileStreamText | EFileShareAny );
	CHECK(err);

	// TODO: Backup the depfile in case of Bootstrap version 2 and save the name as well.

	// Write the AppFilename to resume info file.
	err = WriteToConfigFile( outFile, KAppFileName, destFileName->Des() );
	CHECK(err);
	CleanupStack::PopAndDestroy(); // destFileName
#endif
	// Backup the info. The file is not present yet.
	RFile outFile;

	// If no resume info file exists, create a new one.
	// In case a resume info file exists, replace the file with a new one.
	// (One resume information file supported at any point of time)
	TInt err = outFile.Replace( iStateMachine.FsSession(), KADMResumeInfoFile, EFileWrite | EFileStreamText | EFileShareAny );

	if (err != KErrNone)
		{
		goto failed;
		}

	// TODO: Backup the depfile in case of Bootstrap version 2 and save the name as well.

	// Write the AppFilename to resume info file.
	err = WriteToConfigFile( outFile, KRFieldAppFileName, iStateMachine.SisFilename()->Des() );

	if (err != KErrNone)
		{
failed:
		SLOG2( "- BackupFiles(): LEAVE %d", err );
		outFile.Close();
		User::Leave(err);
		}

	outFile.Close();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TStateInstallDependency::TStateInstallDependency(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateInstallDependency::Enter() const
	{
	SLOG3( "Installing '%S' to %c:", iStateMachine.iDownloadedFile, (char)iStateMachine.iCurrentPackage->GetInstallDrive() );
	// Replace the downloading note with installing note
	iStateMachine.ShowInstallingWaitNoteL( iStateMachine.iCounterCurrent, iStateMachine.iCounterMax );
	iStateMachine.StopProgressBar();

	// TODO: FIX THIS! This is an ugly hack to give time for S60 DlMgr AOs to
	// finish their cleanups. To fix this properly, we need to wait till we get
	// "EHttpDlDeleting" event before starting the installation. This has to be
	// called before CancelAllDownloads(), otherwise DlMgrServer will die in
	// CBase91 PANIC.
	User::After(1000000);
	// We need to remove the download(s) before starting the installation.
	// Otherwise we might get KErrInUse error while installing. This happens
	// quite often when using E: / F: drive as the download drive.
	iStateMachine.iDownloadHandler->CancelAllDownloads();

	// Wait for the "DlDeleting" event
	iStateMachine.iDlDeletingSemaphore.Wait(KDlDeletingTimeout);

	iStateMachine.iInstallManager->SilentInstallPackageL(
			*iStateMachine.iDownloadedFile,
			iStateMachine.iCurrentPackage->GetInstallDrive()
		 );

	iStateMachine.UpdateProgressBar(iStateMachine.iCumulativeDepDownloaded,
			iStateMachine.iProgressMaxValue );
	}

void TStateInstallDependency::Exit() const
	{
	// Remove the wait note
#if 0
	if ( !iStateMachine.OviStoreRunning() )
		{
		iStateMachine.StopWaitNote();
		}
#endif
	}

void TStateInstallDependency::InstallCompleted(TInt aStatus)
	{
	LOG_FUNC

	CPackageInfo *curr = iStateMachine.iDepTree->GetCurrentFetchNode();
	User::LeaveIfNull(curr);
	SLOG3( "Removing '%S%S'", &iStateMachine.iDepTree->GetDownloadPath(), curr->GetSisPackageName() );

	// Delete the .sis file as its no longer required.
	iStateMachine.iDepTree->DeleteFile( iStateMachine.FsSession(),
					*curr->GetSisPackageName(),
					iStateMachine.iDepTree->GetDownloadPath()
					);
	if (aStatus == KErrNone)
		{
		if (curr->GetPackageStatus() == EPackageToBeFetched )
			{
			curr->SetPackageStatus(EPackageFetchedInstalled);
			}
		else if (curr->GetPackageStatus() == EPackageRootToBeInstalled )
			{
			curr->SetPackageStatus(EPackageRootInstalled);
			}
		else
			{
			SLOG2( "Unexpected package status %d", curr->GetPackageStatus() );
			}

		// Dependency package succesfully installed, update the counter to keep UI happy
		iStateMachine.iCounterCurrent++;
		// Download the next dependency
		iStateMachine.SetState( CStateFactory::EStateDownloadDependency );
		}
	else
		{
		SLOG2( "Dependency installation failed %d", aStatus );
		iStateMachine.SetFailureReason( EInstallationFailed );
		iStateMachine.SetState( CStateFactory::EStatePrepareExitWithError );
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TStateInstallAppSis::TStateInstallAppSis(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateInstallAppSis::Enter() const
	{
	// We need to find the root package
	if (iStateMachine.iCurrentPackage == NULL)
		{
		SLOG( "Finding root package" );
		CPackageInfo* package = NULL;
		while ( (package = iStateMachine.iDepTree->GetNextFetchNode()) != NULL )
			{
			if (package->GetPackageStatus() == EPackageRootToBeInstalled)
				{
				SLOG3( "Root: UID 0x%08X, '%S'", package->GetPackageUid(), package->GetPackageName() );
				break;
				}
			}
		iStateMachine.iCurrentPackage = package;
		}
	// iStateMachine.iCurrentPackage should now point to a correct packege,
	// the application.sis, in this case. Just make sure, it really does that.
	// Do we have a package set?
	if ( iStateMachine.iCurrentPackage )
		{
		if ( iStateMachine.BootstrapVersion() == 3 )
			{
/*
			SLOG( "Uninstalling smartinstaller (bootstrap)" );
			TInt err = iStateMachine.iInstallManager->SilentUnInstallPackage(
					iStateMachine.WrapperPackageUid() );
			SLOG2( "Uninstall status: %d", err);
*/
			// Delete the dependency file that was provided with the wrapper
			// TODO: Remove if(), DeleteFile() copes with NULL filenames.
			if ( iStateMachine.DepFilename() )
				{
				iStateMachine.iAppUi.DeleteFile( *iStateMachine.DepFilename() );
				}

#ifdef FEATURE_LAUNCH_INSTALLED_APP
			SLOG( "Dependencies and application are installed, ready to launch!" );
			iStateMachine.SetState( CStateFactory::EStateLaunchApp );
#else
			SLOG( "Dependencies and application are installed, ready to exit!" );
			iStateMachine.SetState( CStateFactory::EStateExit );
#endif
			}
		else
			{
			SLOG3( "* Installing AppSis 0x%08x '%S'", iStateMachine.iCurrentPackage->GetPackageUid(), iStateMachine.SisFilename() );

			// TODO: Leave or not to leave?
			//User::LeaveIfNull( iStateMachine.SisFilename() );

			// Update the progress bar
			iStateMachine.UpdateProgressBar( iStateMachine.iProgressMaxValue,
				iStateMachine.iProgressMaxValue );

			//First try silent install - if package is self-signed, this will fail
			//according to policy, so retry with a normal install.

			//User might not have been prompted for drive selection.
			//In that case, it is ok to prompt for drive selection as part of the Wrapper Package Installation.
			//This will be the case when files are injected into the Bootstrap import folder and not in a common "?:/ADM" drive.

			TChar installDrive = iStateMachine.SisFilename()->Des()[0];
			installDrive.UpperCase();
			if (installDrive < 'A' || installDrive > 'Z')
				{
				installDrive = KNoDriveSelected;
				}

			// If the wrapper installed drive is not selected, use the application.sis
			// destination drive as the installed drive. This way the developer can
			// control, if he/she wants to prompt the installation drive or force it
			// already in the wrapper/installer .pkg file. This can be controlled
			// by using '!' as the destination drive (=prompt) or by using a fixed
			// drive letter A->Z (=don't prompt).
			if ( iStateMachine.iWrapperInstalledDrive == KNoDriveSelected )
				{
				iStateMachine.iWrapperInstalledDrive = installDrive;
				}

			if ( iStateMachine.iWrapperInstalledDrive != KNoDriveSelected )
				{
				SLOG3( "Installing AppSis '%S' to %c:", iStateMachine.SisFilename(), (char)iStateMachine.iWrapperInstalledDrive );

				// First show the wait note and then remove progress bar.
				// MUCH less flicker this way.
				iStateMachine.ShowWaitNoteL( R_ADM_FINALIZING_INSTALLATION_WAIT_TEXT );
				iStateMachine.StopProgressBar();
				iStateMachine.iInstallManager->SilentInstallPackageL( *iStateMachine.SisFilename(), iStateMachine.iWrapperInstalledDrive );
				}
			else
				{
				SLOG( "! No installation drive selected, prompting" );
				// User will be prompted for the drive.

				//TODO: This is a hack. Change the Installcompleted logic to accomodate this.
				iStateMachine.StopWaitNote();
				iStateMachine.StopProgressBar();
				TInt aStatus = iStateMachine.iInstallManager->
						InstallPackage( *iStateMachine.SisFilename() );
				iStateMachine.SetNonSilentInstallation(ETrue);
				iStateMachine.HandleInstallCompletedL( aStatus );
				}
			} // if
		}
	else
		{
		// Hmm. We should've had a root package, but apparently we didn't. Odd.
		SLOG( "! Unexpected root package. PANIC" );
		PanicInState(EPanicStateUnexpectedRootPackage);
		}
	}

void TStateInstallAppSis::InstallCompleted(TInt aStatus)
	{
	LOG_FUNC

	//It is possible for silent installation of the main app to fail due to insufficient memory
	//TODO: would it be wise to move the below conditions to InstallManager and have some different error returned to statemachine?

	if ( ( aStatus == SwiUI::KSWInstErrSecurityFailure ) || ( aStatus == SwiUI::KSWInstErrInsufficientMemory ) )
		{
		// We have a wait note displayed. If silent install failed,
		// it needs to be removed so the installer UI won't be affected.
		iStateMachine.StopWaitNote();

		//SwiUI collapses the real error, Swi::ESignatureSelfSigned is what we should
		//be checking here. SwiUI::GetLastErrorL is not implemented
		//If the silent install fails, because of a self signed package, try again
		//with interactive install.
		// This is a synchronous call which returns the control here after
		// installation is finished (or cancelled, or an error occurs or..)
		aStatus = iStateMachine.iInstallManager->InstallPackage(
				*iStateMachine.SisFilename() );
		iStateMachine.SetNonSilentInstallation(ETrue);
		}
	else if ( aStatus == KErrNone )
		{
		// We end up here, if the silent installation completes succesfully
		iStateMachine.SetSilentInstallationOk(ETrue);
		}
	// Was installation succesful?
	if (aStatus == KErrNone)
		{
		CPackageInfo *curr = iStateMachine.iDepTree->GetCurrentFetchNode();
		User::LeaveIfNull(curr);
		//TODO: What if root package is set in HandleInstallComplete!!!
		// The next if should _always_ be true here
		if ( curr->GetPackageStatus() == EPackageRootToBeInstalled )
			{
			curr->SetPackageStatus(EPackageRootInstalled);
			}
		else if ( curr->GetPackageStatus() != EPackageRootInstalled )
			{
			SLOG2( "! Invalid root package status %d", curr->GetPackageStatus() );
			}

		CPackageInfo* fetchPackage = iStateMachine.iDepTree->GetNextFetchNode();

		// TODO: This is duplicated code from TStateDownloadDependency
		if ( !fetchPackage )
			{
/*
			//end of list - we're finished
			SLOG( "Uninstalling wrapper" );
			TInt err = iStateMachine.iInstallManager->SilentUnInstallPackage(
					iStateMachine.WrapperPackageUid() );
			SLOG2( "Uninstall status: %d", err);
			// Wrapper has been uninstalled, ready to exit
*/
#ifdef FEATURE_LAUNCH_INSTALLED_APP
			iStateMachine.SetState( CStateFactory::EStateLaunchApp );
#else
			iStateMachine.SetState( CStateFactory::EStateExit );
#endif
			return;
			}
		else
			{
			// We should be at the end of the list, but seems that we're not.
			SLOG2( "! Unexpected package 0x%08X. PANIC", fetchPackage->GetPackageUid() );
			PanicInState(EPanicStateUnexpectedPackage);
			}
		}
	else
		{
		const TExitReason reason = (aStatus == SwiUI::KSWInstErrUserCancel)
				? EUserCancelled
				: EInstallationFailed;
		iStateMachine.SetFailureReason( reason );
		iStateMachine.SetState( CStateFactory::EStatePrepareExitWithError );
		}
	}

// -----------------------------------------------------------------------------
// Confirms the user to continue the download when download threshold is exceeded
// -----------------------------------------------------------------------------
//
TStateConfirmDownload::TStateConfirmDownload(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateConfirmDownload::Enter() const
	{
	// Show download query as we are roaming
	iStateMachine.ShowDownloadQueryL( R_ADM_DL_PROMPT_TEXT );
	// the next state will get invoked by the user reply to the query
	}

void TStateConfirmDownload::UserAccepted()
	{
	// This is called when the user has selected "Yes" in the confirmation query
	iStateMachine.SetState( CStateFactory::EStateStartDependencyDownload );
	}

// -----------------------------------------------------------------------------
// Confirms the user to continue the download if device is roaming
// -----------------------------------------------------------------------------
//
TStateConfirmRoamingDownload::TStateConfirmRoamingDownload(const CStateFactory& aFactory) :
	TState(aFactory)
	{
	}

void TStateConfirmRoamingDownload::Enter() const
	{
	// Show download query as we are roaming
	iStateMachine.ShowDownloadQueryL( R_ADM_DL_PROMPT_TEXT, R_ADM_ROAMING_TEXT );
	// the next state will get invoked by the user reply to the query
	}

void TStateConfirmRoamingDownload::UserAccepted()
	{
	// This is called when the user has selected "Yes" in the confirmation query
	iStateMachine.SetState( CStateFactory::EStateStartDependencyDownload );
	}
