/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CSWInstInstallRequest 
*                class 
*
*                This class implements functionality for install request.
*
*/


#ifndef SWINSTINSTALLREQUEST_H
#define SWINSTINSTALLREQUEST_H

//  INCLUDES
#include <e32base.h>
#include <SWInstDefs.h>

#include "SWInstRequestObject.h"

// FORWARD DECLARATIONS
namespace ContentAccess { class CContent; }

namespace SwiUI
{

// FORWARD DECLARATIONS

class CSWInstUIPluginAPI;

// CLASS DECLARATION

/**
* This class implements functionality for install request.
*
* @since 3.0
*/
class CSWInstInstallRequest : public CSWInstRequestObject
    {
    public:  // Constructors and destructor
     
        /**
        * Two-phased constructor.
        * @param aMessage - Message object having this request.
        */
        static CSWInstInstallRequest* NewL( const RMessage2& aMessage );

        /**
        * Two-phased constructor.
        * @param aMessage - Message object having this request.
        */
        static CSWInstInstallRequest* NewL( TInt aObjectId );
    
        /**
        * Destructor.
        */
        virtual ~CSWInstInstallRequest();

    public: // New functions

        /**
        * Perform installation
        * @since 3.0
        * @param aFile - Handle to the installation package.
        * @param aParams - install parameters.
        */
        void Install( RFile& aFile, const TInstallReq& aParams );

        /**
        * Perform installation
        * @since 3.0
        * @param aFileName - Path to the installation package.
        * @param aParams - install parameters.
        */
        void Install( const TDesC& aFileName, const TInstallReq& aParams );

        /**
        * Perform silent installation
        * @since 3.0
        * @param aFile - Handle to the installation package.
        * @param aParams - install parameters.
        * @param aOptions - Options for installation
        */
        void SilentInstall( RFile& aFile, 
                             const TInstallReq& aParams, 
                             const TInstallOptions& aOptions );  

        /**
        * Perform silent installation
        * @since 3.0
        * @param aFileName - Path to the installation package.
        * @param aParams - install parameters.
        * @param aOptions - Options for installation
        */
        void SilentInstall( const TDesC& aFileName, 
                             const TInstallReq& aParams, 
                             const TInstallOptions& aOptions );          

    protected: // Functions from base classes

        /**
        * From CSWInstRequestObject, Cancels the current request.
        * @since 3.0
        */
        void CancelRequest();

        /**
        * From CSWInstRequestObject, Gets called when the request is completed.
        * @since 3.0
        */
        void RequestCompleteL( TInt aResult );
        
    private:  // New functions
        
        /**
        * C++ default constructor.
        * @param aMessage - Message object having this request.
        */
        CSWInstInstallRequest( const RMessage2& aMessage );

        /**
        * C++ default constructor.
        * @param aMessage - Message object having this request.
        */
        CSWInstInstallRequest( TInt aObjectId );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Handles the error when installation of the given package is not supported.
        * @since 3.0
        * @param aFileName - File name of the package.
        */
        void HandleNotSupportedL( const TDesC& aFileName ) const;

        /**
        * Performs the actual installation.
        * @since 3.0
        * @param aFile - Handle to the package.
        */
        void DoInstallL( RFile& aFile);   

        /**
        * Gets MIME type of the package.
        * @since 3.0
        * @param aContent - Package content.
        * @param aMIME - On return contains the MIME type of the package.
        */
        void GetMIMETypeL( const ContentAccess::CContent& aContent, TDes8& aMIME ); 
        
        /**
        * Helper to handle protected content.
        * @since 3.0
        * @param aFile - Handle to the package.
        * @param 
        * @return ETrue if rights exist and package can be installed.
        *         EFalse if the installation needs to be terminated.
        */
        TBool HandleProtectedContentL( RFile& aFile );
        
    private: // Data
        
        TInstallReq iParams;        
        TInstallOptions iOptions;

        RFile iFile;
        RFs iFs;        
    };
}

#endif      // SWINSTINSTALLREQUEST_H   
            
// End of File
