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
* Description:   This file contains the header file of the RSWInstLauncher
*                class.
*
*/


#ifndef SWINSTAPI_H
#define SWINSTAPI_H

// INCLUDES
#include <AknServerApp.h>
#include <SWInstDefs.h>

// FORWARD DECLARATIONS
class RFile;

namespace SwiUI
{

// FORWARD DECLARATIONS
class CErrDetails;

// CLASS DECLARATION

/**
* Client side handle to access SW Installer UI services.
*  
* @lib swinst.lib
* @since 3.0
*/
class RSWInstLauncher : public RAknAppServiceBase
    {
    public:  // Constructors and destructor
    
        /**
        * Constructor.
        */
        IMPORT_C RSWInstLauncher();

        /**
        * Creates connection to the server. This will launch the installer
        * as server application.
        * @since 3.0
        * @return Error code
        */
        IMPORT_C TInt Connect();

        /**
        * Closes the connection to the server and close the installer server
        * application.
        * @since 3.0
        */
        IMPORT_C void Close();
        
        /**
        * Start Installation of the package. The call will return when the installation
        * if over.
        * @since 3.0
        * @param aFileName - Path to the installation package
        * @return Error code.
        */
        IMPORT_C TInt Install( const TDesC& aFileName );
   
        /**
        * Asynchronous version. The aReqStatus will be completed when installation is finished.
        * To cancel the request, use CancelAsyncRequest with ERequestInstall.
        */
        IMPORT_C void Install( TRequestStatus& aReqStatus, const TDesC& aFileName );

        /**
        * Start Installation of the package 
        * @since 3.0
        * @param aFile - Handle to the installation package.
        * @return Error code
        */
        IMPORT_C TInt Install( RFile& aFile );

        /**
        * Asynchronous version. 
        * To cancel the request, use CancelAsyncRequest with ERequestInstall.
        */
        IMPORT_C void Install( TRequestStatus& aReqStatus, RFile& aFile );
        
        /**
        * Start Installation of the package 
        * @since 3.0
        * @param aFileName - Path to the installation package
        * @param aParams - Installer parameters. Use TInstallReqPckg to package.
        * @return Error code
        */
        IMPORT_C TInt Install( const TDesC& aFileName, const TDesC8& aParams );
   
        /**
        * Asynchronous version
        * To cancel the request, use CancelAsyncRequest with ERequestInstallParams.
        */
        IMPORT_C void Install( TRequestStatus& aReqStatus,
                               const TDesC& aFileName,
                               const TDesC8& aParams );

        /**
        * Start Installation of the package 
        * @since 3.0
        * @param aFile - Handle to the installation package.
        * @param aParams - Installer parameters. Use TInstallReqPckg to package.
        * @return Error code
        */
        IMPORT_C TInt Install( RFile& aFile, const TDesC8& aParams );

        /**
        * Asynchronous version
        * To cancel the request, use CancelAsyncRequest with ERequestInstallParams.
        */
        IMPORT_C void Install( TRequestStatus& aReqStatus,
                               RFile& aFile, 
                               const TDesC8& aParams );
        
        /**
        * Start silent installation of the package (client must have TrustedUI capabilities)
        * @since future
        * @param aFileName - Path to the installation package
        * @param aOptions - Default options for installation. Use TInstallOptionsPckg to package.
        * @return Error code
        */  
        IMPORT_C TInt SilentInstall( const TDesC& aFileName, const TDesC8& aOptions );
  
        /**
        * Asynchronous version
        * To cancel the request, use CancelAsyncRequest with ERequestSilentInstall.
        */
        IMPORT_C void SilentInstall( TRequestStatus& aReqStatus, 
                                     const TDesC& aFileName, 
                                     const TDesC8& aOptions );

        /**
        * Start silent installation of the package (client must have TrustedUI capabilities)
        * @since future
        * @param aFile - Handle to the installation package.
        * @param aOptions - Default options for installation. Use TInstallOptionsPckg to package.
        * @return Error code
        */  
        IMPORT_C TInt SilentInstall( RFile& aFile, const TDesC8& aOptions );
  
        /**
        * Asynchronous version
        * To cancel the request, use CancelAsyncRequest with ERequestSilentInstall.
        */
        IMPORT_C void SilentInstall( TRequestStatus& aReqStatus, 
                                     RFile& aFile, 
                                     const TDesC8& aOptions );
   
        /**
        * Start silent installation of the package (client must have TrustedUI capabilities)
        * @since future
        * @param aFileName - Path to the installation package
        * @param aParams - Installer parameters. Use TInstallReqPckg to package.
        * @param aOptions - Default options for installation. Use TInstallOptionsPckg to package.
        * @return Error code
        */  
        IMPORT_C TInt SilentInstall( const TDesC& aFileName, 
                                     const TDesC8& aParams, 
                                     const TDesC8& aOptions );  

        /**
        * Asynchronous version
        * To cancel the request, use CancelAsyncRequest with ERequestSilentInstallParams.
        */
        IMPORT_C void SilentInstall( TRequestStatus& aReqStatus, 
                                     const TDesC& aFileName,
                                     const TDesC8& aParams, 
                                     const TDesC8& aOptions );

        /**
        * Start silent installation of the package (client must have TrustedUI capabilities)
        * @since future
        * @param aFile - Handle to the installation package.
        * @param aParams - Installer parameters. Use TInstallReqPckg to package.
        * @param aOptions - Default options for installation. Use TInstallOptionsPckg to package.
        * @return Error code
        */  
        IMPORT_C TInt SilentInstall( RFile& aFile, 
                                     const TDesC8& aParams, 
                                     const TDesC8& aOptions );

