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

#include "ncdstoragemanagerimpl.h"
#include "ncdstorageimpl.h"
#include "ncdstorageclientimpl.h"
#include "ncdproviderdefines.h"

#include "catalogsconstants.h"
#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdStorageManager::ConstructL( const TDesC& aRootDirectory )
    {
    DLTRACEIN( ("") );    
    iRootDir.CreateL( aRootDirectory );    
    iFileMan = CFileMan::NewL( iFs );    
    DLTRACEOUT( ("" ) );
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdStorageManager* CNcdStorageManager::NewL( 
    RFs& aFs,
    const TDesC& aRootDirectory )
    {
    CNcdStorageManager* self = new (ELeave ) CNcdStorageManager( aFs );
    CleanupStack::PushL( self );
    self->ConstructL( aRootDirectory );
    CleanupStack::Pop( self );
    return self;
    }



// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdStorageManager::~CNcdStorageManager()
    {
    DLTRACEIN( ( "" ) );
    
    iClients.ResetAndDestroy();    
    
    delete iFileMan;   
    iRootDir.Close();
    }


// ---------------------------------------------------------------------------
// CreateOrGetStorageL
// ---------------------------------------------------------------------------
//
MNcdStorage& CNcdStorageManager::CreateOrGetStorageL( const TDesC& aClientUid, 
    const TDesC& aNamespace )
    {
    DLTRACEIN((""));
    MNcdStorage* storage = NULL;
    
    TRAPD( err,        
           storage =
           &StorageL( aClientUid, aNamespace ) );
    
    if ( err == KErrNotFound ) 
        {
        DLINFO(("Creating storage for the client"));
        storage = 
            &CreateStorageL( aClientUid, 
                             aNamespace );
        }
    else if ( err != KErrNone )
        {
        DLERROR(("Leaving: %i", err));
                
        User::Leave( err );   
        }
            

    DLTRACEOUT((""));
    return *storage;    
    }


// ---------------------------------------------------------------------------
// CreateStorageL
// ---------------------------------------------------------------------------
//
MNcdStorage& CNcdStorageManager::CreateStorageL( const TDesC& aClientUid, 
    const TDesC& aNamespace )
    {
    DLTRACEIN( ("") );
    TInt index = FindClientByUid( aClientUid );
    
    CNcdStorageClient* client = NULL;
    
    if ( index == KErrNotFound ) 
        {
        DLTRACE( ("Create a new client") );
        // Create a new client
        client = CNcdStorageClient::NewLC( *this, aClientUid );
        iClients.AppendL( client );
        CleanupStack::Pop( client );
        }
    else 
        {
        DLTRACE( ("Use an existing client") );
        // Use existing client
        client = iClients[ index ];
        DASSERT( client );
        }

    // Create the storage
    MNcdStorage& storage = client->CreateStorageL( aNamespace );
    DLTRACEOUT( ("") );
    return storage;                
    }


// ---------------------------------------------------------------------------
// Provider storage getter
// ---------------------------------------------------------------------------
//
MNcdStorage& CNcdStorageManager::ProviderStorageL( const TDesC& aClientUid )
    {
    return CreateOrGetStorageL( 
        aClientUid, 
        NcdProviderDefines::KProviderStorageNamespace );
    }
    
    
// ---------------------------------------------------------------------------
// Storage getter
// ---------------------------------------------------------------------------
//
MNcdStorage& CNcdStorageManager::StorageL( const TDesC& aClientUid, 
    const TDesC& aNamespace )
    {
    DLTRACEIN( ("") );
    TInt index = FindClientByUidL( aClientUid );
            
    return iClients[index]->StorageL( aNamespace );
    }


// ---------------------------------------------------------------------------
// Storage client namespace list getter
// ---------------------------------------------------------------------------
//
MDesCArray* CNcdStorageManager::StorageNamespacesLC( 
    const TDesC& aClientUid ) const
    {
    TInt index = FindClientByUidL( aClientUid );
    return iClients[index]->NamespacesLC();
    }
    
// ---------------------------------------------------------------------------
// Storage client getter
// ---------------------------------------------------------------------------
//
MNcdStorageClient& CNcdStorageManager::StorageClientL( 
    const TDesC& aClientUid )
    {
    TInt index = FindClientByUid( aClientUid );
    if ( index == KErrNotFound ) 
        {
        // Create a new client
        CNcdStorageClient* client = CNcdStorageClient::NewLC( *this, aClientUid );
        iClients.AppendL( client );
        CleanupStack::Pop( client );
        return *client;
        }
    return *iClients[ index ];
    }


// ---------------------------------------------------------------------------
// Client remover
// ---------------------------------------------------------------------------
//
/*
void CNcdStorageManager::RemoveClientL( const TDesC& aClientUid )
    {    
    DLTRACEIN( ("") );
    TInt index = KErrNone;
    
    TRAPD( err, index = FindClientByUidL( aClientUid ) );
    if ( err == KErrNotFound ) 
        {
        // Create storage client temporarily.
        CNcdStorageClient* client = CNcdStorageClient::NewLC( *this, aClientUid );
        iClients.AppendL( client );
        CleanupStack::Pop( client );
        index = iClients.Count() - 1;
        }
    
    RBuf path;
    CleanupClosePushL( path );
    path.CreateL( KMaxPath );
    
    iClients[index]->AppendRoot( path );
        
    DLINFO( ( _L("Removing client: %S"), &path ) );
    
    delete iClients[index];
    iClients.Remove( index );
    
    // Remove client's directory
    RemoveDirectoryL( iFs, path );
    
    CleanupStack::PopAndDestroy( &path );
    DLTRACEOUT((""));
    }
*/    
    
// ---------------------------------------------------------------------------
// Storage remover
// ---------------------------------------------------------------------------
//
void CNcdStorageManager::RemoveStorageL( const TDesC& aClientUid, 
    const TDesC& aNamespace)
    {
    DLTRACEIN( ("") );
    TInt index = KErrNone;
    TRAPD( err, index = FindClientByUidL( aClientUid ) );
    if ( err == KErrNotFound ) 
        {
        // Create storage client.
        CNcdStorageClient* client = CNcdStorageClient::NewLC( *this, aClientUid );
        iClients.AppendL( client );
        CleanupStack::Pop( client );
        index = iClients.Count() - 1;
        }
    
    // Removes the actual storage directory
    TRAP( err, iClients[ index ]->RemoveStorageL( aNamespace ) );
    if ( err != KErrNotFound ) 
        {
        User::LeaveIfError( err );
        }
        
    DLTRACEOUT( ( "" ) );
    }

// ---------------------------------------------------------------------------
// Close
// ---------------------------------------------------------------------------
//
void CNcdStorageManager::CloseClient( const TDesC& aClientUid )
    {
    DLTRACEIN((""));
    TInt index = FindClientByUid( aClientUid );
    if ( index != KErrNotFound )
        {
        delete iClients[ index ];
        iClients.Remove( index );
        }
    }


// ---------------------------------------------------------------------------
// File session getter
// ---------------------------------------------------------------------------
//
RFs& CNcdStorageManager::FileSession()
    {
    DLTRACEIN((""));

    return iFs;
    }
    

// ---------------------------------------------------------------------------
// Prepends root dir to the des
// ---------------------------------------------------------------------------
//
void CNcdStorageManager::AppendRoot( TDes& aPath ) const
    {
    DLTRACEIN((""));

    aPath.Append( iRootDir );
    
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// File manager getter
// ---------------------------------------------------------------------------
//
CFileMan& CNcdStorageManager::FileManager()
    {
    DASSERT( iFileMan );
    return *iFileMan;
    }


// ---------------------------------------------------------------------------
// Utility function for removing a directory
// ---------------------------------------------------------------------------
//
void CNcdStorageManager::RemoveDirectoryL( RFs& aFileSession, 
    const TDesC& aDirectory )
    {
    CFileMan* fileman = CFileMan::NewL( aFileSession );
    CleanupStack::PushL( fileman );
    User::LeaveIfError( fileman->RmDir( aDirectory ) );    
    CleanupStack::PopAndDestroy( fileman );    
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdStorageManager::CNcdStorageManager( RFs& aFs ) : iFs( aFs )
    {
    }
    

// ---------------------------------------------------------------------------
//  Searches for the storage client by uid
// ---------------------------------------------------------------------------
//
TInt CNcdStorageManager::FindClientByUidL( const TDesC& aUid ) const
    {
    DLTRACEIN(( _L("Uid: %S"), &aUid ));
    TInt index = FindClientByUid( aUid );    
    User::LeaveIfError( index );
    return index;
    }
    
    
// ---------------------------------------------------------------------------
//  Searches for the storage client by uid
// ---------------------------------------------------------------------------
//
TInt CNcdStorageManager::FindClientByUid( const TDesC& aUid ) const
    {
    DLTRACEIN(( _L("Uid: %S"), &aUid ));
    TInt count = iClients.Count();
    for ( TInt i = 0; i < count; ++i ) 
        {
        if ( iClients[ i ]->ClientUid().Compare( aUid ) == 0 ) 
            {
            DLTRACEOUT(("index: %d", i));
            return i;
            }
        }
    DLTRACEOUT(("Err: -1"));    
    return KErrNotFound;
    }
    
    
    
