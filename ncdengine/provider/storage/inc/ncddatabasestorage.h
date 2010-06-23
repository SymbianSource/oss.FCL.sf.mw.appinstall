/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef M_NCDDATABASESTORAGE_H
#define M_NCDDATABASESTORAGE_H


#include <e32base.h>
#include <d32dbms.h>


class MNcdDatabaseStorage;
class MNcdStorageItem;
class RNcdDatabaseItems;

/**
 * Interface for database storages
 * @ Improve comments
 */

class MNcdDatabaseStorage
{
public:

    /**
     * Database size information
     */
    typedef RDbDatabase::TSize TNcdDatabaseSize;
        
public: 


    /**
     * Returns ponter to storage item. If storage item doesn't exist
     * it is created. Ownerhip of the storage item is not transferred
     * to caller.
     */
    virtual MNcdStorageItem* StorageItemL( const TDesC& aUid, TInt aType ) = 0;

    /**
     * Gets all storage items from storage. This method doens't create
     * new items to storage but returns all items being currently stored
     * to it.
     *
     * @param aItems An array where to storage items will be added.
     */
    virtual void StorageItemsL( RPointerArray<MNcdStorageItem>& aItems ) = 0;
    
    
    virtual void RemoveItemsL( 
        const RArray<RNcdDatabaseItems>& aDoNotRemoveItems ) = 0;
        

    /**
     * Begins transaction into storage.
     */
    virtual void Begin() = 0;

    /**
     * Commits changes to storage.
     */
    virtual void CommitL() = 0;

    /**
     * Discards changes and restores storage to state it was before
     * transaction was started.
     */
    virtual void Rollback() = 0;

    /**
     * Ensures that given storage item exists in storage.
     *
     * NOTE: It is possible that storage item is created and exists
     * but it is not stored to storage.
     *
     * @param aStorageItem The item to be checked
     * @return Boolean value indicating whether the item exsists in
     * storage.
     */
    virtual TBool ItemExistsInStorageL( MNcdStorageItem& aStorageItem ) = 0;

    /**
     * Ensures that given storage item exists in storage.
     *
     * NOTE: It is possible that storage item is created and exists
     * but it is not stored to storage.
     *
     * @param aUid Uid of the item to be checked
     * @param aType Type of the item to be checked
     * @return Boolean value indicating whether the item exsists in
     * storage.
     */
    virtual TBool ItemExistsInStorageL( const TDesC& aUid, TInt aType ) = 0;


    /**
     * Storage UID getter
     * @return Storage UID
     */
    virtual const TDesC& Uid() const = 0;
    
    
    /**
     * Database size getter
     */
    virtual TNcdDatabaseSize Size() const = 0;
    
    
    /**
     * Commits and compacts the db
     */
    virtual void Compact() = 0;
    
protected:
    
    virtual ~MNcdDatabaseStorage()
        {
        }
    

};


#endif // M_NCDDATABASESTORAGE_H
