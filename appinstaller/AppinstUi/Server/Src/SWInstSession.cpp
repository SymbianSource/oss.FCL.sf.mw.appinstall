/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CSWInstSession class 
*                member functions.
*
*/


// INCLUDE FILES
#include <ecom/ecom.h>
#include <w32std.h>
#include <apgtask.h>
#include <s32mem.h> //RDesReadStream 

#include "SWInstSession.h"
#include "SWInstServer.h"
#include "SWInstServerPanic.h"
#include "SWInstInstallRequest.h"
#include "SWInstUninstallRequest.h"
#include "SWInstRequestStore.h"
#include "SWInstPrivateUid.h"

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSWInstSession::CSWInstSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSWInstSession::CSWInstSession()
    {
    }
    
// Destructor
CSWInstSession::~CSWInstSession()
    {   
    if ( iCanceller )
        {
        iCanceller->Cancel();
        delete iCanceller;
        }    
         
    delete iRequestStore;    
    // Call final close for ecom    
    REComSession::FinalClose();  
      
    delete iHeapSourceUrl;   
    }

// -----------------------------------------------------------------------------
// CSWInstSession::PrepareForExit
// Prepare the session for exit.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstSession::PrepareForExit( MRequestCallback* aExitCallback )
    {
    iExitCallback = aExitCallback;

    // If we have any pending requests, cancel them all
    if ( iRequestStore )
        {
        iRequestStore->CancelAllRequests();
        }

    if ( iExitCallback )
        {
        iExitCallback->RequestCompleted( KErrNone ); 
        iExitCallback = NULL;        
        }    
    }

// -----------------------------------------------------------------------------
// CSWInstSession::ServiceL
// Handles the received message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstSession::ServiceL( const RMessage2& aMessage )
    {
    // Dispatch the message in trap harness.
    TRAPD( result, HandleRequestL( aMessage ) );
    
    if ( result == KErrBadDescriptor )
        {
        PanicClient( aMessage, ESWInstPanicBadDescriptor );
        }    

    // In case of leave, we need to complete the client request here
    else if ( result != KErrNone )
        {
        if ( iRequestStore )
            {
            CSWInstRequestObject* req = iRequestStore->GetRequest( aMessage.Function() );
            if ( req && req->IsActive() )
                {
                req->Cancel();                
                }            
            else 
                {
                // No active request found, we must complete the message manually
                aMessage.Complete( result );
                }            
            } 
        else
            {
            aMessage.Complete( result );
            }        
        }
    }

// -----------------------------------------------------------------------------
// CSWInstSession::ServiceError
// Called back by the server framework if this sessions RunL function returns an
// error.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstSession::ServiceError( const RMessage2& aMessage, TInt aError )
    {
    // A bad descriptor error implies a badly programmed client, so panic it;
    // otherwise use the default handling (report the error to the client)
    if ( aError == KErrBadDescriptor )
        {
        PanicClient( aMessage, ESWInstPanicBadDescriptor );
        }
    CAknAppServiceBase::ServiceError( aMessage, aError );    
    }

// -----------------------------------------------------------------------------
// CSWInstSession::CreateL
// Completes construction of this server-side client session object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstSession::CreateL()
    {
    iRequestStore = CSWInstRequestStore::NewL();  
    CAknAppServiceBase::CreateL();   
    iAppInForeground = ETrue; 
    }

// -----------------------------------------------------------------------------
// CSWInstSession::RequestCompleted
// Called when request is completed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstSession::RequestCompleted( TInt /*aResult*/ )
    {
    // Delete completed requests
    iRequestStore->Flush();
    TInt pendingRequests = iRequestStore->PendingRequestCount();

    // See if we need to notify this completion
    if( iExitCallback && !pendingRequests )
        {
        iExitCallback->RequestCompleted( KErrNone );
        iExitCallback = NULL;
        }    
    }

// -----------------------------------------------------------------------------
// CSWInstSession::Server
// Returns reference to the server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSWInstServer& CSWInstSession::Server()
    {
    return *static_cast<CSWInstServer*>( const_cast<CServer2*>
                                         ( CSession2::Server() ) );
    }

