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
* Description:   This module contains the implementation of RIAUpdateClient
*                class member functions.
*
*/



//INCLUDES
#include <eikenv.h>
#include <apgcli.h>
#include <e32math.h>
#include "iaupdateclient.h"
#include "iaupdateclientdefines.h"
#include "iaupdatetools.h"
#include "iaupdatedebug.h"


// -----------------------------------------------------------------------------
// RIAUpdateClient::RIAUpdateClient
// 
// -----------------------------------------------------------------------------
// 
RIAUpdateClient::RIAUpdateClient() 
: iPtr1( NULL, 0 ),
  iPtr2( NULL, 0 ),
  iPtr3( NULL, 0 )
    {
    }


// -----------------------------------------------------------------------------
// RIAUpdateClient::Open
// 
// -----------------------------------------------------------------------------
// 
TInt RIAUpdateClient::Open()
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::Open() begin");
    TInt error( KErrNone );
    
    if ( !iConnected )
        {
        TRAP( error, ConnectNewAppL( ServiceUid() ) );
        if ( error == KErrNone ) 
            {
            iConnected = ETrue;
            CEikonEnv* eikEnv = CEikonEnv::Static();
            if ( eikEnv )
                {
            	RWindowGroup owngroup;
		        iOwnWgId = eikEnv->RootWin().Identifier(); 
		        
	            TPckg<TInt> wgId( iOwnWgId );
	            delete iData;
	            iData = NULL;
	             TRAP_IGNORE( iData = wgId.AllocL() );
	            
                TIpcArgs args;
                args.Set( 0, iData );
                SendReceive( IAUpdateClientDefines::EIAUpdateServerSendWgId, args );    
	            } 
            }
        }
    IAUPDATE_TRACE_1("[IAUPDATE] RIAUpdateClient::Open() end error code: %d", error );
    return error;
    }

// -----------------------------------------------------------------------------
// RIAUpdateClient::OpenToBackroundAsync
// 
// -----------------------------------------------------------------------------
//
TInt RIAUpdateClient::OpenToBackroundAsync( TRequestStatus& aStatus )
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::OpenToBackroundAsync() begin");
    TInt error( KErrNone );
    if ( !iConnected )
        {
        TRAP( error, StartNewAppToBackgroundL( ServiceUid(), aStatus ) );
        }
    IAUPDATE_TRACE_1("[IAUPDATE] RIAUpdateClient::OpenToBackroundAsync() end error code: %d", error );
    return error;
    }

// -----------------------------------------------------------------------------
// RIAUpdateClient::ConnectToApp
// 
// -----------------------------------------------------------------------------
//
TInt RIAUpdateClient::ConnectToApp()
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::ConnectToApp() begin");
    TInt error( KErrNone );
    TName serverName;
    ServerName(serverName, ServiceUid(), iDifferentiator);
    TRAP( error,ConnectExistingByNameL( serverName ) );
    
    if ( error == KErrNone ) 
        {
        iConnected = ETrue;
        CEikonEnv* eikEnv = CEikonEnv::Static();
        if ( eikEnv )
            {
            RWindowGroup owngroup;
            iOwnWgId = eikEnv->RootWin().Identifier(); 
                    
            TPckg<TInt> wgId( iOwnWgId );
            delete iData;
            iData = NULL;
            TRAP_IGNORE( iData = wgId.AllocL() );
                    
            TIpcArgs args;
            args.Set( 0, iData );
            SendReceive( IAUpdateClientDefines::EIAUpdateServerSendWgId, args );    
            } 
        }
    IAUPDATE_TRACE_1("[IAUPDATE] RIAUpdateClient::ConnectToApp() end error code: %d", error );
    return error; 
    }




// -----------------------------------------------------------------------------
// RIAUpdateClient::Close
// 
// -----------------------------------------------------------------------------
//
void RIAUpdateClient::Close()
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::Close() begin");
    // Let the parent handle closing.
    REikAppServiceBase::Close();
    iConnected = EFalse;
    delete iData;
    iData = NULL;
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::Close() end");
    }

