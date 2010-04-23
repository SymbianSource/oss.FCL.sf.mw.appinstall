/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdActiveOperationObserver
*
*/


#include "ncdactiveoperationobserver.h"
#include "ncdasyncoperationobserver.h"


CNcdActiveOperationObserver* CNcdActiveOperationObserver::NewL( MNcdAsyncOperationObserver& aObserver )
    {
    CNcdActiveOperationObserver* self = 
        CNcdActiveOperationObserver::NewLC( aObserver ); 
    CleanupStack::Pop( self );
    return self;
    }


CNcdActiveOperationObserver* CNcdActiveOperationObserver::NewLC( MNcdAsyncOperationObserver& aObserver )
    {
    CNcdActiveOperationObserver* self = 
        new( ELeave ) CNcdActiveOperationObserver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CNcdActiveOperationObserver::CNcdActiveOperationObserver( MNcdAsyncOperationObserver& aObserver )
: CActive( CActive::EPriorityStandard ), 
  iObserver( aObserver )
    {
    
    }


void CNcdActiveOperationObserver::ConstructL()
    {
    CActiveScheduler::Add( this );
    }


CNcdActiveOperationObserver::~CNcdActiveOperationObserver()
    {
    // This is always good and safe to do in CActive objects.
    Cancel();
    }


void CNcdActiveOperationObserver::DoCancel()
    {
    // Because this is just an observer itself.
    // Do not do anything. Do not even inform the observer
    // of this class object about this cancellation. 
    // Because the observer is not observing the events of 
    // this active object. It is observing the operations of 
    // some other object.
    }


void CNcdActiveOperationObserver::StartToObserve()
    {
    SetActive();
    }


void CNcdActiveOperationObserver::RunL()
    {
    // Just forward the information to the observer.
    AsyncObserver().AsyncOperationComplete( iStatus.Int() );
    }


MNcdAsyncOperationObserver& CNcdActiveOperationObserver::AsyncObserver() const
    {
    return iObserver;
    }