// -----------------------------------------------------------------------------
// CSWInstSession::HandleRequestL
// Handles the request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstSession::HandleRequestL( const RMessage2& aMessage )
    {       
    TServerRequest opCode( ( TServerRequest ) aMessage.Function() );  
    
    // Send application to background if installation is silent. This way
    // we do not block other apps if installation takes long time.   
    if ( opCode == ERequestSilentInstall && iAppInForeground || 
         opCode == ERequestSilentInstallParams && iAppInForeground ||
         opCode == ERequestSilentInstallHandle && iAppInForeground ||
         opCode == ERequestSilentInstallParamsHandle && iAppInForeground )
        {        
        SendAppToBackgroundL();                                
        }      

    // Handle requests for the session.
    switch ( opCode ) 
        {
        case ERequestCancelRequest:
            {
            // We need to cancel and destroy the corresponding request. 
            TInt functionToCancel( GetInt( aMessage, KRequestIpcSlot ) );            
            CSWInstRequestObject* requestObj = iRequestStore->GetRequest( functionToCancel );
            if ( requestObj && requestObj->IsActive() )
                {
                requestObj->Cancel( aMessage );
                }
            else
                {                
                // If we are cancelling we may have a dialog waiting. We need to 
                // use this special method to cancel dialog before exit.
                // See error TSW ELBA-78SBTS
                TRAP_IGNORE(
                    iCanceller = CommonUI::CCUICancelTimer::NewL( this );
                    iCanceller->StartCancelling();            
                    );
                                     
                aMessage.Complete( KErrNotFound );
                }           

            break;                
            }                    

        case ERequestInstall:
        case ERequestInstallParams:
        case ERequestSilentInstall:
        case ERequestSilentInstallParams:
            {          
            // Delete completed requests
            iRequestStore->Flush();
                
            CSWInstInstallRequest* installRequestObj = CSWInstInstallRequest::NewL( aMessage );
            CleanupStack::PushL( installRequestObj );
            // If store is full, this leaves with KSWInstErrBusy
            iRequestStore->AddRequestL( installRequestObj );  // Ownership transfers to store
            CleanupStack::Pop( installRequestObj );

            TFileName fileName;            
            TInstallReq params;

            // Parse filename
            __ASSERT_ALWAYS( aMessage.GetDesLength( KFileNameIpcSlot ) > 0, 
                             User::Leave( KErrBadDescriptor ) );

            aMessage.ReadL( KFileNameIpcSlot, fileName );                

            if ( opCode == ERequestInstall )
                {                
                installRequestObj->Install( fileName, params );                
                }            

            else if( opCode == ERequestSilentInstall )
                {            
                __ASSERT_ALWAYS( aMessage.GetDesLength( KOptionsIpcSlot ) > 0, 
                                 User::Leave( KErrBadDescriptor ) );
            
                // Parse install options
                TInstallOptions options;
                TPckg<TInstallOptions> optionsPckg( options );            
                aMessage.ReadL( KOptionsIpcSlot, optionsPckg );

                installRequestObj->SilentInstall( fileName, params, options );
                }
            
            else if ( opCode == ERequestInstallParams )
                {                              
                //TSW JROL-7B8K6H            
                TPckg<TInstallReq> paramsPckg( params );        
                TInt desLength = aMessage.GetDesLength( KParamsIpcSlot );                 				
                __ASSERT_ALWAYS( desLength > 0, 
                        User::Leave( KErrBadDescriptor ) );                          
                    
                // If length is more then sizeof TInstallReqPckg, input data 
                // is stored to to heap. Read data to descriptor.                                  
                if ( desLength > sizeof( TInstallReq ) )
                    {                       	          	
                    HBufC8* paramDescriptor = HBufC8::NewLC( desLength );
                    TPtr8 paramDesPtr = paramDescriptor->Des();						                	                                                                
                    aMessage.ReadL( KParamsIpcSlot, paramDesPtr );                  	
                    // Get install parameters and store URL to heap. 
                    GetReqParamsL( *paramDescriptor, params );					
                    CleanupStack::PopAndDestroy( paramDescriptor ); 		
                    }
                else
                    {             	
                    aMessage.ReadL( KParamsIpcSlot, paramsPckg );	
                    }	

                installRequestObj->Install( fileName, params );
                }
            
            else if ( opCode == ERequestSilentInstallParams )
                {                            
                __ASSERT_ALWAYS( aMessage.GetDesLength( KParamsIpcSlot ) > 0 && 
                                 aMessage.GetDesLength( KOptionsIpcSlot ) > 0, 
                                 User::Leave( KErrBadDescriptor ) );

                //TSW JROL-7B8K6H 
                TPckg<TInstallReq> paramsPckg( params );        
                TInt desLength = aMessage.GetDesLength( KParamsIpcSlot );                 
                     			                                          
                // If length is more then sizeof TInstallReqPckg, input data 
                // is stored to to heap. Read data to descriptor.                                  
                if ( desLength > sizeof( TInstallReq ) )
                    {                       	          	
                    HBufC8* paramDescriptor = HBufC8::NewLC( desLength );
                    TPtr8 paramDesPtr = paramDescriptor->Des();						                	                                                                
                    aMessage.ReadL( KParamsIpcSlot, paramDesPtr );                  	
                    // Get install parameters and store URL to heap. 
                    GetReqParamsL( *paramDescriptor, params );					
                    CleanupStack::PopAndDestroy( paramDescriptor ); 		
                    } 
                // Read params to pckg if lenght is more then 0.	               
                else
                    {             	
                    aMessage.ReadL( KParamsIpcSlot, paramsPckg );	
                    }	
               
                // Parse install options
                TInstallOptions options;
                TPckg<TInstallOptions> optionsPckg( options );            
                aMessage.ReadL( KOptionsIpcSlot, optionsPckg );                                 

                installRequestObj->SilentInstall( fileName, params, options );
                }

            break;            
            }

        case ERequestInstallHandle:
        case ERequestInstallParamsHandle:
        case ERequestSilentInstallHandle:
        case ERequestSilentInstallParamsHandle:
            {
            // Delete completed requests
            iRequestStore->Flush();
                
            CSWInstInstallRequest* installRequestObj = CSWInstInstallRequest::NewL( aMessage );
            CleanupStack::PushL( installRequestObj );
            // If store is full, this leaves with KSWInstErrBusy
            iRequestStore->AddRequestL( installRequestObj );  // Ownership transfers to store
            CleanupStack::Pop( installRequestObj );

            RFile file;            
            TInstallReq params;

            // Get the file handle
            User::LeaveIfError( file.AdoptFromClient( aMessage, KFileHandleIpcSlot, KFileSrvSessionIpcSlot ) );
            CleanupClosePushL( file );            
            
            if ( opCode == ERequestInstallHandle )
                {                
                installRequestObj->Install( file, params ); // Ownership of file transfers
                CleanupStack::Pop(); // file
                }            

            else if( opCode == ERequestSilentInstallHandle )
                {            
                __ASSERT_ALWAYS( aMessage.GetDesLength( KOptionsIpcSlot ) > 0, 
                                 User::Leave( KErrBadDescriptor ) );
            
                // Parse install options
                TInstallOptions options;
                TPckg<TInstallOptions> optionsPckg( options );            
                aMessage.ReadL( KOptionsIpcSlot, optionsPckg );

                installRequestObj->SilentInstall( file, params, options ); // Ownership of file transfers
                CleanupStack::Pop(); // file
                }
            
            else if ( opCode == ERequestInstallParamsHandle )
                {                        
                TPckg<TInstallReq> paramsPckg( params );        
                TInt desLength = aMessage.GetDesLength( KParamsIpcSlot );                 				              
                __ASSERT_ALWAYS( desLength > 0, 
                        User::Leave( KErrBadDescriptor ) );                          
                    
                // If length is more then sizeof TInstallReqPckg, input data 
                // is stored to to heap. Read data to descriptor.                                           
                if ( desLength > sizeof( TInstallReq ) )
                    {                       	          	
                    HBufC8* paramDescriptor = HBufC8::NewLC( desLength );
                    TPtr8 paramDesPtr = paramDescriptor->Des();						                	                                                                
                    aMessage.ReadL( KParamsIpcSlot, paramDesPtr );                  	
                    // Get install parameters and store URL to heap. 
                    GetReqParamsL( *paramDescriptor, params );					
                    CleanupStack::PopAndDestroy( paramDescriptor ); 		
                    }
                else 
                    {             	
                    aMessage.ReadL( KParamsIpcSlot, paramsPckg );	
                    }	

                installRequestObj->Install( file, params ); // Ownership of file transfers
                CleanupStack::Pop(); // file                
                }
            
            else if ( opCode == ERequestSilentInstallParamsHandle )
                {                          
                __ASSERT_ALWAYS( aMessage.GetDesLength( KParamsIpcSlot ) > 0 && 
                                 aMessage.GetDesLength( KOptionsIpcSlot ) > 0, 
                                 User::Leave( KErrBadDescriptor ) );

                TPckg<TInstallReq> paramsPckg( params );        
                TInt desLength = aMessage.GetDesLength( KParamsIpcSlot );                 
                     			                                          
                // If length is more then sizeof TInstallReqPckg, input data 
                // is stored to to heap. Read data to descriptor.                                  
                if ( desLength > sizeof( TInstallReq ) )
                    {                       	          	
                    HBufC8* paramDescriptor = HBufC8::NewLC( desLength );
                    TPtr8 paramDesPtr = paramDescriptor->Des();						                	                                                                
                    aMessage.ReadL( KParamsIpcSlot, paramDesPtr );                  	
                    // Get install parameters and store URL to heap. 
                    GetReqParamsL( *paramDescriptor, params );					
                    CleanupStack::PopAndDestroy( paramDescriptor ); 		
                    }
                else 
                    {             	
                    aMessage.ReadL( KParamsIpcSlot, paramsPckg );	
                    }	
               
                // Parse install options
                TInstallOptions options;
                TPckg<TInstallOptions> optionsPckg( options );
                aMessage.ReadL( KOptionsIpcSlot, optionsPckg );

                installRequestObj->SilentInstall( file, params, options ); // Ownership of file transfers
                CleanupStack::Pop(); // file
                }

            break;            
            }
            
        case ERequestUninstall:
        case ERequestSilentUninstall:
            {
            // Delete completed requests
            iRequestStore->Flush();

            CSWInstUninstallRequest* uninstRequestObj = CSWInstUninstallRequest::NewL( aMessage );
            CleanupStack::PushL( uninstRequestObj );                
            // If store is full, this leaves with KSWInstErrBusy
            iRequestStore->AddRequestL( uninstRequestObj );  // Ownership transfers to store
            CleanupStack::Pop( uninstRequestObj );  
           
            TUid uid;
            HBufC8* mime = NULL;
        
            if ( opCode == ERequestUninstall )
                {            
                 __ASSERT_ALWAYS( aMessage.GetDesLength( KMimeIpcSlot ) > 0, 
                             User::Leave( KErrBadDescriptor ) );

                // Parse Uid            
                uid.iUid = GetInt( aMessage, KUidIpcSlot );                

                // Parse the mime
                mime = HBufC8::NewLC( aMessage.GetDesLength( KMimeIpcSlot ) );
                TPtr8 ptr( mime->Des() );            
                aMessage.ReadL( KMimeIpcSlot, ptr );
            
                uninstRequestObj->Uninstall( uid, *mime );
                CleanupStack::PopAndDestroy( mime );            
                }
            
            else if ( opCode == ERequestSilentUninstall )
                {            
                __ASSERT_ALWAYS( aMessage.GetDesLength( KOptionsIpcSlot ) > 0 && 
                                 aMessage.GetDesLength( KMimeIpcSlot ) > 0, 
                                 User::Leave( KErrBadDescriptor ) );
            
                // Parse Uid                
                uid.iUid = GetInt( aMessage, KUidIpcSlot );                

                TUninstallOptions options;            
                // Parse uninstall options
                TPckg<TUninstallOptions> optionsPckg( options );
                aMessage.ReadL( KOptionsIpcSlot, optionsPckg );

                // Parse the mime
                mime = HBufC8::NewLC( aMessage.GetDesLength( KMimeIpcSlot ) );
                TPtr8 ptr( mime->Des() );            
                aMessage.ReadL( KMimeIpcSlot, ptr );

                uninstRequestObj->SilentUninstall( uid, *mime, options );
                CleanupStack::PopAndDestroy( mime );
                }

            break;        
            }            

        case ERequestCustomUninstall:
        case ERequestSilentCustomUninstall:
            {
            __ASSERT_ALWAYS( aMessage.GetDesLength( KCommonParamsIpcSlot ) > 0 &&
                             aMessage.GetDesLength( KMimeIpcSlot ) > 0,
                             User::Leave( KErrBadDescriptor ) );

             // Get the operation we need to perform
            TOperation operation = (TOperation) GetInt( aMessage, KOperationIpcSlot );

            // Get the params
            HBufC8* params = HBufC8::NewLC( aMessage.GetDesLength( KCommonParamsIpcSlot ) );
            TPtr8 paramPtr( params->Des() );            
            aMessage.ReadL( KCommonParamsIpcSlot, paramPtr );

            // Parse the mime
            HBufC8* mime = HBufC8::NewLC( aMessage.GetDesLength( KMimeIpcSlot ) );
            TPtr8 mimePtr( mime->Des() );            
            aMessage.ReadL( KMimeIpcSlot, mimePtr );

            // Delete completed requests
            iRequestStore->Flush();
            
            CSWInstUninstallRequest* uninstRequestObj = CSWInstUninstallRequest::NewL( aMessage );
            CleanupStack::PushL( uninstRequestObj );                
            // If store is full, this leaves with KSWInstErrBusy
            iRequestStore->AddRequestL( uninstRequestObj );  // Ownership transfers to store
            CleanupStack::Pop( uninstRequestObj );  
            
            if ( opCode == ERequestSilentCustomUninstall )
                {
                __ASSERT_ALWAYS( aMessage.GetDesLength( KOptionsIpcSlot ) > 0,
                                 User::Leave( KErrBadDescriptor ) );

                TUninstallOptions options;            
                // Parse uninstall options
                TPckg<TUninstallOptions> optionsPckg( options );
                aMessage.ReadL( KOptionsIpcSlot, optionsPckg );

                uninstRequestObj->SilentCustomUninstall( operation, options, *params, *mime );
                }
            else
                {                
                uninstRequestObj->CustomUninstall( operation, *params, *mime );
                }            

            CleanupStack::PopAndDestroy( 2, params );            
            }
            
            break;          

        default:
            CAknAppServiceBase::ServiceL( aMessage );
            break;            
        }
       
    }    

