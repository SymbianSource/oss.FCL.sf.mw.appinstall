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
#include <bautils.h>

#include "ncdstorageimpl.h"
#include "ncddbstoragemanager.h"
#include "ncdfilestorageimpl.h"
#include "catalogsdebug.h"
#include "catalogsutils.h"
#include "catalogsconstants.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdStorage::ConstructL( const TDesC& aNamespace )
    {
    DLTRACEIN( ("") );
    iNamespace.CreateL( aNamespace ); 
    iDirectory.Assign( EncodeFilenameLC( aNamespace, iOwner.FileSession() ) );
    CleanupStack::Pop(); // iDirectory

    HBufC* path = CurrentPathLC();
    
    DLTRACE(( _L("Creating the directory: %S"), path));
    // Create the directory
    BaflUtils::EnsurePathExistsL( iOwner.FileSession(), 
        *path );    
    
    CleanupStack::PopAndDestroy( path );
    DLTRACEOUT( ("") );   
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdStorage::ConstructL( HBufC* aNamespace, HBufC* aDirectory )
    {
    DLTRACEIN( ("") );
    iNamespace.Assign( aNamespace ); 
    iDirectory.Assign( aDirectory );    

    HBufC* path = CurrentPathLC();
    
    DLTRACE(( _L("Creating the directory: %S"), path));
    // Create the directory
    BaflUtils::EnsurePathExistsL( iOwner.FileSession(), 
        *path );    
    
    CleanupStack::PopAndDestroy( path );
    DLTRACEOUT( ("") );   
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdStorage* CNcdStorage::NewL( MNcdStorageOwner& aOwner, 
    const TDesC& aNamespace )
    {
    CNcdStorage* self = CNcdStorage::NewLC( aOwner, aNamespace );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CNcdStorage* CNcdStorage::NewLC( MNcdStorageOwner& aOwner, 
    const TDesC& aNamespace )
    {
    CNcdStorage* self = new( ELeave ) CNcdStorage( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL( aNamespace );
    return self;
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdStorage* CNcdStorage::NewL( MNcdStorageOwner& aOwner, 
    HBufC* aNamespace, HBufC* aDirectory )
    {
    CNcdStorage* self = new( ELeave ) CNcdStorage( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL( aNamespace, aDirectory );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdStorage::~CNcdStorage()
    {
    DLTRACEIN((""));
    iNamespace.Close();        
    iDirectory.Close();
    iDbStorages.ResetAndDestroy();    
    iFileStorages.ResetAndDestroy();
    }
       

// ---------------------------------------------------------------------------
// Gets an existing database storage or creates a new one
// ---------------------------------------------------------------------------
//
MNcdDatabaseStorage& CNcdStorage::DatabaseStorageL( const TDesC& aUid )
    {
    DLTRACEIN( ( _L("Uid: %S"), &aUid ) );
    TInt index = FindDbStorageByUid( aUid );
    
    CNcdDbStorageManager* db = NULL;
    if ( index == KErrNotFound ) 
        {
        RBuf path;
        CleanupClosePushL( path );
        path.CreateL( KMaxPath );

        
        DLTRACE( ("DB not found, creating new object" ) );

        // Creates the path
        CreateCurrentPathL( path, aUid );      
            
        DLTRACE(( _L("Checking if the directory exists: %S"),
            &path ));
            
        db = CNcdDbStorageManager::NewLC( FileSession(), aUid, path, 
            iDirectory );
        
        
        iDbStorages.AppendL( db );
        CleanupStack::Pop( db );
        CleanupStack::PopAndDestroy( &path );
        }
    else 
        {
        db = iDbStorages[index];
        }
        
    DASSERT( db );
    DLTRACEOUT( ( "db: %x", db ) );
    return *db;
    }


// ---------------------------------------------------------------------------
// Gets an existing file storage or creates a new one
// ---------------------------------------------------------------------------
//
MNcdFileStorage& CNcdStorage::FileStorageL( const TDesC& aUid )
    {
    DLTRACEIN( ( _L("Uid: %S"), &aUid ) );
    TInt index = FindFileStorageByUid( aUid );
    
    CNcdFileStorage* fs = NULL;
    if ( index == KErrNotFound ) 
        {
        RBuf path;
        CleanupClosePushL( path );
        path.CreateL( KMaxPath );
        
        CreateCurrentPathL( path, aUid );
                    
        // Note: giving the owner of this object instead of giving *this
        fs = CNcdFileStorage::NewLC( iOwner, aUid, path );
        iFileStorages.AppendL( fs );
        CleanupStack::Pop( fs );        
        CleanupStack::PopAndDestroy( &path );
        }
    else 
        {
        fs = iFileStorages[index];
        }
        
    DASSERT( fs );
    DLTRACEOUT( ( _L("fs: %x"), fs ) );
    return *fs;    
    }


// ---------------------------------------------------------------------------
// Namespace getter 
// ---------------------------------------------------------------------------
//
const TDesC& CNcdStorage::Namespace() const
    {
    return iNamespace;
    }


// ---------------------------------------------------------------------------
// Directory getter 
// ---------------------------------------------------------------------------
//
const TDesC& CNcdStorage::Directory() const
    {
    return iDirectory;
    }


// ---------------------------------------------------------------------------
// File session getter
// ---------------------------------------------------------------------------
//    
RFs& CNcdStorage::FileSession()
    {
    return iOwner.FileSession();
    }


// ---------------------------------------------------------------------------
// Size getter
// ---------------------------------------------------------------------------
//    
TInt CNcdStorage::SizeL()
    {
    DLTRACEIN((""));
    HBufC* path = CurrentPathLC();
    TInt size = FileSystemAllocationL( FileSession(), *path );
    DLTRACEOUT(( _L("Size for path %S is %d bytes"), path, size ));
    CleanupStack::PopAndDestroy( path );
    return size;
    }
    

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//    
CNcdStorage::CNcdStorage( MNcdStorageOwner& aOwner ) : iOwner( aOwner )
    {
    }

// ---------------------------------------------------------------------------
// Searches for a database by name
// ---------------------------------------------------------------------------
//
TInt CNcdStorage::FindDbStorageByUid( const TDesC& aUid )
    {
    for ( TInt i = 0; i < iDbStorages.Count(); ++i )
        {
        if ( iDbStorages[i]->Uid().Compare( aUid ) == 0 ) 
            {
            return i;
            }
        }
    return KErrNotFound;
    }
    
    
// ---------------------------------------------------------------------------
// Searches for a file storage by name
// ---------------------------------------------------------------------------
//
TInt CNcdStorage::FindFileStorageByUid( const TDesC& aUid )
    {
    for ( TInt i = 0; i < iFileStorages.Count(); ++i )
        {
        if ( iFileStorages[i]->Uid().Compare( aUid ) == 0 ) 
            {
            return i;
            }
        }
    return KErrNotFound;
    }
    
    
// ---------------------------------------------------------------------------
// Generates the current path
// ---------------------------------------------------------------------------
//
void CNcdStorage::CreateCurrentPathL( TDes& aPath, const TDesC& aUid ) const
    {
    iOwner.AppendRoot( aPath );
       
    DLTRACE(( _L("Adding namespace %S"), &iNamespace ));
    aPath.Append( iDirectory );
    aPath.Append( KDirectorySeparator );    
    DLTRACE(( _L("Adding uid %S"), &aUid ));
    aPath.Append( aUid );                
    aPath.Append( KDirectorySeparator );    
    
    // Create the directory
    BaflUtils::EnsurePathExistsL( iOwner.FileSession(), aPath );    
    }


// ---------------------------------------------------------------------------
// Storage path getter
// ---------------------------------------------------------------------------
//
HBufC* CNcdStorage::CurrentPathLC() const
    {
    HBufC* path = HBufC::NewLC( KMaxPath );
    TPtr ptr( path->Des() );
    iOwner.AppendRoot( ptr );
    ptr.Append( iDirectory );
    ptr.Append( KDirectorySeparator );    
    return path;
    }
