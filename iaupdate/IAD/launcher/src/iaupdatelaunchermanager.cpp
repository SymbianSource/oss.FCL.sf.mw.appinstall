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
* Description:   This module contains the implementation of CIAUpdateLauncherManager
*                class member functions.
*
*/



// INCLUDES
#include <eikenv.h>

#include "iaupdatelauncherappui.h"
#include "iaupdatelaunchermanager.h"
#include "iaupdatelauncherclient.h"
#include "iaupdatedebug.h"


// -----------------------------------------------------------------------------
// CIAUpdateLauncherManager::NewL
// 
// -----------------------------------------------------------------------------
// 
CIAUpdateLauncherManager* CIAUpdateLauncherManager::NewL( TBool& aRefreshFromNetworkDenied )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherManager::NewL() begin");
    CIAUpdateLauncherManager* self = CIAUpdateLauncherManager::NewLC( aRefreshFromNetworkDenied );
    CleanupStack::Pop( self );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherManager::NewL() end");
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateLauncherManager::NewLC
// 
// -----------------------------------------------------------------------------
//    
CIAUpdateLauncherManager* CIAUpdateLauncherManager::NewLC( TBool& aRefreshFromNetworkDenied )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherManager::NewLC() begin");
    CIAUpdateLauncherManager* self = new( ELeave ) CIAUpdateLauncherManager();
    CleanupStack::PushL( self );
    self->ConstructL( aRefreshFromNetworkDenied );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherManager::NewLC() end");
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateLauncherManager::CIAUpdateLauncherManager
// 
// -----------------------------------------------------------------------------
//    
CIAUpdateLauncherManager::CIAUpdateLauncherManager()
: CActive( CActive::EPriorityStandard )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherManager::CIAUpdateLauncherManager()");
    }

// -----------------------------------------------------------------------------
// CIAUpdateLauncherManager::ConstructL()
// 
// -----------------------------------------------------------------------------
//    
void CIAUpdateLauncherManager::ConstructL( TBool& aRefreshFromNetworkDenied )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherManager::ConstructL() begin");
    CActiveScheduler::Add( this );
    TInt error( iUpdateClient.Open() );
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateLauncherManager::ConstructL() error code: %d", error);
    if ( error == KErrNone )
        {    
        iUpdateClient.ShowUpdates( aRefreshFromNetworkDenied, iStatus );
        SetActive();        
        }
    else
        {
        iEikEnv = CEikonEnv::Static();
        CIAUpdateLauncherAppUi* appUi = 
       	            static_cast< CIAUpdateLauncherAppUi* >( iEikEnv->EikAppUi() );
        appUi->Exit();
        }
 
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherManager::ConstructL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateLauncherManager::~CIAUpdateLauncherManager
// 
// -----------------------------------------------------------------------------
//
CIAUpdateLauncherManager::~CIAUpdateLauncherManager()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherManager::~CIAUpdateLauncherManager() begin");
    Cancel();
    iUpdateClient.Close();
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherManager::~CIAUpdateLauncherManager() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateLauncherManager::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateLauncherManager::DoCancel()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherManager::DoCancel() begin");
    iUpdateClient.CancelAsyncRequest();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherManager::DoCancel() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateLauncherManager::RunL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateLauncherManager::RunL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherManager::RunL() begin");
    iEikEnv = CEikonEnv::Static();
    CIAUpdateLauncherAppUi* appUi = 
       	            static_cast< CIAUpdateLauncherAppUi* >( iEikEnv->EikAppUi() );
    appUi->Exit();
     
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateLauncherManager::RunL() end");
    }










