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


#ifndef MCSTORAGE_H
#define MCSTORAGE_H


#include <e32base.h>
#include <bamdesca.h>
#include "ncddatabasestorage.h"
#include "ncdstorageitem.h"

class CNcdStorageItem;
class MNcdStorageDataItem;
class CNcdStorageBaseListener;
class CNcdStorageItemIdentifier;



class CNcdStorageBase : public CBase
{

    public:


         virtual ~CNcdStorageBase();


    public:

        /**
        * @note Only ENcdStorageTypeDatabase is currently supported
        */
         static CNcdStorageBase* NewL(
            RFs& aFs,
            const TDesC& aUid,
            const TDesC& aStorageFolder,
            const TDesC& aStorageName );


    public:


         CNcdStorageItem* StorageItemL( const TDesC& aUid, TInt aType );

         void StorageItemsL( RPointerArray<MNcdStorageItem>& aItems );


        void RemoveItemsL( 
            const RArray<RNcdDatabaseItems>& aDoNotRemoveItems );

         void OpenItemL( CNcdStorageItem* aItem );

         void SaveItemL( CNcdStorageItem* aItem );

         void RemoveItemL( CNcdStorageItem* aItem );

         void ReadDataL( CNcdStorageItem* aItem, MNcdStorageDataItem& aDataItem );

         void WriteDataL( CNcdStorageItem* aItem, MNcdStorageDataItem& aDataItem );

         TBool ItemsOpen() const;

         HBufC8* GetDataL( CNcdStorageItem* aItem );


         TInt Commit();

         void RollbackItems();


    public:


         TInt Open();

         void Close();

         TBool IsOpen() const;

         const TDesC& Uid() const;

         const TDesC& StorageFolder() const;

         const TDesC& Name() const;

         void SetListener( CNcdStorageBaseListener* aListener );

         void Rollback();

         TBool ItemExistsInStorageL( const TDesC& aUid, TInt aType );


         virtual void Compact() = 0;
    public:


        virtual RDbDatabase::TSize Size() const = 0;

        
    protected:


         CNcdStorageBase( HBufC* aUid, HBufC* aStorageFolder, HBufC* aName );


    protected:

         CNcdStorageItem* FindStorageItem( const TDesC& aUid, TInt aType );


    protected:

        virtual CNcdStorageItem* CreateStorageItemLC( const TDesC& aUid, TInt aType ) = 0;

        virtual TBool ItemExistsL( const TDesC& aUid, TInt aType ) = 0; 

        virtual void DoOpenItemL( CNcdStorageItem* aItem ) = 0;

        virtual void DoRollback() = 0;

        virtual void DoRemoveItemL( CNcdStorageItem* aItem ) = 0;

        virtual void DoReadDataL( CNcdStorageItem* aItem, MNcdStorageDataItem& aDataItem ) = 0;

        virtual void DoWriteDataL( CNcdStorageItem* aItem, MNcdStorageDataItem& aDataItem ) = 0;

        virtual TInt DoCommit() = 0;

        virtual void GetAllItemsFromStorageL( RPointerArray<CNcdStorageItemIdentifier>& aItems ) = 0;

        virtual HBufC8* DoGetDataL( CNcdStorageItem* aItem ) = 0;
        
        virtual void DoRemoveItemsL( 
            const RArray<RNcdDatabaseItems>& aDoNotRemoveItems ) = 0;


    protected:

        virtual void DoOpenL() = 0;

        virtual void DoClose() = 0;


    protected:

         HBufC8* GetAllDataL( RReadStream& aStream );
        
        
         HBufC8* GetAllDataLC( RReadStream& aStream );


    protected:



        HBufC*                              iUid;

        HBufC*                              iStorageFolder;

        HBufC*                              iName;

        TBool                               iOpen;


    private:

        RPointerArray<CNcdStorageItem>       iItems;

        RPointerArray<CNcdStorageItem>       iOpenItems;

        RPointerArray<CNcdStorageItem>       iRemovedItems;

        CNcdStorageBaseListener*             iListener;
};


class CNcdStorageItem : public CBase, public MNcdStorageItem
    {

    public:

        /**
        * @note Uids must be unique in a type
        */
        static CNcdStorageItem* NewL( CNcdStorageBase* aStorage, const TDesC& aUid, TInt aType );

        ~CNcdStorageItem();

    public:


        void SetOpen( TBool aOpen );


    public: // From MNcdStorageItem


        virtual void OpenL();

        /**
         * Marks the item as saved but doesn't commit it to storage until
         * all open items are saved
         */
        virtual void SaveL();

        /**
         * @note Rollbacks all open & saved items
         */
        virtual void Rollback();

        virtual void RemoveFromStorageL();

        /**
         * Read data item from the storage
         */
        virtual void ReadDataL();

        /**
         * Write data item to the storage
         */
        virtual void WriteDataL();

        virtual TBool IsOpen() const;

        virtual const TDesC& Uid() const;

        virtual TInt Type() const;

        virtual void SetDataItem( MNcdStorageDataItem* aDataItem );


        virtual HBufC8* GetDataLC();


    protected:

        CNcdStorageItem( CNcdStorageBase* aStorage );

        void ConstructL( const TDesC& aUid, TInt aType );


    private:

        CNcdStorageBase*                 iStorage;

        MNcdStorageDataItem*         iDataItem;

        CNcdStorageItemIdentifier*   iIdentifier;

        TBool                       iOpen;

    };
    

class CNcdStorageItemIdentifier : public CBase
    {
    public:
        static CNcdStorageItemIdentifier* NewLC( const TDesC8& aId, TInt aType );
        static CNcdStorageItemIdentifier* NewL( const TDesC16& aId, TInt aType );
        static CNcdStorageItemIdentifier* NewLC( const TDesC16& aId, TInt aType );

        ~CNcdStorageItemIdentifier();

    public:

        const TDesC& Id() const;

        TInt Type() const;


    private:

        void ConstructL( const TDesC16& aId );
        void ConstructL( const TDesC8& aId );

        CNcdStorageItemIdentifier( TInt aType );


    private:


        HBufC*          iId;

        TInt            iType;
    };


class CNcdStorageBaseListener : public CBase
    {

     public:

        virtual void CacheOpened() = 0;

        virtual void CacheReadyL() = 0;

        virtual void NotifyRollback() = 0;

    };


#endif
