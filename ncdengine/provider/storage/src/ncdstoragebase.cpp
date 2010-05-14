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


#include "ncdstoragebase.h"

#include <s32buf.h>

#include "ncddatabasestorageimpl.h"
#include "ncdstoragedataitem.h"
#include "ncdstoragepanics.pan"
#include "catalogsutils.h"

#include "catalogsdebug.h"


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdStorageBase::~CNcdStorageBase()
    {
    DLTRACEIN(( "" ));
    delete iUid;
    delete iStorageFolder;
    delete iName;
    iOpenItems.Reset();
    iRemovedItems.Reset();
    iItems.ResetAndDestroy();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdStorageBase::CNcdStorageBase( HBufC* aUid, HBufC* aStorageFolder, 
        HBufC* aName ) :
    iUid( aUid ),
    iStorageFolder( aStorageFolder ),
    iName( aName ),
    iOpen( EFalse ),
    iListener( NULL )
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdStorageBase* CNcdStorageBase::NewL(
    RFs& aFs,
    const TDesC& aUid,
    const TDesC& aStorageFolder,
    const TDesC& aStorageName )
    {
    DLTRACE( ( _L("Path: %S"), &aStorageFolder ) );
    CNcdStorageBase* storage = 
        storage = CNcdDatabaseStorage::NewL( 
            aFs, aUid, aStorageFolder, 
            aStorageName ); 

    return storage;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CNcdStorageBase::Uid() const
    {
    DASSERT( iUid );
    DLTRACE( ( _L("Uid: %S"), iUid ) );
    
    return *iUid;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CNcdStorageBase::StorageFolder() const
    {
    DASSERT( iStorageFolder );
    return *iStorageFolder;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CNcdStorageBase::Name() const
    {
    DASSERT( iName );
    return *iName;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CNcdStorageBase::Open()
    {
    DLTRACEIN(("this: %x", this));
    if( IsOpen() )
        {
        User::Panic( KNcdStoragePanic, ENcdStoragePanicAlreadyOpen );
        }
    
    TRAPD( err, DoOpenL() );

    if( err == KErrNone )
        {
        iOpen = ETrue;
        }

    return err;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageBase::Close()
    {
    DLTRACEIN(("this: %x", this));
    if( IsOpen() )
        {
        DoClose();
        iOpen = EFalse;
        }

    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CNcdStorageBase::IsOpen() const
    {
    return iOpen;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdStorageItem* CNcdStorageBase::StorageItemL( const TDesC& aUid, TInt aType )
    {
    DLTRACEIN( (_L("aUid: %S, aType: %d"), &aUid, aType ) );
    DPROFILING_BEGIN( x );
    CNcdStorageItem* item = FindStorageItem( aUid, aType );

    if ( !item )
        {
        TRAPD( err,
            {
            // Create item
            item = CreateStorageItemLC( aUid, aType );            

            // Store item to array
            iItems.InsertInAddressOrderL( item );
            DLTRACE(("Items: %d", iItems.Count() ));
            CleanupStack::Pop( item );
            });

        if( err == KErrNone )
            {
            iListener->CacheOpened();
            TRAP( err, iListener->CacheReadyL() );
            }

        if( err != KErrNone )
            {
            if( !ItemsOpen() )
                {
                DoRollback();
                }

            User::Leave( err );
            }
        }

    DPROFILING_END( x );
    DLTRACEOUT((""));
    return item;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageBase::StorageItemsL( RPointerArray<MNcdStorageItem>& aItems )
    {
    RPointerArray<CNcdStorageItemIdentifier> items;
    CleanupResetAndDestroyPushL( items );

    // Get storage item identifiers from storage
    GetAllItemsFromStorageL( items );

    // Get storage items
    TInt count = items.Count();
    aItems.ReserveL( aItems.Count() + count );
    
    for( TInt i = 0; i < count; i++ )
        {
        aItems.AppendL( StorageItemL( items[i]->Id(), items[i]->Type() ) );
        }

    CleanupStack::PopAndDestroy( &items );    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageBase::RemoveItemsL( 
    const RArray<RNcdDatabaseItems>& aDoNotRemoveItems )
    {
    DLTRACEIN((""));
    DoRemoveItemsL( aDoNotRemoveItems );
    
    // reset arrays so that removed items are not left hanging aroung
    iOpenItems.Reset();
    iRemovedItems.Reset();
    iItems.ResetAndDestroy();
    }



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageBase::OpenItemL( CNcdStorageItem* aItem )
    {
    DLTRACEIN(("this: %x", this));
    iOpenItems.ReserveL( iOpenItems.Count() + 1 );

    DoOpenItemL( aItem );

    // Same item can't be opened multiple times (CNcdStorageItem::OpenL)
    // so we don't have to worry about adding it many times to the open item
    // array. Also we can't run out of memory because we already reserved 
    // memory for the item so we can ignore the error
    iOpenItems.InsertInAddressOrder( aItem );
    
    if( iListener && iOpenItems.Count() == 1 )
        {
        iListener->CacheOpened();
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageBase::SaveItemL( CNcdStorageItem* aItem )
    {
    DLTRACEIN(("this: %x", this));
    TInt index = iOpenItems.FindInAddressOrder( aItem );

    if( index < 0 )
        {
        User::Panic( KNcdStoragePanic, ENcdStoragePanicItemNotOpen );
        }

    iOpenItems.Remove( index );

    if( iListener && iOpenItems.Count() == 0 )
        {
        iListener->CacheReadyL();
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageBase::RollbackItems()
    {
    DLTRACEIN((""));
    Rollback();

    iListener->NotifyRollback();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CNcdStorageBase::Commit()
    {
    DLTRACEIN((""));
    TInt err = DoCommit();

    if( err == KErrNone )
        {
        TInt count = iRemovedItems.Count();
        TInt index = 0;
        for( TInt i = 0; i < count; i++ )
            {
            index = iItems.FindInAddressOrder( iRemovedItems[i] );
            if( index >= 0 )
                {
                iItems.Remove( index );
                }
            DLTRACE(("Items: %d", iItems.Count() ));
            delete iRemovedItems[i];
            iRemovedItems[i] = NULL;
            }
        }

    iRemovedItems.Reset();    
    return err;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageBase::RemoveItemL( CNcdStorageItem* aItem )
    {
    DLTRACEIN(("this: %x", this));
    TInt insertError( iRemovedItems.InsertInAddressOrder( aItem ) );
    if ( insertError == KErrAlreadyExists )
        {
        DLINFO(("Remove item was already in the array."));
        return;
        }
    User::LeaveIfError( insertError );

    TRAPD( err, DoRemoveItemL( aItem ) );

    if( err != KErrNone )
        {
        TInt index = iRemovedItems.FindInAddressOrder( aItem );
        if( index >= 0 )
            {
            iRemovedItems.Remove( index );
            }
        DLERROR(("Error: %d occurred when removing", err));
        User::Leave( err );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageBase::Rollback()
    {
    DoRollback();

    TInt count = iOpenItems.Count();
    for( TInt i = 0; i < count; i++ )
        {
        iOpenItems[i]->SetOpen( EFalse );
        }

    iOpenItems.Reset();
    iRemovedItems.Reset();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageBase::ReadDataL( CNcdStorageItem* aItem, 
    MNcdStorageDataItem& aDataItem )
    {
    DoReadDataL( aItem, aDataItem );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageBase::WriteDataL( CNcdStorageItem* aItem, 
    MNcdStorageDataItem& aDataItem )
    {
    DoWriteDataL( aItem, aDataItem );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CNcdStorageBase::ItemsOpen() const
    {
    DLTRACEIN(("this: %x, items open: %d", this, iOpenItems.Count() ));
    return ( iOpenItems.Count() > 0 );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC8* CNcdStorageBase::GetDataL( CNcdStorageItem* aItem )
    {
    return DoGetDataL( aItem );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdStorageItem* CNcdStorageBase::FindStorageItem( const TDesC& aUid, 
    TInt aType )
    {
    TInt count = iItems.Count();
    CNcdStorageItem* item = NULL;

    for( TInt i = 0; i < count; i++ )
        {
        item = iItems[i];

        if( item->Type() == aType && 
            item->Uid().Compare( aUid ) == 0 )
            {
            return item;
            }
        }    
        
    return NULL;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageBase::SetListener( CNcdStorageBaseListener* aListener )
    {
    iListener = aListener;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CNcdStorageBase::ItemExistsInStorageL( const TDesC& aUid, TInt aType )
    {
    return ItemExistsL( aUid, aType );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC8* CNcdStorageBase::GetAllDataLC( RReadStream& aStream )
    {
    MStreamBuf* streamBuf = aStream.Source();
    TInt size = streamBuf->SizeL();

    HBufC8* dataBuffer = HBufC8::NewLC( size );
    TPtr8 data = dataBuffer->Des();

    aStream.ReadL( data, size );

    return dataBuffer;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC8* CNcdStorageBase::GetAllDataL( RReadStream& aStream )
    {
    HBufC8* dataBuffer = GetAllDataLC( aStream );
    CleanupStack::Pop( dataBuffer );    
    return dataBuffer;
    }



// ********************************
//         CNcdStorageItem
// ********************************


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdStorageItem* CNcdStorageItem::NewL( CNcdStorageBase* aStorage, 
    const TDesC& aUid, TInt aType )
    {
    CNcdStorageItem* item = new (ELeave) CNcdStorageItem( aStorage );
    CleanupStack::PushL( item );
    item->ConstructL( aUid, aType );
    CleanupStack::Pop( item );
    return item;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdStorageItem::~CNcdStorageItem()
    {
    DLTRACEIN((""));
    delete iIdentifier;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdStorageItem::CNcdStorageItem( CNcdStorageBase* aStorage ) :
    iStorage( aStorage ),
    iOpen( EFalse )
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageItem::ConstructL( const TDesC& aUid, TInt aType  )
    {
    DLTRACEIN(("this: %x", this));
    iIdentifier = CNcdStorageItemIdentifier::NewL( aUid, aType );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageItem::SetOpen( TBool aOpen )
    {
    iOpen = aOpen;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CNcdStorageItem::IsOpen() const
    {
    return iOpen;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CNcdStorageItem::Uid() const
    {
    return iIdentifier->Id();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CNcdStorageItem::Type() const
    {
    return iIdentifier->Type();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageItem::OpenL()
    {
    DLTRACEIN(("this: %x", this));
    if( iOpen )
        {
        User::Panic( KNcdStoragePanic, ENcdStoragePanicItemAlreadyOpen );
        }

    iStorage->OpenItemL( this );
    iOpen = ETrue;
    DLTRACEOUT(("Opened %x", this));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageItem::SaveL()
    {
    DLTRACEIN(("this: %x", this));
    if( !iOpen )
        {
        User::Panic( KNcdStoragePanic, ENcdStoragePanicItemNotOpen );
        }

    TRAPD( err, iStorage->SaveItemL( this ) );
    if( err != KErrNone )
        {
        Rollback();
        User::Leave( err );
        }
    iOpen = EFalse;
    DLTRACEOUT(("closed: %x", this));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageItem::Rollback()
    {
    DLTRACEIN(("this: %x", this));
    if( iOpen )
        {
        iOpen = EFalse;
        iStorage->RollbackItems();
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageItem::RemoveFromStorageL()
    {
    DLTRACEIN(("this: %x", this));
    if( iOpen )
        {
        User::Panic( KNcdStoragePanic, ENcdStoragePanicItemAlreadyOpen );
        }

    iStorage->RemoveItemL( this );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageItem::SetDataItem( MNcdStorageDataItem* aDataItem )
    {
    iDataItem = aDataItem;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageItem::ReadDataL()
    {
    iStorage->ReadDataL( this, *iDataItem );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageItem::WriteDataL()
    {
    DLTRACEIN(("this: %x", this));
    if( iOpen )
        {
        TRAPD( err, iStorage->WriteDataL( this, *iDataItem ) );
        if ( err != KErrNone ) 
            {
            Rollback();
            User::Leave( err );
            }
        }
    else
        {
        User::Panic( KNcdStoragePanic, ENcdStoragePanicItemNotOpen );
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC8* CNcdStorageItem::GetDataLC()
    {
    HBufC8* data = iStorage->GetDataL( this );
    CleanupStack::PushL( data );
    return data;
    }


// ********************************
//     CNcdStorageItemIdentifier
// ********************************

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdStorageItemIdentifier* CNcdStorageItemIdentifier::NewLC( 
    const TDesC8& aId, TInt aType )
    {
    CNcdStorageItemIdentifier* identifier = new (ELeave) 
        CNcdStorageItemIdentifier( aType );
    CleanupStack::PushL( identifier );
    identifier->ConstructL( aId );
    return identifier;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdStorageItemIdentifier* CNcdStorageItemIdentifier::NewL( 
    const TDesC16& aId, TInt aType )
    {
    CNcdStorageItemIdentifier* identifier = 
        CNcdStorageItemIdentifier::NewLC( aId, aType );
    CleanupStack::Pop( identifier );
    return identifier;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdStorageItemIdentifier* CNcdStorageItemIdentifier::NewLC( 
    const TDesC16& aId, TInt aType )
    {
    CNcdStorageItemIdentifier* identifier = new (ELeave) 
        CNcdStorageItemIdentifier( aType );
    CleanupStack::PushL( identifier );
    identifier->ConstructL( aId );
    return identifier;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdStorageItemIdentifier::CNcdStorageItemIdentifier( TInt aType ) :
    iType( aType )
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdStorageItemIdentifier::~CNcdStorageItemIdentifier()
    {
    delete iId;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CNcdStorageItemIdentifier::Id() const
    {
    return *iId;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CNcdStorageItemIdentifier::Type() const
    {
    return iType;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdStorageItemIdentifier::ConstructL( const TDesC16& aId )
    {
    iId = aId.AllocL();
    }


void CNcdStorageItemIdentifier::ConstructL( const TDesC8& aId )
    {  
    TInt length = aId.Length();

    if( length % 2 != 0 )
      {
      User::Panic( KNcdStoragePanic, ENcdStoragePanicIdDescAlign );
      }

    iId = HBufC16::NewL( length / 2 );
    iId->Des().Copy( (const TUint16*)aId.Ptr(), length / 2 );
    }
