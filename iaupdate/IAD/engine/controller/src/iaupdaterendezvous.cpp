/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdateRendezvous
*
*/




#include "iaupdaterendezvous.h"
#include "iaupdaterendezvousobserver.h"
#include "iaupdatedebug.h"


CIAUpdateRendezvous* CIAUpdateRendezvous::NewL( MIAUpdateRendezvousObserver& aObserver )
    {
    CIAUpdateRendezvous* self = 
        CIAUpdateRendezvous::NewLC( aObserver ); 
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateRendezvous* CIAUpdateRendezvous::NewLC( MIAUpdateRendezvousObserver& aObserver )
    {
    CIAUpdateRendezvous* self = 
        new( ELeave ) CIAUpdateRendezvous( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CIAUpdateRendezvous::CIAUpdateRendezvous( MIAUpdateRendezvousObserver& aObserver )
: CActive( CActive::EPriorityStandard ), 
  iObserver( aObserver )
    {
    
    }


void CIAUpdateRendezvous::ConstructL()
    {
    CActiveScheduler::Add( this );
    }


CIAUpdateRendezvous::~CIAUpdateRendezvous()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRendezvous::~CIAUpdateRendezvous() begin");
    
    // It is always good and safe to do cancel in CActive objects.
    Cancel();

    // No need to close the rendezvous process here anymore.
    // It is closed in cancel above if this object was active.
    // Or, if the object was not active, then the process has already 
    // been closed before in RunL or it has not been even opened.

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRendezvous::~CIAUpdateRendezvous() end");
    }


void CIAUpdateRendezvous::DoCancel()
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateRendezvous::DoCancel() begin: %d", iStatus.Int());
    
    // Notice, that the active observer Cancel-function waits for the
    // silent launcher to send request complete before cancel of this
    // active object can finish. This is normal active object cancel operation behaviour.
    // Notice, that we do not call the callback functions of the observer here
    // because we suppose that the observer has started the cancel operation itself
    // or the caller will inform the observer itself.

    // Cancel the previously requested rendezvous with the process.
    // This will make the request complete with KErrCancel value.    
    TInt cancelError( RendezvousProcess().RendezvousCancel( iStatus ) );
    IAUPDATE_TRACE_1("[IAUPDATE] Rendezvous cancel error code: %d", cancelError);
    IAUPDATE_TRACE_1("[IAUPDATE] Rendezvous cancelled. Current iStatus: %d", iStatus.Int());
        
    // Because process is not needed anymore, close it.
    RendezvousProcess().Close();

    // If cancel error is KErrNone, then the rendezvous cancel above has 
    // completed the request with KErrCancel.
    if ( cancelError != KErrNone )
        {
        // We should never come here, because DoCancel is called only if this
        // active object is active. And then, rendezvous should be also active and
        // cancel above should finish correctly.
        IAUPDATE_TRACE("[IAUPDATE] WARNING: Rendezvous could not be cancelled.");
        TRequestStatus* ptrStatus = &iStatus;
        User::RequestComplete( ptrStatus, KErrCancel );        
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRendezvous::DoCancel() end");
    }


void CIAUpdateRendezvous::StartL( const TProcessId& aProcessId )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRendezvous::StartL() begin");
    
    if ( IsActive() )
        {
        IAUPDATE_TRACE("[IAUPDATE] StartL already in use");
        User::Leave( KErrInUse );
        }
    
    TInt error( RendezvousProcess().Open( aProcessId ) );
    if ( error != KErrNone )
        {
        IAUPDATE_TRACE_1("[IAUPDATE] Rendezvous open error: %d", error);
        RendezvousProcess().Close();
        User::Leave( error );        
        }

    RendezvousProcess().Rendezvous( iStatus );
    if ( iStatus != KRequestPending )
        {
        // Rendezvous operation could not be completed.
        IAUPDATE_TRACE_1("[IAUPDATE] Rendezvous error: %d", iStatus.Int());
        RendezvousProcess().Close();
        User::Leave( iStatus.Int() );
        }

    SetActive();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRendezvous::StartL() end");
    }


void CIAUpdateRendezvous::RunL()
    {
    // This is called when Rendezvous is finished in the iProcess.
    RendezvousProcess().Close();
    Observer().RendezvousComplete( iStatus.Int() );
    }


MIAUpdateRendezvousObserver& CIAUpdateRendezvous::Observer()
    {
    return iObserver;
    }


RProcess& CIAUpdateRendezvous::RendezvousProcess()
    {
    return iProcess;
    }