// -----------------------------------------------------------------------------
// CSWInstSession::SecurityCheckL
// Virtual framework function that is called on receipt of a message from the 
// client. This allows the service implementation to define a security policy 
// for messages from the client. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPolicyServer::TCustomResult CSWInstSession::SecurityCheckL( const RMessage2& aMsg, 
                                                             TInt& aAction, 
                                                             TSecurityInfo& aMissing )
    {
    switch ( aMsg.Function() )
        {
        case ERequestSilentInstall:
        case ERequestSilentInstallParams:
        case ERequestSilentUninstall:
        case ERequestSilentInstallHandle:
        case ERequestSilentInstallParamsHandle:

            if ( aMsg.HasCapability( TCapability( ECapabilityTrustedUI ) ) )
                {
                // AllFiles capability required to install untrusted SW silently.
                if( aMsg.Function() != ERequestSilentUninstall )
                    {
                    TInstallOptions options;
                    TPckg<TInstallOptions> optionsPckg( options );
                    aMsg.ReadL( KOptionsIpcSlot, optionsPckg );
                    if( options.iUntrusted == EPolicyAllowed )
                        {
                        if( !aMsg.HasCapability( TCapability( ECapabilityAllFiles ) ) )
                            {
                            return CPolicyServer::EFail;
                            }
                        }
                    }

                return CPolicyServer::EPass;
                }
            else
                {
                return CPolicyServer::EFail;
                }            

        case ERequestInstall:
        case ERequestInstallParams:
        case ERequestUninstall:
        case ERequestInstallHandle:
        case ERequestInstallParamsHandle:
        case ERequestCancelRequest:
            // No capabilities needed for these
            return CPolicyServer::EPass;            

        default:
            // Not recognized message, pass to CAknAppServiceBase
            return CAknAppServiceBase::SecurityCheckL( aMsg, aAction, aMissing );            
        }    
    }

