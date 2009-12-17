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


#include "ncdloadbundlenodeoperationimpl.h"
#include "ncdoperationfunctionids.h"
#include "catalogsbasemessage.h"
#include "catalogshttpsession.h"
#include "catalogshttpoperation.h"
#include "catalogshttpconfig.h"
#include "catalogsbigdes.h"
#include "catalogsaccesspointmanagerimpl.h"
#include "ncdrequestgenerator.h"

#include "ncdrequestbase.h"
#include "ncdrequestbrowsesearch.h"
#include "ncdrequestconfiguration.h"
#include "ncd_pp_itemref.h"
#include "ncd_pp_folderref.h"
#include "ncd_pp_dataentity.h"
#include "ncd_cp_query.h"
#include "ncd_cp_queryelement.h"
#include "ncd_cp_queryoption.h"

#include "ncdprotocolutils.h"
#include "ncdprotocol.h"
#include "ncdprotocolimpl.h"
#include "ncdparser.h"
#include "ncdnodemanager.h"
#include "ncdproviderdefines.h"
#include "ncdnodeidentifier.h"
#include "ncdnodeclassids.h"
#include "ncdnodefolder.h"
#include "ncdrootnode.h"
#include "ncdbundlefolder.h"
#include "ncdloadnodeoperationimpl.h"
#include "ncd_cp_detail.h"
#include "ncd_cp_clientconfiguration.h"
#include "ncd_cp_error.h"
#include "catalogscontext.h"
#include "ncd_cp_serverdetails.h"
#include "ncdqueryimpl.h"
#include "ncdnodelink.h"
#include "ncd_cp_queryresponseimpl.h"
#include "catalogsutils.h"
#include "ncderrors.h"
#include "ncdconfigurationmanager.h"
#include "ncdoperationremovehandler.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodeiconimpl.h"
#include "ncdsessionhandler.h"
#include "ncd_pp_error.h"
#include "ncdconfigurationkeys.h"
#include "ncdutils.h"
#include "ncdnodeidentifiereditor.h"
#include "ncdnodeidentifier.h"
#include "ncdprotocolstrings.h"
#include "ncdnodeseeninfo.h"
#include "ncdchildentitymap.h"
#include "ncdproviderutils.h"
#include "ncdhttputils.h"
#include "ncdoperationqueue.h"
#include "ncdgeneralmanager.h"
#include "ncd_cp_queryimpl.h"
#include "ncdstring.h"
#include "ncdnodedisclaimer.h"

