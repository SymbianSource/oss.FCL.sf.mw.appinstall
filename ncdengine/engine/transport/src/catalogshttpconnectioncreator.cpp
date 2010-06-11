/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of CCatalogsHttpConnectionCreator
*
*/


#include "catalogshttpconnectioncreator.h"

#include <exterror.h>
#include <es_sock.h>
#include <es_enum.h>

#include "catalogshttpconnectionmanager.h"
#include "catalogshttptypes.h"
#include "catalogserrors.h"
#include "catalogshttputils.h"

#include "catalogsdebug.h"

CCatalogsHttpConnectionCreator* CCatalogsHttpConnectionCreator::NewL()
    {
    CCatalogsHttpConnectionCreator* self = 
        new(ELeave) CCatalogsHttpConnectionCreator();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CCatalogsHttpConnectionCreator::~CCatalogsHttpConnectionCreator()
    {
    DLTRACEIN((""));
    Cancel();
    iConnectionQueue.Close();
    }
    
    
void CCatalogsHttpConnectionCreator::ConnectL( 
    TCatalogsConnectionMethod& aMethod, 
    RConnection& aConnection,
    TRequestStatus& aStatus,
    CCatalogsHttpConnectionManager* aConnectionManager )
    {
    DLTRACEIN(("Ap: %d, %u, %u, &aStatus: %x", 
        aMethod.iType, aMethod.iId, aMethod.iApnId, &aStatus ));
        
    aStatus = KRequestPending;
    TConnectionItem item( aMethod, aConnection, aStatus, aConnectionManager );
        
    iConnectionQueue.AppendL( item );        
    DoConnectL();
    DLTRACEOUT(("ConnectL finished"));
    }


CCatalogsHttpConnectionCreator::CCatalogsHttpConnectionCreator() :
    CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }
    

void CCatalogsHttpConnectionCreator::ConstructL()
    {
    }


void CCatalogsHttpConnectionCreator::RunL()
    {
    DLTRACEIN( ( "iStatus: %d, this: %X, queued: %d",                  
                     iStatus.Int(),
                     this,
                     iConnectionQueue.Count() ) );
    
    // Handle AP-selection dialog cancellation   
    if ( iStatus.Int() == KErrCancel ) 
        {        
        if ( iConnectionQueue.Count() ) 
            {            
            DLTRACE(("Canceling a request"));            
            DoCancel();
            }
            
        iStatus = KErrNone;
        DLTRACEOUT(("iStatus was KErrCancel"));
        return;
        }
        
    TConnectionItem& currentItem = iConnectionQueue[0];
    TInt err = iStatus.Int();
    
    switch ( iStatus.Int() )
        {
        case KErrNone:              // Connection created
        case KErrAlreadyExists:     // Connection exists
            {
            TUint32 openedAPN = 0;
        
            err = CatalogsHttpUtils::ApnFromConnection( 
                *currentItem.iConnection,
                openedAPN );

            DLTRACE(("Opened apn: %d", openedAPN ));

            currentItem.iConnectionMethod.iApnId = openedAPN;

            if ( currentItem.iConnectionManager && 
                 err == KErrNone ) 
                {                
                // Sets the apn as default if the connection method otherwise matches
                // the default
                currentItem.iConnectionManager->SetAccessPointForDefaultConnectionMethod( 
                    currentItem.iConnectionMethod );                    

                }
            break;
            }
            
        case KErrNotFound:
            {
            DLTRACE(("Couldn't find a suitable AP"));
            DLERROR(("KCatalogsErrorCodeNoApn"));
            // Using just this one code from now on
            err = KCatalogsErrorHttpNoApn;
            break;
            }
                    
            
        default:
            {
            break;
            }
        }
    DLTRACE(("Completing %x with %d", currentItem.iStatus, err ));
    

    User::RequestComplete( currentItem.iStatus, err );
    iConnectionQueue.Remove( 0 );
        
    DoConnectL();    
    }


void CCatalogsHttpConnectionCreator::DoCancel()
    {
    DLTRACEIN((""));
    DASSERT( iConnectionQueue.Count() );
        
    iConnectionQueue[0].iConnection->Close();
    for ( TInt i = 0; i < iConnectionQueue.Count(); ++i )
        {
        DLTRACE(("Canceling request %x", iConnectionQueue[i].iStatus ));
        User::RequestComplete( iConnectionQueue[i].iStatus, KErrCancel );      
        }
    iConnectionQueue.Reset();
    }


TInt CCatalogsHttpConnectionCreator::RunError( TInt aError )
    {
    DLTRACEIN(("aError: %d", aError));
    return aError;
    }


