/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of RIAUpdateLauncherClient
*                class member functions.
*
*/



//INCLUDES
#include "iaupdatelauncherclient.h"
#include "iaupdateclientdefines.h"
#include "iaupdatedebug.h"

// -----------------------------------------------------------------------------
// RIAUpdateLauncherClient::RIAUpdateLauncherClient
// 
// -----------------------------------------------------------------------------
// 
RIAUpdateLauncherClient::RIAUpdateLauncherClient() 
: iPtr1( NULL, 0 )
    {
    }


// -----------------------------------------------------------------------------
// RIAUpdateLauncherClient::Open
// 
// -----------------------------------------------------------------------------
// 
TInt RIAUpdateLauncherClient::Open()
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateLauncherClient::Open() begin");
        
    TInt error = KErrNone;
    if ( !iConnected )
        {
        TRAP( error, ConnectChainedAppL( ServiceUid() ) );
        if ( error == KErrNone )
            {
            iConnected = ETrue;
            }
        }	
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", error );
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateLauncherClient::Open() end");
    return error;
    }

// -----------------------------------------------------------------------------
// RIAUpdateLauncherClient::Close
// 
// -----------------------------------------------------------------------------
//
void RIAUpdateLauncherClient::Close()
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateLauncherClient::Close() begin");
    // Let the parent handle closing.
    RAknAppServiceBase::Close();
    iConnected = EFalse;
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateLauncherClient::Close() end");
    }


// -----------------------------------------------------------------------------
// RIAUpdateLauncherClient::ShowUpdates()
// 
// -----------------------------------------------------------------------------
//
void RIAUpdateLauncherClient::ShowUpdates( TBool& aRefreshFromNetworkDenied, 
                                           TRequestStatus& aStatus )
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateLauncherClient::ShowUpdates() begin");
    if ( iConnected )
        {
        TPckg<TBool> refreshFromNetworkDenied( aRefreshFromNetworkDenied );
	    iPtr1.Set( refreshFromNetworkDenied );
	    TIpcArgs args;
        args.Set( 0, &iPtr1 );
    	SendReceive( IAUpdateClientDefines::EIAUpdateServerStartedByLauncher, 
    	             args,
    	             aStatus );
        }
 
        
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateLauncherClient::ShowUpdates() end");
    }

// -----------------------------------------------------------------------------
// RIAUpdateLauncherClient::CancelAsyncRequest()
// 
// -----------------------------------------------------------------------------
//    
void RIAUpdateLauncherClient::CancelAsyncRequest()
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateLauncherClient::CancelAsyncRequest() begin");
    if ( iConnected )
        {
    	SendReceive( IAUpdateClientDefines::EIAUpdateServerCancel ); 
        }
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateLauncherClient::CancelAsyncRequest() end");
    }


// -----------------------------------------------------------------------------
// RIAUpdateLauncherClient::ServiceUid()
// 
// -----------------------------------------------------------------------------
//
TUid RIAUpdateLauncherClient::ServiceUid() const
    {
    IAUPDATE_TRACE("[IAUPDATE] RIAUpdateLauncherClient::ServiceUid()");
    return IAUpdateClientDefines::KIAUpdateServiceUid;    
    }

