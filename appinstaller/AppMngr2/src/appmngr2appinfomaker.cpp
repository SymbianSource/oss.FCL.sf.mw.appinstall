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
* Description:   Creates AppInfo objects asynchronously
*
*/


#include "appmngr2appinfomaker.h"       // CAppMngr2AppInfoMaker
#include "appmngr2infomakerobserver.h"  // MAppMngr2InfoMakerObserver
#include <appmngr2runtime.h>            // CAppMngr2Runtime
#include <appmngr2appinfo.h>            // CAppMngr2AppInfo
#include <appmngr2debugutils.h>         // FLOG macros


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2AppInfoMaker::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2AppInfoMaker* CAppMngr2AppInfoMaker::NewL( CAppMngr2Runtime& aPlugin,
        MAppMngr2InfoMakerObserver& aObserver, RFs& aFs )
    {
    CAppMngr2AppInfoMaker* self = CAppMngr2AppInfoMaker::NewLC( aPlugin,
            aObserver, aFs );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppInfoMaker::NewLC()
// ---------------------------------------------------------------------------
//
CAppMngr2AppInfoMaker* CAppMngr2AppInfoMaker::NewLC( CAppMngr2Runtime& aPlugin,
        MAppMngr2InfoMakerObserver& aObserver, RFs& aFs )
    {
    CAppMngr2AppInfoMaker* self = new (ELeave) CAppMngr2AppInfoMaker( aPlugin,
            aObserver, aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppInfoMaker::~CAppMngr2AppInfoMaker()
// ---------------------------------------------------------------------------
//
CAppMngr2AppInfoMaker::~CAppMngr2AppInfoMaker()
    {
    Cancel();
    iAppInfos.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppInfoMaker::DoCancel()
// ---------------------------------------------------------------------------
//
void CAppMngr2AppInfoMaker::DoCancel()
    {
    iPlugin.CancelGetInstalledApps();
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppInfoMaker::RunL()
// ---------------------------------------------------------------------------
//
void CAppMngr2AppInfoMaker::RunL()
    {
    if( iStatus.Int() == KErrNone )
        {
        iObserver.NewAppsCreatedL( *this, iAppInfos );
        }
    else
        {
        iObserver.ErrorInCreatingAppsL( *this, iStatus.Int() );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppInfoMaker::StartGettingInstalledAppsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2AppInfoMaker::StartGettingInstalledAppsL()
    {
    if( !IsActive() )
        {
        TRAPD( err, iPlugin.GetInstalledAppsL( iAppInfos, iFs, iStatus ) );
        SetActive();
        
        // Complete immediately if there was some error
        if( err != KErrNone )
            {
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, err );
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppInfoMaker::CAppMngr2AppInfoMaker()
// ---------------------------------------------------------------------------
//
CAppMngr2AppInfoMaker::CAppMngr2AppInfoMaker( CAppMngr2Runtime& aPlugin,
        MAppMngr2InfoMakerObserver& aObserver, RFs& aFs ) :
        CAppMngr2InfoMaker( aPlugin , aObserver, aFs )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2AppInfoMaker::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2AppInfoMaker::ConstructL()
    {
    }

