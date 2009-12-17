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
* Description:   AppInfo implementation
*
*/


#include "appmngr2appinfo.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2AppInfo::ConstructL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2AppInfo::ConstructL()
    {
    CAppMngr2InfoBase::ConstructL();
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppInfo::~CAppMngr2AppInfo()
// ---------------------------------------------------------------------------
//
EXPORT_C CAppMngr2AppInfo::~CAppMngr2AppInfo()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppInfo::CAppMngr2AppInfo()
// ---------------------------------------------------------------------------
//
EXPORT_C CAppMngr2AppInfo::CAppMngr2AppInfo( CAppMngr2Runtime& aRuntime,
        RFs& aFsSession ) : CAppMngr2InfoBase( aRuntime, aFsSession )
    {
    }

