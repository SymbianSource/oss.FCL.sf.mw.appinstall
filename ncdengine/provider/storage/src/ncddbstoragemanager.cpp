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


#include "ncddbstoragemanager.h"

#include "ncdstoragebase.h"
#include "ncdstoragepanics.pan"

#include "catalogsdebug.h"

#include <f32file.h>
#include <bautils.h>

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdDbStorageManager* CNcdDbStorageManager::NewLC(
    RFs& aFs,
    const TDesC& aStorageUid,
    const TDesC& aStorageFolder,
    const TDesC& aStorageName )
    {
    CNcdDbStorageManager* manager = new (ELeave) CNcdDbStorageManager;
    CleanupStack::PushL( manager );
    manager->ConstructL( aFs, aStorageUid, aStorageFolder, aStorageName );
    return manager;
    }



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdDbStorageManager::~CNcdDbStorageManager()
    {
    DLTRACEIN(("this-ptr: %X", this));        
    delete iStorage;
    delete iData;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdDbStorageManager::CNcdDbStorageManager() :
    iLocallyOpenCount( 0 ),
    iForceOpen( EFalse )
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDbStorageManager::ConstructL( 
    RFs& aFs,
    const TDesC& aStorageUid, 
    const TDesC& aStorageFolder, const TDesC& aStorageName )
    {
    DLTRACEIN((""));    
    iStorage = CNcdStorageBase::NewL( aFs, aStorageUid, aStorageFolder, 
        aStorageName );

    DLTRACE(("CNcdStorageBase created"));
    iStorage->SetListener( this );

    User::LeaveIfError( iStorage->Open() );

    // Create data object
    iData = new (ELeave) CNcdDbStorageManagerData( this );

    DLTRACEOUT( ("") );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
MNcdStorageItem* CNcdDbStorageManager::StorageItemL( const TDesC& aUid, 
    TInt aType )
    {
    return (MNcdStorageItem*)iStorage->StorageItemL( aUid, aType );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDbStorageManager::StorageItemsL( 
    RPointerArray<MNcdStorageItem>& aItems )
    {
    iStorage->StorageItemsL( aItems );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDbStorageManager::RemoveItemsL( 
    const RArray<RNcdDatabaseItems>& aDoNotRemoveItems )
    {
    iStorage->RemoveItemsL( aDoNotRemoveItems );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDbStorageManager::Begin()
    {
    iForceOpen = ETrue;
    iData->StorageOpened();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDbStorageManager::CommitL()
    {
    DLTRACEIN((""));
    iForceOpen = EFalse;
    iData->StorageClosedL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDbStorageManager::Rollback()
    {
    iForceOpen = EFalse;
    iData->Rollback();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CNcdDbStorageManager::ItemExistsInStorageL( 
    MNcdStorageItem& aStorageItem )
    {
    return ItemExistsInStorageL( aStorageItem.Uid(), aStorageItem.Type() );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CNcdDbStorageManager::ItemExistsInStorageL( const TDesC& aUid, 
    TInt aType )
    {
    return iStorage->ItemExistsInStorageL( aUid, aType );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
MNcdDatabaseStorage::TNcdDatabaseSize CNcdDbStorageManager::Size() const
    {
    return iStorage->Size();
    }


// ---------------------------------------------------------------------------
// Compact
// ---------------------------------------------------------------------------
//
void CNcdDbStorageManager::Compact() 
    {
    DLTRACEIN((""));
    CommitCachedItems();
    iStorage->Compact();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDbStorageManager::CacheOpened()
    {
    DLTRACEIN(("this: %x", this));
    iLocallyOpenCount++;
    iData->StorageOpened();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDbStorageManager::CacheReadyL()
    {
    DLTRACEIN(("this: %x", this));
    iLocallyOpenCount--;
    iData->StorageClosedL();

    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CNcdDbStorageManager::CommitCachedItems()
    {
    DLTRACEIN(("this: %x", this));
    iForceOpen = EFalse;

    TInt err = iStorage->Commit();

    if( err == KErrNone )
        {
        iLocallyOpenCount = 0;
        }
    DLTRACEOUT(("err: %d", err));
    return err;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDbStorageManager::NotifyRollback()
    {
    iLocallyOpenCount--;
    iData->Rollback();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDbStorageManager::RollbackItems()
    {
    iLocallyOpenCount = 0;
    iForceOpen = EFalse;

    iStorage->Rollback();

    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdDbStorageManager::CNcdDbStorageManagerData::CNcdDbStorageManagerData( 
    CNcdDbStorageManager* aRoot ) :
    iRoot( aRoot ),
    iOpenStorages( 0 )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdDbStorageManager::CNcdDbStorageManagerData::~CNcdDbStorageManagerData()
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDbStorageManager::CNcdDbStorageManagerData::StorageOpened()
    {
    DLTRACEIN(("this: %x, open storages: %d", this, iOpenStorages + 1 ));
    iOpenStorages++;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDbStorageManager::CNcdDbStorageManagerData::StorageClosedL()
    {
    DLTRACEIN(("OpenStorages: %d", iOpenStorages));
    if ( iOpenStorages != 0 ) 
        {
        iOpenStorages--;
        }
    else
        {
        DLTRACE(("All storages were already closed by tried to close one again"));
        }
        
    if( iOpenStorages == 0 )
        {
        DLTRACE(("Committing cached items"));
        TInt err = iRoot->CommitCachedItems();
        DLINFO(( "Commit error: %d", err ));
        User::LeaveIfError( err );
        }

    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CNcdDbStorageManager::CNcdDbStorageManagerData::OpenStorages()
    {
    DLTRACEIN(( "this: %x, open storages: %d", this, iOpenStorages ));
    return iOpenStorages;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDbStorageManager::CNcdDbStorageManagerData::Rollback()
    {
    if( iOpenStorages > 0 )
        {
        iRoot->RollbackItems();
        iOpenStorages = 0;
        }
    }


// ---------------------------------------------------------------------------
// UID getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdDbStorageManager::Uid() const
    {
    return iStorage->Uid();
    }


