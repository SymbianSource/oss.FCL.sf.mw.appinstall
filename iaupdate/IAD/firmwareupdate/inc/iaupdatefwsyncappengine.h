/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
class MIAUpdateFWUpdateObserver;


// CLASS DECLARATION
/**
* CIAUpdateFWSyncAppEngine
* Sync engine for handling synchronization and profiles.
*/
NONSHARABLE_CLASS (CIAUpdateFWSyncAppEngine) : public CBase
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
        
        void SetObserver( MIAUpdateFWUpdateObserver* aObserver );
 
        /**
        * Return current profile
        * @param None.
        * @return CIAUpdateFWSyncProfile*
        */
        CIAUpdateFWSyncProfile* Profile( );
         
         
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
    };

#endif      // CIAUpdateFWSYNCAPPENGINE_H

// End of File
