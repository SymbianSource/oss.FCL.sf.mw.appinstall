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


#include "ncdcontentdownloadoperation.h"

#include <s32mem.h>
#include <f32file.h>
#include <apmstd.h>
#include <e32math.h>

#include "catalogsbasemessage.h"
#include "catalogshttpsession.h"
#include "catalogshttpconfig.h"
#include "catalogshttpoperation.h"
#include "catalogsconstants.h"
#include "ncdnodemanager.h"
#include "ncdnodeimpl.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodeidentifier.h"
#include "ncdsessionhandler.h"
#include "ncdhttpheaders.h"
#include "catalogsutils.h"
#include "catalogshttpheaders.h"
#include "ncdconfigurationmanager.h"
#include "catalogscontext.h"
#include "ncdproviderdefines.h"
#include "ncdnodeclassids.h"
#include "ncdnodelink.h"
#include "ncdoperationremovehandler.h"
#include "ncddownloadsuboperation.h"
#include "ncddescriptordownloadsuboperation.h"
#include "ncdnodedownloadimpl.h"
#include "ncdpurchasedetails.h"
#include "ncdpurchasedownloadinfo.h"
#include "ncdpurchasehistorydbimpl.h"
#include "ncdutils.h"
#include "ncderrors.h"
#include "ncdpanics.h"
#include "ncdstorageitem.h"
#include "ncddatabasestorage.h"
#include "ncdcontentdescriptor.h"
#include "ncdproviderutils.h"
#include "ncdengineconfiguration.h"
#include "ncdnodecontentinfoimpl.h"
#include "ncdnodeinstallimpl.h"
#include "catalogshttpconnectionmanager.h"
#include "ncdpurchasehistoryutils.h"
#include "ncdnodedependencyimpl.h"
#include "catalogsaccesspointmanager.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========


const TInt KNoDownloads = 25000;

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdContentDownloadOperation* CNcdContentDownloadOperation::NewL( 
    MNcdOperationRemoveHandler& aRemoveHandler, 
    const CNcdNodeIdentifier& aNodeId,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdDownloadReportObserver& aReportObserver,
    MNcdSessionHandler* aSessionHandler,    
    MNcdDatabaseStorage& aDownloadStorage,
    MCatalogsSession& aSession,
    TInt aDownloadIndex )
    {
    CNcdContentDownloadOperation* self = new( ELeave ) 
        CNcdContentDownloadOperation( 
        aRemoveHandler,
        aGeneralManager, 
        aHttpSession, 
        aReportObserver,
        aSessionHandler, 
        aSession,
        aDownloadStorage );
        
    CleanupClosePushL( *self );
    self->ConstructL( aNodeId, aDownloadIndex );

    CleanupStack::Pop();
    return self;
    }



// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdContentDownloadOperation* CNcdContentDownloadOperation::NewLC( 
    MNcdOperationRemoveHandler& aRemoveHandler, 
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdDownloadReportObserver& aReportObserver,
    MNcdDatabaseStorage& aDownloadStorage,
    MCatalogsSession& aSession )
    {
    CNcdContentDownloadOperation* self = new( ELeave ) 
        CNcdContentDownloadOperation( 
        aRemoveHandler,
        aGeneralManager, 
        aHttpSession, 
        aReportObserver,
        NULL, 
        aSession,
        aDownloadStorage );
        
    CleanupClosePushL( *self );
    
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdContentDownloadOperation::~CNcdContentDownloadOperation()
    {
    DLTRACEIN( ( "" ) );
    
    if ( iDownloadState == ENcdDownloadInProgress )
        {        
        if ( iDownload )
            {
            DLTRACE(("Pausing download"));
            iDownload->Pause();
            }
        iDownloadState = ENcdDownloadPaused;
        }
    
    if ( iDownloadState == ENcdDownloadPaused && 
         iOperationState != EStateCancelled )
        {
        DLINFO(("Externalizing the download since it's paused"));

        TRAP_IGNORE( SaveStateL() );            
        }
    else
        {
        TRAP_IGNORE( RemoveTempInfoL() );
        }
        
    if ( iDownload ) 
        {
        iDownload->Close();
        iDownload = NULL;
        }

    if ( iDescriptorDownload )
        {
        iDescriptorDownload->Close();
        iDescriptorDownload = NULL;
        }
    
    delete iDescriptor;
    delete iMimeType;
    
    delete iSessionId;
    delete iContentDescriptor;
    
    delete iContentFilename;
    delete iStorageUid;
    DLTRACEOUT((""));
    }



// ---------------------------------------------------------------------------
// Cancel
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::Cancel() 
    {    
    DLTRACEIN(( "" ));
    
    // Can't handle leaves in cancel
    TNcdReportStatusInfo info( ENcdReportCancel, KErrCancel );
    TRAP_IGNORE( ReportStatusL( info ) );
    TRAP_IGNORE( SendOmaNotificationL( info ) );
    
    if ( iDownload ) 
        {
        // Content downloads should be cancelled only when the client says so
        if ( iDeleting )
            {
            iDownload->Pause();            
            }
        else
            {                        
            iDownload->Cancel();
            ClosePtr( iDownload );            
            }        
        }
        
    if ( iDescriptorDownload )
        {
        iDescriptorDownload->Cancel();
        iDescriptorDownload = NULL;
        }

    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// ReceiveMessage
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::ReceiveMessage( 
    MCatalogsBaseMessage* aMessage,
    TInt aFunctionNumber )
    {
    DLTRACEIN(( "Function: %d", aFunctionNumber ));
    TNcdOperationMessageCompletionId completionId;
    TInt err = KErrNone;
    
    TBool requireStart = EFalse;
    // Response to pause and resume messages, other messages are handled
    // by the base class
    switch ( aFunctionNumber )
        {
        // GetData which in fact is pausable check
        case ENCDOperationFunctionGetData: 
            {
            DLTRACE(( "ENCDOperationFunctionGetData "));
            // Only error will come from CompleteMessageL
            TRAP_IGNORE( GetPausableStateL( *aMessage ) );
            return;
            }
        
        case ENCDOperationFunctionStart:
            {
            // Resume if paused, otherwise start
            if ( iDownload && iDownload->Progress().iState 
                 == ECatalogsHttpOpPaused )
                {
                DLTRACE(("Resuming download"));
                err = iDownload->Start();
                if ( err == KErrNone ) 
                    {
                    iDownloadState = ENcdDownloadInProgress;
                    }
    
                completionId = ENCDOperationMessageCompletionResume;
                }
            else 
                {
                CNcdBaseOperation::ReceiveMessage( aMessage, aFunctionNumber );
                return;
                }
            break;
            }
        
        // Pause download if it's pausable
        case ENCDOperationFunctionPause:
            {
            DLTRACE( ( "ENCDOperationFunctionPause" ) );
            if ( iOperationState != EStateComplete && 
                 iOperationState != EStateCancelled ) 
                {
                err = KErrNone;
                if ( iDownload ) 
                    {
                    // Don't care about pausable status since the user wants to pause
                    // we must pause the operation. If the download is not pausable,
                    // it will be started from the beginning                   
                    err = iDownload->Pause();              
                    }
                                    
                if ( iDescriptorDownload )
                    {
                    iDescriptorDownload->Cancel();
                    iDescriptorDownload = NULL;
                    }
                    
                if ( err == KErrNone ) 
                    {
                    iDownloadState = ENcdDownloadPaused;
                    TNcdReportStatusInfo info( ENcdReportPause, KErrNone );
                    TRAP( err, ReportStatusL( info, EFalse ) );
                    }
                }
            completionId = ENCDOperationMessageCompletionPause;
            break;
            }
            
            
        // Resume download
        case ENCDOperationFunctionResume:
            {
            DLTRACE( ( "ENCDOperationFunctionResume" ) );
            
            if ( iOperationState != EStateComplete &&
                 iOperationState != EStateCancelled )             
                {
                if ( iDownload ) 
                    {                    
                    err = iDownload->Start();                
                    if ( err == KErrNone ) 
                        {
                        iDownloadState = ENcdDownloadInProgress;
                        iOperationState = EStateRunning;
                        }                
                    }
                else 
                    {
                    DLTRACE(("No download, should Start"));
                    iDownloadState = ENcdDownloadStopped;
                    iOperationState = EStateStopped;
                    requireStart = ETrue;
                    }
                }
            completionId = ENCDOperationMessageCompletionResume;
            
            break;
            }
            
        default:
            {
            DLTRACE(("Calling baseclass"));
            // Call implementation in the base class
            CNcdBaseOperation::ReceiveMessage( aMessage, aFunctionNumber );
            DLTRACEOUT(( "Called baseclass" ));
            return;
            }
        }
        
    
    if ( iDownload ) 
        {        
        iProgress = iDownload->Progress();        
        }
        
    iProgress.iState = iDownloadState;
    if ( !iPendingMessage && 
          iDownloadState != ENcdDownloadPaused )
        {
        DLTRACE(("No pending message, ask for one"));        
        if ( requireStart ) 
            {
            iProgress.iState = KNcdDownloadStartMessageRequired;    
            }
        else
            {
            iProgress.iState = KNcdDownloadContinueMessageRequired;    
            }
        }
    
    if ( err == KErrNone ) 
        {        
        TRAP( err, SaveStateL() );
        }
    
    // Complete pause/resume message
    // If this fails then it fails
    CompleteMessage( aMessage, completionId, iProgress, err );

    // Ensure that iState stays valid
    iProgress.iState = iCurrentFile;
    iProgress.iOperationId = iTotalFileCount;

    // The operation will halt if we don't do this because it has been paused
    // when there was a pending event so the proxy side won't send a new
    // message until we complete the pending message
    if ( iPendingMessage && 
         requireStart ) 
        {
        DLTRACE(("There was a pending message when resume was called -> Run"));
        RunOperation();
        }

    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::CounterPartLost( 
    const MCatalogsSession& aSession )
    {
    DLTRACEIN((""));
    iDeleting = ETrue;
    CNcdBaseOperation::CounterPartLost( aSession ); 
    }


// ---------------------------------------------------------------------------
// Externalize the download
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));
    
    // Externalize node id
    iNode->Identifier().ExternalizeL( aStream );
    
    // Externalize metadata id
    iNode->NodeMetaDataL().Identifier().ExternalizeL( aStream );
    
    ExternalizeDesL( *iStorageUid, aStream );
    
    // Externalize node type so that we can create a correct kind of
    // a temp node when we restore the download
    ExternalizeEnumL( CNcdNodeFactory::NodeTypeL( *iNode ), aStream );
    
    if ( iSessionId )
        {        
        ExternalizeDesL( *iSessionId, aStream );
        }
    else
        {
        ExternalizeDesL( KNullDesC, aStream );
        }
    
    aStream.WriteInt32L( iDownloadIndex );
    
    aStream.WriteInt32L( iContentDownloadState );
    
    aStream.WriteInt32L( iDownloadState );
    
    aStream.WriteInt32L( iDependenciesUpdated );
    
    if ( iDownload )
        {
        DLTRACE(("Download exists"));
        aStream.WriteInt8L( 1 );
        iDownload->ExternalizeL( aStream );
        }
    else
        {
        DLTRACE(("No download"));
        // no download 
        aStream.WriteInt8L( 0 );
        }
    }


