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
*     Declares UI class for application.
*
*
*/


#ifndef __ADMAPPUI_h__
#define __ADMAPPUI_h__

// INCLUDE FILES
#include <aknappui.h>
#include <aknglobalnote.h>
#include <aknglobalconfirmationquery.h>
#include <etelmm.h>
#include <flogger.h>
#include "progressdialog.h"
#include "globalwaitnote.h"
#include "globalqueryobserver.h"
#include "ADMDownloadHandler.h"
#include "ADMInstallManager.h"
#include "networkstatusobserver.h"

#include "debug.h"

_LIT(KADMPrivatePath, "c:\\private\\2002CCCE\\");
_LIT(KADMResumeInfoFile, "c:\\private\\2002CCCE\\resume");

// FORWARD DECLARATIONS
class CEikonEnv;
class CStateMachine;
class CDepTree;
class CXmlParser;
class CSisParser;
class CPackageInfo;
class CAknGlobalNote;
class CGlobalQueryObserver;
class CGlobalWaitNoteObserver;
class CNetworkStatusListener;

enum TExitReason
	{
	EExitNoError = 0,
	EUserCancelled,
	EDownloadFailed,
	EInsufficientMemory,
	EDeviceNotSupported,       // 5
	EInstallationFailed,
	EInvalidBootstrapVersion,
	ERootInstallationFailed,

	ELastExitState
	};

// Maximum length of the size prefix ("kB", "MB", "GB"). Must match resource
// file definition (TBUF16).
const TInt KMaxPrefixLen = 16;

// CLASS DECLARATION
/**
 * CADMAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CADMAppUi : public CAknAppUi,
	public CGlobalQueryObserver::MGlobalQueryClient,
	public CProgressDialog::MProgressDialogClient,
	public CGlobalWaitNote::MGlobalWaitNoteClient
	{
public:
	// Constructors and destructor

	/**
	 * ConstructL.
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CADMAppUi.
	 * C++ default constructor. This needs to be public due to
	 * the way the framework constructs the AppUi
	 */
	CADMAppUi();

	/**
	 * ~CADMAppUi.
	 * Virtual Destructor.
	 */
	virtual ~CADMAppUi();

	/**
	* Handle the Global Query Response
	*/
	void HandleGlobalQueryResponseL(const TInt aResponse);

	// Returns EikonEnv
	inline CEikonEnv* EikonEnv() const { return iEikonEnv; }

	// -------------------------------------------------------------------------
	// Interfaces for CStateFactory
	/**
	 * Displays proper error dialogs before starting the cleanups.
	 * This function MUST NOT LEAVE.
	 */
	void HandleFailure();

	/**
	 * Shows the final error message, if any, and exits the application.
	 */
	void ExitApp();

	/**
	 * Shows the progress bar.
	 */
	void ShowGlobalProgressL(const TInt aResourceId, const TInt aStartValue, const TInt aEndValue, const TInt aCurrent = -1, const TInt aLast = -1);

	/**
	 * Shows global error dialog with 'OK' CBA.
	 */
	void ShowGlobalErrorQueryL(const TInt aResourceId);

	/**
	 * Display global query seeking user permission
	 * to fetch dependencies from the web server.
	 */
	void ShowDownloadQueryL(const TInt aResourceId, const TInt aInfoResourceId = -1);

	/**
	 * Shows the application launch confirmation prompt.
	 */
	void ShowLaunchPromptL(const TDesC& aAppName, const TDesC& aIconFilename = KNullDesC);

	/**
	 * Deletes a file pointed by aFilename.
	 */
	void DeleteFile(const TDesC& aFilename);

	// -------------------------------------------------------------------------

private:
	// Functions from base classes

	/**
	 * From CEikAppUi, HandleCommandL.
	 * Takes care of command handling.
	 * @param aCommand Command to be handled.
	 */
	void HandleCommandL(TInt aCommand);

	/**
	 * Window server event. Handles key presses.
	 */
	void HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination );

	//From MProgressDialogClient
	void ProgressDialogCancelled();
	void WaitNoteCancelled();

	TInt ShowGlobalNoteL( const TAknGlobalNoteType aType , const TInt aResourceId );
	TInt ShowExitNoteL();
	void ShowWaitNoteL( const TInt aResourceId, const TBool aCancellable, const TInt aDetailedResourceId = -1 );
	void ShowWaitNoteNumL( const TInt aResourceId, const TInt aCurrent, const TInt aLast );
	void CancelWaitNote();
	void CancelProgressBar();
	void ProcessCommandLineArgsL();

	/**
	 * Verifies available disk space.
	 *
	 * Returns true, if enough disk space is available
	 */
	TBool VerifyAvailableDiskSpaceL();

	/**
	 * Pretty prints aSize to aDescr and prefixes it with kB, MB or GB
	 * depending on the value of aSize.
	 */
	void PrettyPrint(const TUint32 aSize, TDes &aDescr);

	/**
	 * Reads the resources.
	 *
	 * Leaves with standard error codes, if it encounters errors.
	 */
	void ReadStaticResourcesL();

	/**
	 * Loads required icons.
	 */
	void LoadIconsL();

	/**
	 * Reads the configuration file.
	 *
	 * @return KErrNone if all succesful, error code otherwise
	 */
	TInt ReadConfigurationL();

	/**
	 * Returns the phone information
	 *
	 * @return KErrNone, if phone information was succesfully retrieved, error number otherwise.
	 */
	TInt GetPhoneInformation();

	/**
	 * Returns roaming status.
	 *
	 * @return ETrue, if phone is roaming
	 */
	TBool RegisteredRoaming();

	/**
	 * Returns the phone memory drive.
	 *
	 * @return Phone memory drive
	 */
	TInt PhoneMemoryDrive();

	/**
	 * Returns the memory card drive.
	 *
	 * @return Memory card drive
	 */
	TInt MemoryCardDrive();

	/**
	 * Returns true, if the user selected IAP is a WLAN access point.
	 */
	TBool UsingWLAN();

	/**
	 * Returns the Bearer type corresponding to the selected IAP Id.
	 *
	 * @return Bearer type
	 */
	TInt BearerType();

	/**
	 * Read the bootstrap version and file names from the resume file.
	 *
	 * @return Standard Symbian error code
	 */
	TInt ReadResumeInfoL();

