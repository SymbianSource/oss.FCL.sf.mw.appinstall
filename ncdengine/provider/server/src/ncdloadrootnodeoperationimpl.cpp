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


#include <badesca.h>
#include <s32mem.h>

#include "ncdloadrootnodeoperationimpl.h"
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
#include "ncdchildentity.h"
#include "ncdprotocolstrings.h"
#include "ncdnodeseeninfo.h"
#include "ncdchildentitymap.h"
#include "ncdproviderutils.h"
#include "ncdhttputils.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadRootNodeOperation* CNcdLoadRootNodeOperation::NewL(
    TInt aClientUid,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler* aRemoveHandler,
    MCatalogsSession& aSession )
    {
    CNcdLoadRootNodeOperation* self = CNcdLoadRootNodeOperation::NewLC(
        aClientUid,
        aGeneralManager,
        aHttpSession,
        aRemoveHandler,
        aSession );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadRootNodeOperation* CNcdLoadRootNodeOperation::NewLC(
    TInt aClientUid,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler* aRemoveHandler,
    MCatalogsSession& aSession )
    {
    CNcdLoadRootNodeOperation* self =
        new( ELeave ) CNcdLoadRootNodeOperation( 
            aClientUid, 
            aGeneralManager,
            aHttpSession,
            aRemoveHandler, 
            aSession );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadRootNodeOperation::~CNcdLoadRootNodeOperation()
    {
    DLTRACEIN((""));
    DASSERT( !iNodeDbLocked );
    iConfigManager.RemoveObserver( *this );
    
    iLoadedNodes.ResetAndDestroy();
    
    DLTRACE(("Delete id, buffer"));
    delete iRootNodeIdentifier;
    
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

const CNcdNodeIdentifier& CNcdLoadRootNodeOperation::NodeIdentifier() const
    {
    return *iRootNodeIdentifier;
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadRootNodeOperation::HandleCancelMessage( MCatalogsBaseMessage* aMessage ) 
    {
    DLTRACEIN((""));
    Cancel();
    CNcdBaseOperation::HandleCancelMessage( aMessage );
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadRootNodeOperation::Cancel()
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
        iNodeManager->UnlockNodeDb( TUid::Uid( iClientUid ) );
        iNodeDbLocked = EFalse;
        TRAP_IGNORE( iNodeManager->RevertNodeCacheL( *iRootNodeIdentifier ) );
        }
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadRootNodeOperation::HandleHttpEventL( 
        MCatalogsHttpOperation& aOperation, 
        TCatalogsHttpEvent aEvent )
    {
    DLTRACEIN((""));
    DASSERT( &aOperation == iTransaction );
    DASSERT( aOperation.OperationType() == ECatalogsHttpTransaction );

    TCatalogsTransportProgress progress( iTransaction->Progress() );
    
    // Are state and id needed?
    iProgress = TNcdSendableProgress( iRootNodeState,
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
                if ( iRootNodeState == EReceiveConf ) 
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
    
    
TBool CNcdLoadRootNodeOperation::HandleHttpError( 
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
            iRootNodeState = EFailed;
            }
        else
            {
            iMasterServerRedirectionState = EReverted;
            iRootNodeState = EConfRequest;
            }
        }
    else
        {
        iError = aError.iError;
        iRootNodeState = EFailed;
        }
    RunOperation();
    return ETrue;
    }

void CNcdLoadRootNodeOperation::ParseError( TInt aErrorCode )
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
                iRootNodeState = EFailed;
                }
            else
                {
                iMasterServerRedirectionState = EReverted;
                iRootNodeState = EConfRequest;
                }
            }
        else 
            {            
            iRootNodeState = EFailed;
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
void CNcdLoadRootNodeOperation::ParseCompleteL( TInt aError )
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
        iRootNodeState = EFailed;
        RunOperation();
        }
    else
        {
        DASSERT( iRootNodeState == EReceiveConf || iRootNodeState == EConfRequest );
        if ( iConfQuery )
            {
            DLTRACE(("Query received, let base op handle it"));
            iRootNodeState = EConfQuery;
            // let base op handle the query
            TRAPD( err, CNcdBaseOperation::QueryReceivedL( iConfQuery ) );
            if ( err != KErrNone ) 
                {
                iError = err;
                iRootNodeState = EFailed;
                RunOperation();
                }
            }
        else if ( iRootNodeState == EConfRequest ) 
            {
            DASSERT( iMasterServerRedirectionState == ERedirecting );
            RunOperation();
            }
        else if ( iContentSourceMap->ContentSourceCount() < 1 &&
                  iContentSourceMap->BundleFolderCount() < 1 )
            {
            iError = KNcdErrorNoContentSources;
            iRootNodeState = EFailed;
            DLINFO(("No content sources received, stop operation!"))
            RunOperation();
            }
        else
            {            
            iRootNodeState = EBrowseRequest;
            RunOperation();
            }
        }
    DLTRACEOUT((""));
    }

void CNcdLoadRootNodeOperation::ConfigurationBeginL( const TDesC& aVersion, 
                                                     TInt aExpirationDelta )
    {
    DLTRACEIN((""));
    // set expiration delta for root node
    // No need to set metadata value for root, because it does not have one.
    iNodeManager->RootNodeL( iRootNodeIdentifier->ClientUid() ).
        CreateAndSetLinkL().SetValidUntilDelta( aExpirationDelta );
	// Pass to default observer.
	DASSERT( iDefaultConfigurationProtocolObserver != NULL );
    iDefaultConfigurationProtocolObserver->ConfigurationBeginL( aVersion, aExpirationDelta );
    
    DLTRACEOUT((""));
    }
    
void CNcdLoadRootNodeOperation::ConfigurationQueryL(
    MNcdConfigurationProtocolQuery* aQuery )
    {
    DLTRACEIN((""));
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

void CNcdLoadRootNodeOperation::ClientConfigurationL(
    MNcdConfigurationProtocolClientConfiguration* aConfiguration )
    {
    DLTRACEIN((""));
        
    CleanupDeletePushL( aConfiguration );
    if ( !iNodeDbLocked )
        {        
        iNodeManager->LockNodeDbL( TUid::Uid( iClientUid ) );        
        iNodeDbLocked = ETrue;
        }
    DLINFO(("detail count:%d", aConfiguration->DetailCount()));

    // Parse access point data from client configuration response.
    iAccessPointManager.ParseAccessPointDataFromClientConfL(
        *aConfiguration, iPendingMessage->Session().Context().FamilyId() );
        
    iBundleInsertIndex = 0;
    
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
            
            // create a content source
            // The parameter is the parent node identifier.
            CNcdContentSource* contentSource =
                CNcdContentSource::NewLC( *iRootNodeIdentifier );
            contentSource->SetNameSpaceL( csDetail->Value() );
            
            // get details
            const RPointerArray<MNcdConfigurationProtocolDetail>& nsDetails =
                csDetail->Details();
            DLINFO(("nsDetails count=%d", nsDetails.Count()));
            for ( TInt k = 0 ; k < nsDetails.Count() ; k++ )
                {
                MNcdConfigurationProtocolDetail* nsDetail = nsDetails[k];
                DLINFO((_L("nsDetail: id=%S value=%S"), &nsDetail->Id(), &nsDetail->Value() ));
                if ( nsDetail->Id() == KUri )
                    {
                    contentSource->SetUriL( nsDetail->Value() );
                    }
                }
                
            // get contents
            const RPointerArray<MNcdConfigurationProtocolContent>& nsContents =
                csDetail->Contents();
            DLINFO(("nsContents count=%d", nsContents.Count()));
            for ( TInt l = 0 ; l < nsContents.Count() ; l++ )
                {
                MNcdConfigurationProtocolContent* nsContent = nsContents[l];
                DLINFO((_L("nsContent: key=%S value=%S"), &nsContent->Key(), &nsContent->Value() ));
                if ( nsContent->Key() == KProvider )
                    {
                    contentSource->SetProviderL( nsContent->Value() );
                    }
                else if ( nsContent->Key() == KId )
                    {
                    // Notice that the the content source ids should be node identifiers.
                    // Not the actual metadata identifiers. So, append the root info
                    // in front of the value gotten from the server.
                    CNcdNodeIdentifier* contentIdentifier =
                        CNcdNodeIdentifier::NewLC( contentSource->NameSpace(),
                                                   nsContent->Value(),
                                                   iRootNodeIdentifier->ClientUid() );                        
                    CNcdNodeIdentifier* actualNodeIdentifier =
                        NcdNodeIdentifierEditor::CreateNodeIdentifierLC( *iRootNodeIdentifier, 
                                                                         *contentIdentifier );
                    DLINFO((_L("Actual nodeidentifier for content source: %S"), 
                            &actualNodeIdentifier->NodeId()));
                    contentSource->SetNodeIdL( actualNodeIdentifier->NodeId() );
                    CleanupStack::PopAndDestroy( actualNodeIdentifier );
                    CleanupStack::PopAndDestroy( contentIdentifier );
                    }
                else if ( nsContent->Key() == KAlwaysVisible ) 
                    {
                    TBool visible = EFalse;
                    NcdProtocolUtils::DesToBool( visible, nsContent->Value() );
                    contentSource->SetAlwaysVisible( visible );
                    }
                else if ( nsContent->Key() == KTransparent ) 
                    {
                    TBool transparent = EFalse;
                    NcdProtocolUtils::DesToBool( transparent, nsContent->Value() );
                    contentSource->SetTransparent( transparent );
                    }
                }
            
            DLINFO((_L("content source: namespace=%S, uri=%S, provider=%S, nodeid=%S"),
                &contentSource->NameSpace(),
                &contentSource->Uri(),
                &contentSource->Provider(),
                &contentSource->NodeId()));
            iContentSourceMap->AppendContentSourceL( contentSource );
            CleanupStack::Pop( contentSource );
            iBundleInsertIndex++;
            }           
        }

    CleanupStack::Pop( aConfiguration );
    // Pass ownership to default observer.
	DASSERT( iDefaultConfigurationProtocolObserver != NULL );
    iDefaultConfigurationProtocolObserver->ClientConfigurationL( aConfiguration );
    DLTRACEOUT((""));
    }
    
