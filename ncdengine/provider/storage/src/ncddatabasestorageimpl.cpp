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
* Description:   ?Description
*
*/


#include "ncddatabasestorageimpl.h"

#include <bamdesca.h>
#include <bautils.h>

#include "ncdstoragedataitem.h"
#include "ncdstoragepanics.pan"
#include "ncdpanics.h"
#include "catalogsutils.h"
#include "ncddatabaseitems.h"


//#include "catalogsdebug.h"

// disable logging for this file
#undef DLTRACEIN
#define DLTRACEIN( x )

#undef DLTRACEOUT
#define DLTRACEOUT( x )

#undef DLTRACE
#define DLTRACE( x )

#undef DLINFO
#define DLINFO( x )

#undef DLERROR
#define DLERROR( x )

#undef DASSERT
#define DASSERT( x )


// SQL statement for creation of the database table
_LIT( KNcdSqlCreateStorageTable, "\
 CREATE TABLE storage (\
 row_id COUNTER,\
 item_id LONG VARCHAR,\
 item_type INTEGER,\
 item_data LONG VARBINARY)" );

enum TNcdDatabaseTableColumns
    {
    ENcdDatabaseTableColumnsId = 1,
    ENcdDatabaseTableColumnsItemId,
    ENcdDatabaseTableColumnsItemType,
    ENcdDatabaseTableColumnsItemData
    };


// SQL statement for deleting a row from the database table
_LIT( KNcdSqlRemoveItem, "DELETE FROM storage WHERE row_id = %d" );

// These are used for removing everything else but some certain ids from a db
_LIT( KNcdSqlRemoveItemsStart, "DELETE FROM storage WHERE " );           
_LIT( KNcdSqlRemoveItemType, "NOT ( item_type = %d" );
_LIT( KNcdSqlRemoveItemAnd, " AND " );
_LIT( KNcdSqlRemoveItemStart, " AND ( item_id = '" );        
_LIT( KNcdSqlRemoveItemStartNoType, " NOT (( item_id = '" );        
_LIT( KNcdSqlRemoveItemOrId, "' OR item_id = '" );
_LIT( KNcdSqlRemoveItemEnd, "'))" );
_LIT( KNcdSqlRemoveItemEmptyEnd, ")" );


// Select one item from the database by item id and type
_LIT( KNcdSqlSelectOneRowByIdAndType,  "SELECT * FROM storage WHERE item_id = '%S' AND item_type = %d" );

// Select one item from the database by item key
_LIT( KNcdSqlSelectOneRowByKey,  "SELECT * FROM storage WHERE row_id = %d" );

// Select all items from the database
_LIT( KNcdSqlSelectAll,  "SELECT * FROM storage" );

// File extension for database file
_LIT( KNcdDatabaseFileExtension, "*.db" );

