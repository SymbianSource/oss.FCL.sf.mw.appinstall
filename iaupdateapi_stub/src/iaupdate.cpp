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
* Description:   CIAUpdate
*
*/


#include <iaupdate.h>


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
    
void CIAUpdate::ConstructL( MIAUpdateObserver& /*aObserver*/ )
    {
    // This is just a stub file.
    // This stuf file does not provide any functionality.
    // So, leave during construction and inform possible user
    // that this functionality is not supported.
    User::Leave( KErrNotSupported );
    }


EXPORT_C CIAUpdate::~CIAUpdate()
    {
    // Nothing to do here.
    }


EXPORT_C void CIAUpdate::CheckUpdates( const CIAUpdateParameters& /*aUpdateParameters*/ )
    {
    }


EXPORT_C void CIAUpdate::ShowUpdates( const CIAUpdateParameters& /*aUpdateParameters*/ )
    {
    }
    

EXPORT_C void CIAUpdate::Update( const CIAUpdateParameters& /*aUpdateParameters*/ )
    {
    }


EXPORT_C void CIAUpdate::UpdateQuery()
    {
    }


CIAUpdateManager& CIAUpdate::UpdateManager() const
    {
    // This class object leaves with KErrNotSupported when
    // NewL or NewLC is called. So, it does not matter even
    // if we return reference to NULL here.
    CIAUpdateManager* ptr( NULL );
    return *ptr;
    }
