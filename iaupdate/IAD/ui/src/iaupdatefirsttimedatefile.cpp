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
* Description:   This module contains the implementation of CIAUpdateFirstTimeDateFile 
*                class  member functions.
*
*/




#include <bautils.h>
#include <s32file.h>
#include <sysutil.h>

#include "iaupdatefirsttimedatefile.h"


const TInt KDrive( EDriveC );
const TInt KSizeOfFile( 8 );
    

// -----------------------------------------------------------------------------
// CIAUpdateFirstTimeDateFile::NewL
//
// -----------------------------------------------------------------------------
//
CIAUpdateFirstTimeDateFile* CIAUpdateFirstTimeDateFile::NewL( 
    const TDesC& aFile )
    {
    CIAUpdateFirstTimeDateFile* self =
        CIAUpdateFirstTimeDateFile::NewLC( aFile );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateFirstTimeDateFile::NewLC
//
// -----------------------------------------------------------------------------
//
CIAUpdateFirstTimeDateFile* CIAUpdateFirstTimeDateFile::NewLC( 
    const TDesC& aFile )
    {
    CIAUpdateFirstTimeDateFile* self =
        new( ELeave) CIAUpdateFirstTimeDateFile();
    CleanupStack::PushL( self );
    self->ConstructL( aFile );
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdateFirstTimeDateFile::CIAUpdateFirstTimeDateFile
//
// -----------------------------------------------------------------------------
//
CIAUpdateFirstTimeDateFile::CIAUpdateFirstTimeDateFile()
: CBase(),
  iFirstTime( 0 )
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdateFirstTimeDateFile::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFirstTimeDateFile::ConstructL( const TDesC& aFile )
    {
    User::LeaveIfError( iFsSession.Connect() );
    User::LeaveIfError( 
        iFsSession.SetSessionToPrivate( KDrive ) );
    
    User::LeaveIfError( iFsSession.SessionPath( iPath ) );
    BaflUtils::EnsurePathExistsL( iFsSession, iPath );
    iPath.Append( aFile );
    
    // Read data from the file if the file exists.
    // Otherwise, let default values remain.
    ReadDataL(); 
    }


// -----------------------------------------------------------------------------
// CIAUpdateFirstTimeDateFile::~CIAUpdateFirstTimeDateFile
//
// -----------------------------------------------------------------------------
//
CIAUpdateFirstTimeDateFile::~CIAUpdateFirstTimeDateFile()
    {
    iFsSession.Close();
    }


// -----------------------------------------------------------------------------
// CIAUpdateFirstTimeDateFile::ReadDataL
//
// -----------------------------------------------------------------------------
//
TBool CIAUpdateFirstTimeDateFile::ReadDataL()
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
// CIAUpdateFirstTimeDateFile::WriteDataL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFirstTimeDateFile::WriteDataL()
	{
	TDriveUnit driveUnit( KDrive );
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
// CIAUpdateFirstTimeDateFile::FirstTime
//
// -----------------------------------------------------------------------------
//
const TTime& CIAUpdateFirstTimeDateFile::FirstTime() const
    {
    return iFirstTime;
    }
    

// -----------------------------------------------------------------------------
// CIAUpdateFirstTimeDateFile::SetCurrentFirstTime
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFirstTimeDateFile::SetCurrentFirstTime()
    {
    TTime universalTime;
    universalTime.UniversalTime();
    iFirstTime = universalTime;
    }


// -----------------------------------------------------------------------------
// CIAUpdateFirstTimeDateFile::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFirstTimeDateFile::InternalizeL( RReadStream& aStream )
	{
	// If you make changes here, 
	// remember to update ExternalizeL accordingly!!!

	TInt64 firstTime( 0 );
	aStream >> firstTime;
    iFirstTime = firstTime;
 	}


// -----------------------------------------------------------------------------
// CIAUpdateFirstTimeDateFile::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFirstTimeDateFile::ExternalizeL( RWriteStream& aStream )
	{
	// If you make changes here, 
	// remember to update InternalizeL accordingly!!!

	TInt64 firstTime( FirstTime().Int64() );
	aStream << firstTime;
	}
