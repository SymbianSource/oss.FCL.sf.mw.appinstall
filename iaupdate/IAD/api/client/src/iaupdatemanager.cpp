/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdateManager
*                class member functions.
*
*/



// INCLUDES
#include <eikenv.h>

#include "iaupdatemanager.h"
#include "iaupdateclient.h"
#include "iaupdatedebug.h"



// -----------------------------------------------------------------------------
// CIAUpdateManager::NewL
// 
// -----------------------------------------------------------------------------
// 
CIAUpdateManager* CIAUpdateManager::NewL( MIAUpdateObserver& aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::NewL() begin");
    CIAUpdateManager* self = CIAUpdateManager::NewLC( aObserver );
    CleanupStack::Pop( self );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::NewL() end");
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::NewLC
// 
// -----------------------------------------------------------------------------
//    
CIAUpdateManager* CIAUpdateManager::NewLC( MIAUpdateObserver& aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::NewLC() begin");
    CIAUpdateManager* self = new( ELeave ) CIAUpdateManager();
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::NewLC() end");
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::CIAUpdateManager
// 
// -----------------------------------------------------------------------------
//    
CIAUpdateManager::CIAUpdateManager()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::CIAUpdateManager()");
    iEikEnv = CEikonEnv::Static();
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::ConstructL()
// 
// -----------------------------------------------------------------------------
//    
void CIAUpdateManager::ConstructL( MIAUpdateObserver& aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::ConstructL() begin");
    iUpdateClient = new IAUpdateClient( aObserver );
    if ( !iUpdateClient )
        {
        User::Leave(KErrNoMemory);
        }
    if ( iEikEnv )
        {
    	iEikEnv->AddForegroundObserverL(*this);
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::ConstructL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::~CIAUpdateManager
// 
// -----------------------------------------------------------------------------
//
CIAUpdateManager::~CIAUpdateManager()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::~CIAUpdateManager() begin");
    delete iUpdateClient;
    if ( iEikEnv )
        {
    	iEikEnv->RemoveForegroundObserver(*this);
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::~CIAUpdateManager() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::CheckUpdates
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateManager::CheckUpdates( const CIAUpdateParameters& aUpdateParameters )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::CheckUpdates() begin");
    
    iUpdateClient->checkUpdates( aUpdateParameters );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::CheckUpdates() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::ShowUpdates
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateManager::ShowUpdates( const CIAUpdateParameters& aUpdateParameters )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::ShowUpdates() begin");
    iUpdateClient->showUpdates( aUpdateParameters );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::ShowUpdates() end");
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateManager::Update
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateManager::Update( const CIAUpdateParameters& /*aUpdateParameters*/ )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::Update() begin");

    iUpdateClient->update();
   
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::Update() begin");
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::UpdateQuery
// 
// -----------------------------------------------------------------------------
//   
void CIAUpdateManager::UpdateQuery()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::UpdateQuery() begin");
        
    iUpdateClient->updateQuery();
          
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::UpdateQuery() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateManager::HandleGainingForeground
//  iaupdate.exe is brought to foreground if visible request is ongoing
// -----------------------------------------------------------------------------
//     
void CIAUpdateManager::HandleGainingForeground()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::HandleGainingForeground() begin");
	//iUpdateClient->broughtToForeground();   
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::HandleGainingForeground() end");  
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::HandleLosingForeground
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateManager::HandleLosingForeground()
    {
    }