// ---------------------------------------------------------------------------
// Internalize the download
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    iIsOk = EFalse;
    CNcdNodeIdentifier* nodeId = CNcdNodeIdentifier::NewLC( aStream );
    DLINFO(( _L("Internalized nodeid: %S::%S"), &nodeId->NodeNameSpace(),
        &nodeId->NodeId() ));
    
    CNcdNodeIdentifier* metadataId = CNcdNodeIdentifier::NewLC( aStream );
    DLINFO(( _L("Internalized nodeid: %S::%S"), &metadataId->NodeNameSpace(),
        &metadataId->NodeId() ));
    
    InternalizeDesL( iStorageUid, aStream );
    
    CNcdNodeFactory::TNcdNodeType nodeType( CNcdNodeFactory::ENcdNodeItem );
    InternalizeEnumL( nodeType, aStream );

    CNcdNodeManager::TNcdTemporaryNodeType tempNodeType( 
        CNcdNodeManager::ENcdTemporaryNodeItem );

    switch( nodeType ) 
        {
        case CNcdNodeFactory::ENcdNodeItem:
            {
            tempNodeType = CNcdNodeManager::ENcdTemporaryNodeItem;
            break;
            }
        
        case CNcdNodeFactory::ENcdNodeFolder:
            {
            tempNodeType = CNcdNodeManager::ENcdTemporaryNodeFolder;
            break;
            }
                
        default:
            {
            DLERROR(("Corrupt data, leave"));
            User::Leave( KErrCorrupt );
            }
        };
    
    // Get node pointer and NodeDownload-pointer

    DLINFO(( "Node was not found. Must create a temp node" ));
    // Create temp node and update temp node usage status
    // nodeId is a temp node id because content downloads always use
    // temp nodes so we don't have to create a new temp node id from metadata id
    iNode = &iNodeManager->CreateTemporaryNodeL( 
            *nodeId, tempNodeType, ETrue );
   
    DLTRACE(("Updating access point"));
    UpdateAccessPointsL( *nodeId );
        
    CleanupStack::PopAndDestroy( 2, nodeId ); // metadataId, nodeId
    
    DLTRACE(("Getting metadata reference"));
    // Notice that the node has always some kind of metadata
    // when content is available
    CNcdNodeMetaData& metaData( iNode->NodeMetaDataL() );
    iNodeDownload = &metaData.DownloadL();    
    

    // Sets iSessionId only if the original session ID != NULL
    HBufC* sessionId = NULL;
    InternalizeDesL( sessionId, aStream );
    if ( *sessionId == KNullDesC )
        {
        delete sessionId;
        sessionId = NULL;
        }
    iSessionId = sessionId;
    
    iDownloadIndex = aStream.ReadInt32L();
    
    iContentDownloadState = static_cast<TContentDownloadState>( 
        aStream.ReadInt32L() );
    
    iDownloadState = static_cast<TNcdDownloadState>( 
        aStream.ReadInt32L() );
    
    if ( iDownloadState != ENcdDownloadComplete &&
         iDownloadState != ENcdDownloadFailed ) 
        {
        DLTRACE(("Setting iDownloadState to ENcdDownloadPaused"));
        iDownloadState = ENcdDownloadPaused;
        }
    
    iDependenciesUpdated = aStream.ReadInt32L();
        
    TInt8 downloadExists = aStream.ReadInt8L();
    if ( downloadExists )
        {
        DLTRACE(("Internalizing download suboperation"));
        TRAPD( err, 
            iDownload = CNcdDownloadSubOperation::NewL(
                iGeneralManager,
                iHttpSession, 
                *this, 
                aStream,
                iSession ) );
        
        if ( err == KErrNone ) 
            {
            // Register download to report manager, iReportId will be the old one
            // if the manager still has the old report
            HBufC* uri = ConvertUtf8ToUnicodeL( iDownload->HttpOperation().Uri() );
            CleanupStack::PushL( uri );

            // This also update the accesspoint though that's unnecessary
            // since it has been saved by the report itself
            RegisterDownloadL( *uri, metaData.Identifier() );
            CleanupStack::PopAndDestroy( uri );    
            }
        // CNcdDownloadSubOperation::NewL leaves with KErrNotFound if the
        // platform download is not found but we don't want to let it
        // go any further than this
        else if ( err != KErrNotFound ) 
            {
            DLERROR(("Leaving with err: %d", err ));
            User::Leave( err );
            }
        }
    iIsOk = ETrue;    
    }


