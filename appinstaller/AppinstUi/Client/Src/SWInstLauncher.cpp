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
* Description:   This file contains the implementation of RSWInstLauncher 
*                class member functions.
*
*/


// INCLUDE FILES
#include <f32file.h>
#include <eikenv.h>

#include "SWInstApi.h"
#include "SWInstDefs.h"
#include "SWInstCommon.h"

using namespace SwiUI;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// RSWInstLauncher::RSWInstLauncher
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RSWInstLauncher::RSWInstLauncher()
    : iConnected( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Connect
// Creates connection to the server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::Connect()
    {
    TInt result( KErrNone );

    if ( !iConnected )
        {
        if ( CEikonEnv::Static() )
            {            
            TRAP( result, ConnectChainedAppL( KUidSWInstSvr ) );
            }
        else
            {
            TRAP( result, REikAppServiceBase::ConnectNewAppL( KUidSWInstSvr ) );            
            }
        
        if ( result == KErrNone )
            {
            iConnected = ETrue;
            }        
        }
    
    // Return the result code           
    return result; 
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Close
// Closes the connection to the server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
EXPORT_C void RSWInstLauncher::Close()
    {
    if ( iConnected )
        {
        RAknAppServiceBase::Close();
        iConnected = EFalse;
        }    
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// Launches the software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::Install( const TDesC& aFileName )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    args.Set( KFileNameIpcSlot, &aFileName );
    return SendReceive( ERequestInstall, args );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// Launches the software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::Install( TRequestStatus& aReqStatus, 
                                        const TDesC& aFileName )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    args.Set( KFileNameIpcSlot, &aFileName );
    SendReceive( ERequestInstall, args, aReqStatus );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// Launches the software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::Install( RFile& aFile )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    aFile.TransferToServer( args, KFileHandleIpcSlot, KFileSrvSessionIpcSlot );    
    return SendReceive( ERequestInstallHandle, args );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// Launches the software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::Install( TRequestStatus& aReqStatus, 
                                        RFile& aFile )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    aFile.TransferToServer( args, KFileHandleIpcSlot, KFileSrvSessionIpcSlot );    
    SendReceive( ERequestInstallHandle, args, aReqStatus );
    }
  
// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// Launches the software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::Install( const TDesC& aFileName, 
                                        const TDesC8& aParams )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    args.Set( KFileNameIpcSlot, &aFileName );
    args.Set( KParamsIpcSlot, &aParams );

    return SendReceive( ERequestInstallParams, args );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// Launches the software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::Install( TRequestStatus& aReqStatus, 
                                        const TDesC& aFileName,
                                        const TDesC8& aParams )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    args.Set( KFileNameIpcSlot, &aFileName );
    args.Set( KParamsIpcSlot, &aParams );

    SendReceive( ERequestInstallParams, args, aReqStatus );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// Launches the software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::Install( RFile& aFile,
                                        const TDesC8& aParams )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    aFile.TransferToServer( args, KFileHandleIpcSlot, KFileSrvSessionIpcSlot );    
    args.Set( KParamsIpcSlot, &aParams );    

    return SendReceive( ERequestInstallParamsHandle, args );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// Launches the software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::Install( TRequestStatus& aReqStatus, 
                                        RFile& aFile,
                                        const TDesC8& aParams )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    aFile.TransferToServer( args, KFileHandleIpcSlot, KFileSrvSessionIpcSlot );    
    args.Set( KParamsIpcSlot, &aParams );   

    SendReceive( ERequestInstallParamsHandle, args, aReqStatus );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// Launches the silent software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::SilentInstall( const TDesC& aFileName, 
                                              const TDesC8& aOptions )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    args.Set( KFileNameIpcSlot, &aFileName );
    args.Set( KOptionsIpcSlot, &aOptions );       

    return SendReceive( ERequestSilentInstall, args );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// Launches the silent software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::SilentInstall( TRequestStatus& aReqStatus, 
                                              const TDesC& aFileName, 
                                              const TDesC8& aOptions )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    args.Set( KFileNameIpcSlot, &aFileName );
    args.Set( KOptionsIpcSlot, &aOptions );   
    
    SendReceive( ERequestSilentInstall, args, aReqStatus );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// Launches the silent software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::SilentInstall( RFile& aFile,
                                              const TDesC8& aOptions )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    aFile.TransferToServer( args, KFileHandleIpcSlot, KFileSrvSessionIpcSlot );
    args.Set( KOptionsIpcSlot, &aOptions );    

    return SendReceive( ERequestSilentInstallHandle, args );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// Launches the silent software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::SilentInstall( TRequestStatus& aReqStatus, 
                                              RFile& aFile,
                                              const TDesC8& aOptions )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    aFile.TransferToServer( args, KFileHandleIpcSlot, KFileSrvSessionIpcSlot );
    args.Set( KOptionsIpcSlot, &aOptions );   
    
    SendReceive( ERequestSilentInstallHandle, args, aReqStatus );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// Launches the silent software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::SilentInstall( const TDesC& aFileName,
                                              const TDesC8& aParams, 
                                              const TDesC8& aOptions )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );

    TIpcArgs args;
    args.Set( KFileNameIpcSlot, &aFileName );
    args.Set( KParamsIpcSlot, &aParams );   
    args.Set( KOptionsIpcSlot, &aOptions );   

    return SendReceive( ERequestSilentInstallParams, args );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// Launches the silent software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::SilentInstall( TRequestStatus& aReqStatus,
                                              const TDesC& aFileName,
                                              const TDesC8& aParams, 
                                              const TDesC8& aOptions )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    args.Set( KFileNameIpcSlot, &aFileName );
    args.Set( KParamsIpcSlot, &aParams );   
    args.Set( KOptionsIpcSlot, &aOptions );
    
    SendReceive( ERequestSilentInstallParams, args, aReqStatus );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// Launches the silent software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::SilentInstall( RFile& aFile,
                                              const TDesC8& aParams, 
                                              const TDesC8& aOptions )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );

    TIpcArgs args;
    aFile.TransferToServer( args, KFileHandleIpcSlot, KFileSrvSessionIpcSlot );
    args.Set( KParamsIpcSlot, &aParams );
    args.Set( KOptionsIpcSlot, &aOptions );    

    return SendReceive( ERequestSilentInstallParamsHandle, args );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// Launches the silent software install procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::SilentInstall( TRequestStatus& aReqStatus,
                                              RFile& aFile,
                                              const TDesC8& aParams, 
                                              const TDesC8& aOptions )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    aFile.TransferToServer( args, KFileHandleIpcSlot, KFileSrvSessionIpcSlot );
    args.Set( KParamsIpcSlot, &aParams );
    args.Set( KOptionsIpcSlot, &aOptions );    
    
    SendReceive( ERequestSilentInstallParamsHandle, args, aReqStatus );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Uninstall
// Launches the software uninstall procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::Uninstall( const TUid& aUid,
                                          const TDesC8& aMIME )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
       
    TIpcArgs args;
    args.Set( KUidIpcSlot, aUid.iUid );
    args.Set( KMimeIpcSlot, &aMIME );

    return SendReceive( ERequestUninstall, args );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Uninstall
// Launches the software uninstall procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::Uninstall( TRequestStatus& aReqStatus, 
                                          const TUid& aUid,
                                          const TDesC8& aMIME ) 
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
  
    TIpcArgs args;
    args.Set( KUidIpcSlot, aUid.iUid );
    args.Set( KMimeIpcSlot, &aMIME );
    
    SendReceive( ERequestUninstall, args, aReqStatus );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentUninstall
// Launches the silent software uninstall procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::SilentUninstall( const TUid& aUid,
                                                const TDesC8& aOptions,
                                                const TDesC8& aMIME )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );

    TIpcArgs args;
    args.Set( KUidIpcSlot, aUid.iUid );
    args.Set( KOptionsIpcSlot, &aOptions );    
    args.Set( KMimeIpcSlot, &aMIME );

    return SendReceive( ERequestSilentUninstall, args );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentUninstall
