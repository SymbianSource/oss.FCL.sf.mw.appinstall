/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdateSession class 
*                member functions.
*
*/



// INCLUDE FILES
#include <w32std.h>
#include <apgtask.h>
#include <iaupdateparameters.h>
#include <iaupdateresult.h>

#include "iaupdatesession.h"
#include "iaupdateserver.h"

// Parameters are transmitted from the client side to the server
#include "iaupdatetools.h"
#include "iaupdateclientdefines.h"
#include "iaupdateappui.h"
#include "iaupdatelauncheruids.h"
#include "iaupdatedebug.h"

// CONSTANTS
const TSecureId KSIDBackgroundChecker = 0x200211f4;
const TSecureId KSIDCwrtWidget = 0x200267C0;
const TSecureId KSIDLauncher = KIAUpdateLauncherUid;
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIAUpdateSession::CIAUpdateSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateSession::CIAUpdateSession()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::CIAUpdateSession()");
    iEikEnv = CEikonEnv::Static();
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateSession::CIAUpdateSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateSession::~CIAUpdateSession()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::~CIAUpdateSession() begin");    
    CIAUpdateAppUi* appUi = 
        static_cast<CIAUpdateAppUi*>( iEikEnv->EikAppUi() );
    if ( appUi ) 
        {
    	appUi->HandleObserverRemoval();
        }
    delete iMessage;
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::~CIAUpdateSession() end");  
    }


// -----------------------------------------------------------------------------
// CIAUpdateSession::ServiceL
// Handles the received message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIAUpdateSession::ServiceL( const RMessage2& aMessage )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::ServiceL() begin");
    // Dispatch the message in trap harness.
    TRAPD( result, HandleRequestL( aMessage ) );
    // If HandleRequest Left, the message has to be completed
    // here. Otherwise, the RequestCompleted callback will be called
    // when the operations finishes
    if ( result != KErrNone )   
        {
        IAUPDATE_TRACE_1("[IAUPDATE] HandleRequestL leave with error: %d", result );
        aMessage.Complete( result );
        } 
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::ServiceL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateSession::ServiceError
// Called back by the server framework if this sessions RunL function returns an
// error.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIAUpdateSession::ServiceError( const RMessage2& aMessage, TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::ServiceError() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", aError );
    // A bad descriptor error implies a badly programmed client, so panic it;
    // otherwise use the default handling (report the error to the client)
    if ( aError == KErrBadDescriptor )
        {
        //PanicClient( aMessage, ECIAUpdatePanicBadDescriptor );
        }
    CAknAppServiceBase::ServiceError( aMessage, aError );   
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::ServiceError() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateSession::CreateL
// Completes construction of this server-side client session object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIAUpdateSession::CreateL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::CreateL() begin");
    CAknAppServiceBase::CreateL();   
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::CreateL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateSession::RequestCompleted
// @see MIAUpdateRequestObserver::RequestCompleted
// -----------------------------------------------------------------------------
//
void CIAUpdateSession::RequestCompleted( CIAUpdateResult* aResult, 
                                         TInt aCountOfAvailableUpdates,  
                                         TBool aUpdateNow, 
                                         TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::RequestCompleted() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", aError );

    if ( !iMessage ) //be sure that there is still message to complete
        {
        IAUPDATE_TRACE("[IAUPDATE] No message to complete");
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::RequestCompleted() end");
        return;
        }

    IAUpdateClientDefines::TIAUpdateServerFunctions functionId =
        static_cast<IAUpdateClientDefines::TIAUpdateServerFunctions>( iMessage->Function() );
 
    TInt error( KErrNone );       

    switch( functionId )
        {
        case IAUpdateClientDefines::EIAUpdateServerCheckUpdates:
            {
            TPckg<TInt> availableUpdates( aCountOfAvailableUpdates ); 
            TPtr8 ptr1( availableUpdates );
            TRAP( error, iMessage->WriteL( 1, ptr1 ) );
            break;	
            }
             
        case IAUpdateClientDefines:: EIAUpdateServerShowUpdateQuery:
            {
            TPckg<TInt> updateNow( aUpdateNow ); 
            TPtr8 ptr1( updateNow );
            TRAP( error, iMessage->WriteL( 1, ptr1 ) );
            break;	
            }

        case IAUpdateClientDefines::EIAUpdateServerShowUpdates:
            {
            TPckg<TInt> successCount( aResult->SuccessCount() ); 
            TPtr8 ptr1( successCount ); 
  
            TPckg<TInt> failCount( aResult->FailCount() );
            TPtr8 ptr2( failCount );
         	
            TPckg<TInt> cancelCount( aResult->CancelCount() );
            TPtr8 ptr3( cancelCount );
    
            TRAP( error,
                  iMessage->WriteL( 1, ptr1 );
                  iMessage->WriteL( 2, ptr2 );
                  iMessage->WriteL( 3, ptr3 ); );            
            break;	
            }
        case IAUpdateClientDefines::EIAUpdateServerStartedByLauncher:
            break;    
        default:
            // We should never come here.
            break;
        }            

    if ( error == KErrNone )
        {
        // Writing of the data in to the message went ok.
        // So, nothing to inform about that.
        // Insert possible error code of the update operation.
        error = aError;
        }

    delete aResult;
    aResult = NULL;

    iMessage->Complete( error );
    delete iMessage;
    iMessage = NULL;
                
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::RequestCompleted() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateSession::Server
// Returns reference to the server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CIAUpdateServer& CIAUpdateSession::Server()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::Server()");
    return *static_cast<CIAUpdateServer*>( const_cast<CServer2*>
                                         ( CSession2::Server() ) );
    }


