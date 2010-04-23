/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/


#include "catalogsconnection.h"
#include "catalogshttpconnectioncreator.h"
#include "catalogsutils.h"

#include "catalogsdebug.h"

// Default for socket server message slots is 8 which is too little
// when we use the same socket server connection for several HTTP
// connections
const TInt KSocketServerMessageSlots = 20;

    
CCatalogsConnectionMonitor* CCatalogsConnectionMonitor::NewL( 
    MCatalogsConnectionStateObserver& aObserver )
    {
    CCatalogsConnectionMonitor* self = new( ELeave ) 
        CCatalogsConnectionMonitor( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CCatalogsConnectionMonitor::~CCatalogsConnectionMonitor()
    {
    DLTRACEIN((""));
    Cancel();
    iConnection.Close();
    iSs.Close();
    }


void CCatalogsConnectionMonitor::StartL( 
    const TConnectionInfoBuf& aInfo )
    {
    DLTRACEIN((""));
    
    TNifProgress progress;
    User::LeaveIfError( iConnection.Attach( 
        aInfo, RConnection::EAttachTypeMonitor ) );
    
    iConnection.ProgressNotification( iProgressBuf, iStatus );
        
    TInt err = iConnection.Progress( progress );
    
    DLTRACE(("Progress stage: %d, error: %d", 
        progress.iStage, progress.iError ));
    
    if ( err != KErrNone )
        {
        DLERROR(("Error when getting progress info: %d", err));
        iConnection.CancelProgressNotification();
        // Prevent RunL from running
        User::WaitForRequest( iStatus );
        User::Leave( err );
        }
    else if ( progress.iStage == KConnectionUninitialised ) 
        {
        DLTRACE(("Connection already dead"));
        iConnection.CancelProgressNotification();
        iProgressBuf = progress;
        }
    SetActive();
    }
    
    
void CCatalogsConnectionMonitor::RunL()
    {
    DLTRACEIN((""));
    LeaveIfNotErrorL( iStatus.Int(), KErrCancel );

    // Ensure that we don't miss any events
    if ( iStatus.Int() == KErrNone )
        {
        iConnection.ProgressNotification( iProgressBuf, iStatus );
        SetActive();
        }    
    
    TInt stage = iProgressBuf().iStage;    
    DLTRACE(("Stage: %d, error: %d", stage, iProgressBuf().iError ));
    
    // Take possible connection error code. This is needed for "Red phone"
    // detection
    if ( stage == KLinkLayerClosed ) 
        {
        TInt error = iProgressBuf().iError;
        if ( error ) 
            {
            stage = error;
            }
        }
    
    iObserver.ConnectionStateChangedL( stage );    
    }
    
    
void CCatalogsConnectionMonitor::DoCancel()
    {
    iConnection.CancelProgressNotification();
    
    }
    
    
TInt CCatalogsConnectionMonitor::RunError( TInt aError ) 
    {
    iObserver.ConnectionStateError( aError );
    return KErrNone;
    }
    
CCatalogsConnectionMonitor::CCatalogsConnectionMonitor( 
    MCatalogsConnectionStateObserver& aObserver ) :
    CActive( EPriorityHigh ),
    iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

void CCatalogsConnectionMonitor::ConstructL()
    {
    User::LeaveIfError( iSs.Connect() );
    User::LeaveIfError( iConnection.Open( iSs ) );
    }
    


// ---------------------------------------------------------------------------
// CCatalogsConnection
// ---------------------------------------------------------------------------
//    


    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
CCatalogsConnection* CCatalogsConnection::NewL(         
    CCatalogsHttpConnectionCreator& aConnectionCreator,
    CCatalogsHttpConnectionManager& aConnectionManager,
    MCatalogsConnectionObserver& aObserver )
    {
    CCatalogsConnection* self = new( ELeave ) 
        CCatalogsConnection( 
            aConnectionCreator, 
            aConnectionManager,
            aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CCatalogsConnection::ConnectL( 
    const TCatalogsConnectionMethod& aMethod )
    {
    DLTRACEIN((""));
    iConnectionMethod = aMethod;
    iConnCreator.ConnectL( 
        iConnectionMethod, 
        iConnection,
        iStatus,
        &iConnManager );
        
    SetActive();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
CCatalogsConnection::~CCatalogsConnection()
    {
    DLTRACEIN((""));
    Cancel();    
    iConnection.Close();
    iSs.Close();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
RConnection& CCatalogsConnection::Connection()
    {
    return iConnection;
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
RSocketServ& CCatalogsConnection::SocketServer()
    {
    return iSs;
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
const TCatalogsConnectionMethod& CCatalogsConnection::ConnectionMethod() const
    {
    return iConnectionMethod;
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TBool CCatalogsConnection::IsConnectedL()
    {
    DLTRACEIN((""));
    if ( iConnection.SubSessionHandle() ) 
        {
        
        TNifProgress progress;
        User::LeaveIfError( iConnection.Progress( progress ) );
        if ( progress.iStage == KLinkLayerOpen ) 
            {
            DLTRACEOUT(("Connected"));
            return ETrue;
            }
        }
    DLTRACEOUT(("Not connected"));
    return EFalse;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CCatalogsConnection::RunL()
    {
    DLTRACEIN(("")); 

    // Let RunError handle errors
    User::LeaveIfError( iStatus.Int() );

    iObserver.ConnectionCreatedL( iConnectionMethod );
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CCatalogsConnection::DoCancel()
    {
    iConnCreator.Cancel();
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TInt CCatalogsConnection::RunError( TInt aError )
    {
    DLTRACEIN(("aError: %d", aError));
    iObserver.ConnectionError( aError );
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
CCatalogsConnection::CCatalogsConnection(
    CCatalogsHttpConnectionCreator& aConnectionCreator,
    CCatalogsHttpConnectionManager& aConnectionManager,
    MCatalogsConnectionObserver& aObserver ) :     
    iObserver( aObserver ),
    iConnCreator( aConnectionCreator ),
    iConnManager( aConnectionManager ),
    iConnectionMethod( 0, ECatalogsConnectionMethodTypeDeviceDefault )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CCatalogsConnection::ConstructL()
    {
    DLTRACEIN((""));
    User::LeaveIfError( iSs.Connect( KSocketServerMessageSlots ) );
    User::LeaveIfError( iConnection.Open( iSs ) );        
    }
