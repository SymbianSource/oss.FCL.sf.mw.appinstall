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
* Declaration of CStateMachine class.
*
*
*/


#include <e32base.h>
#include <downloadmgrclient.h>
#include <swinstapi.h>

#include "bootstrapappui.h"
#include "globals.h"
#include "debug.h"

// Mandatory changes XML elements
_LIT8(KFieldVersion, "Version");
_LIT8(KFieldSisName, "SisFileName");
// Optional changes XML elements
_LIT8(KFieldUrl, "Url");

// SmartInstaller bootstrap states
enum EBSAppState
	{
	EBSStateInit = 0,		// 0
	EBSStateSetup,
	EBSStateDownloadChanges,
	EBSStateParseChanges,
	EBSStateDownloadAdm,
	EBSStateInstallAdm,		// 5
	EBSStateLaunchAdm,
	EBSStateExitWithError,
	EBSStateExit,
	EBSStateLast			// not actual state
	};

enum EXmlParserError
	{
	EXmlErrorNotWelformed,
	EXmlErrorMissingVersion,
	EXmlErrorInvalidVersion,
	EXmlErrorMissingSisFileName,
	};

struct TAdmChangesInfo
	{
TAdmChangesInfo():
	iVersion(0,0,0),
	iSisName(NULL),
	iUrl(NULL)
	{}
~TAdmChangesInfo()
	{
	delete iSisName;
	delete iUrl;
	}

	TVersion iVersion;
	HBufC* iSisName;
	HBufC8* iUrl;
	};

// Forward declarations
class CInstallObserver;
class RConnectionMonitor;
class CHelper;

