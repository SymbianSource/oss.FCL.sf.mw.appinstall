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
* Description:    
*
*/




#ifndef CIAUPDATEFWSYNCPROFILE_H
#define CIAUPDATEFWSYNCPROFILE_H

//  INCLUDES
#include <e32base.h>
#include <SyncMLClient.h>
#include <SyncMLClientDM.h>
#include "iaupdatefwconst.h"

/**
* CIAUpdateFWSyncProfile 
*
* CIAUpdateFWSyncProfile is used for getting/setting sync profile settings.
*/
NONSHARABLE_CLASS (CIAUpdateFWSyncProfile) : public CBase
    {
	enum TOpenMode
		{
	    EOpenRead,
	    EOpenReadWrite
		};
		
    public:
        /**
        * Two-phased constructor.
        * @param aParam
        */
        static CIAUpdateFWSyncProfile* NewLC( RSyncMLSession* aSyncSession);

        /**
        * Two-phased constructor.
        * @param aParam
        */
         static CIAUpdateFWSyncProfile* NewL( RSyncMLSession* aSyncSession );

	    /**
        * Destructor.
        */
        virtual ~CIAUpdateFWSyncProfile();

    private:
        /**
        * C++ default constructor.
        * @param aParam
        */
	    CIAUpdateFWSyncProfile( RSyncMLSession* aSyncSession );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();
	    
    public:
        /**
		* Open profile
        * @param aProfileId Profile id to open
        * @param aOpenMode Opening mode
		* @return None
        */
         void OpenL( TInt aProfileId, TInt aOpenMode );
        
        
        /**
		* Save profile
        * @param None
		* @return None
        */
         void SaveL();
        
	public:
        
        
        /**
		* Get profile name
        * @param aText name for profile
		* @return None
        */
         void GetName( TDes& aText );

        /**
		* Get profile id
        * @param None
		* @return Currently open profile id
        */
         TInt ProfileId();
        
    
 
    // settings that connection dialog displays
    public:
         
        /**
		* Get bearer type
        * @param None
		* @return bearer type
        */
         TInt BearerType();
         
        /**
		* Get accesspoint
        * @param None
		* @return accespoint
        */
         TInt AccessPointL();
        
        /**
		* Set accesspoint
        * @param aId accespoint id
		* @return None
        */
         void SetAccessPointL(const TInt aId);

        
        /**
		* Set server alert state
        * @param aState server alert state
		* @return None
        */        
         void SetSASyncStateL( TInt aState );
        
        /**
		* Get Server alert state
        * @param None
		* @return server alert state
        */
         TInt SASyncState();
 
         
 
               
    public:
       
        /**
		* Open connection
        * @param None
		* @return None
        */
         void OpenConnection();
        
 
        /**
		* Return DevMan profile
        * @param None
		* @return RSyncMLDevManProfile&
        */
         RSyncMLDevManProfile& Profile();
        
        /**
		* Return SyncML Session
        * @param None
		* @return RSyncMLSession&
        */
         RSyncMLSession& Session();
        
    public:
        /**
		* Get connection name
        * @param aText name
        * @param aPropertyPos position at the array
		* @return None
        */
         void GetConnectionPropertyNameL( TDes8& aText, TInt aPropertyPos );
        
	private:
	    // profile id
        TInt                    iProfileId;
        // Log open
        //TBool                   iHistoryLogOpen;
        // Connection open
        TBool                   iConnectionOpen;
   		// needed for setting creator id
		TInt                    iApplicationId;
		// long buffer for string handling 
		TBuf<KBufSize255>       iBuf;
		// long buffer for string handling 
		TBuf8<KBufSize255>      iBuf8;
   		// sync session
		RSyncMLSession*         iSyncSession;
		// sync profile
		RSyncMLDevManProfile    iProfile;
		// sync connection
		RSyncMLConnection       iConnection;
    };

#endif  // CIAUpdateFWSYNCPROFILE_H

// End of file
