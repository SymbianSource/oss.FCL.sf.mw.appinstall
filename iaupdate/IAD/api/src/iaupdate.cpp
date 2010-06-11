/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdate class 
*                member functions.
*
*/



// INCLUDES
#include <iaupdate.h>
#include "iaupdatemanager.h"


EXPORT_C CIAUpdate* CIAUpdate::NewL( MIAUpdateObserver& aObserver )
    {
    CIAUpdate* self = CIAUpdate::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CIAUpdate* CIAUpdate::NewLC( MIAUpdateObserver& aObserver )
    {
    CIAUpdate* self = new( ELeave ) CIAUpdate();
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    return self;
    }

    
CIAUpdate::CIAUpdate()
: CBase()
    {
    
    }
    
void CIAUpdate::ConstructL( MIAUpdateObserver& aObserver )
    {
    iUpdateManager = CIAUpdateManager::NewL( aObserver );
    }


EXPORT_C CIAUpdate::~CIAUpdate()
    {
    delete iUpdateManager;
    }


EXPORT_C void CIAUpdate::CheckUpdates( const CIAUpdateParameters& aUpdateParameters )
    {
    // Forward request to the actual updater
    UpdateManager().CheckUpdates( aUpdateParameters );
    }


EXPORT_C void CIAUpdate::ShowUpdates( const CIAUpdateParameters& aUpdateParameters )
    {
    // Forward request to the actual updater
    UpdateManager().ShowUpdates( aUpdateParameters );    
    }
    

EXPORT_C void CIAUpdate::Update( const CIAUpdateParameters& aUpdateParameters )
    {
    // Forward request to the actual updater
    UpdateManager().Update( aUpdateParameters );        
    }


EXPORT_C void CIAUpdate::UpdateQuery()
    {
    UpdateManager().UpdateQuery();
    }


CIAUpdateManager& CIAUpdate::UpdateManager() const
    {
    return *iUpdateManager;
    }