class CStateMachine : public CActive,
	public MHttpDownloadMgrObserver
	{
public:
	static CStateMachine* NewL(CBootstrapAppUi& aSIUiObj, TUint32 aWrapperUid);
	static CStateMachine* NewLC(CBootstrapAppUi& aSIUiObj, TUint32 aWrapperUid);

	/**
	 * ~CStateMachine.
	 * Virtual Destructor.
	 */
	virtual ~CStateMachine();

	/**
	 * Start the state machine
	 */
	void Start();

	/**
	 * Stop the state machine
	 */
	void Stop();

	/**
	 * HandleWaitNoteCancel
	 * Called by AppUi on User Press Cancel
	 */
	void HandleWaitNoteCancel();

	/**
	 * HandleInstallCompleteL
	 * Called on installation complete by Installation Observer
	 */
	void HandleInstallCompleteL(const TInt& aStatus);

	/**
	 * HandleDMgrEventL
	 * Handle download manager events.
	 */
	void HandleDMgrEventL(RHttpDownload& aDownload, THttpDownloadEvent aEvent );

	/**
	 * HandleFailure
	 * Perform the state specific cleanup in case of a failure
	 */
	void HandleFailure(TBool aUserCancel = EFalse);

private:
	/**
	 * CStateMachine.
	 * C++ default constructor.
	 */
	CStateMachine(CBootstrapAppUi& aSIUiObj, TUint32 aWrapperUid);

	/**
	 * ConstructL.
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * RunL
	 * Called for each state transition
	 */
	void RunL();

	/**
	 * RunError
	 * Called incase of an error
	 */
	TInt RunError(TInt aError);

	/**
	 * DoCancel
	 *
	 */
	void DoCancel();

	/**
	 * SetNextState
	 * Transition to the next state
	 */
	void RequestState(const EBSAppState aNextState);

	/**
	 * StateInitL
	 * Set the sisfilename and dep filename to be passed to ADM
	 */
	void StateInitL();

	/**
	 * StateDownloadL
	 * Download the specified file.
	 */
	void StateDownloadL();

	/**
	 * StateParseChangesL
	 * Parse the ADM changes file
	 */
	void StateParseChangesL();

	/**
	 * StateInstallAdmL
	 * Installs the specified package
	 */
	void StateInstallAdmL();

	/**
	 * StateLaunchAdmL
	 * Launch ADM
	 */
	void StateLaunchAdmL();

	/**
	 * IsAdmDownloadRequired
	 * Check whether ADM sis needs to be downloaded
	 */
	TBool IsAdmDownloadRequired();

	/**
	 * DeleteFile
	 * Delete the given file
	 */
	void DeleteFile(const TDesC& aFileName);

	/**
	* IsPackagePresentL
	* Check if the ADM is already present in the device or not
	*/
	TBool IsPackagePresentL(const TUid& aUid, TVersion& aVersion);

	/**
	 * ReadConfigurationL
	 * Read the config file
	 */
	TInt ReadConfigurationL();

	/**
	 * ProcessDlErrors
	 * Identify the download error
	 */
	TBool ProcessDlErrors(const TInt32 aErrId, const TInt32 aGlErrId, EDownloadError& aDownloadError);

	/**
	 * Returns ETrue, if Ovi Store client is running
	 */
	inline TBool OviStoreRunning() const { return iAppUi.iOviStoreRunning; }

#ifdef FEATURE_BOOTSTRAP_SETIAP
	void SetIAP(const TUint aIAP);
	void SetAndValidateIAPL();
	void GetSnapIapsL(const TUint aSNAPId);
	TBool IsIapActive(RConnectionMonitor& aMonitor, TInt& aIAP);
#endif

private:
	CBootstrapAppUi& iAppUi;

	/**
	 * ADM installation status on the device
	 */
	TBool iIsAdmInstalled;

	/**
	 * Current application state
	 */
	EBSAppState iAppState;

	/**
	 * Wrapper package UID. Passed to ADM.
	 */
	TUint32 iWrapperPackageUid;

	/**
	 * IAP for Download Manager. Passed to ADM.
	 */
	TInt32 iIAP;

	/**
	 * IAP array for the browser SNAP.
	 */
	RArray<TUint32> iIapArray;

	/**
	 * IAP array index. Points to the IAP currently in use.
	 */
	TUint32 iIapArrayIndex;

	/**
	 * Dep filename. Passed to ADM.
	 */
	HBufC* iDepFilename;

	/**
	 * Main application SIS filename. Passed to ADM.
	 */
	HBufC* iSisFilename;

	/**
	 * ADM changes info, filled by parsing downloaded adm_changes.xml
	 */
	TAdmChangesInfo iAdmChangesInfo;

	/**
	 * Bootstrap failure status. Passed to ADM.
	 */
	 TBool iBootstrapFailed;

	/**
	 * IAP cancel flag.
	 */
	TBool iIsIapCancelled;

	/**
	 * URL provided by the configuration file to the server root or
	 * the default URL, if config file is missing.
	 */
	HBufC8* iConfigUrl;

	/**
	 * URL
	 */
	HBufC8* iUrl;

	/**
	 * File to be downloaded
	 */
	TPtrC iFileName;

	/**
	 * Url to download from.
	 */
	HBufC8* iDownloadUrl;

	/**
	 * Downloaded filename
	 */
	HBufC* iDownloadFileName;

	/**
	 * Version of ADM present in the device.
	 */
	TVersion iAdmInstalledVersion;

	/*
	 * DownloadManager
	 */
	RHttpDownloadMgr iDownloadManager;

	/**
	 * SwiUI launcher
	 */
	SwiUI::RSWInstSilentLauncher iLauncher;

	/**
	 * Options for Swi Launcher
	 */
	SwiUI::TInstallOptionsPckg iOptionsPckg;

	/**
	 * Current installation attempt
	 */
	TInt iInstallAttempt;

	/**
	 * Track the download State - mainly for logging.
	 */
	TInt iDownloadState;

	/**
	 * Track the Progress State of a download - mainly for logging.
	 */
	TInt iProgressState;

	/**
	 * Current connnection reattempt number.
	 */
	TInt iConnectionAttempt;

	/**
	 * Specific download error.
	 */
	EDownloadError iDownloadError;

	CInstallObserver* iInstallObsrv; //< owns

#ifdef USE_LOGFILE
	RFileLogger& iLog; //< reference to iAppUi.iLog
#endif
};