// ---------------------------------------------------------------------------
// Checks if the download matches the given criteria
// ---------------------------------------------------------------------------
//
TBool CNcdContentDownloadOperation::MatchDownload( 
    const CNcdNodeIdentifier& aId, 
    TNcdDownloadDataType& aType, 
    TInt /* aIndex */ ) const
    {
    return aType == ENcdContentDownload && 
        aId.Equals( MetadataId() );
         //&& aIndex == iDownloadIndex;
    }


// ---------------------------------------------------------------------------
// Node ID getter
// ---------------------------------------------------------------------------
//
const CNcdNodeIdentifier& CNcdContentDownloadOperation::NodeId() const
    {
    return iNode->Identifier();
    }
    

// ---------------------------------------------------------------------------
// Metadata id getter
// ---------------------------------------------------------------------------
//
const CNcdNodeIdentifier& CNcdContentDownloadOperation::MetadataId() const
    {
    
    return iNode->NodeMetaData()->Identifier();
    }


// ---------------------------------------------------------------------------
// Ok status getter
// ---------------------------------------------------------------------------
//
TBool CNcdContentDownloadOperation::IsOk() const
    {
    return iIsOk;
    }


// ---------------------------------------------------------------------------
// Current download index
// ---------------------------------------------------------------------------
//
TInt CNcdContentDownloadOperation::CurrentDownload() const
    {
    return iDownloadIndex;
    }

   
// ---------------------------------------------------------------------------
// Progress
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::Progress( CNcdBaseOperation& aOperation )
    {
    DLTRACEIN((""));
    
    // Report operation start to report manager
    if ( aOperation.Progress().iState == ENcdDownloadStarted )
        {
        TNcdReportStatusInfo info( ENcdReportStart, KErrNone );
        TRAPD( err, ReportStatusL( info, EFalse ) );
        if ( err != KErrNone ) 
            {
            DLERROR(("Error: %d", err));
            iError = err;
            RunOperation();
            }
        }
    
    if ( !iDescriptorDownload && iContentDownloadState == EContentDownload )
        {        
        iProgress = aOperation.Progress();   
        iProgress.iState = iCurrentFile;
        iProgress.iOperationId = iTotalFileCount;
        iUnhandledEvent = ETrue; 
        RunOperation();    
        }
    }

// ---------------------------------------------------------------------------
// QueryReceived
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::QueryReceived( 
    CNcdBaseOperation& /* aOperation */,
    CNcdQuery* /* aQuery */ )
    {
    DLTRACEIN((""));
    }

// ---------------------------------------------------------------------------
// OperationComplete
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::OperationComplete( 
    CNcdBaseOperation* aOperation, 
    TInt aError )
    {
    DLTRACEIN(( "aError: %d", aError ));
    iProgress = aOperation->Progress();
    iProgress.iState = iCurrentFile;
    iProgress.iOperationId = iTotalFileCount;
    
    // Ignore errors in notification sending
    if ( aError == KErrNone  ) 
        {             
        TRAPD( err,
            {

            if ( iDescriptorDownload )
                {
                DLTRACE(("Descriptor download finished"));
                DASSERT( aOperation == static_cast<CNcdBaseOperation*>( 
                    iDescriptorDownload ) );
                FinishDescriptorDownloadL();
                ReleaseDownload( aOperation );                
                
                // Initialize & start next descriptor/content download
                InitializeDownloadL( iDownloadIndex );
                    
                StartDownloadL();
                }
            else
                {                
                TBool contentDownloaded = FinishDownloadL();
                 
                ReleaseDownload( aOperation );
                
                if ( contentDownloaded && iContentDownloadState == EDownloadDone ) 
                    {                    
                    iContentDownloadState = ENoDownload;
                    DLTRACE(("Content file downloaded"));
                    ++iDownloadIndex;
                    if ( iDownloadIndex == iNodeDownload->DownloadInfo().Count() )
                        {
                        DLTRACE(("Download is complete"));
                        iDownloadState = ENcdDownloadComplete;       
                        
                        // Remove state information from the download db
                        RemoveTempInfoL();                 
                        }
                    else
                        {
                        DLTRACE(("Starting the next file from index: %d", 
                            iDownloadIndex ));
                        iCurrentFile++;
                        iDownloadState = ENcdDownloadInProgress;
                        iUnhandledEvent = ETrue;
                        iStartNextFile = ETrue;
                        }
                    }
                else 
                    {
                    DLTRACE(("Start downloading the actual content file/send notification"));
                    iDownloadState = ENcdDownloadInProgress;
                    iStartNextFile = ETrue;
                    }
                }
            });
        
        if ( err != KErrNone ) 
            {
            if ( err == KNoDownloads ) 
                {
                DLTRACE(("Everything has already been downloaded & installed"));
                iError = KErrNone;
                iDownloadState = ENcdDownloadComplete;
                iContentDownloadState = ENoDownload;
                }
            else 
                {
                DLERROR(("Error %d when handling a completed download", err ));
                iError = err;
                iDownloadState = ENcdDownloadFailed;
                }
            }
        RunOperation();
        }
    else
        {
        DLERROR(( "Handling error: %d", aError ));
        
        // Already handling an error so we can forget about this one
        TNcdReportStatusInfo info( ENcdReportFail, aError );
        TRAP_IGNORE( ReportStatusL( info ) );
        ReleaseDownload( aOperation );
        
        iError = aError;
        iDownloadState = ENcdDownloadFailed;
        RunOperation();
        }            
    }

    
// ---------------------------------------------------------------------------
// RunOperation
// ---------------------------------------------------------------------------
//
TInt CNcdContentDownloadOperation::RunOperation()
    {
    DLTRACEIN(( "Pending message: %X", iPendingMessage ));
    
    if ( !iPendingMessage ) 
        {      
        DLTRACE(("No pending message"));  
        return KErrNotReady;
        }
     
    TInt err = KErrNone;
    DLTRACE(("DL state: %i", iDownloadState ));

    switch( iDownloadState ) 
        {
        // Start the download
        case ENcdDownloadStopped: 
            {            
            DLTRACE(("ENcdDownloadStopped"));
            TRAP( err,
                {               
                InitializeDownloadL( iDownloadIndex );                
                DLTRACE(("Starting the download"));                
                StartDownloadL();                
                });
            
            if ( err == KNoDownloads ) 
                {
                DLTRACE(("No downloads"));
                iDownloadState = ENcdDownloadComplete;
                RunOperation();
                // error handled
                err = KErrNone;
                }
            DLTRACE(("ENcdDownloadStopped done, err: %d", err));
            break;
            }
        
        case ENcdDownloadInProgress: 
            {
            DLTRACE(("ENcdDownloadInProgress"));
            if ( iUnhandledEvent ) 
                {   
                CompleteMessage( iPendingMessage,
                    ENCDOperationMessageCompletionProgress, 
                    iProgress, err );
                iUnhandledEvent = EFalse;
                }
                
            if ( iStartNextFile ) 
                {
                DLTRACE(("Starting the next file"));
                iStartNextFile = EFalse;

                TRAP( err,
                    {                    
                    InitializeDownloadL( iDownloadIndex );
                    DLTRACE(("Starting the download"));
                    StartDownloadL();
                    });
                    
                if ( err == KNoDownloads ) 
                    {
                    DLTRACE(("No more downloads"));
                    iDownloadState = ENcdDownloadComplete;
                    RunOperation();
                    // error handled
                    err = KErrNone;
                    }
                }
                
            DLTRACE(("ENcdDownloadInProgress done"));
            break;
            }
            
        case ENcdDownloadComplete:
            {
            DLTRACE(("ENcdDownloadComplete"));            
            CompleteMessage( 
                iPendingMessage,
                ENCDOperationMessageCompletionComplete, 
                iProgress, err ); 

            // This prevents download pausing/resuming problems after
            // the download has been completed
            iOperationState = EStateComplete;
            break;
            }
            
        case ENcdDownloadFailed:
            {
            DLTRACE(("ENcdDownloadFailed"));
            // Send error message in case didn't have any pending
            // messages when the download actually failed
            
            CompleteMessage( iPendingMessage,
                ENCDOperationMessageCompletionError, iProgress,
                iError );

            // This prevents download pausing/resuming problems after
            // the download has been completed
            iOperationState = EStateComplete;

            DLTRACE(("ENcdDownloadFailed done"));
            break;            
            }
            
        case ENcdDownloadPaused:
            {
            DLTRACE(("Operation paused"));
            break;
            }
            
        default: 
            {
            DLTRACE(("Default"));
            DASSERT( 0 );
            }
        }
    
    if ( err != KErrNone )
        {
        DLTRACE(("error: %d", err));
        Cancel();
        iDownloadState = ENcdDownloadFailed;
        iError = err;
        if ( iPendingMessage )
            {
            CompleteMessage( iPendingMessage,
                ENCDOperationMessageCompletionError, iError );
            }
        // call observers
        CompleteCallback();
        }
    DLTRACEOUT(("err: %d", err));
    return err;
    }


