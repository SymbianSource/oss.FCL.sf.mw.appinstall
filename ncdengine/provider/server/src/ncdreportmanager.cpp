/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdReportManager implementation
*
*/


#include "ncdreportmanager.h"

#include "ncdoperationobserver.h"

// Supported reports
#include "ncdreportdownload.h"
#include "ncdreportomadownload.h"
#include "ncdreportinstall.h"

#include "ncdproviderdefines.h"
#include "ncdproviderutils.h"
#include "ncdpanics.h"
#include "ncdnodeidentifier.h"
#include "ncdconfigurationmanager.h"
#include "ncdserverdetails.h"
#include "ncdgeneralmanager.h"

// Protocol
#include "ncdprotocol.h"
#include "ncdrequestgenerator.h"
#include "ncdrequestinstallation.h"
#include "ncdcapabilities.h"
#include "ncdprotocol.h"

// Storage
#include "ncdstorage.h"
#include "ncddatabasestorage.h"
#include "ncdstoragemanager.h"
#include "ncdstorageitem.h"
#include "ncdstoragedataitem.h"

#include "catalogscontext.h"
#include "catalogsutils.h"

// HTTP
#include "catalogshttpincludes.h"
#include "catalogshttptransaction.h"
#include "catalogshttpsessionmanagerimpl.h"
#include "catalogsnetworkmanager.h"
#include "catalogshttpconnectionmanager.h"

#include "catalogsdebug.h"


