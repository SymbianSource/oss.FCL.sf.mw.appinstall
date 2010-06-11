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
* Description:   Directory scanner
*
*/


#include "appmngr2scannerdir.h"         // CAppMngr2ScannerDir
#include "appmngr2scannerdirobserver.h" // MAppMngr2ScannerDirObserver
#include <driveinfo.h>                  // DriveInfo

_LIT( KBackslash, "\\" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2ScannerDir::NewLC()
// ---------------------------------------------------------------------------
//
CAppMngr2ScannerDir* CAppMngr2ScannerDir::NewL( RFs& aFs, const TDesC& aDir,
        MAppMngr2ScannerDirObserver& aObs )
    {
    CAppMngr2ScannerDir* self = new (ELeave) CAppMngr2ScannerDir( aFs, aObs );
    CleanupStack::PushL( self );
    self->ConstructL( aDir );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2ScannerDir::~CAppMngr2ScannerDir()
// ---------------------------------------------------------------------------
//
CAppMngr2ScannerDir::~CAppMngr2ScannerDir()
    {
    Cancel();
    delete iDir;
    }

// ---------------------------------------------------------------------------
// CAppMngr2ScannerDir::DirName()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2ScannerDir::DirName() const
    {
    if( iDir )
        {
        return *iDir;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CAppMngr2ScannerDir::StartWatchingChanges()
// ---------------------------------------------------------------------------
//
void CAppMngr2ScannerDir::StartWatchingChanges()
    {
    if( !IsActive() )
        {
        iFs.NotifyChange( ENotifyEntry, iStatus, *iDir );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ScannerDir::StopWatchingChanges()
// ---------------------------------------------------------------------------
//
void CAppMngr2ScannerDir::StopWatchingChanges()
    {
    if( IsActive() )
        {
        Cancel();
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ScannerDir::DoCancel()
// ---------------------------------------------------------------------------
//
void CAppMngr2ScannerDir::DoCancel()
    {
    iFs.NotifyChangeCancel();
    }

// ---------------------------------------------------------------------------
// CAppMngr2ScannerDir::RunL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ScannerDir::RunL()
    {
    // Status is KErrCancel when StopWatchingChanges() has been called
    if( iStatus.Int() == KErrNone )
        {
        iFs.NotifyChange( ENotifyEntry, iStatus, *iDir );
        SetActive();
    
        iObserver.DirectoryChangedL( *iDir );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2ScannerDir::CAppMngr2ScannerDir()
// ---------------------------------------------------------------------------
//
CAppMngr2ScannerDir::CAppMngr2ScannerDir( RFs& aFs, MAppMngr2ScannerDirObserver& aObs ) :
        CActive( CActive::EPriorityStandard ), iObserver( aObs ), iFs( aFs )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CAppMngr2ScannerDir::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2ScannerDir::ConstructL( const TDesC& aDir )
    {
    // Check if aDir has trailing backslash, add it if it does not have.
    // RFs::NotifyChange() and also RApaLsSession::RecognizeFilesL() that scanner
    // uses require trailing backslashes in directory names, hence create iDir so
    // that is always ends with trailing backslash.
    if( aDir.Right( 1 ).Compare( KBackslash ) == 0 )
        {
        // RFs::IsValidName() does not accept trailing backslashes, so check the
        // the directory name first without trailing backslash
        TPtrC dirWithoutTrailingString( aDir.Left( aDir.Length() - 1 ) );
        if( !iFs.IsValidName( dirWithoutTrailingString ) )
            {
            User::LeaveIfError( KErrBadName );
            }
        iDir = aDir.AllocL();
        }
    else
        {
        // No trailing backslash, check the dir name and then add the backslash
        if( !iFs.IsValidName( aDir ) )
            {
            User::LeaveIfError( KErrBadName );
            }
        iDir = HBufC::NewL( aDir.Length() + 1 );
        TPtr dirPtr = iDir->Des();
        dirPtr.Copy( aDir );
        dirPtr.Append( KBackslash );
        }
    }

