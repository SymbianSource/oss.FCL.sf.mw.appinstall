/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdaterFileListFile
*
*/




#include <bautils.h>
#include <s32file.h>
#include <sysutil.h>

#include "iaupdaterfilelistfile.h"
#include "iaupdaterfilelist.h"
#include "iaupdaterfileinfo.h"
#include "iaupdaterfileconsts.h"
#include "iaupdatedebug.h"


// This constant will be used to check if the IAD application
// and iaupdater are using the same version of the file. If the versions
// differ, then this may require additional checking in the future versions
// when data is internalized. For example, IAD may give old version data 
// for iaupdater during self update just after iaupdater has been updated
// and IAD is itself updated after that.
const TInt KVersion( 1 );


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::NewL
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterFileListFile* CIAUpdaterFileListFile::NewL()
    {
    CIAUpdaterFileListFile* self =
        CIAUpdaterFileListFile::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::NewLC
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterFileListFile* CIAUpdaterFileListFile::NewLC()
    {
    CIAUpdaterFileListFile* self =
        new( ELeave) CIAUpdaterFileListFile();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::CIAUpdaterFileListFile
//
// -----------------------------------------------------------------------------
//
CIAUpdaterFileListFile::CIAUpdaterFileListFile()
: CBase()
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterFileListFile::ConstructL()
    {
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterFileListFile::ConstructL start");
    
    User::LeaveIfError( iFsSession.Connect() );
    User::LeaveIfError( 
        iFsSession.SetSessionToPrivate( 
            IAUpdaterFileConsts::KDrive ) );    
    User::LeaveIfError( iFsSession.SessionPath( iPath ) );
    BaflUtils::EnsurePathExistsL( iFsSession, iPath );
    iPath.Append( IAUpdaterFileConsts::KFileListFile );
    
    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterFileListFile::ConstructL end: %S", &iPath);
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::~CIAUpdaterFileListFile
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterFileListFile::~CIAUpdaterFileListFile()
    {
    iFsSession.Close();

    Reset();
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::Reset
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterFileListFile::Reset()
    {
    iSilent = EFalse;
    iPckgStartIndex = 0;
    iTotalPckgCount = 0;
    
    iFileList.ResetAndDestroy();    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::Silent
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIAUpdaterFileListFile::Silent() const
    {
    return iSilent;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::SetSilent
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterFileListFile::SetSilent( TBool aSilent )
    {
    iSilent = aSilent;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::PckgStartIndex
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIAUpdaterFileListFile::PckgStartIndex() const
    {
    return iPckgStartIndex;
    }

// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::SetPckgStartIndex
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterFileListFile::SetPckgStartIndex( const TInt aIndex )
    {
    iPckgStartIndex = aIndex;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::TotalPckgCount
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIAUpdaterFileListFile::TotalPckgCount() const
    {
    return iTotalPckgCount;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::SetTotalPckgCount
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterFileListFile::SetTotalPckgCount( const TInt aCount )
    {
    iTotalPckgCount = aCount;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::FileList
//
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray< CIAUpdaterFileList >& CIAUpdaterFileListFile::FileList()
    {
    return iFileList;    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::FilePath
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC&CIAUpdaterFileListFile::FilePath() const 
    {
    return iPath;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::SetFilePathL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterFileListFile::SetFilePathL( const TDesC& aPath )
    {
    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterFileListFile::SetFilePathL old path: %S", &iPath);

    TParsePtrC path( aPath );

    if ( !path.NamePresent() )
        {
        IAUPDATE_TRACE("[IAUpdater] No file name");
        // Do not accept path that does not contain file name.
        User::Leave( KErrArgument );
        }

    if ( path.PathPresent() || path.DrivePresent() )
        {
        IAUPDATE_TRACE("[IAUpdater] Path present");
        if ( path.DrivePresent() )
            {
            IAUPDATE_TRACE("[IAUpdater] Drive present");
            User::LeaveIfError( iFsSession.SetSessionPath( path.DriveAndPath() ) );
            // Because the path and the drive were given, the given parameter path
            // can be used as it was given.
            iPath.Copy( aPath );
            }
        else
            {
            IAUPDATE_TRACE("[IAUpdater] Only path present");
            User::LeaveIfError( iFsSession.SetSessionPath( path.Path() ) );
            // Because the given path was missing the drive letter,
            // get the correct path with some default drive letter from the
            // session.
            User::LeaveIfError( iFsSession.SessionPath( iPath ) );
            // Append the file name to the end of the path.
            iPath.Append( path.NameAndExt() );
            }
        }
    else
        {
        IAUPDATE_TRACE("[IAUpdater] Path was not present");
        // Only the file name was given. So, use the session path to get the whole
        // path of the file.
        User::LeaveIfError( iFsSession.SessionPath( iPath ) );
        iPath.Append( aPath );        
        }

    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterFileListFile::SetFilePathL new path: %S", &iPath);
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::ReadDataL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIAUpdaterFileListFile::ReadDataL()
	{
    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterFileListFile::ReadDataL path: %S", &iPath);
    
	RFile file;
    TInt err( file.Open( iFsSession, iPath, EFileRead ) );
    User::LeaveIfError( err );
    	    	
    CleanupClosePushL( file );
    
    RFileReadStream stream( file, 0 );
    CleanupClosePushL( stream );

    InternalizeL( stream );

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &file );

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterFileListFile::ReadDataL end");
    
    return ETrue;
	}


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::WriteDataL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterFileListFile::WriteDataL()
	{
    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterFileListFile::WriteDataL path: %S", &iPath);
    
    TInt sizeOfFile = 5 * sizeof( TInt ); //KVersion, iSilent, iPckgStartIndex, iTotalPckgCount, iFileList.Count()
    
    TInt count( iFileList.Count() );
	for( TInt i = 0; i < count; ++i )
	    {
	    CIAUpdaterFileList* file( iFileList[ i ] );
        sizeOfFile += sizeof( TInt ) // version
                   + file->BundleName().Size() //Bundle name
                   + sizeof( TInt ) // Bundle name length
                   + sizeof( TInt ) //hidden flag
                   + sizeof( TInt ); //file infos count
        TInt fileInfosCount =  file->FileInfos().Count();          
        for( TInt j = 0; j < fileInfosCount; ++j )
	        {
	        CIAUpdaterFileInfo* info( file->FileInfos()[ j ] );
	        sizeOfFile += sizeof( TInt ) // version
	                   + info->FilePath().Size()
	                   + sizeof( TInt ); // File path length
	        }           
	    }
     
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFsSession, 
	                                            sizeOfFile, 
	                                            IAUpdaterFileConsts::KDrive ) )
	    {
		User::Leave( KErrDiskFull );
	    }
	RFile file;
    User::LeaveIfError( file.Replace( iFsSession, iPath, EFileWrite ) );
    CleanupClosePushL( file );
    
    RFileWriteStream stream( file, 0 );
    CleanupClosePushL( stream );

    ExternalizeL( stream );
    stream.CommitL();
    
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &file );

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterFileListFile::WriteDataL end");
	}


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::RemoveFile
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIAUpdaterFileListFile::RemoveFile()
    {
    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterFileListFile::RemoveFile path: %S", &iPath);
    
    // May either be a full path, or relative to the session path.
    // Even if session path has already been set in the ConstructL,
    // we do not use the relative path and only the hard coded filename here. 
    // A new path may have been given by calling SetFilePathL. So, use that
    // value here.
    TInt errorCode( BaflUtils::DeleteFile( iFsSession, iPath ) );

    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterFileListFile::RemoveFile end: %d", errorCode);
    
    return errorCode;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterFileListFile::InternalizeL( RReadStream& aStream )
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

    iSilent = aStream.ReadInt32L();
    iPckgStartIndex = aStream.ReadInt32L();
    iTotalPckgCount = aStream.ReadInt32L();
	
    iFileList.ResetAndDestroy();
	TInt count( aStream.ReadInt32L() );
	for( TInt i = 0; i < count; ++i )
	    {
	    CIAUpdaterFileList* file( 
	        CIAUpdaterFileList::NewLC() );
	    file->InternalizeL( aStream );
        iFileList.AppendL( file );        
	    CleanupStack::Pop( file );
	    }	
	}


// -----------------------------------------------------------------------------
// CIAUpdaterFileListFile::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterFileListFile::ExternalizeL( RWriteStream& aStream )
	{
	// If you make changes here, 
	// remember to update InternalizeL accordingly!!!

    aStream.WriteInt32L( KVersion );

    aStream.WriteInt32L( iSilent );
    aStream.WriteInt32L( iPckgStartIndex );
    aStream.WriteInt32L( iTotalPckgCount );

    TInt count( iFileList.Count() );
	aStream.WriteInt32L( count );
	
	for( TInt i = 0; i < count; ++i )
	    {
	    CIAUpdaterFileList* file( iFileList[ i ] );
	    file->ExternalizeL( aStream );
	    }	
	}
