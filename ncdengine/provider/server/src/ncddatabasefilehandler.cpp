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
* Description:   Implements CNcdDatabaseFileHandler class
*
*/


#include <f32file.h> // RFs

#include "ncddatabasefilehandler.h"
#include "ncdproviderdefines.h"
#include "catalogsdebug.h"
#include "ncdstorageclient.h"
#include "ncdstorage.h"
#include "ncddatabasestorage.h"
#include "ncdstoragefiledataitem.h"
#include "ncdstoragebase.h"
#include "catalogsutils.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdDatabaseFileHandler* CNcdDatabaseFileHandler::NewL( 
    MNcdStorageClient& aStorageClient, 
    NcdNodeClassIds::TNcdNodeClassType aDataType )
    {
    CNcdDatabaseFileHandler* self = new( ELeave ) 
        CNcdDatabaseFileHandler( aStorageClient, aDataType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CNcdDatabaseFileHandler::~CNcdDatabaseFileHandler()
    {
    DLTRACE(("--><--"));
    }
    
    
// ---------------------------------------------------------------------------
// MoveL
// ---------------------------------------------------------------------------
//
void CNcdDatabaseFileHandler::MoveFileL( const TDesC& aSourceFile,
    const TDesC& aNamespaceOrDir, const TDesC& aIdOrName,
    TBool /* aOverwrite */ )
    {    
    DLTRACEIN((""));
    DLINFO(( _L("Src: %S, ns: %S, id: %S"), &aSourceFile,
        &aNamespaceOrDir, &aIdOrName ));
        
    // Get storage or create it if it doesn't exist
    MNcdStorage& storage = StorageL( aNamespaceOrDir );        
    
    MNcdDatabaseStorage& db = storage.DatabaseStorageL( 
        NcdProviderDefines::KDefaultDatabaseUid );
    
    // Stored data is of the given type
    MNcdStorageItem* item = db.StorageItemL( aIdOrName, 
        iDataType );
    
    
    CNcdStorageFileDataItem* dataItem = 
        CNcdStorageFileDataItem::NewL( aSourceFile, 
        storage.FileSession() );
    CleanupStack::PushL( dataItem );
    
    //DLINFO(("Internalizing the data"));
    item->SetDataItem( dataItem );
    
    item->OpenL();
    // Write the file to the storage
    //DLINFO(("Writing data"));
    item->WriteDataL();
    
    DLINFO(("Saving data"));
    item->SaveL();
    
    CleanupStack::PopAndDestroy( dataItem );
    
    DLTRACE(( "Deleting source file" ));
    // Delete sourcefile after successful internalization
    User::LeaveIfError( storage.FileSession().Delete( aSourceFile ) );
    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdDatabaseFileHandler::CNcdDatabaseFileHandler( 
    MNcdStorageClient& aStorageClient, 
    NcdNodeClassIds::TNcdNodeClassType aDataType ) : 
    iStorageClient( aStorageClient ), iDataType( aDataType )
    {    
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdDatabaseFileHandler::ConstructL()
    {
    }
    

// ---------------------------------------------------------------------------
// Ensures the storage exists
// ---------------------------------------------------------------------------
//
MNcdStorage& CNcdDatabaseFileHandler::StorageL( const TDesC& aNamespace )
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

