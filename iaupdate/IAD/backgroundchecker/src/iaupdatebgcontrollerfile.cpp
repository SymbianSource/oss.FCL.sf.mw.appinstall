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
* Description:    
*
*/



#include <bautils.h>
#include <s32file.h>
#include "iaupdatebgcontrollerfile.h"


const TInt KVersion( 1 );

// Constants to create the file.
_LIT( KFile, "iaupdatecontrollerfile");
_LIT( KPath, "c:\\private\\2000f85a\\");


// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::NewL
//
// -----------------------------------------------------------------------------
//
CIAUpdateBGControllerFile* CIAUpdateBGControllerFile::NewL()
    {
    CIAUpdateBGControllerFile* self =
        CIAUpdateBGControllerFile::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::NewLC
//
// -----------------------------------------------------------------------------
//
CIAUpdateBGControllerFile* CIAUpdateBGControllerFile::NewLC()
    {
    CIAUpdateBGControllerFile* self =
        new( ELeave) CIAUpdateBGControllerFile();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::CIAUpdateBGControllerFile
//
// -----------------------------------------------------------------------------
//
CIAUpdateBGControllerFile::CIAUpdateBGControllerFile()
: CBase(),
  iRefreshTime( 0 ),
  iLanguage( User::Language() )
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGControllerFile::ConstructL()
    {
    User::LeaveIfError( iFsSession.Connect() );
    iPath.Zero();
    iPath.Append( KPath );
    BaflUtils::EnsurePathExistsL( iFsSession, iPath );
    iPath.Append( KFile );
    
    // Read data from the file if the file exists.
    // Otherwise, let default values remain.
    ReadControllerDataL(); 
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::~CIAUpdateBGControllerFile
//
// -----------------------------------------------------------------------------
//
CIAUpdateBGControllerFile::~CIAUpdateBGControllerFile()
    {
    iFsSession.Close();
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::ReadControllerDataL
//
// -----------------------------------------------------------------------------
//
TBool CIAUpdateBGControllerFile::ReadControllerDataL()
    {
    RFile file;
    TInt err = file.Open( iFsSession, iPath, EFileRead );
    if ( err == KErrNotFound )
    	{
    	// File did not exist. 
    	// So, nothing to do here anymore.
    	return EFalse;
    	}
    User::LeaveIfError( err );
    	
    CleanupClosePushL( file );
    
    RFileReadStream stream( file, 0 );
    CleanupClosePushL( stream );

    InternalizeL( stream );

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &file );

    return ETrue;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::WriteControllerDataL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGControllerFile::WriteControllerDataL()
    {
    RFile file;
    User::LeaveIfError( file.Replace( iFsSession, iPath, EFileWrite ) );
    CleanupClosePushL( file );
    
    RFileWriteStream stream( file, 0 );
    CleanupClosePushL( stream );

    ExternalizeL( stream );
    
    stream.CommitL();
    
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &file );
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::RefreshTime
//
// -----------------------------------------------------------------------------
//
const TTime& CIAUpdateBGControllerFile::RefreshTime() const
    {
    return iRefreshTime;
    }
    

// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::SetRefreshTime
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGControllerFile::SetRefreshTime( const TTime& aTime )
    {
    iRefreshTime = aTime;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::SetRefreshTime
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGControllerFile::SetRefreshTime( TInt64 aTime )
    {
    iRefreshTime = aTime;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::SetCurrentRefreshTime
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGControllerFile::SetCurrentRefreshTime()
    {
    TTime universalTime;
    universalTime.UniversalTime();
    iRefreshTime = universalTime;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::Language
//
// -----------------------------------------------------------------------------
//
TLanguage CIAUpdateBGControllerFile::Language() const
    {
    return iLanguage;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::SetLanguage
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGControllerFile::SetLanguage( TLanguage aLanguage )
    {
    iLanguage = aLanguage;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGControllerFile::InternalizeL( RReadStream& aStream )
    {
    // If you make changes here, 
    // remember to update ExternalizeL accordingly!!!

    TInt version( aStream.ReadInt32L() );

    // Notice! In the future, some checking maybe required here
    // to be sure that file version is correct and the data can be
    // internalized correctly between different versions.
    if ( version != KVersion )
        {
        // For now, just leave with the corrupt error.
        User::Leave( KErrCorrupt );
        }
	
    TInt64 refreshTime( 0 );
    aStream >> refreshTime;
    SetRefreshTime( refreshTime );
    
    TInt language( aStream.ReadInt32L() );
    // Static casting is safe to do here because enum and TInt are the same.
    SetLanguage( static_cast< TLanguage >( language ) );
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGControllerFile::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGControllerFile::ExternalizeL( RWriteStream& aStream )
    {
    // If you make changes here, 
    // remember to update InternalizeL accordingly!!!

    aStream.WriteInt32L( KVersion );
	
    TInt64 refreshTime( RefreshTime().Int64() );
    aStream << refreshTime;

    TInt language( Language() );
    aStream.WriteInt32L( language );
    }
