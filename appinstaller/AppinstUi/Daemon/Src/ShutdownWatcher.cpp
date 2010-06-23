/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Shutdown watcher implementation
*
*/


#include "ShutdownWatcher.h"
#include <startupdomainpskeys.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CShutdownWatcher::NewL
// ---------------------------------------------------------------------------
//
CShutdownWatcher* CShutdownWatcher::NewL( MShutdownObserver& aObserver )
    {
    CShutdownWatcher* self = new(ELeave) CShutdownWatcher( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CShutdownWatcher::~CShutdownWatcher
// ---------------------------------------------------------------------------
//
CShutdownWatcher::~CShutdownWatcher()
    {
    Cancel();
    iProperty.Close();
    }


// ---------------------------------------------------------------------------
// CShutdownWatcher::Start
// ---------------------------------------------------------------------------
//
void CShutdownWatcher::Start()
    {
    TInt status( 0 );
    TInt err = iProperty.Get( KPSUidStartup, KPSGlobalSystemState, status );
    if( err == KErrNone )
        {
        if( status == ESwStateShuttingDown )
            {
            iObserver.NotifyShuttingDown();
            return;     // that was all, system is already going down
            }
        }
    IssueRequest();
    }


// ---------------------------------------------------------------------------
// CShutdownWatcher::IssueRequest
// ---------------------------------------------------------------------------
//
void CShutdownWatcher::IssueRequest()
    {
    if( !IsActive() )
        {
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    }


// ---------------------------------------------------------------------------
// CShutdownWatcher::DoCancel
// ---------------------------------------------------------------------------
//
void CShutdownWatcher::DoCancel()
    {
    iProperty.Cancel();
    }


// ---------------------------------------------------------------------------
// CShutdownWatcher::RunL()
// ---------------------------------------------------------------------------
//
void CShutdownWatcher::RunL()
    {
    IssueRequest();

    TInt status( 0 );
    iProperty.Get( status );
    
    if( status == ESwStateShuttingDown )
        {
        iObserver.NotifyShuttingDown();
        Cancel();   // that was all, no other notifications needed
        }
    }


// ---------------------------------------------------------------------------
// CShutdownWatcher::CShutdownWatcher
// ---------------------------------------------------------------------------
//
CShutdownWatcher::CShutdownWatcher( MShutdownObserver& aObserver )
    : CActive( EPriorityStandard ), iObserver( aObserver )
    {
    }


// ---------------------------------------------------------------------------
// CShutdownWatcher::ConstructL()
// ---------------------------------------------------------------------------
//
void CShutdownWatcher::ConstructL()
    {
    TInt err = iProperty.Attach( KPSUidStartup, KPSGlobalSystemState );
    User::LeaveIfError( err );

    CActiveScheduler::Add( this );
    }


// End of file
