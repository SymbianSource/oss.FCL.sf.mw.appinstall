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


#ifndef MCDATABASESTORAGE_H
#define MCDATABASESTORAGE_H


#include <e32base.h>
#include <f32file.h>
#include <d32dbms.h> // RDbNamedDatabase

#include "ncdstoragebase.h"

/**
 * 
 */
class CNcdDatabaseStorage : public CNcdStorageBase
{

    friend CNcdStorageBase* CNcdStorageBase::NewL( 
        RFs&, const TDesC&, const TDesC&, 
        const TDesC& );


    public: // Destructor

         virtual ~CNcdDatabaseStorage();


    protected: // Construction

         static CNcdDatabaseStorage* NewL(
            RFs& aFs,
            const TDesC& aUid,
            const TDesC& aStorageFolder,
            const TDesC& aName );

         CNcdDatabaseStorage( 
            RFs& aFs,
            HBufC* aUid, 
            HBufC* aStorageFolder, 
            HBufC* aName );

         void ConstructL();


    public: // From CNcdStorageBase

         virtual void Compact();
         
         virtual RDbDatabase::TSize Size() const;
        
         virtual TInt DoCommit();

        
    protected: // From CNcdStorageBase

         virtual CNcdStorageItem* CreateStorageItemLC( const TDesC& aUid, TInt aType );


         virtual TBool ItemExistsL( const TDesC& aUid, TInt aType );

         virtual void DoOpenItemL( CNcdStorageItem* aItem );

         virtual void DoRollback();

         virtual void DoRemoveItemL( CNcdStorageItem* aItem );

         virtual void DoReadDataL( CNcdStorageItem* aItem, MNcdStorageDataItem& aDataItem );

         virtual void DoWriteDataL( CNcdStorageItem* aItem, MNcdStorageDataItem& aDataItem );

         virtual void DoOpenL();

         virtual void DoClose();

         virtual void GetAllItemsFromStorageL( RPointerArray<CNcdStorageItemIdentifier>& aItems );

         virtual HBufC8* DoGetDataL( CNcdStorageItem* aItem );

         virtual void DoRemoveItemsL( 
            const RArray<RNcdDatabaseItems>& aDoNotRemoveItems );


    private: // New methods

        TInt CalculateSqlRemovalLength( 
            const RNcdDatabaseItems& aItem ) const;


        inline TBool BeginTransactionL();

        inline TBool CloseTransactionL( TBool aForceClose = EFalse );

        inline void ReadItemDataLC(
            RDbView& aItemData,
            const TDesC& aUid,
            TInt aType,
            RDbRowSet::TAccess aAccess = RDbRowSet::EReadOnly );

        inline void ReadItemDataLC(
            RDbView& aItemData,
            TUint32 aKey,
            RDbRowSet::TAccess aAccess = RDbRowSet::EReadOnly );
            
        void RecreateDatabaseL();


    private:


        RFs&                    iFs;

        HBufC*                  iDatabaseFileName;

        RDbNamedDatabase        iDb;

        TBool                   iUpdated;

        TInt                    iWastedSpace;

        TInt                    iNewWastedSpace;
    };


class CNcdDatabaseStorageItem : public CNcdStorageItem
    {

    public:


        static CNcdDatabaseStorageItem* NewLC(
            CNcdStorageBase* aStorage,
            const TDesC& aUid,
            TInt aType,
            TUint32 aDbKey = 0 );


        virtual ~CNcdDatabaseStorageItem();


    public:


        void SetDbKey( TUint32 aDbKey );

        TUint32 DbKey() const;


    protected:

        CNcdDatabaseStorageItem( CNcdStorageBase* aStorage, TUint32 aDbKey );

    private:

        TUint32            iDbKey;
    };

#endif
