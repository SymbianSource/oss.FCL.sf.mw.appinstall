/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CCUICancelTimer
*                class member functions.
*
*/


// INCLUDE FILES
#include <eikenv.h>

#include "CUICancelTimer.h"

using namespace SwiUI::CommonUI;

const TInt KCancelTries = 4;
const TInt KCancelFirstTryInterval = 1; // 1us
const TInt KCancelTryInterval = 500000; // 0,5 sec

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCUICancelTimer::CCUICancelTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCUICancelTimer::CCUICancelTimer( MCUICancellable* aCancellable )
    : CTimer( CActive::EPriorityHigh ),
      iCancellable( aCancellable ),
      iTryCount( KCancelTries )
    {
    CActiveScheduler::Add( this );    
    }

// -----------------------------------------------------------------------------
// CCUICancelTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCUICancelTimer::ConstructL()
    {    
    CTimer::ConstructL(); 
    iEikEnv = CEikonEnv::Static();    
    }

// -----------------------------------------------------------------------------
// CCUICancelTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCUICancelTimer* CCUICancelTimer::NewL( MCUICancellable* aCancellable )
    {
    CCUICancelTimer* self = new( ELeave ) CCUICancelTimer( aCancellable );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
// Destructor
EXPORT_C CCUICancelTimer::~CCUICancelTimer()
    {
    Cancel();    
    }

// -----------------------------------------------------------------------------
// CCUICancelTimer::InstallL
// Perform installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCUICancelTimer::StartCancelling()
    {
    iCancellable->StartedCancellingL();
    iCancellable->CancelEngine();
    if ( iCancellable->IsShowingDialog() )
        {
        TKeyEvent key;
        key.iRepeats = 0;
        key.iCode = EKeyEscape;
        key.iModifiers = 0;
        iEikEnv->SimulateKeyEventL( key, EEventKey );
        }  
    iTryCount--;
    TTimeIntervalMicroSeconds32 timeout( KCancelFirstTryInterval );
    After( timeout );    
    }

// -----------------------------------------------------------------------------
// CCUICancelTimer::RunL
// Called when timer has triggered.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CCUICancelTimer::RunL()
    {
    if ( --iTryCount >= 0 )
        {
        iCancellable->CancelEngine();
        if ( iCancellable->IsShowingDialog() )
            {
            TKeyEvent key;
            key.iRepeats = 0;
            key.iCode = EKeyEscape;
            key.iModifiers = 0;
            iEikEnv->SimulateKeyEventL( key, EEventKey );
            }  
        TTimeIntervalMicroSeconds32 timeout( KCancelTryInterval );
        After( timeout );        
        }    
    else if ( iCancellable->IsShowingDialog() )
        {
        // In this case we we are still showing a dialog, even we have tried
        // to dismiss it. So force the cancel
        iCancellable->ForceCancel();
        }    
    }

// -----------------------------------------------------------------------------
// CCUICancelTimer::RunError
// Handles a leave occurring in the request completion event handler RunL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CCUICancelTimer::RunError( TInt /*aError*/ )
    {
    iCancellable->ForceCancel();    

    return KErrNone;    
    }

//  End of File  
