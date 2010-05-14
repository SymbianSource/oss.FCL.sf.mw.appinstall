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
* Description:   Implements CNcdNodeDbManager class
*
*/


#include "ncdnodedbmanager.h"

#include <e32err.h>
#include <s32mem.h>
#include <badesca.h>

#include "ncdnodeidentifier.h"
#include "ncdnodeidentifiereditor.h"
#include "catalogsutils.h"
#include "ncdstoragemanager.h"
#include "ncdstoragebase.h"
#include "ncdstorage.h"
#include "ncdfilestorage.h"
#include "ncddatabasestorage.h"
#include "ncdproviderdefines.h"
#include "ncdstorageclient.h"
#include "ncdstorageitem.h"
#include "catalogsconstants.h"
#include "ncdnodeidentifierutils.h"

#include "catalogsdebug.h"

CNcdNodeDbManager::CNcdNodeDbManager(
    MNcdStorageManager& aStorageManager )
: CBase(),
  iStorageManager( aStorageManager )
    {
    DLTRACEIN((""));

    DLTRACEOUT((""));
    }

void CNcdNodeDbManager::ConstructL()
    {
    DLTRACEIN((""));

    DLTRACEOUT((""));
    }

CNcdNodeDbManager* CNcdNodeDbManager::NewL(
    MNcdStorageManager& aStorageManager )
    {
    CNcdNodeDbManager* self =
        CNcdNodeDbManager::NewLC( aStorageManager );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeDbManager* CNcdNodeDbManager::NewLC(
    MNcdStorageManager& aStorageManager )
    {
    CNcdNodeDbManager* self =
        new( ELeave ) CNcdNodeDbManager( aStorageManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;        
    }


CNcdNodeDbManager::~CNcdNodeDbManager()
    {
    DLTRACEIN((""));

    // Do not delete storage manager here because it is not owned
    // by this class object.
    DLTRACEOUT((""));
    }



// Database info functions

// ---------------------------------------------------------------------------
// StorageSizeL
// ---------------------------------------------------------------------------
//
TInt CNcdNodeDbManager::StorageSizeL( const TUid& aClientUid,
                                      const MDesCArray& aSkipNamespaces )
    {
    DLTRACEIN((""));
    DPROFILING_BEGIN( x );
    TInt size( 0 );

    // Get the namespaces from the storage of the given client and insert
    // the namespace array to the cleanup stack. Notice that the array items
    // are deleted when the array is deleted.
    MNcdStorageClient& client = iStorageManager.StorageClientL( aClientUid.Name() );
    MDesCArray* namespaces = client.NamespacesLC();

    // Temporary variables for the loop
    TPtrC clientNamespace;
    TBool doNotSkip( ETrue );  
    
    // Start to remove all the possible namespaces from the client.  
    for ( TInt i = 0; i < namespaces->MdcaCount(); ++i )
        {
        doNotSkip = ETrue;
        clientNamespace.Set( namespaces->MdcaPoint( i ) );

        // Check if this namespace should be skipped because its content
        // is not wanted to be part of the size info.
        for ( TInt j = 0; j < aSkipNamespaces.MdcaCount(); ++j )
            {
            if ( clientNamespace == aSkipNamespaces.MdcaPoint( j ) )
                {
                DLINFO(("Skip namespace"));
                // This namespace should be skipped.
                doNotSkip = EFalse;
                break;
                }
            }
        if ( doNotSkip ) 
            {
            DLINFO(("Add namespace size"));
            // This size increase should not be skipped. So, add the size to the total.
            // Storage will contains the data of the client identified by its UID
            // The identifier id can be empty when creating storage. Only,
            // namespace and uid are actually used.
            CNcdNodeIdentifier* storageIdentifier = 
                CNcdNodeIdentifier::NewLC( clientNamespace, KNullDesC, aClientUid );
            MNcdStorage& storage = StorageL( *storageIdentifier );
            CleanupStack::PopAndDestroy( storageIdentifier );

            size += storage.SizeL();
            }
        }
    
    CleanupStack::PopAndDestroy( namespaces );
    DPROFILING_END( x );
    DLTRACEOUT(("Storage size: %d", size));
    
    return size;
    }


// Database read functions

// ---------------------------------------------------------------------------
// ReadFromDatabaseL
// ---------------------------------------------------------------------------
//
HBufC8* CNcdNodeDbManager::ReadDataFromDatabaseL( 
    const CNcdNodeIdentifier& aIdentifier,
    const NcdNodeClassIds::TNcdNodeClassType aClassType )
    {
    DLTRACEIN((""));

    HBufC8* ret( ReadDataFromDatabaseLC( aIdentifier, 
                                         aClassType ) );
    if ( ret != NULL )
        {
        CleanupStack::Pop( ret );
        }

    DLTRACEOUT((""));    
    return ret;
    }
   
// ---------------------------------------------------------------------------
// ReadFromDatabaseLC
// ---------------------------------------------------------------------------
//    
HBufC8* CNcdNodeDbManager::ReadDataFromDatabaseLC( 
    const CNcdNodeIdentifier& aIdentifier,
    const NcdNodeClassIds::TNcdNodeClassType aClassType )
    {
    DLTRACEIN((""));
    DPROFILING_BEGIN( x );
    DASSERT( !aIdentifier.ContainsEmptyFields() );
        
    // Storage will contains the data of the client identified by its UID
    MNcdStorage& storage = StorageL( aIdentifier );
    
    // NOTE: this creates the database if it does not already exist.
    MNcdDatabaseStorage& database = 
        storage.DatabaseStorageL( NcdProviderDefines::KDefaultDatabaseUid );
                        
    // Get the storage item from which the node is loaded
    // Note: database has the ownership of the item
    MNcdStorageItem* item = 
        database.StorageItemL( aIdentifier.NodeId(), 
                               aClassType );    
        
    DLINFO(("Get data"));
    HBufC8* data = item->GetDataLC();

    DLTRACEOUT(("Returning data, length: %d", data->Length()));
    DPROFILING_END( x );
    return data;
    }


// ---------------------------------------------------------------------------
// StartStorageLoadActionL
// ---------------------------------------------------------------------------
//
void CNcdNodeDbManager::StartStorageLoadActionL( 
    const CNcdNodeIdentifier& aIdentifier,
    MNcdStorageDataItem& aDataItem,
    const NcdNodeClassIds::TNcdNodeClassType aClassType )
    {
    DLTRACEIN((""));
    DPROFILING_BEGIN( x );
    if( aIdentifier.ContainsEmptyFields() )
        {
        DLERROR(("Empty identifier fields given"));

        // For debugging purposes
        DASSERT( EFalse );
        
        User::Leave( KErrArgument );
        }
    
    // Here we will get the data from the storage according
    // to the namespace and data id information. Also, class id
    // is used to define the type of the data that is searched.
    
    // The type of the data should be gotten from the first four bytes (TInt32)
    // of the stream when the Internalize function of the data item is called
    // from the storage handler.

    // Storage will contains the data of the client identified by its UID
    MNcdStorage& storage = StorageL( aIdentifier );
    
    // NOTE: If db does not already exist, 
    // this creates one into the storage.
    MNcdDatabaseStorage& database = 
        storage.DatabaseStorageL( NcdProviderDefines::KDefaultDatabaseUid );
        
    // Ensure the node exists in the database    
    
    if( !database.ItemExistsInStorageL( aIdentifier.NodeId(), 
                                        aClassType ) )
        {
        DLINFO(("Data was not in namespace"));
        User::Leave( KErrNotFound );            
        }
    
    // Get the storage item from which the node is loaded
    // Note: database has the ownership of the item
    MNcdStorageItem* item = 
        database.StorageItemL( aIdentifier.NodeId(), 
                               aClassType );    
        
    // Get data from database by using aDataItem as the target so that 
    // internalize will be called for it
    item->SetDataItem( &aDataItem );
    
    // Read node data -> calls InternalizeL of aDataItem
    item->ReadDataL();
    DPROFILING_END( x );
    DLTRACEOUT(("Item IsOpen: %d", item->IsOpen() ));
    }


// ---------------------------------------------------------------------------
// GetAllClientItemIdentifiersL
// ---------------------------------------------------------------------------
//    
void CNcdNodeDbManager::GetAllClientItemIdentifiersL( 
    RPointerArray<CNcdNodeIdentifier>& aItemIdentifiers,
    const TUid& aClientUid,
    const MDesCArray& aSkipNamespaces,
    const RArray<NcdNodeClassIds::TNcdNodeClassType>& aAcceptClassTypes  )
    {
    DLTRACEIN((""));
    DPROFILING_BEGIN( x );
    // This temporary array is used for the items that are gotten from
    // the storages.
    // Insert it into the cleanup stack to be sure that the array is closed
    // if leave occurs.No need to worry about deleting array items here.
    // They are owned by the storage.
    RPointerArray<MNcdStorageItem> tmpItemArray;
    CleanupClosePushL( tmpItemArray );
    
    // Get all the namespaces of the client. Notice that the namespaces are
    // deleted from the array when the array is deleted.  
    MNcdStorageClient& client = 
        iStorageManager.StorageClientL( aClientUid.Name() );
    MDesCArray* namespaces = client.NamespacesLC();

    // Temporary variables that are used in the loops.
    TPtrC clientNamespace;
    TBool doNotSkip( ETrue );
    MNcdStorageItem* tmpItem( NULL );  
    CNcdNodeIdentifier* tmpItemIdentifier( NULL );
    
    // Start to remove all the possible namespaces from the client.  
    for ( TInt i = 0; i < namespaces->MdcaCount(); ++i )
        {
        doNotSkip = ETrue;
        clientNamespace.Set( namespaces->MdcaPoint( i ) );

        // Check if this namespace should be skipped.
        for ( TInt j = 0; j < aSkipNamespaces.MdcaCount(); ++j )
            {
            if ( clientNamespace == aSkipNamespaces.MdcaPoint( j ) )
                {
                DLINFO(("Skip namespace"));
                // This namespace should be skipped
                doNotSkip = EFalse;
                break;
                }
            }
            
        if ( doNotSkip ) 
            {
            DLINFO(("Did not skip namespace"));
            // This namespace should not be skipped. So, do your thing.
            
            // Storage will contains the data of the client identified by its UID
            // The identifier id can be empty when creating storage. Only,
            // namespace and uid are actually used.
            CNcdNodeIdentifier* storageIdentifier = 
                CNcdNodeIdentifier::NewLC( clientNamespace, KNullDesC, aClientUid );
            MNcdStorage& storage = StorageL( *storageIdentifier );
            CleanupStack::PopAndDestroy( storageIdentifier );
                
            // NOTE: If db does not already exist, 
            // this creates one into the storage.
            MNcdDatabaseStorage& database = 
                storage.DatabaseStorageL( NcdProviderDefines::KDefaultDatabaseUid );
                    
            // Now get all the items from the db storage.
            // Notice that the tmpItemArray will not own the items, but they
            // are owned by the database.
            database.StorageItemsL( tmpItemArray );
            
            DLINFO(("tmpItemArrayCount: %d", tmpItemArray.Count()));
            TInt count = tmpItemArray.Count();
            while ( count-- )
                {
                // Move the item info from tmpItemArray into the return array.
                tmpItem = tmpItemArray [ count ];
                for ( TInt m = 0; m < aAcceptClassTypes.Count(); ++m )
                    {
                    if ( tmpItem->Type() == aAcceptClassTypes[ m ] )
                        {
                        tmpItemIdentifier = 
                            CNcdNodeIdentifier::NewLC( clientNamespace,
                                                       tmpItem->Uid(), 
                                                       aClientUid );

                        aItemIdentifiers.AppendL( tmpItemIdentifier );
                        
                        CleanupStack::Pop( tmpItemIdentifier );                        
                       
                        tmpItemIdentifier = NULL;     
                        break;                       
                        }
                    }
                // Notice that the item array does not own the
                // items. So, do not delete them but only remove them
                // from the array.
                tmpItemArray.Remove( count );
                }                
            }
        }
    
    // Deletes the array and its items
    CleanupStack::PopAndDestroy( namespaces );
    
    // Closes the array but does not delete items because the items
    // are not owned by this array.
    CleanupStack::PopAndDestroy( &tmpItemArray );
    DPROFILING_END( x );
    DLTRACEOUT(("Items load ok."));
    }



// Database write functions

// ---------------------------------------------------------------------------
// SaveDataIntoDatabaseL
// ---------------------------------------------------------------------------
//
void CNcdNodeDbManager::SaveDataIntoDatabaseL( 
    const CNcdNodeIdentifier& aIdentifier,
    MNcdStorageDataItem& aDataItem,
    const NcdNodeClassIds::TNcdNodeClassType aClassType )
    {
    DLTRACEIN((""));
    DPROFILING_BEGIN( x );
    if( aIdentifier.ContainsEmptyFields() )
        {
        DLERROR(("Empty identifier fields given"));
        
        // For debugging purposes
        DASSERT( EFalse );
        
        User::Leave( KErrArgument );
        }
    
    // Storage will contains the data of the client identified by its UID
    MNcdStorage& storage = StorageL( aIdentifier );
        
    // NOTE: If db does not already exist, 
    // this creates one into the storage.
    MNcdDatabaseStorage& database = 
        storage.DatabaseStorageL( NcdProviderDefines::KDefaultDatabaseUid );
            
    // Get/create the storage item where the data is saved
    // Note: database has the ownership of the item
    MNcdStorageItem* storageItem = 
        database.StorageItemL( aIdentifier.NodeId(), 
                               aClassType );    
    
    // Here call the storage functions that will handle 
    // the saving of the data
    
    // Save new item to database
    storageItem->SetDataItem( &aDataItem );
    storageItem->OpenL();
    
    // Calls ExternalizeL for data item
    storageItem->WriteDataL();
    
    // Save the data to the database.
    // The data object implements MNcdStorageDataItem interface.
    // So, the externalize function will insert the data to the stream
    // that the database handler will save to the database.
    storageItem->SaveL();        
    DPROFILING_END( x );
    DLTRACEOUT((""));    
    }
  


// Database remove functions
  
// ---------------------------------------------------------------------------
// RemoveDataFromDatabaseL
// ---------------------------------------------------------------------------
//    
void CNcdNodeDbManager::RemoveDataFromDatabaseL( 
    const CNcdNodeIdentifier& aIdentifier,
    const NcdNodeClassIds::TNcdNodeClassType aClassType ) 
    {
    DLTRACEIN((""));
    DPROFILING_BEGIN( x );
    if( aIdentifier.ContainsEmptyFields() )
        {
        DLERROR(("Empty identifier fields given"));
        DASSERT( EFalse );
        User::Leave( KErrArgument );
        }
        
    // Storage will contains the data of the client identified by its UID
    MNcdStorage& storage = StorageL( aIdentifier );
        
    // NOTE: If db does not already exist, 
    // this creates one into the storage.
    MNcdDatabaseStorage& database = 
        storage.DatabaseStorageL( NcdProviderDefines::KDefaultDatabaseUid );
            
    // Get/create the storage item where the data is saved
    // Note: database has the ownership of the item.
    MNcdStorageItem* storageItem = 
        database.StorageItemL( aIdentifier.NodeId(), aClassType );    

    // Remove the item from the storage
    storageItem->RemoveFromStorageL();

    // Make the removing happen.
    database.CommitL();    
    DPROFILING_END( x );
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// RemoveDataFromDatabaseL
// ---------------------------------------------------------------------------
//    
void CNcdNodeDbManager::RemoveDataFromDatabaseL( 
    const RPointerArray<CNcdNodeIdentifier>& aIdentifiers,
    const RArray<NcdNodeClassIds::TNcdNodeClassType>& aClassTypes,
    TBool aCompact ) 
    {
    DLTRACEIN((""));
    DPROFILING_BEGIN( x );
    CNcdNodeIdentifier* identifier( NULL );
    
    TPtrC currentNamespace;
    TInt i = 0;
    const TInt count = aIdentifiers.Count();
    const TInt classCount = aClassTypes.Count();
    while( i < count )
        {
        identifier = aIdentifiers[ i ];
        if( identifier == NULL
            || identifier->ContainsEmptyFields() )
            {
            DLERROR(("NULL identifier or empty identifier fields given"));
            DASSERT( EFalse );
            User::Leave( KErrArgument );
            }    

        currentNamespace.Set( identifier->NodeNameSpace() );    
        // Storage will contains the data of the client identified by its UID
        MNcdStorage& storage = StorageL( *identifier );
            
        // NOTE: If db does not already exist, 
        // this creates one into the storage.
        MNcdDatabaseStorage& database = 
            storage.DatabaseStorageL( NcdProviderDefines::KDefaultDatabaseUid );

        for ( ; i < count && 
                currentNamespace == aIdentifiers[ i ]->NodeNameSpace(); ++i ) 
            {
            identifier = aIdentifiers[ i ];
            for ( TInt j = 0; j < classCount; ++j )
                {
                // Get/create the storage item where the data is saved
                // Note: database has the ownership of the item.
                MNcdStorageItem* storageItem = 
                    database.StorageItemL( identifier->NodeId(), aClassTypes[ j ] );    

                // Remove the item from the storage
                storageItem->RemoveFromStorageL();        
                }
            }
            
        // Commit & compact or just commit
        if ( aCompact ) 
            {            
            database.Compact();        
            }
        else 
            {
            database.CommitL();
            }
        }        
    DPROFILING_END( x );
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// RemoveDataFromDatabaseL
// ---------------------------------------------------------------------------
//    
void CNcdNodeDbManager::RemoveDataFromDatabaseL( 
    const CNcdNodeIdentifier& aNodeIdentifier,
    const RArray<RNcdDatabaseItems>& aDoNotRemoveItems )
    {
    DLTRACEIN((""));
    DPROFILING_BEGIN( x );
    // Storage will contain the data of the client identified by its UID
    // and node's namespace    
    MNcdStorage& storage = StorageL( aNodeIdentifier );
        
    // NOTE: If db does not already exist, 
    // this creates one into the storage.
    MNcdDatabaseStorage& database = 
        storage.DatabaseStorageL( NcdProviderDefines::KDefaultDatabaseUid );
    
    DLTRACE(("Removing from database"));
    // commits and compacts data
    database.RemoveItemsL( aDoNotRemoveItems );  
    DPROFILING_END( x );          
    }


// ---------------------------------------------------------------------------
// ClearClientL
// ---------------------------------------------------------------------------
//    
void CNcdNodeDbManager::ClearClientL( const TUid& aClientUid,
                                      const MDesCArray& aSkipNamespaces )
    {
    DLTRACEIN((""));
    DPROFILING_BEGIN( x );
    // Get the namespaces of the given client.
    // The namespaces are inserted into the array. Notice that the items in array
    // are deleted when the array is deleted.
    MNcdStorageClient& client = iStorageManager.StorageClientL( aClientUid.Name() );
    MDesCArray* namespaces = client.NamespacesLC();

    // Temporary variables for the array.
    TPtrC clientNamespace;
    TBool doNotSkip( ETrue );  
    
    // Start to remove all the possible namespaces from the client.  
    for ( TInt i = 0; i < namespaces->MdcaCount(); ++i )
        {
        doNotSkip = ETrue;
        clientNamespace.Set( namespaces->MdcaPoint( i ) );

        // Check if this namespace should be skipped.
        for ( TInt j = 0; j < aSkipNamespaces.MdcaCount(); ++j )
            {
            if ( clientNamespace == aSkipNamespaces.MdcaPoint( j ) )
                {
                DLINFO(("Skip namespace"));
                // This namespace should be skipped and not removed.
                doNotSkip = EFalse;
                break;
                }
            }
        if ( doNotSkip ) 
            {
            DLINFO((_L("Remove namespace: %S"),
                    &clientNamespace));
            // This removal should not be skipped. So, remove.
            client.RemoveStorageL( clientNamespace );
            }
        }
    
    CleanupStack::PopAndDestroy( namespaces );
    DPROFILING_END( x );
    DLTRACEOUT(("Client cleared"));
    }


// Misc functions

// ---------------------------------------------------------------------------
// StorageL
// ---------------------------------------------------------------------------
//    
MNcdStorage& CNcdNodeDbManager::StorageL( 
    const CNcdNodeIdentifier& aIdentifier ) const
    {    
    DLTRACEIN((""));
    DPROFILING_BEGIN( x );
    // This function needs the namespace and uid information.
    // So, check them. Id can be empty because it is not used here.
    if( aIdentifier.NodeNameSpace() == KNullDesC
        || aIdentifier.ClientUid() == TUid::Null() )
        {
        DLERROR(("Empty ns or uid fields given"));

        User::Leave( KErrArgument );
        }    

    DLINFO(("Identifier was not empty"));

    return iStorageManager.CreateOrGetStorageL(
        aIdentifier.ClientUid().Name(),
        aIdentifier.NodeNameSpace() );
    }