void CCatalogsHttpConnectionCreator::DoConnectL()
    {
    DLTRACEIN(("iConnectionQueue.Count: %d, IsActive: %d", 
        iConnectionQueue.Count(), IsActive() ));
        
    while ( !IsActive() && iConnectionQueue.Count() )
        {        
        TBool activate = ETrue;
        TConnectionItem& currentItem = iConnectionQueue[0];

        // Try to get default ap id in case it has been updated after
        // this item was added to the queue
        if ( currentItem.iConnectionMethod.iId == 0
             && currentItem.iConnectionMethod.iApnId == 0
             && currentItem.iConnectionManager ) 
            {
            DLTRACE(("Getting default accesspoint from connection manager"));
            currentItem.iConnectionMethod = 
                currentItem.iConnectionManager->DefaultConnectionMethod();
            }
        
        iConnected = EFalse;        
        
        TBool iapExists = CatalogsHttpUtils::ConnectionMethodExistsL( 
            currentItem.iConnectionMethod );        
        
        if ( !iapExists )
            {
            DLTRACE( ( "Bad AP, %i", currentItem.iConnectionMethod.iApnId ) );
            // We haven't connected before or previously used access point
            // has been deleted. Prompt for APN.                        
            
            StartConnection( currentItem, ETrue );
            }
        else
            {
            // Connected before, use the same access point
            
            // Check for existing connections
            TUint count;
            currentItem.iConnection->EnumerateConnections( count );

            if ( count == 0 )
                {
                DLTRACE( ( "-> No existing connections, initiating new, APID: %d",
                    currentItem.iConnectionMethod.iId ) );
                StartConnection( currentItem, EFalse );
                }
            else
                {                
                TInt err = KErrNotFound;

                DLTRACE(("connection count: %d",count));
                #ifdef CATALOGS_BUILD_CONFIG_DEBUG
                for (TInt i = 1 ; i <= count ; i++ )
                    {
                    TPckgBuf<TConnectionInfo> pkg;
                    currentItem.iConnection->GetConnectionInfo( i, pkg );
                    DLINFO(("connection: %d, ap: %d",i, pkg().iIapId ));
                    }
                #endif
                
                // Check for a connection matching the one selected before                                
                TConnectionInfoBuf connectionInfo;
                if ( CatalogsHttpUtils::IsConnectionMethodOpen( 
                        *currentItem.iConnection,
                        count,
                        currentItem.iConnectionMethod,
                        connectionInfo ) ) 
                    {
                    // Desired connection exists, attach
                    err = currentItem.iConnection->Attach( 
                        connectionInfo, 
                        RConnection::EAttachTypeNormal );
                        
                    if ( err == KErrNone || err == KErrInUse )
                        {
                        // Connected
                        
                        // We need to update the correct IAP id here because we won't
                        // go to RunL now
                        currentItem.iConnectionMethod.iApnId = connectionInfo().iIapId;                        
                        
                        DLTRACE(( "-> Using existing connection, APID: %u", 
                            currentItem.iConnectionMethod.iApnId ));
                                                
                        iConnected = ETrue;  
                        err = KErrNone;                              
                        }
                    
                    }

                if ( !iConnected )
                    {
                    DLTRACE(("No existing connection, creating a new one"));
                    // No suitable connection found, connect
                    StartConnection( currentItem, EFalse );
                    
                    }
                else 
                    {
                    DLTRACE(("Using existing connection if no error, err: %d", err));
                    
                    User::RequestComplete( currentItem.iStatus, err );
                    iConnectionQueue.Remove( 0 );
                    activate = EFalse;
                    }
                }
            }
        
        DLTRACE(("Setting active"));
        if ( activate ) 
            {            
            SetActive();
            }
        }        
    }


void CCatalogsHttpConnectionCreator::StartConnection( 
    const TConnectionItem& aItem, 
    TBool aHandleAsAlwaysAsk ) 
    {
    DLTRACEIN((""));
    TCatalogsConnectionMethodType type = aItem.iConnectionMethod.iType;
    
    if ( aHandleAsAlwaysAsk ) 
        {
        type = ECatalogsConnectionMethodTypeAlwaysAsk;
        }
    // If APN id is set we don't care about the original id or type
    else if ( aItem.iConnectionMethod.iApnId ) 
        {
        DLTRACE(("ApnId set, handling like accesspoint"));
        type = ECatalogsConnectionMethodTypeAccessPoint;
        }
    
    
    switch( type ) 
        {
        case ECatalogsConnectionMethodTypeDestination: // snap
            {
            DLTRACE(("Snap"));            
            iSnapConnectionPrefs.SetSnap( aItem.iConnectionMethod.iId );
            aItem.iConnection->Start( iSnapConnectionPrefs, iStatus );
            break;         
            }
            
        case ECatalogsConnectionMethodTypeAccessPoint: // iap
            {
            DLTRACE(("Iap"));                       
            
            iIapConnectionPrefs.SetIapId( aItem.iConnectionMethod.CurrentApnId() );
            iIapConnectionPrefs.SetNetId( 0 );
            iIapConnectionPrefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
            aItem.iConnection->Start( iIapConnectionPrefs, iStatus );
            break;
            }
        
        case ECatalogsConnectionMethodTypeDeviceDefault: // default connection
            {
            DLTRACE(("Device default"));
            aItem.iConnection->Start( iStatus );
            break;
            }
            
        case ECatalogsConnectionMethodTypeAlwaysAsk: // ask always
        default:
            {
            DLTRACE(("Always ask"));            
            iIapConnectionPrefs.SetIapId( 0 );
            iIapConnectionPrefs.SetNetId( 0 );
            iIapConnectionPrefs.SetDialogPreference( ECommDbDialogPrefPrompt );
            aItem.iConnection->Start( iIapConnectionPrefs, iStatus );
            break;
            }                
        }                
    }
