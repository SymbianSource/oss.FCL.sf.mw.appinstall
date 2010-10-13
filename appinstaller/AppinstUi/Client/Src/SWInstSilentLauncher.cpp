/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of RSWInstSilentLauncher 
*                class member functions.
*
*/


// INCLUDE FILES
#include <f32file.h>
#include <apgcli.h>
#include <e32math.h>
#include <eikenv.h>

#include "SWInstApi.h"
#include "SWInstDefs.h"
#include "SWInstCommon.h"

using namespace SwiUI;

const TUint KMaxServerPoll = 100;

#ifdef __WINS__
const TUint KServertPollTimeout = 1000000; //1sec. Wins needs more time.
#else 
const TUint KServertPollTimeout = 100000;
#endif 

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// RSWInstSilentLauncher::RSWInstSilentLauncher
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RSWInstSilentLauncher::RSWInstSilentLauncher()    
    {        
    }

// -----------------------------------------------------------------------------
// RSWInstSilentLauncher::Connect
// Creates connection to the server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstSilentLauncher::Connect()
    {
    TInt result( KErrNone );

    TUint differentiator = 0;
    TRAP( result, differentiator = StartServerL( KUidSWInstSvr ) );
    if ( result == KErrNone )
        {        
        TName serverName;
        ConstructServerName( serverName, KUidSWInstSvr, differentiator );
        TRAP( result, ConnectExistingByNameL( serverName ) );
        if ( result == KErrNone )
            {            
            iConnected = ETrue;
            }
        }   
    // Return the result code           
    return result; 
    }

// -----------------------------------------------------------------------------
// RSWInstSilentLauncher::ServiceUid
// Returns the UID of the service that this session provides an interface for.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid RSWInstSilentLauncher::ServiceUid() const
    {
    return TUid::Uid( KSWInstSilentInstallServiceUid );    
    }

// -----------------------------------------------------------------------------
// RSWInstSilentLauncher::ServerName
// Constructs the name of the server application.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RSWInstSilentLauncher::ConstructServerName( TName& aServerName, 
                                        TUid aAppServerUid, 
                                        TUint aServerDifferentiator )
    {
    _LIT(KServerNameFormat, "%08x_%08x_AppServer");
    aServerName.Format( KServerNameFormat, aServerDifferentiator, aAppServerUid );
    }

// -----------------------------------------------------------------------------
// RSWInstSilentLauncher::StartServerL
// Starts the server application.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint RSWInstSilentLauncher::StartServerL( TUid aAppUid )
    {
    // Start the server application
    TName serverName;
    TUint differentiator( 0 );
    while ( ETrue )
        {
        differentiator = Math::Random();
        if ( differentiator==0 )
        continue;
        ConstructServerName( serverName, aAppUid, differentiator );
        TFindServer find( serverName );
        TFullName fullName;
        if ( find.Next( fullName ) == KErrNone )
        continue;
        break;
        }		
    
    TThreadId serverThreadId;
    LaunchAppL( aAppUid, differentiator, serverThreadId );

    // Poll for the server to be started
    for ( TInt ii = 0; ii < KMaxServerPoll; ii++ )
        {
        // look for the server name
        TFindServer find( serverName );
        TFullName fullName;
        if ( find.Next( fullName ) == KErrNone )
            {
            return differentiator;		// found the server, so return
            }
        User::After( KServertPollTimeout );			// wait before trying again
        }
    User::Leave( KErrNotFound );	// failed to find the server, bomb out
    
    return differentiator;
    }

// -----------------------------------------------------------------------------
// RSWInstSilentLauncher::LaunchAppL
// Launches the server application.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RSWInstSilentLauncher::LaunchAppL( TUid aAppUid, TUint aServerDifferentiator, TThreadId& aThreadId )
	{
	RApaLsSession apa;
	User::LeaveIfError( apa.Connect() );
	CleanupClosePushL( apa );
	
	TApaAppInfo info;
	User::LeaveIfError( apa.GetAppInfo( info, aAppUid ) );

	CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
	cmdLine->SetExecutableNameL( info.iFullName );
	cmdLine->SetServerRequiredL( aServerDifferentiator );
        // Set the command to start the server in background
        cmdLine->SetCommandL( EApaCommandBackground );        
		
	User::LeaveIfError( apa.StartApp( *cmdLine, aThreadId ) );

	CleanupStack::PopAndDestroy( 2, &apa );	// cmdLine and apa
	}


//  End of File  
