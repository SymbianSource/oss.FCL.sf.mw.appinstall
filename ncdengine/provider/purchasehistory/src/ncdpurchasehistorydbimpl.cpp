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


#include <eikenv.h>
#include <f32file.h>
#include <bautils.h>
#include <s32mem.h>

#include "ncdpurchasehistorydbimpl.h"
#include "ncdpurchasedownloadinfo.h"
#include "ncdpurchaseinstallinfo.h"
#include "ncdpurchasehistoryfilter.h"
#include "ncdnodefunctionids.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "catalogsbigdes.h"
#include "catalogsconstants.h"
#include "catalogsutils.h"
#include "ncdutils.h"

#include "catalogsdebug.h"

// Create a purchases table.
// This column order is always used when inserting data or in queries.
// Column numbering is stored in CNcdPurchaseHistoryDb::PurchaseColumns.
_LIT( KSqlCreatePurchasesTable, "\
 CREATE TABLE purchases ( \
 purchase_id COUNTER,\
 event_id UNSIGNED INTEGER,\
 client_uid INTEGER,\
 namespace LONG VARCHAR,\
 entity_id LONG VARCHAR,\
 item_name LONG VARCHAR,\
 item_purpose UNSIGNED INTEGER,\
 catalog_name LONG VARCHAR,\
 download_info LONG VARBINARY,\
 purchase_option_id LONG VARCHAR,\
 purchase_option_name LONG VARCHAR,\
 purchase_option_price LONG VARCHAR,\
 final_price LONG VARCHAR,\
 payment_method_name LONG VARCHAR,\
 purchase_time BIGINT,\
 downloaded_files LONG VARBINARY,\
 file_install_infos LONG VARBINARY,\
 icon LONG VARBINARY,\
 downloadaccesspoint LONG VARCHAR,\
 description LONG VARCHAR,\
 version LONG VARCHAR,\
 server_uri LONG VARCHAR,\
 item_type INTEGER, \
 total_content_size INTEGER, \
 origin_node_id LONG VARCHAR, \
 last_operation_time BIGINT, \
 last_operation_error_code INTEGER, \
 has_icon INTEGER, \
 attributes LONG VARBINARY )" );


// Create a event counter table.
_LIT( KSqlCreateEventCounterTable,
    "CREATE TABLE event_counter_table ( event_counter UNSIGNED INTEGER )" );

// Event counter column number.
const TInt KEventCounterColumnNumber = 1;


// Select purchases.
_LIT( KSqlPurchasesStart,
     "SELECT purchase_id FROM purchases WHERE event_id >= " );
_LIT( KSqlPurchasesNamespace, " AND namespace = '" );
_LIT( KSqlPurchasesEntityId, " AND entity_id = '" );
_LIT( KSqlPurchasesClientUid, " AND ( client_uid = " );
_LIT( KSqlPurchasesOrClientUid, " OR client_uid = " );
_LIT( KSqlPurchasesEndClientUid, " )" );
_LIT( KSqlPurchasesEnd, "'" );
_LIT( KSqlPurchasesOrderNewestFirst, " ORDER BY event_id DESC" );
_LIT( KSqlPurchasesOrderOldestFirst, " ORDER BY event_id ASC" );

// Select purchase to be updated.
_LIT( KSqlPurchaseUpdateStart,
     "SELECT * FROM purchases WHERE client_uid = " );
_LIT( KSqlPurchasesUpdateNamespace, " AND namespace = '" );
_LIT( KSqlPurchasesUpdateEntityId, "' AND entity_id = '" );
_LIT( KSqlPurchasesUpdatePurchaseTime, "' AND purchase_time = " );
_LIT( KSqlPurchasesUpdateEnd, "" );

// Select purchase.
_LIT( KSqlPurchaseByPurchaseIdStart,
    "SELECT * FROM purchases WHERE purchase_id = " );
_LIT( KSqlPurchaseByPurchaseIdEnd, "" );

// Select all purchases.
_LIT( KSqlPurchasesAllNone, "SELECT * FROM purchases" );

// Delete specified purchase event.
_LIT( KSqlPurchasesDeleteStart,
    "DELETE FROM purchases WHERE purchase_id = " );
_LIT( KSqlPurchasesDeleteEnd,   "" );


// Select current event count.
_LIT( KSqlCurrentEventCount,
    "SELECT event_counter FROM event_counter_table" );

// Delete event count from event count table.
_LIT( KSqlEventCountDelete, "DELETE FROM event_counter_table" );

// "-2147483648"
const TInt KMaxLengthOfInt = 11;

// "-9223372036854775808"
const TInt KMaxLengthOfTint64 = 20;

static void AppendWithQuotesDuplicatedL(
    CCatalogsBigDes* aOutput,
    const TDesC& aInput )
    {
    _LIT( KQuote, "'" );
    
    if ( aOutput == NULL || &aInput == NULL )
        {
        return;
        }

    TPtrC in = aInput.Mid( 0 );
    while ( in.Length() > 0 )
        {
        TInt i = in.Locate( '\'' );
        if ( i == KErrNotFound )
            {
            // Quote not found, all done
            aOutput->AppendL( in );
            return;
            }
        aOutput->AppendL( in.Mid( 0, i + 1 ) );
        // Append extra quote
        aOutput->AppendL( KQuote );
        if ( i + 1 < in.Length() )
            {
            // Get end part of the input descriptor
            in.Set( in.Mid( i + 1 ) );
            }
        else
            {
            // End of the input descriptor reached
            return;
            }
        
        }
    }