// -----------------------------------------------------------------------------
// RIAUpdateClient::CheckUpdates
// 
// -----------------------------------------------------------------------------
//
TInt RIAUpdateClient::CheckUpdates( const CIAUpdateParameters& aUpdateParameters,
                                    TInt& aAvailableUpdates, 
                                    TRequestStatus& aStatus )
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::CheckUpdates()");
    // Inform the caller about the success of the request initializing.
    return SendCheckUpdatesRequest( IAUpdateClientDefines::EIAUpdateServerCheckUpdates,
                                    aUpdateParameters,
                                    aAvailableUpdates,
                                    aStatus );
    }

// -----------------------------------------------------------------------------
// RIAUpdateClient::ShowUpdates
// 
// -----------------------------------------------------------------------------
//
TInt RIAUpdateClient::ShowUpdates( const CIAUpdateParameters& aUpdateParameters,
                                   TInt& aNumberOfSuccessfullUpdates,
                                   TInt& aNumberOfFailedUpdates,
                                   TInt& aNumberOfCancelledUpdates,
                                   TRequestStatus& aStatus )
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::ShowUpdates()");
    // Inform the caller about the success of the request initializing.
    return SendUpdateRequest( IAUpdateClientDefines::EIAUpdateServerShowUpdates,
                              aUpdateParameters,
                              aNumberOfSuccessfullUpdates,
                              aNumberOfFailedUpdates,
                              aNumberOfCancelledUpdates,
                              aStatus );
    }
    

// -----------------------------------------------------------------------------
// RIAUpdateClient::UpdateQuery
// 
// -----------------------------------------------------------------------------
//    
TInt RIAUpdateClient::UpdateQuery( TBool& aUpdateNow, TRequestStatus& aStatus )
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::UpdateQuery() begin");
	TPckg<TBool> updateNow( aUpdateNow );
	iPtr1.Set( updateNow );
	            
    TIpcArgs args;
    args.Set( 1, &iPtr1 );
    
	SendReceive( IAUpdateClientDefines::EIAUpdateServerShowUpdateQuery, 
	             args, 
	             aStatus );    
	IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::UpdateQuery() begin");    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RIAUpdateClient::BroughtToForeground
// 
// -----------------------------------------------------------------------------
//
void RIAUpdateClient::BroughtToForeground()
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::BroughtToForeground() begin");
    if ( iConnected )
        {
    	SendReceive( IAUpdateClientDefines::EIAUpdateServerToForeground );
        }
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::BroughtToForeground() end");
    }


// -----------------------------------------------------------------------------
// RIAUpdateClient::CancelAsyncRequest
// 
// -----------------------------------------------------------------------------
//
void RIAUpdateClient::CancelAsyncRequest()
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::CancelAsyncRequest() begin");
    // We suppose that an active object will
    // wait for the cancellation to complete.
    // So, let the server know that operation is cancelled.
    // The server should set the correct status and complete
    // the request of the active object. So, the cancellation 
    // can proceed to the end.
    if ( iConnected )
        {
    	SendReceive( IAUpdateClientDefines::EIAUpdateServerCancel ); 
        }
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::CancelAsyncRequest() end");
    }



// -----------------------------------------------------------------------------
// RIAUpdateClient::ServiceUid()
// 
// -----------------------------------------------------------------------------
//
TUid RIAUpdateClient::ServiceUid() const
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::ServiceUid()");
    return IAUpdateClientDefines::KIAUpdateServiceUid;    
    }

// -----------------------------------------------------------------------------
// RIAUpdateClient::SendCheckUpdatesRequest
// 
// -----------------------------------------------------------------------------
//
TInt RIAUpdateClient::SendCheckUpdatesRequest( TInt aUpdateFunction,
                                               const CIAUpdateParameters& aUpdateParameters,
                                               TInt& aCount,
                                               TRequestStatus& aStatus )
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::SendCheckUpdatesRequest() begin");
    
    aStatus = KRequestPending;

    delete iData;
    iData = NULL;   
    
    TInt error( KErrNone );
    TRAP( error, 
          IAUpdateTools::ExternalizeParametersL( iData, 
                                                 aUpdateParameters ) );
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", error );

    // Because this function does not leave,
    // use error value to check if request can be done.
    if ( error == KErrNone )
        {
        TPckg<TInt> count( aCount );
	    iPtr1.Set( count );
	            
        TIpcArgs args;
        args.Set( 0, iData );
        args.Set( 1, &iPtr1 );
              
        // Start the asynchronous operation in the server side.
        SendReceive( aUpdateFunction, args, aStatus );        
        }

    // Inform the caller about the success of the request initializing.
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::SendCheckUpdatesRequest() begin");
    return error;
    }

