/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Unsupported old RSWInstSilentLauncher API
*
*/

#include "SWInstApi.h"
#include "SWInstDefs.h"
#include <e32debug.h>

_LIT(KUnsupportedMsg, "RSWInstSilentLauncher is obsolete. Use Usif::RSoftwareInstall instead.");

using namespace SwiUI;

#define KSWInstSilentInstallServiceUid 0x102072F0


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// RSWInstSilentLauncher::RSWInstSilentLauncher
// -----------------------------------------------------------------------------
//
EXPORT_C RSWInstSilentLauncher::RSWInstSilentLauncher()
    {
    RDebug::Print(KUnsupportedMsg);
    }

// -----------------------------------------------------------------------------
// RSWInstSilentLauncher::Connect
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstSilentLauncher::Connect()
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstSilentLauncher::ServiceUid
// -----------------------------------------------------------------------------
//
TUid RSWInstSilentLauncher::ServiceUid() const
    {
    return TUid::Uid( KSWInstSilentInstallServiceUid );    
    }