static HBufC* ReadLongTextColumnL( RDbRowSet aView, TDbColNo aCol )
    {
    TInt length = aView.ColLength( aCol );
    if ( length == 0 )
        {
        return KNullDesC().Alloc();
        }
    RDbColReadStream readStream;
    readStream.OpenLC( aView, aCol );
    HBufC* result = HBufC::NewLC( length );
    TPtr resultPtr = result->Des();
    readStream.ReadL( resultPtr, length );
    readStream.Close();
    CleanupStack::Pop( result );
    CleanupStack::Pop(); //readStream
    return result;
    }

static void WriteLongTextColumnL(
    RDbRowSet aView,
    TDbColNo aCol,
    const TDesC& aValue )
    {
    RDbColWriteStream writeStream;
    writeStream.OpenLC( aView, aCol );
    if ( &aValue )
        {
        writeStream.WriteL( aValue );
        }
    else
        {
        writeStream.WriteL( KNullDesC );
        }
    writeStream.Close();
    CleanupStack::Pop( &writeStream );
    }

static HBufC8* ReadLongTextColumn8L( RDbRowSet aView, TDbColNo aCol )
    {
    TInt length = aView.ColLength( aCol );
    if ( length == 0 )
        {
        return KNullDesC8().Alloc();
        }
    RDbColReadStream readStream;
    readStream.OpenLC( aView, aCol );
    HBufC8* result = HBufC8::NewLC( length );
    TPtr8 resultPtr = result->Des();
    readStream.ReadL( resultPtr, length );
    readStream.Close();
    CleanupStack::Pop( result );
    CleanupStack::Pop(); //readStream
    return result;
    }

static void WriteLongTextColumn8L(
    RDbRowSet aView,
    TDbColNo aCol,
    const TDesC8& aValue )
    {
    RDbColWriteStream writeStream;
    writeStream.OpenLC( aView, aCol );
    if ( &aValue )
        {
        writeStream.WriteL( aValue );
        }
    else
        {
        writeStream.WriteL( KNullDesC8 );
        }
    writeStream.Close();
    CleanupStack::Pop( &writeStream );
    }


