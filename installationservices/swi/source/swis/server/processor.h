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
 @released
 @internalTechnology 
*/

#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <e32base.h>
#include <f32file.h>

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/sts/sts.h>
#include "registrywrapper.h"
#else
#include "sisregistrywritablesession.h"
#endif

#include "sisregistryfiledescription.h"
#include "sisinstallerrors.h"
#include "sistruststatus.h"
#include "securitypolicy.h"
#include "swiobserverclient.h"

namespace Swi
{
const TInt KMaxNoOfDeletionAttempts=3;
const TInt KRetryInterval=250000;
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
class CIntegrityServices;
#endif
class CApplication;
class CSecurityManager;
class RSisHelper;
class RUiHandler;
class CHashContainer;
class CSisRegistryFileDescription;
class CPlan;

/**
 * This class processes a CApplication created by the installation planner
 * @released
 * @internalTechnology 
 */
class CProcessor : public CActive
	{
protected:
	enum TProcessingState
		{
		EInitialize,
		EProcessEmbedded,
		EShutdownAllApps,
		ECheckApplicationInUse,
		EShutdownExe,
		ERemoveFiles,
		EExtractFiles,
		EProcessFiles,
		EVerifyPaths,
		ERemovePrivateDirectories,
		EInstallFiles,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		EParseApplicationRegistrationFiles,
#endif		
		EDisplayFiles, 
		ERunFiles,
		EUpdateRegistry,
		EFinished,
		EProcessSkipFiles,
		ECurrentState=128, // Used to change to the current state
		};

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	CProcessor(const CPlan& aPlan, RUiHandler& aUiHandler, 
			Usif::RStsSession& aStsSession, CRegistryWrapper& aRegistryWrapper, 
			RSwiObserverSession& aObserver);
#else
	CProcessor(const CPlan& aPlan, RUiHandler& aUiHandler, 
			CIntegrityServices& aIntegrityServices, RSwiObserverSession& aObserver);
#endif

	virtual ~CProcessor();

	void ConstructL();			

// from CActive
	virtual TInt RunError(TInt aError);
	virtual void DoCancel();
	virtual void RunL();

// interface for the processors derived classes to implement to do actual processing
	virtual TBool DoStateInitializeL() = 0;
	virtual TBool DoStateProcessEmbeddedL() = 0;
	virtual TBool DoStateShutdownAllAppsL();
	virtual TBool DoStateCheckApplicationInUseL();
	virtual TBool DoStateShutdownExeL();
	virtual TBool DoStateExtractFilesL() = 0;
	virtual TBool DoStateVerifyPathsL() = 0;
	virtual TBool DoStateProcessFilesL();
	virtual TBool DoStateInstallFilesL() = 0;
	virtual TBool DoStateDisplayFilesL() = 0;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	virtual TBool DoParseApplicationRegistrationFilesL() = 0;
#endif	
	virtual TBool DoStateUpdateRegistryL() = 0;
	virtual TBool DoStateRemoveFilesL();
	virtual TBool DoStateRemovePrivateDirectoriesL();
	virtual void DoStateFinishedL();
	virtual TBool DoStateProcessSkipFilesL() = 0;
	virtual void Reset();
	
// accessors
	const CPlan& Plan() const;
	RUiHandler& UiHandler();
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	Usif::RStsSession& TransactionSession();
#else
	CIntegrityServices& IntegrityServices();
#endif
	RFs& Fs();
	const CApplication& ApplicationL() const;

// utility functions

	/**
	 * Runs files from the target location specified in the SISX file.
	 * @param aFileDescription - the file to run
	 */
	void RunFileL(const TDesC& aFileName, const TDesC& aMimeType, Sis::TSISFileOperationOptions aFileOperationOption);

	/**
	 * Uses transaction support to first backup then remove old files
	 * @param aFileToRemove - the file to remove
	 */
	void RemoveFileL(const CSisRegistryFileDescription& aFileDescription);

	/**
	 * Uses transaction support to first backup then remove the private directory
	 * @param aFileToRemove - the sid of the private directory to remove
	 */
	void RemovePrivateDirectoryL(TUid aSid);
	
	TBool DisplayApplicationInUseL();
	
	RSwiObserverSession& Observer();
	
public:
	
	void ProcessPlanL(TRequestStatus& aClientStatus);
	
	void ProcessApplicationL(const CApplication& aApplication, TRequestStatus& aClientStatus);

	/**
	 * Runs the RBS files from the target location specified in the SISX file.
	 *  
	 */
	void RunBeforeShutdown();

protected:

	/**
	 * Switches state of machine.
	 */
	void SwitchState(TProcessingState aNextState);
		
	/**
	 * Sets ourselves as active. Used to wait for a request to an external async call
	 * to complete, before going to the next state.
	 */
	void WaitState(TProcessingState aNextState);

protected:
	/**
	 * A list of executable sids needed to verify all the files are being copied
	 * to private directories of executables which are being installed.
	 */
	RArray<TUid> iSidsAdded;

	TProcessingState iUiState;	
	
	/// The drive on which to store device integrity data (hashes, registry etc) 
	TChar iSystemDriveChar;
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/// Wrapper object to access sisregistry for mutable operations
	CRegistryWrapper& iRegistryWrapper;
#endif

private:
	TBool IsSafeUninstallModeSetL();
	TInt  RemoveWithRetryAttemptL(TDesC& aFileName);
private:
	/// Installation plan
	const CPlan& iPlan;

	/// Client-owned application
	const CApplication* iApplication;
	
	/// Client-owned status
	TRequestStatus* iClientStatus;
	
	/// UiHandler provided by SWIS
	RUiHandler& iUiHandler;
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/// STS used to provide rollback functionality
	Usif::RStsSession& iStsSession;
#else
	/// Integrity services used to provide rollback functionality
	CIntegrityServices& iIntegrityServices;
#endif
	
	RFs iFs;

	TProcessingState iState;
	
	/**
	 * A list of executable sids needed (with iSidsAdded) to determine which
	 * private directories can be removed.
	 */
	RArray<TUid> iSidsRemoved;
	
	RArray<TUid> iSidsToShutdown;

	TInt iErrorCode;
	
	TInt iCurrent;
	
	TBool iCancelled;
	
	TInt iShutdownTimeout;

	TValidationStatus iValidationStatus;

	RSecPolHandle iSecPolHandle;
	
	RSwiObserverSession& iObserver; ///< SWI Observer session handle provided by SWIS.
	};

// inline functions from CProcessor

inline const CPlan& CProcessor::Plan() const
	{
	return iPlan;
	}

inline RUiHandler& CProcessor::UiHandler()
	{
	return iUiHandler;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
inline Usif::RStsSession& CProcessor::TransactionSession()
	{
	return iStsSession;
	}
#else
inline CIntegrityServices& CProcessor::IntegrityServices()
	{
	return iIntegrityServices;
	}
#endif

inline RFs& CProcessor::Fs()
	{
	return iFs;
	}
	
inline const CApplication& CProcessor::ApplicationL() const
	{
	if (!iApplication)
		{
		User::Leave(KErrBadUsage);
		}
	return *iApplication;
	}

inline RSwiObserverSession& CProcessor::Observer()
	{
	return iObserver;
	}
	
} // namespace Swi


#endif
