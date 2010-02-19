/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Recognizes files to get corresponding MIME types
*
*/


#include "appmngr2filerecognizer.h"     // CAppMngr2FileRecognizer
#include <appmngr2recognizedfile.h>     // CAppMngr2RecognizedFile
#include <f32file.h>                    // RFs, CDir
#include <apmstd.h>                     // TDataType
#include <caf/caf.h>                    // ContentAccess


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2FileRecognizer::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2FileRecognizer* CAppMngr2FileRecognizer::NewL( RFs& aFs )
    {
    CAppMngr2FileRecognizer* self = new (ELeave) CAppMngr2FileRecognizer( aFs );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2FileRecognizer::~CAppMngr2FileRecognizer()
// ---------------------------------------------------------------------------
//
CAppMngr2FileRecognizer::~CAppMngr2FileRecognizer()
    {
    Cancel();
    iDirStatus = NULL;
    delete iDirPath;
    delete iDirEntries;
    iResultArray.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CAppMngr2FileRecognizer::RecognizeFilesL()
// ---------------------------------------------------------------------------
//
void CAppMngr2FileRecognizer::RecognizeFilesL( const TDesC& aPath,
        TRequestStatus& aStatus )
    {
    if( IsActive() )
        {
        User::Leave( KErrInUse );
        }

    if( iDirPath )
        {
        delete iDirPath;
        iDirPath = NULL;
        }
    iDirPath = aPath.AllocL();

    if( iDirEntries )
        {
        delete iDirEntries;
        iDirEntries = NULL;
        }
    
    TInt err = iFs.GetDir( *iDirPath, KEntryAttNormal, ESortNone, iDirEntries );
    User::LeaveIfError( err );

    aStatus = KRequestPending;  // for recognizing all files in the directory
    iDirStatus = &aStatus;

    iResultArray.ResetAndDestroy();
    iIndex = 0;
    iState = ERecognizing;
    
    // start recognizing the first file asynchronously
    SetActive();
    TRequestStatus* fileStatus = &iStatus;
    User::RequestComplete( fileStatus, KErrNone );
    }

// ---------------------------------------------------------------------------
// CAppMngr2FileRecognizer::CancelRecognizeFiles()
// ---------------------------------------------------------------------------
//
void CAppMngr2FileRecognizer::CancelRecognizeFiles()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CAppMngr2FileRecognizer::Results()
// ---------------------------------------------------------------------------
//
RPointerArray<CAppMngr2RecognizedFile>& CAppMngr2FileRecognizer::Results()
    {
    return iResultArray;
    }

// ---------------------------------------------------------------------------
// CAppMngr2FileRecognizer::DoCancel()
// ---------------------------------------------------------------------------
//
void CAppMngr2FileRecognizer::DoCancel()
    {
    switch( iState )
        {
        case EIdle:
            break;
        case ERecognizing:
            iState = EIdle;
            if( iDirStatus )
                {
                User::RequestComplete( iDirStatus, KErrCancel );
                iDirStatus = NULL;
                }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2FileRecognizer::RunL()
// ---------------------------------------------------------------------------
//
void CAppMngr2FileRecognizer::RunL()
    {
    User::LeaveIfError( iStatus.Int() );
    
    switch( iState )
        {
        case EIdle:
            break;
        case ERecognizing:
            RecognizeNextFileL();
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2FileRecognizer::RunError()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2FileRecognizer::RunError( TInt aError )
    {
    // RunL() leaved, pass the error code to caller
    if( iDirStatus )
        {
        User::RequestComplete( iDirStatus, aError );
        iDirStatus = NULL;
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CAppMngr2FileRecognizer::CAppMngr2FileRecognizer()
// ---------------------------------------------------------------------------
//
CAppMngr2FileRecognizer::CAppMngr2FileRecognizer( RFs& aFs ) :
        CActive( CActive::EPriorityLow ), iFs( aFs )
    {
    // EPriorityLow used for smooth operation. Low priority allows UI refreshes
    // to proceed when some files have been recognized (even if there are still
    // more files to be recognized and recognizer is waiting for the next file
    // to be recognized).
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CAppMngr2FileRecognizer::RecognizeNextFileL()
// ---------------------------------------------------------------------------
//
void CAppMngr2FileRecognizer::RecognizeNextFileL()
    {
    if( iIndex < iDirEntries->Count() )
        {
        // recognize the file which index is iIndex
        const TEntry& file = ( *iDirEntries )[ iIndex ];
        
        HBufC* fullName = HBufC::NewLC( iDirPath->Length() + file.iName.Length() );
        TPtr namePtr( fullName->Des() );
        namePtr.Copy( *iDirPath );
        namePtr.Append( file.iName );
        
        ContentAccess::CContent* content = ContentAccess::CContent::NewLC(
                *fullName, ContentAccess::EContentShareReadWrite );
        
        HBufC* mimeType = HBufC::NewLC( KMaxDataTypeLength );
        TPtr mimePtr( mimeType->Des() );
        User::LeaveIfError( content->GetStringAttribute( ContentAccess::EMimeType, mimePtr ) );

        CAppMngr2RecognizedFile* recFile = CAppMngr2RecognizedFile::NewL( fullName, mimeType );
        CleanupStack::Pop( mimeType );		// CAppMngr2RecognizedFile takes ownership
        CleanupStack::PopAndDestroy( content );
        CleanupStack::Pop( fullName );
        
        CleanupStack::PushL( recFile );
        iResultArray.AppendL( recFile );
        CleanupStack::Pop( recFile );

        // and then recognize the next file
        iIndex++;
        SetActive();
        TRequestStatus* fileStatus = &iStatus;
        User::RequestComplete( fileStatus, KErrNone );
        }
    else
        {
        // all done, directory has been processed
        User::RequestComplete( iDirStatus, KErrNone );
        iDirStatus = NULL;
        }
    }

