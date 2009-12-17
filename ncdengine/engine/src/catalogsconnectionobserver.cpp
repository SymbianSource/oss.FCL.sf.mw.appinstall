/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include "catalogsconnectionobserver.h"

#include "catalogsconstants.h"
#include "catalogsengineobserver.h"
#include "catalogsdebug.h"

CCatalogsConnectionObserver* CCatalogsConnectionObserver::NewL( MCatalogsEngineObserver& aObserver )
    {
    DLTRACEIN((""));
    CCatalogsConnectionObserver* self = new (ELeave) CCatalogsConnectionObserver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CCatalogsConnectionObserver::~CCatalogsConnectionObserver()
    {
    DLTRACEIN((""));
    Cancel();
    iConnectionEventProperty.Close();
    }

CCatalogsConnectionObserver::CCatalogsConnectionObserver( MCatalogsEngineObserver& aObserver )
    : CActive( EPriorityNormal ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

void CCatalogsConnectionObserver::ConstructL()
    {
    DLTRACEIN((""));    
    
    // Important: this client side process and the server side process 
    // must agree what the key is. The current agreement is that the key is
    // client process SID.
    TUint key = RProcess().SecureId().iId;    
    
    User::LeaveIfError( iConnectionEventProperty.Attach( KCatalogsEnginePropertyCategory, key ) );

    iConnectionEventProperty.Subscribe( iStatus );
    SetActive();
    }

void CCatalogsConnectionObserver::RunL()
    {
    DLTRACEIN(( "iStatus=%d", iStatus.Int() )); 
    
    // On error handling: if in any situation request completes with error,
    // we stop further processsing. This is non-critical system, and it doesn't
    // really make sense to build complex error recovery here.
    //
    // Note that when RProperty is deleted on the server side, all subscriptions are completed
    // with KErrNotFound.
    if ( iStatus.Int() != KErrNone )
        {
        return;
        }
    
    // first subscribe, then get value, otherwise we might miss an update.
    iConnectionEventProperty.Subscribe( iStatus );
    SetActive();
    
    TBool connectionActive;
    TInt err = iConnectionEventProperty.Get( connectionActive );
    DLINFO(("err = %d", err));        
    if ( err == KErrNone )
        {
        iObserver.CatalogsConnectionEvent( connectionActive );
        }
    else
        {
        // Error occured, something is seriously wrong.
        // Nothing really to do than shut down this subsystem by canceling
        // request.
        DLWARNING(("Error occurred while reading connection event property!"));
        Cancel();
        }
    }

void CCatalogsConnectionObserver::DoCancel()
    {
    DLTRACEIN((""));
    iConnectionEventProperty.Cancel();
    }
