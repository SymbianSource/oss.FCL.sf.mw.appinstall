/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdateControllerFile 
*                class  member functions.
*
*/




#include <bautils.h>
#include <s32file.h>
#include <sysutil.h>

#include "iaupdatecontrollerfile.h"
#include "iaupdatectrlfileconsts.h"


// This constant will be used to check if the IAD application
// and its updates are using the same version of the file. If the versions
// differ, then this may require additional checking in the future versions
// when data is internalized. For example, previous IAD may give old version data 
// into the file during self update.
const TInt KVersion( 1 );

const TInt KSizeOfFile( 16 );
    

// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::NewL
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdateControllerFile* CIAUpdateControllerFile::NewL( 
    const TDesC& aFile )
    {
    CIAUpdateControllerFile* self =
        CIAUpdateControllerFile::NewLC( aFile );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::NewLC
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdateControllerFile* CIAUpdateControllerFile::NewLC( 
    const TDesC& aFile )
    {
    CIAUpdateControllerFile* self =
        new( ELeave) CIAUpdateControllerFile();
    CleanupStack::PushL( self );
    self->ConstructL( aFile );
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::CIAUpdateControllerFile
//
// -----------------------------------------------------------------------------
//
CIAUpdateControllerFile::CIAUpdateControllerFile()
: CBase(),
  iRefreshTime( 0 ),
  iLanguage( User::Language() )
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateControllerFile::ConstructL( const TDesC& aFile )
    {
    User::LeaveIfError( iFsSession.Connect() );
    User::LeaveIfError( 
        iFsSession.SetSessionToPrivate( 
            IAUpdateCtrlFileConsts::KDrive ) );
    
    User::LeaveIfError( iFsSession.SessionPath( iPath ) );
    BaflUtils::EnsurePathExistsL( iFsSession, iPath );
    iPath.Append( aFile );
    
    // Read data from the file if the file exists.
    // Otherwise, let default values remain.
    ReadControllerDataL(); 
    }


// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::~CIAUpdateControllerFile
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdateControllerFile::~CIAUpdateControllerFile()
    {
    iFsSession.Close();
    }


// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::ReadControllerDataL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIAUpdateControllerFile::ReadControllerDataL()
	{
	RFile file;
    TInt err = file.Open( iFsSession, iPath, EFileRead|EFileShareAny );
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
// CIAUpdateControllerFile::WriteControllerDataL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdateControllerFile::WriteControllerDataL()
	{
	TDriveUnit driveUnit( IAUpdateCtrlFileConsts::KDrive );
	if ( SysUtil::DiskSpaceBelowCriticalLevelL( 
	        &iFsSession, KSizeOfFile, driveUnit ) )
	    {
		User::Leave( KErrDiskFull );
	    }
	
	RFile file;
    User::LeaveIfError( 
        file.Replace( iFsSession, iPath, EFileWrite|EFileShareAny ) );
    CleanupClosePushL( file );
    
    RFileWriteStream stream( file, 0 );
    CleanupClosePushL( stream );

    ExternalizeL( stream );
    
    stream.CommitL();
    
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &file );
	}


// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::SetCurrentData
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdateControllerFile::SetCurrentData()
    {
    SetCurrentRefreshTime();
    SetCurrentLanguage();
    }


// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::RefreshTime
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TTime& CIAUpdateControllerFile::RefreshTime() const
    {
    return iRefreshTime;
    }
    

// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::SetRefreshTime
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdateControllerFile::SetRefreshTime( const TTime& aTime )
    {
    iRefreshTime = aTime;
    }


// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::SetRefreshTime
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdateControllerFile::SetRefreshTime( TInt64 aTime )
    {
    iRefreshTime = aTime;
    }


// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::SetCurrentRefreshTime
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdateControllerFile::SetCurrentRefreshTime()
    {
    TTime universalTime;
    universalTime.UniversalTime();
    iRefreshTime = universalTime;
    }


// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::Language
//
// -----------------------------------------------------------------------------
//
EXPORT_C TLanguage CIAUpdateControllerFile::Language() const
    {
    return iLanguage;
    }


// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::SetLanguage
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdateControllerFile::SetLanguage( TLanguage aLanguage )
    {
    iLanguage = aLanguage;
    }


// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::SetCurrentLanguage
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdateControllerFile::SetCurrentLanguage()
    {
    SetLanguage( User::Language() );
    }


// -----------------------------------------------------------------------------
// CIAUpdateControllerFile::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateControllerFile::InternalizeL( RReadStream& aStream )
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
// CIAUpdateControllerFile::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateControllerFile::ExternalizeL( RWriteStream& aStream )
	{
	// If you make changes here, 
	// remember to update InternalizeL accordingly!!!

    aStream.WriteInt32L( KVersion );
	
	TInt64 refreshTime( RefreshTime().Int64() );
	aStream << refreshTime;

    TInt language( Language() );
    aStream.WriteInt32L( language );
	}
