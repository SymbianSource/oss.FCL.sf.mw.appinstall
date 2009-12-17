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
* Description:   Implementation of CAppMngr2App class
*
*/


#include <eikstart.h>                   // EikStart
#include "appmngr2app.h"                // CAppMngr2App
#include "appmngr2document.h"           // CAppMngr2Document
#include <appmngr2common.hrh>           // KAppMngr2AppUidValue

const TUid KAppMngr2AppUid = { KAppMngr2AppUidValue };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2App::AppDllUid()
// ---------------------------------------------------------------------------
//
TUid CAppMngr2App::AppDllUid() const
    {
    return KAppMngr2AppUid;
    }

   
// ---------------------------------------------------------------------------
// CAppMngrApp::CreateDocumentL()
// ---------------------------------------------------------------------------
//
CApaDocument* CAppMngr2App::CreateDocumentL()
    {
    return CAppMngr2Document::NewL( *this );
    }


// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructs and returns an application object.
// ---------------------------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication()
    {
    return new CAppMngr2App;
    }

// ---------------------------------------------------------------------------
// Main function of the application executable.
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

