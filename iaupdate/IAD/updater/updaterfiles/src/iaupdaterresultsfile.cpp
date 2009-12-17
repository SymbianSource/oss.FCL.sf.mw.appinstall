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
* Description:   CIAUpdaterResultsFile
*
*/




#include <bautils.h>
#include <s32file.h>
#include <sysutil.h>

#include "iaupdaterresultsfile.h"
#include "iaupdateridentifier.h"
#include "iaupdaterfileconsts.h"
#include "iaupdaterresult.h"
#include "iaupdatedebug.h"


// This constant will be used to check if the IAD application
// and iaupdater are using the same version of the file. If the versions
// differ, then this may require additional checking in the future versions
// when data is internalized. For example, IAD may give old version data 
// for iaupdater during self update just after iaupdater has been updated
// and IAD is itself updated after that.
const TInt KVersion( 1 );


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::NewL
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterResultsFile* CIAUpdaterResultsFile::NewL()
    {
    CIAUpdaterResultsFile* self =
        CIAUpdaterResultsFile::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::NewLC
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterResultsFile* CIAUpdaterResultsFile::NewLC()
    {
    CIAUpdaterResultsFile* self =
        new( ELeave) CIAUpdaterResultsFile();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::CIAUpdaterResultsFile
//
// -----------------------------------------------------------------------------
//
CIAUpdaterResultsFile::CIAUpdaterResultsFile()
: CBase()
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterResultsFile::ConstructL()
    {
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterResultsFile::ConstructL start");
    
    User::LeaveIfError( iFsSession.Connect() );
    User::LeaveIfError( 
        iFsSession.SetSessionToPrivate( 
            IAUpdaterFileConsts::KDrive ) );    
    User::LeaveIfError( iFsSession.SessionPath( iPath ) );
    BaflUtils::EnsurePathExistsL( iFsSession, iPath );
    iPath.Append( IAUpdaterFileConsts::KResultsFile() );
    
    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterResultsFile::ConstructL end: %S", &iPath);
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::~CIAUpdaterResultsFile
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterResultsFile::~CIAUpdaterResultsFile()
    {
    iFsSession.Close();
    
    iResults.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::SuccessCount
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIAUpdaterResultsFile::SuccessCount() const
    {
    TInt count( 0 );

    for ( TInt i = 0; i < iResults.Count(); ++i )
        {
        CIAUpdaterResult* result( iResults[ i ] );
        if ( result->ErrorCode() == KErrNone
             && !result->Hidden() )
            {
            ++count;
            }
        }
        
    return count;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::FailCount
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIAUpdaterResultsFile::FailCount() const
    {
    TInt count( 0 );

    for ( TInt i = 0; i < iResults.Count(); ++i )
        {
        CIAUpdaterResult* result( iResults[ i ] );
        if ( result->ErrorCode() != KErrNone
             && result->ErrorCode() != KErrCancel
             && !result->Hidden() )
            {
            ++count;
            }
        }
        
    return count;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::CancelCount
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIAUpdaterResultsFile::CancelCount() const
    {
    TInt count( 0 );

    for ( TInt i = 0; i < iResults.Count(); ++i )
        {
        CIAUpdaterResult* result( iResults[ i ] );
        if ( result->ErrorCode() == KErrCancel
             && !result->Hidden() )
            {
            ++count;
            }
        }
        
    return count;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::Reset
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterResultsFile::Reset()
    {
    iPckgStartIndex = 0;
    iTotalPckgCount = 0;
    iResults.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::PckgStartIndex
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIAUpdaterResultsFile::PckgStartIndex() const
    {
    return iPckgStartIndex;
    }

// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::SetPckgStartIndex
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterResultsFile::SetPckgStartIndex( const TInt& aIndex )
    {
    iPckgStartIndex = aIndex;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::TotalPckgCount
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIAUpdaterResultsFile::TotalPckgCount() const
    {
    return iTotalPckgCount;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::SetTotalPckgCount
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterResultsFile::SetTotalPckgCount( const TInt& aCount )
    {
    iTotalPckgCount = aCount;
    }
        

// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::Results
//
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray< CIAUpdaterResult >& CIAUpdaterResultsFile::Results()
    {
    return iResults;
    }
    

// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::FilePath
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIAUpdaterResultsFile::FilePath() const 
    {
    return iPath;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::SetFilePathL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterResultsFile::SetFilePathL( const TDesC& aPath )
    {
    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterResultsFile::SetFilePathL old path: %S", &iPath);

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

    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterResultsFile::SetFilePathL new path: %S", &iPath);
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::ReadDataL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIAUpdaterResultsFile::ReadDataL()
	{
    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterResultsFile::ReadDataL path: %S", &iPath);
    
	RFile file;
    TInt err( file.Open( iFsSession, iPath, EFileRead ) );
    User::LeaveIfError( err );
    	
    CleanupClosePushL( file );
    
    RFileReadStream stream( file, 0 );
    CleanupClosePushL( stream );

    InternalizeL( stream );

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &file );

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterResultsFile::ReadDataL end");
    
    return ETrue;
	}


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::WriteDataL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterResultsFile::WriteDataL()
	{
    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterResultsFile::WriteDataL path: %S", &iPath);
    
    TInt sizeOfFile = sizeof( TInt ) //KVersion
                    + sizeof( TInt ) //iPckgStartIndex
                    + sizeof( TInt ) //iTotalPckgCount
                    + sizeof( TInt ); //iResults.Count()
                    
    
    TInt count( iResults.Count() );
    for( TInt i = 0; i < count; ++i )
	    {
	    sizeOfFile += sizeof( TInt ) * 3; //version, error code, hidden flag
	    CIAUpdaterResult* result( iResults[ i ] );
	    sizeOfFile += sizeof( TInt ) * 3; //version, length of Id, length of namespace, UId
	    sizeOfFile += result->Identifier().Id().Size(); //size of Id
	    sizeOfFile += result->Identifier().Namespace().Size(); //size of namespace
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

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterResultsFile::WriteDataL end");
	}


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::RemoveFile
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIAUpdaterResultsFile::RemoveFile()
    {
    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterResultsFile::RemoveFile path: %S", &iPath);
    
    // May either be a full path, or relative to the session path.
    // Even if session path has already been set in the ConstructL,
    // we do not use the relative path and only the hard coded filename here. 
    // A new path may have been given by calling SetFilePathL. So, use that
    // value here.
    TInt errorCode( BaflUtils::DeleteFile( iFsSession, iPath ) );

    IAUPDATE_TRACE_1("[IAUpdater] CIAUpdaterResultsFile::RemoveFile end: %d", errorCode);
    
    return errorCode;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterResultsFile::InternalizeL( RReadStream& aStream )
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

    iPckgStartIndex = aStream.ReadInt32L();
    iTotalPckgCount = aStream.ReadInt32L();

    iResults.ResetAndDestroy();    
	TInt count( aStream.ReadInt32L() );
	for( TInt i = 0; i < count; ++i )
	    {
	    CIAUpdaterResult* result( 
	        CIAUpdaterResult::NewLC() );
	    result->InternalizeL( aStream );
        iResults.AppendL( result );        
	    CleanupStack::Pop( result );
	    }
	}


// -----------------------------------------------------------------------------
// CIAUpdaterResultsFile::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterResultsFile::ExternalizeL( RWriteStream& aStream )
	{
	// If you make changes here, 
	// remember to update InternalizeL accordingly!!!

    aStream.WriteInt32L( KVersion );

    aStream.WriteInt32L( iPckgStartIndex );
    aStream.WriteInt32L( iTotalPckgCount );

    TInt count( iResults.Count() );
	aStream.WriteInt32L( count );
	
	for( TInt i = 0; i < count; ++i )
	    {
	    CIAUpdaterResult* result( iResults[ i ] );
	    result->ExternalizeL( aStream );
	    }
	}