void CNcdLoadRootNodeOperation::ConfigurationDetailsL(
    CArrayPtr<MNcdConfigurationProtocolDetail>* aDetails )
    {
    DLTRACEIN((""));
    
    // Pass ownership to default observer.
	DASSERT( iDefaultConfigurationProtocolObserver != NULL );
	iDefaultConfigurationProtocolObserver->ConfigurationDetailsL( aDetails );
	
	DLTRACEOUT((""));
    }
    
void CNcdLoadRootNodeOperation::ConfigurationActionRequestL(
    MNcdConfigurationProtocolActionRequest* aActionRequest )
    {
    DLTRACEIN((""));
	// Pass ownership to default observer.
	DASSERT( iDefaultConfigurationProtocolObserver != NULL );
    iDefaultConfigurationProtocolObserver->ConfigurationActionRequestL( aActionRequest );
    DLTRACEOUT((""));
    }

void CNcdLoadRootNodeOperation::ConfigurationServerDetailsL( MNcdConfigurationProtocolServerDetails* aServerDetails )
    {
    DLTRACEIN((""));
	// Pass ownership to default observer.
	DASSERT( iDefaultConfigurationProtocolObserver != NULL );
    iDefaultConfigurationProtocolObserver->ConfigurationServerDetailsL( aServerDetails );
    DLTRACEOUT((""));
    }