CNcdPurchaseHistoryDb* CNcdPurchaseHistoryDb::NewL(
    const TDesC& aDbFilename )
    {
    CNcdPurchaseHistoryDb* self = NewLC( aDbFilename );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPurchaseHistoryDb* CNcdPurchaseHistoryDb::NewLC(
    const TDesC& aDbFilename )
    {
    CNcdPurchaseHistoryDb* self =
        new ( ELeave ) CNcdPurchaseHistoryDb();
    CleanupClosePushL( *self );
    self->ConstructL( aDbFilename );
    return self;    
    }

CNcdPurchaseHistoryDb::~CNcdPurchaseHistoryDb()
    {
    iDatabase.Close();
    delete iDbFilename;
    iFs.Close();
    }

void CNcdPurchaseHistoryDb::ReceiveMessage(
    MCatalogsBaseMessage* aMessage,
    TInt aFunctionNumber )
    {
    DLTRACEIN((""));

    DASSERT( aMessage );
    
    // Now, we can be sure that rest of the time iMessage exists.
    // This member variable is set for the CounterPartLost function.
    iMessage = aMessage;
        
    TInt trapError( KErrNone );
        
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdPurchaseHistorySavePurchase:
            DLTRACE(("Insert purchase event"));
            TRAP( trapError, SavePurchaseRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdPurchaseHistorySavePurchaseWithOldIcon:
            DLTRACE(("Insert purchase event with old icon"));
            TRAP( trapError, SavePurchaseRequestL( *aMessage, EFalse ) );
            break;
            
        case NcdNodeFunctionIds::ENcdPurchaseHistoryRemovePurchase:
            DLTRACE(("Remove purchase"));
            TRAP( trapError, RemovePurchaseRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdPurchaseHistoryGetPurchaseIds:
            DLTRACE(("Get purchase IDs"));
            TRAP( trapError, GetPurchaseIdsRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdPurchaseHistoryGetPurchase:
            DLTRACE(("Get purchase"));
            TRAP( trapError, GetPurchaseRequestL( *aMessage, ETrue ) );
            break;
            
        case NcdNodeFunctionIds::ENcdPurchaseHistoryGetPurchaseNoIcon:
            DLTRACE(("Get purchase"));
            TRAP( trapError, GetPurchaseRequestL( *aMessage, EFalse ) );
            break;
            
        case NcdNodeFunctionIds::ENcdPurchaseHistoryEventCount:
            DLTRACE(("Get purchase event count"));
            TRAP( trapError, EventCountRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdRelease:
            DLTRACE(("Release purchase history"));
            ReleaseRequest( *aMessage );
            break;
            
        default:
            break;
        }

    if ( trapError != KErrNone )
        {
        // Because something went wrong the complete has not been
        // yet called for the message.
        // So, inform the client about the error.
        DLTRACE(("ERROR, Complete and release %d", trapError));
        
        aMessage->CompleteAndRelease( trapError );
        }

    // Because the message should not be used after this, set it NULL.
    // So, CounterPartLost function will know that no messages are
    // waiting the response at the moment.
    iMessage = NULL;        
            
    DLTRACEOUT((""));
    }

void CNcdPurchaseHistoryDb::CounterPartLost(
    const MCatalogsSession& aSession )
    {
    // This function may be called whenever -- when the message is waiting
    // response or when the message does not exist.
    // iMessage may be NULL here, because in the end of the
    // ReceiveMessage it is set to NULL. The life time of the message
    // ends shortly after CompleteAndRelease is called.
    if ( iMessage != NULL )
        {
        iMessage->CounterPartLost( aSession );
        }    
    }

void CNcdPurchaseHistoryDb::ConstructL( const TDesC& aDbFilename )
    {
    if ( aDbFilename == KNullDesC )
        {
        User::Leave( KErrBadName );
        }
        
    User::LeaveIfError( iFs.Connect() );

    // Set database name.
    iDbFilename = aDbFilename.AllocL();
    BaflUtils::EnsurePathExistsL( iFs, *iDbFilename );
    TInt err = iDatabase.Open( iFs, *iDbFilename );

    if ( err == KErrNotFound || err == KErrCorrupt )
        {
        // Delete existing database file. This should happen only if
        // database file is corrupted. Ignoring errors as database file
        // may not be present.
        BaflUtils::DeleteFile( iFs, *iDbFilename );

        // See if the database can be found from another drive
        TFindFile finder( iFs );
        CDir* dir = NULL;

        err = finder.FindWildByDir( *iDbFilename, KNullDesC, dir );

        if ( err == KErrNone )
            {
            // Move old database to correct folder and try to open it
            CleanupStack::PushL( dir );
            CFileMan* fileman = CFileMan::NewL( iFs );
            CleanupStack::PushL( fileman );
            User::LeaveIfError( fileman->Move( finder.File(),
                                               *iDbFilename,
                                               CFileMan::EOverWrite ) );
            CleanupStack::PopAndDestroy( fileman );
            CleanupStack::PopAndDestroy( dir );

            // NOTE: Error ignored. If folder contains files,
            // it will not be deleted.
            iFs.RmDir( finder.File() );

            err = iDatabase.Open( iFs, *iDbFilename );
            if ( err == KErrNotFound || err == KErrCorrupt )
                {
                BaflUtils::DeleteFile( iFs, *iDbFilename );
                // Trying to create the database
                User::LeaveIfError( iDatabase.Create( iFs, *iDbFilename ) );
                User::LeaveIfError(
                    iDatabase.Execute( KSqlCreatePurchasesTable ) );
                User::LeaveIfError(
                    iDatabase.Execute( KSqlCreateEventCounterTable ) );
                }
            else if ( err != KErrNone )
                {
                User::Leave( err );
                }
            }
        else
            {
            // Trying to create the database
            User::LeaveIfError( iDatabase.Create( iFs, *iDbFilename ) );
            User::LeaveIfError(
                iDatabase.Execute( KSqlCreatePurchasesTable ) );
            User::LeaveIfError(
                iDatabase.Execute( KSqlCreateEventCounterTable ) );
            }
        }
    else if ( err != KErrNone )
        {
        User::Leave( err );
        }

    if ( iDatabase.IsDamaged() )
        {
        // Database has been partly damaged. Try to recover it.
        iDatabase.Recover();
        }
    }

CNcdPurchaseHistoryDb::CNcdPurchaseHistoryDb()
    : CCatalogsCommunicable()
    {    
    }

void CNcdPurchaseHistoryDb::SavePurchaseL(
    CNcdPurchaseDetails& aPurchase, 
    TBool aSaveIcon )
    {
    DLTRACEIN((""));
    // Ensure that purchase history handling bugs in PCD! don't crash the client :)
    ValidatePurchaseDetailsL( aPurchase );
    
    CCatalogsBigDes* sqlStatement = CCatalogsBigDes::NewLC();
    
    sqlStatement->AppendL( KSqlPurchaseUpdateStart );
    TBuf<KMaxLengthOfInt> intBuf;
    intBuf.Num( aPurchase.ClientUid().iUid );
    sqlStatement->AppendL( intBuf );
    sqlStatement->AppendL( KSqlPurchasesUpdateNamespace );
    AppendWithQuotesDuplicatedL( sqlStatement, aPurchase.Namespace() );
    sqlStatement->AppendL( KSqlPurchasesUpdateEntityId );
    AppendWithQuotesDuplicatedL( sqlStatement, aPurchase.EntityId() );
    sqlStatement->AppendL( KSqlPurchasesUpdatePurchaseTime );
    TBuf<KMaxLengthOfTint64> bigIntBuf;
    bigIntBuf.Num( aPurchase.PurchaseTime().Int64() );
    sqlStatement->AppendL( bigIntBuf );
    sqlStatement->AppendL( KSqlPurchasesUpdateEnd );
    
    TUint newEventCount = EventCountL() + 1;

    RDbView view;
    CleanupClosePushL( view );

    HBufC* sqlStatementBuf = sqlStatement->DesLC();
    User::LeaveIfError(
        view.Prepare( iDatabase, TDbQuery( *sqlStatementBuf ) ) );
    CleanupStack::PopAndDestroy( sqlStatementBuf );

    User::LeaveIfError( view.EvaluateAll() );
    view.FirstL();

    if ( view.AtRow() )
        {
        UpdatePurchaseL( view, aPurchase, newEventCount, aSaveIcon );
        }
    else
        {
        view.Close();
        NewPurchaseL( aPurchase, newEventCount );
        }
        
    // Set new purchase count
    SetEventCountL( newEventCount );

    CleanupStack::PopAndDestroy( &view );
    CleanupStack::PopAndDestroy( sqlStatement );
    DLTRACEOUT((""));
    }

void CNcdPurchaseHistoryDb::RemovePurchaseL( TUint aPurchaseId )
    {
    if ( ! PurchaseExistsL( aPurchaseId ) )
        {
        // Purchase didn't exist in the database.
        User::Leave( KErrNotFound );
        }

    TInt maxLength = KSqlPurchasesDeleteStart().Length() + KMaxLengthOfInt
        + KSqlPurchasesDeleteEnd().Length();

    HBufC* sqlStatement = HBufC::NewLC( maxLength );
    TPtr statementPtr( sqlStatement->Des() );

    statementPtr.Copy( KSqlPurchasesDeleteStart );
    statementPtr.AppendNum( aPurchaseId );
    statementPtr.Append( KSqlPurchasesDeleteEnd );

    // Remove purchase from the database using event ID.
    User::LeaveIfError( iDatabase.Execute( *sqlStatement ) );
    // Remove all unnecessary data from the database.
    User::LeaveIfError( iDatabase.Compact() );

    CleanupStack::PopAndDestroy( sqlStatement );
    }

RArray<TUint> CNcdPurchaseHistoryDb::PurchaseIdsL(
    const CNcdPurchaseHistoryFilter& aFilter,
    const TSortingOrder aSortingOrder )
    {
    CCatalogsBigDes* sqlStatement = CCatalogsBigDes::NewLC();

    // Construct the SQL query.
    sqlStatement->AppendL( KSqlPurchasesStart );
    TBuf<KMaxLengthOfInt> eventId;
    eventId.Num( aFilter.EventId() );
    sqlStatement->AppendL( eventId );
    
    if ( aFilter.Namespace().Compare( KNullDesC ) != 0 )
        {
        sqlStatement->AppendL( KSqlPurchasesNamespace );
        AppendWithQuotesDuplicatedL( sqlStatement, aFilter.Namespace() );
        sqlStatement->AppendL( KSqlPurchasesEnd );
        }
        
    if ( aFilter.EntityId().Compare( KNullDesC ) != 0 )
        {
        sqlStatement->AppendL( KSqlPurchasesEntityId );
        AppendWithQuotesDuplicatedL( sqlStatement, aFilter.EntityId() );
        sqlStatement->AppendL( KSqlPurchasesEnd );
        }
    
    if ( aFilter.ClientUids().Count() > 0 )
        {
        TArray< TUid > uids = aFilter.ClientUids();
        TInt count = uids.Count();
        sqlStatement->AppendL( KSqlPurchasesClientUid );
        for ( TInt i = 0; i < count; i++ )
            {
            TBuf<KMaxLengthOfInt> clientUid;
            clientUid.Num( uids[i].iUid );
            sqlStatement->AppendL( clientUid );
            if ( i + 1 < count )
                {
                // More UIDs in the filter.
                sqlStatement->AppendL( KSqlPurchasesOrClientUid );
                }
            }
        sqlStatement->AppendL( KSqlPurchasesEndClientUid );
        }

    switch ( aSortingOrder )
        {
        case CNcdPurchaseHistoryDb::ENewestFirst:
            sqlStatement->AppendL( KSqlPurchasesOrderNewestFirst );
            break;
        case CNcdPurchaseHistoryDb::EOldestFirst:
            sqlStatement->AppendL( KSqlPurchasesOrderOldestFirst );
            break;
        case CNcdPurchaseHistoryDb::ENone:
        default:
            break;
        }

    RDbView view;
    CleanupClosePushL( view );

    HBufC* sqlStatementBuf = sqlStatement->DesLC();
    User::LeaveIfError(
        view.Prepare( iDatabase, TDbQuery( *sqlStatementBuf ) ) );
    CleanupStack::PopAndDestroy( sqlStatementBuf );

    User::LeaveIfError( view.EvaluateAll() );
    view.FirstL();

    RArray<TUint> purchaseIds;
    CleanupClosePushL( purchaseIds );

    while( view.AtRow() )
        {
        view.GetL();
        purchaseIds.Append(
            view.ColUint32( CNcdPurchaseHistoryDb::EPurchaseId ) );
        view.NextL();
        }

    CleanupStack::Pop( &purchaseIds );
    CleanupStack::PopAndDestroy( &view );
    CleanupStack::PopAndDestroy( sqlStatement );
    
    return purchaseIds;
    }

CNcdPurchaseDetails* CNcdPurchaseHistoryDb::PurchaseL( 
    TUint aPurchaseId,
    TBool aLoadIcon )
    {
    TInt maxLength = KSqlPurchaseByPurchaseIdStart().Length()
        + KMaxLengthOfInt
        + KSqlPurchaseByPurchaseIdEnd().Length();

    HBufC* sqlStatement = HBufC::NewLC( maxLength );
    TPtr statementPtr( sqlStatement->Des() );

    statementPtr.Copy( KSqlPurchaseByPurchaseIdStart );
    statementPtr.AppendNum( aPurchaseId );
    statementPtr.Append( KSqlPurchaseByPurchaseIdEnd );

    RDbView view;
    CleanupClosePushL( view );

    User::LeaveIfError(
        view.Prepare( iDatabase, TDbQuery( *sqlStatement ) ) );

    User::LeaveIfError( view.EvaluateAll() );
    view.FirstL();

    CNcdPurchaseDetails* details = NULL;

    if ( view.AtRow() )
        {
        view.GetL();
        
        details = CNcdPurchaseDetails::NewLC();
        
        details->SetClientUid( TUid::Uid( view.ColInt32( EClientUid ) ) );
        details->SetNamespace( ReadLongTextColumnL( view, ENamespace ) );
        details->SetEntityId( ReadLongTextColumnL( view, EEntityId ) );
        details->SetItemName( ReadLongTextColumnL( view, EItemName ) );
        details->SetItemPurpose( view.ColUint32( EItemPurpose ) );
        details->SetCatalogSourceName(
            ReadLongTextColumnL( view, ECatalogName ) );

        // Get download info buffer from database
        HBufC8* downloadBuf = ReadLongTextColumn8L( view, EDownloadInfo );
        CleanupStack::PushL( downloadBuf );

        if ( downloadBuf->Length() > 0 )
            {
            // There is some data.
            RDesReadStream readStream( *downloadBuf );
            CleanupClosePushL( readStream );

            // Get count of download infos.
            TInt count = readStream.ReadInt32L();
            for ( TInt i = 0; i < count; i++ )
                {
                // Internalize download infos from the buffer.

                CNcdPurchaseDownloadInfo* info =
                    CNcdPurchaseDownloadInfo::NewLC();

                info->InternalizeL( readStream );
                details->AddDownloadInfoL( info );

                CleanupStack::Pop( info );
                }

            CleanupStack::PopAndDestroy( &readStream );
            }

        CleanupStack::PopAndDestroy( downloadBuf );

        details->SetPurchaseOptionId(
            ReadLongTextColumnL( view, EPurchaseOptionId ) );
        details->SetPurchaseOptionName(
            ReadLongTextColumnL( view, EPurchaseOptionName ) );
        details->SetPurchaseOptionPrice(
            ReadLongTextColumnL( view, EPurchaseOptionPrice ) );
        details->SetFinalPrice( ReadLongTextColumnL( view, EFinalPrice ) );
        details->SetPaymentMethodName(
            ReadLongTextColumnL( view, EPaymentMethodName ) );
        details->SetPurchaseTime( view.ColInt64( EPurchaseTime ) );

        HBufC8* filesBuf = ReadLongTextColumn8L( view, EDownloadedFiles );
        CleanupStack::PushL( filesBuf );
        CDesCArray* downloadedFiles =
            new (ELeave) CDesCArrayFlat( KListGranularity );
        CleanupStack::PushL( downloadedFiles );
        
        if ( filesBuf->Length() > 0 )
            {
            // There is some data.
            RDesReadStream readStream( *filesBuf );
            CleanupClosePushL( readStream );

            TInt filesCount = readStream.ReadInt32L();
            for ( TInt i = 0; i < filesCount; i++ )
                {
                HBufC* buf = NULL;
                InternalizeDesL( buf, readStream );
                CleanupStack::PushL( buf );
                downloadedFiles->AppendL( *buf );
                CleanupStack::PopAndDestroy( buf );
                }

            CleanupStack::PopAndDestroy( &readStream );
            }

        details->SetDownloadedFiles( downloadedFiles );
        CleanupStack::Pop( downloadedFiles );
        CleanupStack::PopAndDestroy( filesBuf );
        
        // Get install info buffer from database
        HBufC8* installBuf = ReadLongTextColumn8L( view, EFileInstallInfos );
        CleanupStack::PushL( installBuf );

        if ( installBuf->Length() > 0 )
            {
            // There is some data.
            RDesReadStream readStream( *installBuf );
            CleanupClosePushL( readStream );

            // Get count of install infos.
            TInt count = readStream.ReadInt32L();
            for ( TInt i = 0; i < count; i++ )
                {
                // Internalize install infos from the buffer.

                CNcdPurchaseInstallInfo* info =
                    CNcdPurchaseInstallInfo::NewLC();

                info->InternalizeL( readStream );
                details->AddInstallInfoL( info );

                CleanupStack::Pop( info );
                }

            CleanupStack::PopAndDestroy( &readStream );
            }

        CleanupStack::PopAndDestroy( installBuf );
        
        if ( aLoadIcon ) 
            {
            DLTRACE(("Loading icon from the purchase history"));            
            details->SetIcon( ReadLongTextColumn8L( view, EIcon ) );
            }
        details->SetDownloadAccessPoint( ReadLongTextColumnL( view, EDownloadAccessPoint ) );
        details->SetDescription( ReadLongTextColumnL( view, EDescription ) );
        details->SetVersion( ReadLongTextColumnL( view, EVersion ) );
        details->SetServerUri( ReadLongTextColumnL( view, EServerUri ) );
        details->SetItemType( (MNcdPurchaseDetails::TItemType)view.ColInt32( EItemType ) );
        details->SetTotalContentSize( view.ColInt32( ETotalContentSize ) );
        details->SetOriginNodeId( ReadLongTextColumnL( view, EOriginNodeId ) );
        details->SetLastOperationTime( view.ColInt64( ELastOperationTime ) );
        details->SetLastOperationErrorCode( view.ColInt32( ELastOperationErrorCode ) );
        details->SetHasIcon( view.ColInt32( EHasIcon ) );
        
        // Get attributes buffer from database
        HBufC8* attributesBuf = ReadLongTextColumn8L( view, EAttributes );
        CleanupStack::PushL( attributesBuf );

        if ( attributesBuf->Length() > 0 )
            {
            // There is some data.
            RDesReadStream readStream( *attributesBuf );
            CleanupClosePushL( readStream );
            details->InternalizeAttributesL( readStream );
            CleanupStack::PopAndDestroy( &readStream );
            }

        CleanupStack::PopAndDestroy( attributesBuf );
        }

    if ( ! details )
        {
        User::Leave( KErrNotFound );
        }

    // Ensure that purchase history handling bugs in PCD! don't crash the client :)
    ValidatePurchaseDetailsL( *details );
    
    CleanupStack::Pop( details );
    CleanupStack::PopAndDestroy( &view );
    CleanupStack::PopAndDestroy( sqlStatement );
    
    return details;
    }

TUint CNcdPurchaseHistoryDb::EventCountL()
    {
    TUint eventCount = 0;

    RDbView view;
    CleanupClosePushL( view );

    User::LeaveIfError( view.Prepare(
        iDatabase,
        TDbQuery( KSqlCurrentEventCount ),
        RDbView::EReadOnly ) );

    view.EvaluateAll();
    view.FirstL();

    if ( view.AtRow() )
        {
        view.GetL();
        eventCount = view.ColUint32( KEventCounterColumnNumber );
        }

    CleanupStack::PopAndDestroy( &view );
    
    return eventCount;
    }

void CNcdPurchaseHistoryDb::SavePurchaseRequestL( MCatalogsBaseMessage& aMessage, 
                                                  TBool aSaveIcon )
    {
    TInt inputLength = aMessage.InputLength();
    User::LeaveIfError( inputLength );
    
    HBufC8* message = HBufC8::NewLC( inputLength );
    TPtr8 ptr = message->Des();
    User::LeaveIfError( aMessage.ReadInput( ptr ) );
    
    CNcdPurchaseDetails* details = CNcdPurchaseDetails::NewLC();
    
    RDesReadStream stream( *message );
    CleanupClosePushL( stream );
    
    details->InternalizeL( stream );
    
    CleanupStack::PopAndDestroy( &stream );
    
    SavePurchaseL( *details, aSaveIcon );
    
    CleanupStack::PopAndDestroy( details );
    CleanupStack::PopAndDestroy( message );
    
    aMessage.CompleteAndRelease( KErrNone );
    }

void CNcdPurchaseHistoryDb::RemovePurchaseRequestL( MCatalogsBaseMessage& aMessage )
    {
    TInt inputLength = aMessage.InputLength();
    User::LeaveIfError( inputLength );
    
    HBufC8* message = HBufC8::NewLC( inputLength );
    TPtr8 ptr = message->Des();
    User::LeaveIfError( aMessage.ReadInput( ptr ) );
    
    TUint purchaseId = Des8ToUint( *message );
    
    CleanupStack::PopAndDestroy( message );
    
    RemovePurchaseL( purchaseId );
    
    aMessage.CompleteAndRelease( KErrNone );
    }

void CNcdPurchaseHistoryDb::GetPurchaseIdsRequestL( MCatalogsBaseMessage& aMessage )
    {
    TInt inputLength = aMessage.InputLength();
    User::LeaveIfError( inputLength );
    
    HBufC8* message = HBufC8::NewLC( inputLength );
    TPtr8 ptr = message->Des();
    User::LeaveIfError( aMessage.ReadInput( ptr ) );
    
    CNcdPurchaseHistoryFilter* filter = CNcdPurchaseHistoryFilter::NewLC();
    
    RDesReadStream stream( *message );
    CleanupClosePushL( stream );

    filter->InternalizeL( stream );
    
    CleanupStack::PopAndDestroy( &stream );
    
    RArray<TUint> purchaseIds = PurchaseIdsL( *filter, ENewestFirst );
    
    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy( message );
    
    CleanupClosePushL( purchaseIds );

    TUint count = purchaseIds.Count();

    message = HBufC8::NewLC( ( count + 1 ) * sizeof(TUint) );
    
    HBufC8* temp = UintToDes8LC( count );
    message->Des().Copy( *temp );
    CleanupStack::PopAndDestroy( temp );
    
    for ( TInt i = 0; i < count; i++ )
        {
        temp = UintToDes8LC( purchaseIds[i] );
        message->Des().Append( *temp );
        CleanupStack::PopAndDestroy( temp );
        }

    aMessage.CompleteAndReleaseL( *message, KErrNone );
    
    CleanupStack::PopAndDestroy( message );
    
    CleanupStack::PopAndDestroy( &purchaseIds );
    }

void CNcdPurchaseHistoryDb::GetPurchaseRequestL( 
    MCatalogsBaseMessage& aMessage,
    TBool aLoadIcon )
    {
    DLTRACEIN(("aLoadIcon: %d", aLoadIcon));
    TInt inputLength = aMessage.InputLength();
    User::LeaveIfError( inputLength );
    
    HBufC8* message = HBufC8::NewLC( inputLength );
    TPtr8 ptr = message->Des();
    User::LeaveIfError( aMessage.ReadInput( ptr ) );

    TUint purchaseId = Des8ToUint( *message );

    CleanupStack::PopAndDestroy( message );

    CNcdPurchaseDetails* details = PurchaseL( purchaseId, aLoadIcon );
    if ( details )
        {
        CleanupStack::PushL( details );
        
        CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
        CleanupStack::PushL( buf );
        
        RBufWriteStream stream( *buf );
        CleanupClosePushL( stream );
        
        details->ExternalizeL( stream );
        
        CleanupStack::PopAndDestroy( &stream );

        aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );
        
        CleanupStack::PopAndDestroy( buf );
        CleanupStack::PopAndDestroy( details );
        }
    else
        {
        aMessage.CompleteAndRelease( KErrNotFound );
        }
    }

void CNcdPurchaseHistoryDb::EventCountRequestL( MCatalogsBaseMessage& aMessage )
    {
    TUint count = EventCountL();
    
    HBufC8* message = UintToDes8LC( count );
    
    aMessage.CompleteAndReleaseL( *message, KErrNone );
    
    CleanupStack::PopAndDestroy( message );
    }

void CNcdPurchaseHistoryDb::ReleaseRequest( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );

    // Send complete information back to proxy.
    aMessage.CompleteAndRelease( KErrNone );
        
    // Remove this object from the session.
    requestSession.RemoveObject( handle );
        
    DLTRACEOUT((""));
    }

void CNcdPurchaseHistoryDb::NewPurchaseL(
    CNcdPurchaseDetails& aPurchase,
    TUint aNewEventCount )
    {
    // Construct database view.
    RDbView view;
    CleanupClosePushL( view );
    User::LeaveIfError( view.Prepare(
        iDatabase,
        TDbQuery( KSqlPurchasesAllNone ),
        RDbView::EInsertOnly ) );
    view.InsertL();
    
    // Set initial value for EHasIcon here. This is needed because in SetPurchaseViewL
    // the flag is never unset after being set to true. The reason for this is that
    // the flag is unkwnown to PCD! Adapter and when it syncs PH data with the engine
    // it cannot set the flag correctly. Therefore the flag is always kept as true 
    // regardless of what PCD! Adapter sends in the sync. This is correct because
    // once set, the icon data is never removed from the purchase detail entry.
    view.SetColL( EHasIcon, EFalse );
    
    SetPurchaseViewL( view, aPurchase, aNewEventCount, ETrue );
    
    
    // Insert purchase into the database
    view.PutL();
    view.Close();
    CleanupStack::PopAndDestroy( &view );
    }


void CNcdPurchaseHistoryDb::UpdatePurchaseL(
    RDbRowSet& aView,
    CNcdPurchaseDetails& aPurchase,
    TUint aNewEventCount,
    TBool aSaveIcon )
    {
    aView.UpdateL();
    
    SetPurchaseViewL( aView, aPurchase, aNewEventCount, aSaveIcon );

    // Update purchase details.
    aView.PutL();
    }


void CNcdPurchaseHistoryDb::SetPurchaseViewL(
    RDbRowSet& aView,
    CNcdPurchaseDetails& aPurchase,
    TUint aNewEventCount,
    TBool aSaveIcon )
    {
    aView.SetColL( EEventId, aNewEventCount );
    aView.SetColL( EClientUid, aPurchase.ClientUid().iUid );
    WriteLongTextColumnL( aView, ENamespace, aPurchase.Namespace() );
    WriteLongTextColumnL( aView, EEntityId, aPurchase.EntityId() );
    WriteLongTextColumnL( aView, EItemName, aPurchase.ItemName() );
    aView.SetColL( EItemPurpose, aPurchase.ItemPurpose() );
    WriteLongTextColumnL(
        aView,
        ECatalogName,
        aPurchase.CatalogSourceName() );

    CBufBase* downloadBuf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( downloadBuf );

    RBufWriteStream downloadStream( *downloadBuf );
    CleanupClosePushL( downloadStream );

    TInt downloadCount = aPurchase.DownloadInfoCount();
    downloadStream.WriteInt32L( downloadCount );
    for ( TInt i = 0; i < downloadCount; i++ )
        {
        // Download information present.

        CNcdPurchaseDownloadInfo& info = aPurchase.DownloadInfo( i );
        info.ExternalizeL( downloadStream );
        }

    CleanupStack::PopAndDestroy( &downloadStream );
    TPtr8 downloadPtr = downloadBuf->Ptr( 0 );
    WriteLongTextColumn8L( aView, EDownloadInfo, downloadPtr );
    CleanupStack::PopAndDestroy( downloadBuf );

    WriteLongTextColumnL( aView, EPurchaseOptionId, aPurchase.PurchaseOptionId() );
    WriteLongTextColumnL( aView, EPurchaseOptionName, aPurchase.PurchaseOptionName() );
    WriteLongTextColumnL( aView, EPurchaseOptionPrice, aPurchase.PurchaseOptionPrice() );
    WriteLongTextColumnL( aView, EFinalPrice, aPurchase.FinalPrice() );
    WriteLongTextColumnL( aView, EPaymentMethodName, aPurchase.PaymentMethodName() );
    aView.SetColL( EPurchaseTime, aPurchase.PurchaseTime().Int64() );

    const MDesCArray& downloadedFiles = aPurchase.DownloadedFiles();

    CBufBase* filesBuf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( filesBuf );

    RBufWriteStream filesStream( *filesBuf );
    CleanupClosePushL( filesStream );

    TInt filesCount = downloadedFiles.MdcaCount();
    filesStream.WriteInt32L( filesCount );
    for ( TInt i = 0; i < filesCount; i++ )
        {
        // Downloaded files present.

        ExternalizeDesL( downloadedFiles.MdcaPoint( i ), filesStream );
        }

    CleanupStack::PopAndDestroy( &filesStream );
    TPtr8 filesPtr = filesBuf->Ptr( 0 );
    WriteLongTextColumn8L( aView, EDownloadedFiles, filesPtr );
    CleanupStack::PopAndDestroy( filesBuf );

    CBufBase* installBuf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( installBuf );

    RBufWriteStream installStream( *installBuf );
    CleanupClosePushL( installStream );

    TInt installCount = aPurchase.InstallInfoCount();
    installStream.WriteInt32L( installCount );
    for ( TInt i = 0; i < installCount; i++ )
        {
        // Install information present.

        CNcdPurchaseInstallInfo& info = aPurchase.InstallInfo( i );
        info.ExternalizeL( installStream );
        }

    CleanupStack::PopAndDestroy( &installStream );
    TPtr8 installPtr = installBuf->Ptr( 0 );
    WriteLongTextColumn8L( aView, EFileInstallInfos, installPtr );
    CleanupStack::PopAndDestroy( installBuf );
    
    if ( aSaveIcon ) 
        {
        DLTRACE(("Saving the icon to purchase history"));
        WriteLongTextColumn8L( aView, EIcon, aPurchase.Icon() );
        }
    WriteLongTextColumnL(
        aView,
        EDownloadAccessPoint,
        aPurchase.DownloadAccessPoint() );
    WriteLongTextColumnL( aView, EDescription, aPurchase.Description() );
    WriteLongTextColumnL( aView, EVersion, aPurchase.Version() );
    WriteLongTextColumnL( aView, EServerUri, aPurchase.ServerUri() );
    aView.SetColL( EItemType, aPurchase.ItemType() );
    aView.SetColL( ETotalContentSize, aPurchase.TotalContentSize() );
    WriteLongTextColumnL( aView, EOriginNodeId, aPurchase.OriginNodeId() );
    aView.SetColL( ELastOperationTime, aPurchase.LastOperationTime().Int64() );
    aView.SetColL( ELastOperationErrorCode, aPurchase.LastOperationErrorCode() );
    
    if( aPurchase.HasIcon() )
        {
        // HasIcon flag can be set but never unset.
        aView.SetColL( EHasIcon, ETrue );
        }
    
    // Externalize attributes
    RCatalogsBufferWriter attributeWriter;
    attributeWriter.OpenLC();
    aPurchase.ExternalizeAttributesL( attributeWriter() );    
    WriteLongTextColumn8L( aView, EAttributes, attributeWriter.PtrL() );
    CleanupStack::PopAndDestroy( &attributeWriter );
    
    }

void CNcdPurchaseHistoryDb::SetEventCountL( TUint aEventCount )
    {
    // Remove old event count from the database.
    User::LeaveIfError( iDatabase.Execute( KSqlEventCountDelete ) );
    
    // Construct database view.
    RDbView view;
    User::LeaveIfError( view.Prepare(
        iDatabase,
        TDbQuery( KSqlCurrentEventCount ),
        RDbView::EInsertOnly ) );
    view.InsertL();

    view.SetColL( KEventCounterColumnNumber, aEventCount );

    // Insert event count into the database
    view.PutL();
    view.Close();
    
    // Remove all unnecessary data from the database.
    User::LeaveIfError( iDatabase.Compact() );
    }

TBool CNcdPurchaseHistoryDb::PurchaseExistsL( TUint aPurchaseId )
    {
    TInt maxLength = KSqlPurchaseByPurchaseIdStart().Length()
        + KMaxLengthOfInt
        + KSqlPurchaseByPurchaseIdEnd().Length();

    HBufC* sqlStatement = HBufC::NewLC( maxLength );
    TPtr statementPtr( sqlStatement->Des() );

    statementPtr.Copy( KSqlPurchaseByPurchaseIdStart );
    statementPtr.AppendNum( aPurchaseId );
    statementPtr.Append( KSqlPurchaseByPurchaseIdEnd );

    RDbView view;
    CleanupClosePushL( view );

    User::LeaveIfError(
        view.Prepare( iDatabase, TDbQuery( *sqlStatement ) ) );

    User::LeaveIfError( view.EvaluateAll() );
    view.FirstL();

    TBool exists = EFalse;
    
    if ( view.AtRow() )
        {
        exists = ETrue;
        }

    CleanupStack::PopAndDestroy( &view );
    CleanupStack::PopAndDestroy( sqlStatement );
    
    return exists;
    }


void CNcdPurchaseHistoryDb::ValidatePurchaseDetailsL( 
    CNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN((""));
    
    TInt dlInfoCount = aDetails.DownloadInfoCount();
    
    // Add/remove filepaths so that there's as many filepaths as there are
    // download infos. Download infos determine the real count because they
    // are received from the protocol
    TInt filePathDiff = dlInfoCount - aDetails.DownloadedFiles().MdcaCount();
    if ( filePathDiff > 0 ) 
        {
        DLTRACE(("Adding %d empty filepaths", filePathDiff ));
        while ( filePathDiff-- ) 
            {
            aDetails.AddDownloadedFileL( KNullDesC() );
            }        
        }
    else if ( filePathDiff < 0 ) // This shouldn't really ever happen
        {        
        DLTRACE(("Removing %d filepaths", -filePathDiff ));
        // Start from the end
        TInt index = aDetails.DownloadedFiles().MdcaCount() - 1;
        
        filePathDiff = index + filePathDiff;
        for ( ; index > filePathDiff; index-- ) 
            {
            aDetails.RemoveDownloadedFile( index );            
            }
        }
    
    // install info count must be <= download info count
    TInt installDiff = dlInfoCount - aDetails.InstallInfoCount();
    if ( installDiff < 0 ) // This shouldn't really ever happen
        {
        DLTRACE(("Removing %d install infos", -installDiff ));
        // Start from the end
        TInt index = aDetails.InstallInfoCount() - 1;
        
        installDiff = index + installDiff;
        for ( ; index > installDiff; index-- ) 
            {
            aDetails.RemoveInstallInfo( index );            
            }        
        }
    }

