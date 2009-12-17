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
* Description:   Implements CNcdFileStorageFileHandler class
*
*/


#include "ncdfilestoragefilehandler.h"
#include "ncdproviderdefines.h"
#include "catalogsdebug.h"
#include "ncdstorageclient.h"
#include "ncdstorage.h"
#include "ncdfilestorage.h"
#include "catalogsutils.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdFileStorageFileHandler* CNcdFileStorageFileHandler::NewL( 
    MNcdStorageClient& aStorageClient )
    {
    CNcdFileStorageFileHandler* self = new( ELeave ) 
        CNcdFileStorageFileHandler( aStorageClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CNcdFileStorageFileHandler::~CNcdFileStorageFileHandler()
    {
    DLTRACE(("--><--"));
    }
    
    
// ---------------------------------------------------------------------------
// MoveL
// ---------------------------------------------------------------------------
//
void CNcdFileStorageFileHandler::MoveFileL( const TDesC& aSourceFile,
    const TDesC& aNamespaceOrDir, const TDesC& /* aIdOrName */,
    TBool aOverwrite )
    {    
    DLTRACEIN(( _L("Src: %S, dir: %S"), &aSourceFile,
        &aNamespaceOrDir ) );
    
    // Get storage or create it if it doesn't exist
    MNcdStorage& storage = StorageL( aNamespaceOrDir );
    
    MNcdFileStorage& fs = storage.FileStorageL( 
        NcdProviderDefines::KDefaultFileStorageUid );
    
    
    fs.AddFileL( aSourceFile, aOverwrite );
    DLTRACEOUT( ( "" ) );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdFileStorageFileHandler::CNcdFileStorageFileHandler( 
    MNcdStorageClient& aStorageClient ) : 
    iStorageClient( aStorageClient )
    {    
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdFileStorageFileHandler::ConstructL()
    {
    }
    

// ---------------------------------------------------------------------------
// Ensures the storage exists
// ---------------------------------------------------------------------------
//
MNcdStorage& CNcdFileStorageFileHandler::StorageL( const TDesC& aNamespace )
    {
    DLTRACEIN((""));
    MNcdStorage* storage = NULL;
    // Search for the storage
    TRAPD( err,                 
        storage = &iStorageClient.StorageL( aNamespace ) );

    if ( err == KErrNone ) 
        {
        DLTRACEOUT((""));
        return *storage;
        }
    else if ( err != KErrNotFound ) 
        {
        DLTRACEOUT(("Leaving with: %d", err));
        User::Leave( err );    
        }

    // Create the namespace
    return iStorageClient.CreateStorageL( aNamespace );    
    }