// ---------------------------------------------------------------------------
// Initializer
// ---------------------------------------------------------------------------
//
TInt CNcdContentDownloadOperation::Initialize()
    {
    DLTRACEIN( ( "" ) );
    
    TRAPD( err, DoInitializationL() );
    
    if ( err != KErrNone ) 
        {
        DLTRACE( ( "Err: %d", err ) );
        iPendingMessage->CompleteAndRelease( err );
        iPendingMessage = NULL;
        }
    DLTRACEOUT(( "err: %d", err ));
    return err;    
    }


// ---------------------------------------------------------------------------
// Actual initializer
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::DoInitializationL()
    {
    DLTRACEIN(("Writing initialize response"));
    
    // Write the response
    RCatalogsBufferWriter writer;
    writer.OpenLC();
    writer().WriteInt32L( ENCDOperationMessageCompletionInit );
    
    if ( !iDependenciesUpdated ) 
        {
        TRAPD( err, UpdateDependenciesL() );        
        LeaveIfNotErrorL( err, KErrNotFound );
        }
    
    iCurrentFile = 1;
    iTotalFileCount = CalculateMissingFilesL();
     
    writer().WriteInt32L( iTotalFileCount );
    // write pause-status
    writer().WriteInt32L( iDownloadState == ENcdDownloadPaused );
    
    iPendingMessage->CompleteAndReleaseL( writer.PtrL(), KErrNone );
    iPendingMessage = NULL;
    CleanupStack::PopAndDestroy( &writer );

    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdContentDownloadOperation::CNcdContentDownloadOperation( 
    MNcdOperationRemoveHandler& aRemoveHandler,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession, 
    MNcdDownloadReportObserver& aReportObserver,
    MNcdSessionHandler* aSessionHandler, 
    MCatalogsSession& aSession,
    MNcdDatabaseStorage& aDownloadStorage ) : 
        CNcdBaseOperation( aGeneralManager, &aRemoveHandler, EContentDownloadOperation,
            aSession ),
        iHttpSession( aHttpSession ),
        iReportObserver( aReportObserver ),
        iSessionHandler( aSessionHandler ),
        iConfigurationManager( aGeneralManager.ConfigurationManager() ),
        iAccessPointManager( aGeneralManager.AccessPointManager() ),
        iStorage( aDownloadStorage ),
        iContext( aSession.Context() ),
        iDownloadState( ENcdDownloadStopped )
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::ConstructL( 
    const CNcdNodeIdentifier& aNodeId, 
    TInt aDownloadIndex )
    {
    DLTRACEIN(( "DL index: %i", aDownloadIndex ));
    (void) aDownloadIndex; // prevents compiler warning of unused parameter

    ConstructL();    
    
    GenerateStorageUidL();
    
    // Get pointer to node, using NodeL since it leaves if the node is not found
    iNode = &iNodeManager->NodeL( aNodeId );
    
    if ( iSessionHandler && iNode->NodeLink() ) 
        {        
        // Get server URI from the node
        const TDesC& serverUri = iNode->NodeLink()->ServerUri();
        
        if ( iSessionHandler->DoesSessionExist( 
                serverUri,
                aNodeId.NodeNameSpace() ) )
            {        
            // Get session ID for the URI
            iSessionId = iSessionHandler->Session( 
                serverUri,
                aNodeId.NodeNameSpace() ).AllocL();
            }
        }
    
                        
    UpdateAccessPointsL( aNodeId );
    
    // Get NodeDownload-interface
    // Notice that if content is available the node has some kind of
    // metadata.
    CNcdNodeMetaData& metaData( iNode->NodeMetaDataL() );
    iNodeDownload = &metaData.DownloadL();
    
    SaveStateL();
    iIsOk = ETrue;
    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::ConstructL()
    {
    // Call ConstructL for the base class    
    CNcdBaseOperation::ConstructL(); 
    iContentDescriptor = CNcdContentDescriptor::NewL();
    iProgress.iMaxProgress = 1;
    }
    
// ---------------------------------------------------------------------------
// FinishDescriptorDownloadL
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::FinishDescriptorDownloadL()
    {
    DLTRACEIN((""));
    
    DLTRACE(("Reporting successful download"));
    TNcdReportStatusInfo info( ENcdReportSuccess, KErrNone );
    ReportStatusL( info );
    iReportId = KNcdReportNotSupported;
    
    if ( iContentDownloadState == ERightsDownload )
        {                
        const TDesC8& contentType( 
            iDescriptorDownload->HttpOperation().ContentType() );
            
        // Get download info for the current DL
        const MNcdPurchaseDownloadInfo& info = *iNodeDownload->DownloadInfo()[
            iDownloadIndex ];
        
        
        TDataType dataType;        
        
        // Try to find out the correct mime type for the rights object    
        DLTRACE(("Handling rights object"));
        if( contentType.MatchF( KMimeTypeMatchDrmRightsXml8 ) != KErrNotFound ||
            contentType.MatchF( KMimeTypeMatchDrmRightsWbxml8 ) != KErrNotFound )
            {
            DLTRACE(("Rights type: %S", &contentType ));
            dataType = TDataType( contentType );
            }
        else if ( info.RightsType().MatchF( KMimeTypeMatchDrmRightsXml ) != KErrNotFound ||
            info.RightsType().MatchF( KMimeTypeMatchDrmRightsWbxml ) != KErrNotFound )
            {            
            HBufC8* tempBuf = Des16ToDes8LC( info.RightsType() );
            dataType = TDataType( *tempBuf );
            CleanupStack::PopAndDestroy( tempBuf );
            DLINFO(("Rights: %S", &dataType.Des8() ));
            }
                    
        CNcdProviderUtils::InstallationServiceL().AppendRightsL( 
            iDescriptorDownload->Body(), 
            dataType );
        
        }
    else if ( iContentDownloadState == EDescriptorDownload )
        {
        DLTRACE(("Handling downloaded descriptor"));
        delete iDescriptor;
        iDescriptor = NULL;
        iDescriptor = iDescriptorDownload->Body().AllocL();    

        HBufC* mime = Des8ToDes16L( 
            iDescriptorDownload->HttpOperation().ContentType() );
            
        TDescriptorType descType( MatchDescriptor( *mime ) );
        delete mime;
            
        if ( descType == EDescriptorDd ) 
            {
            HandleDescriptorL( KDescriptorTypeOdd, *iDescriptor );
            }
        else 
            {
            HandleDescriptorL( KDescriptorTypeJad, *iDescriptor );
            }
            
        UpdatePurchaseHistoryL( KNullDesC() ); 
        }
    else
        {
        DASSERT( 0 );
        }
    }


// ---------------------------------------------------------------------------
// StartDownloadL
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::StartDownloadL()
    {
    DLTRACEIN((""));    
    
    if ( iDescriptorDownload ) 
        {
        DLTRACE(("Start descriptor download"));
        User::LeaveIfError( iDescriptorDownload->Start() );
        iProgress = iDescriptorDownload->Progress();    
        }
    else        
        {
        DLTRACE(("Start file download"));
        // Set these before start so that if a progress callback is
        // called during start the operation functions correctly in
        // RunOperation
        iDownloadState = ENcdDownloadInProgress;
        iOperationState = EStateRunning;
        TInt err = iDownload->Start();
        if ( err != KErrNone ) 
            {
            DLERROR(("Failed: %d", err))
            iDownloadState = ENcdDownloadFailed;
            iOperationState = EStateComplete;
            User::Leave( err );
            }
        iProgress = iDownload->Progress();    
        }
    
    DLTRACE(("Download started successfully"));
    iDownloadState = ENcdDownloadInProgress;
    iProgress.iState = iCurrentFile;
    iProgress.iOperationId = iTotalFileCount;
    
    iOperationState = EStateRunning;
    SaveStateL();
    }
    

// ---------------------------------------------------------------------------
// FinishDownloadL
// ---------------------------------------------------------------------------
//
TBool CNcdContentDownloadOperation::FinishDownloadL()
    {
    DLTRACEIN((""));
    
    DLTRACE(("Reporting successful download"));
    TNcdReportStatusInfo info( ENcdReportSuccess, KErrNone );
    ReportStatusL( info );
    
    // iDownload doesn't exist if we are sending the notification
    if ( iDownload && !iContentFilename ) 
        {
        DLTRACE(("Get target filename"));
        iContentFilename = iDownload->Config().FullPathLC();
        CleanupStack::Pop( iContentFilename );
        }
                
    HBufC* mimeType = NULL;
    TBool mimeOwned = EFalse;
    if ( iDownload ) 
        {        
        mimeType = Des8ToDes16LC( iDownload->HttpOperation().ContentType() );
        }
    else if ( iMimeType && iMimeType->Length() ) 
        {
        mimeType = iMimeType->AllocLC();           
        }
    else
        {
        mimeType = KNullDesC().AllocLC();
        }        
    
    
    // No MIME type, update from HTTP headers
    if ( !iMimeType || !iMimeType->Length() ) 
        {
        DLTRACE(("Update mime type"));
        delete iMimeType;
        iMimeType = NULL;
        iMimeType = mimeType;
        DLTRACE(( _L("MIME type: %S"), iMimeType ));
        iMimeUpdated = ETrue;
        mimeOwned = ETrue;        
        }

    if ( !iContentMime.Length() ) 
        {
        iContentMime.Set( *iMimeType );
        }

    // Using the actual mime from the response
    TDescriptorType descType( MatchDescriptor( 
        *mimeType ) );
    
    if ( !mimeOwned ) 
        {
        CleanupStack::PopAndDestroy( mimeType );
        }
    else 
        {
        CleanupStack::Pop( mimeType );
        }
    
    // iDownloadType is checked so that the actual content file is not handled like
    // the downloaded descriptor
    if ( descType != EDescriptorUnknown && 
        iDownloadType == EDescriptorUnknown )
        {
        DLTRACE(("Downloaded a descriptor in a file"));
        DASSERT( iContentFilename );
        delete iDescriptor;
        iDescriptor = NULL;
        iDescriptor = ReadFileL( CNcdProviderUtils::FileSession(), 
            *iContentFilename );
        
        // Beware: this debug print can easily panic if the descriptor is long
        // enough
        //DLINFO(( "Descriptor: %S", iDescriptor ));
        CNcdProviderUtils::FileSession().Delete( *iContentFilename );
        delete iContentFilename;
        iContentFilename = NULL;
        
        if ( descType == EDescriptorDd ) 
            {
            HandleDescriptorL( KDescriptorTypeOdd, *iDescriptor );
            }
        else 
            {
            HandleDescriptorL( KDescriptorTypeJad, *iDescriptor );
            }
        
        // Writes the descriptor to purchase history and updates
        // content type
        UpdatePurchaseHistoryL( KNullDesC() ); 
        
        return EFalse;
        }

    // Notice that purchase history has to be updated before sending any
    // notifications to server to prevent situations where purchase history
    // could not be updated after notification sending.
    // (examples of such situations: out of memory, user cancellation)
    // Notice also that the update is not done if download state is
    // ESendNotification to prevent double update (before notification
    // sending and after).
    DASSERT( iContentFilename );
    UpdatePurchaseHistoryL( *iContentFilename );

    // Update node's status from the purchase history
    DLTRACE(("Updating node from purchase history"));
    iNodeManager->DownloadDataHandlerL( iNode->Identifier() );


    SendOmaNotificationL( info );
    
    delete iContentFilename;
    iContentFilename = NULL;
    
    // Reset URI and mime
    iContentUri.Set( KNullDesC );
    iContentMime.Set( KNullDesC );
    iNotificationUri.Set( KNullDesC );
    iDownloadType = EDescriptorUnknown;
    iContentDownloadState = EDownloadDone;
    iReportId = KNcdReportNotSupported;
    SaveStateL();
    return ETrue;
    }
    
    
// ---------------------------------------------------------------------------
// UpdatePurchaseHistoryL
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::UpdatePurchaseHistoryL( 
    const TDesC& aDownloadedFile )
    {    
    DLTRACEIN(("Updating download in index: %d", iDownloadIndex ));
    
    CNcdPurchaseDetails* purchase = GetPurchaseDetailsLC();

    if ( !purchase->DownloadInfo( iDownloadIndex ).ContentMimeType().Length() )
        {
        DLTRACE(( _L("Updating mime type to purchase details, Mime: %S"), 
            &iContentMime ));                
        purchase->DownloadInfo( iDownloadIndex ).SetContentMimeTypeL( 
            iContentMime );        
        }
        
    if ( iDescriptor )
        {
        DLTRACE(("Setting descriptor data, length: %d", iDescriptor->Length() ));

        if ( iDownloadType == EDescriptorDd ) 
            {
            purchase->DownloadInfo( iDownloadIndex ).SetDescriptorTypeL( 
                KDescriptorTypeOdd );
            }
        else if ( iDownloadType == EDescriptorJad )
            {
            purchase->DownloadInfo( iDownloadIndex ).SetDescriptorTypeL( 
                KDescriptorTypeJad );
            }

        // Write the descriptor data only if it's of a known type
        if ( iDownloadType != EDescriptorUnknown )
            {            
            purchase->DownloadInfo( iDownloadIndex ).SetDescriptorDataL(
                *iDescriptor );
            }
           
        delete iDescriptor;
        iDescriptor = NULL;
        }
    
    // This download needs to be handled afterwards
    purchase->DownloadInfo( iDownloadIndex ).SetAttributeL( 
        MNcdPurchaseDownloadInfo::EDownloadAttributeRequiredDownload, 1 );

    
    DASSERT( iDownloadIndex < purchase->DownloadedFiles().MdcaCount() );
    
    // 
    purchase->ReplaceDownloadedFileL( iDownloadIndex, aDownloadedFile );

    DLTRACE(("Saving purchase history"));
    iNodeManager->PurchaseHistory().SavePurchaseL( *purchase, EFalse );
    
    CleanupStack::PopAndDestroy( purchase );
    DLTRACEOUT(("Purchase history updated"));
    }


// ---------------------------------------------------------------------------
// Updates a skipped download to purchase history
// ---------------------------------------------------------------------------
//    
void CNcdContentDownloadOperation::UpdateSkippedDownloadToPurchaseHistoryL(
    TInt aIndex )
    {
    DLTRACEIN(("Updating download in index: %d", aIndex ));
        
    CNcdPurchaseDetails* purchase = GetPurchaseDetailsLC();
    
    purchase->DownloadInfo( aIndex ).SetAttributeL( 
        MNcdPurchaseDownloadInfo::EDownloadAttributeRequiredDownload, 0 );
        
    purchase->ReplaceDownloadedFileL( aIndex, KNullDesC );
    
    DLTRACE(("Saving purchase history"));
    iNodeManager->PurchaseHistory().SavePurchaseL( *purchase, EFalse );
    
    CleanupStack::PopAndDestroy( purchase );
    DLTRACEOUT(("Purchase history updated"));

    }

// ---------------------------------------------------------------------------
// Updates the download headers
// ---------------------------------------------------------------------------
//    
void CNcdContentDownloadOperation::UpdateHeadersL( 
    MCatalogsHttpHeaders& aHeaders )
    {    
    DLTRACEIN((""));
    
    DLTRACE(( _L("Client id: %S"), &iConfigurationManager.ClientIdL( 
        iContext) ));    
    
    // Add ClientId, SSID and sessionId to file downloads    
    aHeaders.AddHeaderL( NcdHttpHeaders::KClientIdHeader, 
        iConfigurationManager.ClientIdL( iContext) );
    aHeaders.AddHeaderL( NcdHttpHeaders::KSsidHeader, 
        iConfigurationManager.SsidL( iContext) );
        
    if ( iSessionId )
        {  
        DLTRACE(( _L("Adding session id: %S"), iSessionId ));      
        aHeaders.AddHeaderL( NcdHttpHeaders::KSessionIdHeader, *iSessionId );
        }
    DLTRACEOUT(( "" ));
    }
    

// ---------------------------------------------------------------------------
// Resets the correct pointer
// ---------------------------------------------------------------------------
//      
void CNcdContentDownloadOperation::ReleaseDownload( 
    CNcdBaseOperation* aOperation )
    {
    if ( aOperation == static_cast<CNcdBaseOperation*>( iDownload ) )
        {
        iDownload = NULL;
        }
    else if ( aOperation == static_cast<CNcdBaseOperation*>( 
        iDescriptorDownload ) )
        {
        iDescriptorDownload = NULL;            
        }
    else
        {
        NCD_ASSERT_ALWAYS( 0, ENcdPanicInvalidArgument );
        }
    aOperation->Close();
    }
        

// ---------------------------------------------------------------------------
// Handles a download descriptor
// ---------------------------------------------------------------------------
//      
void CNcdContentDownloadOperation::HandleDescriptorL( 
    const TDesC& aDescriptorType, const TDesC8& aDescriptor )
    {
    DLTRACEIN((""));
    DLINFO(("Descriptor: %S", &aDescriptor));
    // Start descriptor parsing
    iContentDescriptor->SetDescriptorL( 
        aDescriptorType,
        aDescriptor );
        
    if ( iContentDescriptor->DataUriL().Length() ) 
        {
        DLINFO(( _L("Data URI from descriptor: %S"), 
            &iContentDescriptor->DataUriL() ));
        iContentUri.Set( iContentDescriptor->DataUriL() );        
        }         
        
    if ( iContentDescriptor->MimeType().Length() ) 
        {
        DLINFO(( _L("Setting content mime as: %S"), 
            &iContentDescriptor->MimeType() ));
            
        iContentMime.Set( iContentDescriptor->MimeType() );
        }
    
    // Update type of the download
    iDownloadType = MatchDescriptor( aDescriptorType );   
    
    if ( iContentDescriptor && 
         iContentDescriptor->InstallNotificationUri().Length() )
        {
        // Send notification only for DDs because installer
        // handles JADs
        if ( iDownloadType == EDescriptorDd ) 
            {
            DLTRACEOUT(("Setting state as ESendNotification, using URI from descriptor"));
            iNotificationUri.Set( 
                iContentDescriptor->InstallNotificationUri() );
            }
        else 
            {
            iNotificationUri.Set( KNullDesC );
            }
        }
        
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
void CNcdContentDownloadOperation::InitializeDownloadL( TInt aIndex )
    {
    DLTRACEIN(( "Download index: %d", aIndex ));

    DASSERT( aIndex >= 0 );
    
    // Find the next file that needs to be downloaded
    if ( aIndex >= iNodeDownload->DownloadInfo().Count() ||
         SkipInstalledFilesL() ) 
        {
        DLTRACEOUT(("Everything has been installed already"));
        User::Leave( KNoDownloads );
        }
    
    aIndex = iDownloadIndex;
        
    // Get download info for the current DL
    const MNcdPurchaseDownloadInfo& info = *iNodeDownload->DownloadInfo()[
        aIndex ];

    
    HBufC* clientDataPath = CNcdProviderUtils::EngineConfig().ClientDataPathLC( 
        iGeneralManager.FamilyName(), EFalse );
    
    DASSERT( clientDataPath );
    DLINFO(( _L("Client data path: %S"), clientDataPath ));  
    WouldDiskSpaceGoBelowCriticalLevelL( *clientDataPath,
        CNcdProviderUtils::FileSession(), 
        info.ContentSize() );
    
    DLTRACE(("Reset progress to -1"));
    iProgress.iState = iCurrentFile;
    iProgress.iOperationId = iTotalFileCount;
    
    iProgress.iProgress = -1;
    iProgress.iMaxProgress = -1;
    
    const CNcdNodeIdentifier& identifier = 
        iNode->NodeMetaDataL().Identifier();
    
    // Handle required descriptors    
    if ( iContentDownloadState < ERightsDownload && info.RightsUri().Length() ) 
        {
        DLTRACE(( _L("Creating descriptor download operation for Rights-URI: %S"),
            &info.RightsUri() ));
        iContentDownloadState = ERightsDownload;
        iDescriptorDownload = CNcdDescriptorDownloadSubOperation::NewL( 
            iGeneralManager,
            iHttpSession, 
            info.RightsUri(), 
            *this, 
            iSession );
        
        iDescriptorDownload->Config().SetConnectionMethod( iApId );
        UpdateHeadersL( iDescriptorDownload->RequestHeaders() );
        CleanupStack::PopAndDestroy( clientDataPath );

        RegisterDownloadL( info.RightsUri(), identifier );
        return;
        }
        
    else if ( info.RightsUri().Length() <= 0 && info.RightsType().Length() )
        {
        DLERROR(( _L("Rights type set but no Rights Uri available -> ERROR!") ));
        User::Leave( KErrNotFound );
        }
    
    // Handle descriptor embedded in the protocol response
    // Embedded descriptors don't have DescriptorUris
    else if ( iContentDownloadState < EEmbeddedDescriptor && 
              info.DescriptorData().Length() && 
              !info.DescriptorUri().Length() ) 
        {
        DLTRACE(("Handling embedded descriptor"));
        HandleDescriptorL( info.DescriptorType(), info.DescriptorData() );
        iContentDownloadState = EEmbeddedDescriptor;                
        }
    // Handle descriptor URI from the protocol response
    else if ( iContentDownloadState < EDescriptorDownload &&
              info.DescriptorUri().Length() ) 
        {
        DLTRACE(( _L("Download descriptor from URI: %S"),
            &info.DescriptorUri() ));
            
        iDescriptorDownload = CNcdDescriptorDownloadSubOperation::NewL( 
            iGeneralManager,
            iHttpSession, 
            info.DescriptorUri(), 
            *this, 
            iSession );
                    
        iDescriptorDownload->Config().SetConnectionMethod( iApId );
        
        UpdateHeadersL( iDescriptorDownload->RequestHeaders() );
        iContentDownloadState = EDescriptorDownload;        
        CleanupStack::PopAndDestroy( clientDataPath );

        RegisterDownloadL( info.DescriptorUri(), identifier );
        // Don't start content download just yet
        return;
        }
    
    // Note: embedded descriptor handling continues in here -> no "else if"
    if ( iContentDownloadState <= EContentDownload )
        {
        
        // Content download
        DLTRACE(("Handling content download"));
        if ( !iContentUri.Length() ) 
            {            
            iContentUri.Set( info.ContentUri() );
            
            DLTRACE(( _L("Content URI from protocol response: %S"),
                &iContentUri ));
            }
        
        if ( !iContentMime.Length() )
            {
            iContentMime.Set( info.ContentMimeType() );
            DLTRACE(( _L("Content mimetype from protocol response: %S"),
                &iContentMime ));
                
            // Dependencies and upgrades don't necessarily share the mime type
            // with the actual content so we don't update it from the protocol for
            // them        
            if ( !iContentMime.Length() && 
                 info.ContentUsage() != MNcdPurchaseDownloadInfo::EDependency ) 
                {
                const CNcdNodeContentInfo* contentInfo = NULL;
                
                TRAP_IGNORE( contentInfo = 
                    &iNode->NodeMetaDataL().ContentInfoL() );
                if ( contentInfo ) 
                    {                    
                    DLTRACE(( _L("Content mimetype from content info: %S"),
                        &iContentMime ));                            
                    iContentMime.Set( contentInfo->MimeType() );
                    }
                }
            }
        iContentDownloadState = EContentDownload;
        
        if ( info.InstallNotificationUri().Length() ) 
            {
            iNotificationUri.Set( info.InstallNotificationUri() );
            DLINFO(( _L("Set install notification URI: %S"), 
                &iNotificationUri ));
            }

        iDownload = CNcdDownloadSubOperation::NewL( 
            iGeneralManager,
            iHttpSession,
            iContentUri,
            *clientDataPath,
            *this,
            iSession );
                        
        iDownload->HttpOperation().SetContentTypeL( iContentMime );
           
        // Force HTTP header getting with transactions because DL manager doesn't
        // support content-disposition for file names etc.
        
        iDownload->HttpOperation().SetHeaderMode( 
            ECatalogsHttpHeaderModeForceHead );        
        iDownload->Config().SetConnectionMethod( iApId );
        
        // Ensure that we use HEAD when downloading content files from
        // URIs that we got from descriptors even if HEAD has been
        // disabled
        if ( iDownloadType != EDescriptorUnknown ) 
            {
            iDownload->Config().SetOptions(
                iDownload->Config().Options() & ~ECatalogsHttpDisableHeadRequest );
            }
        
        // Get mime type set in the protocol/descriptor
        AssignDesL( iMimeType, info.ContentMimeType() );
        iMimeUpdated = EFalse;
        
        UpdateHeadersL( iDownload->RequestHeaders() );    
        
        const CNcdNodeIdentifier& identifier = 
            iNode->NodeMetaDataL().Identifier();

        RegisterDownloadL( iContentUri, identifier );
        }

    CleanupStack::PopAndDestroy( clientDataPath );    
    }
  
  
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
TBool CNcdContentDownloadOperation::IsFileInstalledL( 
    TInt aIndex, 
    TBool aCheckOnly )
    {
    DLTRACEIN((""));
    const MNcdPurchaseDownloadInfo& info = 
        *iNodeDownload->DownloadInfo()[ aIndex ];

    // AttributeInt32L leaves only with KErrNotFound and if that happens,
    // it means that this file has not been handled before -> Buy
    TRAPD( isFirstDownload, info.AttributeInt32L( 
        MNcdPurchaseDownloadInfo::EDownloadAttributeRequiredDownload ) );

    // If handling buy, we can not skip content downloads
    if ( isFirstDownload &&
         info.ContentUsage() == MNcdPurchaseDownloadInfo::EDownloadable )
        {
        DLTRACEOUT(("Not a dependency"));
        return EFalse;
        }

    // If launcher is defined but there's no URI, there's no point
    // checking whether it is installed or not because can't download it
    // anyway
    if ( !info.ContentUri().Length() &&
         IsOneOf( info.ContentUsage(), 
            MNcdPurchaseDownloadInfo::ELauncher,
            MNcdPurchaseDownloadInfo::ELauncherOpen ) ) 
        {
        DLTRACEOUT(("Launcher without URI, skip"));
        // This ensures that for instance MNcdNodeDownload::IsDownloadedL
        // ignores these files         
        if ( !aCheckOnly ) 
            {            
            UpdateSkippedDownloadToPurchaseHistoryL( aIndex );
            }
        return ETrue;
        }

    CNcdNodeInstall* install = NULL;
    TRAPD( err, install = &iNode->NodeMetaData()->InstallL() );
    
    if ( err != KErrNone ) 
        {
        DLERROR(("Error %d when getting CNcdNodeInstall", err));
        if ( err == KErrNotFound ) 
            {
            return EFalse;
            }
        User::Leave( err );
        }
            
    TBool available = 
        ( install->IsContentInstalledL( aIndex, EFalse ) >= ENcdApplicationInstalled );
    
    if ( available && !aCheckOnly ) 
        {
        // This ensures that for instance MNcdNodeDownload::IsDownloadedL
        // ignores these files 
        UpdateSkippedDownloadToPurchaseHistoryL( aIndex );
        }
    DLTRACEOUT(("Content installed: %d", available));
    return available;
    }    


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
TBool CNcdContentDownloadOperation::SkipInstalledFilesL()
    {
    DLTRACEIN((""));
    TBool installed = ETrue;
    TInt count = iNodeDownload->DownloadInfo().Count();
    
    while ( installed && iDownloadIndex < count )
        {        
        DLTRACE(("Checking deps for index: %d", iDownloadIndex ));
            // Get download info for the current DL

        // Check if dependency/upgrade has already been installed
        if ( !IsFileInstalledL( iDownloadIndex, EFalse ) ) 
            {
            installed = EFalse;            
            }
        else 
            {            
            ++iDownloadIndex;
            }
        }
    
    if ( installed ) 
        {
        DLTRACE(("All files have been downloaded or installed"));
        //iOperationState = EStateComplete;
        iDownloadState = ENcdDownloadComplete;
        }
    return installed;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
TInt CNcdContentDownloadOperation::CalculateMissingFilesL()
    {
    DLTRACEIN((""));    
    TInt count = iNodeDownload->DownloadInfo().Count();    
    TInt missing = 0;
    
    while ( count-- )
        {        
        // Check if the file is already installed
        if ( !IsFileInstalledL( count, ETrue ) ) 
            {
            missing++;
            }
        }
    
    DLTRACEOUT(("Missing: %d files", missing));
    return missing;
    }
        

// ---------------------------------------------------------------------------
// Saves the current state of the download to db
// ---------------------------------------------------------------------------
//      
void CNcdContentDownloadOperation::SaveStateL()
    {
    DLTRACEIN((""));
    DASSERT( iStorageUid );
    // Externalize the download
    MNcdStorageItem* item = iStorage.StorageItemL( *iStorageUid, 
        NcdProviderDefines::ENcdDownloadData );
    item->SetDataItem( this );
    item->OpenL();
    item->WriteDataL();
    
    item->SaveL();
    iStorage.CommitL();
    DLTRACEOUT(("Download state saved"));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
void CNcdContentDownloadOperation::GenerateStorageUidL()
    {    
    DLTRACEIN((""));
    
    // Enough for timestamp (64bit) + rand (32bit)
    HBufC* id = HBufC::NewLC( NcdProviderDefines::KClientIdMaxLength );  

    TTime now;
    now.HomeTime();
    TInt64 int64 = now.Int64();
    
    TInt64 ptrInt = reinterpret_cast<TInt64>( this );
    // Use both timestamp and free disk space to get a unique seed
    TInt rand = Math::Rand( ptrInt );

    TPtr ptr( id->Des() );


    ptr.NumFixedWidth( int64 >> 32, EHex, 8 );
    ptr.AppendNumFixedWidth( int64 & 0xffffffff, EHex, 8 );
    ptr.AppendNumFixedWidth( rand, EHex, 8 );
    
    delete iStorageUid;    
    iStorageUid = id;
    CleanupStack::Pop( id );
    }


// ---------------------------------------------------------------------------
// Removes state information from the download db
// ---------------------------------------------------------------------------
//      
void CNcdContentDownloadOperation::RemoveTempInfoL()
    {
    DLTRACEIN((""));
    MNcdStorageItem* item = iStorage.StorageItemL( *iStorageUid, 
        NcdProviderDefines::ENcdDownloadData );
    
    item->RemoveFromStorageL();
    iStorage.CommitL();
    DLTRACEOUT(("Temp info removed successfully"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
CNcdContentDownloadOperation::TDescriptorType 
    CNcdContentDownloadOperation::MatchDescriptor( 
    const TDesC& aMimeType ) const
    {
    DLTRACEIN((""));
    if ( aMimeType.MatchF( KMimeTypeMatchJad ) != KErrNotFound ||
         aMimeType.CompareF( KDescriptorTypeJad ) == 0 )
        {
        DLTRACEOUT(("Jad"));
        return EDescriptorJad;
        }
    else if ( aMimeType.MatchF( KMimeTypeMatchOdd ) != KErrNotFound ||
              aMimeType.CompareF( KDescriptorTypeOdd ) == 0 )
        {
        DLTRACEOUT(("DD"));
        return EDescriptorDd;
        }
    DLTRACEOUT(("Unknown/not a descriptor"));
    return EDescriptorUnknown;
    }


// ---------------------------------------------------------------------------
// GetPurchaseDetailsLC
// ---------------------------------------------------------------------------
//    
CNcdPurchaseDetails* CNcdContentDownloadOperation::GetPurchaseDetailsLC()
    {
    DLTRACEIN((""))    
        
    return NcdPurchaseHistoryUtils::PurchaseDetailsLC( 
        iNodeManager->PurchaseHistory(), 
        iContext.FamilyId(),
        iNode->NodeLinkL().MetaDataIdentifier(),
        EFalse );
    }
    
    
// ---------------------------------------------------------------------------
// UpdateAccessPointsL
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::UpdateAccessPointsL( 
    const CNcdNodeIdentifier& aNodeId )
    {
    DLTRACEIN((""));
    // Get origin node id from purchase history
    CNcdPurchaseDetails* purchase = GetPurchaseDetailsLC();
    
    // Create origin identifier
    CNcdNodeIdentifier* originIdentifier = CNcdNodeIdentifier::NewL(
        aNodeId.NodeNameSpace(), purchase->OriginNodeId(), aNodeId.ClientUid() );
    CleanupStack::PopAndDestroy( purchase );
    
    CleanupStack::PushL( originIdentifier );
    
    // Get download ap
    TUint32 apId = 0;
    
    TInt error = iAccessPointManager.AccessPointIdL(
        *originIdentifier, 
        MCatalogsAccessPointManager::EDownload, 
        iContext.FamilyId(), 
        apId );
    
    if ( error == KErrNone ) 
        {
        DLTRACE(( "Setting access point %d for content download", apId ))   
        iApId = TCatalogsConnectionMethod( 
            apId, 
            ECatalogsConnectionMethodTypeAccessPoint );
        }
    
    // Get report ap    
    apId = 0;
    error = iAccessPointManager.AccessPointIdL(
        *originIdentifier, 
        MCatalogsAccessPointManager::EBrowse, 
        iContext.FamilyId(), 
        apId );
        
    if ( error == KErrNone ) 
        {
        DLTRACE(( "Setting access point %d for reports", apId ))   
        iReportAp = TCatalogsConnectionMethod( 
            apId, 
            ECatalogsConnectionMethodTypeAccessPoint );
        }
    
    if ( iReportAp.iId == 0 ) 
        {
        iReportAp = iHttpSession.ConnectionManager().DefaultConnectionMethod();
        }
    CleanupStack::PopAndDestroy( originIdentifier );

    }


// ---------------------------------------------------------------------------
// Report download status
// ---------------------------------------------------------------------------
//
void CNcdContentDownloadOperation::ReportStatusL( 
    const TNcdReportStatusInfo& aStatus,
    TBool aSendable )
    {
    DLTRACEIN(("aStatus: %d", aStatus.iStatus));
    iReportObserver.ReportDownloadStatusL(
        iReportId,
        aStatus,
        aSendable );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
void CNcdContentDownloadOperation::SendOmaNotificationL( 
    const TNcdReportStatusInfo& aStatus )
    {
    DLTRACEIN((""));
    if ( ( aStatus.iStatus != ENcdReportCancel && aStatus.iStatus != ENcdReportSuccess ) ||
        !iContentUri.Length() ||
        !iNotificationUri.Length() )
        {
        DLTRACEOUT(("Nothing to report"));
        return;
        }
    
    const CNcdNodeIdentifier& identifier = 
        iNode->NodeMetaDataL().Identifier();
    
    TNcdReportStatusInfo info( ENcdReportCreate, KErrNone );
    TNcdReportId omaReportId = iReportObserver.RegisterOmaDownloadL( 
        iContentUri,
        identifier,
        info,
        iNotificationUri );
    
    iReportObserver.SetDownloadReportAccessPoint(
        omaReportId,
        iApId );
        
    DLTRACE(("Report registered, now trigger sending"));
    iReportObserver.ReportDownloadStatusL(
        omaReportId,
        aStatus,
        ETrue );            
    DLTRACEOUT(("Report sending initiated"));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
void CNcdContentDownloadOperation::UpdateDependenciesL()
    {
    DLTRACEIN((""));
    CNcdPurchaseDetails* purchase = GetPurchaseDetailsLC();
            
    iNode->NodeMetaDataL().DependencyL().UpdateDependenciesL( *purchase );
    
    DLTRACE(("Saving purchase history"));
    iNodeManager->PurchaseHistory().SavePurchaseL( *purchase, EFalse );
    
    DLTRACE(("Updating node download from purchase details"));
    iNodeDownload->InternalizeL( *purchase );
    
    DLTRACE(("Updating node install from purchase details"));
    CNcdNodeInstall& install = iNode->NodeMetaData()->InstallL();
    install.InternalizeL( *purchase );
    
    CleanupStack::PopAndDestroy( purchase );
    iDependenciesUpdated = ETrue;
    DLTRACEOUT(("Purchase history updated"));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
void CNcdContentDownloadOperation::GetPausableStateL( 
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    TInt pausableState = KNcdDownloadIsNotPausable;
    if ( iDownload && iDownload->HttpOperation().IsPausable() ) 
        {
        DLTRACE(("Download is pausable"));
        pausableState = KNcdDownloadIsPausable;
        }
                
    aMessage.CompleteAndReleaseL(
        pausableState, 
        KErrNone );    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
void CNcdContentDownloadOperation::RegisterDownloadL(
    const TDesC& aUri,
    const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN((""));
    TNcdReportStatusInfo statusInfo( ENcdReportCreate, KErrNone );
    iReportId = iReportObserver.RegisterDownloadL( 
        aUri,
        aIdentifier,
        statusInfo,
        aIdentifier.ServerUri(),
        aIdentifier.NodeNameSpace() );
    
    iReportObserver.SetDownloadReportAccessPoint( 
        iReportId,
        iReportAp );    
    }
