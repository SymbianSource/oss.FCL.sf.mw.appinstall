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
* Description:   This file contains the implementation of CIAUpdateUITime
*                class member functions.
*
*/

#include "iaupdateuitimer.h"
#include "iaupdateuitimerobserver.h"


CIAUpdateUITimer* CIAUpdateUITimer::NewL( 
    MIAUpdateUITimerObserver& aObserver,
    TTimerType aTimerType )
    {
    CIAUpdateUITimer* self(
        CIAUpdateUITimer::NewLC( aObserver, aTimerType ) );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateUITimer* CIAUpdateUITimer::NewLC( 
    MIAUpdateUITimerObserver& aObserver,
    TTimerType aTimerType )
    {
    CIAUpdateUITimer* self(
        new( ELeave ) CIAUpdateUITimer( aObserver, aTimerType ) );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CIAUpdateUITimer::CIAUpdateUITimer( 
    MIAUpdateUITimerObserver& aObserver,
    TTimerType aTimerType )
: CTimer( CActive::EPriorityStandard ),
iObserver( aObserver ),
iTimerType( aTimerType )
    {
    // Active object needs to be added to the scheduler.
    CActiveScheduler::Add( this );
    }


CIAUpdateUITimer::~CIAUpdateUITimer()
    {
    // No need to call Cancel here because
    // DoCancel is not implemented in this class 
    // and the parent destructor calls Cancel itself
    // which also uses the DoCancel of the parent.
    }


void CIAUpdateUITimer::RunL()
    {
    // Timer has completed its operation.
    // Inform observer.
    switch ( iTimerType )
        {
        case EBackgroundDelay:
            iObserver.BackgroundDelayComplete( iStatus.Int() ); 
            break;
        case EForegroundDelay:   
            iObserver.ForegroundDelayComplete( iStatus.Int() );
            break;
        case EProcessStartDelay:
            iObserver.ProcessStartDelayComplete( iStatus.Int() );
        default:
            break;
        }
    }
