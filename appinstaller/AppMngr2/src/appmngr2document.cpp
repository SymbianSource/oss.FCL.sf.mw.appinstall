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
* Description:   Implementation of CAppMngr2Document class
*
*/


#include "appmngr2document.h"           // CAppMngr2Document
#include "appmngr2appui.h"              // CAppMngr2AppUi


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2Document::CAppMngr2Document()
// ---------------------------------------------------------------------------
//
CAppMngr2Document::CAppMngr2Document( CEikApplication& aApp )
        : CAknDocument( aApp )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2Document::~CAppMngr2Document()
// ---------------------------------------------------------------------------
//
CAppMngr2Document::~CAppMngr2Document()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2Document::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Document::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2Document::NewL
// ---------------------------------------------------------------------------
//
CAppMngr2Document* CAppMngr2Document::NewL( CEikApplication& aApp )
    {
    CAppMngr2Document* self = new (ELeave) CAppMngr2Document( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CAppMngr2Document::CreateAppUiL()
// ----------------------------------------------------
//
CEikAppUi* CAppMngr2Document::CreateAppUiL()
    {
    return new (ELeave) CAppMngr2AppUi;
    }