// Launches the silent software uninstall procedure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::SilentUninstall( TRequestStatus& aReqStatus,
                                                const TUid& aUid,
                                                const TDesC8& aOptions,
                                                const TDesC8& aMIME )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    args.Set( KUidIpcSlot, aUid.iUid );
    args.Set( KOptionsIpcSlot, &aOptions );    
    args.Set( KMimeIpcSlot, &aMIME );

    SendReceive( ERequestSilentUninstall, args, aReqStatus );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::CancelAsyncRequest
// Cancel Asynchronous requests.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::CancelAsyncRequest( TInt aReqToCancel )
    {
    TIpcArgs args;
    args.Set( KRequestIpcSlot, aReqToCancel );

    return SendReceive( ERequestCancelRequest, args );    
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::CustomUninstall
// Performs a custom uninstallation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::CustomUninstall( TOperation aOperation, 
                                                const TDesC8& aParams,
                                                const TDesC8& aMIME )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    args.Set( KOperationIpcSlot, aOperation );
    args.Set( KCommonParamsIpcSlot, &aParams );   
    args.Set( KMimeIpcSlot, &aMIME ); 

    return SendReceive( ERequestCustomUninstall, args );
    }
  
// -----------------------------------------------------------------------------
// RSWInstLauncher::CustomUninstall
// Performs a custom uninstallation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::CustomUninstall( TRequestStatus& aReqStatus, 
                                                TOperation aOperation, 
                                                const TDesC8& aParams,
                                                const TDesC8& aMIME )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    args.Set( KOperationIpcSlot, aOperation );
    args.Set( KCommonParamsIpcSlot, &aParams );  
    args.Set( KMimeIpcSlot, &aMIME );  

    SendReceive( ERequestCustomUninstall, args, aReqStatus );
    }

EXPORT_C CErrDetails* RSWInstLauncher::GetLastErrorL()
    {
    User::Leave( KErrNotSupported );    
    // Keep the compiler happy
    return NULL;    
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentCustomUninstall
// Performs a silent custom uninstallation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::SilentCustomUninstall( TOperation aOperation, 
                                                      const TDesC8& aOptions,
                                                      const TDesC8& aParams,
                                                      const TDesC8& aMIME )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    args.Set( KOptionsIpcSlot, &aOptions );
    args.Set( KOperationIpcSlot, aOperation );
    args.Set( KCommonParamsIpcSlot, &aParams );   
    args.Set( KMimeIpcSlot, &aMIME ); 

    return SendReceive( ERequestSilentCustomUninstall, args );
    }
  
// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentCustomUninstall
// Performs a silent custom uninstallation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::SilentCustomUninstall( TRequestStatus& aReqStatus,                                                       
                                                      TOperation aOperation, 
                                                      const TDesC8& aOptions,
                                                      const TDesC8& aParams,
                                                      const TDesC8& aMIME )
    {
    __ASSERT_ALWAYS( iConnected, PanicClient( ESWInstPanicBadHandle ) );
    
    TIpcArgs args;
    args.Set( KOptionsIpcSlot, &aOptions );
    args.Set( KOperationIpcSlot, aOperation );
    args.Set( KCommonParamsIpcSlot, &aParams );  
    args.Set( KMimeIpcSlot, &aMIME );  

    SendReceive( ERequestSilentCustomUninstall, args, aReqStatus );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::PanicClient
// Panics the client.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RSWInstLauncher::PanicClient( TInt aPanic ) const
    {  
    _LIT( KSWInstClientFault, "SWInstCli" );
    User::Panic( KSWInstClientFault, aPanic );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::ServiceUid
// Returns the UID of the service that this session provides an interface for.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid RSWInstLauncher::ServiceUid() const
    {
    return TUid::Uid( KSWInstInstallServiceUid );    
    }


//  End of File  
