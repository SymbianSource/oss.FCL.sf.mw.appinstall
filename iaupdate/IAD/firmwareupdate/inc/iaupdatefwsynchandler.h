/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef IAUPDATEFWSYNCHANDLER_H
#define IAUPDATEFWSYNCHANDLER_H

// INCLUDES
#include <e32base.h>
#include <SyncMLObservers.h>
#include <cmmanager.h>

#include "iaupdatefwsyncutil.h"
#include "iaupdatefwsyncprofile.h"

#include "iaupdatefwconst.h"

// FORWARD DECLARATIONS
class CIAUpdateFWSyncState;
class CIAUpdateFWActiveCaller;

class CIAUpdateFWSyncAppEngine;
class CAknWaitDialog;
class CIAUpdateFWFotaModel;
// CLASS DECLARATION

/**
* CIAUpdateFWSyncHandler class
* 
*/
NONSHARABLE_CLASS (CIAUpdateFWSyncHandler) : public CBase, 
						public MIAUpdateFWActiveCallerObserver,
						public MSyncMLEventObserver,
						public MSyncMLProgressObserver
	{
    public:
        /**
        * Two-phased constructor.
        */
	    static CIAUpdateFWSyncHandler* NewL( RSyncMLSession* aSyncSession,
	                                     CIAUpdateFWSyncAppEngine* aAppEngine,
	                                     CIAUpdateFWFotaModel* aFotaModel );

	    /**
        * Destructor.
        */
	    virtual ~CIAUpdateFWSyncHandler();

    private:
	    /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

	    /**
        * C++ default constructor.
        */
	    CIAUpdateFWSyncHandler( RSyncMLSession* iSyncSession,
                            CIAUpdateFWSyncAppEngine* aAppEngine,
                            CIAUpdateFWFotaModel* aFotaModel );

		

    
	private: // from MAspActiveCallerObserver
		/**
        * From MAspActiveCallerObserver
		* Called when CAspActiveCaller completes.
        * @param None
        * @return None.
        */
		void HandleActiveCallL();


    private:    // from MSyncMLEventObserver
        
		/**
        * From MSyncMLEventObserver
		* Called when SyncML session events oocur.
        * @param aEvent Sync event
        * @param aIdentifier Identifier for the sync event
        * @param aError Synce event error
        * @param aAdditionalData.
        * @return None.
        */
        void OnSyncMLSessionEvent( TEvent aEvent,
                                   TInt aIdentifier,
                                   TInt aError,
                                   TInt aAdditionalData );
	
	
    private:  //from MSyncMLProgressObserver
	    
	    /**
	    * Receives notification of a synchronisation error.
        * @param aErrorLevel  The error level.
	    * @param aError		  The type of error. This is one of 
	    *                     the SyncMLError error values.	
	    * @param aTaskId      The ID of the task for which the error occurred.
	    * @param aInfo1       An integer that can contain additional
	    *                     information about the error. Normally 0.
	    * @param aInfo2       An integer that can contain additional
	    *                     information about the error. Normally 0.
	    * @return             None.
	    */
	    void OnSyncMLSyncError( TErrorLevel aErrorLevel,
	                            TInt aError,
	                            TInt aTaskId,
	                            TInt aInfo1,
	                            TInt aInfo2 );
       	
       	/**
	    * Receives notification of synchronisation progress.
	    * @param aStatus	The current status, e.g. 'Connecting'.
	    * @param aInfo1	    An integer that can contain additional
	    *                   information about the progress.
	    * @param aInfo2	    An integer that can contain additional
        *                   information about the progress.
	    */
	    void OnSyncMLSyncProgress( TStatus aStatus, TInt aInfo1, TInt aInfo2);
	    
	    /**
	    * Receives notification of modifications to synchronisation tasks.
        * @param aTaskId               The ID of the task.
	    * @param aClientModifications  Modifications made on 
	    *                              the clients Data Store.
	    * @param aServerModifications  Modifications made on
	    *                              the server Data Store.
	    */
	    void OnSyncMLDataSyncModifications( 
	                TInt /*aTaskId*/,
	                const TSyncMLDataSyncModifications& aClientModifications,
	                const TSyncMLDataSyncModifications& aServerModifications );
        
	private:

        /**
        * Shows error dialog.
		* @param None.
        * @return Error code.
        */
        void HandleSyncErrorL();
        
        /**
        * Called when sync completes
		* @param aError.
        * @return None.
        */        
        void SynchronizeCompletedL(TInt aError);

        
        /**
        * Read sync error from CNSmlSyncLog and show error message.
		* @param aProfileId Profile id.
        * @return Result code.
        */
		TInt HandleSyncErrorL(TInt aProfileId);
	
	
	public:
        /**
        * Performs synchronization.
        * @param aServerName Name of the server
		* @param aProfileId Profile identifier.
		* @param aConnectionBearer Bearer
		* @param aUseFotaProgressNote Should the simplified progress note used. Only
		*                             used when checking for firmware updates.
        * @return None
        */
		void SynchronizeL( TDesC& aServerName,
		                   const TInt aProfileId,
		                   const TInt aConnectionBearer,
                           const TBool aUseFotaProgressNote );
		
        /**
        * Performs ServerInitiated synchronization.
        * @param aServerName Name of the server
		* @param aProfileId Profile id
		* @param aJobId Job id.
		* @param aConnectionBearer Bearer
		* @param aUseFotaProgressNote Should the simplified progress note used. Only
		*                             used when checking for firmware updates.
        * @return None
        */		
		void SynchronizeL( TDesC& aServerName, 
		                   const TInt aProfileId,
		                   const TInt aJobId,
		                   const TInt aConnectionBearer,
                           const TBool aUseFotaProgressNote );
		
        /**
        * Shows the progress dialog.
		* @param None.
        * @return None.
        */
        void ShowProgressDialogL();
        
        /**            
         * Select IAP
         * @param None.
         * @return None.
         */
        void SelectIAPL();

        /**
        * Deletes the progress dialog if it exists.
		* @param None.
        * @return None.
        */
        void HideProgressDialogL();
        
		/**
        * Cancel synchronization.
		* @param None 
        * @return None
        */
		void CancelSynchronizeL();
		
        /**
        * From MProgressDialogCallback. Handles the situation when the dialog
        * is dismissed.
        * @param aButtonId The identifier of the button, with which the dialog
        *                  was dismissed.
        * @return None
        */
        void DialogDismissedL( TInt aButtonId );
		
	private:
        
        /**
        * Performs synchronization.
        * @return None
        */	
		void SynchronizeL();
		
		/**
        * Utility function.
        * @return Sync session.
        */
		RSyncMLSession& Session();
		

		/**
        * Utility function.
        * @return Sync state.
        */
		CIAUpdateFWSyncState* State();

    public:

		/**
        * Utility function.
        * @return ETrue if sync is currently running, EFalse otherwise.
        */
		TBool SyncRunning();

    private:
        
        TUint32 SelectConnectionMethodL();
        TUint32 GetBestIAPInInternetSNAPL( RCmManager& aCmManager );
        TUint32 GetBestIAPInThisSNAPL( RCmManager& aCmManager, TUint32 aSNAPID  );

    private:
		// session with sync server
		RSyncMLSession*                 iSyncSession;
		// app engine
		CIAUpdateFWSyncAppEngine*           iSyncAppEngine;
        // Pointer to the application document class
		CIAUpdateFWFotaModel*            iFotaModel;
		// profile id
		TInt                            iProfileId;
		// sync job id
		TInt                            iSyncJobId;
		// sync job
		RSyncMLDevManJob                iSyncJob;

        // The alternative wait dialog used in FOTA
        CAknWaitDialog*                 iWaitDialog;
        
		// sync handler state
		CIAUpdateFWSyncState*               iState;
		// for making function call via active scheduler
		CIAUpdateFWActiveCaller*            iActiveCaller;
		// is sync currently running
		TBool                           iSyncRunning;
		// sync error code
		TInt                            iSyncError;
		// long buffer for string handling
		TBuf<KBufSize256>               iBuf;
		//Job id
        TSmlJobId                       iJobId;
        // Bearer
        TInt                            iConnectionBearer;
        // Server name
        TBuf<KNSmlMaxProfileNameLength> iServerName;
        // Server sync
        TBool                           iServerAlertedSync;
        // Should the simpler FOTA progress not be used
        TBool                           iUseFotaProgressNote;
        // Retry sync (authentication error)
        TBool							iRetrySync;
	};

#endif  // NSMLDMSYNCHANDLER_H

// End of file
