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




#ifndef CIAUPDATEFWFOTAMODEL_H
#define CIAUPDATEFWFOTAMODEL_H

//  INCLUDES
#include <fotaengine.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Functionality required by the fota user interface.
*
*/

NONSHARABLE_CLASS (CIAUpdateFWFotaModel) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
         static CIAUpdateFWFotaModel* NewL( /*KNST CNSCDocument* aDocument*/ );
        
        /**
        * Destructor.
        */
        virtual ~CIAUpdateFWFotaModel();

    public: // New functions
        
        /**
        * Retrieves the default fota profile identifier from
        * the central repository.
        * @since Series 60 3.1
        * @param None
        * @return Profile identifier
        */
          TInt DefaultFotaProfileIdL() const;

        /**
        * Saves the default fota profile identifier to the
        * central repository.
        * @since Series 60 3.1
        * @param aProfileId Profile identifier.
        * @return None
        */
        void SetDefaultFotaProfileIdL( const TInt aProfileId );

        /**
        * Sets the central repository key to indicate that the
        * defined profile should be used to request firmware
        * update.
        * @since Series 60 3.1
        * @param aProfileId Profile identifier.
        * @return None
        */
        void EnableFwUpdRequestL( const TInt aProfileId );
       
 
        /**
        * Saves the current state of update packages.
        * @param None.
        * @return None.
        */
         void MarkFwUpdChangesStartL();

        
         RFotaEngineSession::TState GetCurrentFwUpdState();
         
         /**
          * Runtime if DM support is available or not
          * @return ETrue if DM support is available
          */
         TBool IsDMSupportAvailableL();
         
         /**
         * Tries to resume suspended firmware update package download.
         * @return Error code.
         */
         TInt TryResumeFwUpdDownload();


    private:

        /**
        * C++ default constructor.
        */
        CIAUpdateFWFotaModel();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

     
        RFotaEngineSession      iFotaEngine;     
    };

#endif      // CIAUPDATEFWFOTAMODEL_H  
            
// End of File