// Amount of disk space in bytes that database is
// allowed to waste before it is compressed
const TInt KNcdMaxDatabaseWastedSpace = 100000;



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdDatabaseStorage::~CNcdDatabaseStorage()
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    DoClose();

    if ( iDatabaseFileName ) 
        {
        DLTRACE(( _L("Database filename: %S"), iDatabaseFileName ));
        }
    delete iDatabaseFileName;
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdDatabaseStorage* CNcdDatabaseStorage::NewL(
    RFs& aFs,
    const TDesC& aUid,
    const TDesC& aStorageFolder,
    const TDesC& aName )
    {
    DLTRACEIN(( _L("Uid: %S, folder: %S, name: %S"), &aUid, &aStorageFolder,
        &aName ));
        
    HBufC* uid = aUid.AllocLC();
    HBufC* storageFolder = aStorageFolder.AllocLC();
    HBufC* name = aName.AllocLC();

    DLTRACE(("Creating CNcdDatabaseStorage"));
    CNcdDatabaseStorage* storage = new (ELeave) CNcdDatabaseStorage( 
        aFs, uid, storageFolder, name );
    DLINFO(("CNcdDatabaseStorage created"));
    CleanupStack::Pop( 3 ); // uid, storageFolder, name
    
    DLTRACE(("storage-ptr: %X", storage));
    DASSERT( storage );
    CleanupStack::PushL( storage );
    DLTRACE(("Calling constructL"));
    storage->ConstructL();
    CleanupStack::Pop( storage );    
    
    return storage;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdDatabaseStorage::CNcdDatabaseStorage( 
    RFs& aFs,
    HBufC* aUid, HBufC* aStorageFolder, HBufC* aName ) :
    CNcdStorageBase( aUid, aStorageFolder, aName ),
    iFs( aFs ),
    iDatabaseFileName( NULL ),
    iUpdated( EFalse ),
    iWastedSpace( 0 ),
    iNewWastedSpace( 0 )
    {    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDatabaseStorage::ConstructL()
    {    
    DLTRACEIN(( _L("StorageFolder: %S, Name: %S"), iStorageFolder, iName ));
    TParse fileName;
    
    fileName.Set( *iStorageFolder, iName, &KNcdDatabaseFileExtension );
    iDatabaseFileName = fileName.FullName().AllocL();
    
    DLTRACEOUT(( _L("Fullname: %S"), iDatabaseFileName ));
    }


// ---------------------------------------------------------------------------
// Compact
// ---------------------------------------------------------------------------
//    
void CNcdDatabaseStorage::Compact()
    {
    DLTRACEIN((""))
    iDb.Compact();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
RDbDatabase::TSize CNcdDatabaseStorage::Size() const
    {
    if( iOpen )
        {
        return iDb.Size();
        }
    else
        {
        User::Panic( KNcdStoragePanic, ENcdStoragePanicStorageNotOpen );
        }
    
    // For compiler
    return RDbDatabase::TSize();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CNcdDatabaseStorage::DoCommit()
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    if( iDb.InTransaction() )
        {
        TRAPD( err, CloseTransactionL( ETrue ) );
        return err;
        }
    else
        {
        DLTRACE(("Nothing to commit"));
        return KErrNone;
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDatabaseStorage::DoOpenL()
    {
    DLTRACEIN(("this-ptr: %X", this));
    //User::LeaveIfError( iFs.Connect() );
    
    DLTRACE((_L("Opening database: %S"), iDatabaseFileName ));
    // Try to open the database
    TInt err = iDb.Open( iFs, *iDatabaseFileName );

    if( err == KErrNone )
        {       
        if ( iDb.IsDamaged() ) 
            {
            DLTRACE(( "DB is damaged, try to recover" ));            
            TInt recoverErr = iDb.Recover();
            if ( recoverErr != KErrNone ) 
                {
                DLTRACE(("DB couldn't be recovered, recreate it"));
                RecreateDatabaseL();
                }
            }
            
        iDb.Compact();        
        }
    else if( err == KErrNotFound ) // If not found, create it
        {
        DLINFO(( "Data base file not found. New one is created." ));
        RecreateDatabaseL();
        }
    else if ( err == KErrCorrupt || err == KErrEof ) 
        {
        DLINFO(("Database corrupted! Create new one"));
        RecreateDatabaseL();
        }
    else
        {
        DLERROR(( "Failed with err: %i", err ));
        User::Leave( err );
        }
    DLTRACEOUT((""));
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDatabaseStorage::DoClose()
    {
    DLTRACEIN(("this-ptr: %X", this));
    if( iOpen && iDb.InTransaction() )
        {
        DLTRACE(("Rollback"));
        iDb.Rollback();
        }

    DLINFO(("Close db"));
    iDb.Close();      
    DLINFO(("db closed"));
  
    iWastedSpace = 0;
    iNewWastedSpace = 0;
    iUpdated = EFalse;

    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdStorageItem* CNcdDatabaseStorage::CreateStorageItemLC( const TDesC& aUid, 
    TInt aType )
    {
    DLTRACEIN(( "this-ptr: %X",this ));
    CNcdDatabaseStorageItem* item = CNcdDatabaseStorageItem::NewLC( this, aUid, 
        aType );    

    // Begin transaction if not already began
    BeginTransactionL();

    RDbView dbView;
    ReadItemDataLC( dbView, aUid, aType, RDbRowSet::EUpdatable );

    
    if( !dbView.FirstL() ) // Item doesn't exist in database. Create it.
        {
        // Add new row
        dbView.InsertL();

        // Write uid to newly created row
        dbView.SetColL( ENcdDatabaseTableColumnsItemId, aUid );

        // Write type to newly created row
        dbView.SetColL( ENcdDatabaseTableColumnsItemType, aType );
        
        // Confirms row addition
        // This doesn't commit row to the database if transaction is started
        // In this case the row is not permanently inserted to database until the
        // last open item is closed and transaction commited.
        dbView.PutL();
        }

    dbView.GetL(); // Get item data
    
     // Read key column from db table and store it to the item
    item->SetDbKey( dbView.ColUint32( ENcdDatabaseTableColumnsId ) );

    CleanupStack::PopAndDestroy( &dbView );    

    return item;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDatabaseStorage::GetAllItemsFromStorageL( 
    RPointerArray<CNcdStorageItemIdentifier>& aItems )
    {
    DLTRACEIN(( "this-ptr: %X",this ));
    CleanupResetAndDestroyPushL( aItems );
    
    RDbView dbView;    
    
    // Read all data from database
    User::LeaveIfError( dbView.Prepare( iDb, TDbQuery( KNcdSqlSelectAll ), 
        RDbRowSet::EReadOnly ) );

    CleanupClosePushL( dbView );
    
    User::LeaveIfError( dbView.EvaluateAll() );

    dbView.BeginningL();

    RDbColReadStream readStream;
    
    TInt count = dbView.CountL( RDbRowSet::EQuick );
    if ( count && count != KDbUndefinedCount ) 
        {
        DLTRACE(("Reserving %d slots for the items array", count ));
        aItems.ReserveL( count );
        }
    // Loop through the whole record set and store
    // item ids and types to array
    while( dbView.NextL() )
        {
        dbView.GetL();

        TInt type = dbView.ColInt( ENcdDatabaseTableColumnsItemType );

        readStream.OpenLC( dbView, ENcdDatabaseTableColumnsItemId );

        HBufC8* id = GetAllDataLC( readStream );
                   
        CNcdStorageItemIdentifier* itemIdentifier = 
            CNcdStorageItemIdentifier::NewLC(
                *id,
                type );

        aItems.AppendL( itemIdentifier );

        CleanupStack::Pop( itemIdentifier );
        CleanupStack::PopAndDestroy( 2, &readStream ); // id, readStream

        }

    CleanupStack::PopAndDestroy( &dbView );
    CleanupStack::Pop( &aItems );

    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC8* CNcdDatabaseStorage::DoGetDataL( CNcdStorageItem* aItem )
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    CNcdDatabaseStorageItem* item = (CNcdDatabaseStorageItem*) aItem;

    HBufC8* dataBuffer = NULL;

    RDbView dbView;
    ReadItemDataLC( dbView, item->DbKey() );

    if( dbView.FirstL() )
        {
        dbView.GetL();

        RDbColReadStream readStream;
        readStream.OpenLC( dbView, ENcdDatabaseTableColumnsItemData );
        dataBuffer = GetAllDataL( readStream );
        CleanupStack::PopAndDestroy( &readStream );
        }
    else
        {
        DLINFO(("Didn't find the item. Leaving with KErrNotFound"));
        // Item not found.
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( &dbView );
    DLTRACEOUT((""));
    return dataBuffer;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDatabaseStorage::DoRemoveItemsL( 
    const RArray<RNcdDatabaseItems>& aDoNotRemoveItems )
    {
    DLTRACEIN((""));
    NCD_ASSERT_ALWAYS( aDoNotRemoveItems.Count(), ENcdPanicInvalidArgument );
        
   
/*
    This is the intended output:
    DELETE FROM storage WHERE 
        NOT ( item_type = 5 AND ( item_id = 'id1' OR item_id = 'id2' ))
        AND NOT ( item_type = 6 AND ( item_id = 'id3' OR item_id = 'id2' )) 
        AND NOT ( item_id = 'id4' )
*/    
    
    TInt sqlStatementSize = KNcdSqlRemoveItemsStart().Length();
        
    TInt count = aDoNotRemoveItems.Count();
    while ( count-- ) 
        {
        sqlStatementSize += 
            CalculateSqlRemovalLength( aDoNotRemoveItems[ count ] ) +
            KNcdSqlRemoveItemAnd().Length();
        
        }
    
    DLTRACE(("Max length of sql statement: %d", sqlStatementSize ));
    // SQL statement
    HBufC* sqlBuf = HBufC::NewLC( sqlStatementSize );
    TPtr sql = sqlBuf->Des();
    
    // DELETE FROM storage WHERE
    sql.Append( KNcdSqlRemoveItemsStart );    
           
    count = aDoNotRemoveItems.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        DASSERT( aDoNotRemoveItems[i].iType != KErrNotFound ||
                 aDoNotRemoveItems[i].Uids().MdcaCount() );
                 
        if ( aDoNotRemoveItems[i].iType != KErrNotFound ) 
            {            
            // NOT ( item_type = x
            sql.AppendFormat( KNcdSqlRemoveItemType, aDoNotRemoveItems[i].iType );
            }
            
        if ( aDoNotRemoveItems[i].Uids().MdcaCount() ) 
            {
            const MDesCArray& uids = aDoNotRemoveItems[i].Uids();
            TInt uidCount = uids.MdcaCount();
            
            // append first item
            // AND ( item_id = '
            if ( aDoNotRemoveItems[i].iType != KErrNotFound ) 
                {                
                sql.Append( KNcdSqlRemoveItemStart );
                }
            else 
                {
                // NOT (( item_id = '
                sql.Append( KNcdSqlRemoveItemStartNoType );
                }
                
            sql.Append( uids.MdcaPoint( 0 ) );
            DLTRACE(( _L("Added: %S"), &uids.MdcaPoint( 0 ) ));
            
            for ( TInt u = 1; u < uidCount; ++u ) 
                {
                // ' OR item_id = '
                sql.Append( KNcdSqlRemoveItemOrId ); 
                sql.Append( uids.MdcaPoint( u ) );
                DLTRACE(( _L("Added: %S"), &uids.MdcaPoint( u ) ));
                }
            // '))    
            sql.Append( KNcdSqlRemoveItemEnd );
            }
        else 
            {
            DLTRACE(("No uids for type: %d", aDoNotRemoveItems[i].iType ));
            
            // )
            sql.Append( KNcdSqlRemoveItemEmptyEnd );
            }
        
        // and needed for others but the last        
        if ( i < count - 1 ) 
            {
            // AND
            sql.Append( KNcdSqlRemoveItemAnd );        
            }            
        }

    DLINFO(( _L("Sql: %S"), sqlBuf ));
    
    // Begin transaction if not already began
    TBool newTransaction = BeginTransactionL();

    DLINFO((" DB size before remove: %d", iDb.Size() ));
    
    // Deletes row from the database
    // Returns number of deleted rows if success
    // Otherwise returns error code
    TInt err = iDb.Execute( sql );

    CleanupStack::PopAndDestroy( sqlBuf );  

    DLTRACE(("Removed %d rows", err));
    CloseTransactionL();
    
    DLTRACE(("Compacting"));
    iDb.Compact();
    DLINFO(("DB size after remove: %d", iDb.Size() ));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CNcdDatabaseStorage::CalculateSqlRemovalLength( 
    const RNcdDatabaseItems& aItem ) const
    {
    DLTRACEIN(("type: %d, uids: %d", aItem.iType, aItem.Uids().MdcaCount() ));
    const TInt KMax32BitIntLength = 11;
    TInt size = KNcdSqlRemoveItemType().Length() + 
                KMax32BitIntLength + 
                KNcdSqlRemoveItemStart().Length() +
                KNcdSqlRemoveItemEnd().Length();
    
    const MDesCArray& uids( aItem.Uids() );
    TInt count = uids.MdcaCount();            
    while ( count-- ) 
        {
        size += uids.MdcaPoint( count ).Length() +
            KNcdSqlRemoveItemOrId().Length();    
        }
    
    DLTRACEOUT(("length: %d", size ));
    return size;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CNcdDatabaseStorage::BeginTransactionL()
    {
    DLTRACEIN(( "this-ptr: %X",this ));
    // Begins transaction if not already begun
    if( iDb.InTransaction() )
        {
        DLTRACE(("Old transaction"));
        return EFalse; // Transaction already exists
        }
    else
        {
        DLTRACE(("Starting new transaction"));    
        User::LeaveIfError( iDb.Begin() ); // Try to begin new transaction
        DLTRACE(("New transaction started"));    
        return ETrue;
        }
    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CNcdDatabaseStorage::CloseTransactionL( TBool aForceClose )
    {
    DLTRACEIN(( "this-ptr: %X",this ));
    if( !aForceClose && ItemsOpen() ) // Do not commit, items open
        {
        DLTRACE(("Items open, not committing"));
        return EFalse;
        }
    else
        {
        DLTRACE(("Commit"));
        TInt err = iDb.Commit();
        if( err != KErrNone )
            {
            DLERROR(( "Commit failed with: %d", err ));
            iUpdated = EFalse;
            iDb.Rollback();
            User::Leave( err );
            }

        if( iUpdated )
            {
            DLTRACE(( "Updated" ));
            iUpdated = EFalse;
            iWastedSpace += iNewWastedSpace;
            iNewWastedSpace = 0;

            if( iWastedSpace > KNcdMaxDatabaseWastedSpace )
                {
                DLINFO(("WastedSpace %i > MaxWastedSpace %i -> Compact",
                    iWastedSpace, KNcdMaxDatabaseWastedSpace ));
                iDb.Compact();
                iWastedSpace = 0;
                }

            }

        return ETrue;
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CNcdDatabaseStorage::ItemExistsL( const TDesC& aUid, TInt aType )
    {
    DLTRACEIN((_L("Uid: %S, type: %d"), &aUid, aType ));    
    
    RDbView dbView;
    ReadItemDataLC( dbView, aUid, aType );

    TBool exists = ( dbView.CountL() > 0 );

    CleanupStack::PopAndDestroy( &dbView );

    return exists;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDatabaseStorage::DoOpenItemL( CNcdStorageItem* /*aItem*/ )
    {
    BeginTransactionL();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDatabaseStorage::DoRollback()
    {
    DLTRACEIN((""));
    if( iDb.InTransaction() )
        {
        DLINFO(("Rollbacking"));
        iDb.Rollback();
        }

    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDatabaseStorage::DoRemoveItemL( CNcdStorageItem* aItem )
    {
    DLTRACEIN((""));
    CNcdDatabaseStorageItem* item = (CNcdDatabaseStorageItem*)aItem;

    // SQL statement
    HBufC* sqlBuf = HBufC::NewLC( KNcdSqlRemoveItem().Length() + 8 );
    TPtr sql = sqlBuf->Des();
    sql.Format( KNcdSqlRemoveItem, item->DbKey() );

    // Begin transaction if not already began
    TBool newTransaction = BeginTransactionL();

    // Deletes row from the database
    // Returns number of deleted rows if success
    // Otherwise returns error code
    TInt err = iDb.Execute( sql );

    CleanupStack::PopAndDestroy( sqlBuf );  

    DLTRACE(("Removed %d rows", err));
    if( err != 1 ) // Error occured
        {
        DLTRACE(("Error! Rollback and leave"));
        if( newTransaction )
            {
            iDb.Rollback();
            }

        User::Leave( KErrNotFound );
        }
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDatabaseStorage::DoReadDataL( CNcdStorageItem* aItem, 
    MNcdStorageDataItem& aDataItem )
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    CNcdDatabaseStorageItem* item = (CNcdDatabaseStorageItem*)aItem;

    DASSERT( aItem );
    RDbView dbView;
    
    ReadItemDataLC( dbView, item->DbKey() );

    if( dbView.FirstL() )
        {
        dbView.GetL();

        RDbColReadStream readStream;
        readStream.OpenLC( dbView, ENcdDatabaseTableColumnsItemData );

        aDataItem.InternalizeL( readStream );
        CleanupStack::PopAndDestroy( &readStream );
        }
    else
        {
        // Item not found.
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( &dbView );
    DLTRACEOUT(( "" ));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDatabaseStorage::DoWriteDataL( CNcdStorageItem* aItem, 
    MNcdStorageDataItem& aDataItem )
    {
    DLTRACEIN(( "this-ptr: %X", this ));

    DASSERT( aItem );
    CNcdDatabaseStorageItem* item = (CNcdDatabaseStorageItem*)aItem;

    // Begin transaction if not alredy begun
    BeginTransactionL();

    RDbView dbView;
    ReadItemDataLC( dbView, item->DbKey(), RDbRowSet::EUpdatable );

    if( dbView.FirstL() )
        {
        dbView.UpdateL();

        TInt wastedSpace = dbView.ColSize( ENcdDatabaseTableColumnsId ) +
            dbView.ColSize( ENcdDatabaseTableColumnsItemId ) +
            dbView.ColSize( ENcdDatabaseTableColumnsItemType ) +
            dbView.ColSize( ENcdDatabaseTableColumnsItemData );

        RDbColWriteStream stream;
        stream.OpenL( dbView, ENcdDatabaseTableColumnsItemData );
        CleanupClosePushL( stream );        

        aDataItem.ExternalizeL( stream );

        CleanupStack::PopAndDestroy( &stream );

        dbView.PutL();
        iUpdated = ETrue;
        iNewWastedSpace += wastedSpace;

        }
    else
        {
        DLERROR(("Item not found"));    
        // Item not found
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( &dbView );
    DLTRACEOUT(( "" ));
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDatabaseStorage::ReadItemDataLC( RDbView& aItemData, 
    const TDesC& aUid, TInt aType, RDbRowSet::TAccess aAccess )
    {
    DLTRACEIN(("this-ptr: %X", this));    
    
    // Create SQL statement
    HBufC* sqlBuf = HBufC::NewLC( KNcdSqlSelectOneRowByIdAndType().Length() 
        + aUid.Length() + 7 );        
            
    TPtr sql = sqlBuf->Des();
    sql.Format( KNcdSqlSelectOneRowByIdAndType, &aUid, aType );
           

    User::LeaveIfError( aItemData.Prepare( iDb, TDbQuery( sql ), aAccess ) );
    
    CleanupStack::PopAndDestroy( sqlBuf );
    CleanupClosePushL( aItemData );    

    User::LeaveIfError( aItemData.EvaluateAll() );
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDatabaseStorage::ReadItemDataLC( RDbView& aItemData, TUint32 aKey, 
    RDbRowSet::TAccess aAccess )
    {    
    DLTRACEIN(("this-ptr: %X", this));
    
    // Create SQL statement
    HBufC* sqlBuf = HBufC::NewLC( KNcdSqlSelectOneRowByKey().Length() + 9 );
    TPtr sql = sqlBuf->Des();
    sql.Format( KNcdSqlSelectOneRowByKey, aKey );

    User::LeaveIfError( aItemData.Prepare( iDb, TDbQuery( sql ), aAccess ) );
    CleanupStack::PopAndDestroy( sqlBuf );
    CleanupClosePushL( aItemData );

    User::LeaveIfError( aItemData.EvaluateAll() );
    DLTRACEOUT(("no error"));
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDatabaseStorage::RecreateDatabaseL() 
    {
    DLTRACEIN((""));
        
    // Delete existing database file. Ignoring errors as database file
    // may not be present.
    iDb.Close();
    BaflUtils::DeleteFile( iFs, *iDatabaseFileName );

    // Trying to create the database
    User::LeaveIfError( iDb.Create( iFs, *iDatabaseFileName ) );
    User::LeaveIfError( iDb.Execute( KNcdSqlCreateStorageTable ) );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdDatabaseStorageItem* CNcdDatabaseStorageItem::NewLC(
            CNcdStorageBase* aStorage,
            const TDesC& aUid,
            TInt aType,
            TUint32 aDbKey )
    {
    CNcdDatabaseStorageItem* item = new (ELeave) CNcdDatabaseStorageItem( 
        aStorage, aDbKey );
        
    CleanupStack::PushL( item );
    item->ConstructL( aUid, aType );    
    return item;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdDatabaseStorageItem::CNcdDatabaseStorageItem( CNcdStorageBase* aStorage, 
    TUint32 aDbKey ) :
    CNcdStorageItem( aStorage ),
        iDbKey( aDbKey )
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdDatabaseStorageItem::~CNcdDatabaseStorageItem()
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdDatabaseStorageItem::SetDbKey( TUint32 aDbKey )
    {
    iDbKey = aDbKey;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint32 CNcdDatabaseStorageItem::DbKey() const
    {
    return iDbKey;
    }

