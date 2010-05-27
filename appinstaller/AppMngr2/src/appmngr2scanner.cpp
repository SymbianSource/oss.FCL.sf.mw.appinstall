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


#include "appmngr2scanner.h"            // CAppMngr2Scanner
#include "appmngr2scannerdir.h"         // CAppMngr2ScannerDir
#include "appmngr2scannerobserver.h"    // MAppMngr2ScannerObserver
#include "appmngr2filerecognizer.h"     // CAppMngr2FileRecognizer
#include <appmngr2debugutils.h>         // FLOG macros
#include <driveinfo.h>                  // DriveInfo


// ======== LOCAL FUNCTIONS =========

static TBool EqualF( const CAppMngr2ScannerDir& p, const CAppMngr2ScannerDir& q )
    {
    return ( p.DirName().CompareF( q.DirName() ) == 0 );
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2Scanner::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2Scanner* CAppMngr2Scanner::NewL( MAppMngr2ScannerObserver& aObs )
    {
    CAppMngr2Scanner* self = new (ELeave) CAppMngr2Scanner( aObs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2Scanner::~CAppMngr2Scanner()
// ---------------------------------------------------------------------------
//
CAppMngr2Scanner::~CAppMngr2Scanner()
    {
    Cancel();
    iDirs.ResetAndDestroy();
    delete iRecognizer;
    iFs.Close();
    }

// ---------------------------------------------------------------------------
// CAppMngr2Scanner::DoCancel()
// ---------------------------------------------------------------------------
//
void CAppMngr2Scanner::DoCancel()
    {
    switch( iState )
        {
        case EScanning:
            iRecognizer->CancelRecognizeFiles();
            iState = EIdle;
            break;
            
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2Scanner::RunL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Scanner::RunL()
    {
    switch( iState )
        {
        case EScanning:
            HandleScanningResultsL();
            break;

        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2Scanner::RunError()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2Scanner::RunError( TInt aError )
    {
    FLOG( "Scanning error %d", aError );
    
    // RunL() leaved, scanner is not active any more, inform observer
    iObserver.ScanningComplete();
    iState = EIdle;
    return aError;
    }

// ---------------------------------------------------------------------------
// CAppMngr2Scanner::DirectoryChangedL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Scanner::DirectoryChangedL( const TDesC& aDirName )
    {
    iObserver.DirectoryChangedL( aDirName );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Scanner::AddDirectoryL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Scanner::AddDirectoryL( const TDesC& aPath )
    {
    __ASSERT_ALWAYS( iState == EIdle, User::Leave( KErrInUse ) );

    // ignore invalid directory names, CAppMngr2ScannerDir checks it
    CAppMngr2ScannerDir* dir = NULL;
    TRAPD( err, dir = CAppMngr2ScannerDir::NewL( iFs, aPath, *this ) );
    if( err == KErrNone )
        {
        CleanupStack::PushL( dir );
        
        // add to scanning directories, ignore duplicates
        TIdentityRelation<CAppMngr2ScannerDir> match( EqualF );
        TInt findResult = iDirs.Find( dir, match ); 
        if( findResult == KErrNotFound )
            {
            FLOG( "Scanner: dir %S added", &( dir->DirName() ) );
            iDirs.AppendL( dir );
            CleanupStack::Pop( dir );
            }
        else
            {
            CleanupStack::PopAndDestroy( dir );
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2Scanner::StartScanningL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Scanner::StartScanningL()
    {
    FLOG( "CAppMngr2Scanner::StartScanningL()" );
    
    // cancel possible previous, still on-going scanning
    if( IsActive() )
        {
        FLOG( "CAppMngr2Scanner::StartScanningL(): previous scan cancelled" );
        Cancel();
        iObserver.ScanningComplete();
        }
    
    // find first valid directory name to scan
    iScanningIndex = -1;
    NextValidScanningIndex();
    
    // start scanning
    FLOG( "Scanning %d: %S", iScanningIndex, &( iDirs[ iScanningIndex ]->DirName() ) );
    iDirs[ iScanningIndex ]->StopWatchingChanges();
    iRecognizer->RecognizeFilesL( iDirs[ iScanningIndex ]->DirName(), iStatus );
    iState = EScanning;
    SetActive();
    }

// ---------------------------------------------------------------------------
// CAppMngr2Scanner::CAppMngr2Scanner()
// ---------------------------------------------------------------------------
//
CAppMngr2Scanner::CAppMngr2Scanner( MAppMngr2ScannerObserver& aObs ) :
        CActive( CActive::EPriorityStandard ), iObserver( aObs )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Scanner::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Scanner::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    iRecognizer = CAppMngr2FileRecognizer::NewL( iFs );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Scanner::NextValidScanningIndex()
// ---------------------------------------------------------------------------
//
void CAppMngr2Scanner::NextValidScanningIndex()
    {
    TInt err;
    TEntry entry;
    TInt dirCount = iDirs.Count();
    
    iScanningIndex++;
    while( iScanningIndex < dirCount )
        {
        err = iFs.Entry( iDirs[ iScanningIndex ]->DirName(), entry );
        if( err == KErrNone )
            {
            break;
            }
        FLOG( "Scanner skips %d: %S, err = %d", iScanningIndex,
                &( iDirs[ iScanningIndex ]->DirName() ), err );
        iScanningIndex++;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2Scanner::HandleScanningResultsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Scanner::HandleScanningResultsL()
    {
    FLOG( "Scanning result: %d, for %S, %d files", iStatus.Int(),
            &( iDirs[ iScanningIndex ]->DirName() ), iRecognizer->Results().Count() );

    // start monitoring changes in this directory
    iDirs[ iScanningIndex ]->StartWatchingChanges();
    
    // report results if there were no errors
    if( iStatus.Int() == KErrNone )
        {
        TRAP_IGNORE( iObserver.ScanningResultL( iRecognizer->Results() ) );
        }
    
    // get next valid directory name
    NextValidScanningIndex();

    // start file recognizer, or notify that all done
    if( iScanningIndex < iDirs.Count() )
        {
        FLOG( "Scanning %d: %S", iScanningIndex,
                &( iDirs[ iScanningIndex ]->DirName() ) );
        iDirs[ iScanningIndex ]->StopWatchingChanges();
        iRecognizer->RecognizeFilesL( iDirs[ iScanningIndex ]->DirName(), iStatus );
        SetActive();
        }
    else
        {
        FLOG( "Scanning complete" );
        iObserver.ScanningComplete();
        iState = EIdle;
        }
    }

