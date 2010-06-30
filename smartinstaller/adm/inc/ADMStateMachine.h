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
*     State machine
*
*
*/


#ifndef ADMSTATEMACHINE_H_
#define ADMSTATEMACHINE_H_

#include <e32base.h>
#include <e32debug.h>

#include "ADMAppUi.h"
#include "ADMInstallManager.h"
#include "ADMPackageInfo.h"
#include "ADMSisParser.h"
#include "ADMXmlParser.h"

// Forward declarations
class TState;
class CStateMachine;

enum TStatePanic
	{
	EPanicStateIndexOutOfBounds = 1000,
	EPanicStateImplementationMissing,
	EPanicStateInvalidBootstrapVersion,
	EPanicStateInvalidMachineUid,
	EPanicStateFetchStatus,
	EPanicStateFetchPackageNull,
	EPanicStateUnexpectedPackage,
	EPanicStateUnexpectedRootPackage,
	EPanicStateWrapperNotInstalled
	};

const TInt KUnknownState = -40000;

// Timeout for waiting for download deletion. Default 2 s.
const TInt KDlDeletingTimeout = 2*1000*1000;

class CStateFactory : public CBase
	{
public:
	enum TAppState
		{
		EStateSetup = 0,                   // 0
		EStateSetupParsing,
		EStateBuildDepTree,
		EStateDownloadChangesFile,
		EStateParseChangesFile,
		EStateDownloadDepFile,             // 5
		EStateParseDepFile,
		EStateBuildFetchList,
		EStateVerifyAvailableDiskSpace,
		EStateConfirmDownload,
		EStateConfirmRoamingDownload,      // 10
		EStateStartDependencyDownload,
		EStateDownloadDependency,
		EStateInstallDependency,
		EStateInstallAppSis,
		EStateUninstallDependency,         // 15
		EStatePrepareExitWithError,
		EStateLaunchApp,
		EStateExit,

		EStateLast
		};

	TState& GetState(const TAppState aState) const;
	TInt StateIndex(const TState* aState) const;

	inline CStateMachine& StateMachine() const { return iStateMachine; }

	static CStateFactory* NewL(CStateMachine& aStateMachine);
	~CStateFactory();

private:
	CStateFactory(CStateMachine& aStateMachine);
	void ConstructL();

private:
	TFixedArray<TState*, EStateLast> iStates;
	CStateMachine&   iStateMachine;
	};

// Base class for a state
class TState
	{
public:
	//friend class CStateFactory;
	//friend class CStateMachine;

	TState(const CStateFactory& aFactory);

	// Change of states
	virtual void Enter() const;
	virtual void Exit() const;

	// Events from UI
	virtual void UserAccepted();
	virtual void UserRejected();
	virtual void UserCancelled();

	// Events from DownloadManager
	virtual void DownloadInProgress(const TInt aAlreadyDownloaded, const TInt aDownloadSize, const TReal32 aAvgDlSpeed);
	virtual void DownloadCompleted(const TInt aBytesDownloaded);
	virtual void DownloadFailed();
	virtual void DownloadCancelled();

	// Events from InstallManager
	virtual void InstallInProgress();
	virtual void InstallCompleted(TInt aStatus);
	virtual void InstallationCancelled();

protected:
	void DownloadFailed(const CStateFactory::TAppState aState);
	void PanicInState(TStatePanic aPanic) const;

protected:
	const CStateFactory& iFactory;
	CStateMachine& iStateMachine;
	};

// ----------------------------------------------------------------------------
class TStateSetup : public TState
	{
public:
	TStateSetup(const CStateFactory& aFactory);

	void Enter() const;
	};
// ----------------------------------------------------------------------------
class TStateSetupParsing : public TState
	{
public:
	TStateSetupParsing(const CStateFactory& aFactory);

	void Enter() const;
	};
// ----------------------------------------------------------------------------
class TStateDownloadDepFile : public TState
	{
public:
	TStateDownloadDepFile(const CStateFactory& aFactory);

	void Enter() const;

	void DownloadCompleted(const TInt aBytesDownloaded);
	void DownloadFailed();
	};
// ----------------------------------------------------------------------------
class TStateParseDepFile : public TState
	{
public:
	TStateParseDepFile(const CStateFactory& aFactory);

	void Enter() const;
	};
