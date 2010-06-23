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
* Description:   CIAUpdateParametersFileManager
*
*/




#include <bautils.h>
#include <s32file.h>
#include <sysutil.h>
#include <iaupdateparameters.h>

#include "iaupdateparametersfilemanager.h"
#include "iaupdatefileconsts.h"
#include "iaupdatetools.h"
#include "iaupdatedebug.h"







// This constant will be used to check if the IAD application
// is using the same version of the file. If the versions
// differ, then this may require additional checking in the future versions
// when data is internalized. For example, IAD may give old version data 
// when self update is started and after update new version is used.
const TInt KVersion( 1 );


// -----------------------------------------------------------------------------
// CIAUpdateParametersFileManager::NewL
//
// -----------------------------------------------------------------------------
//
CIAUpdateParametersFileManager* CIAUpdateParametersFileManager::NewL()
    {
    CIAUpdateParametersFileManager* self =
        CIAUpdateParametersFileManager::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateParametersFileManager::NewLC
//
// -----------------------------------------------------------------------------
//
CIAUpdateParametersFileManager* CIAUpdateParametersFileManager::NewLC()
    {
    CIAUpdateParametersFileManager* self =
        new( ELeave) CIAUpdateParametersFileManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdateParametersFileManager::CIAUpdateParametersFileManager
//
// -----------------------------------------------------------------------------
//
CIAUpdateParametersFileManager::CIAUpdateParametersFileManager()
: CBase()
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdateParametersFileManager::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateParametersFileManager::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateParametersFileManager::ConstructL start");
    
    User::LeaveIfError( iFsSession.Connect() );
    User::LeaveIfError( 
        iFsSession.SetSessionToPrivate( 
            IAUpdateFileConsts::KDrive ) );    
    User::LeaveIfError( iFsSession.SessionPath( iPath ) );
    BaflUtils::EnsurePathExistsL( iFsSession, iPath );
    iPath.Append( IAUpdateFileConsts::KParamsFile() );
    
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateParametersFileManager::ConstructL end: %S", &iPath);
    }


// -----------------------------------------------------------------------------
// CIAUpdateParametersFileManager::~CIAUpdateParametersFileManager
//
// -----------------------------------------------------------------------------
//
CIAUpdateParametersFileManager::~CIAUpdateParametersFileManager()
    {
    iFsSession.Close();
    }


// -----------------------------------------------------------------------------
// CIAUpdateParametersFileManager::ReadL
//
// -----------------------------------------------------------------------------
//
CIAUpdateParameters* CIAUpdateParametersFileManager::ReadL()
	{
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateParametersFileManager::ReadDataL path: %S", &iPath);

    CIAUpdateParameters* params( NULL );
    
	RFile file;
    TInt err( file.Open( iFsSession, iPath, EFileRead ) );
    if ( err == KErrNotFound )
        {
        IAUPDATE_TRACE("[IAUPDATE] Parameter file did not exist.");
        // File was not found.
        // So, return NULL.
        return params;
        }

    // If error occurred then leave.
    User::LeaveIfError( err );
    	
    CleanupClosePushL( file );
    
    RFileReadStream stream( file, 0 );
    CleanupClosePushL( stream );

    params = InternalizeL( stream );
    
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &file );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateParametersFileManager::ReadDataL end");
    
    return params;
	}


// -----------------------------------------------------------------------------
// CIAUpdateParametersFileManager::WriteL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateParametersFileManager::WriteL( const CIAUpdateParameters& aParams )
	{
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateParametersFileManager::WriteDataL path: %S", &iPath);
    TInt sizeOfFile = sizeof( TInt ) //length integer in stream
                    + sizeof( TInt ) //KVersion
                    + sizeof( TInt ) * 5  //integers in externalised params
                    + aParams.SearchCriteria().Size()
                    + aParams.CommandLineExecutable().Size()
                    + aParams.CommandLineArguments().Size(); 
        
	if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFsSession, 
	                                            sizeOfFile, 
	                                            IAUpdateFileConsts::KDrive ) )
	    {
		User::Leave( KErrDiskFull );
	    }
    
	RFile file;
    User::LeaveIfError( file.Replace( iFsSession, iPath, EFileWrite ) );
    CleanupClosePushL( file );
    
    RFileWriteStream stream( file, 0 );
    CleanupClosePushL( stream );

    ExternalizeL( stream, aParams );
    stream.CommitL();
    
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &file );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateParametersFileManager::WriteDataL end");
	}


// -----------------------------------------------------------------------------
// CIAUpdateParametersFileManager::RemoveFile
//
// -----------------------------------------------------------------------------
//
TInt CIAUpdateParametersFileManager::RemoveFile()
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateParametersFileManager::RemoveFile path: %S", &iPath);
    
    // May either be a full path, or relative to the session path.
    // Even if session path has already been set in the ConstructL,
    // we do not use the relative path and only the hard coded filename here. 
    // A new path may have been given by calling SetFilePathL. So, use that
    // value here.
    TInt errorCode( BaflUtils::DeleteFile( iFsSession, iPath ) );

    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateParametersFileManager::RemoveFile end: %d", errorCode);
    
    return errorCode;
    }


// -----------------------------------------------------------------------------
// CIAUpdateParametersFileManager::InternalizeL
//
// -----------------------------------------------------------------------------
//
CIAUpdateParameters* CIAUpdateParametersFileManager::InternalizeL( RReadStream& aStream )
	{
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateParametersFileManager::InternalizeL start");
    
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

    CIAUpdateParameters* params( NULL );

    HBufC8* data( NULL );
    IAUpdateTools::InternalizeDes8L( data, aStream );
    if ( data )
        {
        IAUPDATE_TRACE("[IAUPDATE] Create parameters object");
        params = CIAUpdateParameters::NewLC();
        IAUpdateTools::InternalizeParametersL( *params, *data );
        delete data;
        data = NULL;
        CleanupStack::Pop( params );        
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateParametersFileManager::InternalizeL end");

    return params;
	}


// -----------------------------------------------------------------------------
// CIAUpdateParametersFileManager::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateParametersFileManager::ExternalizeL( RWriteStream& aStream,
                                                   const CIAUpdateParameters& aParams )
	{
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateParametersFileManager::ExternalizeL start");
    
	// If you make changes here, 
	// remember to update InternalizeL accordingly!!!

    aStream.WriteInt32L( KVersion );

    // Write parameter data into the stream.
    HBufC8* data( NULL );
    IAUpdateTools::ExternalizeParametersL( data, aParams );
    if ( data )
        {
        IAUPDATE_TRACE("[IAUPDATE] Externalize parameter data into the stream.");
        IAUpdateTools::ExternalizeDes8L( *data, aStream );
        delete data;
        data = NULL;
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateParametersFileManager::ExternalizeL start");
	}
