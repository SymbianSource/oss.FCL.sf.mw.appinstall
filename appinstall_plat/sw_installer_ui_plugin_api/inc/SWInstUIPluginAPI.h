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
* Description:   This file contains the header file of the 
*                CSWInstUIPluginAPI class.
*
*                This class is the ECom interface for SWInst UI plugins.
*
*/


#ifndef SWINSTUIPLUGINAPI_H
#define SWINSTUIPLUGINAPI_H

//  INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include <SWInstUid.h>
#include <SWInstDefs.h>

namespace SwiUI
{

//  CONSTANTS
const TUid KCSWInstUIPluginInterfaceUid = 
    { KSWInstUIPluginInterfaceUid } ;

// FORWARD DECLARATIONS
class CErrDetails;

/**
* This class is the ECom interface class for SWInstaller Ui plugins.
*
* @since 3.0
*/
class CSWInstUIPluginAPI : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        inline static CSWInstUIPluginAPI* NewL( const TDesC8& aMIME );
        
        /**
        * Destructor.
        */
        inline virtual ~CSWInstUIPluginAPI();

    public: // New functions
        
        /**
        * Perform installation.
        * @since 3.0
        * @param aFile - Handle to the installation package.
        * @param aInstallParams - Object containing the installation parameters.
        * @param aIsDRM - Indicates if installation pkg has DRM.
        * @return Error code.
        */
        virtual void InstallL( RFile& aFile,
                               TInstallReq& aInstallParams, 
                               TBool aIsDRM,
                               TRequestStatus& aStatus ) = 0;

        /**
        * Perform silent installation.
        * @since future
        * @param aFile - Handle to the installation package.
        * @param aInstallParams - Object containing the installation parameters.
        * @param aIsDRM - Indicates if installation pkg has DRM.
        * @param aOptions - Values for installation options.
        * @return Error code.
        */
        virtual void SilentInstallL( RFile& aFile,
                                     TInstallReq& aInstallParams,
                                     TBool aIsDRM, 
                                     TInstallOptions& aOptions,
                                     TRequestStatus& aStatus ) = 0;
        
        /**
        * Perform uninstallation.
        * @since 3.0
        * @param aUid - Uid of the package to be uninstalled.
        * @param aMIME - Mime type of the package.
        * @return Error code.
        */
        virtual void UninstallL( const TUid& aUid, 
                                 const TDesC8& aMIME,
                                 TRequestStatus& aStatus ) = 0;
        
        /**
        * Perform silent uninstallation.
        * @since future
        * @param aUid - Uid of the package to be uninstalled.
        * @param aMIME - Mime type of the package.
        * @param aOptions - Values for uninstallation options.
        * @return Error code.
        */
        virtual void SilentUninstallL( const TUid& aUid, 
                                       const TDesC8& aMIME, 
                                       TUninstallOptions& aOptions,
                                       TRequestStatus& aStatus ) = 0;
                
        /**
        * Indicates if the application shell must be updated.
        * Obsolete since 3.1.
        * @since 3.0
        * @return ETrue, if yes, EFalse otherwise.
        */
        virtual TBool IsAppShellUpdate() = 0;

        /**
        * Cancel the current operation.
        * @since 3.0
        */
        virtual void Cancel() = 0;

        /**
        * Informs the plugin about completed operation. Called when all tasks 
        * are completed.
        * @since 3.0
        */
        virtual void CompleteL() = 0;

        /**
        *
        */
        virtual CErrDetails* GetErrorDetails() = 0;
        
        /**
        * Perform a custom uninstallation.
        * @since 3.0
        * @param aOperation - The uninstall operation to be performed
        * @param aParams - Parameters for the operation
        */        
        virtual void CustomUninstallL( TOperation aOperation, 
                                       const TDesC8& aParams, 
                                       TRequestStatus& aStatus ) = 0;

        /**
        * Perform a silent custom uninstallation.
        * @since 3.0
        * @param aOperation - The uninstall operation to be performed
        * @param aParams - Parameters for the operation
        */        
        virtual void SilentCustomUninstallL( TOperation aOperation,
                                             TUninstallOptions& aOptions,
                                             const TDesC8& aParams, 
                                             TRequestStatus& aStatus ) = 0;
        
    private:    // Data
        
        TUid iDestructKey; // An identifier used during destruction 
    };
}

#include "SWInstUIPluginAPI.inl"

#endif      // SWINSTALLERUIHANDLER_H   
            
// End of File
