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


#ifndef MCSTORAGEMANAGER_H
#define MCSTORAGEMANAGER_H

#include "ncddatabasestorage.h"
#include "ncdstoragebase.h"


#include <e32base.h>
#include <bamdesca.h>


class CNcdStorageBase;



class CNcdDbStorageManager : 
    public CNcdStorageBaseListener,
    public MNcdDatabaseStorage
{
    class CNcdDbStorageManagerData;

    public:

         static CNcdDbStorageManager* NewLC(
            RFs& aFs,
            const TDesC& aStorageUid,
            const TDesC& aStorageFolder,
            const TDesC& aStorageName );

         ~CNcdDbStorageManager();


    public: // New methods
         

         TInt CommitCachedItems();

         void RollbackItems();


    public: // From MNcdDatabaseStorage


        /**
         * Returns ponter to storage item. If storage item doesn't exist
         * it is created. Ownerhip of the storage item is not transferred
         * to caller.
         */
         MNcdStorageItem* StorageItemL( const TDesC& aUid, TInt aType );

        /**
         * Gets all storage items from storage. This method doens't create
         * new items to storage but returns all items being currently stored
         * to it.
         *
         * @param aItems An array where to storage items will be added.
         */
         void StorageItemsL( RPointerArray<MNcdStorageItem>& aItems );


        void RemoveItemsL( 
            const RArray<RNcdDatabaseItems>& aDoNotRemoveItems );


        /**
         * Begins transaction into storage.
         */
         void Begin();

        /**
         * Commits changes to storage.
         */
         void CommitL();

        /**
         * Discards changes and restores storage to state it was before
         * transaction was started.
         */
         void Rollback();

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
         TBool ItemExistsInStorageL( MNcdStorageItem& aStorageItem );

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
         TBool ItemExistsInStorageL( const TDesC& aUid, TInt aType );


        /**
         * Storage UID getter
         * @return Storage UID
         */
        const TDesC& Uid() const;


        /**
         * @see MNcdDatabaseStorage::Size()
         */
        TNcdDatabaseSize Size() const;
        

        /**
         * @see MNcdDatabaseStorage::Compact()
         */
        void Compact();
        
        
    public: // From CNcdStorageBaseListener

         void CacheOpened();

         void CacheReadyL();

         void NotifyRollback();


    private:

        CNcdDbStorageManager();

        void ConstructL(
            RFs& aFs,
            const TDesC& aStorageUid,
            const TDesC& aStorageFolder,
            const TDesC& aStorageName );


    private:

        class CNcdDbStorageManagerData : public CBase
            {
            public:

                CNcdDbStorageManagerData( CNcdDbStorageManager* aRoot );

                ~CNcdDbStorageManagerData();

            public:

                void StorageOpened();


                void StorageClosedL();

                TInt OpenStorages();

                void Rollback();


            private:

                CNcdDbStorageManager*      iRoot;                
                
                TInt                    iOpenStorages;

            };


    private:

        CNcdDbStorageManagerData*           iData;

        CNcdStorageBase*                    iStorage;

        TInt                                iLocallyOpenCount;

        TBool                               iForceOpen;

};


#endif