// -----------------------------------------------------------------------------
// RIAUpdateClient::SendUpdateRequest
// 
// -----------------------------------------------------------------------------
//
TInt RIAUpdateClient::SendUpdateRequest( TInt aUpdateFunction,
                                         const CIAUpdateParameters& aUpdateParameters,
                                         TInt& aNumberOfSuccessfullUpdates,
                                         TInt& aNumberOfFailedUpdates,
                                         TInt& aNumberOfCancelledUpdates,
                                         TRequestStatus& aStatus )
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::SendUpdateRequest() begin");
    aStatus = KRequestPending;

    delete iData;
    iData = NULL;   
    
    TInt error( KErrNone );
    TRAP( error, 
          IAUpdateTools::ExternalizeParametersL( iData, 
                                                 aUpdateParameters ) );
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", error );
    // Because this function does not leave,
    // use error value to check if request can be done.
    if ( error == KErrNone )
        {
        TPckg<TInt> successCount( aNumberOfSuccessfullUpdates );
	    iPtr1.Set( successCount );

        TPckg<TInt> failCount( aNumberOfFailedUpdates );
	    iPtr2.Set( failCount );

        TPckg<TInt> cancelCount( aNumberOfCancelledUpdates );
	    iPtr3.Set( cancelCount );
	    	            
        TIpcArgs args;
        args.Set( 0, iData );
        args.Set( 1, &iPtr1 );
        args.Set( 2, &iPtr2 );
        args.Set( 3, &iPtr3 );
      
        // Start the asynchronous operation in the server side.
        SendReceive( aUpdateFunction, args, aStatus );        
        }

    // Inform the caller about the success of the request initializing.
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::SendUpdateRequest() end");
    return error;
    }    

// -----------------------------------------------------------------------------
// RIAUpdateClient::StartNewAppToBackgroundL
// 
// -----------------------------------------------------------------------------
//
void RIAUpdateClient::StartNewAppToBackgroundL( TUid aAppUid, TRequestStatus& aStatus )
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::StartNewAppToBackgroundL() begin");
    TName notUsed;
    iDifferentiator = GenerateServerDifferentiatorAndName(notUsed, aAppUid);
       
    RApaLsSession apa;
    User::LeaveIfError(apa.Connect());
    CleanupClosePushL(apa);
    
    TApaAppInfo info;
    User::LeaveIfError(apa.GetAppInfo(info, aAppUid));
    
    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
    cmdLine->SetExecutableNameL(info.iFullName);
    cmdLine->SetServerRequiredL( iDifferentiator );
    cmdLine->SetCommandL(EApaCommandBackground);
         
    TThreadId notUsedId;
    User::LeaveIfError(apa.StartApp(*cmdLine, notUsedId, &aStatus));
          
    CleanupStack::PopAndDestroy(2, &apa);   // cmdLine and apa
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateClient::StartNewAppToBackgroundL() end");
    }



void RIAUpdateClient::ServerName(TName& aServerName, TUid aAppServerUid, TUint aServerDifferentiator)
    {
    _LIT(KServerNameFormat, "%08x_%08x_AppServer");
    aServerName.Format(KServerNameFormat, aServerDifferentiator, aAppServerUid);
    }
    
TUint RIAUpdateClient::GenerateServerDifferentiatorAndName(TName& aServerName, TUid aAppServerUid)
    {
    TUint r;
    FOREVER
        {
        r = Math::Random();
        if (r==0)
            continue;
        ServerName(aServerName, aAppServerUid, r);
        TFindServer find(aServerName);
        TFullName fullName;
        if (find.Next(fullName) == KErrNone)
            continue;
        break;
        }       
    return r;
    }
