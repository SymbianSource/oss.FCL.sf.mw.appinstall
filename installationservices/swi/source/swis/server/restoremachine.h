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

#ifndef __RESTOREMACHINE_H__
#define __RESTOREMACHINE_H__

#include <e32base.h>

#include "installationprocessor.h"
#include "swi/siscontroller.h"
#include "sisregistrywritablesession.h"
#include "secutils.h"

namespace Swi 
	{

	class CPlan;
	class CRestoreController;
	class CSecurityManager;
	class CSisRegistryFileDescription;
	class CRestoreProcessor;
	class RSisRegistryEntry;
	class RSisRegistrySession;



	/**
	 * State machine controlled by events from an installserver
	 * client that takes backed up metadata and files and 
	 * restores them to the device in a secure manner.
	 *
	 * @internalComponent
	 * @released
	 */
	
	class CRestoreMachine : public CBase
		{
		friend class CCommitWatcher;
	public:
		static CRestoreMachine* NewL(const RMessage2& aMessage);
		static CRestoreMachine* NewLC(const RMessage2& aMessage);
		
		~CRestoreMachine();
			
		void ServiceFileRequestL(const RMessage2& aMessage);
		void ServiceCommitRequestL(const RMessage2& aMessage);
		
		TBool IsComplete();
		RSwiObserverSession& Observer();
		
	private:
		void ConstructL(const RMessage2& aMessage);
		
		CPlan* CreateUninstallPlanL();
		TBool IsUniqueL(const TDesC& aFileName, TBool& aCheckFileHashOut);
		void PrepareUpgradesL();
        void CheckVersionUpgradeL();
		void RemovePackageFilesL(RSisRegistryEntry& aEntry);
		TBool FindInstalledAugmentationL(RSisRegistrySession& aSession, const Sis::CController& aController, 
				RSisRegistryEntry& aEntryToOverwrite);		
		TVersion GetLatestVersionOfPackageToRestore();

		
	private:
		
		/**
	 	 * Watches the restore process state machine and commits or
	 	 * rolls back the transaction depending on the sucess or
	 	 * failure of that state machine.
	 	 * 
		 *
		 * @internalComponent
	 	 * @released
	 	 */
	
		class CCommitWatcher : public CActive
			{
			
		public:
			static CCommitWatcher* NewL(const RPointerArray<CPlan>& aPlans, CSecurityManager& aSecurityManager,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
				Usif::RStsSession& aStsSession, RSisRegistryWritableSession& aRegistrySession,
#else
				CIntegrityServices& aIntegrityServices,
#endif
				CRestoreController& aController, const RMessage2& aMessage, RFs& aFs, CRestoreMachine& aMachine);
			
			
			void StartL();
			
			~CCommitWatcher();
			
		protected:
			CCommitWatcher(const RPointerArray<CPlan>& aPlans, CSecurityManager& aSecurityManager,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
				Usif::RStsSession& aStsSession, RSisRegistryWritableSession& aRegistrySession,
#else
				CIntegrityServices& aIntegrityServices,
#endif
				CRestoreController& aController, const RMessage2& aMessage, RFs& aFs, CRestoreMachine& aMachine);				

			
			void RunL();
			void DoCancel();
			TInt RunError(TInt aError);
			
			void SwitchStateL();
			void CompleteL();
		
		private:
			TBool IsValidUpgradeL(const Sis::CController& aController, RArray<TUid>& aSids);
			TBool ValidEclipsingL(const CApplication& aApplication);			

			
		private:
			const RMessage2& iClientMessage; // The client message, used to complete the request on termination of the processor
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			Usif::RStsSession& iStsSession; // The restore STS session, used to commit or rollback the transaction
			RSisRegistryWritableSession& iRegistrySession; // Used to commit or rollback registry changes
#else
			CIntegrityServices& iIntegrityServices; // The restore integrity services session, used to commit or rollback the transaction
#endif
			CRestoreController& iRestoreController; // The sis controller support class
			CSecurityManager& iSecurityManager;
			const RPointerArray<CPlan>& iPlans;
			
			CRestoreProcessor* iProcessor; // The restore processor state machine to watch
			TInt iPlanIndex;
			TInt iVerifierIndex;
			
			RFs& iFs;
			CRestoreMachine& iMachine;
			};
	
	private: 
		TUid iPackageUid; 	// The UID of the top level package being restored
		CRestoreController* iRestoreController; // The controller handler used to verify the packages
		CSecurityManager* iSecurityManager; // A security manager for this transaction
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		Usif::RStsSession iStsSession; // An STS session for this restore
		RSisRegistryWritableSession iRegistrySession;
#else
		CIntegrityServices* iIntegrityServices; // An integrity services session for this restore
#endif

		CCommitWatcher* iCommitWatcher; // Watcher to complete the session when the restore processor terminates
		RFs iFs; // A file server session for the restore server
		CFileMan* iFileMan;
		
		/// The drive on which to store device integrity data (hashes, registry etc) 
		TChar iSystemDriveChar;
		RSwiObserverSession iObserver;			///< Swi Observer session handle.
		HBufC8 *iBuf;
		
		//An array whose first element holds the number of uids in the subsequent indices of the array, followed by the the list of uids that are being published.
		TUid iUidList[KMaxUidCount];  
		};
		
			
	inline TBool CRestoreMachine::IsComplete()
		{
	
		if (iCommitWatcher)
			{
			return (iCommitWatcher->iStatus != KRequestPending);
			}
		return EFalse;
	
		}
		
	}


#endif // __RESTOREMACHINE_H__
