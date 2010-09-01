/*
*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CAppMngr2ExitTimer
*                class member functions.
*
*/


// INCLUDE FILES

#include <appmngr2runtime.h>            // FLOG macros
#include <appmngr2debugutils.h>         // FLOG macros
#include "appmngr2exittimer.h"
#include "appmngr2appui.h"              // CAppMngr2AppUi

const TInt KAppMngr2ExitDelayInterval = 1000000; // 1,0 sec

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAppMngr2ExitTimer::CAppMngr2ExitTimer
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CAppMngr2ExitTimer::CAppMngr2ExitTimer( CAppMngr2AppUi* aAppUi )
    : CTimer( CActive::EPriorityHigh ),
    iAppUi( aAppUi )
    {
    FLOG( "CAppMngr2ExitTimer::CAppMngr2ExitTimer" );
    CActiveScheduler::Add( this );    
    }

// -----------------------------------------------------------------------------
// CAppMngr2ExitTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAppMngr2ExitTimer::ConstructL()
    {    
    CTimer::ConstructL();      
    }

// -----------------------------------------------------------------------------
// CAppMngr2ExitTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAppMngr2ExitTimer* CAppMngr2ExitTimer::NewL( CAppMngr2AppUi* aAppUi )
    {
    CAppMngr2ExitTimer* self = new( ELeave ) CAppMngr2ExitTimer( aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// -----------------------------------------------------------------------------
// CAppMngr2ExitTimer::Destructor
// 
// -----------------------------------------------------------------------------
//
EXPORT_C CAppMngr2ExitTimer::~CAppMngr2ExitTimer()
    {
    FLOG( "CAppMngr2ExitTimer::~CAppMngr2ExitTimer" );
    CTimer::Cancel();    
    }

// -----------------------------------------------------------------------------
// CAppMngr2ExitTimer::StartExitTimer
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2ExitTimer::StartExitTimer()
    {
    FLOG( "CAppMngr2ExitTimer::StartExitTimer" );
    FLOG( "CAppMngr2ExitTimer After = %d", KAppMngr2ExitDelayInterval );
    TTimeIntervalMicroSeconds32 timeout( KAppMngr2ExitDelayInterval );    
    After( timeout );
    FLOG( "CAppMngr2ExitTimer::StartExitTimer END" );
    }

// -----------------------------------------------------------------------------
// CAppMngr2ExitTimer::RunL
//
// -----------------------------------------------------------------------------
//    
void CAppMngr2ExitTimer::RunL()
    {
    FLOG( "CAppMngr2ExitTimer::RunL START" );
    // Exit will leave but this is handled in RunError function.    
    FLOG( "CAppMngr2ExitTimer::RunL: iAppUi->Exit()" );
    iAppUi->Exit();       
    FLOG( "CAppMngr2ExitTimer::RunL END" );
    }

// -----------------------------------------------------------------------------
// CAppMngr2ExitTimer::RunError
// 
// -----------------------------------------------------------------------------
// 
TInt CAppMngr2ExitTimer::RunError( TInt aError )
    {
    // RunL will leave with the error -1003 KLeaveExit and RunError
    // needs to return this error to CActiveScheduler which will then
    // make exit leave in CBaActiveScheduler::Error function.
    FLOG( "CAppMngr2ExitTimer::RunError error = %d", aError );     
    return aError;    
    }

//  End of File  
