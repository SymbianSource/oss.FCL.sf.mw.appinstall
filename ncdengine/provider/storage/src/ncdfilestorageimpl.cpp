/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include <f32file.h>

#include "ncdfilestorageimpl.h"
#include "ncdstorageowner.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdFileStorage::ConstructL( const TDesC& aUid, const TDesC& aPath )
    {
    DLTRACEIN( ("") );
    
    iRoot.CreateL( aPath );
    iUid.CreateL( aUid );
    
    DLTRACEOUT( ( _L("Root: %S"), &iRoot ) );
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdFileStorage* CNcdFileStorage::NewL( MNcdStorageOwner& aOwner, 
    const TDesC& aUid, const TDesC& aPath )
    {
    CNcdFileStorage* self = CNcdFileStorage::NewLC( aOwner, aUid, aPath );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CNcdFileStorage* CNcdFileStorage::NewLC( MNcdStorageOwner& aOwner, 
    const TDesC& aUid, const TDesC& aPath )
    {
    CNcdFileStorage* self = new( ELeave ) CNcdFileStorage( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL( aUid, aPath );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdFileStorage::~CNcdFileStorage()
    {
    DLTRACEIN( ( "" ) );
    iRoot.Close();
    iUid.Close();
    }
          

// ---------------------------------------------------------------------------
// AddFileL
// ---------------------------------------------------------------------------
//
void CNcdFileStorage::AddFileL( const TDesC& aFilepath, TBool aOverwrite )
    { 
    if ( !aFilepath.Length() ) 
        {
        User::Leave( KErrBadName );
        }
        
    DLTRACEIN( ( "" ) );
    TParse destPath;
    
    // Minimize stack usage
        {        
        TParsePtrC source( aFilepath );
        User::LeaveIfError( destPath.Set( source.NameAndExt(), NULL, &iRoot ) );
        }
    
    DLTRACE( ( _L("Target: %S"), &destPath.FullName() ) );
    
    
    TUint overwrite( 0 );
    if ( aOverwrite ) 
        {
        overwrite = CFileMan::EOverWrite;
        }
        
    User::LeaveIfError( iOwner.FileManager().Move( aFilepath, 
        destPath.FullName(), overwrite ) );
    DLTRACEOUT( ( "" ) );
    }

     
// ---------------------------------------------------------------------------
// RemoveFileL
// ---------------------------------------------------------------------------
//
void CNcdFileStorage::RemoveFileL( const TDesC& aFilename )
    {
    if ( !aFilename.Length() ) 
        {
        User::Leave( KErrBadName );
        }
        
    TParse path;
    User::LeaveIfError( path.Set( aFilename, NULL, &iRoot ) );
    //path.AddDir( iRoot );
    DLTRACE( ( _L("Trying to remove: %S"), &path.FullName() ) );
    User::LeaveIfError( iOwner.FileManager().Delete( path.FullName() ) );
    }


// ---------------------------------------------------------------------------
// OpenFileL
// ---------------------------------------------------------------------------
//
RFile CNcdFileStorage::OpenFileL( RFs& aFs, const TDesC& aFilename, 
    TUint aFileMode )
    {
    DLTRACEIN(( _L("File: %S"), &aFilename ));
    if ( !aFilename.Length() ) 
        {
        User::Leave( KErrBadName );
        }
        
    TParse path;
    User::LeaveIfError( path.Set( aFilename, NULL, &iRoot ) );
    //path.AddDir( iRoot );
    DLTRACE( ( _L("Filepath: %S"), &path.FullName() ) );
    
    // Open the file
    RFile file;
    User::LeaveIfError( file.Open( aFs, path.FullName(), 
        aFileMode ) );
    return file;
    }


// ---------------------------------------------------------------------------
// UID getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdFileStorage::Uid() const
    {
    return iUid;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdFileStorage::CNcdFileStorage( MNcdStorageOwner& aOwner ) : 
    iOwner( aOwner )
    {
    }
    
    