#include "catalogsdebug.h"


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadBundleNodeOperation* CNcdLoadBundleNodeOperation::NewL(
    const CNcdNodeIdentifier& aNodeIdentifier,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler* aRemoveHandler,
    MNcdOperationQueue& aOperationQueue,
    MCatalogsSession& aSession )
    {
    CNcdLoadBundleNodeOperation* self = CNcdLoadBundleNodeOperation::NewLC(
        aNodeIdentifier,
        aGeneralManager,
        aHttpSession,
        aRemoveHandler,
        aOperationQueue,
        aSession );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadBundleNodeOperation* CNcdLoadBundleNodeOperation::NewLC(
    const CNcdNodeIdentifier& aNodeIdentifier,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler* aRemoveHandler,
    MNcdOperationQueue& aOperationQueue,
    MCatalogsSession& aSession )
    {
    CNcdLoadBundleNodeOperation* self =
        new( ELeave ) CNcdLoadBundleNodeOperation( 
            aGeneralManager,
            aHttpSession, 
            aRemoveHandler, 
            aOperationQueue, 
            aSession );
    CleanupClosePushL( *self );
    self->ConstructL( aNodeIdentifier );
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadBundleNodeOperation::~CNcdLoadBundleNodeOperation()
    {
    DLTRACEIN((""));
    DASSERT( !iNodeDbLocked );
    iConfigManager.RemoveObserver( *this );
    
    iLoadedNodes.ResetAndDestroy();
    
    DLTRACE(("Delete id, buffer"));
    delete iNodeIdentifier;
    
    DLTRACE(("Delete parser"));
    delete iParser;
    if ( iTransaction )
        {
        DLTRACE(("Releasing transaction"));
        iTransaction->Release();
        }
    DLTRACE(("Delete loaded nodes"));
    iLoadedNodes.ResetAndDestroy();
        
    DLTRACE(("Closing suboperations"));
    // Close operations
    for ( TInt i = 0; i < iSubOps.Count(); ++i )
        {
        iSubOps[i]->Close();        
        }
    DLTRACE(("Suboperations closed"));
    iSubOps.Reset();
    iFailedSubOps.Reset();
    iCompletedSubOps.Reset();
    
    DLTRACE(("Deleting content sources"));
    delete iContentSourceMap;
    
    DLTRACE(("Delete conf response buffer"));
    delete iConfigResponseBuf;
        
    if( iConfQuery )
        {
        iConfQuery->InternalRelease();
        }

    iSubOpQuerys.Close();
    
    delete iServerUri;
    
    iChildEntityMaps.ResetAndDestroy();
    
    DLTRACEOUT((""));
    }

const CNcdNodeIdentifier& CNcdLoadBundleNodeOperation::NodeIdentifier() const
    {
    return *iNodeIdentifier;
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadBundleNodeOperation::HandleCancelMessage( MCatalogsBaseMessage* aMessage ) 
    {
    DLTRACEIN((""));
    Cancel();
    CNcdBaseOperation::HandleCancelMessage( aMessage );
    iOperationQueue.QueuedOperationComplete( *this );
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//    
TInt CNcdLoadBundleNodeOperation::CompleteMessage(
    MCatalogsBaseMessage* & aMessage,
    TNcdOperationMessageCompletionId aId,
    const MNcdSendable& aSendableObject,
    TInt aStatus ) 
    {
    DLTRACEIN((""));    
    NotifyCompletionOfQueuedOperation( aId );
    return CNcdBaseOperation::CompleteMessage( aMessage, aId, aSendableObject, aStatus );
    }
    

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//    
TInt CNcdLoadBundleNodeOperation::CompleteMessage(
    MCatalogsBaseMessage* & aMessage,
    TNcdOperationMessageCompletionId aId,
    TInt aStatus )
    {
    DLTRACEIN((""));
    NotifyCompletionOfQueuedOperation( aId );
    return CNcdBaseOperation::CompleteMessage( aMessage, aId, aStatus );
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//    
TInt CNcdLoadBundleNodeOperation::CompleteMessage(
    MCatalogsBaseMessage*& aMessage,
    TNcdOperationMessageCompletionId aId,
    const MNcdSendable& aSendableObject,
    RPointerArray<CNcdNodeIdentifier>& aNodes,
    TInt aStatus )
    {
    DLTRACEIN((""));
    NotifyCompletionOfQueuedOperation( aId );
    return CNcdBaseOperation::CompleteMessage( aMessage, aId, aSendableObject, aNodes, aStatus );
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//    
TInt CNcdLoadBundleNodeOperation::CompleteMessage(
    MCatalogsBaseMessage*& aMessage,
    TNcdOperationMessageCompletionId aId,
    RPointerArray<CNcdExpiredNode>& aExpiredNodes,
    TInt aStatus )
    {
    DLTRACEIN((""));
    NotifyCompletionOfQueuedOperation( aId );
    return CNcdBaseOperation::CompleteMessage( aMessage, aId, aExpiredNodes, aStatus );
    }
    

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CNcdLoadBundleNodeOperation::Start()
    {
    DLTRACEIN((""));
    if ( iOperationState == EStateStopped )
        {
        // Op not yet running, queue it
        iOperationState = EStateRunning;                
        TRAPD( err, iOperationQueue.QueueOperationL( *this ) );        
        return err;
        }
    else
        {
        return KErrInUse;
        }
    }    
    
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadBundleNodeOperation::Cancel()
    {
    DLTRACEIN( ( "" ) );
    if ( iTransaction )
        {
        iTransaction->Cancel();        
        iTransaction = NULL;
        }
    if ( iParser )
        {
        iParser->CancelParsing();
        }    
    for ( TInt i = 0 ; i < iSubOps.Count() ; i++ )
        {
        iSubOps[i]->Cancel();
        iSubOps[i]->Close();
        }
    iSubOps.Reset();
    if ( iNodeDbLocked ) 
        {        
        iNodeManager->UnlockNodeDb( iNodeIdentifier->ClientUid() );
        iNodeDbLocked = EFalse;
        TRAP_IGNORE( iNodeManager->RevertNodeCacheL( *iNodeIdentifier ) );
        }
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadBundleNodeOperation::HandleHttpEventL( 
        MCatalogsHttpOperation& aOperation, 
        TCatalogsHttpEvent aEvent )
    {
    DLTRACEIN((""));
    DASSERT( &aOperation == iTransaction );
    DASSERT( aOperation.OperationType() == ECatalogsHttpTransaction );

    TCatalogsTransportProgress progress( iTransaction->Progress() );
    
    // Are state and id needed?
    iProgress = TNcdSendableProgress( iBundleNodeState,
        iTransaction->OperationId().Id(), progress.iProgress,
        progress.iMaxProgress );

    switch( aEvent.iOperationState ) 
        {
        // Handle completed operation
        case ECatalogsHttpOpCompleted:
            {
            
            iTransaction->Release();
            iTransaction = NULL;
            // Inform parser that no more data will be sent
            iParser->EndL();
            break;
            }     
               
        // Handle operation in progress
        case ECatalogsHttpOpInProgress:
            {
            if( aEvent.iProgressState == ECatalogsHttpResponseBodyReceived )
                {
                // If config response, append the contents to the buffer.
                if ( iBundleNodeState == EReceiveConf ) 
                    {
                    iConfigResponseBuf->InsertL(
                        iConfigResponseBuf->Size(), aOperation.Body() );                        
                    }               

                // send received data to parser
                iParser->ParseL( aOperation.Body() );
                }
            break;
            }
                    
        default:
            {
            break;
            }
        }
    }
    
    
TBool CNcdLoadBundleNodeOperation::HandleHttpError( 
    MCatalogsHttpOperation& aOperation, 
    TCatalogsHttpError aError )    
    {
    DLTRACEIN(("Error type: %d, code: %d", aError.iType, aError.iError ));    
    DASSERT( &aOperation == iTransaction );
    
    aOperation.Release();
    iTransaction = NULL;

    if ( iMasterServerRedirectionState == ERedirecting ) 
        {
        // Return back to original master server uri.
        iConfigManager.RemoveObserver( *this );
        TRAPD(err, iConfigManager.ResetMasterServerAddressL(
            iPendingMessage->Session().Context() ));
        if( err != KErrNone )
            {
            iError = err;
            iBundleNodeState = EFailed;
            }
        else
            {
            iMasterServerRedirectionState = EReverted;
            iBundleNodeState = EConfRequest;
            }
        }
    else
        {
        iError = aError.iError;
        iBundleNodeState = EFailed;
        }
    RunOperation();
    return ETrue;
    }

void CNcdLoadBundleNodeOperation::ParseError( TInt aErrorCode )
    {
    DLTRACEIN(("error:%d", aErrorCode ));
    
    // Handle error only if not handling an error already
    // (cancellation of parsing may cause an unnecessary call to this function).
    if ( iError == KErrNone )
        {
        if ( iMasterServerRedirectionState == ERedirecting ) 
            {
            // Return back to original master server uri.
            iConfigManager.RemoveObserver( *this );
            TRAPD(err, iConfigManager.ResetMasterServerAddressL(
                iPendingMessage->Session().Context() ));
            if( err != KErrNone )
                {
                iError = err;
                iBundleNodeState = EFailed;
                }
            else
                {
                iMasterServerRedirectionState = EReverted;
                iBundleNodeState = EConfRequest;
                }
            }
        else 
            {            
            iBundleNodeState = EFailed;
            iError = aErrorCode;
            }
            
        if ( iTransaction )
            {
            iTransaction->Cancel();
            iTransaction = NULL;
            }
        if ( iParser )
            {
            iParser->CancelParsing();
            }
        
        RunOperation();
        }
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadBundleNodeOperation::ParseCompleteL( TInt aError )
    {
    DLTRACEIN((_L("error:%d"), aError ));
    
    if ( iParser )
        {
        delete iParser;
        iParser = NULL;
        }
    if ( aError != KErrNone )
        {
        DLTRACE(("Parsing error, stop operation!"))
        iError = aError;
        iBundleNodeState = EFailed;
        RunOperation();
        }
    else
        {
        DASSERT( iBundleNodeState == EReceiveConf || iBundleNodeState == EConfRequest );
        if ( iConfQuery )
            {
            DLTRACE(("Query received, let base op handle it"));
            iBundleNodeState = EConfQuery;
            // let base op handle the query
            TRAPD( err, CNcdBaseOperation::QueryReceivedL( iConfQuery ) );
            if ( err != KErrNone ) 
                {
                iError = err;
                iBundleNodeState = EFailed;
                RunOperation();
                }
            }
        else if ( iBundleNodeState == EConfRequest )
            {
            DASSERT( iMasterServerRedirectionState == ERedirecting );
            RunOperation();
            }
        else if ( iContentSourceMap->ContentSourceCount() < 1 ) 
            {
            iError = KNcdErrorNoContentSources;
            iBundleNodeState = EFailed;
            DLINFO(("No content sources received, stop operation!"))
            RunOperation();
            }
        else
            {            
            iBundleNodeState = EBrowseRequest;
            RunOperation();
            }
        }
    DLTRACEOUT((""));
    }

void CNcdLoadBundleNodeOperation::ConfigurationBeginL( const TDesC& aVersion, 
                                                     TInt aExpirationDelta )
    {
    DLTRACEIN((""));

    // set expiration delta for the bundle node
    iNodeManager->NodeL( *iNodeIdentifier ).
        CreateAndSetLinkL().SetValidUntilDelta( aExpirationDelta );
    // Pass to default observer.
    DASSERT( iDefaultConfigurationProtocolObserver != NULL );
    iDefaultConfigurationProtocolObserver->ConfigurationBeginL( aVersion, aExpirationDelta );
    DLTRACEOUT((""));
    }
    
void CNcdLoadBundleNodeOperation::ConfigurationQueryL(
    MNcdConfigurationProtocolQuery* aQuery )
    {
    DLTRACEIN(("Query received"));
    DASSERT( !iConfQuery );
    CleanupDeletePushL( aQuery );
    // create a query object form the protocol entity
    iConfQuery = CNcdQuery::NewL( *aQuery, IsHttpsUri( *iServerUri ) );
    CleanupStack::Pop( aQuery );    
    
	// Pass ownership to default observer.
	DASSERT( iDefaultConfigurationProtocolObserver != NULL );
    iDefaultConfigurationProtocolObserver->ConfigurationQueryL( aQuery );
    DLTRACEOUT((""));
    }

void CNcdLoadBundleNodeOperation::ClientConfigurationL(
    MNcdConfigurationProtocolClientConfiguration* aConfiguration )
    {
    DLTRACEIN((""));
    
    CleanupDeletePushL( aConfiguration );
    if ( !iNodeDbLocked )
        {        
        iNodeManager->LockNodeDbL( iNodeIdentifier->ClientUid() );
        iNodeDbLocked = ETrue;
        }
    DLINFO(("detail count:%d", aConfiguration->DetailCount()));

    // Parse access point data from client configuration response.
    iAccessPointManager.ParseAccessPointDataFromClientConfL(
        *aConfiguration, iPendingMessage->Session().Context().FamilyId() );
    
    for( TInt i = 0 ; i < aConfiguration->DetailCount() ; i++ )
        {
        const MNcdConfigurationProtocolDetail& detail = aConfiguration->DetailL( i );
        DLINFO((_L("detail: id=%S value=%S"), &detail.Id(), &detail.Value() ));
        
        if ( detail.Id() != KContentSources )
            {
            continue;
            }
        const RPointerArray<MNcdConfigurationProtocolDetail>& csDetails =
            detail.Details();
        DLINFO(("csDetails count=%d", csDetails.Count()));
        for ( TInt j = 0 ; j < csDetails.Count() ; j++ )
            {            
            MNcdConfigurationProtocolDetail* csDetail = csDetails[j];
            DLINFO((_L("csDetail: id=%S value=%S"), &csDetail->Id(), &csDetail->Value() ));
            if ( csDetail->Id() != KNameSpace )
                {
                continue;
                }
            
            if ( csDetail->GroupId() == KCatalogBundle ) 
                {
                ParseCatalogBundleL( *csDetail );
                continue;
                }

            }           
        }
    if ( iContentSourceMap->ContentSourceCount() < 1 &&
              iMasterServerRedirectionState == EReverted )
        {
        iError = KNcdErrorNoContentSources;
        iBundleNodeState = EFailed;
        DLINFO(("No content sources received, stop operation!"))
        RunOperation();
        // Leave so that parsing is not continued.
        User::Leave( iError );
        }
    CleanupStack::Pop( aConfiguration );
	// Pass ownership to default observer.
	DASSERT( iDefaultConfigurationProtocolObserver != NULL );
    iDefaultConfigurationProtocolObserver->ClientConfigurationL( aConfiguration );
    DLTRACEOUT((""));
    }
    
void CNcdLoadBundleNodeOperation::ConfigurationDetailsL(
    CArrayPtr<MNcdConfigurationProtocolDetail>* aDetails )
    {
    DLTRACEIN((""));
    
    // Pass ownership to default observer.
	DASSERT( iDefaultConfigurationProtocolObserver != NULL );
	iDefaultConfigurationProtocolObserver->ConfigurationDetailsL( aDetails );
	
	DLTRACEOUT((""));
    }
    
void CNcdLoadBundleNodeOperation::ConfigurationActionRequestL(
    MNcdConfigurationProtocolActionRequest* aActionRequest )
    {
    DLTRACEIN((""));
	// Pass ownership to default observer.
	DASSERT( iDefaultConfigurationProtocolObserver != NULL );
    iDefaultConfigurationProtocolObserver->ConfigurationActionRequestL( aActionRequest );
    DLTRACEOUT((""));
    }

void CNcdLoadBundleNodeOperation::ConfigurationServerDetailsL( MNcdConfigurationProtocolServerDetails* aServerDetails )
    {
    DLTRACEIN((""));
	// Pass ownership to default observer.
	DASSERT( iDefaultConfigurationProtocolObserver != NULL );
    iDefaultConfigurationProtocolObserver->ConfigurationServerDetailsL( aServerDetails );
    DLTRACEOUT((""));
    }

void CNcdLoadBundleNodeOperation::ConfigurationErrorL( MNcdConfigurationProtocolError* aError )
    {
    DLTRACEIN((""));
    iBundleNodeState = EFailed;
    iError = aError->Code();

	// Pass ownership to default observer.
	DASSERT( iDefaultConfigurationProtocolObserver != NULL );
    iDefaultConfigurationProtocolObserver->ConfigurationErrorL( aError );
   
    RunOperation();

    DLTRACEOUT((""));
    }
    
void CNcdLoadBundleNodeOperation::ConfigurationEndL()
    {
    DLTRACEIN((""));
	// Pass to default observer.
	DASSERT( iDefaultConfigurationProtocolObserver != NULL );
    iDefaultConfigurationProtocolObserver->ConfigurationEndL();
    DLTRACEOUT((""));
    }


void CNcdLoadBundleNodeOperation::Progress( CNcdBaseOperation& aOperation )
    {
    (void) aOperation; // suppresses compiler warning
    DASSERT( iBundleNodeState == EReceiveBrowse )
    DASSERT( aOperation.Type() == ELoadNodeOperation )
    }
    
void CNcdLoadBundleNodeOperation::QueryReceived( CNcdBaseOperation& /*aOperation*/,
    CNcdQuery* aQuery )
    {
    DASSERT( iBundleNodeState == EReceiveBrowse )
    TRAPD( err, iSubOpQuerys.AppendL( aQuery ) );
    aQuery->InternalAddRef();
    if( err != KErrNone )
        {
        iError = err;
        iBundleNodeState = EFailed;
        }
    RunOperation();
    }
    
void CNcdLoadBundleNodeOperation::OperationComplete( CNcdBaseOperation* aOperation,
                                                   TInt aError )
    {
    DLTRACEIN(("error=%d", aError));
    DLINFO((("iBundleNodeState = %d"), iBundleNodeState ));
    (void) aError; // suppresses compiler warning

    DASSERT( iBundleNodeState == EReceiveBrowse || 
             iBundleNodeState == EBrowseRequest )
    DASSERT( aOperation->Type() == ELoadNodeOperation )
    
    DLINFO(("subop count: failed:%d completed:%d total:%d",
                iFailedSubOps.Count(), iCompletedSubOps.Count(), iSubOps.Count() ));
    
    TRAPD(err, 
    CNcdLoadNodeOperationImpl* loadOp =
        static_cast<CNcdLoadNodeOperationImpl*>( aOperation );
    DASSERT( loadOp->State() == CNcdLoadNodeOperationImpl::EFailed ||
        loadOp->State() == CNcdLoadNodeOperationImpl::EComplete )
    if ( loadOp->State() == CNcdLoadNodeOperationImpl::EFailed )
        {
        iFailedSubOps.AppendL( loadOp );
        }
    else if ( loadOp->State() == CNcdLoadNodeOperationImpl::EComplete )
        {
        iCompletedSubOps.AppendL( loadOp );
        const RPointerArray<CNcdNodeIdentifier>& loadedNodes = loadOp->LoadedNodes();

        // add loaded nodes from child op to our own array
        for ( TInt i = 0 ; i < loadedNodes.Count() ; i++ )
            {
            CNcdNodeIdentifier* id = CNcdNodeIdentifier::NewLC( *loadedNodes[i] );
            iLoadedNodes.AppendL( id );
            CleanupStack::Pop( id );
            }
        }
    
    if ( iBundleNodeState ==  EReceiveBrowse )
        {
        // call RunOperation only in this state,
        // otherwise RunOperation could call itself immediately
        // after starting a sub op
        // (sub-op start -> error -> complete callback -> run op )
        RunOperation();
        }); //TRAPD    
    if ( err != KErrNone )
        {
        iError = err;
        iBundleNodeState = EFailed;
        RunOperation();
        }
    }
    
void CNcdLoadBundleNodeOperation::ConfigurationChangedL() 
    {
    DLTRACEIN((( "iBundleNodeState: %d"), iBundleNodeState ));
    DASSERT( iBundleNodeState == EReceiveConf )    
    // Master server address changed. Restart operation.
    iBundleNodeState = EConfRequest;
    iMasterServerRedirectionState = ERedirecting;    
    }
    
void CNcdLoadBundleNodeOperation::ReceiveMessage(
    MCatalogsBaseMessage* aMessage,
    TInt aFunctionNumber ) 
    {    
    DLTRACEIN((_L("Handle: %i, aFunctionNumber=%d"), aMessage->Handle(),
    aFunctionNumber));

    switch ( aFunctionNumber ) 
        {
        case ENCDOperationFunctionGetData:
            {
            HandleConfigurationDataRequestMessage( *aMessage );
            break;
            }
        default:
            {
            CNcdBaseOperation::ReceiveMessage( aMessage, aFunctionNumber );
            break;
            }
        }
    }

void CNcdLoadBundleNodeOperation::ErrorL( MNcdPreminetProtocolError* aData )
    {
    DLTRACEIN((""));    
    // Default observer deletes aData
    iParser->DefaultObserver().ErrorL( aData );
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadBundleNodeOperation::CNcdLoadBundleNodeOperation(
    CNcdGeneralManager& aGeneralManager, 
    MCatalogsHttpSession& aHttpSession,  
    MNcdOperationRemoveHandler* aRemoveHandler,
    MNcdOperationQueue& aOperationQueue,
    MCatalogsSession& aSession )
    : CNcdBaseOperation( aGeneralManager, aRemoveHandler, ELoadBundleNodeOperation,
        aSession ), 
      iAccessPointManager( aGeneralManager.AccessPointManager() ),
      iHttpSession( aHttpSession ),
      iProtocol( aGeneralManager.ProtocolManager() ),
      iConfigManager( aGeneralManager.ConfigurationManager() ),
      iNodeDbLocked( EFalse ),
      iOperationQueue( aOperationQueue ),
      iFirstConfRequest( ETrue )
    {
    iBundleNodeState = EConfRequest;
    iMasterServerRedirectionState = EBegin;
    iProgress.iState = 0;
    iProgress.iOperationId = 0;
    iProgress.iProgress = 0;
    iProgress.iMaxProgress = 100;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CNcdLoadBundleNodeOperation::ConstructL(
    const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));
    CNcdBaseOperation::ConstructL();
    
    iConfigResponseBuf = CBufFlat::NewL( 100 );
    iNodeIdentifier = CNcdNodeIdentifier::NewL( aNodeIdentifier );
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
HBufC8* CNcdLoadBundleNodeOperation::CreateConfRequestLC( CNcdQuery* aQuery )
    {
    DLTRACEIN((""));
    CNcdRequestConfiguration* req =
        NcdRequestGenerator::CreateConfigurationRequestLC();
    CNcdNode& bundleNode = iNodeManager->NodeL( *iNodeIdentifier );
    
    CNcdNodeIdentifier* metadataIdentifier = 
        NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( *iNodeIdentifier );
            
    RBuf8 buf;
    CleanupClosePushL( buf );
    buf.CreateL( metadataIdentifier->NodeId().Length() );
    buf.Copy( metadataIdentifier->NodeId() );
    
    req->AddCatalogBundleRequestL( buf );
    CleanupStack::PopAndDestroy( &buf );
    
    CleanupStack::PopAndDestroy( metadataIdentifier );
    
    if( aQuery )
        {
        DASSERT(( aQuery->Response() == MNcdQuery::EAccepted ));
        
        MNcdConfigurationProtocolQueryResponse* queryResponse =
            CreateResponseL( *aQuery );
        CleanupStack::PushL( queryResponse );
        req->AddQueryResponseL( queryResponse );
        CleanupStack::Pop( queryResponse );
        }
        
    HBufC8* data = iProtocol.ProcessConfigurationRequestL(
        iPendingMessage->Session().Context(), *req );
    CleanupStack::PopAndDestroy( req );
    CleanupStack::PushL( data );    
    return data;
    }


void CNcdLoadBundleNodeOperation::RevertNodesOfBrokenSourcesToCacheL() 
    {
    DLTRACEIN((""));
    // Revert the nodes from broken content sources
    CNcdRootNode* rootNode( NULL );
    TRAPD( err, rootNode = &iNodeManager->RootNodeL( iNodeIdentifier->ClientUid() ) );
    
    if ( err != KErrNone )
        {
        // Root node has not been created, so there is no old content source map.
        // There is no need to revert anything.
        return;
        }
        
    DASSERT( rootNode );
    
    CNcdContentSourceMap& oldContentSourceMap = rootNode->ContentSourceMap();
    DLINFO(("old content source count=%d", oldContentSourceMap.ContentSourceCount() ));
    for ( TInt i = 0; i < oldContentSourceMap.ContentSourceCount(); i++ ) 
        {
        CNcdContentSource& oldSource = oldContentSourceMap.ContentSource( i );

        // Note that the content sources contain node identifiers, 
        // not metadata identifiers.        
        RPointerArray<CNcdNodeIdentifier>& oldNodes = 
            oldContentSourceMap.NodesL( oldSource );
            
        if ( iContentSourceMap->HasContentSource( oldSource ) )
            {           
            DLINFO(("same content source")); 
            CNcdContentSource& source = 
                iContentSourceMap->ContentSourceL( oldSource );
            if ( source.IsBroken() ) 
                {
                // Broken content source, revert the nodes belonging to this source                                             
                for ( TInt i = 0; i < oldNodes.Count(); i++ ) 
                    {
                    RevertNodeL( *oldNodes[i], source );
                    }
                }
            }
        else if ( oldSource.ParentIdentifier().Equals( *iNodeIdentifier ) )
            {
            DLINFO(("content source disappeared"));
            for ( TInt i = 0; i < oldNodes.Count(); i++ ) 
                {                
                iNodeManager->RemoveNodeL( *oldNodes[i] );
                }
            }            
        }
    }
        
    
void CNcdLoadBundleNodeOperation::RevertNodeL(
    const CNcdNodeIdentifier& aNodeIdentifier,
    const CNcdContentSource& aContentSource ) 
    {
    DLTRACEIN((_L("node ns: %S, id: %S"), 
        &aNodeIdentifier.NodeNameSpace(), 
        &aNodeIdentifier.NodeId() ));
        
    iNodeManager->RevertNodeFromTempCacheL( aNodeIdentifier );
    CNcdNode* node = iNodeManager->NodePtrL( aNodeIdentifier );
    DASSERT( node );
    
    // If the node is bundle's child, it must be added as a child since it was removed from the 
    // child list as it was moved to temp cache.
    CNcdNodeIdentifier* parentId = NcdNodeIdentifierEditor::ParentOfLC( aNodeIdentifier );
    if ( parentId->Equals( *iNodeIdentifier ) ) 
        {
        DLINFO(("Child of bundle"));
        CNcdNode& bundleNode = 
            iNodeManager->NodeL( *iNodeIdentifier );
        CNcdNodeIdentifier* metaOfChild = 
            NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( aNodeIdentifier );
        iNodeManager->AddToParentL(
            *parentId, *metaOfChild, CNcdNodeFactory::ENcdNodeFolder, CNcdNodeFactory::ENcdBundleNode,
            CNcdNodeFactory::NodePurposeL( *node ), CNcdNodeManager::EInsert,
            iContentSourceMap->GetInsertIndexL( aContentSource, *parentId ) );
        CleanupStack::PopAndDestroy( metaOfChild );
        }
    CleanupStack::PopAndDestroy( parentId );        
    }
    
    
void CNcdLoadBundleNodeOperation::RemoveOldNodesL() 
    {
    DLTRACEIN((""));
    CNcdRootNode& rootNode = 
        iNodeManager->RootNodeL( iNodeIdentifier->ClientUid() );
    CNcdContentSourceMap& oldContentSourceMap = rootNode.ContentSourceMap();
    DLINFO(("old content source count=%d", oldContentSourceMap.ContentSourceCount() ));
    for ( TInt i = 0; i < oldContentSourceMap.ContentSourceCount(); i++ ) 
        {
        CNcdContentSource& oldSource = oldContentSourceMap.ContentSource( i );

        // Note that the content sources contain node identifiers, 
        // not metadata identifiers.        
        RPointerArray<CNcdNodeIdentifier>& oldNodes = 
            oldContentSourceMap.NodesL( oldSource );
            
        if ( iContentSourceMap->HasContentSource( oldSource ) )
            {           
            DLINFO(("same content source")); 
            CNcdContentSource& source = 
                iContentSourceMap->ContentSourceL( oldSource );
            if ( !source.IsBroken() ) 
                {                
                RPointerArray<CNcdNodeIdentifier>& nodes = 
                    iContentSourceMap->NodesL( source );
                for ( TInt i = 0; i < oldNodes.Count(); i++ ) 
                    {
                    if ( !ContainsNode( nodes, *oldNodes[i] ) ) 
                        {                        
                        // Note that the content sources contain node identifiers, 
                        // not metadata identifiers.
                        iNodeManager->RemoveNodeL( *oldNodes[i] );
                        }
                    }
                }
            }
        else if ( oldSource.ParentIdentifier().Equals( *iNodeIdentifier ) )
            {
            DLINFO(("content source disappeared"));
            for ( TInt i = 0; i < oldNodes.Count(); i++ ) 
                {                
                iNodeManager->RemoveNodeL( *oldNodes[i] );
                }
            }
        }
    }
    
void CNcdLoadBundleNodeOperation::AddBundleToLoadedNodesL() 
    {
    DLTRACEIN((""));
    CNcdNodeIdentifier* bundleId = CNcdNodeIdentifier::NewLC( *iNodeIdentifier );
    iLoadedNodes.AppendL( bundleId );
    CleanupStack::Pop( bundleId );    
    }
    
void CNcdLoadBundleNodeOperation::SetAlwaysVisibleFlagsL() 
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iContentSourceMap->ContentSourceCount(); i++ ) 
        {
        CNcdContentSource& contentSource = iContentSourceMap->ContentSource( i );
        if ( !contentSource.AlwaysVisible() ) 
            {
            continue;
            }
        // Get the reference. So, no need to close this array.
        RPointerArray<CNcdNodeIdentifier>& nodes = iContentSourceMap->NodesL( contentSource );
        TRAPD( err, 
            {            
            for ( TInt i = 0; i < nodes.Count(); i++ ) 
                {
                // Because content sources use node id 
                CNcdNodeIdentifier* nodeId = nodes[i];
                
                // Check whether the node is loaded completely
                for ( TInt i = 0; i < iLoadedNodes.Count(); i++ ) 
                    {
                    if ( iLoadedNodes[i]->Equals( *nodeId ) ) 
                        {                        
                        CNcdNode& node = iNodeManager->NodeL( *nodeId );
                        CNcdNodeMetaData& metadata = node.NodeMetaDataL();                
                        metadata.SetAlwaysVisible( ETrue );
                        
                        // DLMAIN-523, "Always visible information is not 
                        // persisted to disk"
                        iNodeManager->DbSaveNodeMetaDataL( metadata );
                        break;
                        }
                    }
                }
            }); // TRAP


        if ( err == KErrNotFound ) 
            {
            DASSERT( EFalse );
            }
        else if ( err != KErrNone )
            {
            User::Leave( err );
            }

        }
    DLTRACEOUT(("Ok."));
    }
    
void CNcdLoadBundleNodeOperation::UpdateCsMapToRootNodeL() 
    {
    DLTRACEIN((""));
    CNcdRootNode& rootNode = iNodeManager->CreateRootL( iNodeIdentifier->ClientUid() );
    CNcdContentSourceMap& oldMap = rootNode.ContentSourceMap();
    
    // Find the bundle from old map
    TInt folderIndex = oldMap.FindFolder( *iNodeIdentifier );
    if ( folderIndex == KErrNotFound ) 
        {
        // This bundle folder is not in root node's content source map.
        // Can happen for example when cache has been cleared and favorite view opened
        // if there is a bundle folder in favorites.
        for ( TInt i = 0; i < iContentSourceMap->ContentSourceCount(); i++ ) 
            {
            CNcdContentSource& cs = iContentSourceMap->ContentSource( i );
            CNcdContentSource* copy = cs.CopyL();
            CleanupStack::PushL( copy );
            oldMap.AppendContentSourceL( copy );
            CleanupStack::Pop( copy );
            }
        }
    else 
        {
        // Root node's content source map contains this bundle folder already. Clear
        // the old content sources from the bundle and add the new ones.
        CNcdFolderContent& content = oldMap.FolderContent( folderIndex );
        content.ClearContentSources();
        for ( TInt i = 0; i < iContentSourceMap->ContentSourceCount(); i++ ) 
            {
            CNcdContentSource& cs = iContentSourceMap->ContentSource( i );
            CNcdContentSource* copy = cs.CopyL();
            CleanupStack::PushL( copy );
            content.AppendContentSourceL( copy );    
            CleanupStack::Pop( copy );
            }
        }
        
    DASSERT( !iNodeDbLocked );        
    // Root node's content source map updated, save root node to db
    iNodeManager->DbSaveNodeL( rootNode );
    }

     
void CNcdLoadBundleNodeOperation::ParseCatalogBundleL(
    const MNcdConfigurationProtocolDetail& aDetail ) 
    {  
    DLTRACEIN((""));
    DASSERT( aDetail.GroupId() == KCatalogBundle );
    DASSERT( aDetail.Id() == KNameSpace );
    
    // The bundle node should already exist.
    CNcdNode& bundleFolder = iNodeManager->NodeL( *iNodeIdentifier );
    CNcdNodeLink& link = bundleFolder.NodeLinkL();

    CNcdNodeIdentifier* metaDataId =
        NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( *iNodeIdentifier );
    
    const RPointerArray<MNcdConfigurationProtocolContent>& bundleContents = 
        aDetail.Contents();

#ifdef CATALOGS_BUILD_CONFIG_DEBUG    
    const TDesC* bundleId = NULL;
    for ( TInt i = 0; i < bundleContents.Count(); i++ ) 
        {
        const MNcdConfigurationProtocolContent* content = bundleContents[i];
        if ( content->Key() == KId ) 
            {
            bundleId = &content->Value();
            break;
            }
        }
        
    DASSERT( bundleId );
    DASSERT( *bundleId == metaDataId->NodeId() );
    DASSERT( aDetail.Value() == metaDataId->NodeNameSpace() );    
#endif  
    
    // Also, notice that it is essential to insert the metadata identifier into the
    // link info. So, the right metadata will be found when the bundle is opened
    // from the database. For example when application has been started.
    link.SetMetaDataIdentifierL( *metaDataId );
    
    // Create meta data for bundle folder.
    CNcdNodeMetaData& bundleMetaData = 
        iNodeManager->CreateNodeMetaDataL( *metaDataId, CNcdNodeFactory::ENcdNodeFolder );
    
    for ( TInt i = 0; i < bundleContents.Count(); i++ )
        {
        const MNcdConfigurationProtocolContent* content = bundleContents[i];
        if ( content->Key() == KName ) 
            {
            bundleMetaData.SetNodeNameL( content->Value() );
            }
        else if ( content->Key() == KDescription ) 
            {
            bundleMetaData.SetDescriptionL( content->Value() );
            }
        else if ( content->Key() == KValidUntil ) 
            {
            link.SetValidUntilDelta(
                NcdProtocolUtils::DesDecToIntL( content->Value() ) );
            }
        else if ( content->Key() == KViewType ) 
            {
            static_cast<CNcdBundleFolder&>( bundleFolder ).SetViewTypeL( 
                content->Value() );
            }
        else if ( content->Key() == KDisclaimer )
            {
            HandleBundleDisclaimerL( bundleMetaData, content->Value() );
            }
        }
            
    const RPointerArray<MNcdConfigurationProtocolDetail>& subCatalogs = 
        aDetail.Details();

    // Parse icon data.
    for ( TInt i = 0; i < subCatalogs.Count(); i++ ) 
        {
        MNcdConfigurationProtocolDetail* detail = subCatalogs[i];
        if ( detail->Id() != KIcon ) 
            {
            continue;
            }
        const RPointerArray<MNcdConfigurationProtocolContent> iconContents = 
            detail->Contents();
        CNcdNodeIcon* icon = CNcdNodeIcon::NewL( *iNodeManager, bundleMetaData );
        bundleMetaData.SetIcon( icon );
        
        for ( TInt i = 0; i < iconContents.Count(); i++ ) 
            {
            MNcdConfigurationProtocolContent* content = iconContents[i];
            if ( content->Key() == KId ) 
                {
                icon->SetIconIdL( content->Value() );
                }
            else if ( content->Key() == KData ) 
                {
                const TDesC& iconData = content->Content();
                HBufC8* iconData8 = HBufC8::NewLC( iconData.Length() );
                iconData8->Des().Copy( iconData );
                HBufC8* decodedData = 
                    NcdProtocolUtils::DecodeBase64LC( *iconData8 );
                
                // Save the icon data to database.
                CNcdNodeIdentifier* iconId = CNcdNodeIdentifier::NewLC(
                    metaDataId->NodeNameSpace(), icon->IconId(), 
                    icon->Uri(), metaDataId->ClientUid() );
                iNodeManager->DbSaveIconDataL( *iconId, *decodedData );
                CleanupStack::PopAndDestroy( iconId );
                CleanupStack::PopAndDestroy( decodedData );
                CleanupStack::PopAndDestroy( iconData8 );
                }
            else if ( content->Key() == KUri && content->Value() != KNullDesC )
                {
                DLTRACE((_L("Setting bundle icon uri: %S"), &content->Value() ));
                icon->SetUriL( content->Value() );
                }
            }
        }
        
    CleanupStack::PopAndDestroy( metaDataId );
    
    iNodeManager->DbSaveNodeMetaDataL( bundleMetaData );          

    // Set the metadata to the bundle folder because it was not set during creation.
    bundleFolder.SetNodeMetaDataL( bundleMetaData );
    
    iNodeManager->DbSaveNodeL( bundleFolder );        

    // Parse sub catalogs.            
    for ( TInt i = 0; i < subCatalogs.Count(); i++ ) 
        {
        MNcdConfigurationProtocolDetail* subCatalog = subCatalogs[i];
        if ( subCatalog->GroupId() != KSubCatalogs ) 
            {
            continue;
            }
            
        DASSERT( subCatalog->Id() == KNameSpace );
        // The parameter is the parent node identifier.
        CNcdContentSource* contentSource = CNcdContentSource::NewLC( bundleFolder.Identifier() );
        contentSource->SetNameSpaceL( subCatalog->Value() );

        DLINFO((_L("Bundle subcatalog ns(/value): %S"), 
                &subCatalog->Value()));
        
        const RPointerArray<MNcdConfigurationProtocolDetail>& subCatalogDetails =
            subCatalog->Details();
        for ( TInt i = 0; i < subCatalogDetails.Count(); i++ ) 
            {
            const MNcdConfigurationProtocolDetail* detail = subCatalogDetails[i];
            if ( detail->Id() == KUri ) 
                {
                contentSource->SetUriL( detail->Value() );
                DLINFO((_L("Bundle subcatalog detail uri(/value): %S"), 
                        &detail->Value()));
                }
            }
        
        const RPointerArray<MNcdConfigurationProtocolContent>& subCatalogContents =
            subCatalog->Contents();
        for ( TInt i = 0; i < subCatalogContents.Count(); i++ ) 
            {
            const MNcdConfigurationProtocolContent* content = subCatalogContents[i];
            if ( content->Key() == KId ) 
                {
                // Notice that the the content source ids should be node identifiers.
                // Not the actual metadata identifiers. So, append the root info
                // in front of the value gotten from the server.
                CNcdNodeIdentifier* contentIdentifier =
                    CNcdNodeIdentifier::NewLC( contentSource->NameSpace(),
                                               content->Value(),
                                               contentSource->Uri(),
                                               bundleFolder.Identifier().ClientUid() );

                CNcdNodeIdentifier* actualNodeIdentifier =
                    NcdNodeIdentifierEditor::CreateNodeIdentifierLC( bundleFolder.Identifier(),
                                                                      *contentIdentifier );
                                                              
                contentSource->SetNodeIdL( actualNodeIdentifier->NodeId() );
                
                DLINFO((_L("Bundle subcatalog content set node id(/value): %S"), 
                        &actualNodeIdentifier->NodeId()));
                        
                CleanupStack::PopAndDestroy( actualNodeIdentifier );
                CleanupStack::PopAndDestroy( contentIdentifier );
                }
            else if ( content->Key() == KProvider ) 
                {
                contentSource->SetProviderL( content->Value() );
                DLINFO((_L("Bundle subcatalog content set provider (/value): %S"), 
                        &content->Value()));
                }
            else if ( content->Key() == KTransparent ) 
                {
                TBool transparent = EFalse;
                NcdProtocolUtils::DesToBool( transparent, content->Value() );
                contentSource->SetTransparent( transparent );
                }
            }
            
        // add the content source to content source map
        iContentSourceMap->AppendContentSourceL( contentSource );
        CleanupStack::Pop( contentSource );
        }
    }
    
    
void CNcdLoadBundleNodeOperation::NotifyCompletionOfQueuedOperation(
    TNcdOperationMessageCompletionId aId )
    {
    DLTRACEIN((""));
    if ( aId == ENCDOperationMessageCompletionComplete ||
         aId == ENCDOperationMessageCompletionError )
        {
        iOperationQueue.QueuedOperationComplete( *this );
        }
    }
    
    
TBool CNcdLoadBundleNodeOperation::ContainsNode(
    const RPointerArray<CNcdNodeIdentifier>& aNodes,
    const CNcdNodeIdentifier& aNode) 
    {
    for ( TInt i = 0; i < aNodes.Count(); i++ ) 
        {
        CNcdNodeIdentifier* nodeId = aNodes[i];
        DASSERT( nodeId->ClientUid() == aNode.ClientUid() );
        if ( nodeId->Equals( aNode ) ) 
            {
            return ETrue;
            }
        }
    return EFalse;
    }


void CNcdLoadBundleNodeOperation::HandleBundleDisclaimerL(
    CNcdNodeMetaData& aMetadata, 
    const TDesC& aDisclaimer )
    {
    DLTRACEIN((""));
    // Create a temp query that is initialized with the disclaimer data
    // and internalize the disclaimer-object with the query. This
    // way we don't have to add new setters to CNcdNodeDisclaimer
    CNcdConfigurationProtocolQueryImpl* tempQuery = 
        CNcdConfigurationProtocolQueryImpl::NewLC();
    tempQuery->iBodyText->SetDataL( aDisclaimer );
    tempQuery->iSemantics = MNcdQuery::ESemanticsDisclaimer;
    
    CNcdNodeDisclaimer* disclaimer = CNcdNodeDisclaimer::NewLC();
    disclaimer->InternalizeL( *tempQuery );
    aMetadata.SetDisclaimer( disclaimer );
    
    CleanupStack::Pop( disclaimer );
    CleanupStack::PopAndDestroy( tempQuery );
    }

    
void CNcdLoadBundleNodeOperation::HandleConfigurationDataRequestMessage(
    MCatalogsBaseMessage& aMessage ) 
    {
    DLTRACEIN((""));
    TPtr8 data = iConfigResponseBuf->Ptr( 0 );
    
    TRAPD( err, aMessage.CompleteAndReleaseL( data, KErrNone ) );
    if ( err != KErrNone ) 
        {
        aMessage.CompleteAndRelease( err );
        }        
    }    
// ---------------------------------------------------------------------------
// From class CNcdBaseOperation.
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CNcdLoadBundleNodeOperation::RunOperation()
    {
    DLTRACEIN((("this-ptr: %x"), this ));
  
    TRAPD( err, DoRunOperationL() ); 
     
     if ( err != KErrNone )
        {
        DLTRACE(("error: %d", err));
        Cancel();
        iBundleNodeState = EFailed;
        iError = err;
        if ( iPendingMessage )
            {
            // ignoring error because operation already failed
            CompleteMessage( iPendingMessage,
                ENCDOperationMessageCompletionError, iError );
            }
        }

    DLTRACEOUT(("err: %d", err));
    return err;    
    }

void CNcdLoadBundleNodeOperation::DoRunOperationL()
    {
    DLTRACEIN((""));
    switch ( iBundleNodeState )
        {
        case EConfRequest:
            {            
            DLTRACE((_L("->EConfRequest")));
            DASSERT ( iPendingMessage );
                                    
            // remove old content sources
            delete iContentSourceMap;
            iContentSourceMap = NULL;
            iContentSourceMap = CNcdContentSourceMap::NewL();
            
            delete iServerUri;
            iServerUri = NULL;
            
            // Use the server uri from bundle node's node link, if it exists.
            // This way we use the correct uri in case of scheme bundle folder.
            CNcdNodeFolder& bundleNode = iNodeManager->FolderL( *iNodeIdentifier );
            if ( bundleNode.CreateAndSetLinkL().ServerUri() != KNullDesC ) 
                {
                iServerUri = bundleNode.NodeLinkL().ServerUri().AllocL();
                }
            else 
                {
                iServerUri = iConfigManager.MasterServerAddressL(
                    iPendingMessage->Session().Context() ).AllocL();
                }
            DLINFO((_L("server uri: %S"), iServerUri ));
                        
            
            if ( iFirstConfRequest )
                {
                // Store previous list only if some children have been previously loaded.
                if( bundleNode.ChildrenPreviouslyLoaded() )
                    {
                    bundleNode.StoreChildrenToPreviousListL();
                    }
                
                // Create previous lists of children for new checking.
                iChildEntityMaps.ResetAndDestroy();
                iNodeManager->SeenInfo().CreatePreviousListsForChildrenL( bundleNode, iChildEntityMaps );

                // Backup node RAM cache to temp cache.
                iNodeManager->BackupAndClearCacheL( *iNodeIdentifier );
                iNodeManager->LockNodeDbL( iNodeIdentifier->ClientUid() );
                iNodeDbLocked = ETrue;                                  
                }
            
            HBufC8* request = CreateConfRequestLC( iConfQuery );
            
            if ( iConfQuery )
                {
                iConfQuery->InternalRelease();
                iConfQuery = NULL;
                }
                        
            DLINFO(( "request= %S", request ));
            
            // create transaction
            iGeneralManager.HttpUtils().CreateTransactionL( 
                iHttpSession,
                iTransaction,
                *iServerUri,
                *this,
                *request );

            
            // create parser
            delete iParser;
            iParser = NULL;
            iParser = iProtocol.CreateParserL( 
                iPendingMessage->Session().Context(), *iServerUri );
            MNcdParserObserverBundle& observers = iParser->Observers();
            observers.SetParserObserver( this );
            iDefaultConfigurationProtocolObserver =
                observers.ConfigurationProtocolObserver();
            observers.SetConfigurationProtocolObserver( this );
            observers.SetInformationObserver( this );
            
            iConfigManager.AddObserverL( *this, iPendingMessage->Session().Context() );
            
            iParser->BeginAsyncL();
  
            // start transaction
            User::LeaveIfError( iTransaction->Start() );            
            CleanupStack::PopAndDestroy( request );
            
            iFirstConfRequest = EFalse;
            iBundleNodeState = EReceiveConf;
            DLTRACE((_L("->EConfRequest done")));
            break;
            }
            
        case EReceiveConf:
            {
            DLTRACE((_L("->EReceiveConf")));
            // Should send progress only if progress is made
            
            if ( iPendingMessage && iSendProgress )
                {
                User::LeaveIfError( CompleteMessage( iPendingMessage,
                    ENCDOperationMessageCompletionProgress,
                    iProgress, KErrNone ) );
                iSendProgress = EFalse;
                }
            DLTRACE((_L("->EReceiveConf done")));
            break;
            }
            
        case EConfQuery:
            {
            DLTRACE(("->EConfQuery"));
            DASSERT( iConfQuery );
            DASSERT( iConfQuery->Response() == MNcdQuery::EAccepted ||
                iConfQuery->Response() == MNcdQuery::ERejected )
            if ( iConfQuery->Response() == MNcdQuery::EAccepted)
                {
                DLTRACE(("Query accepted"));
                if ( iConfQuery->ItemCount() == 0 )
                    {
                    // SPECIAL CASE:
                    // querys with no items don't need responding to
                    // e.g. a server message
                    if( iContentSourceMap->ContentSourceCount() > 0 )
                        {
                        // content sources received in the earlier response,
                        // start loading them
                        iBundleNodeState = EBrowseRequest;
                        }
                    else if ( iMasterServerRedirectionState == ERedirecting )
                        {
                        // Redirect. Delete conf query since it does not need
                        // responding to new master server.
                        DLINFO(("Redirect"));
                        iConfQuery->InternalRelease();
                        iConfQuery = NULL;                       
                        iBundleNodeState = EConfRequest;
                        }                        
                    else
                        {
                        // no content sources received
                        // e.g. server didn't give any content sources 
                        // with the set provisioning, only a 
                        // "service not available..." message is received
                        iBundleNodeState = EComplete;
                        }                                
                    }
                else
                    {
                    // NORMAL CASE:
                    // Respond to the query in a new request
                    iBundleNodeState = EConfRequest;
                    }            
                }            
            else
                {
                DLTRACE(("Query rejected"));
                if ( iConfQuery->IsOptional() )
                    {
                    DLTRACE(("Query is optional"));
                    if( iContentSourceMap->ContentSourceCount() > 0 )
                        {
                        DLTRACE(("Content sources received previously, start loading"));
                        // content sources received in the earlier response,
                        // start loading them
                        iBundleNodeState = EBrowseRequest;
                        }
                    else if ( iMasterServerRedirectionState == ERedirecting )
                        {
                        // Redirect. Delete conf query since it does not need
                        // responding to new master server.
                        DLINFO(("Redirect"));
                        iConfQuery->InternalRelease();
                        iConfQuery = NULL;                       
                        iBundleNodeState = EConfRequest;
                        }                                                
                    else
                        {
                        DLTRACE(("No sources received!"));
                        // no content sources received with an optional query
                        // this should never happen!                        
                        iError = KNcdErrorNoContentSources;
                        iBundleNodeState = EFailed;
                        }                            
                    }
                else
                    {
                    DLTRACE(("Query not optional, operation will stop!"));
                    iBundleNodeState = EComplete;
                    }
                }
                            
            RunOperation();            
            DLTRACE(("->EConfQuery done"));
            break;
            }
            
        case EBrowseRequest:
            {
            DLTRACE((_L("->EBrowseRequest")));
                        
            iSubOps.ResetAndDestroy();

            // Create load node op for each content source.
            DLINFO((("Content source count=%d"), iContentSourceMap->ContentSourceCount()));
            for ( TInt i = 0 ; i < iContentSourceMap->ContentSourceCount() ; i++ )
                {
                CNcdContentSource& contentSource =
                    iContentSourceMap->ContentSource( i );
                DLINFO((_L("content source: namespace=%S, uri=%S, id=%S"),
                    &contentSource.NameSpace(), &contentSource.Uri(), &contentSource.NodeId()));
                
                CNcdLoadNodeOperationImpl* loadOp =
                    CNcdLoadNodeOperationImpl::NewLC(
                        contentSource,
                        iContentSourceMap,
                        contentSource.ParentIdentifier(),
                        iGeneralManager,                        
                        iHttpSession,                        
                        iRemoveHandler,
                        iSession );
                
                loadOp->AddObserverL( this );
                iSubOps.AppendL( loadOp );
                CleanupStack::Pop( loadOp );
                // error code ignored, errors handled via callback
                loadOp->Start();
                }
            
            iBundleNodeState = EReceiveBrowse;
            DLINFO(("subop count: failed:%d completed:%d total:%d",
                iFailedSubOps.Count(), iCompletedSubOps.Count(), iSubOps.Count() ));
            if ( iFailedSubOps.Count() + iCompletedSubOps.Count() ==
                 iSubOps.Count() )
                {
                // all sub ops have either completed or failed
                // -> this operation is now complete
                iBundleNodeState = EComplete;
                if ( iFailedSubOps.Count() > 0 ) 
                    {
                    DLINFO(("Some catalogs failed to load"));
                    iError = KNcdErrorSomeCatalogsFailedToLoad;
                    }
                RunOperation();
                }
                
            DLTRACE((_L("->EBrowseRequest done")));
            
            break;
            }
            
        case EReceiveBrowse:
            {
            DLTRACE((_L("->EReceiveBrowse")));
            if( iSubOpQuerys.Count() > 0 )
                {
                // send sub op query to proxy
                CNcdBaseOperation::QueryReceivedL( iSubOpQuerys[0] );
                iSubOpQuerys.Remove( 0 );
                }                
            else if ( iPendingMessage && iLoadedNodes.Count() > 0 )
                {
                SetAlwaysVisibleFlagsL();
                
                // send updated nodes identifiers to proxy
                User::LeaveIfError( CompleteMessage( iPendingMessage,
                    ENCDOperationMessageCompletionNodesUpdated,
                    iProgress,
                    iLoadedNodes,
                    KErrNone ) );
                
                iLoadedNodes.ResetAndDestroy();
                }
            else if ( iFailedSubOps.Count() + iCompletedSubOps.Count() ==
                 iSubOps.Count() )
                {
                // all sub ops have either completed or failed
                // -> this operation is now complete
                iBundleNodeState = EComplete;
                if ( iFailedSubOps.Count() > 0 ) 
                    {
                    DLINFO(("Some catalogs failed to load"));
                    iError = KNcdErrorSomeCatalogsFailedToLoad;
                    }
                RunOperation();
                }
                
            DLTRACE((_L("->EReceiveBrowse done")));
            break;
            }
            
        case EComplete:
            {
            DLTRACE((_L("->EComplete")));
            
            // Compare the ContentSourceMaps of previous root node load and this
            // load and delete the nodes that were removed from server.
            if ( iContentSourceMap && iContentSourceMap->ContentSourceCount() > 0 ) 
                {
                DLINFO(("Content source map count"));
                RevertNodesOfBrokenSourcesToCacheL();                                            
                AddBundleToLoadedNodesL();
                SetAlwaysVisibleFlagsL();

                // Now the RAM node cache should be updated correctly, save it to database.
                iNodeManager->UnlockNodeDb( iNodeIdentifier->ClientUid() );
                iNodeDbLocked = EFalse;

                UpdateCsMapToRootNodeL();
                
                delete iContentSourceMap;
                iContentSourceMap = NULL;

                iNodeManager->DbSaveNodesL( *iNodeIdentifier );                                
                }
                
            if( iNodeDbLocked )
                {
                iNodeManager->UnlockNodeDb( iNodeIdentifier->ClientUid() );
                iNodeDbLocked = EFalse;
                }
            
            iNodeManager->ClearTempCacheL( *iNodeIdentifier );
                            
            CNcdNodeFolder& bundleNode = 
                iNodeManager->FolderL( *iNodeIdentifier );
                
            // Set the children loaded flag so that next refresh stores
            // previous child list (needed for new checking)
            if( bundleNode.ChildCount() )
                {
                bundleNode.SetChildrenPreviouslyLoaded();
                iNodeManager->DbSaveNodeL( bundleNode );
                }
                
            // Check new status
            iNodeManager->SeenInfo().StorePreviousListsToExistingChildrenL(
                bundleNode,
                iChildEntityMaps );
                
            iChildEntityMaps.ResetAndDestroy();
            
            // Check new status for transparent folders (transparent folders are
            // loaded during bundle op so they need to be checked here)
            iNodeManager->SeenInfo().DoNewCheckForTransparentChildrenL(
                bundleNode );
            
            iNodeManager->SeenInfo().RefreshFolderSeenStatusL( *iNodeIdentifier );
            
            if ( iPendingMessage && iLoadedNodes.Count() > 0 )
                {
                DLINFO(("Pending message loaded nodes"));
                // send updated nodes identifiers to proxy
                User::LeaveIfError( CompleteMessage( iPendingMessage,
                    ENCDOperationMessageCompletionNodesUpdated,
                    iProgress,
                    iLoadedNodes,
                    KErrNone ) );
                
                iLoadedNodes.ResetAndDestroy();                
                }
            else if ( iPendingMessage )
                {
                DLINFO(("Pending message"));
                if ( iError == KErrNone ) 
                    {                    
                    // Send complete message to proxy.
                    User::LeaveIfError( CompleteMessage( iPendingMessage,
                        ENCDOperationMessageCompletionComplete,
                        iProgress,
                        KErrNone ) );
                                        
                    iOperationState = EStateComplete;
                    }
                else 
                    {
                    iBundleNodeState = EFailed;
                    RunOperation();
                    }
                }
            DLTRACE((_L("->EComplete done")));
            break;
            }
            
        case EFailed:
            {
            DLTRACE((_L("->EFailed")));
            Cancel();
            // Send error message in case didn't have any pending
            // messages when the operation actually failed
            if( iPendingMessage )
                {
                DLINFO(("Pending"));
                // ignoring error because operation already failed
                CompleteMessage( iPendingMessage,
                    ENCDOperationMessageCompletionError, iError );
                }
            DLTRACE((_L("->EFailed done")));
            break;
            }
            
        default:
            {
            DLTRACE(("default"));
            DASSERT(0);
            break;
            }
        }
    }
    
void CNcdLoadBundleNodeOperation::ChangeToPreviousStateL()
    {
    DLTRACEIN(("no implementation needed"));
    }

TBool CNcdLoadBundleNodeOperation::QueryCompletedL( CNcdQuery* aQuery )
    {    
    DLTRACEIN((""));
    TBool handled = EFalse;
 
    if ( aQuery == iConfQuery )
        {
        // handle conf query
        DASSERT( iBundleNodeState == EConfQuery );
        if ( aQuery->Response() == MNcdQuery::ERejected )
            {
            Cancel();
            }
        handled = ETrue;
        }
    else
        {
        // handle child ops's querys
        TBool found = EFalse;
        for( TInt i = 0 ; i < iSubOps.Count() ; i++ )
            {
            CNcdQuery* query = iSubOps[i]->ActiveQuery();
            if ( aQuery == query )
                {
                iSubOps[i]->QueryHandledL( aQuery );
                TInt index = iSubOpQuerys.Find( aQuery );
                if ( index != KErrNotFound )
                    {
                    // remove reference
                    iSubOpQuerys[index]->InternalRelease();
                    iSubOpQuerys.Remove( index );
                    }
                query->InternalRelease();
                query = NULL;
                found = ETrue;
                break;
                }
            if ( query )
                {
                query->InternalRelease();
                }
            }
        if ( !found )
            {
            DLINFO(("Query doesn't belong to any sub op! ERROR!"))
            DASSERT( 0 );
            User::Leave( KErrArgument );
            }
        else
            {
            handled = ETrue;
            }
        }

    return handled;
    }
