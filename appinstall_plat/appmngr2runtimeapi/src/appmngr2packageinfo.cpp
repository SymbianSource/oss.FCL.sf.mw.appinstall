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
* Description:   PackageInfo implementation
*
*/


#include "appmngr2packageinfo.h"
#include <StringLoader.h>               // StringLoader
#include <AknQueryDialog.h>             // CAknQueryDialog
//#include <appmngr2.rsg>                 // Resource IDs


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfo::ConstructL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2PackageInfo::ConstructL()
    {
    CAppMngr2InfoBase::ConstructL();    // base class
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfo::~CAppMngr2PackageInfo()
// ---------------------------------------------------------------------------
//
EXPORT_C CAppMngr2PackageInfo::~CAppMngr2PackageInfo()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfo::CAppMngr2PackageInfo()
// ---------------------------------------------------------------------------
//
EXPORT_C CAppMngr2PackageInfo::CAppMngr2PackageInfo( CAppMngr2Runtime& aRuntime,
        RFs& aFsSession ) : CAppMngr2InfoBase( aRuntime, aFsSession )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfo::ShowDeleteConfirmationQueryL()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAppMngr2PackageInfo::ShowDeleteConfirmationQueryL()
    {
    /* Temporary fix until appmngr2pluginapi is removed

    HBufC* deleteQuery = StringLoader::LoadLC( R_AM_DELETEQUERY, Name() );

    CAknQueryDialog* dlg = CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );
    TBool okToDelete = dlg->ExecuteLD( R_APPMNGR2_YES_NO_CONFIRMATION_QUERY, *deleteQuery );
    CleanupStack::PopAndDestroy( deleteQuery );

    return okToDelete;
    */
    return ETrue;
    }

