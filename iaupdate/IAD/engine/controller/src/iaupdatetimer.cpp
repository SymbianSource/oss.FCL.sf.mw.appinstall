/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "iaupdatetimer.h"
#include "iaupdatetimerobserver.h"


CIAUpdateTimer* CIAUpdateTimer::NewL( 
    MIAUpdateTimerObserver& aObserver )
    {
    CIAUpdateTimer* self(
        CIAUpdateTimer::NewLC( aObserver ) );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateTimer* CIAUpdateTimer::NewLC( 
    MIAUpdateTimerObserver& aObserver )
    {
    CIAUpdateTimer* self(
        new( ELeave ) CIAUpdateTimer( aObserver ) );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CIAUpdateTimer::CIAUpdateTimer( 
    MIAUpdateTimerObserver& aObserver )
: CTimer( CActive::EPriorityStandard ),
iObserver( aObserver )
    {
    // Active object needs to be added to the scheduler.
    CActiveScheduler::Add( this );
    }


CIAUpdateTimer::~CIAUpdateTimer()
    {
    // No need to call Cancel here because
    // DoCancel is not implemented in this class 
    // and the parent destructor calls Cancel itself
    // which also uses the DoCancel of the parent.
    }


void CIAUpdateTimer::RunL()
    {
    // Timer has completed its operation.
    // Inform observer.
    iObserver.TimerComplete( iStatus.Int() );
    }
