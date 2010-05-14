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


#include "ncddownloadoperationimpl.h"

#include <s32mem.h>
#include <f32file.h>

#include "catalogsbasemessage.h"
#include "ncdnodemanager.h"
#include "ncdfilehandler.h"
#include "ncdskindownloadhandler.h"
#include "ncdicondownloadhandler.h"
#include "ncdpreviewdownloadhandler.h"
#include "ncdfiledownloadhandler.h"
#include "ncdscreenshotdownloadhandler.h"
#include "ncdfilestoragefilehandler.h"
#include "ncdnodeimpl.h"
#include "ncddatabasefilehandler.h"
#include "ncdnodeidentifier.h"
#include "ncdsessionhandler.h"
#include "ncdhttpheaders.h"
#include "catalogsutils.h"
#include "ncdconfigurationmanager.h"
#include "catalogscontext.h"
#include "ncdproviderdefines.h"
#include "ncdnodeclassids.h"
#include "ncdnodelink.h"
#include "catalogsaccesspointmanager.h"
#include "ncdoperationremovehandler.h"
#include "ncdproviderutils.h"
#include "ncdnodemetadataimpl.h"
#include "catalogshttpincludes.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdDownloadOperation* CNcdDownloadOperation::NewL( 
    MNcdOperationRemoveHandler& aRemoveHandler, 
    TNcdDownloadDataType aType, 
    const CNcdNodeIdentifier& aNodeId,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdSessionHandler* aSessionHandler,
    MNcdStorageClient* aStorageClient,
    const TUid& aClientUid,
    TInt aDownloadIndex,
    MCatalogsSession& aSession )
    {
    CNcdDownloadOperation* self = new( ELeave ) CNcdDownloadOperation( 
        aRemoveHandler,
        aGeneralManager, 
        aHttpSession, 
        aSessionHandler, 
        aType, 
        aClientUid, 
        aSession );
    CleanupClosePushL( *self );
    self->ConstructL( aNodeId, aDownloadIndex, aStorageClient );

    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdDownloadOperation::~CNcdDownloadOperation()
    {
    DLTRACEIN( ( "" ) );
    if ( iDownload ) 
        {
        iDownload->Release();
        iDownload = NULL;
        }
        
    delete iFileHandler;
    delete iDownloadHandler;
    
    delete iSessionId;
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Download config getter
// ---------------------------------------------------------------------------
//
MCatalogsHttpConfig& CNcdDownloadOperation::Config()
    {
    DASSERT( iDownload );
    return iDownload->Config();
    }

// ---------------------------------------------------------------------------
// Node Id getter
// ---------------------------------------------------------------------------
//
const CNcdNodeIdentifier& CNcdDownloadOperation::NodeIdentifier() const
    {
    return iDownloadHandler->NodeId();
    }

// ---------------------------------------------------------------------------
// Cancel
// ---------------------------------------------------------------------------
//
void CNcdDownloadOperation::Cancel() 
    {    
    DLTRACEIN(( "" ));
    if ( iDownload ) 
        {
        // Cancel also releases the dl        
        iDownload->Cancel();
        iDownload = NULL;
        }
    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// ReceiveMessage
// ---------------------------------------------------------------------------
//
void CNcdDownloadOperation::ReceiveMessage( 
    MCatalogsBaseMessage* aMessage,
    TInt aFunctionNumber )
    {
    DLTRACEIN(( "Function: %d", aFunctionNumber ));
    TNcdOperationMessageCompletionId completionId;
    TInt err = KErrNone;
        
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
            
        // Pause download if it's pausable
        case ENCDOperationFunctionPause:
            {
            DLTRACE( ( "ENCDOperationFunctionPause" ) );
            err = iDownload->Pause();              
            if ( err == KErrNone ) 
                {
                iDownloadState = ENcdDownloadPaused;
                }
            completionId = ENCDOperationMessageCompletionPause;
            break;
            }
            
        
        // Resume download
        case ENCDOperationFunctionResume:
            {
            DLTRACE( ( "ENCDOperationFunctionResume" ) );
            
            if ( iDownload->Progress().iState == ECatalogsHttpOpPaused ) 
                {
                err = iDownload->Start();                
                if ( err == KErrNone ) 
                    {
                    iDownloadState = ENcdDownloadInProgress;
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
        
        
    TCatalogsTransportProgress progress( iDownload->Progress() );
    
    iProgress = TNcdSendableProgress( 
        iCurrentFile,
        iTotalFileCount, 
        progress.iProgress,
        progress.iMaxProgress );
    
    // this ensures that the proxy handles pausing correctly
    iProgress.iState = iDownloadState;

    if ( !iPendingMessage && 
          iDownloadState != ENcdDownloadPaused )
        {
        DLTRACE(("No pending message, ask for one"));        
        iProgress.iState = KNcdDownloadContinueMessageRequired;    
        }
    
    // Complete pause/resume message
    if ( CompleteMessage( aMessage, completionId, iProgress, err ) 
        != KErrNone ) 
        {
        DLTRACE( ( "Complete message failed" ) );
        }
        
    iProgress.iState = iCurrentFile;
    iProgress.iOperationId = iTotalFileCount;
    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// Checks if the download matches the given criteria
// ---------------------------------------------------------------------------
//
TBool CNcdDownloadOperation::MatchDownload( 
    const CNcdNodeIdentifier& aId, 
    TNcdDownloadDataType& aType, TInt aIndex ) const
    {
    DLTRACEIN((""));
    if ( aType != iType || 
         aIndex != ( iDownloadHandler->CurrentDownload() - 1 ) ) 
        {
        DLTRACEOUT(("Type or index didn't match"));
        return EFalse;
        }
    
    // Generic file download don't have node nor metadata
    if ( aType != ENcdGenericFileDownload ) 
        {
        DLTRACE(("Not a generic file download"));
        // Compare metadata id's because temp nodes have different node 
        // id's but same metadata id's as normal nodes
        const CNcdNode* node = NULL;    
        TRAP_IGNORE( node = &iDownloadHandler->NodeL() );
        
        if ( node ) 
            {        
            const CNcdNodeMetaData* metadata = node->NodeMetaData();
            if ( metadata && metadata->Identifier().Equals( aId ) )
                {
                return ETrue;
                }
            }
        }
    else // generic file downloads
        {
        DLTRACEOUT(("Generic file download"));
        return aId.Equals( iDownloadHandler->NodeId() );
        }
    return EFalse;
    }


// ---------------------------------------------------------------------------
// HandleHttpEvent
// ---------------------------------------------------------------------------
//
void CNcdDownloadOperation::HandleHttpEventL( 
    MCatalogsHttpOperation& aOperation, 
    TCatalogsHttpEvent aEvent )
    {
    DLTRACEIN( ( "" ) );
    (void) aOperation; // suppresses compiler warning
    
    DASSERT( &aOperation == iDownload );
    DASSERT( aOperation.OperationType() == ECatalogsHttpDownload );

        // minimizing stack usage
        {        
        TCatalogsTransportProgress progress( iDownload->Progress() );
        
        iProgress = TNcdSendableProgress( 
            iCurrentFile,
            iTotalFileCount, 
            progress.iProgress,
            progress.iMaxProgress );
        }        
    
    switch( aEvent.iOperationState ) 
        {
        // Handle completed operation
        case ECatalogsHttpOpCompleted:
            {
            iCurrentFile++;
            DLTRACE(("ECatalogsHttpOpCompleted"));
            if ( iPendingMessage ) 
                {
                DLTRACE( ( "Completing message" ) );
                // Finish the download = move it to destination
                TRAPD( err, iDownloadHandler->FinishDownloadL( 
                    *iDownload, iFileHandler ) );
                
                // Release old download                
                iDownload->Release();
                iDownload = NULL;
                
                if ( iDownloadHandler->CurrentDownload() == 
                     iDownloadHandler->DownloadCount() )
                    {   
                    DLTRACE(("Download(s) finished"));                 
                    CompleteMessage( iPendingMessage,
                        ENCDOperationMessageCompletionComplete,
                        iProgress, err );
                    iDownloadState = ENcdDownloadComplete;
                    iOperationState = EStateComplete;
                    }
                else 
                    {
                    DLTRACE(("More downloads"));

                    
                    // Get the next download
                    TRAP( err, iDownload = iDownloadHandler->DownloadL() );
                    iError = err;
                    if ( err != KErrNone ) 
                        {
                        /**
                         * @ Is this correct?
                         */
                        return;
                        }
                    
                    // Set observer
                    iDownload->Config().SetObserver( this );
                    UpdateHeadersL( iPendingMessage->Session().Context() );
                    err = iDownload->Start( MCatalogsHttpOperation::EAutomaticResume );
                    if ( err >= 0 ) 
                        {
                        iDownloadState = ENcdDownloadInProgress;
                        err = KErrNone;
                        iOperationState = EStateRunning;
                        TCatalogsTransportProgress progress( 
                            iDownload->Progress() );
                        
                        iProgress = TNcdSendableProgress( 
                            iCurrentFile,
                            iTotalFileCount, progress.iProgress,                            
                            progress.iMaxProgress );
                            
                        CompleteMessage( iPendingMessage,
                            ENCDOperationMessageCompletionProgress,
                            iProgress, KErrNone );
                        }
                     else 
                        {
                        /** @ Handle error
                        */
                        iOperationState = EStateComplete;
                        iDownloadState = ENcdDownloadComplete;

                        }
                    
                    }                
                iPendingMessage = NULL;
                }
            else 
                {
                iDownloadState = ENcdDownloadComplete;
                }
            break;
            }
        
        // Handle operation in progress
        case ECatalogsHttpOpInProgress:
            {
            iDownloadState = ENcdDownloadInProgress;
            switch( aEvent.iProgressState ) 
                {
                case ECatalogsHttpResponseBodyReceived:
                    {                    
                    //Notify Proxy about progress                    
                    /*
                    if ( iPendingMessage ) 
                        {            
                                   
                        CompleteMessage( iPendingMessage,
                            ENCDOperationMessageCompletionProgress,
                            iProgress, KErrNone );
                        }
                    */
                    iOperationState = EStateRunning;
                    iUnhandledEvent = ETrue;
                    RunOperation();
                    break;
                    }
                    
                default:
                    {
                    }
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
// HandleHttpError
// ---------------------------------------------------------------------------
//
TBool CNcdDownloadOperation::HandleHttpError(
    MCatalogsHttpOperation& aOperation,
    TCatalogsHttpError aError )
    {    
    DLTRACEIN( ( "Error type: %i, id: %i", aError.iType, aError.iError ) );
    DASSERT( &aOperation == iDownload );
    
    if ( aError.iError == KErrCancel ) 
        {
        aOperation.Cancel();
        }
    else 
        {        
        aOperation.Release();
        }
    iDownload = NULL;
    iError = aError.iError;
    iDownloadState = ENcdDownloadFailed;
    if ( iPendingMessage ) 
        {        
        
        CompleteMessage( iPendingMessage,
            ENCDOperationMessageCompletionError, iProgress, aError.iError );
        
        iOperationState = EStateComplete;   
        }
    DLTRACEOUT((""));
    return ETrue;
    }


// ---------------------------------------------------------------------------
// RunOperation
// ---------------------------------------------------------------------------
//
TInt CNcdDownloadOperation::RunOperation()
    {
    DLTRACEIN(( "Pending message: %X", iPendingMessage ));
    DASSERT( iDownload );
    
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
            
            TRAP( err, UpdateHeadersL( iPendingMessage->Session().Context() ));
            if ( err != KErrNone ) 
                {
                break;
                }
            
            DLTRACE(("Starting the download"));
            err = iDownload->Start();
            if ( err >= 0 ) 
                {
                iDownloadState = ENcdDownloadInProgress;
                err = KErrNone;
                iOperationState = EStateRunning;
                TCatalogsTransportProgress progress( iDownload->Progress() );
                
                // Send progress information
                iProgress = TNcdSendableProgress( iCurrentFile,
                    iTotalFileCount, 
                    progress.iProgress,
                    progress.iMaxProgress );
                }
            DLTRACE(("ENcdDownloadStopped done"));
            break;
            }
        
        case ENcdDownloadInProgress: 
            {
            DLTRACE(("ENcdDownloadInProgress"));
            if ( iUnhandledEvent ) 
                {
                DLTRACE(("Completing progress message"));
                CompleteMessage( iPendingMessage,
                    ENCDOperationMessageCompletionProgress,
                    iProgress, KErrNone );
                iUnhandledEvent = EFalse;
                }
                
            DLTRACE(("ENcdDownloadInProgress done"));
            break;
            }
            
        case ENcdDownloadComplete:
            {
            DLTRACE(("ENcdDownloadComplete"));
            iUnhandledEvent = EFalse;
            // Finish the download = move it to destination
            TRAP( err, iDownloadHandler->FinishDownloadL( *iDownload,
                iFileHandler ) );
            
            // Check if the download was the last
            if ( iDownloadHandler->CurrentDownload() == 
                 iDownloadHandler->DownloadCount() ) 
                {
                // Send completion message in case didn't have any pending
                // messages when the download actually completed
                
                if ( CompleteMessage( iPendingMessage,
                    ENCDOperationMessageCompletionComplete, 
                    iProgress, err ) != KErrNone ) 
                    {
                    DLERROR(( "Message completion failed" ));
                    }
                    
                iOperationState = EStateComplete;
                }
             else 
                {
                // Release old download
                iDownload->Release();
                iDownload = NULL;
                
                // Get the next download
                TRAP( err, iDownload = iDownloadHandler->DownloadL() );
                if ( err != KErrNone ) 
                    {
                    /**
                     * @ Is this correct?
                     */
                    break;
                    }
                
                // Set observer
                iDownload->Config().SetObserver( this );
                TRAP( err, UpdateHeadersL( iPendingMessage->Session().Context() ));
                if ( err != KErrNone ) 
                    {
                    break;
                    }
                err = iDownload->Start( MCatalogsHttpOperation::EAutomaticResume );
                if ( err >= 0 ) 
                    {
                    iDownloadState = ENcdDownloadInProgress;
                    err = KErrNone;
                    iOperationState = EStateRunning;
                    TCatalogsTransportProgress progress( iDownload->Progress() );
                    
                    // Send progress information
                    iProgress = TNcdSendableProgress( iCurrentFile,
                        iTotalFileCount, 
                        progress.iProgress,
                        progress.iMaxProgress );
                        
                    CompleteMessage( iPendingMessage,
                        ENCDOperationMessageCompletionProgress, iProgress, iError );
                    }
                
                }
            iPendingMessage = NULL;
            DLTRACE(("ENcdDownloadComplete done"));
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
            DLTRACE(("ENcdDownloadFailed done"));
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
TInt CNcdDownloadOperation::Initialize()
    {
    DLTRACEIN( ( "" ) );
    DASSERT( iDownloadHandler );
    TRAPD(err,
        {
    
        switch( iType ) 
            {
            // read the target filename and path from the message
            case ENcdSkinDownload: // Flowthrough
                {
                RCatalogsMessageReader reader;
                reader.OpenLC( *iPendingMessage );
                              
                HBufC* filename = NULL;
                DLTRACE(("Internalizing filename"));
                InternalizeDesL( filename, reader() );
                                
                CleanupStack::PopAndDestroy( &reader );
                
                CleanupStack::PushL( filename );
                
                DLTRACE(( _L("Parsing filename: %S"), filename ));
                // Parse the path and set directory and filename
                TParsePtrC parse( *filename );
                iDownload->Config().SetDirectoryL( parse.DriveAndPath() );
                iDownload->Config().SetFilenameL( parse.NameAndExt() );
                
                CleanupStack::PopAndDestroy( filename );
                
                // Just check that we're not already below critical level
                WouldDiskSpaceGoBelowCriticalLevelL( 
                    iDownload->Config().Directory(),
                    CNcdProviderUtils::FileSession(),
                    0 );
                
                break;
                }
                
            default:
                // nothing to initialize for other types
                break;
                
            } // switch
    

    
        DLTRACE(("Writing initialize response"));
        iCurrentFile = 1;
        iTotalFileCount = iDownloadHandler->DownloadCount();
        // Write the response
        RCatalogsBufferWriter writer;
        writer.OpenLC();
        writer().WriteInt32L( ENCDOperationMessageCompletionInit );
        writer().WriteInt32L( iDownloadHandler->DownloadCount() );
        
        // write pause-status
        writer().WriteInt32L( iDownloadState == ENcdDownloadPaused );
        
        iPendingMessage->CompleteAndReleaseL( writer.PtrL(), KErrNone );
        iPendingMessage = NULL;
        CleanupStack::PopAndDestroy( &writer );
        });
        
    if ( err != KErrNone ) 
        {
        DLINFO(("err: %d", err));
        iPendingMessage->CompleteAndRelease( err );
        iPendingMessage = NULL;
        }
    
    DLTRACEOUT(( "err: %d", err ));
    return err;    
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdDownloadOperation::CNcdDownloadOperation( 
    MNcdOperationRemoveHandler& aRemoveHandler,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession, 
    MNcdSessionHandler* aSessionHandler, 
    TNcdDownloadDataType aType,
    const TUid& aClientUid,
    MCatalogsSession& aSession ) : 
        CNcdBaseOperation( aGeneralManager, &aRemoveHandler, EDownloadOperation,
            aSession ),
        iHttpSession( aHttpSession ),
        iSessionHandler( aSessionHandler ),
        iConfigurationManager( aGeneralManager.ConfigurationManager() ),
        iAccessPointManager( aGeneralManager.AccessPointManager() ),
        iType( aType ),
        iDownloadState( ENcdDownloadStopped ),
        iClientUid( aClientUid )
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdDownloadOperation::ConstructL( const CNcdNodeIdentifier& aNodeId, 
    TInt aDownloadIndex,
    MNcdStorageClient* aStorageClient )
    {
    DLTRACEIN(( "DL index: %i", aDownloadIndex ));
    // Call ConstructL for the base class
    CNcdBaseOperation::ConstructL();    

    if ( iSessionHandler && iType != ENcdGenericFileDownload ) 
        {        
        // Ignore error, it's possible that there's no session for URI
        // anymore
        TRAP_IGNORE( 
            {
            // Leaves if the node is not found        
            CNcdNode& node = iNodeManager->NodeL( aNodeId );                    

            // Get server URI from the node
            const TDesC& serverUri = node.NodeLinkL().ServerUri();
            if ( iSessionHandler->DoesSessionExist( serverUri, 
                aNodeId.NodeNameSpace() ) ) 
                {        
                // Get session ID for the URI
                iSessionId = iSessionHandler->Session( serverUri,
                    aNodeId.NodeNameSpace() ).AllocL();
                }
            });
        }
    
        
    // Choose the correct handlers for the download
    switch ( iType ) 
        {
        case ENcdSkinDownload : 
            {
            DLTRACE(("Skin download"));
            iFileHandler = NULL;
            iDownloadHandler = CNcdSkinDownloadHandler::NewL( aNodeId, 
                *iNodeManager,
                iHttpSession );
            break;
            
            }
        case ENcdGenericFileDownload :
            {
            DLTRACE(("Generic file download"));
            // No file handler, Transport takes care of moving the file
            iFileHandler = NULL;
            iDownloadHandler = CNcdFileDownloadHandler::NewL( aNodeId,
                *iNodeManager,
                iHttpSession );
            break;
            }

        case ENcdPreviewDownload :
            {
            DLTRACE(("Preview download"));
            DASSERT( aStorageClient );
            iFileHandler = CNcdFileStorageFileHandler::NewL( *aStorageClient );
            iDownloadHandler = CNcdPreviewDownloadHandler::NewL( aNodeId,
                *iNodeManager,
                iHttpSession );
            break;
            }

        case ENcdIconDownload :
            {
            DLTRACE(("Icon download"));
            DASSERT( aStorageClient );
            iFileHandler = CNcdDatabaseFileHandler::NewL( *aStorageClient,
                NcdNodeClassIds::ENcdIconData );            
            iDownloadHandler = CNcdIconDownloadHandler::NewL( aNodeId,
                *iNodeManager,
                iHttpSession );
            break;
            }

        case ENcdScreenshotDownload :
            {
            DLTRACE(("Screenshot download"));
            DASSERT( aStorageClient );
            iFileHandler = CNcdDatabaseFileHandler::NewL( *aStorageClient,
                NcdNodeClassIds::ENcdScreenshotData );
            iDownloadHandler = CNcdScreenshotDownloadHandler::NewL( aNodeId,
                *iNodeManager,
                iHttpSession );
            break;
            }
        
        default:
            {
            DASSERT( 0 );
            }
        }
    
    // Start downloading from the correct index
    iDownloadHandler->SetCurrentDownload( aDownloadIndex );
    
    // Create the download
    iDownload = iDownloadHandler->DownloadL();
    iDownload->Config().SetObserver( this );
    
    DLTRACE(("Check that we're not below critical level on disk space"));
    
    // Target path for skin download is set in Initialize so
    // we can't check it yet
    if ( iType != ENcdSkinDownload )
        {        
        // Just check that we're not already below critical level
        WouldDiskSpaceGoBelowCriticalLevelL( 
            iDownload->Config().Directory(),
            CNcdProviderUtils::FileSession(),
            0 );
        }
    
    if ( iType == ENcdSkinDownload || iType == ENcdPreviewDownload || 
         iType == ENcdIconDownload || iType == ENcdScreenshotDownload ) 
        {
        // verify the node exists, otherwise call to AccessPointIdL won't work correctly
        DASSERT( iNodeManager->NodePtrL( aNodeId ) != NULL );        
        TUint32 apId;
        TInt error = iAccessPointManager.AccessPointIdL(
            aNodeId, MCatalogsAccessPointManager::EBrowse, iClientUid, apId);
        if ( error == KErrNone ) 
            {
            DLTRACE(( "Setting access point %d for browse", apId ))   
            Config().SetConnectionMethod( 
                TCatalogsConnectionMethod( 
                    apId, ECatalogsConnectionMethodTypeAccessPoint ) );
            }
        }
    
    DLTRACEOUT(( "" ));
    }
    

// ---------------------------------------------------------------------------
// Updates the download headers
// ---------------------------------------------------------------------------
//    
void CNcdDownloadOperation::UpdateHeadersL( const MCatalogsContext& aContext )
    {    
    DLTRACEIN((""));
    
    DLTRACE(( _L("Client id: %S"), &iConfigurationManager.ClientIdL( 
        aContext) ));
    DASSERT( iDownload );
    
    // Add ClientId, SSIS and sessionId to file downloads
    MCatalogsHttpHeaders& headers = iDownload->Config().RequestHeaders();
    headers.AddHeaderL( NcdHttpHeaders::KClientIdHeader, 
        iConfigurationManager.ClientIdL( aContext) );
    headers.AddHeaderL( NcdHttpHeaders::KSsidHeader, 
        iConfigurationManager.SsidL( aContext) );
        
    if ( iSessionId )
        {  
        DLTRACE(( _L("Adding session id: %S"), iSessionId ));      
        headers.AddHeaderL( NcdHttpHeaders::KSessionIdHeader, *iSessionId );
        }
    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CNcdDownloadOperation::GetPausableStateL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    TInt pausableState = KNcdDownloadIsNotPausable;
    if ( iDownload && iDownload->IsPausable() ) 
        {
        DLTRACE(("Download is pausable"));
        pausableState = KNcdDownloadIsPausable;
        }
                
    aMessage.CompleteAndReleaseL(
        pausableState, 
        KErrNone );
    
    }
