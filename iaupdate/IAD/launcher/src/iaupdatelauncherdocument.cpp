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
* Description:   This module contains the implementation of CIAUpdateLauncherDocument class 
*                member functions.
*
*/



// INCLUDE FILES
#include "iaupdatelauncherappui.h"
#include "iaupdatelauncherdocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIAUpdateLauncherDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateLauncherDocument* CIAUpdateLauncherDocument::NewL( CEikApplication& aApp )
    {
    CIAUpdateLauncherDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateLauncherDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateLauncherDocument* CIAUpdateLauncherDocument::NewLC( CEikApplication& aApp )
    {
    CIAUpdateLauncherDocument* self =
        new ( ELeave ) CIAUpdateLauncherDocument( aApp );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateLauncherDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateLauncherDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CIAUpdateLauncherDocument::CIAUpdateLauncherDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateLauncherDocument::CIAUpdateLauncherDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CIAUpdateLauncherDocument::~CIAUpdateLauncherDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CIAUpdateLauncherDocument::~CIAUpdateLauncherDocument()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CIAUpdateLauncherDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CIAUpdateLauncherDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return ( static_cast <CEikAppUi*> ( new ( ELeave ) CIAUpdateLauncherAppUi ) );
    }

// End of File