// -----------------------------------------------------------------------------
// CIAUpdateSession::HandleRequestL
// Handles the request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIAUpdateSession::HandleRequestL( const RMessage2& aMessage )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::HandleRequestL() begin");    
    
    CIAUpdateAppUi* appUi = 
        static_cast<CIAUpdateAppUi*>( iEikEnv->EikAppUi() );     
    // First value is the function number
    IAUpdateClientDefines::TIAUpdateServerFunctions functionId =
        static_cast<IAUpdateClientDefines::TIAUpdateServerFunctions>( aMessage.Function() );
    IAUPDATE_TRACE_1("[IAUPDATE] function id: %d", functionId );
      
    if ( functionId == IAUpdateClientDefines::EIAUpdateServerCancel )  
        {
        appUi->PotentialCancel();
        if ( iMessage ) //complete possible ongoing request
            {
        	iMessage->Complete( KErrCancel );
        	delete iMessage;
        	iMessage = NULL;  
            }
        // Because cancel is sent synchronously from the client side,
        // aMessage is different than the iMessage for other operation
        // that is currently going on asynchronously.
        // So, RequestCompleted above closed that connection.
        // Now, inform the caller of the cancel operation
        // that cancellation was success.
        aMessage.Complete( KErrNone );
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::HandleRequestL() end");
    	return;
        }
    
    if ( functionId == IAUpdateClientDefines::EIAUpdateServerToForeground )  
        {
        appUi->HandleClientToForegroundL();
        aMessage.Complete( KErrNone );
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::HandleRequestL() end");
        return;
        }
    
    if ( functionId == IAUpdateClientDefines::EIAUpdateServerSendWgId )  
        {
        TPckgBuf<TInt> wgIdPkg( 0 );
        aMessage.ReadL( 0, wgIdPkg ); 
        TInt wgId = wgIdPkg(); 
        appUi->SetClientWgId( wgId );
        aMessage.Complete( KErrNone );
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::HandleRequestL() end");
        return;
        }
    
        
    if ( iMessage )
        {
        // This session is used in a wrong way because the
        // old operation has not been completed before new one is
        // started.
        User::Leave( KErrInUse );
        }
        
    iMessage = new( ELeave ) RMessage2( aMessage );
    if ( functionId == IAUpdateClientDefines::EIAUpdateServerStartedByLauncher )
        {
        TPckgBuf<TBool> refreshFromNetworkDeniedPkg( 0 );
        aMessage.ReadL( 0, refreshFromNetworkDeniedPkg ); 
        TBool refreshFromNetworkDenied = refreshFromNetworkDeniedPkg(); 
    	appUi->StartedByLauncherL( *this, refreshFromNetworkDenied );
        }
    else if ( functionId == IAUpdateClientDefines::EIAUpdateServerShowUpdateQuery )  
        {
        // Handle the update query dialog request
        TUint id = aMessage.SecureId().iId;
    	appUi->ShowUpdateQueryRequestL( *this, id ); 
        }
    else
        {
        // Handle update operation related requests.
        HBufC8* data = NULL;
        data = HBufC8::NewLC( aMessage.GetDesLength( 0 ) );
        TPtr8 ptr( data->Des() );            
        aMessage.ReadL( 0, ptr );
    
        CIAUpdateParameters* params( CIAUpdateParameters::NewLC() );
        IAUpdateTools::InternalizeParametersL( *params, *data );
        // Do not destroy, because ownership of params is transferred
        // below.
        CleanupStack::Pop( params );
        CleanupStack::PopAndDestroy( data );
        
        if ( ( aMessage.SecureId() != KSIDBackgroundChecker ) && ( aMessage.SecureId() != KSIDCwrtWidget ) )      
            {
            // other processes than backroundchecker are not allowed to cause refresh from network 
            params->SetRefresh( EFalse );
            }
        switch( functionId )
            {
            case IAUpdateClientDefines::EIAUpdateServerCheckUpdates:
                appUi->CheckUpdatesRequestL( *this, params, params->Refresh() && aMessage.SecureId() == KSIDCwrtWidget );
                break;

            case IAUpdateClientDefines::EIAUpdateServerShowUpdates:
                appUi->ShowUpdatesRequestL( *this, params );
                break;
                    
            default:
            // We should never come here
                delete params;
                params = NULL;
                break;
            }    
        }
     IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::HandleRequestL() end"); 
     }    