private:
	friend class CStateMachine;
	friend class CDownloadHandler;

	/**
	 * State machine running the application logic.
	 */
	CStateMachine*     iStateMachine;

	/**
	 * Application exit reason
	 */
	TExitReason iExitReason;

	/**
	 * Silent installation completion status
	 */
	TBool iSilentInstallationOk;

	/**
	 * Set to ETrue, if non-silent installation was used. Used to hide
	 * our "Installation complete" note.
	 */
	TBool iNonSilentInstallation;

	CProgressDialog* iProgress;
	CGlobalWaitNote* iWaitNote;
	CAknGlobalConfirmationQuery* iGlobalConfirmationQuery;
	CGlobalQueryObserver* iGlobalQueryObserver;

	/**
	 * Full path to the application icon we're installing. Provided in the
	 * installer/wrapper package.
	 */
	HBufC* iAppIconFilename;

	/**
	 * Holds the current string shown in the query dialog
	 */
	HBufC* iQueryMessage;

	/**
	 * Path to dependency name. Given as a command line parameter.
	 */
	HBufC* iDepFileName;

	/**
	 * Path to the application sis file. Given as a command line parameter.
	 */
	HBufC* iSisFileName;

	/**
	 * The application name to be installed. Read from the resume file,
	 * not available if not resuming the installation.
	 */
	HBufC* iMainAppName;

	/**
	 * Bootstrap version that was used to launch ADM.
	 */
	TInt iBootstrapVersion;

	/**
	 * IAP for the download manager. We try to utilise any existing connection.
	 * If that is not available, browser default IAP will be used, if set.
	 * If that is not available, user will be prompted for the IAP.
	 */
	TUint32 iIAP;

	/**
	 * URL to server stored to config.ini. If configuration file does not exist,
	 * default hard-coded (KDefaultDepServerName) URL is used.
	 */
	HBufC8* iConfigUrl;

	/**
	 * The wrapper package UID. This package will be uninstalled
	 * after succesful installation as it is not required any longer.
	 */
	TUid iWrapperPackageUid;

	/**
	 * Handle to file server we got from CEikonEnv::Static()->FsSession().
	 */
	RFs iRfs;

	/**
	 * Telephony Server (etelmm).
	 */
	RTelServer iTelServer;

	/**
	 * Phone Object
	 */
	RMobilePhone iPhone;

	/**
	 * Software version string
	 */
	HBufC* iSwVersion; ///< owned

	/**
	 * Machine UID
	 */
	TInt iMachineUid;

	/**
	 * True, if resuming installation
	 */
	TBool iResumingInstallation;

	/**
	 * True, if installation resume information is required
	 */
	TBool iIsResumeRequired;

	/**
	 * True, if Ovi Store client is running
	 */
	TBool iOviStoreRunning;

	/**
	* True, if App Launch is possible.
	 */
	TBool iAppLaunch;

#ifdef _DEBUG
	/**
	 * Manufacturer ID as described by EHalData.
	 */
	TInt iManufacturer;

	/**
	 * Model ID
	 */
	TInt iModel;

	/**
	 * Hardware revision
	 */
	TInt iHardwareRev;

	/**
	 * Software revision
	 */
	TInt iSoftwareRev;

	/**
	 * Device family
	 */
	TInt iDeviceFamily;

	/**
	 * Device family revision
	 */
	TInt iDeviceFamilyRev;

	/**
	 * CPU architecture
	 */
	TInt iCpuArch;

	/**
	 * CPU ABI
	 */
	TInt iCpuABI;

	/**
	 * CPU speed
	 */
	TInt iCpuSpeed;
#endif

	/**
	 * Localised resource strings for "kB", "MB" and "GB" used by PrettyPrint().
	 */
	TBuf<KMaxPrefixLen> iPrefixKb;
	TBuf<KMaxPrefixLen> iPrefixMb;
	TBuf<KMaxPrefixLen> iPrefixGb;

	INIT_OBJECT_DEBUG_LOG
	};

#endif // __ADMAPPUI_h__

// End of File