// ----------------------------------------------------------------------------
class TStateBuildDepTree : public TState
	{
public:
	TStateBuildDepTree(const CStateFactory& aFactory);

	void Enter() const;
	};
// ----------------------------------------------------------------------------
class TStateDownloadChangesFile : public TState
	{
public:
	TStateDownloadChangesFile(const CStateFactory& aFactory);

	void Enter() const;

	void DownloadCompleted(const TInt aBytesDownloaded);
	void DownloadFailed();
	};
// ----------------------------------------------------------------------------
class TStateParseChangesFile : public TState
	{
public:
	TStateParseChangesFile(const CStateFactory& aFactory);

	void Enter() const;
	};
// ----------------------------------------------------------------------------
class TStateBuildFetchList : public TState
	{
public:
	TStateBuildFetchList(const CStateFactory& aFactory);

	void Enter() const;
	};
// ----------------------------------------------------------------------------
class TStateVerifyAvailableDiskSpace : public TState
	{
public:
	TStateVerifyAvailableDiskSpace(const CStateFactory& aFactory);

	void Enter() const;
	};
// ----------------------------------------------------------------------------
class TStateConfirmDownload : public TState
	{
public:
	TStateConfirmDownload(const CStateFactory& aFactory);

	void Enter() const;
	void UserAccepted();
	};
// ----------------------------------------------------------------------------
class TStateConfirmRoamingDownload : public TState
	{
public:
	TStateConfirmRoamingDownload(const CStateFactory& aFactory);

	void Enter() const;
	void UserAccepted();
	};
// ----------------------------------------------------------------------------
class TStateStartDependencyDownload : public TState
	{
public:
	TStateStartDependencyDownload(const CStateFactory& aFactory);

	void Enter() const;
	};
// ----------------------------------------------------------------------------
class TStateDownloadDependency : public TState
	{
public:
	TStateDownloadDependency(const CStateFactory& aFactory);

	void Enter() const;

	void DownloadCompleted(const TInt aBytesDownloaded);
	void DownloadInProgress(const TInt aAlreadyDownloaded, const TInt aDownloadSize, const TReal32 aAvgDlSpeed);
	void DownloadFailed();

private:
	TBool iRetryDownload; // Set to true, if we're retrying download
	};
// ----------------------------------------------------------------------------
class TStateInstallDependency : public TState
	{
public:
	TStateInstallDependency(const CStateFactory& aFactory);

	void Enter() const;
	void Exit() const;
	void InstallCompleted(TInt aStatus);
	};
// ----------------------------------------------------------------------------
class TStateInstallAppSis : public TState
	{
public:
	TStateInstallAppSis(const CStateFactory& aFactory);

	void Enter() const;
	void InstallCompleted(TInt aStatus);
	};
// ----------------------------------------------------------------------------
class TStateUninstallDependency : public TState
	{
public:
	TStateUninstallDependency(const CStateFactory& aFactory);

	void Enter() const;
	void InstallCompleted(TInt aStatus);
	};
// ----------------------------------------------------------------------------
class TStatePrepareExitWithError : public TState
	{
public:
	TStatePrepareExitWithError(const CStateFactory& aFactory);

	void Enter() const;

private:
	TInt CreateInstallationResumeInfo() const;
	void BackupFilesL() const;
	};
// ----------------------------------------------------------------------------
class TStateLaunchApp : public TState
	{
public:
	TStateLaunchApp(const CStateFactory& aFactory);

	void Enter() const;
	void UserAccepted();
	void UserRejected();
};
// ----------------------------------------------------------------------------
class TStateExit : public TState
	{
public:
	TStateExit(const CStateFactory& aFactory);

	void Enter() const;

private:
	void RemoveResumeFiles() const;

#ifdef FEATURE_REPORTING
	void Report() const;

public:
	void DownloadInProgress(const TInt /* aAlreadyDownloaded */, const TInt /* aDownloadSize */, const TReal32 /* aAvgDlSpeed */);
	void DownloadCompleted(const TInt /* aBytesDownloaded */);
	void DownloadFailed();
	void DownloadCancelled();
#endif
	};