// -----------------------------------------------------------------------------
// CIAUpdateSession::SecurityCheckL
// Virtual framework function that is called on receipt of a message from the
// client. This allows the service implementation to define a security policy
// for messages from the client.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPolicyServer::TCustomResult CIAUpdateSession::SecurityCheckL( const RMessage2& aMsg,
                                                             TInt& /*aAction*/,
                                                             TSecurityInfo& /*aMissing*/ )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::SecurityCheckL() begin");
    
    CPolicyServer::TCustomResult result = CPolicyServer::EFail; 
    switch ( aMsg.Function() )
        {
        case IAUpdateClientDefines::EIAUpdateServerStartedByLauncher:
            if ( aMsg.SecureId() == KSIDLauncher )  
                {
                result = CPolicyServer::EPass;
                }
            break;
        case IAUpdateClientDefines::EIAUpdateServerCheckUpdates:
        case IAUpdateClientDefines::EIAUpdateServerShowUpdates:
        // no capability check is needed because SID is checked later
        // that means that only bacgkroundchecker is allowed to initiate network refresh
            
        case IAUpdateClientDefines::EIAUpdateServerShowUpdateQuery:
        case IAUpdateClientDefines::EIAUpdateServerCancel:
        case IAUpdateClientDefines::EIAUpdateServerToForeground:
        case IAUpdateClientDefines::EIAUpdateServerSendWgId:
        // No capabilities needed
            result = CPolicyServer::EPass;
            break;

        default:
            // Not recognized message
            result = CPolicyServer::EFail;
        }
    IAUPDATE_TRACE_1("[IAUPDATE] result: %d", result );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSession::SecurityCheckL() end");
    return result; 
    }


//  End of File  
