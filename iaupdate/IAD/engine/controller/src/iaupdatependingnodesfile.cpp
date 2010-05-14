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
* Description:   CIAUpdatePendingNodesFile
*
*/




#include <bautils.h>
#include <s32file.h>
#include <sysutil.h>

#include "iaupdatependingnodesfile.h"
#include "iaupdatectrlfileconsts.h"
#include "iaupdateridentifier.h"
#include "iaupdatedebug.h"


// This constant will be used to check if the IAD application
// and its updates are using the same version of the file. If the versions
// differ, then this may require additional checking in the future versions
// when data is internalized. For example, previous IAD may give old version data 
// into the file during self update.
const TInt KVersion( 1 );


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::NewL
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdatePendingNodesFile* CIAUpdatePendingNodesFile::NewL()
    {
    CIAUpdatePendingNodesFile* self =
        CIAUpdatePendingNodesFile::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::NewLC
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdatePendingNodesFile* CIAUpdatePendingNodesFile::NewLC()
    {
    CIAUpdatePendingNodesFile* self =
        new( ELeave) CIAUpdatePendingNodesFile();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::CIAUpdatePendingNodesFile
//
// -----------------------------------------------------------------------------
//
CIAUpdatePendingNodesFile::CIAUpdatePendingNodesFile()
: CBase()
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdatePendingNodesFile::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePendingNodesFile::ConstructL start");
    
    User::LeaveIfError( iFsSession.Connect() );
    User::LeaveIfError( 
        iFsSession.SetSessionToPrivate( 
            IAUpdateCtrlFileConsts::KDrive ) );    
    User::LeaveIfError( iFsSession.SessionPath( iPath ) );
    BaflUtils::EnsurePathExistsL( iFsSession, iPath );
    iPath.Append( IAUpdateCtrlFileConsts::KPendingNodesFile() );
    
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdatePendingNodesFile::ConstructL end: %S", &iPath);
    }


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::~CIAUpdatePendingNodesFile
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdatePendingNodesFile::~CIAUpdatePendingNodesFile()
    {
    iFsSession.Close();
    
    Reset();
    }


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::Reset
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdatePendingNodesFile::Reset()
    {
    iIndex = 0;
    iPendingNodes.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::Index
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIAUpdatePendingNodesFile::Index() const
    {
    return iIndex;
    }           


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::SetIndex
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdatePendingNodesFile::SetIndex( TInt aIndex )
    {
    iIndex = aIndex;
    }


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::Results
//
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray< CIAUpdaterIdentifier >& CIAUpdatePendingNodesFile::PendingNodes()
    {
    return iPendingNodes;
    }
    

// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::FilePath
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIAUpdatePendingNodesFile::FilePath() const 
    {
    return iPath;
    }


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::SetFilePathL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdatePendingNodesFile::SetFilePathL( const TDesC& aPath )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdatePendingNodesFile::SetFilePathL old path: %S", &iPath);

    TParsePtrC path( aPath );

    if ( !path.NamePresent() )
        {
        IAUPDATE_TRACE("[IAUPDATE] No file name");
        // Do not accept path that does not contain file name.
        User::Leave( KErrArgument );
        }

    if ( path.PathPresent() || path.DrivePresent() )
        {
        IAUPDATE_TRACE("[IAUPDATE] Path present");
        if ( path.DrivePresent() )
            {
            IAUPDATE_TRACE("[IAUPDATE] Drive present");
            User::LeaveIfError( iFsSession.SetSessionPath( path.DriveAndPath() ) );
            // Because the path and the drive were given, the given parameter path
            // can be used as it was given.
            iPath.Copy( aPath );
            }
        else
            {
            IAUPDATE_TRACE("[IAUPDATE] Only path present");
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
        IAUPDATE_TRACE("[IAUPDATE] Path was not present");
        // Only the file name was given. So, use the session path to get the whole
        // path of the file.
        User::LeaveIfError( iFsSession.SessionPath( iPath ) );
        iPath.Append( aPath );        
        }

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdatePendingNodesFile::SetFilePathL new path: %S", &iPath);
    }


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::ReadDataL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIAUpdatePendingNodesFile::ReadDataL()
	{
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePendingNodesFile::ReadDataL begin");
    
	RFile file;
    TInt err( file.Open( iFsSession, iPath, EFileRead ) );
    User::LeaveIfError( err );
    	
    CleanupClosePushL( file );
    
    RFileReadStream stream( file, 0 );
    CleanupClosePushL( stream );

    InternalizeL( stream );

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &file );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePendingNodesFile::ReadDataL end");
    
    return ETrue;
	}


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::WriteDataL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdatePendingNodesFile::WriteDataL()
	{
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdatePendingNodesFile::ReadDataL path: %S", &iPath);
    TInt sizeOfFile = 2 * sizeof( TInt ); //version, count of identifiers
    
    TInt count( iPendingNodes.Count() );
	for( TInt i = 0; i < count; ++i )
	    {
	    CIAUpdaterIdentifier* identifier( iPendingNodes[ i ] );
	    sizeOfFile += sizeof( TInt ) // version 
	                + identifier->Id().Size() //Id
	                + sizeof( TInt ) //Id length
	                + identifier->Namespace().Size() //namespace
	                + sizeof( TInt ); //namespace length
	    }
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( 
            &iFsSession, 
	        sizeOfFile, 
	        IAUpdateCtrlFileConsts::KDrive ) )
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


    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePendingNodesFile::WriteDataL end");
	}


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::RemoveFile
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIAUpdatePendingNodesFile::RemoveFile()
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdatePendingNodesfile::RemoveFile path: %S", &iPath);
    
    // May either be a full path, or relative to the session path.
    // Even if session path has already been set in the ConstructL,
    // we do not use the relative path and only the hard coded filename here. 
    // A new path may have been given by calling SetFilePathL. So, use that
    // value here.
    TInt errorCode( BaflUtils::DeleteFile( iFsSession, iPath ) );

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdatePendingNodesfile::RemoveFile end: %d", errorCode);
    
    return errorCode;
    }


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdatePendingNodesFile::InternalizeL( RReadStream& aStream )
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

    iPendingNodes.ResetAndDestroy();
	TInt count( aStream.ReadInt32L() );
	for( TInt i = 0; i < count; ++i )
	    {
	    CIAUpdaterIdentifier* identifier( CIAUpdaterIdentifier::NewLC() );
	    identifier->InternalizeL( aStream );
        iPendingNodes.AppendL( identifier );
        CleanupStack::Pop( identifier ); 
	    }

    iIndex = aStream.ReadInt32L();
	}


// -----------------------------------------------------------------------------
// CIAUpdatePendingNodesFile::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdatePendingNodesFile::ExternalizeL( RWriteStream& aStream )
	{
	// If you make changes here, 
	// remember to update InternalizeL accordingly!!!

    aStream.WriteInt32L( KVersion );

    TInt count( iPendingNodes.Count() );
	aStream.WriteInt32L( count );	
	for( TInt i = 0; i < count; ++i )
	    {
	    CIAUpdaterIdentifier* identifier( iPendingNodes[ i ] );
	    identifier->ExternalizeL( aStream );
	    }

    aStream.WriteInt32L( iIndex );
	}
