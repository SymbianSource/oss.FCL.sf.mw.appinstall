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
* Description:   Utility class to listen SWI operations.
*
*/


#include "appmngr2sisxswimonitor.h"     // CAppMngr2SisxSwiMonitor
#include <swi/swispubsubdefs.h>         // KUidSoftwareInstallKey
#include <appmngr2debugutils.h>         // FLOG macros


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2SisxSwiMonitor::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxSwiMonitor* CAppMngr2SisxSwiMonitor::NewL( MAppMngr2RuntimeObserver& aObs )
    {
    CAppMngr2SisxSwiMonitor* self = new( ELeave ) CAppMngr2SisxSwiMonitor( aObs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxSwiMonitor::~CAppMngr2SisxSwiMonitor()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxSwiMonitor::~CAppMngr2SisxSwiMonitor()
    {
    Cancel();
    iSwInstallKey.Close();
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxSwiMonitor::DoCancel()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxSwiMonitor::DoCancel()
    {
    iSwInstallKey.Cancel();
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxSwiMonitor::RunL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxSwiMonitor::RunL()
    {
    iSwInstallKey.Subscribe( iStatus );
    SetActive();
    
    TInt value = 0;
    if( iSwInstallKey.Get( value ) == KErrNone )
        {
        TInt operation( value & Swi::KSwisOperationMask );
        TInt operationStatus( value & Swi::KSwisOperationStatusMask );
        FLOG( "CAppMngr2SisxSwiMonitor::RunL(): operation %d, status %d",
                operation, operationStatus );
        
        if( operationStatus == Swi::ESwisStatusSuccess && (
                operation == Swi::ESwisInstall ||
                operation == Swi::ESwisUninstall ||
                operation == Swi::ESwisRestore ) )
            {
            iObs.RefreshInstalledApps();
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxSwiMonitor::CAppMngr2SisxSwiMonitor()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxSwiMonitor::CAppMngr2SisxSwiMonitor( MAppMngr2RuntimeObserver& aObs )
        : CActive( CActive::EPriorityStandard ), iObs( aObs )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxSwiMonitor::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxSwiMonitor::ConstructL()
    {
    User::LeaveIfError( iSwInstallKey.Attach( KUidSystemCategory, Swi::KUidSoftwareInstallKey ) );
    iSwInstallKey.Subscribe( iStatus );
    SetActive();
    
    FLOG( "CAppMngr2SisxSwiMonitor::ConstructL(): subscribed to Swi::KUidSoftwareInstallKey" );
    }