void CNcdLoadRootNodeOperation::ConfigurationErrorL( MNcdConfigurationProtocolError* aError )
    {
    DLTRACEIN((""));
    iRootNodeState = EFailed;
    iError = aError->Code();

	// Pass ownership to default observer.
	DASSERT( iDefaultConfigurationProtocolObserver != NULL );
	// Operation already failed, ignore possible error here
    TRAP_IGNORE(iDefaultConfigurationProtocolObserver->ConfigurationErrorL( aError ));
   
    RunOperation();

    DLTRACEOUT((""));
    }
    
void CNcdLoadRootNodeOperation::ConfigurationEndL()
    {
    DLTRACEIN((""));
	// Pass to default observer.
	DASSERT( iDefaultConfigurationProtocolObserver != NULL );
    iDefaultConfigurationProtocolObserver->ConfigurationEndL();
    DLTRACEOUT((""));
    }


void CNcdLoadRootNodeOperation::Progress( CNcdBaseOperation& aOperation )
    {
    (void) aOperation; // suppresses compiler warning
    DASSERT( iRootNodeState == EReceiveBrowse )
    DASSERT( aOperation.Type() == ELoadNodeOperation )
    //CNcdLoadNodeOperationImpl& loadOp =
    //    static_cast<CNcdLoadNodeOperationImpl&>( aOperation );
    
    /*for ( TInt i = 0 ; i < loadOp.LoadedNodes().Count() ; i++ )
        {
        iLoadedNodes.AppendL( loadOp.LoadedNodes()[i] );
        }*/
    }
    
void CNcdLoadRootNodeOperation::QueryReceived( CNcdBaseOperation& /*aOperation*/,
    CNcdQuery* aQuery )
    {
    DASSERT( iRootNodeState == EReceiveBrowse )
    TRAPD( err, iSubOpQuerys.AppendL( aQuery ) );
    aQuery->InternalAddRef();
    if( err != KErrNone )
        {
        iError = err;
        iRootNodeState = EFailed;
        }
    RunOperation();
    }
    
