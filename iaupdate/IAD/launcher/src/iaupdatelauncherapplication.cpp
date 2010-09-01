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
* Description:   IAUpdate client launcher.
*
*/




// INCLUDE FILES
#include "iaupdatelauncherdocument.h"
#include "iaupdatelauncherapplication.h"
#include "iaupdatelauncheruids.h"

// ============================ MEMBER FUNCTIONS ===============================

// UID for the application;
// this should correspond to the uid defined in the mmp file
static const TUid KUidIaupdateLauncherApp = { KIAUpdateLauncherUid };


// -----------------------------------------------------------------------------
// CIAUpdateLauncherApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CIAUpdateLauncherApplication::CreateDocumentL()
    {
    // Create an Launcher document, and return a pointer to it
    return (static_cast<CApaDocument*>
                    ( CIAUpdateLauncherDocument::NewL( *this ) ) );
    }

// -----------------------------------------------------------------------------
// CIAUpdateLauncherApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CIAUpdateLauncherApplication::AppDllUid() const
    {
    // Return the UID for the Launcher application
    return KUidIaupdateLauncherApp;
    }

// End of File

