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
* Description:   Application engine for the sync events
*
*/




#ifndef CIAUPDATEFWSYNCAPPENGINE_H
#define CIAUPDATEFWSYNCAPPENGINE_H

//  INCLUDES
#include <e32base.h>
#include <SyncMLClient.h>

#include "iaupdatefwconst.h"

// FORWARD DECLARATIONS
class CIAUpdateFWSyncHandler;
class CIAUpdateFWSyncProfile;
class CIAUpdateFWFotaModel;


// CLASS DECLARATION
/**
* CIAUpdateFWSyncAppEngine
* Sync engine for handling synchronization and profiles.
*/
class CIAUpdateFWSyncAppEngine : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CIAUpdateFWSyncAppEngine* NewL( CIAUpdateFWFotaModel* aFotaModel  );
        
        /**
        * Destructor.
        */
        virtual ~CIAUpdateFWSyncAppEngine();

    public: // New functions
        
        /**
        * Delete profile
        * @param aProfileId Profile to be deleted.
        * @return None
        */
        void DeleteProfileL( TInt aProfileId );

        /**
        * Create profile
        * @param None.
        * @return None
        */
        TInt CreateProfileL();

        /**
        * Create copy profile
        * @param aProfileId Profile that is used for copying
        * @return None
        */
        CIAUpdateFWSyncProfile* CreateCopyProfileL( TInt aProfileId );

        /**
        * Return current profile
        * @param None.
        * @return CIAUpdateFWSyncProfile*
        */
        CIAUpdateFWSyncProfile* Profile( );

        /**
        * Cancel synchronization
        * @param None.
        * @return None
        */
        void CancelSyncL();
        
        /**
        * Return RSyncMLSession
        * @param None.
        * @return RSyncMLSession*
        */
        RSyncMLSession* Session();
        
        /**
        * Open profile
        * @param aProfileId Profile id to open
        * @param aOpenMode opening mode
        * @return None
        */
        CIAUpdateFWSyncProfile* OpenProfileL( TInt aProfileId,
                                          TInt aOpenMode = ESmlOpenReadWrite );

        /**
        * Close profile
        * @param None.
        * @return None
        */        
        void CloseProfile();
        
        /**
        * Start synchronization
        * @param aProfileId Profile id to sync
        * @return None
        */               
        void SynchronizeL( TDesC& aServerName,
                           const TInt aProfileId,
                           const TInt aConnectionBearer,
                           const TBool aUseFotaProgressNote);
        
        /**
        * Start synchronization
        * @param aProfileId Profile id to sync
        * @param aJobId Jod id to sync
        * @return None
        */         
        void SynchronizeL( TDesC& aServerName,
                           const TInt aProfileId,
                           const TInt aJobId,
                           const TInt aConnectionBearer,
                           const TBool aUseFotaProgressNote );
        
        /**
        * Check if duplicate server id is found
        * @param  aServerId Server id
        * @param  aProfileId current profile id
        * @return ETrue if server id found
        */                 
        TBool ServerIdFoundL( const TDesC& aServerId, const TInt aProfileId );
        
		/**
		* Compare two descriptors
        * @param aLeft  
		* @param aRight  
		* @return - Positive, if this descriptor is 
        *                     greater than the specified descriptor.
		*           Negative, if this descriptor is
		*                     less than the specified descriptor.
		*           Zero, if the content of both descriptors match
        */
        TInt Compare( const TDesC& aLeft, const TDesC& aRight );
        
        /**
        * Sync state
        * @param None
        * @return ETrue if sync is running
        */          
        TBool SyncRunning();
        
        /**
        * Utility function.
        * @param aSyncObserver
        * @return None
        */
        //void RequestSyncStatus( MIAUpdateFWSyncObserver* aSyncObserver );
		
	void SyncCompleted( TNSmlStatus aStatus );

        
    private:

        /**
        * C++ default constructor.
        */
        CIAUpdateFWSyncAppEngine();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CIAUpdateFWFotaModel* aFotaModel );


    private:    // Data
        // SyncML Session
        RSyncMLSession          iSyncMLSession;
        // Sync profile
        CIAUpdateFWSyncProfile*     iProfile;
        // Sync handler
        CIAUpdateFWSyncHandler*     iSyncHandler;
        // Sync Observer
        //MIAUpdateFWSyncObserver*    iSyncObserver;
    };

#endif      // CIAUpdateFWSYNCAPPENGINE_H

// End of File
