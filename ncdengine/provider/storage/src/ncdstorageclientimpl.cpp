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


#include <bamdesca.h>

#include "ncdstorageclientimpl.h"

#include <f32file.h>
#include <bautils.h>
#include <badesca.h>
#include <s32file.h>

#include "ncdstorageimpl.h"
#include "ncdstoragemanagerimpl.h"
#include "catalogsconstants.h"
#include "catalogsutils.h"

#include "catalogsdebug.h"


_LIT( KNamespaceFile, "namespaces" );

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdStorageClient::ConstructL( const TDesC& aUid )
    {
    DLTRACEIN( ("") );
    // Copy the uid
    iUid.CreateL( aUid );
    
    RBuf path;
    CleanupClosePushL( path );
    path.CreateL( KMaxPath );
    
    // Create the path string for the storage    
    AppendRoot( path );
    
    // Create the directory
    BaflUtils::EnsurePathExistsL( iOwner.FileSession(), path );  
    
    // Create storages for each found directory
    //CreateStoragesFromFileSystemL( path );      
    CleanupStack::PopAndDestroy( &path );
    
    // Read namespaces and create storages
    TRAPD( err, ReadNamespacesL() );
    if ( err != KErrNone && err != KErrNotFound ) 
        {
        User::Leave( err );
        }
    DLTRACEOUT( ("") );
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdStorageClient* CNcdStorageClient::NewL( MNcdStorageOwner& aOwner,
    const TDesC& aUid )
    {
    CNcdStorageClient* self = CNcdStorageClient::NewLC( aOwner, aUid );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CNcdStorageClient* CNcdStorageClient::NewLC( MNcdStorageOwner& aOwner,
    const TDesC& aUid )
    {
    CNcdStorageClient* self = new( ELeave ) CNcdStorageClient( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL( aUid );    
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdStorageClient::~CNcdStorageClient()
    {
    iUid.Close();
    iStorages.ResetAndDestroy();
    }


// ---------------------------------------------------------------------------
// Storage creator
// ---------------------------------------------------------------------------
//
MNcdStorage& CNcdStorageClient::CreateStorageL( const TDesC& aNamespace )
    {
    DLTRACEIN( ("") );
    
    TInt index = FindStorageByNamespace( aNamespace );
    
    if ( index != KErrNotFound ) 
        {
        return *( iStorages[ index ] );
        }
            
    CNcdStorage* storage = CNcdStorage::NewLC( *this, aNamespace );
    iStorages.AppendL( storage );
    CleanupStack::Pop( storage );
    
    // Save namespaces
    SaveNamespacesL();
    return *static_cast<MNcdStorage*>( storage );
    }
    
    
// ---------------------------------------------------------------------------
// Storage getter
// ---------------------------------------------------------------------------
//
MNcdStorage& CNcdStorageClient::StorageL( const TDesC& aNamespace )
    {
    DLTRACEIN(( _L("NS: %S"), &aNamespace ));
    TInt index = FindStorageByNamespace( aNamespace );
    User::LeaveIfError( index );
    DLTRACEOUT(("index: %d", index));
    return *(iStorages[index]);
    }
    
    
// ---------------------------------------------------------------------------
// Storage remover
// ---------------------------------------------------------------------------
//
void CNcdStorageClient::RemoveStorageL( const TDesC& aNamespace )
    {
    DLTRACEIN((""));
    if ( !aNamespace.Length() ) 
        {
        User::Leave( KErrArgument );
        }
    
    TInt index = FindStorageByNamespace( aNamespace );
    
    if ( index == KErrNotFound )
        {
        DLTRACEOUT(("Namespace not found"));
        return;
        }
    
    // Generate path before deleting the storage in case aNamespace is a
    // reference to storage's member variable
    RBuf path;
    CleanupClosePushL( path );
    path.CreateL( KMaxPath );
    
    // Generate path to the storage directory
    
    AppendRoot( path );
    path.Append( iStorages[ index ]->Directory() );
    path.Append( KDirectorySeparator );

        
    delete iStorages[ index ];
    iStorages.Remove( index );
    
    
    DLTRACE( ( _L("Deleting: %S"), &path ) );
    
    CNcdStorageManager::RemoveDirectoryL( FileSession(), 
        path );

    CleanupStack::PopAndDestroy( &path );
    
    // Updates the list of namespaces
    SaveNamespacesL();
    DLTRACEOUT( ("") );
    }


// ---------------------------------------------------------------------------
// Path generator
// ---------------------------------------------------------------------------
//    
void CNcdStorageClient::AppendRoot( TDes& aDes ) const
    {
    DLTRACEIN((""));

    iOwner.AppendRoot( aDes );        
    aDes.Append( iUid );
    aDes.Append( KDirectorySeparator );

    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// File session getter
// ---------------------------------------------------------------------------
//    
RFs& CNcdStorageClient::FileSession()
    {
    DLTRACEIN((""));
    return iOwner.FileSession();
    }


// ---------------------------------------------------------------------------
// File manager getter
// ---------------------------------------------------------------------------
//    
CFileMan& CNcdStorageClient::FileManager()
    {
    return iOwner.FileManager();
    }
    
// ---------------------------------------------------------------------------
// Client UID getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdStorageClient::ClientUid() const
    {
    return iUid;
    }


// ---------------------------------------------------------------------------
// NamespacesL
// ---------------------------------------------------------------------------
//
MDesCArray* CNcdStorageClient::NamespacesLC() const
    {
    DLTRACEIN((""));
    CPtrCArray* array = new(ELeave) CPtrCArray( KListGranularity );
    
    // Casting ensures that the popped pointer matches the returned pointer
    CleanupDeletePushL( static_cast<MDesCArray*>( array) );
    array->SetReserveL( iStorages.Count() );
    
    for ( TInt i = 0; i < iStorages.Count(); ++i )
        {
        array->AppendL( iStorages[i]->Namespace() );
        }
            
    return array;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdStorageClient::CNcdStorageClient( MNcdStorageOwner& aOwner ) : 
    iOwner( aOwner )
    {
    }    
    

// ---------------------------------------------------------------------------
// Searches for a storage that matches the given namespace
// ---------------------------------------------------------------------------
//        
TInt CNcdStorageClient::FindStorageByNamespace( 
    const TDesC& aNamespace ) const
    {
    TInt count = iStorages.Count();
    for ( TInt i = 0; i < count; ++i ) 
        {
        if ( iStorages[ i ]->Namespace().Compare( aNamespace ) == 0) 
            {
            return i;
            }            
        }
    
    return KErrNotFound;
    }


// ---------------------------------------------------------------------------
// Creates a storage object for each found directory
// ---------------------------------------------------------------------------
//    
/*    
void CNcdStorageClient::CreateStoragesFromFileSystemL( const TDesC& aPath )
    {
    DLTRACEIN((""));
    CDir* dirList = NULL;
    CDir* fileList = NULL;
    User::LeaveIfError( FileSession().GetDir( aPath, 
        KEntryAttNormal, ESortByName, fileList, dirList ) );
    DLTRACE(("Creating storages"));
    TInt err = KErrNone;
    
    // No need for this
    delete fileList;
    
    CleanupStack::PushL( dirList );
    
    for ( TInt i = 0; i < dirList->Count(); ++i )
        {
        TRAP( err, CreateStorageL( (*dirList)[i].iName ) );
        if ( err != KErrNone && err != KErrAlreadyExists )
            {
            User::Leave( err );
            }
        }
    CleanupStack::PopAndDestroy( dirList );    
    }
*/
    
// ---------------------------------------------------------------------------
// Reads namespace encodings from the namespace file
// ---------------------------------------------------------------------------
//
void CNcdStorageClient::ReadNamespacesL()
    {
    DLTRACEIN((""));    
    RBuf filename;
    CleanupClosePushL( filename );
    filename.CreateL( KMaxPath );
        
    AppendRoot( filename );
    filename.Append( KNamespaceFile );
    
    HBufC8* data = ReadFileL( FileSession(), filename );
    CleanupStack::PopAndDestroy( &filename );
    
    CleanupStack::PushL( data );
    RDesReadStream reader( *data );
    CleanupClosePushL( reader );
    
    // number of namespaces
    TInt32 count = reader.ReadInt32L();
    
    iStorages.ReserveL( count );
    
    HBufC* namesp = NULL;
    HBufC* dir = NULL;
    DLINFO(("Internalizing %d namespaces"));
    while ( count )
        {       
        DLTRACE(("Internalizing namespace and directory")); 
        InternalizeDesL( namesp, reader );                
        CleanupStack::PushL( namesp );
        
        InternalizeDesL( dir, reader );
        CleanupStack::PushL( dir );
        
        TBool exists = DirectoryExistsL( *dir );
        if ( exists ) 
            {
            DLTRACE(( _L("Client nsp: %S in dir: %S"),
                namesp, dir ));
                
            // Ownership of the namesp and dir is transferred
            CNcdStorage* client = CNcdStorage::NewL(
                *this, namesp, dir );
            CleanupStack::Pop( 2, namesp ); // dir, namesp
            
            // Won't fail because of the ReserveL-call
            iStorages.Append( client );            
            DLTRACE(("Client created successfully"));
            }
        else
            {
            DLTRACE(("Client doesn't exist anymore"));
            CleanupStack::PopAndDestroy( 2, namesp ); // dir, namesp
            }
        namesp = NULL;
        dir = NULL;
        --count;
        }
    CleanupStack::PopAndDestroy( 2, data ); // reader, data
    }

// ---------------------------------------------------------------------------
// Save namespaces
// ---------------------------------------------------------------------------
//
void CNcdStorageClient::SaveNamespacesL()
    {
    DLTRACEIN((""));
    TFileName tempName;

    // Get root dir
    RBuf target;
    CleanupClosePushL( target );
    target.CreateL( KMaxPath );
    AppendRoot( target );
    
    DLINFO(( "Creating file write stream.." ));
    RFileWriteStream writer;
    CleanupClosePushL( writer );
    
    DLINFO(( "Getting file session.." ));
    RFs& fs( FileSession() );
    // Create a temp file
    User::LeaveIfError( writer.Temp( 
        fs, 
        target, 
        tempName, 
        EFileWrite | EFileStream ) );
        
    writer.WriteInt32L( iStorages.Count() );
    for ( TInt i = 0; i < iStorages.Count(); ++i )
        {
        ExternalizeDesL( iStorages[i]->Namespace(), writer );
        ExternalizeDesL( iStorages[i]->Directory(), writer );
        }
    // Commit explicitly so we can react to errors
    writer.CommitL();
    CleanupStack::PopAndDestroy( &writer );
    
    // Append the namespace-file to root dir
    target.Append( KNamespaceFile );
    
    // Replace the old file with the new one
    BaflUtils::DeleteFile( fs, target );
    User::LeaveIfError( fs.Rename( tempName, target ) );
    
    CleanupStack::PopAndDestroy( &target ); // target
    DLTRACEOUT(("File saved successfully"));
    
    }


// ---------------------------------------------------------------------------
// Checks if the directory exists
// ---------------------------------------------------------------------------
//
TBool CNcdStorageClient::DirectoryExistsL( const TDesC& aDir )
    {
    DLTRACEIN(( _L("Directory: %S"), &aDir ));
    RBuf path;
    CleanupClosePushL( path );
    path.CreateL( KMaxPath );
    AppendRoot( path );    
    path.Append( aDir );
    
    TBool exists = BaflUtils::FolderExists( FileSession(), path );
    CleanupStack::PopAndDestroy( &path );
    return exists;
    }