// ----------------------------------------------------------------------------
class CStateMachine : public CActive,
	public CInstallManager::MInstallClient,
	public CDownloadHandler::MDownloadClient
	{
public:
	friend class TState;
	friend class TStateBuildDepTree;
	friend class TStateBuildFetchList;
	friend class TStateConfirmDownload;
	friend class TStateConfirmRoaming;
	friend class TStateDownloadChangesFile;
	friend class TStateDownloadDepFile;
	friend class TStateDownloadDependency;
	friend class TStateExit;
	friend class TStateInstallAppSis;
	friend class TStateInstallDependency;
	friend class TStateLaunchApp;
	friend class TStateParseChangesFile;
	friend class TStateParseDepFile;
	friend class TStateParseSisFile;
	friend class TStatePrepareExitWithError;
	friend class TStateSetup;
	friend class TStateSetupParsing;
	friend class TStateStartDependencyDownload;
	friend class TStateUninstallDependency;
	friend class TStateVerifyAvailableDiskSpace;
	friend class TStateWaitForUserConfirmation;

	static CStateMachine* NewL(CADMAppUi& aAppUi, const TDesC& aDownloadPath);
	~CStateMachine();

	void Start();
	void Stop();
	void SetStartState(const CStateFactory::TAppState aState);
	void SetState(const CStateFactory::TAppState aState);
	TInt StateIndex() const;

	// Interfacing for ADM
	// Called when user has made a selection in a query dialog
	void SetIAP(TUint32 aIAP);
	void HandleUserResponse(TInt aResponse);
	CStateFactory::TAppState FailedState() const { return iFailedState; }
	inline TInt32 TotalDownloadSize() const { return iDepTree->GetTotalDownloadSize(); }
	inline HBufC* LaunchCaption() const
		{
			CPackageInfo* root = iDepTree->GetRootNode();
			if (root)
				return root->GetPackageName();
			else
				return NULL;
		}

#ifdef USE_LOGFILE
	RFileLogger& Log() const { return iAppUi.iLog; }
#endif

	// Interface to ADM
	inline TBool ResumeRequired() const { return iAppUi.iIsResumeRequired; }
	inline TInt MachineUid() const { return iAppUi.iMachineUid; }
	inline TInt BootstrapVersion() const { return iAppUi.iBootstrapVersion; }
	inline TBool RegisteredRoaming() const { return iAppUi.RegisteredRoaming(); }
	inline TBool UsingWLAN() const { return iAppUi.UsingWLAN(); }
	inline TBool OviStoreRunning() const { return iAppUi.iOviStoreRunning; }
	inline TBool ResumingInstallation() const { return iAppUi.iResumingInstallation; }
	inline HBufC* DepFilename() const { return iAppUi.iDepFileName; }
	inline HBufC* SisFilename() const { return iAppUi.iSisFileName; }
	inline HBufC8* ConfigUrl() const { return iAppUi.iConfigUrl; }
	inline TUid& WrapperPackageUid() const { return (TUid&)iAppUi.iWrapperPackageUid; }
	inline RFs& FsSession() const { return iAppUi.EikonEnv()->FsSession(); }
	inline void SetSilentInstallationOk(const TBool aOk) { iAppUi.iSilentInstallationOk = aOk; }
	inline void SetNonSilentInstallation(const TBool aVal) { iAppUi.iNonSilentInstallation = aVal; }
	inline void SetAppLaunch(const TBool aSuccess) { iAppUi.iAppLaunch = aSuccess; }
	inline TBool AppLaunch() const { return iAppUi.iAppLaunch; }
	inline TInt ExitReason() const { return iAppUi.iExitReason; }
	void UpdateProgressBar(const TInt aStartValue, const TInt aEndValue);
	void StopProgressBar() const;
	void SetFailureReason(TExitReason aReason);
	void ShowDownloadQueryL(const TInt aResourceId, const TInt aInfoResourceId = -1);
	void ShowWaitNoteL(const TInt aResourceId);
	void ShowInstallingWaitNoteL(const TInt aCurrent, const TInt aLast);
	void ShowProgressBarL(const TInt aStartValue, const TInt aEndValue, const TInt aCurrent = -1, const TInt aLast = -1);
	void StopWaitNote();

protected:
	void RunL();
	void DoCancel();
	TInt RunError(const TInt aError);

	void SetState(TState& aState);

	//From MInstallClient
	void HandleInstallCompletedL(TInt aStatus);

	//From MDownloadClient
	/**
	 * Called by Download Manager when IAP has changed.
	 * @param aIAP IAP ID
	 */
	void HandleIapChanged(const TUint32 aIAP);
	/**
	 * Called by Download Manager when downloading of a packet is completed.
	 * @param aDlFileName Name of the downloaded file.
	 * @param aResponseType Download type.
	 * @param aDownloadSize Total number of bytes downloaded.
	 */
	void HandleHttpFetchCompleted( const TDesC& aDlFilename, const TInt aDownloadType, const TInt32 aBytesDownloaded, const TReal32 aAvgDlSpeed, const TInt64 aDlTime );

	/**
	 * Called when download manager has retrieved a packet from the network.
	 * Required for updating the progress bar.
	 */
	void HandleHttpFetchInProgress(const TInt32 aAlreadyDownloaded, const TInt32 aDownloadSize, const TReal32 aAvgDlSpeed);

	/**
	 * Called when download manager encounters a problem while downloading
	 */
	void HandleHttpFetchFailure( const TDesC& aDlFilename, const TInt aGlobalErrorId, const TInt aErrorId, const TInt aFetchType );

	/**
	 * Called when download manager is deleting the download.
	 */
	void HandleDownloadDeleting();

	// State machine required functions
	//void HandleFailure();
	//TBool VerifyAvailableDiskSpace();
	//void SilentInstallDownloadedPackageL();

private:
	CStateMachine(CADMAppUi& aAppUi);
	void ConstructL(const TDesC& aDownloadPath);

private:
	CADMAppUi&			iAppUi;
	CStateFactory*		iStateFactory;     //< owns
	CInstallManager*	iInstallManager;   //< owns
	CDownloadHandler*	iDownloadHandler;  //< owns

	CXmlParser*			iXmlParser; //< owns
	CSisParser*			iSisParser; //< owns
	CDepTree*			iDepTree;   //< owns
	CPackageInfo*		iCurrentPackage;

	/**
	 * Path to the downloaded file. Updated after each finishing each
	 * download (completed or failed).
	 */
	HBufC*              iDownloadedFile;   //< owns

	/**
	 * Current state.
	 */
	TState*             iState;

	/**
	 * The state that experienced failure that causes ADM to exit.
	 */
	CStateFactory::TAppState    iFailedState;

	/**
	 * Number of dependency tree nodes processed so far.
	 */
	TInt   iNodesProcessed;

	/**
	 * Progress bar current value.
	 */
	TInt32 iProgressCurrentValue;

	/**
	 * Progress bar maximum value. Stays constant after being set.
	 */
	TInt32 iProgressMaxValue;

	/**
	 * Current package # being downloaded/installed.
	 */
	TInt iCounterCurrent;

	/**
	 * Total number of packages being downloaded/installed.
	 */
	TInt iCounterMax;

	/**
	 * Total size of the packages to be downloaded in bytes.
	 * Used in progress bar update.
	 */
	TInt32 iDownloadSize;

	/**
	 * Downloaded bytes so far.
	 * Used in progress bar update.
	 */
	TInt32 iCumulativeDepDownloaded;

	/**
	 * Total number of bytes downloaded during session. Includes all
	 * dependency XML file downloads and .sis file downloads.
	 */
	TInt32 iCumulativeDownloaded;

	/**
	 * Time in microseconds used in downloading, used for calculating average download speed.
	 */
	TInt64 iDownloadTime;

	/**
	 * Drive in which wrapper is installed.
	 */
	TChar iWrapperInstalledDrive;

	/**
	 * Restart Attempt for Download Failure
	 */
	TInt iRestartAttempt;

#ifdef FEATURE_LAUNCH_INSTALLED_APP
	/**
	 * App Launch name
	 */
	TFileName iLaunchName;
	TFileName iLaunchIconFilename;
	TApaAppCaption iLaunchCaption;
#endif

	/**
	 * Semaphore for signalling download deleting.
	 */
	RSemaphore iDlDeletingSemaphore;
	};

#endif /* ADMSTATEMACHINE_H_ */