        /**
        * Asynchronous version
        * To cancel the request, use CancelAsyncRequest with ERequestSilentInstallParams.
        */
        IMPORT_C void SilentInstall( TRequestStatus& aReqStatus, 
                                     RFile& aFile, 
                                     const TDesC8& aParams, 
                                     const TDesC8& aOptions );

        /**
        * Start UnInstallation of the package 
        * @since future
        * @param aUid - uid of the installed package
        * @param aMIME - Data type of the package to be uninstalled.
        * @return Error code
        */
        IMPORT_C TInt Uninstall( const TUid& aUid, const TDesC8& aMIME );
   
        /**
        * Asynchronous version
        * To cancel the request, use CancelAsyncRequest with ERequestUninstall.
        */
        IMPORT_C void Uninstall( TRequestStatus& aReqStatus, 
                                 const TUid& aUid,
                                 const TDesC8& aMIME );
        
        /**
        * Start silent UnInstallation of the package, (client must have TrustedUI capabilities) 
        * @since future
        * @param aUid - uid of the installed package
        * @param aOptions - Default options for uninstallation. Use TUninstallOptionsPckg to package.
        * @param aMIME - Data type of the package to be uninstalled. 
        * @return Error code
        */                                
        IMPORT_C TInt SilentUninstall( const TUid& aUid, 
                                       const TDesC8& aOptions,
                                       const TDesC8& aMIME );

        /**
        * Asynchronous version
        * To cancel the request, use CancelAsyncRequest with ERequestSilentUninstall.
        */
        IMPORT_C void SilentUninstall( TRequestStatus& aReqStatus, 
                                       const TUid& aUid,
                                       const TDesC8& aOptions,
                                       const TDesC8& aMIME );

        /**
        * Cancel Asynchronous requests
        * @since 3.0
        */
        IMPORT_C TInt CancelAsyncRequest( TInt aReqToCancel );

        IMPORT_C CErrDetails* GetLastErrorL();

        /**
        * Performs a custom uninstallation.
        * @since 3.0
        * @param aOperation - The operation to be performed.
        * @param aParams - Parameters for the operation
        */ 
        IMPORT_C TInt CustomUninstall( TOperation aOperation, 
                                       const TDesC8& aParams,
                                       const TDesC8& aMIME );

        /**
        * Asynchronous version
        * To cancel the request, use CancelAsyncRequest with ERequestCustomUninstall.
        */
        IMPORT_C void CustomUninstall( TRequestStatus& aReqStatus, 
                                       TOperation aOperation, 
                                       const TDesC8& aParams,
                                       const TDesC8& aMIME );

        /**
        * Performs a silent custom uninstallation.
        * @since 3.0
        * @param aOptions - Default options for uninstallation.
        * @param aOperation - The operation to be performed.
        * @param aParams - Parameters for the operation
        */ 
        IMPORT_C TInt SilentCustomUninstall( TOperation aOperation, 
                                             const TDesC8& aOptions,
                                             const TDesC8& aParams,
                                             const TDesC8& aMIME );

        /**
        * Asynchronous version
        * To cancel the request, use CancelAsyncRequest with ERequestSilentCustomUninstall.
        */
        IMPORT_C void SilentCustomUninstall( TRequestStatus& aReqStatus, 
                                             TOperation aOperation, 
                                             const TDesC8& aOptions,
                                             const TDesC8& aParams,
                                             const TDesC8& aMIME );

    protected:

        /**
        * Panics the client.
        * @since 3.0
        * @param aPanic - Panic ID.
        * @return System wide error code.
        */
        void PanicClient( TInt aPanic ) const;

    private:  // From base classes
       
        /**
        * From RApaAppServiceBase, Returns the UID of the service that this session 
        * provides an interface for.
        * @since 3.0
        */
        TUid ServiceUid() const;

    protected:// Data
       
        TBool iConnected;  // ETrue, if the session has been established

    private:  // Data

        TAny* reserved1;
        TAny* reserved2;
    };

/**
* Client side handle to access SW Installer UI services silenty ( no UI ). 
*  
* @lib swinst.lib
* @since 3.0
*/
class RSWInstSilentLauncher : public RSWInstLauncher
    {
    public:  // Constructors and destructor
    
        /**
        * Constructor.
        */
        IMPORT_C RSWInstSilentLauncher();

        /**
        * Creates connection to the server. This will launch the installer
        * as a server application. However, the application is started in the background
        * so that status pane is not visible and the task cannot be seen in
        * the tasklist.
        * @since 3.0
        * @return Error code
        */
        IMPORT_C TInt Connect();

    private:  // From base classes
       
        /**
        * From RApaAppServiceBase, Returns the UID of the service that this session 
        * provides an interface for.
        * @since 3.0
        */
        virtual TUid ServiceUid() const;

    private:  // New functions

        /**
        * Constructs the name of the server application.
        * @since 3.0
        */
        void ConstructServerName( TName& aServerName, 
                                  TUid aAppServerUid, 
                                  TUint aServerDifferentiator );

        /**
        * Starts the server application.
        * @since 3.0
        */
        TUint StartServerL( TUid aAppUid );
        
        /**
        * Launches the server application.
        * @since 3.0
        */
        void LaunchAppL( TUid aAppUid, TUint aServerDifferentiator, TThreadId& aThreadId );        

    private:    // Data

        TAny* reserved;
    };

}

#endif      //  SWINSTAPI_H 
            
// End of File