// -----------------------------------------------------------------------------
// CSWInstSession::GetInt
// Helper to retrieve an int parameter from message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSWInstSession::GetInt( const RMessage2& aMessage, TInt aIpcSlot )
    {
    switch ( aIpcSlot )
        {
        case 0:
            return aMessage.Int0();
        case 1:
            return aMessage.Int1();
        case 2:
            return aMessage.Int2();
        case 3:
            return aMessage.Int3();              
        default:
            return 0;            
        }    
    }

// -----------------------------------------------------------------------------
// CSWInstSession::SendAppToBackgroundL
// Sends application to background. This is used in silent install to prevent
// blocking of other applications UI.
// -----------------------------------------------------------------------------
//
void CSWInstSession::SendAppToBackgroundL()
    {    	                                                
    RWsSession ws;   
                                             
    if ( ws.Connect() == KErrNone )
        {
        CleanupClosePushL(ws);        
        TApaTaskList tasklist(ws);   
                 
        TApaTask task = tasklist.FindApp( TUid::Uid(KSWInstSvrUid) );
                        
        if ( task.Exists() )
            {                                       
            task.SendToBackground();              
            // Set to false, so we do not do this again.            
            iAppInForeground = EFalse;                                                      
            } 
                                                               
        CleanupStack::PopAndDestroy(); //ws       
        }                           
    }