void CNcdLoadRootNodeOperation::OperationComplete( CNcdBaseOperation* aOperation,
                                                   TInt aError )
    {
    DLTRACEIN(("error=%d", aError));
    DLINFO((("iRootNodeState = %d"), iRootNodeState ));
    (void) aError; // suppresses compiler warning

    DASSERT( iRootNodeState == EReceiveBrowse || 
             iRootNodeState == EBrowseRequest )
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
        CNcdRootNode& rootNode = 
            iNodeManager->RootNodeL( iRootNodeIdentifier->ClientUid() );
        CNcdNodeLink& rootLink = rootNode.NodeLinkL();
        TTime currentTime;
        currentTime.HomeTime();

        // add loaded nodes from child op to our own array
        for ( TInt i = 0 ; i < loadedNodes.Count() ; i++ )
            {
            CNcdNodeIdentifier* id = CNcdNodeIdentifier::NewLC( *loadedNodes[i] );
            iLoadedNodes.AppendL( id );
            CleanupStack::Pop( id );
            }
        }
    
    if ( iRootNodeState ==  EReceiveBrowse )
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
        iRootNodeState = EFailed;
        RunOperation();
        }
    }
    
void CNcdLoadRootNodeOperation::ConfigurationChangedL() 
    {
    DLTRACEIN((( "iRootNodeState: %d"), iRootNodeState ));
    DASSERT( iRootNodeState == EReceiveConf )    
    // Master server address changed. Restart operation.
    iRootNodeState = EConfRequest;
    iMasterServerRedirectionState = ERedirecting;    
    }
    
