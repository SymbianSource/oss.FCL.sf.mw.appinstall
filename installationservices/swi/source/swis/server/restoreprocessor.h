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
 @internalComponent 
*/

#ifndef __RESTOREPROCESSOR_H__
#define __RESTOREPROCESSOR_H__

#include <e32base.h>

#include "processor.h"
#include "securitymanager.h"
#include "installationprocessor.h"
#include "restorecontroller.h"
#include "sisregistrywritablesession.h"

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
class CIntegrityServices;
#endif

namespace Swi 
	{
	class CPlan;

	
	/**
	 * State machine to handle securely restoring files the user
	 * has requested to be restored, and writing registry entries for
	 * same.
	 *
	 * @internalComponent
	 * @released
	 */
	
	class CRestoreProcessor : public CActive
		{
	protected:
		enum TProcessingState
			{
			EInitialize,
			EProcessFiles,
			EVerifyPaths,
			EInstallFiles,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			EParseApplicationRegistrationFiles,
#endif
			EUpdateRegistry,
			EFinished,
			ECurrentState=128, // Used to change to the current state
		};

		
	public:
		static CRestoreProcessor* NewL(const CPlan& aPlan, const TDesC8& aControllerBuffer, 
			CSecurityManager& aSecurityManager,	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			Usif::RStsSession& aStsSession, RSisRegistryWritableSession& aRegistrySession, RArray<TAppUpdateInfo>& aAppInfo,
#else
			CIntegrityServices& aIntegrityServices,
#endif
			const RPointerArray<CRestoreController::CSisCertificateVerifier>& aVerifiers,
			RArray<TUid>& aSids, RSwiObserverSession& aObserver);
		
		static CRestoreProcessor* NewLC(const CPlan& aPlan, const TDesC8& aControllerBuffer,
			CSecurityManager& aSecurityManager, 
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			Usif::RStsSession& aStsSession, RSisRegistryWritableSession& aRegistrySession, RArray<TAppUpdateInfo>& aAppInfo,
#else
			CIntegrityServices& aIntegrityServices,
#endif
			const RPointerArray<CRestoreController::CSisCertificateVerifier>& aVerifiers,
			RArray<TUid>& aSids, RSwiObserverSession& aObserver);
			
		~CRestoreProcessor();
		
		void ProcessPlanL(TRequestStatus& aClientStatus);		
		
	protected:
		virtual TInt RunError(TInt aError);
		virtual void DoCancel();
		virtual void RunL();
		
		void SwitchState(TProcessingState aNextState);
		
	private:
		CRestoreProcessor(const CPlan& aPlan, const TDesC8& aControllerBuffer,
			CSecurityManager& aSecurityManager,	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			Usif::RStsSession& aStsSession, RSisRegistryWritableSession& aRegistrySession, RArray<TAppUpdateInfo>& aAppInfo,
#else
			CIntegrityServices& aIntegrityServices,
#endif
			const RPointerArray<CRestoreController::CSisCertificateVerifier>& aVerifiers,
			RSwiObserverSession& aObserver);
		
		void ConstructL(RArray<TUid>& aSids);
		void ProcessApplicationL(const CApplication& aApplication, TRequestStatus& aClientStatus);	

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
		TBool FileIsApparcReg(const TDesC& aFilename) const;
		void AddApparcFilesInListL(const TDesC& aTargetFileName, const CApplication& aApplication);
		TInt UserSelectedLanguageIndexL(const CApplication& aApplication) const;
		TBool DoParseApplicationRegistrationFilesL();
		TBool ParseRegistrationResourceFileL(const TDesC& aTargetFileName);
		void AddAppArcRegResourceFilesL();
		void AddAppArcRegResourceFilesForRegEntryL( RSisRegistryEntry& aEntry);
		TInt FindAppEntry(RArray<TAppUpdateInfo>& aAffectedApps, TUid& aNewAppUid);
#endif
	private:
	
		// Processor states, called by the state machine
	
		TBool DoStateInitializeL();
		TBool DoStateInstallFilesL();
		TBool DoStateVerifyPathsL();
		TBool DoStateUpdateRegistryL();
		TBool DoStateProcessFilesL();
		void DoStateFinishedL();
		
		void InstallFileL(const CSisRegistryFileDescription& aRegistryFileDescription);
		void ExtractHashL(const CSisRegistryFileDescription& aFileToProcess);
		
	private:		
		const RPointerArray<CRestoreController::CSisCertificateVerifier>& iVerifiers; // verifiers for the sis controllers being restored
		CSecurityManager& iSecurityManager; // Security manager for this restore session
		TInt iCurrent; // current index of a file to be processed
		RArray<TUid> iSids; // SIDs of executables installed
		
		RFs iFs;
		
		const TDesC8& iControllerBuffer;	/// Reference to the raw controller buffer for iPlan
		const CPlan& iPlan;
		const CApplication* iApplication;
		TRequestStatus* iClientStatus;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		Usif::RStsSession& iStsSession;
		RSisRegistryWritableSession& iRegistrySession;
		/** The list of software types to be registered found in the XML registration file. */
		RCPointerArray<Usif::CSoftwareTypeRegInfo> iSoftwareTypeRegInfoArray;
		/**
        * The list of Apparc registration files to parse to populate SCR
        */ 
        RPointerArray<CAppRegFileData> iApparcRegFilesForParsing;
        CAppRegExtractor *iAppRegExtractor;  
        RPointerArray<Usif::CApplicationRegistrationData> iApparcRegFileData;   
        RArray<TAppUpdateInfo> iAppInfo;
#else
		CIntegrityServices& iIntegrityServices;
#endif
		TProcessingState iState;
		CFileMan* iFileMan;
		
		/// The drive on which to store device integrity data (hashes, registry etc) 
		TChar iSystemDriveChar;
		RSwiObserverSession& iObserver; ///< SWI Observer session handle provided by SWIS.				
		};	
	}

#endif //__RESTOREPROCESSOR_H__