// -----------------------------------------------------------------------------
// CSWInstSession::IsShowingDialog()
// From MCUICancellable. Function returns always ETrue so StartCancelling() 
// function will start cancelling the dialog.
// -----------------------------------------------------------------------------
//
TBool CSWInstSession::IsShowingDialog()
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSWInstSession::CancelEngine()
// From MCUICancellable. 
// -----------------------------------------------------------------------------
//
void CSWInstSession::CancelEngine()
    {    
    }

// -----------------------------------------------------------------------------
// CSWInstSession::ForceCancel()
// From MCUICancellable. 
// -----------------------------------------------------------------------------
//          
void CSWInstSession::ForceCancel()
    {        
    }

// -----------------------------------------------------------------------------
// CSWInstSession::StartedCancellingL()
// From MCUICancellable. 
// -----------------------------------------------------------------------------
//
void CSWInstSession::StartedCancellingL()
    {    
    }

// -----------------------------------------------------------------------------
// CSWInstSession::GetReqParamsL
// Reads parameters from descriptor to TInstallReg and buffer.
// -----------------------------------------------------------------------------
//    
void CSWInstSession::GetReqParamsL( TDesC8& aParamsDes, TInstallReq& aParams )	
    {   
    // Delete previous URL.
    delete iHeapSourceUrl;
    iHeapSourceUrl = NULL; 

    RDesReadStream paramStream( aParamsDes );
    CleanupClosePushL( paramStream );

    TPckg<TInstallReq> paramsPckg( aParams );
    // Read install parameters to pckg.
    paramStream.ReadL( paramsPckg );
    // Get length of url
    TInt urlLength = paramStream.ReadInt32L();
    
    
    // Check the url lenght. 
    // Note that given url length may be zero in some cases.
    if( urlLength )
        {         
        iHeapSourceUrl = HBufC::NewL( urlLength );	
        TPtr urlPtr = iHeapSourceUrl->Des();        
        // Get url to heap descriptor
        paramStream.ReadL( urlPtr, urlLength );        	
        // Set url's pointer.		
        TInstallReqHeapURL heapURL;                                           								                                                                                        
        heapURL.iSourcePtr = iHeapSourceUrl;            

        TInstallReqURLPckg heapPckg( heapURL );         	
        // Copy url pointer to TInstallReg params.     	         	            
        aParams.iSourceURL.Copy( heapPckg );            
        }

    CleanupStack::PopAndDestroy( &paramStream );              				
    }        
    
//  End of File  