void CNcdReportManager::ReportIdToDescriptor( 
    const TNcdReportId& aId, TDes& aTarget )
    {                
    aTarget.Num( aId );
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
CNcdReportManager* CNcdReportManager::NewL(
    const MCatalogsContext& aContext,   
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    TBool aClientCrashed )
    {
    CNcdReportManager* self = new(ELeave) CNcdReportManager(
        aContext,
        aGeneralManager,
        aHttpSession,
        aClientCrashed );
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CNcdReportManager::~CNcdReportManager()
    {
    DLTRACEIN((""));
    CancelReportSending();
    Cancel();
    
    iReports.ResetAndDestroy();
    if ( iNetworkManager )
        {        
        iNetworkManager->RemoveObserver( *this );
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CNcdReportManager::SetReportingMethod( const MNcdServerReportManager::TReportingMethod& aMethod )
    {
    DLTRACEIN((""));
    // Here we create a copy of the member variable for later comparison. Notice, that
    // we crete a copy instead of a reference because if reference was used then the value of the
    // variable would change when iReporting is changed.
    MNcdServerReportManager::TReportingMethod tmpMethod( iReportingMethod ); 
    iReportingMethod = aMethod;

    if ( tmpMethod != iReportingMethod
         && iReportingMethod == MNcdServerReportManager::EReportingBackground
         && iManagerState == ENcdReportManagerIdle
         && iTransactions.Count() == 0 )
        {
        // If reporting method was changed to background method,
        // then try one loop to send the reports now, instead of waiting for the next report setting.
        // Also, transactions are checked to be sure that no pending transactions exist and
        // to check if all HTTP operations are completed yet.
        // Probably there is not anything to send, but just in case.
        // This setting will also start the RunL loop for the sending.
        TRAP_IGNORE( SetManagerStateL( ENcdReportManagerPreparing ) );
        }
    }
    
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
const MNcdServerReportManager::TReportingMethod& CNcdReportManager::ReportingMethod() const
    {
    DLTRACEIN((""));
    return iReportingMethod;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CNcdReportManager::SetReportingStyle( const MNcdServerReportManager::TReportingStyle& aStyle )
    {
    DLTRACEIN((""));
    iReportingStyle = aStyle;
    }
    
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
const MNcdServerReportManager::TReportingStyle& CNcdReportManager::ReportingStyle() const
    {
    DLTRACEIN((""));
    return iReportingStyle;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CNcdReportManager::StartSendReportsL( MNcdOperationObserver& aObserver )
    {
    DLTRACEIN((""));

    if ( iObserver != NULL )
        {
        // Operation is already going on and observer is set.
        User::Leave( KErrInUse );
        }
    else if ( iReportingMethod != MNcdServerReportManager::EReportingManaged )
        {
        // This function should be called only for managed reports.
        User::Leave( KErrArgument );
        }
    else if ( iManagerState != ENcdReportManagerIdle
              || iTransactions.Count() > 0 )
        {
        // This case may occur if the reporting method has just been changed and
        // there was still an automated operation pending when sending was asked
        // by the manager.
        // Because observer was not set, then set it here.
        // Observers are set only when reporting is managed.
        // For automated reporting observers are not used.
        // Also, transactions are checked to be sure that no pending transactions exist and
        // to check if all HTTP operations are completed yet.
        // Notice, that ownership is not transferred here.
        iObserver = &aObserver;
        return;
        }
    else
        {
        // We are having a managed report and operation is in idle state.
        // So, start the action.
        SetManagerStateL( ENcdReportManagerPreparing );
        // Notice, that ownership is not transferred here.
        iObserver = &aObserver;
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TNcdReportId CNcdReportManager::RegisterDownloadL(
    const TDesC& aUri,
    const CNcdNodeIdentifier& aMetadataId,
	const TNcdReportStatusInfo& aStatus, 
	const TDesC& aReportUri,
	const TDesC& aReportNamespace )
    {
    DLTRACEIN(( _L("aUri: %S, aStatus: %d"), &aUri, aStatus.iStatus ));

    // Check if the download has already been registered
    CNcdReport* report = FindReport( 
        aUri, 
        aMetadataId, 
        aReportUri,
        ENcdReportDownload, 
        iReports );

    // Use old report only if it can still be updated with a new
    // status before sending. We don't want to overwrite any pending 
    // cancellation/failure/success reports
    if ( report && !report->StatusIsFinal() ) 
        {
        DLTRACEOUT(("Download has already been registered"));
        report->Attributes().SetAttributeL( 
            ENcdReportAttributeIsUsed, ETrue );
        
        return report->ReportId();
        }
        
    report = NULL;
    
    TBool serverSupports = ServerSupportsReports( 
        ENcdReportDownload,
        aReportUri, 
        aReportNamespace );
    
    if ( serverSupports ) 
        {        
        DLTRACE(("Server supports download reports"));
        report = CNcdReportDownload::NewLC( 
            *this,
            aUri, 
            aMetadataId,
            aStatus,
            aReportUri,
            aReportNamespace );

        iReports.InsertInOrderAllowRepeatsL( 
            report, 
            TLinearOrder<CNcdReport>( CNcdReport::Compare ) );

        CleanupStack::Pop( report );
        report->SetReportId( GenerateReportId() );
        report->Attributes().SetAttributeL( 
            ENcdReportAttributeIsUsed, ETrue );
        
        DLTRACEOUT(("Report added"));        
        return report->ReportId();
        }

    return KNcdReportNotSupported;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TNcdReportId CNcdReportManager::RegisterOmaDownloadL(
    const TDesC& aUri,
    const CNcdNodeIdentifier& aMetadataId,
	const TNcdReportStatusInfo& aStatus, 	
	const TDesC& aReportUri )
    {
    DLTRACEIN(( _L("aUri: %S, aStatus: %d"), &aUri, aStatus.iStatus ));       

    // Check if the download has already been registered
    CNcdReport* report = FindReport( 
        aUri, 
        aMetadataId, 
        aReportUri,
        ENcdReportOmaDownload,
        iReports );
    
    // Use old report only if it can still be updated with a new
    // status before sending    
    if ( report && !report->StatusIsFinal() ) 
        {
        
        DLTRACEOUT(("Download has already been registered"));
        report->Attributes().SetAttributeL( 
            ENcdReportAttributeIsUsed, ETrue );
        
        return report->ReportId();
        }
        
    report = NULL;
    
    DLTRACE(("Server supports download reports"));
    report = CNcdReportOmaDownload::NewLC( 
        *this,
        aUri, 
        aMetadataId,
        aStatus,
        aReportUri );

    iReports.InsertInOrderAllowRepeatsL( 
        report, 
        TLinearOrder<CNcdReport>( CNcdReport::Compare ) );

    CleanupStack::Pop( report );    
    report->SetReportId( GenerateReportId() );
    report->Attributes().SetAttributeL( 
        ENcdReportAttributeIsUsed, ETrue );
    
    DLTRACEOUT(("Report added"));
    return report->ReportId();
    }
    

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CNcdReportManager::SetDownloadReportAccessPoint( 
    const TNcdReportId& aReportId,    
    const TCatalogsConnectionMethod& aAccessPoint )
    {
    DLTRACEIN(("aAccessPoint: %d", aAccessPoint.iId ));

    CNcdReport* report = FindReport( 
        aReportId,
        iReports );
        
    if ( !report ) 
        {
        DLERROR(("Download has not been been registered"));
        return KErrNotFound;
        }

    report->SetAccessPoint( aAccessPoint );
    
    DLTRACEOUT(("Access point set successfully"));
    return KErrNone;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportManager::ReportDownloadStatusL( 
    const TNcdReportId& aReportId,
    const TNcdReportStatusInfo& aStatus,
    TBool aSendable )
    {
    DLTRACEIN(("aStatus: %d", aStatus.iStatus ));
    
    CNcdReport* report = FindReport( 
        aReportId,
        iReports );
    
    if ( !report ) 
        {
        DLERROR(("Report not found"));
        return;
        }    
        
    SetReportStatusL( *report, aStatus, aSendable );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TNcdReportId CNcdReportManager::RegisterInstallL(
    const TDesC& aContentIdentifier,
    const CNcdNodeIdentifier& aMetadataId,
	const TNcdReportStatusInfo& aStatus, 
	const TDesC& aReportUri,
	const TDesC& aReportNamespace )
    {
    DLTRACEIN(( _L("aReportUri: %S, aStatus: %d"), &aReportUri, aStatus.iStatus ));

    // Check if the install has already been registered.
    CNcdReport* report = FindReport( 
        aContentIdentifier,
        aMetadataId, 
        aReportUri,
        ENcdReportInstall,
        iReports );
        
    if ( report && !report->StatusIsFinal() ) 
        {
        DLTRACEOUT(("Install has already been registered"));
        report->Attributes().SetAttributeL( 
            ENcdReportAttributeIsUsed, ETrue );
        
        return report->ReportId();
        }        
    
    TBool serverSupports = ServerSupportsReports( 
        ENcdReportInstall,
        aReportUri, 
        aReportNamespace );
    
    if ( serverSupports ) 
        {        
        DLTRACE(("Server supports install reports"));
        report = CNcdReportInstall::NewLC( 
            *this,
            aContentIdentifier,
            aMetadataId,
            aStatus,
            aReportUri,
            aReportNamespace );

        iReports.InsertInOrderAllowRepeatsL( 
            report, 
            TLinearOrder<CNcdReport>( CNcdReport::Compare ) );

        CleanupStack::Pop( report );
        report->SetReportId( GenerateReportId() );
        report->Attributes().SetAttributeL( 
            ENcdReportAttributeIsUsed, ETrue );
        
        DLTRACEOUT(("Report added"));        
        return report->ReportId();
        }

    return KNcdReportNotSupported;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CNcdReportManager::SetInstallReportAccessPoint( 
    const TNcdReportId& aReportId,    
    const TCatalogsConnectionMethod& aAccessPoint )
    {
    DLTRACEIN(("aAccessPoint: %d", aAccessPoint.iId ));

    // Same implementation with download and install.
    TInt error( SetDownloadReportAccessPoint( aReportId, aAccessPoint ) );

    DLTRACEOUT(("Access point set successfully"));
    return error;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportManager::ReportInstallStatusL( 
    const TNcdReportId& aReportId,
    const TNcdReportStatusInfo& aStatus )
    {
    DLTRACEIN(("aStatus: %d", aStatus.iStatus));

    // Same implementation with download and install    
    TBool sendable( EFalse );
    if ( aStatus.iStatus == ENcdReportSuccess
         || aStatus.iStatus == ENcdReportCancel
         || aStatus.iStatus == ENcdReportFail )
        {
        sendable = ETrue;
        }
    ReportDownloadStatusL( aReportId, aStatus, sendable );
    }

    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CNcdReportManager::SetReportsAsUsedL( 
    const CNcdNodeIdentifier& aMetadataId )
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iReports.Count(); ++i )
        {
        if ( iReports[i]->MetadataId().Equals( aMetadataId ) ) 
            {
            DLINFO(("Setting report as used"));       
            iReports[i]->Attributes().SetAttributeL( 
                ENcdReportAttributeIsUsed, ETrue );
            // No break since there may be other reports with
            // the same metadata id
            } 
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportManager::RemoveUnusedReportsL()
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iReports.Count(); ++i )
        {
        if ( !iReports[i]->Attributes().AttributeInt32(
                ENcdReportAttributeIsUsed ) )
            {            
            TNcdReportStatusInfo info( ENcdReportCancel, KErrCancel );
            SetReportStatusL( *iReports[i], info, ETrue );
            }
        }
    }
    
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportManager::OpenStorageL()
    {
    DLTRACEIN((""));
    iDb = NULL;    
    
    HBufC* clientUid = iContext.FamilyId().Name().AllocLC();    
      
    // Recreate necessary storage objects. Nothing is created if not necessary
    MNcdStorage& storage = iStorageManager.CreateOrGetStorageL( 
        *clientUid, NcdProviderDefines::KDownloadNamespace );

    CleanupStack::PopAndDestroy( clientUid );    
    
    iDb = &storage.DatabaseStorageL( 
        NcdProviderDefines::KReportDatabaseUid );
    DLTRACEOUT(("Storage opened, iDb: %x", iDb));
    }
    
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportManager::CloseStorage()
    {
    DLTRACEIN((""));
    
    iDb = NULL;
    }

    
// ---------------------------------------------------------------------------
// Loads unfinished reports 
// ---------------------------------------------------------------------------    
void CNcdReportManager::LoadReportsL()
    {
    DLTRACEIN((""));
   
    OpenStorageL();
        
    DASSERT( iDb );
    
    // Get all items from the db
    RPointerArray<MNcdStorageItem> items;
    iDb->StorageItemsL( items );
    CleanupClosePushL( items );

    for ( TInt i = 0; i < items.Count(); ++i )
        {
        items[i]->SetDataItem( this );
        
        // This will cause a call to report manager's InternalizeL
        items[i]->ReadDataL();
        }

    CleanupStack::PopAndDestroy( &items );
    
    DLTRACEOUT(("Reports loaded"));
    }        
    

// ---------------------------------------------------------------------------
// Saves an individual report to disk
// ---------------------------------------------------------------------------
void CNcdReportManager::SaveReportL( CNcdReport& aReport )
    {
    DLTRACEIN((""));
    
    MNcdStorageItem& item( StorageItemForReportL( aReport.ReportId() ) );
    
    // Save new item to database
    item.SetDataItem( &aReport );
    item.OpenL();
    
    // Calls ExternalizeL for the item
    item.WriteDataL();
    
    // Save the item to the database.
    item.SaveL();      
    
    DLTRACEOUT((""));
    }
    

// ---------------------------------------------------------------------------
// Remove report from db
// ---------------------------------------------------------------------------
void CNcdReportManager::RemoveReportL( const TNcdReportId& aId )
    {
    DLTRACEIN((""));
    
    MNcdStorageItem& item( StorageItemForReportL( aId ) );
    item.RemoveFromStorageL();    
    }


// ---------------------------------------------------------------------------
// Get a storage item that matches the report id
// ---------------------------------------------------------------------------
MNcdStorageItem& CNcdReportManager::StorageItemForReportL( 
    const TNcdReportId& aId )
    {
    DLTRACEIN((""));
    OpenStorageL();
    
    DASSERT( iDb );
    
    TBuf<KNcdReportIdLength16Bit> id;
    ReportIdToDescriptor( aId, id );    
    DLTRACE(( _L("Id: %S"), &id ));
    DLTRACE(("Getting storage item, iDb: %x", iDb ));
    // Get/create the storage item where the data is saved
    // Note: database has the ownership of the item
    MNcdStorageItem* item = iDb->StorageItemL( id, KNcdReportDataType );    
    return *item;
    }
    

// ---------------------------------------------------------------------------
// MCatalogsAccessPointObserver
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportManager::HandleAccessPointEventL( 
    const TCatalogsConnectionMethod& aAp,
    const TCatalogsAccessPointEvent& aEvent )
    {
    DLTRACEIN((""));
    (void) aAp; // suppress warning
    
    // Just try to send reports when an AP is opened.
    if ( aEvent == ECatalogsAccessPointOpened && 
         iManagerState == ENcdReportManagerIdle &&
         iReportingMethod == MNcdServerReportManager::EReportingBackground )
        {
        DLTRACE(("Ap: %d opened", aAp.iId ));
        // Notice, that the manager state is set to ENcdReportManagerPreparing 
        // only if the reporting method should work in the background.
        // This setting will also start the RunL loop for the sending.
        SetManagerStateL( ENcdReportManagerPreparing );
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportManager::HandleHttpEventL( 
    MCatalogsHttpOperation& aOperation, 
    TCatalogsHttpEvent aEvent )
    {
    DLTRACEIN((""));

    switch( aEvent.iOperationState ) 
        {
        // Handle completed operation
        case ECatalogsHttpOpCompleted:
            {
            DLTRACE(("Operation complete"));
            // Finish report releases the transaction
            FinishReportL( aOperation, KErrNone );                                    
            break;
            }   
                 
        // Handle operation in progress
        case ECatalogsHttpOpInProgress:
            {
            if ( aEvent.iProgressState == 
                ECatalogsHttpResponseBodyReceived )
                {
                DLINFO(("response body=%S", &aOperation.Body() ));
                // send received data to parser                
                }
            break;
            }
                    
        default:
            {
            break;
            }
        }
        
    DLTRACEOUT((""));    

    }
        
        
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CNcdReportManager::HandleHttpError(
    MCatalogsHttpOperation& aOperation,
    TCatalogsHttpError aError )
    {
    DLTRACEIN(("Error type: %d, code: %d", aError.iType, aError.iError ));
    
    // Can't do anything for the error anyway 
    // Finish report releases the transaction
    TRAP_IGNORE( FinishReportL( aOperation, aError.iError ) );      
    DLTRACEOUT((""));
    return ETrue;

    }


// ---------------------------------------------------------------------------
// Context getter
// ---------------------------------------------------------------------------
const MCatalogsContext& CNcdReportManager::Context() const
    {
    return iContext;
    }


// ---------------------------------------------------------------------------
// Cancel ongoing report transactions
// ---------------------------------------------------------------------------
void CNcdReportManager::CancelReportSending()
    {
    DLTRACEIN((""));
    
    for ( TInt i = 0; i < iTransactions.Count(); ++i )
        {
        iTransactions[i]->Cancel();        
        }
    iTransactions.Reset();
    
    for ( TInt i = 0; i < iReports.Count(); ++i )
        {
        iReports[i]->SetReportTransaction( NULL );
        }

    if ( iObserver != NULL )
        {
        // Inform the observer about the cancellation.
        iObserver->OperationComplete( NULL, KErrCancel );
        iObserver = NULL;
        }
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TCatalogsConnectionMethod CNcdReportManager::DefaultConnectionMethod()
    {
    DLTRACEIN((""));
    // APN ID of the current/last opened default accesspoint
    TUint32 apn = iHttpSession.ConnectionManager().CurrentAccessPoint();

    // This ensures that reports use the latest connected default accesspoint
    // or the AP that is set to them. 
    // Issue: DLEB-140
    TCatalogsConnectionMethod method( 
        apn, 
        ECatalogsConnectionMethodTypeAccessPoint );
    method.iApnId = apn;

    return method;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CNcdGeneralManager& CNcdReportManager::GeneralManager() const
    {
    return iGeneralManager;
    }


// ---------------------------------------------------------------------------
// From MNcdStorageDataItem
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportManager::ExternalizeL( RWriteStream& /*aStream*/ )
    {
    DLTRACEIN((""));
    // nothing to do
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportManager::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));    
    
    TNcdReportType type;
    InternalizeEnumL( type, aStream );
    
    CNcdReport* report( NULL );
    
    switch( type )
        {
        case ENcdReportDownload:
            {
            report = CNcdReportDownload::NewLC( *this, aStream );            
            break;
            }

        case ENcdReportOmaDownload:
            {
            report = CNcdReportOmaDownload::NewLC( *this, aStream );            
            break;
            }
            
        case ENcdReportInstall:
            {
            report = CNcdReportInstall::NewLC( *this, aStream );            
            break;
            }
                
        default:
            {
            NCD_ASSERT_ALWAYS( 0, ENcdPanicIndexOutOfRange );
            }
        }

    CNcdAttributes& att( report->Attributes() );

    if ( !iClientCrashed ) 
        {
        
        // Set as unused, flag is set to used if a download registers
        // to this report
        att.SetAttributeL( 
            ENcdReportAttributeIsUsed, EFalse );
        
        // Check if the report sending was canceled and
        // set as sendable if it was
        if ( att.AttributeInt32( 
                ENcdReportAttributeReportBeingSent ) != ENcdReportNone &&
             att.AttributeInt32( 
                ENcdReportAttributeLatestSentReport ) == ENcdReportNone )
            {
            DLTRACE(("Set as sendable because previous attempt was canceled"));
            att.SetAttributeL( ENcdReportAttributeSendable, ETrue );
            }
        }
    else 
        {
        DLTRACE(("Set report as failed because engine crashed"));
        TNcdReportStatusInfo info( ENcdReportFail, KErrUnknown );
        report->SetStatus( info );
        att.SetAttributeL( 
            ENcdReportAttributeIsUsed, EFalse );
        
        // Check if the report sending was canceled and
        // set as sendable if it was
        att.SetAttributeL( ENcdReportAttributeSendable, ETrue );

        }
        
    iReports.InsertInOrderAllowRepeatsL( 
        report, 
        TLinearOrder<CNcdReport>( CNcdReport::Compare ) );
    
    CleanupStack::Pop( report ); // report, 
    
    // ensure that iNewReportId is higher than any current report id
    if ( report->ReportId() >= iNewReportId ) 
        {
        iNewReportId = report->ReportId() + 1;
        }
    
    DLTRACEOUT(("Report internalized"));
    }



// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportManager::RunL()
    {
    DLTRACEIN((""));
    if ( iStatus.Int() == KErrNone ) 
        {
        switch ( iManagerState ) 
            {
            case ENcdReportManagerPreparing:
                {
                SendReportsL();
                break;
                };
            
            default:
                {
                DLTRACE(("Default"));
                break;
                }
            }
        }
    }
    
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportManager::DoCancel()
    {
    DLTRACEIN((""));
    
    // No requests to cancel
    }
    
        
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CNcdReportManager::RunError( TInt aError )
    {
    DLTRACEIN(("aError: %d", aError));
    
    if ( iObserver != NULL )
        {
        iObserver->OperationComplete( NULL, aError );
        iObserver = NULL;        
        }

    // Return KErrNone so that CActiveScheduler doesn't panic 
    // with E32USER-CBase 47. Would call an observer if there was any.
    
    return KErrNone;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportManager::FinishReportL( MCatalogsHttpOperation& aOperation, TInt aErrorCode )
    {
    DLTRACEIN((""));

    // This function will insert the information into the reports after the
    // report sending operation has been finished. So, all the reports that have
    // been handled by the operation are removed from the list.
       
    MCatalogsHttpOperation* operationPtr = &aOperation;            
    
    // Remove the transaction before any leaving code so that if a leave
    // occurs, we don't have any hanging transactions in the array
    TInt index = iTransactions.FindInAddressOrder( &aOperation );
    DASSERT( index != KErrNotFound );
    
    iTransactions.Remove( index );
    aOperation.Release();
    
    TInt i = iReports.Count();
    while ( i-- )
        {
        if ( iReports[i]->ReportTransaction() == operationPtr ) 
            {
            iReports[i]->SetReportTransaction( NULL );
            
            CNcdAttributes& att( iReports[i]->Attributes() );
            
            // Update latest sent report status
            att.SetAttributeL( 
                ENcdReportAttributeLatestSentReport,
                att.AttributeInt32(
                    ENcdReportAttributeReportBeingSent ) );
                                            
            if ( iReports[i]->CanBeRemoved() ) 
                {
                DLTRACE(("Deleting report"));                                
                RemoveReportL( iReports[i]->ReportId() );
                delete iReports[i];
                iReports.Remove( i ); 
                }
            else // save info that report was sent
                {                
                SaveReportL( *iReports[i] );
                }
            }
        }
    

    DLTRACE(("Committing report changes"));
    iDb->CommitL();

 
    // Check if all the transactions have been handled. If they are, then no more
    // http operations are going on and the reporting is finished.   
    if ( iTransactions.Count() == 0 )
        {
        DLTRACE(("No more pending reports.")); 
        if ( iReportingMethod == MNcdServerReportManager::EReportingBackground )
            {
            DLINFO(("Start handling next patch"));
            // Notice, that by calling this, we continue to the next round in the RunL.
            // So, then new reports will be sent if they exist then. If there is already new
            // items available, then they will be sent. Otherwise the operation
            // will end after RunL is called next time.
            SetManagerStateL( ENcdReportManagerPreparing );
            }
        else
            {
            DLINFO(("Set to idle because sending is managed by hand."));
            // This will set the state and the RunL loop will not be continued.
            SetManagerStateL( ENcdReportManagerIdle );
            }
            
        // Notice, that observer is set only for the managed reporting. But, in some cases
        // the reporting may have been changed to background before the requested managed send
        // was finished. So, instead of waiting for the background action to finish in next
        // RunL loop, inform the observer already here about the completion. If only background
        // operations have been occurring, then the observer is NULL here.
        if ( iObserver != NULL )
            {
            // Because observer was set for manager to know when managed operation
            // was finished, inform the observer.
            iObserver->OperationComplete( NULL, aErrorCode );
            iObserver = NULL;
            }
        }
    }


// ---------------------------------------------------------------------------
// Server capability check
// ---------------------------------------------------------------------------
TBool CNcdReportManager::ServerSupportsReports( 
    TNcdReportType aType,
    const TDesC& aServerUri,
    const TDesC& aServerNamespace ) const
    {
    DLTRACEIN((""));
    const MNcdServerDetails* serverDetails = 
        iConfigurationManager.ServerDetails( 
            iContext,
            aServerUri,
            aServerNamespace );
    
    // No server details so we don't know whether server supports them
    // or not    
    if ( !serverDetails ) 
        {
        DLTRACEOUT(("Server details not found"));
        return EFalse;
        }

    // Notice, that install capability support is already checked here.
    TBool supports = 
        serverDetails->IsCapabilitySupported( 
            NcdCapabilities::KInstallationReport() );

    // Download report requires install capability and also download capability    
    if ( aType == ENcdReportDownload ) 
        {
        DLTRACE(("Checking for downloadReport capability"));
    
        supports = supports && 
            serverDetails->IsCapabilitySupported( 
                NcdCapabilities::KDownloadReport() );
        }
        
    DLTRACEOUT(("Required caps supported: %d", supports ));
    return supports;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CNcdReportManager::CNcdReportManager( 
    const MCatalogsContext& aContext,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    TBool aClientCrashed ) :
    CActive( EPriorityStandard ),
    iContext( aContext ),
    iGeneralManager( aGeneralManager ),
    iConfigurationManager( aGeneralManager.ConfigurationManager() ),
    iStorageManager( aGeneralManager.StorageManager() ),
    iHttpSession( aHttpSession ),
    iReportingMethod( MNcdServerReportManager::EReportingBackground ),
    iReportingStyle( MNcdServerReportManager::EReportingStyleGeneral ),
    iClientCrashed( aClientCrashed )
    {
    }
    
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportManager::ConstructL()
    {
    CActiveScheduler::Add( this );   
    
    iNetworkManager = &CCatalogsHttpSessionManager::NetworkManagerL();
    // observe changes in access points
    iNetworkManager->AddObserverL( *this );
    LoadReportsL(); 
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CNcdReport* CNcdReportManager::FindReport( 
    const TNcdReportId& aReportId,
    RNcdReportArray& aArray ) const
    {
    DLTRACEIN(("aReportId: %d", aReportId));
    for ( TInt i = 0; i < aArray.Count(); ++i )
        {
        if ( aArray[i]->ReportId() == aReportId ) 
            {
            return aArray[i];
            }
        }
    DLTRACEOUT(("Not found"));
    return NULL;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CNcdReport* CNcdReportManager::FindReport( 
    const TDesC& aId,
    const CNcdNodeIdentifier& aNodeId,
    const TDesC& aReportUri,
    const TNcdReportType& aReportType,
    RNcdReportArray& aArray ) const
    {
    DLTRACEIN(( _L("aId: %S, report URI: %S"), &aId, &aReportUri ));
    // Search backwards so that the newest & changeable report is found
    TInt count = aArray.Count();
    while( count-- )    
        {
        if ( aArray[ count ]->Match( aNodeId, aId, aReportUri, aReportType ) ) 
            {
            return aArray[ count ];
            }
        }
    DLTRACEOUT(("Not found"));
    return NULL;
    }


// ---------------------------------------------------------------------------
// State setter
// ---------------------------------------------------------------------------
void CNcdReportManager::SetManagerStateL( TNcdReportManagerState aState )
    {
    DLTRACEIN(("aState: %d", aState));
        
    iManagerState = aState;
    switch ( iManagerState )
        {
        case ENcdReportManagerPreparing:
            {
            ExecuteRunL( KErrNone );
            break;
            }

        case ENcdReportManagerSending:
            {
            break;
            }
        
        default:
            {
            break;
            }
        }        
    }


// ---------------------------------------------------------------------------
// Initiate report sending
// ---------------------------------------------------------------------------
void CNcdReportManager::SendReportsL()
    {
    DLTRACEIN((""));
    
    DASSERT( !iTransactions.Count() );

    // Update open connections so that IsAccessPointOpen is up-to-date
    iNetworkManager->UpdateConnectionsL();    
    
    TBool defaultApIsOpen = iNetworkManager->IsAccessPointOpen( 
        DefaultConnectionMethod() );
    
    for ( TInt i = 0; i < iReports.Count(); ++i ) 
        {
        CNcdReport& report = *iReports[i];
        
        if ( report.IsSendable() && (
                // either report has no AP set -> use default which has to be open
                // or its AP needs to be open
                ( defaultApIsOpen ) ||
                ( iNetworkManager->IsAccessPointOpen( report.ConnectionMethod() ) ) ) )
            {
            if ( !iNetworkManager->IsAccessPointOpen( report.ConnectionMethod() ) ) 
                {
                DLTRACE(("Using default ap for reporting"));
                report.SetAccessPoint( DefaultConnectionMethod() );
                }

            MCatalogsHttpOperation* transaction = 
                iHttpSession.CreateTransactionL( 
                    report.Attributes().AttributeString16(
                        ENcdReportAttributeReportUri ), this );
            
            CleanupReleasePushL( *transaction );

            if ( report.CanBundle() ) 
                {                
                BundleReportsL( i, *transaction );
                }
            else
                {
                PrepareReportL( report, *transaction );
                }
            
            report.UpdateTransactionConfigL( *transaction );
            User::LeaveIfError( transaction->Start() );
            
            iTransactions.InsertInAddressOrderL( transaction );
            CleanupStack::Pop( transaction );
            }
        }

    // The operations that were created above, will call http callback functions 
    // of this class and the operation will be finished in FinishReportL function.
    
    // Check if the manager state should be set to idle or if we should
    // let the state to be something else and wait for the HTTP operations
    // that were created above to complete..
    if ( iTransactions.Count() == 0 )
        {
        DLTRACE(("Nothing to send anymore."));
        // This will set the state and operation will not continue RunL loop
        // any more.
        SetManagerStateL( ENcdReportManagerIdle );

        if ( iObserver != NULL )
            {
            // Because observer was set for manager to know when managed operation
            // was finished, inform the observer.
            iObserver->OperationComplete( NULL, KErrNone );
            iObserver = NULL;
            }
        }
    }
        
    
// ---------------------------------------------------------------------------
// Bundle reports
// ---------------------------------------------------------------------------
void CNcdReportManager::BundleReportsL( 
    TInt aIndex, 
    MCatalogsHttpOperation& aTransaction )
    {
    DLTRACEIN(("Bundling reports from index %d onwards", aIndex ));
    DASSERT( aIndex < iReports.Count() );
    
    CNcdRequestInstallation* report = 
        NcdRequestGenerator::CreateInstallationReportRequestLC();

    
    CNcdReport& currentReport( *iReports[ aIndex ] );
    CNcdAttributes& att( currentReport.Attributes() );
    
    report->SetNamespaceL( 
        att.AttributeString16( ENcdReportAttributeReportNamespace ) );
    
    currentReport.AddReportDataL( *report );
    currentReport.SetReportTransaction( &aTransaction );
    
    // Update report flags: beingSent = current status, 
    // latestSentReport = none, so we can identify cases were the same
    // status is sent multiple times in a row (pause, pause...)
    UpdateReportAttributesForSendingL( currentReport );
    
    DLTRACE(("Start bundling, report count: %d", iReports.Count() ));
    for ( TInt i = aIndex + 1; i < iReports.Count(); ++i )
        {
        
        TNcdReportBundleMatch bundle = CanBundleWithL( 
            currentReport,
            *iReports[i] );
            
        if ( bundle == ENcdReportBundleMatch && 
             iReports[i]->IsSendable() ) 
            {
            DLTRACE(("Bundling report %d", i ));
            iReports[i]->AddReportDataL( *report );
            iReports[i]->SetReportTransaction( &aTransaction );
            
            // Update report status
            UpdateReportAttributesForSendingL( *iReports[i] );            
            }
        else if ( bundle == ENcdReportBundleNoMatch ) 
            {
            DLTRACE(("Nothing more to bundle, exit loop"));
            break;
            }
        }
    
    DLTRACE(("Process report"));
    HBufC8* body = 
        iGeneralManager.ProtocolManager().ProcessPreminetRequestL(
            Context(),
            *report,
            att.AttributeString16( ENcdReportAttributeReportUri ),
            ETrue );
    CleanupStack::PopAndDestroy( report );
    
    CleanupStack::PushL( body );        
    aTransaction.SetBodyL( *body );
    CleanupStack::PopAndDestroy( body );
    }


// ---------------------------------------------------------------------------
// Updates common attributes of the report for sending
// ---------------------------------------------------------------------------
void CNcdReportManager::UpdateReportAttributesForSendingL( 
    CNcdReport& aReport )
    {
    DLTRACEIN((""));
    CNcdAttributes& att( aReport.Attributes() );
    
    att.SetAttributeL( 
        ENcdReportAttributeReportBeingSent,
        aReport.Status().iStatus );

    att.SetAttributeL( 
        ENcdReportAttributeLatestSentReport,
        ENcdReportNone );
    
    att.SetAttributeL(
        ENcdReportAttributeSendable,
        EFalse );
    }

// ---------------------------------------------------------------------------
// Check if reports can be bundled together
// ---------------------------------------------------------------------------
TNcdReportBundleMatch CNcdReportManager::CanBundleWithL(     
    const CNcdReport& aReport, 
    const CNcdReport& aReport2 ) const
    {
    DLTRACEIN((""));
        // Report URIs must match
    if ( ( aReport.Attributes().AttributeString16(
                ENcdReportAttributeReportUri ).Compare( 
            aReport2.Attributes().AttributeString16(
                ENcdReportAttributeReportUri ) ) != 0 ) )
        {
        DLTRACEOUT(("Can stop"));
        return ENcdReportBundleNoMatch;
        }
        
    if (
        // Report namespaces must match
        ( aReport.Attributes().AttributeString16(
            ENcdReportAttributeReportNamespace ).Compare( 
          aReport2.Attributes().AttributeString16(
            ENcdReportAttributeReportNamespace ) ) == 0 ) 

        &&
        // Report access points must match
        ( CompareConnectionMethods( aReport, aReport2 ) ) )
        {
        return ENcdReportBundleMatch;
        }
    
    DLTRACEOUT((""));
    return ENcdReportBundleUriMatch;
    }
    
    
// ---------------------------------------------------------------------------
// Prepare report for sending
// ---------------------------------------------------------------------------
void CNcdReportManager::PrepareReportL( 
    CNcdReport& aReport,
    MCatalogsHttpOperation& aTransaction )
    {
    DLTRACEIN((""));

    aReport.SetReportTransaction( &aTransaction );
    
    UpdateReportAttributesForSendingL( aReport );    
    
    HBufC8* body = aReport.CreateReportL();
    CleanupStack::PushL( body );
    DLINFO(( "body= %S", body ));
    
    aTransaction.SetBodyL( *body );
    CleanupStack::PopAndDestroy( body );        
    }


// ---------------------------------------------------------------------------
// Complete request
// ---------------------------------------------------------------------------
void CNcdReportManager::ExecuteRunL( TInt aError )    
    {
    DLTRACEIN(("aError: %d", aError));
    
    if ( !IsActive() ) 
        {        
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, aError );
        }
    }


// ---------------------------------------------------------------------------
// Generate a new report id
// ---------------------------------------------------------------------------
TNcdReportId CNcdReportManager::GenerateReportId()
    {
    DLTRACEIN((""));    
    return iNewReportId++;
    }
    

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReportManager::SetReportStatusL( 
    CNcdReport& aReport,
    const TNcdReportStatusInfo& aStatus,
    TBool aSendable )
    {
    DLTRACEIN((""));
    aReport.SetStatus( aStatus );
    aReport.Attributes().SetAttributeL( 
        ENcdReportAttributeSendable, 
        aSendable );
        
    SaveReportL( aReport );
    
    // Start sending if the report is sendable and the manager isn't already
    // sending. Also if the operation is not done in the background then do not
    // do sending here. But, later when it is separately asked.
    if ( aReport.IsSendable() && 
         iManagerState == ENcdReportManagerIdle &&
         iReportingMethod == MNcdServerReportManager::EReportingBackground ) 
        {
        SetManagerStateL( ENcdReportManagerPreparing );
        }    
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CNcdReportManager::CompareConnectionMethods( 
    const CNcdReport& aFirstReport,
    const CNcdReport& aSecondReport ) const
    {
    DLTRACEIN((""));
    const TCatalogsConnectionMethod& first = aFirstReport.ConnectionMethod();
    const TCatalogsConnectionMethod& second = aSecondReport.ConnectionMethod();   
    
    switch( first.iType )
        {                   
        case ECatalogsConnectionMethodTypeAlwaysAsk:
        case ECatalogsConnectionMethodTypeDeviceDefault: // flow-through
            {            
            if ( second.iType == first.iType )
                {
                return ETrue;                
                }
            break;
            }
        
        case ECatalogsConnectionMethodTypeDestination:
            {
            if ( second.iType == ECatalogsConnectionMethodTypeDestination )
                {
                return ( first.iId != 0 && first.iId == second.iId ) || 
                       ( first.iApnId != 0 && first.iApnId == second.iApnId );                
                }
            break;
            }
        
        case ECatalogsConnectionMethodTypeAccessPoint:
            {
            if ( second.iType == ECatalogsConnectionMethodTypeAccessPoint )
                {
                return ( first.iId != 0 && first.iId == second.iId );
                }
            break;
            }
        
        default:
            { 
            DASSERT( 0 );
            }
        }
        
    return ( first.iApnId != 0 && first.iApnId == second.iApnId );     
    }