void CNcdLoadRootNodeOperation::ReceiveMessage(
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

void CNcdLoadRootNodeOperation::ErrorL( MNcdPreminetProtocolError* aData )
    {
    DLTRACEIN((""));    
    // Default observer deletes aData
    iParser->DefaultObserver().ErrorL( aData );
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadRootNodeOperation::CNcdLoadRootNodeOperation( 
    TInt aClientUid,
    CNcdGeneralManager& aGeneralManager,     
    MCatalogsHttpSession& aHttpSession,      
    MNcdOperationRemoveHandler* aRemoveHandler,
    MCatalogsSession& aSession )
    : CNcdBaseOperation( aGeneralManager, aRemoveHandler, ELoadRootNodeOperation,
        aSession ), 
      iAccessPointManager( aGeneralManager.AccessPointManager() ),
      iHttpSession( aHttpSession ),
      iProtocol( aGeneralManager.ProtocolManager() ),
      iConfigManager( aGeneralManager.ConfigurationManager() ),
      iClientUid( aClientUid ),
      iNodeDbLocked( EFalse )
    {
    iRootNodeState = EConfRequest;
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
void CNcdLoadRootNodeOperation::ConstructL()
    {
    DLTRACEIN((""));
    CNcdBaseOperation::ConstructL();
    
    iConfigResponseBuf = CBufFlat::NewL( 100 );
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
HBufC8* CNcdLoadRootNodeOperation::CreateConfRequestLC( CNcdQuery* aQuery )
    {
    DLTRACEIN((""));
    CNcdRequestConfiguration* req =
        NcdRequestGenerator::CreateConfigurationRequestLC();
    
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


void CNcdLoadRootNodeOperation::RevertNodesOfBrokenSourcesToCacheL() 
    {
    DLTRACEIN((""));
    // Revert the nodes from broken content sources
    CNcdRootNode& rootNode = 
        iNodeManager->RootNodeL( iRootNodeIdentifier->ClientUid() );
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
            if ( source.IsBroken() ) 
                {
                // Broken content source, revert the nodes belonging to this source
                DLINFO(("Source was broken, revert %d nodes", oldNodes.Count()));      
                for ( TInt i = 0; i < oldNodes.Count(); i++ ) 
                    {
                    RevertNodeL( *oldNodes[i], source );
                    // Add the node to the new content source map also since the root node's old
                    // content source map is replaced with the new one.
                    CNcdNodeIdentifier* identifier = CNcdNodeIdentifier::NewLC( *oldNodes[i] );
                    iContentSourceMap->AddNodeToContentSourceL( identifier, source );
                    CleanupStack::Pop( identifier );
                    }
                    
                                    
                }
            }
        }
    }
        
    
void CNcdLoadRootNodeOperation::RevertNodeL(
    const CNcdNodeIdentifier& aNodeIdentifier,
    const CNcdContentSource& aContentSource ) 
    {
    DLTRACEIN((_L("node ns: %S, id: %S"), 
        &aNodeIdentifier.NodeNameSpace(), 
        &aNodeIdentifier.NodeId() ));
        
    iNodeManager->RevertNodeFromTempCacheL( aNodeIdentifier );
    CNcdNode* node = iNodeManager->NodePtrL( aNodeIdentifier );
    DASSERT( node );
    
    // The node is root's child, it must be added as a child since it was removed from the 
    // child list as it was moved to temp cache.
    CNcdNodeIdentifier* parentId = NcdNodeIdentifierEditor::ParentOfLC( aNodeIdentifier );
    if ( parentId->Equals( *iRootNodeIdentifier ) ) 
        {
        DLINFO(("Child of root"));
        CNcdRootNode& rootNode = 
            iNodeManager->RootNodeL( iRootNodeIdentifier->ClientUid() );
        CNcdNodeIdentifier* metaOfChild = 
            NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( aNodeIdentifier );
        iNodeManager->AddToParentL(
            *parentId, *metaOfChild, CNcdNodeFactory::ENcdNodeRoot, CNcdNodeFactory::ENcdNormalNode,
            CNcdNodeFactory::NodePurposeL( *node ), CNcdNodeManager::EInsert,
            iContentSourceMap->GetInsertIndexL( aContentSource, *parentId ) );
        CleanupStack::PopAndDestroy( metaOfChild );
        }
    CleanupStack::PopAndDestroy( parentId );        
    }

    
void CNcdLoadRootNodeOperation::AddNodesToDifferentParentsL() 
    {
    DLTRACEIN((""));

    // Move nodes from iBundleFolders to iLoadedNodes to update the bundle folders in proxy
    // side.
    User::LeaveIfError( 
        iLoadedNodes.Reserve( iLoadedNodes.Count() + iContentSourceMap->BundleFolderCount() ) );
        
    for ( TInt i = 0; i < iContentSourceMap->BundleFolderCount(); i++ ) 
        {
        // Append node ids to the loaded nodes list. These identifiers are
        // returned to the proxy side when the operation is completed.
        // Note that the content sources contain node identifiers, 
        // not metadata identifiers.
        CNcdNodeIdentifier* bundle = CNcdNodeIdentifier::NewLC(
            iContentSourceMap->BundleFolder( i ) );

        DLINFO((_L("Root Bundle ns: %S, id_: %S"), 
                &bundle->NodeNameSpace(),
                &bundle->NodeId()));

        iLoadedNodes.AppendL( bundle );

        // Do not delete because array takes ownership.
        CleanupStack::Pop( bundle );
        }
        
    DLTRACEOUT(("Ok"));        
    }
    
void CNcdLoadRootNodeOperation::SetAlwaysVisibleFlagsL() 
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

     
void CNcdLoadRootNodeOperation::ParseCatalogBundleL(
    const MNcdConfigurationProtocolDetail& aDetail ) 
    {  
    DLTRACEIN((""));
    DASSERT( aDetail.GroupId() == KCatalogBundle );
    DASSERT( aDetail.Id() == KNameSpace );
    
    const RPointerArray<MNcdConfigurationProtocolContent>& bundleContents = 
        aDetail.Contents();
        
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
        
    // Create folder for catalog bundle.
    DASSERT( bundleId );
    
    if (!bundleId)
        {
        User::Leave(KErrNotFound);
        }
    
    CNcdNodeIdentifier* metaDataId = CNcdNodeIdentifier::NewLC(
        aDetail.Value(), *bundleId, iPendingMessage->Session().Context().FamilyId() );
    DLINFO((_L("Bundle metaid ns: %S, id: %S"), 
            &metaDataId->NodeNameSpace(),
            &metaDataId->NodeId()));

    // The id has been gotten from the net server. So, it is same as
    // the metadata id that should be used here. Note that the creation of bundle requires the
    // metadataid (instead actual nodeid) for the parameter, and that we have here :)
    // Because the bundle folder is always the child of the root, the function can automatically
    // create the correct node id for the folder. 
    CNcdNodeFolder& bundleFolder = 
        iNodeManager->CreateBundleFolderL( *metaDataId );
        
    // Create the link for the bundle
    CNcdNodeLink& link = bundleFolder.CreateAndSetLinkL();
    
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
        
    iNodeManager->DbSaveNodeMetaDataL( bundleMetaData );          

    // Set the metadata to the bundle folder because it was not set during creation.
    bundleFolder.SetNodeMetaDataL( bundleMetaData );

    // Set the bundle folder as child of root node.
    // Bundle folders are always children of the root.
    iNodeManager->AddToParentL( *iRootNodeIdentifier, *metaDataId, 
                                CNcdNodeFactory::ENcdNodeRoot,
                                CNcdNodeFactory::ENcdNormalNode,
                                CNcdNodeFactory::ENcdBundleNode,
                                CNcdNodeManager::EReplace, iBundleInsertIndex );
    CleanupStack::PopAndDestroy( metaDataId );
    metaDataId = NULL;

    // Because AddToParentL only saves the parent information after child is added to it
    // we should always save the bundle node here. So, it will be usable after client is
    // restarted.
    iNodeManager->DbSaveNodeL( bundleFolder );        

    // Disabled bundle handling temporarily because of NCDALTCI-667
    // Bundles are now loaded explicitly with CNcdLoadBundleNodeOperation because
    // otherwise transparent folders are not refreshed when they are supposed to
/*
    // Parse sub catalogs.            
    for ( TInt i = 0; i < subCatalogs.Count(); i++ ) 
        {
        MNcdConfigurationProtocolDetail* subCatalog = subCatalogs[i];
        if ( subCatalog->GroupId() != KSubCatalogs ) 
            {
            continue;
            }
        
        // Get namespace
        DASSERT( subCatalog->Id() == KNameSpace );
        const TDesC& nameSpace = subCatalog->Value();
        DLINFO((_L("Bundle subcatalog ns(/value): %S"), 
                &subCatalog->Value()));
        
        //Get uri
        const TDesC* uri = NULL;
        const RPointerArray<MNcdConfigurationProtocolDetail>& subCatalogDetails =
            subCatalog->Details();
        for ( TInt i = 0; i < subCatalogDetails.Count(); i++ ) 
            {
            const MNcdConfigurationProtocolDetail* detail = subCatalogDetails[i];
            if ( detail->Id() == KUri )
                {
                uri = &detail->Value();
                DLINFO((_L("Bundle subcatalog detail uri(/value): %S"), 
                        &detail->Value()));
                }
            }
        if( uri == NULL )
            {
            DLTRACE(("Uri not found -> incomplete, skip"));
            continue;
            }
        
        CNcdNodeIdentifier* subCatalogIdentifier = NULL;
        const TDesC* provider = NULL;
        TBool transparent = EFalse;
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
                    CNcdNodeIdentifier::NewLC( nameSpace,
                                               content->Value(),
                                               *uri,
                                               bundleFolder.Identifier().ClientUid() );

                subCatalogIdentifier =
                    NcdNodeIdentifierEditor::CreateNodeIdentifierLC( bundleFolder.Identifier(),
                                                                      *contentIdentifier );
                CleanupStack::Pop( subCatalogIdentifier );
                CleanupStack::PopAndDestroy( contentIdentifier );
                CleanupStack::PushL( subCatalogIdentifier );
                DLINFO((_L("Bundle subcatalog content set node id(/value): %S"), 
                        &subCatalogIdentifier->NodeId()));
                }
            else if ( content->Key() == KProvider ) 
                {
                provider = &content->Value();
                DLINFO((_L("Bundle subcatalog content set provider (/value): %S"), 
                        provider));
                }
            else if ( content->Key() == KTransparent ) 
                {
                NcdProtocolUtils::DesToBool( transparent, content->Value() );
                }
            }
            
        if( subCatalogIdentifier == NULL )
            {
            DLTRACE(("Id not found -> incomplete, skip"));
            continue;
            }
        
        // Create the actual child folder
        CNcdNodeFolder* bundleChild = NULL;
        if( transparent )
            {
            bundleChild = &iNodeManager->CreateNodeFolderL(
                CNcdNodeFactory::ENcdTransparentNode, *subCatalogIdentifier );
            }
        else
            {
            bundleChild = &iNodeManager->CreateNodeFolderL(
                CNcdNodeFactory::ENcdNormalNode, *subCatalogIdentifier );
            }
        CleanupStack::PopAndDestroy( subCatalogIdentifier );
                    
        // Create a link for the child
        CNcdNodeLink& bundleChildLink = bundleChild->CreateAndSetLinkL();
        
        if ( provider )
            {
            bundleChildLink.SetCatalogsSourceNameL( *provider );
            }
        
        // Also, notice that it is essential to insert the metadata identifier into the
        // link info. So, the right metadata will be found when the node is opened
        // from the database. For example when application has been started.
        CNcdNodeIdentifier* bundleChildMeta =
            NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( bundleChild->Identifier() );
        bundleChildLink.SetMetaDataIdentifierL( *bundleChildMeta );
        CleanupStack::PopAndDestroy( bundleChildMeta );
        
        // Set uri for the child
        bundleChildLink.SetServerUriL( *uri );
        
        // Add the child to the bundle.
        if ( transparent )
            {
            iNodeManager->AddToParentL( bundleFolder.Identifier(),
                                    bundleChildLink.MetaDataIdentifier(), 
                                    CNcdNodeFactory::ENcdNodeFolder,
                                    CNcdNodeFactory::ENcdBundleNode,
                                    CNcdNodeFactory::ENcdTransparentNode,
                                    CNcdNodeManager::EAppend, 
                                    0,
                                    transparent );
            }
        else
            {
            iNodeManager->AddToParentL( bundleFolder.Identifier(),
                                    bundleChildLink.MetaDataIdentifier(), 
                                    CNcdNodeFactory::ENcdNodeFolder,
                                    CNcdNodeFactory::ENcdBundleNode,
                                    CNcdNodeFactory::ENcdNormalNode,
                                    CNcdNodeManager::EAppend, 
                                    0,
                                    transparent );
            }
        iNodeManager->DbSaveNodeL( *bundleChild );
        }
    */
    iContentSourceMap->AppendFolderL( bundleFolder.Identifier(), iBundleInsertIndex );
    
    // increment insert index for next bundle
    iBundleInsertIndex++;
    
    // Because AddToParentL only saves the parent information after child is added to it
    // we should always save the bundle node here. So, it will be usable after client is
    // restarted.
    
    // Unnecessary while bundle handling is disabled
    //iNodeManager->DbSaveNodeL( bundleFolder );
    }
    
    
TBool CNcdLoadRootNodeOperation::ContainsNode(
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
            
    
void CNcdLoadRootNodeOperation::HandleConfigurationDataRequestMessage(
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
TInt CNcdLoadRootNodeOperation::RunOperation()
    {
    DLTRACEIN((("this-ptr: %x"), this ));
  
    TInt err( KErrNone );
    TRAP( err, DoRunOperationL() );
     
     if ( err != KErrNone )
        {
        DLTRACE(("error: %d", err));
        Cancel();
        iRootNodeState = EFailed;
        iError = err;
        if ( iPendingMessage )
            {
            // ignoring error because operation already failed
            CNcdBaseOperation::CompleteMessage( iPendingMessage,
                ENCDOperationMessageCompletionError, iError );
            }
        }

    DLTRACEOUT(("err: %d", err));
    return err;    
    }

    
void CNcdLoadRootNodeOperation::DoRunOperationL()
    {
    DLTRACEIN((""));
    switch ( iRootNodeState )
        {
        case EConfRequest:
            {            
            DLTRACE((_L("->EConfRequest")));
            DASSERT ( iPendingMessage );
                        
            CNcdRootNode& root = 
                iNodeManager->CreateRootL( iPendingMessage->Session().Context() );
            root.CreateAndSetLinkL();
            
            if ( !iRootNodeIdentifier )
                {
                // create identifier for root node
                iRootNodeIdentifier = CNcdNodeIdentifier::NewL( root.Identifier() );

                // Store previous list only if some children have been previously loaded.
                if( root.ChildrenPreviouslyLoaded() )
                    {
                    root.StoreChildrenToPreviousListL();
                    }

                // Create previous lists of children for new checking.
                iChildEntityMaps.ResetAndDestroy();
                iNodeManager->SeenInfo().CreatePreviousListsForChildrenL( root, iChildEntityMaps );                    
                    
                iNodeManager->BackupAndClearCacheL( *iRootNodeIdentifier );
                iNodeManager->LockNodeDbL( TUid::Uid( iClientUid ) );
                iNodeDbLocked = ETrue;                  
                }
                            
            // remove old content sources
            delete iContentSourceMap;
            iContentSourceMap = NULL;
            iContentSourceMap = CNcdContentSourceMap::NewL();
                                    
            delete iServerUri;
            iServerUri = NULL;
            iServerUri = iConfigManager.MasterServerAddressL(
                iPendingMessage->Session().Context() ).AllocL();
            
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
            
            
            iRootNodeState = EReceiveConf;
            DLTRACE(( "->EConfRequest done" ));
            break;
            }
            
        case EReceiveConf:
            {
            DLTRACE((_L("->EReceiveConf")));
            // Should send progress only if progress is made
            
            if ( iPendingMessage && iSendProgress )
                {
                User::LeaveIfError( CNcdBaseOperation::CompleteMessage( iPendingMessage,
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
                        iRootNodeState = EBrowseRequest;
                        }
                    else if ( iMasterServerRedirectionState == ERedirecting )
                        {
                        // Redirect. Delete conf query since it does not need
                        // responding to new master server.
                        DLINFO(("Redirect"));
                        iConfQuery->InternalRelease();
                        iConfQuery = NULL;                       
                        iRootNodeState = EConfRequest;
                        }
                    else
                        {
                        // no content sources received
                        // e.g. server didn't give any content sources 
                        // with the set provisioning, only a 
                        // "service not available..." message is received
                        iRootNodeState = EComplete;
                        }                                
                    }
                else
                    {
                    // NORMAL CASE:
                    // Respond to the query in a new request
                    iRootNodeState = EConfRequest;
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
                        iRootNodeState = EBrowseRequest;
                        }
                    else if ( iMasterServerRedirectionState == ERedirecting )
                        {
                        // Redirect. Delete conf query since it does not need
                        // responding to new master server.
                        DLINFO(("Redirect"));
                        iConfQuery->InternalRelease();
                        iConfQuery = NULL;                       
                        iRootNodeState = EConfRequest;
                        }                        
                    else
                        {
                        DLTRACE(("No sources received!"));
                        // no content sources received with an optional query
                        // this should never happen!                        
                        iError = KNcdErrorNoContentSources;
                        iRootNodeState = EFailed;
                        }                            
                    }
                else
                    {
                    DLTRACE(("Query not optional, operation will stop!"));
                    iRootNodeState = EComplete;
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
                
                // The parent identifier should be root here.
                // So, no need to change that parent identifier info. It does not
                // differ from the metadataid because root does not have metadata.
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
            
            iRootNodeState = EReceiveBrowse;
            DLINFO(("subop count: failed:%d completed:%d total:%d",
                iFailedSubOps.Count(), iCompletedSubOps.Count(), iSubOps.Count() ));
            if ( iFailedSubOps.Count() + iCompletedSubOps.Count() ==
                 iSubOps.Count() )
                {
                // all sub ops have either completed or failed
                // -> this operation is now complete
                iRootNodeState = EComplete;
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
                // release own reference and remove
                iSubOpQuerys[0]->InternalRelease();
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
                iRootNodeState = EComplete;
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
            
            CNcdRootNode& rootNode = 
                        iNodeManager->RootNodeL( iRootNodeIdentifier->ClientUid() ); 
            // Compare the ContentSourceMaps of previous root node load and this
            // load and delete the nodes that were removed from server.
            if ( iContentSourceMap && ( iContentSourceMap->ContentSourceCount() > 0 || 
                iContentSourceMap->BundleFolderCount() > 0 ) ) 
                {
                DLINFO(("Content source map count"));
                RevertNodesOfBrokenSourcesToCacheL();                                            
                AddNodesToDifferentParentsL();
                SetAlwaysVisibleFlagsL();

                // Give the new content source map to root node to be used in the next
                // root node load
                rootNode.SetContentSourceMap( iContentSourceMap );
                // Root node takes the ownership. 
                // So, set NULL here. So this class will not delete this.
                iContentSourceMap = NULL;
                // Now the RAM node cache should be updated correctly, save it to database.
                iNodeManager->UnlockNodeDb( TUid::Uid( iClientUid ) );
                iNodeDbLocked = EFalse;
                iNodeManager->DbSaveNodesL( *iRootNodeIdentifier );                                
                }
                
            if( iNodeDbLocked )
                {
                iNodeManager->UnlockNodeDb( TUid::Uid( iClientUid ) );
                iNodeDbLocked = EFalse;
                }
            
            iNodeManager->ClearTempCacheL( *iRootNodeIdentifier );
            
            // Set the children loaded flag so that next refresh stores
            // previous child list (needed for new checking)
            if( rootNode.ChildCount() )
                {
                rootNode.SetChildrenPreviouslyLoaded();
                iNodeManager->DbSaveNodeL( rootNode );
                }
            // Check new status
            iNodeManager->SeenInfo().StorePreviousListsToExistingChildrenL(
                rootNode,
                iChildEntityMaps );
                
            iChildEntityMaps.ResetAndDestroy();
            
            // Check new status for transparent folders (transparent folders are
            // loaded during root op so they need to be checked here)
            iNodeManager->SeenInfo().DoNewCheckForTransparentChildrenL(
                            rootNode );
                            
            iNodeManager->SeenInfo().RefreshFolderSeenStatusL( *iRootNodeIdentifier );
            
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
                    User::LeaveIfError( CNcdBaseOperation::CompleteMessage( iPendingMessage,
                        ENCDOperationMessageCompletionComplete,
                        iProgress,
                        KErrNone ) );
                    
                    iOperationState = EStateComplete;
                    }
                else 
                    {
                    iRootNodeState = EFailed;
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
                CNcdBaseOperation::CompleteMessage( iPendingMessage,
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
    
void CNcdLoadRootNodeOperation::ChangeToPreviousStateL()
    {
    DLTRACEIN(("no implementation needed"));
    }

TBool CNcdLoadRootNodeOperation::QueryCompletedL( CNcdQuery* aQuery )
    {    
    DLTRACEIN((""));
    TBool handled = EFalse;
    if ( aQuery == iConfQuery )
        {
        // handle conf query
        DASSERT( iRootNodeState == EConfQuery );
        //RunOperation();
        // Query has been handled, release it
        /*iConfQuery->Release();
        iConfQuery = NULL;*/
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
