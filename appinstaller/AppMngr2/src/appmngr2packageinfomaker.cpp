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
* Description:   Creates PackageInfo objects asynchronously
*
*/


#include "appmngr2packageinfomaker.h"   // CAppMngr2PackageInfoMaker
#include "appmngr2infomakerobserver.h"  // MAppMngr2InfoMakerObserver
#include <appmngr2runtime.h>            // CAppMngr2Runtime
#include <appmngr2packageinfo.h>        // CAppMngr2PackageInfo
#include <appmngr2recognizedfile.h>     // CAppMngr2RecognizedFile
#include <appmngr2debugutils.h>         // FLOG macros


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfoMaker::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2PackageInfoMaker* CAppMngr2PackageInfoMaker::NewL( CAppMngr2Runtime& aPlugin,
        MAppMngr2InfoMakerObserver& aObserver, RFs& aFs )
    {
    CAppMngr2PackageInfoMaker* self = CAppMngr2PackageInfoMaker::NewLC( aPlugin,
            aObserver, aFs );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfoMaker::NewLC()
// ---------------------------------------------------------------------------
//
CAppMngr2PackageInfoMaker* CAppMngr2PackageInfoMaker::NewLC( CAppMngr2Runtime& aPlugin,
        MAppMngr2InfoMakerObserver& aObserver, RFs& aFs )
    {
    CAppMngr2PackageInfoMaker* self = new (ELeave) CAppMngr2PackageInfoMaker( aPlugin,
            aObserver, aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfoMaker::~CAppMngr2PackageInfoMaker()
// ---------------------------------------------------------------------------
//
CAppMngr2PackageInfoMaker::~CAppMngr2PackageInfoMaker()
    {
    Cancel();
    iFiles.ResetAndDestroy();
    iPackageInfos.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfoMaker::DoCancel()
// ---------------------------------------------------------------------------
//
void CAppMngr2PackageInfoMaker::DoCancel()
    {
    iPlugin.CancelGetInstallationFiles();
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfoMaker::RunL()
// ---------------------------------------------------------------------------
//
void CAppMngr2PackageInfoMaker::RunL()
    {
    if( iStatus.Int() == KErrNone )
        {
        iObserver.NewPackagesCreatedL( *this, iPackageInfos );
        }
    else
        {
        iObserver.ErrorInCreatingPackagesL( *this, iStatus.Int() );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfoMaker::AddFileL()
// ---------------------------------------------------------------------------
//
void CAppMngr2PackageInfoMaker::AddFileL( CAppMngr2RecognizedFile* aFile )
    {
    iFiles.AppendL( aFile );
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfoMaker::StartGettingInstallationFilesL()
// ---------------------------------------------------------------------------
//
void CAppMngr2PackageInfoMaker::StartGettingInstallationFilesL()
    {
    if( !IsActive() )
        {
        TRAPD( err, iPlugin.GetInstallationFilesL( iPackageInfos, iFiles,
                iFs, iStatus ) );
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
// CAppMngr2PackageInfoMaker::CAppMngr2PackageInfoMaker()
// ---------------------------------------------------------------------------
//
CAppMngr2PackageInfoMaker::CAppMngr2PackageInfoMaker( CAppMngr2Runtime& aPlugin,
        MAppMngr2InfoMakerObserver& aObserver, RFs& aFs ) :
        CAppMngr2InfoMaker( aPlugin , aObserver, aFs )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2PackageInfoMaker::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2PackageInfoMaker::ConstructL()
    {
    }

