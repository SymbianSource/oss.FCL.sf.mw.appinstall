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
#include <limits.h>

#include "ncdloadnodeoperationimpl.h"
#include "ncdoperationfunctionids.h"
#include "catalogsbasemessage.h"
#include "catalogsbigdes.h"
#include "ncdrequestgenerator.h"

#include "ncdrequestbase.h"
#include "ncdrequestbrowsesearch.h"
#include "ncdrequestconfiguration.h"
#include "ncd_pp_itemref.h"
#include "ncd_pp_folderref.h"
#include "ncd_pp_dataentity.h"
#include "ncd_pp_error.h"
#include "ncd_pp_datablock.h"
#include "ncd_pp_icon.h"
#include "ncdprotocolutils.h"
#include "ncdprotocol.h"
#include "ncdprotocolimpl.h"
#include "ncdparser.h"
#include "ncdnodemanager.h"
#include "ncdproviderdefines.h"
#include "ncdnodeidentifier.h"
#include "ncdnodeclassids.h"
#include "ncdnodefolder.h"
#include "ncdoperationobserver.h"
#include "catalogssession.h"
#include "ncdnodeimpl.h"
#include "ncdnodelink.h"
#include "ncdqueryimpl.h"
#include "catalogsutils.h"
#include "ncd_cp_query.h"
#include "ncdnodemetadata.h"
#include "ncdnodemetadataimpl.h"
#include "ncderrors.h"
#include "ncdoperationremovehandler.h"
#include "ncdnodeiconimpl.h"
#include "ncdsessionhandler.h"
#include "ncdnodefactory.h"
#include "ncdnodeidentifiereditor.h"
#include "ncdexpirednode.h"
#include "ncdhttputils.h"
#include "ncdproviderutils.h"
#include "catalogshttpincludes.h"
#include "ncdnodeseeninfo.h"
#include "ncdchildentity.h"
#include "ncdoperationqueue.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadNodeOperationImpl* CNcdLoadNodeOperationImpl::NewL(
    const CNcdNodeIdentifier& aNodeIdentifier,
    const CNcdNodeIdentifier& aParentIdentifier,
    CNcdNodeFactory::TNcdNodePurpose aParentNodePurpose,
    TNcdResponseFilterParams aFilterParams,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler* aRemoveHandler,
    MNcdOperationQueue* aOperationQueue,
    MCatalogsSession& aSession,
    TBool aLoadChildren,
    TNcdChildLoadMode aMode,
    TBool aIsSubOperation,
    TBool aCreateParent )
    {
    CNcdLoadNodeOperationImpl* self = CNcdLoadNodeOperationImpl::NewLC(
        aNodeIdentifier,
        aParentIdentifier,
        aParentNodePurpose,
        aFilterParams,        
        aGeneralManager,
        aHttpSession,
        aRemoveHandler,
        aOperationQueue,
        aSession,
        aLoadChildren,
        aMode,
        aIsSubOperation,
        aCreateParent );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadNodeOperationImpl* CNcdLoadNodeOperationImpl::NewLC(
    const CNcdNodeIdentifier& aNodeIdentifier,
    const CNcdNodeIdentifier& aParentIdentifier,
    CNcdNodeFactory::TNcdNodePurpose aParentNodePurpose,
    TNcdResponseFilterParams aFilterParams,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler* aRemoveHandler,
    MNcdOperationQueue* aOperationQueue,
    MCatalogsSession& aSession,
    TBool aLoadChildren,
    TNcdChildLoadMode aMode,
    TBool aIsSubOperation,
    TBool aCreateParent )
    {
    CNcdLoadNodeOperationImpl* self =
        new( ELeave ) CNcdLoadNodeOperationImpl( 
            aParentNodePurpose, 
            aFilterParams,
            aMode, 
            aLoadChildren, 
            aGeneralManager, 
            aHttpSession,
            aRemoveHandler, 
            aOperationQueue, 
            aSession, 
            aIsSubOperation, 
            aCreateParent );
    CleanupClosePushL( *self );
    self->ConstructL( aNodeIdentifier, aParentIdentifier );
    return self;
    }
    
CNcdLoadNodeOperationImpl* CNcdLoadNodeOperationImpl::NewLC(
    CNcdContentSource& aContentSource,
    CNcdContentSourceMap* aContentSourceMap,
    const CNcdNodeIdentifier& aParentIdentifier,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler* aRemoveHandler,
    MCatalogsSession& aSession )
    {
    CNcdLoadNodeOperationImpl* self =
        new( ELeave ) CNcdLoadNodeOperationImpl( 
            aGeneralManager, 
            aHttpSession, 
			aRemoveHandler, 
			aSession );
    CleanupClosePushL( *self );
    self->ConstructL( aContentSource, aContentSourceMap, aParentIdentifier );
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadNodeOperationImpl::~CNcdLoadNodeOperationImpl()
    {
    DLTRACEIN(( "this-ptr: %X", this ));

    // If the operation proxy is released without cancelling, eg. UI exits 
    // while node loading is going on, then the operation is not removed
    // from the operation queue.
    //
    // HandleReleaseMessage() would be a bit better place for this but I
    // don't want to implement it just for this :)
    NotifyCompletionOfQueuedOperation( ENCDOperationMessageCompletionComplete );
    
    delete iNodeIdentifier;
    delete iServerUri;
    delete iParentIdentifier;
    
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
    
    if( iLoadNodeQuery )
        {
        iLoadNodeQuery->InternalRelease();
        }
    
    iSubOpQuerys.Close();
    
    DLTRACE(("Deleting iLoadedNodes"));
    iLoadedNodes.ResetAndDestroy();
    
    DLTRACE(("Deleting iParser"));
    delete iParser;
    
    DLTRACE(("Releasing iTransaction"));
    if ( iTransaction )
        {
        iTransaction->Release();
        }
    
    DLTRACE(("Releasing iHttpSession"));
    iHttpSession.Release();
    
    iRemoteFolders.ResetAndDestroy();
    iTransparentChildFolders.ResetAndDestroy();
    iTransparentChildItems.ResetAndDestroy();
    
    iNodeIconMaps.ResetAndDestroy();

    DLTRACEOUT((""));  
    }

CNcdLoadNodeOperationImpl::TLoadNodeOperationState
    CNcdLoadNodeOperationImpl::State()
    {
    return iLoadNodeState;
    }
    
const RPointerArray<CNcdNodeIdentifier>&
    CNcdLoadNodeOperationImpl::LoadedNodes()
    {
    return iLoadedNodes;
    }

const CNcdNodeIdentifier* CNcdLoadNodeOperationImpl::NodeIdentifier() const
    {
    return iNodeIdentifier;
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CNcdLoadNodeOperationImpl::Start()
    {
    DLTRACEIN((""));
    if ( iOperationState == EStateStopped )
        {
        // Op not yet running, queue it
        iOperationState = EStateRunning;
        TInt err;
        // Do not add sub operations to queue, since the parent operation will jam then.
        if ( IsSubOperation() )
            {
            err = RunOperation();
            }
        else
            {
            DASSERT( iOperationQueue );            
            TRAP( err, iOperationQueue->QueueOperationL( *this ) );        
            }
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
void CNcdLoadNodeOperationImpl::Cancel()
    {
    DLTRACEIN(( "this-ptr: %X", this ));

    if ( iTransaction )
        {
        iTransaction->Cancel();
        iTransaction = NULL;
        }

    if ( iParser )
        {
        iParser->CancelParsing();
        }
        
    for ( TInt i = 0; i < iSubOps.Count(); i++ ) 
        {
        CNcdLoadNodeOperationImpl* operation = iSubOps[i];
        if ( iCompletedSubOps.Find( operation ) == KErrNotFound &&
             iFailedSubOps.Find( operation ) == KErrNotFound ) 
            {
            operation->Cancel();
            }
        } 
    
    DLTRACEOUT((""));
    }
    

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadNodeOperationImpl::HandleCancelMessage( MCatalogsBaseMessage* aMessage ) 
    {
    DLTRACEIN((""));
    CNcdBaseOperation::HandleCancelMessage( aMessage );
    if ( !IsSubOperation() )
        {
        DASSERT( iOperationQueue );    
        iOperationQueue->QueuedOperationComplete( *this );
        }
    }
    
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//    
TInt CNcdLoadNodeOperationImpl::CompleteMessage(
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
TInt CNcdLoadNodeOperationImpl::CompleteMessage(
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
TInt CNcdLoadNodeOperationImpl::CompleteMessage(
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
TInt CNcdLoadNodeOperationImpl::CompleteMessage(
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
void CNcdLoadNodeOperationImpl::HandleHttpEventL( 
        MCatalogsHttpOperation& aOperation, 
        TCatalogsHttpEvent aEvent )
    {
    DLTRACEIN(( "this-ptr: %X", this ));
       
    DASSERT( &aOperation == iTransaction );
    DASSERT( aOperation.OperationType() == ECatalogsHttpTransaction );

    TCatalogsTransportProgress progress( iTransaction->Progress() );
    
    // Are state and id needed?
    iProgress = TNcdSendableProgress( iLoadNodeState,
        iTransaction->OperationId().Id(), progress.iProgress,
        progress.iMaxProgress );

    switch( aEvent.iOperationState ) 
        {
        // Handle completed operation
        case ECatalogsHttpOpCompleted:
            {
            ReleasePtr( iTransaction );
            // Inform parser that no more data will be sent
            iParser->EndL();
            break;
            }        
        // Handle operation in progress
        case ECatalogsHttpOpInProgress:
            {
            if ( aEvent.iProgressState == ECatalogsHttpResponseBodyReceived )
                {
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
        
    DLTRACEOUT((""));    
    }
    
    
TBool CNcdLoadNodeOperationImpl::HandleHttpError( 
    MCatalogsHttpOperation& aOperation, 
    TCatalogsHttpError aError )    
    {
    DLTRACEIN(("Error type: %d, code: %d", aError.iType, aError.iError ));    
    
    DLINFO(( "this-ptr: %X", this ));

    DASSERT( &aOperation == iTransaction );
    
    if ( iLoadMode == EContentSource )
        {
        iContentSource->SetBroken( ETrue );
        }
    
    aOperation.Release();
    iTransaction = NULL;
    iError = aError.iError;
    iLoadNodeState = EFailed;
    RunOperation();
        
    DLTRACEOUT((""));
    return ETrue;
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadNodeOperationImpl::ParseError( TInt aErrorCode )
    {
    DLTRACEIN(("error:%d", aErrorCode ));
    DLINFO(( "this-ptr: %X", this ));
    // Hanlde only if this operation isn't already handling an error.
    // Canceling parsing may lead to such a situation.
    if( iLoadNodeState == EReceive && iError == KErrNone )
        {
        iLoadNodeState = EFailed;
        iError = aErrorCode;

        if ( iTransaction )
            {
            iTransaction->Cancel();        
            iTransaction = NULL;
            }
        
        // There's nothing we can do for errors here
        TRAP_IGNORE( ContinueOperationL() );
        }
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadNodeOperationImpl::ParseCompleteL( TInt aError )
    {
    DLTRACEIN((_L("error:%d, this: %x"), aError, this ));
               
    DLINFO(( "this-ptr: %X", this ));   


    if ( aError != KErrNone )
        {
        iError = aError;
        iLoadNodeState = EFailed;        
        ContinueOperationL();
        }
    // if iError != KErrNone, the error has already been handled
    else if ( iError == KErrNone )
        {
        DASSERT( iLoadNodeState == EReceive );
        // Completed queries have been responded to successfully, remove them.
        ClearCompletedQueries();
        if ( QueriesPending() )
            {
            HandleQuerysL();
            }
        else if ( RemoteFolderCount() )
            {
            // remote folders need to be loaded next
            iLoadNodeState = ERemote;
            ContinueOperationL();            
            }
        else
            {
            // no querys received, go to next state
            iLoadNodeState = EComplete;
            ContinueOperationL();
            }
        }
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadNodeOperationImpl::FolderRefL(
    MNcdPreminetProtocolFolderRef* aData )
    {
    DLTRACEIN(("%X",aData));
    DLINFO(( "this-ptr: %X", this ));

    // Normal PushL causes USER 42
    CleanupDeletePushL( aData );
    
    DLTRACE((_L("folder id=%S"),&aData->Id()));
        
    switch ( iLoadMode )
        {
        case EContentSource:
            {
                
            DLINFO((_L("Cs: id=%S"), &iContentSource->NodeId() ));
            //PrintNodeChildren();
            CNcdNodeManager::TNcdRefHandleMode insertMode = 
                aData->ParentId() == KNullDesC ? 
                    CNcdNodeManager::EInsert : CNcdNodeManager::EAppend;
            
            CNcdNode* node( NULL );                    
            if ( iContentSource->IsTransparent() && aData->ParentId() == KNullDesC ) 
                {
                DLINFO(("Transparent node"));
                // Check if the parent is actually bundle or root node
                if ( iContentSourceMap->HasBundleFolder( *iParentIdentifier ) ) 
                    {
                    // Child of bundle folder.
                    node = 
                        &iNodeManager->RefHandlerL(
                            *iParentIdentifier, *aData, iClientUid, insertMode,
                            iContentSourceMap->GetInsertIndexL(*iContentSource, *iParentIdentifier ),
                            CNcdNodeFactory::ENcdNodeFolder, CNcdNodeFactory::ENcdBundleNode,
                            CNcdNodeFactory::ENcdTransparentNode );
                    }
                else
                    {                        
                    // Child of root node.
                	node =
                    	&iNodeManager->RefHandlerL(
                        	*iParentIdentifier, *aData, iClientUid, insertMode,
                        	iContentSourceMap->GetInsertIndexL(*iContentSource, *iParentIdentifier ),
                        	CNcdNodeFactory::ENcdNodeRoot, CNcdNodeFactory::ENcdNormalNode,
                        	CNcdNodeFactory::ENcdTransparentNode );
					}
                        
                // The content sources are thought to be remote.
                node->NodeLinkL().SetRemoteFlag( ETrue );
                }
            else if ( iContentSource->IsTransparent() ) 
                {
                DLINFO(("Transparent child"));
                // Because we have the transparent content source and the parent id was set
                // it means that we have loaded the child of the transparent folder.
                CNcdNodeIdentifier* metaDataParentIdentifier =
                    CNcdNodeIdentifier::NewLC( aData->ParentNamespace(),
                                               aData->ParentId(),
                                               aData->ServerUri(),
                                               iClientUid );
                CNcdNodeIdentifier* actualParentIdentifier =
                    NcdNodeIdentifierEditor::CreateNodeIdentifierLC( *iParentIdentifier,
                                                                     *metaDataParentIdentifier );
                node = 
                    &iNodeManager->RefHandlerL(
                        *actualParentIdentifier, *aData, iClientUid, insertMode,
                        0, CNcdNodeFactory::ENcdNodeFolder,
                        CNcdNodeFactory::ENcdTransparentNode,
                        CNcdNodeFactory::ENcdChildOfTransparentNode );
                        
                CleanupStack::PopAndDestroy( actualParentIdentifier );
                CleanupStack::PopAndDestroy( metaDataParentIdentifier );
                CNcdNodeIdentifier* nodeId = 
                    CNcdNodeIdentifier::NewLC( node->Identifier() );
                
                if ( aData->RemoteUri() != KNullDesC ) 
                    {
                    DASSERT( node->NodeLinkL().RemoteUri() == aData->RemoteUri() );

                    // Set the remote flag value just in case server has changed its settings
                    // from before.
                    node->NodeLinkL().SetRemoteFlag( ETrue );
                    iRemoteFolders.AppendL( nodeId );
                    }
                else
                    {
                    iTransparentChildFolders.AppendL( nodeId );
                    }
                    
                CleanupStack::Pop( nodeId );
                }
            else if ( aData->ParentId() == KNullDesC )
                {
                // Check if the parent is actually bundle or a real content source
                if ( iContentSourceMap->HasBundleFolder( *iParentIdentifier ) )
                    {
                    DLINFO(("Bundle content source"));
                    node = 
                        &iNodeManager->RefHandlerL(
                            *iParentIdentifier, *aData, iClientUid, insertMode,
                            iContentSourceMap->GetInsertIndexL( *iContentSource, *iParentIdentifier ),
                            CNcdNodeFactory::ENcdNodeFolder, CNcdNodeFactory::ENcdBundleNode,
                            CNcdNodeFactory::ENcdNormalNode );
                            
                    // The node is thought as a remote because it is direct child of bundle
                    node->NodeLinkL().SetRemoteFlag( ETrue );
                    }
                else
                    {
                    DLINFO(("Normal content source"));
                    node =
                        &iNodeManager->RefHandlerL(
                            *iParentIdentifier, *aData, iClientUid, insertMode,
                            iContentSourceMap->GetInsertIndexL( *iContentSource, *iParentIdentifier ),
                            CNcdNodeFactory::ENcdNodeRoot, CNcdNodeFactory::ENcdNormalNode,
                            CNcdNodeFactory::ENcdNormalNode );                        
                    // All the content sources are thought as a remote nodes because they are
                    // catalogs.                        
                    node->NodeLinkL().SetRemoteFlag( ETrue );
                    }
                    
                if( aData->RemoteUri() != KNullDesC )
                    {
                    // empty browse req. may give remote folders
                    CNcdNodeIdentifier* nodeId = 
                        CNcdNodeIdentifier::NewLC( node->Identifier() );
                    DASSERT( node->NodeLinkL().RemoteUri() == aData->RemoteUri() );
                    // Because this is remote node make sure that the flag is correct.
                    // No need the node here, because it will be saved again when remote
                    // folders are loaded.
                    node->NodeLinkL().SetRemoteFlag( ETrue );
                    iRemoteFolders.AppendL( nodeId );
                    CleanupStack::Pop( nodeId );
                    }
                }

            // Notice that we do not need to handle the children of the content sources here
            // even if they would be loaded for some reason. So, all the necessary situations
            // have been handled above.
                
            if ( node != NULL )
                {                    
                node->CreateAndSetLinkL().
                    SetCatalogsSourceNameL( iContentSource->Provider() );
                iNodeManager->DbSaveNodeL( *node );

                // Notice that the content source contains the node identifier, not
                // metadata identifier.
                if ( aData->ParentId() == KNullDesC ) 
                    {                    
                    CNcdNodeIdentifier* contentIdentifier =
                        CNcdNodeIdentifier::NewLC( node->Identifier() );
                    // Note that ownership of the identifier is transferred.
                    iContentSourceMap->AddNodeToContentSourceL( 
                        contentIdentifier, *iContentSource );
                    CleanupStack::Pop( contentIdentifier );
                    }
                }

            break;
            }
            
        case ESingleNode:
            {
            DLINFO(("Single node"));
            DASSERT( iNodeIdentifier );
            DLINFO(("iNodeIdentifier: ns= %S, id= %S, aData: ns= %S, id= %S",
                &iNodeIdentifier->NodeNameSpace(), &iNodeIdentifier->NodeId(),
                &aData->Namespace(), &aData->Id() ));
            
            // Because aData contains metadata ids, we have to get
            // the metadata id from the iNodeIdentifier
            CNcdNodeIdentifier* metaIdentifier =
                NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( *iNodeIdentifier );
            CNcdNode& currentNode = iNodeManager->NodeL( *iNodeIdentifier );
            // Structure is added only for the node that is being loaded,
            // everything else is dumped.
            if ( aData->Id() == metaIdentifier->NodeId() && 
                aData->Namespace() == metaIdentifier->NodeNameSpace() )
                {
                DLINFO(("ESingleNode, adding parent"));
                    
                iNodeManager->RefHandlerL( *iParentIdentifier,
                    *aData,
                    iClientUid,
                    CNcdNodeManager::EUpdate,
                    0,
                    iParentType,
                    iParentPurpose,
					CNcdNodeFactory::NodePurposeL( currentNode ),
				    iCreateParent );
                    
                }
            CleanupStack::PopAndDestroy( metaIdentifier );
            break;
            }
        case EChildren:
            {
            DLINFO(("Children"));
            DASSERT( iNodeIdentifier );
            
            // The comparison has to be made here between the metadata infos
            // So, get the parent node from the manager. So, the node metadata info
            // can be gotten. Notice that the parent already has the link info where
            // we can get its metadata identifier.
            CNcdNode& currentNode = iNodeManager->NodeL( *iNodeIdentifier );
            if ( aData->Id() == currentNode.NodeLinkL().MetaDataIdentifier().NodeId() && 
                 aData->Namespace() == currentNode.NodeLinkL().MetaDataIdentifier().NodeNameSpace() )
                {
                DLINFO(("EChildren Add parent"));
                
                // add parent
                iNodeManager->RefHandlerL( *iParentIdentifier,
                    *aData,
                    iClientUid,
                    CNcdNodeManager::EUpdate,
                    0,
                    iParentType,
                    iParentPurpose,
                    CNcdNodeFactory::NodePurposeL( currentNode ),
                    iCreateParent );
                // Structure loaded for parent -> send update notification
                // for parent node so that it gets internalized.
                CNcdNodeIdentifier* loadedNodeId = 
                    CNcdNodeIdentifier::NewLC( currentNode.Identifier() );
                iLoadedNodes.AppendL( loadedNodeId );
                CleanupStack::Pop( loadedNodeId );
                }
            else
                {
                DLINFO(("EChildren add child"));
                // add child
                // Because aData item did not match the currentNode it means that
                // we are loading the child of the current node.
                // The iNodeIdentifier identifies the parent whose children should be loaded.
                CNcdNode& node = 
                    iNodeManager->RefHandlerL( *iNodeIdentifier,
                                               *aData,
                                               iClientUid,
                                               CNcdNodeManager::EReplace,
                                               iNodeIndex++,
                                               CNcdNodeFactory::NodeTypeL( currentNode ),
                                               CNcdNodeFactory::NodePurposeL( currentNode ) );

                // Before saving the node information make sure that the node remote info is
                // set correctly.
                if ( aData->RemoteUri() != KNullDesC )
                    {
                    DLINFO((_L("Remote node: %S"), &node.Identifier().NodeId()));
                    node.NodeLinkL().SetRemoteFlag( ETrue );
                    CNcdNodeIdentifier* identifier = CNcdNodeIdentifier::NewLC( node.Identifier() );
                    iRemoteFolders.AppendL( identifier );
                    CleanupStack::Pop( identifier );
                    }
                else
                    {
                    DLINFO((_L("Normal node: %S"), &node.Identifier().NodeId()));
                    node.NodeLinkL().SetRemoteFlag( EFalse );
                    }
                iNodeManager->DbSaveNodeL( node );
                }
            break;
            }
        default:
            {
            DASSERT(0)
            User::Leave( KErrGeneral );
            break;
            }
        }
    
    // Delete data because ownership has been transferred.
    CleanupStack::PopAndDestroy( aData );
    
    RunOperation();        
    DLTRACEOUT((""));
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//    
void CNcdLoadNodeOperationImpl::FolderDataL(
    MNcdPreminetProtocolDataEntity* aData )
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    
    // Normal PushL causes USER 42
    CleanupDeletePushL( aData );
    
    // This node will contain the metadata that is updated by calling
    // the handler function.
    // Notice that after this the metadata has also been created.
    // So, metadata can be directly used.
    CNcdNodeIdentifier* parentIdentifier( NULL );
    TBool addMetaData = ETrue;
    if( iLoadMode == EChildren || iLoadMode == ESingleNode )
        {
        DLINFO(("EChildren"));
        DASSERT( iNodeIdentifier );
        CNcdNodeIdentifier* metaIdentifier = 
            NcdNodeIdentifierEditor::CreateMetaDataIdentifierL( *iNodeIdentifier );
        if ( aData->Id() != metaIdentifier->NodeId() || 
             aData->Namespace() != metaIdentifier->NodeNameSpace() )
            {
            // aData must be child of iNodeIdentifier
            delete metaIdentifier;
            metaIdentifier = NULL;
            if( iLoadMode == EChildren )
                {
                parentIdentifier = CNcdNodeIdentifier::NewLC( *iNodeIdentifier );
                }
            else
                {
                // Don't add child metadata in ESingleNode mode.
                addMetaData = EFalse;
                }
            }
        else 
            {
            delete metaIdentifier;
            metaIdentifier = NULL;
            }                
        }
    else if ( iLoadMode == EContentSource && iContentSource->IsTransparent() ) 
        {
        DLINFO(("EContentSource and transparent"));
        // We have to check if the folder data belongs to a folder inside a transparent
        // folder.
        CNcdNodeIdentifier* metaIdentifier = CNcdNodeIdentifier::NewLC(
            aData->Namespace(), aData->Id(), aData->ServerUri(), iClientUid );
        for ( TInt i = 0; i < iTransparentChildFolders.Count(); i++ ) 
            {
            CNcdNodeIdentifier* childOfTransparent = iTransparentChildFolders[i];
            CNcdNodeIdentifier* metaOfChild =
                NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( *childOfTransparent );
            DASSERT( metaOfChild != NULL );
            if ( metaOfChild->Equals( *metaIdentifier ) ) 
                {
                parentIdentifier = NcdNodeIdentifierEditor::ParentOfLC( *childOfTransparent );
                CleanupStack::Pop( parentIdentifier );
                CleanupStack::PopAndDestroy( metaOfChild );
                break;
                }
            CleanupStack::PopAndDestroy( metaOfChild );                
            }
        CleanupStack::PopAndDestroy( metaIdentifier );
        if ( parentIdentifier ) 
            {                
            CleanupStack::PushL( parentIdentifier );
            }
        }
    else if ( iLoadMode == EContentSource )
        {
        DLINFO(("EContentSource"));
        DLTRACE(("Check that node ref already exists."));
        TBool nodeFound = EFalse;
        RPointerArray<CNcdNodeIdentifier>& csNodes = iContentSourceMap->NodesL( *iContentSource );
        for( TInt i = 0 ; i < csNodes.Count() ; i++ )
            {
            CNcdNodeIdentifier* metaIdentifier =
                NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( *csNodes[i] );
            if( metaIdentifier->NodeNameSpace() == aData->Namespace() &&
                metaIdentifier->NodeId() == aData->Id() )
                {
                DLTRACE(("Node ref has been added, proceed with metadata adding."));
                nodeFound = ETrue;
                CleanupStack::PopAndDestroy( metaIdentifier );
                break;
                }
            CleanupStack::PopAndDestroy( metaIdentifier );
            }
        if ( !nodeFound )
            {
            DLTRACE(("Node ref has NOT been added, don't add metadata either."));
            addMetaData = EFalse;
            }
        }
        
        
    if( addMetaData )
        {
        // set iParentIdentifier as parent if not set otherwise, this would be the case of a
        // normal top-level node from a content source
        if ( ! parentIdentifier ) 
            {
            parentIdentifier = CNcdNodeIdentifier::NewLC( *iParentIdentifier );
            }
            
        CNcdNode& node = 
            iNodeManager->DataHandlerL( *parentIdentifier, *aData, iClientUid );
            
        CleanupStack::PopAndDestroy( parentIdentifier );
        
        // Notice that the loaded nodes should contain the actual node identifier
        // instead of metadata identifier, because the identifiers are returned to
        // the proxy side after operation completes.
        CNcdNodeIdentifier* loadedNodeId = 
            CNcdNodeIdentifier::NewLC( node.Identifier() );
        iLoadedNodes.AppendL( loadedNodeId );
        CleanupStack::Pop( loadedNodeId );
        
        DLINFO(( _L("node loaded, id: %S"), &node.Identifier().NodeId() ));

        // If the data contains icon id and datablock id, they are stored until
        // the datablock arrives later (in DataBlocksL method).
        const MNcdPreminetProtocolIcon* icon = aData->Icon();
        if ( icon != NULL ) 
            {
            const TDesC& iconId = icon->Id();
            const TDesC& dataBlockId = icon->DataBlock();
            if ( iconId != KNullDesC && dataBlockId != KNullDesC ) 
                {
                // Icon id may be mapped to the metadata id here
                MapIconIdForDataBlockL( iconId, dataBlockId, 
                                        node.NodeMetaDataL().Identifier() );
                // Notice that here we need to get the node by using the
                // parent identifier and metadata id, because the metadata
                // identifier itself is not enough to identify the node.
                node.NodeMetaDataL().IconL().SetIconDataReady( EFalse );
                }
            }
        }
    
    // Delete data because ownership has been transferred.
    CleanupStack::PopAndDestroy( aData );
    
    RunOperation();
    DLTRACEOUT((""));
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadNodeOperationImpl::ItemRefL( MNcdPreminetProtocolItemRef* aData )
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    
    
    // Normal PushL causes USER 42
    CleanupDeletePushL( aData );
    
    switch ( iLoadMode )
        {
        case EContentSource:
            {
            DLINFO(("EContentSource"));
            DASSERT( aData->ParentId() != KNullDesC );
           
            // The iParentIdentifier is not correct for items since they cannot be in root
            // level. We have to create the actual parent identifier and use it.
            CNcdNodeIdentifier* metaDataParentIdentifier =
                CNcdNodeIdentifier::NewLC(
                    aData->ParentNamespace(), aData->ParentId(),
                    aData->ServerUri(), iClientUid );
            CNcdNodeIdentifier* actualParentIdentifier =
                NcdNodeIdentifierEditor::CreateNodeIdentifierLC(
                    *iParentIdentifier, *metaDataParentIdentifier );
            
            // Check if the parent is actually transparent, scheme, bundle or a real content source
            if ( iContentSource->IsTransparent() ) 
                {
                DLINFO(("Content source transparent"));
                CNcdNode& node = iNodeManager->RefHandlerL( *actualParentIdentifier,
                                                            *aData,
                                                            iClientUid,
                                                            CNcdNodeManager::EAppend,
                                                            0,
                                                            CNcdNodeFactory::ENcdNodeFolder,
                                                            CNcdNodeFactory::ENcdTransparentNode,
                                                            CNcdNodeFactory::ENcdChildOfTransparentNode );
                CNcdNodeIdentifier* nodeId = 
                    CNcdNodeIdentifier::NewLC( node.Identifier() );
                iTransparentChildItems.AppendL( nodeId );
                CleanupStack::Pop( nodeId );                    
                }
            else if ( iContentSourceMap->HasBundleFolder( *iParentIdentifier ) )
                {
                DLINFO(("Bundle content source"));
                iNodeManager->RefHandlerL(  *actualParentIdentifier,
                                            *aData,
                                            iClientUid,
                                            CNcdNodeManager::EAppend,
                                            0,
                                            CNcdNodeFactory::ENcdNodeFolder,
                                            CNcdNodeFactory::ENcdBundleNode );
                }
            else if ( aData->ParentId() == KNullDesC )
                {
                DLINFO(("Normal content source"));
                iNodeManager->RefHandlerL(  *actualParentIdentifier,
                                            *aData,
                                            iClientUid,
                                            CNcdNodeManager::EAppend,
                                            0,
                                            CNcdNodeFactory::ENcdNodeFolder );
                }
            
            CleanupStack::PopAndDestroy( actualParentIdentifier );
            CleanupStack::PopAndDestroy( metaDataParentIdentifier );
            break;
            }
        case ESingleNode:
            {
            DLINFO(("Single node"));
            DASSERT( iNodeIdentifier );
            DLINFO(("iNodeIdentifier: ns= %S, id= %S, aData: ns= %S, id= %S",
                &iNodeIdentifier->NodeNameSpace(), &iNodeIdentifier->NodeId(),
                &aData->Namespace(), &aData->Id() ));
            
            // Because aData contains metadata ids, we have to get
            // the metadata id from the iNodeIdentifier
            CNcdNodeIdentifier* metaIdentifier =
                NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( *iNodeIdentifier );
            CNcdNode& currentNode = iNodeManager->NodeL( *iNodeIdentifier );
            if ( aData->Id() == metaIdentifier->NodeId() && 
                aData->Namespace() == metaIdentifier->NodeNameSpace() )
                {
                DLINFO(("ESingleNode, adding parent"));
                    
                iNodeManager->RefHandlerL( *iParentIdentifier,
                    *aData,
                    iClientUid,
                    CNcdNodeManager::EUpdate,
                    0,
                    iParentType,
                    iParentPurpose,
                    CNcdNodeFactory::NodePurposeL( currentNode ) );
                }
            CleanupStack::PopAndDestroy( metaIdentifier );
            break;
            }
        case EChildren:
            {
            DLINFO(("Children"));
            // Get the parent node. So, we can use its link to get the metadataidentifier.
            // The parent always has the link information set here.    
            CNcdNode& parentNode = iNodeManager->NodeL( *iNodeIdentifier );
            if ( aData->Id() == parentNode.NodeLinkL().MetaDataIdentifier().NodeId() && 
                aData->Namespace() == parentNode.NodeLinkL().MetaDataIdentifier().NodeNameSpace() )
                {
                // add parent
                iNodeManager->RefHandlerL( *iParentIdentifier,
                                            *aData,
                                            iClientUid,
                                            CNcdNodeManager::EUpdate,
                                            0,
                                            iParentType,
                                            iParentPurpose );
                }
            else
                {
                // add child
                // The iNodeIdentifier identifies the parent whose children should be loaded.
                iNodeManager->RefHandlerL( *iNodeIdentifier,
                                            *aData,
                                            iClientUid,
                                            CNcdNodeManager::EReplace,
                                            iNodeIndex++,
                                            CNcdNodeFactory::ENcdNodeFolder,
                                            CNcdNodeFactory::NodePurposeL( parentNode ) );
                }
            break;
            }
        default:
            {
            DASSERT(0)
            User::Leave( KErrGeneral );
            break;
            }
        }
    
    // Delete data because ownership has been transferred.
    CleanupStack::PopAndDestroy( aData ); 
    
    RunOperation();
    DLTRACEOUT((""));
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdLoadNodeOperationImpl::ItemDataL(
    MNcdPreminetProtocolDataEntity* aData )
    {
    DLTRACEIN(( "this-ptr: %X", this ));

    
   
        // Normal PushL causes USER 42
    CleanupDeletePushL( aData );
    TBool addMetaData = ETrue;
    CNcdNodeIdentifier* parentIdentifier( NULL );
    if( iLoadMode == EChildren || iLoadMode == ESingleNode )
        {
        DLINFO(("EChildren or ESingleNode"));
        DASSERT( iNodeIdentifier );
        CNcdNodeIdentifier* metaIdentifier = 
            NcdNodeIdentifierEditor::CreateMetaDataIdentifierL( *iNodeIdentifier );
        if ( aData->Id() != metaIdentifier->NodeId() || 
             aData->Namespace() != metaIdentifier->NodeNameSpace() )
            {
            // aData must be child of iNodeIdentifier
            delete metaIdentifier;
            metaIdentifier = NULL;
            if( iLoadMode == EChildren )
                {
                parentIdentifier = CNcdNodeIdentifier::NewLC( *iNodeIdentifier );
                }
            else
                {
                // Don't add child metadata in ESingleNode mode.
                addMetaData = EFalse;
                }
            }
        else 
            {
            delete metaIdentifier;
            metaIdentifier = NULL;
            }                
        }
    else if ( iLoadMode == EContentSource && iContentSource->IsTransparent() ) 
        {
        // parent is actually the transparent folder, 
        // not iParentIdentifier ( which is root node )
        CNcdNodeIdentifier* metaIdentifier = CNcdNodeIdentifier::NewLC(
            aData->Namespace(), aData->Id(), aData->ServerUri(), iClientUid );
        for ( TInt i = 0; i < iTransparentChildItems.Count(); i++ ) 
            {
            CNcdNodeIdentifier* childOfTransparent = iTransparentChildItems[i];
            CNcdNodeIdentifier* metaOfChild =
                NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( *childOfTransparent );
            DASSERT( metaOfChild != NULL );
            if ( metaOfChild->Equals( *metaIdentifier ) ) 
                {
                parentIdentifier = NcdNodeIdentifierEditor::ParentOfLC( *childOfTransparent );
                CleanupStack::Pop( parentIdentifier );
                CleanupStack::PopAndDestroy( metaOfChild );
                break;
                }
            CleanupStack::PopAndDestroy( metaOfChild );
            }
        CleanupStack::PopAndDestroy( metaIdentifier );
        if ( parentIdentifier )
            {
            CleanupStack::PushL( parentIdentifier );
            }
        }
    else if ( iLoadMode == EContentSource )
        {
        DLINFO(("EContentSource"));
        DLTRACE(("Check that node ref already exists."));
        TBool nodeFound = EFalse;
        RPointerArray<CNcdNodeIdentifier>& csNodes = iContentSourceMap->NodesL( *iContentSource );
        for( TInt i = 0 ; i < csNodes.Count() ; i++ )
            {
            CNcdNodeIdentifier* metaIdentifier =
                NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( *csNodes[i] );
            if( metaIdentifier->NodeNameSpace() == aData->Namespace() &&
                metaIdentifier->NodeId() == aData->Id() )
                {
                DLTRACE(("Node ref has been added, proceed with metadata adding."));
                nodeFound = ETrue;
                CleanupStack::PopAndDestroy( metaIdentifier );
                break;
                }
            CleanupStack::PopAndDestroy( metaIdentifier );
            }
        if ( !nodeFound )
            {
            DLTRACE(("Node ref has NOT been added, don't add metadata either."));
            addMetaData = EFalse;
            }
        }
    
    if( addMetaData )
        {
        // set iParentIdentifier as parent if not set otherwise, this would be the case of a
        // normal top-level node from a content source
        if ( ! parentIdentifier ) 
            {
            parentIdentifier = CNcdNodeIdentifier::NewLC( *iParentIdentifier );
            }
        
        // Get the node reference from the data handler.
        // The node has the given parent and its metadata
        // will be internalized with the given data.
        CNcdNode& node =
            iNodeManager->DataHandlerL( *parentIdentifier, *aData, iClientUid );
            
        CleanupStack::PopAndDestroy( parentIdentifier );

        // Notice that the loaded nodes should contain the actual node identifier
        // instead of metadata identifier, because the identifiers are returned to
        // the proxy side after operation completes.
        CNcdNodeIdentifier* loadedNodeId = 
            CNcdNodeIdentifier::NewLC( node.Identifier() );
        iLoadedNodes.AppendL( loadedNodeId );
        CleanupStack::Pop( loadedNodeId );        

        // If the data contains icon id and datablock id, they are stored until
        // the datablock arrives later.
        const MNcdPreminetProtocolIcon* icon = aData->Icon();
        if ( icon != NULL ) 
            {
            const TDesC& iconId = icon->Id();
            const TDesC& dataBlockId = icon->DataBlock();
            if ( iconId != KNullDesC && dataBlockId != KNullDesC ) 
                {
                // The node metadata was created by using the DataHandlerL
                // and inserted for the node.
                // So, the metadata can be asked from the node now.
                MapIconIdForDataBlockL(iconId, dataBlockId, 
                                       node.NodeMetaDataL().Identifier() );
                node.NodeMetaDataL().IconL().SetIconDataReady( EFalse );
                }
            }
        }
        
    // Delete data because ownership has been transferred.
    CleanupStack::PopAndDestroy( aData );
    
    RunOperation();
    DLTRACEOUT((""));
    }
    
void CNcdLoadNodeOperationImpl::Progress( CNcdBaseOperation& /*aOperation*/ )
    {
    
    }
    
void CNcdLoadNodeOperationImpl::QueryReceived( CNcdBaseOperation& /*aOperation*/,
    CNcdQuery* aQuery )
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    DASSERT( iLoadNodeState == EReceiveRemote )
    TRAPD( err, iSubOpQuerys.AppendL( aQuery ) );
    if( err != KErrNone )
        {
        iError = err;
        iLoadNodeState = EFailed;
        }
    aQuery->InternalAddRef();
    RunOperation();
    }
    
void CNcdLoadNodeOperationImpl::OperationComplete( CNcdBaseOperation* aOperation,
    TInt aError )
    {
    DLTRACEIN(("error=%d, iLoadNodeState=%d, operation=%x", 
        aError, iLoadNodeState, aOperation ));
    (void) aError; // suppresses compiler warning
    DLINFO(( "this-ptr: %X", this ));

    DASSERT( iLoadNodeState == EReceiveRemote || 
        iLoadNodeState == ERemote || 
        iLoadNodeState == EFailed );
    
    // Operation type can be ESearchOperation because CNcdSearchOperation
    // inherits from CNcdLoadNodeOperationImpl
    DASSERT( aOperation->Type() == ELoadNodeOperation ||
             aOperation->Type() == ESearchOperation );

    DLTRACE(("Failed subops: %d, completed: %d, total: %d", 
        iFailedSubOps.Count(), 
        iCompletedSubOps.Count(),
        iSubOps.Count() ));
    
    TRAPD(err, 
    CNcdLoadNodeOperationImpl* loadOp =
        static_cast<CNcdLoadNodeOperationImpl*>( aOperation );
        
    if ( loadOp->State() == CNcdLoadNodeOperationImpl::EFailed )
        {
        iFailedSubOps.AppendL( loadOp );
        }
    else if ( loadOp->State() == CNcdLoadNodeOperationImpl::EComplete )
        {
        DLTRACE(("Op was complete"));
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
    
    if ( iLoadNodeState == EReceiveRemote )
        {
        // call RunOperation only in this state,
        // otherwise RunOperation could call itself immediately
        // after starting a sub op
        // (sub-op start -> error -> complete callback -> run op )
        RunOperation();
        }
    ); //TRAPD
    
    DLTRACE(("Failed subops: %d, completed: %d, total: %d", 
        iFailedSubOps.Count(), 
        iCompletedSubOps.Count(),
        iSubOps.Count() ));
        
    if ( err != KErrNone )
        {
        iError = err;
        iLoadNodeState = EFailed;
        RunOperation();
        }

    DLTRACEOUT((""));
    }
    
void CNcdLoadNodeOperationImpl::DataBlocksL(
    CArrayPtr<MNcdPreminetProtocolDataBlock>* aData ) 
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    CleanupResetAndDestroyPushL( *aData );
    
    // Save the data blocks having icon data to database, taking advance of 
    // the mapping of icon IDs and datablock IDs.
    
    for ( TInt i = 0; i < aData->Count(); i++ ) 
        {
        MNcdPreminetProtocolDataBlock* dataBlock = (*aData)[i];
        DLINFO(( "datablock number: %d", i ));
        RPointerArray<CNcdNodeIconMap> icons = IconsForDataBlockL( dataBlock->Id() );
        CleanupResetAndDestroyPushL( icons );
        
        for ( TInt iconIndex = 0; iconIndex < icons.Count(); iconIndex++ )
            {
            DLINFO(( "icon number: %d", iconIndex ));
            CNcdNodeIconMap* map = icons[iconIndex];
            CNcdNodeIdentifier* metaDataId = map->iMetadataId;
            
            // Metadata should always exist if we are trying to handle the icon data.
            const CNcdNodeMetaData& metaData = 
                iNodeManager->NodeMetaDataL( *metaDataId );
            CNcdNodeIdentifier* iconIdentifier = CNcdNodeIdentifier::NewLC(
                metaData.Identifier().NodeNameSpace(), *map->iIconId,
                metaData.Identifier().ServerUri(), metaData.Identifier().ClientUid() );
            DLTRACE(("Saving icon data"));
            iNodeManager->DbSaveIconDataL( *iconIdentifier, dataBlock->Content() );
            DLTRACE(("Icon data saved"));
            CleanupStack::PopAndDestroy( iconIdentifier );
            iconIdentifier = NULL;                
            DLTRACE(("Marking icon data as ready"));
            // mark the icon data is ready
            metaData.IconL().SetIconDataReady( ETrue );                
            DLTRACE(("Icon data marked ready"));
            }         
            
        CleanupStack::PopAndDestroy( &icons );              
        }
    
    DLTRACE(("Calling default observer"));
    // aData is deleted by default observer
    iParser->DefaultObserver().DataBlocksL( aData );
    CleanupStack::Pop( aData );
    }
    
// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdLoadNodeOperationImpl::CNcdLoadNodeOperationImpl(
    CNcdNodeFactory::TNcdNodePurpose aParentNodePurpose,
    TNcdResponseFilterParams aFilterParams,
    TNcdChildLoadMode aMode,
    TBool aLoadChildren,
    CNcdGeneralManager& aGeneralManager,    
    MCatalogsHttpSession& aHttpSession,    
    MNcdOperationRemoveHandler* aRemoveHandler,
    MNcdOperationQueue* aOperationQueue,
    MCatalogsSession& aSession,
    TBool aIsSubOperation,
    TBool aCreateParent )
    : CNcdBaseOperation( aGeneralManager, aRemoveHandler, ELoadNodeOperation, 
        aSession, aIsSubOperation ),
      iAccessPointManager( aGeneralManager.AccessPointManager() ),
      iProtocol( aGeneralManager.ProtocolManager() ),
      iHttpSession( aHttpSession ),
      iFilterParams( aFilterParams ),
      iChildLoadMode( aMode ),
      iNodeIndex( aFilterParams.iPageStart ),
      iParentType( CNcdNodeFactory::ENcdNodeFolder ),
      iParentPurpose( aParentNodePurpose ),
      iCreateParent( aCreateParent ),
      iOperationQueue( aOperationQueue )
    {
    if ( aLoadChildren )
        {
        iLoadMode = EChildren;
        }
    else
        {
        iLoadMode = ESingleNode;
        }
    iLoadNodeState = ESendRequest;
    iProgress.iState = 0;
    iProgress.iOperationId = 0;
    iProgress.iProgress = 0;
    iProgress.iMaxProgress = 100;
    iHttpSession.AddRef();
    }

CNcdLoadNodeOperationImpl::CNcdLoadNodeOperationImpl(
    CNcdGeneralManager& aGeneralManager,    
    MCatalogsHttpSession& aHttpSession,    
    MNcdOperationRemoveHandler* aRemoveHandler,
    MCatalogsSession& aSession )
    : CNcdBaseOperation( aGeneralManager, aRemoveHandler, ELoadNodeOperation,
        aSession, ETrue ),
      iAccessPointManager( aGeneralManager.AccessPointManager() ),
      iProtocol( aGeneralManager.ProtocolManager() ),
      iHttpSession( aHttpSession ),
      iLoadMode( EContentSource ),
      iParentType( CNcdNodeFactory::ENcdNodeFolder )
    {
    iLoadNodeState = ESendRequest;
    iProgress.iState = 0;
    iProgress.iOperationId = 0;
    iProgress.iProgress = 0;
    iProgress.iMaxProgress = 100;
    iHttpSession.AddRef();
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CNcdLoadNodeOperationImpl::ConstructL(
    const CNcdNodeIdentifier& aNodeIdentifier,
    const CNcdNodeIdentifier& aParentIdentifier )
    {
    DLTRACEIN(( "this-ptr: %X", this ));

    CNcdBaseOperation::ConstructL();
    
    iNodeIdentifier = 
        CNcdNodeIdentifier::NewL( aNodeIdentifier );        
    iParentIdentifier = 
        CNcdNodeIdentifier::NewL( aParentIdentifier );
    iClientUid = aNodeIdentifier.ClientUid();
       
    DetermineParentTypeL( iClientUid );   
    DLTRACEOUT((""));
    }

void CNcdLoadNodeOperationImpl::ConstructL(
    CNcdContentSource& aContentSource,
    CNcdContentSourceMap* aContentSourceMap,
    const CNcdNodeIdentifier& aParentIdentifier )
    {
    DLTRACEIN((""));
    CNcdBaseOperation::ConstructL();
    iContentSourceMap = aContentSourceMap;
    iContentSource = &aContentSource;
    iParentIdentifier = CNcdNodeIdentifier::NewL( aParentIdentifier );
    iClientUid = aParentIdentifier.ClientUid();
       
    DetermineParentTypeL( iClientUid );
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
HBufC8* CNcdLoadNodeOperationImpl::CreateRequestLC(
    CNcdNodeIdentifier* aNodeIdentifier,    
    TNcdResponseFilterParams aFilterParams,
    const TDesC& aUri )
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    
    CNcdRequestBrowseSearch* req =
        NcdRequestGenerator::CreateBrowseRequestLC();
        
    switch ( iLoadMode )
        {
        case EContentSource:
            {
            DLTRACE(("EContentSource"));
            DASSERT( iContentSource )
            req->SetNamespaceL( iContentSource->NameSpace() );
            CDesC16ArrayFlat* elements = new (ELeave) CDesC16ArrayFlat(1);
            CleanupStack::PushL( elements );
            if ( iContentSource->NodeId() != KNullDesC() )
                {                
                DLTRACE((_L("Id found: %S, use it in the request"), &iContentSource->NodeId() ));
                CNcdNodeIdentifier* nodeId = CNcdNodeIdentifier::NewLC(
                    iContentSource->NameSpace(), iContentSource->NodeId(), 
                    iContentSource->Uri(), iClientUid );
                CNcdNodeIdentifier* metaId = NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( *nodeId );

                CNcdNode* node( NULL );
                // Node can tried by using the nodeId which is the identifier included in
                // the content source.
                TRAPD( err, node = &iNodeManager->NodeL( *nodeId ));
                CNcdNodeLink* link( NULL );
                if( err == KErrNone )
                    {
                    DLINFO(("EContentSource Node was found"));
                    link = node->NodeLink();
                    if ( link ) 
                        {                        
                        DLINFO(("node link was found"));
                        // node found, use timestamp
                        // Notice that the content source contains the node id.
                        // Request needs the meta id, so use it here.
                        req->AddEntityL( metaId->NodeId(),
                                         link->Timestamp(),
                                         ETrue );
                        }
                    }
                    
                if ( !link ) 
                    {
                    DLINFO(("EContentSource Node or link not found"));
                    // Notice that the content source contains the node id.
                    // Request needs the meta id, so use it here.
                    // node not found, no timestamp
                    req->AddEntityL( metaId->NodeId(), ETrue );
                    }
                CleanupStack::PopAndDestroy( metaId );
                CleanupStack::PopAndDestroy( nodeId );
                }

            if ( iContentSource->IsTransparent() ) 
                {
                // If content source is transparent, we need to get all the children of the
                // transparent folder too.
                req->AddResponseFilterL(
                    INT_MAX, // pageSize: get all children
                    0, // pageStart: start from the first child
                    1, // structureDepth: load child structure
                    1, // metaDataDepth: load child metadata
                    INT_MAX, // metaDataPerLevel: get all metadata
                    *elements,
                    *elements );
                }
            else
                {
                // Normal case, load the first child's structure to get the child count.
                req->AddResponseFilterL(
                    1, // pageSize: load one child
                    0, // pageStart: load the first child
                    1, // structureDepth: load child structure
                    0, // metaDataDepth: don't load child metadata
                    1, // metaDataPerLevel: only load one metadata per level
                    *elements,
                    *elements );
                }
            CleanupStack::PopAndDestroy( elements );
            DLTRACE(("EContentSource done"))
            break;
            }
        case ESingleNode:
            {
            DLTRACE(("ESingleNode"));
            DASSERT( aNodeIdentifier );
            // loading just one node, don't use filter params 
            // The parameter identifier means the actual node. So,
            // the id may be used to get the node from the manager.
            CNcdNode& node = iNodeManager->NodeL( *aNodeIdentifier );
            CDesC16ArrayFlat* elements = new (ELeave) CDesC16ArrayFlat(1);
            CleanupStack::PushL( elements );
            // Request uses the metadata information not the node information.
            req->SetNamespaceL( node.NodeLinkL().MetaDataIdentifier().NodeNameSpace() );
            req->AddEntityL( node.NodeLinkL().MetaDataIdentifier().NodeId(),
                             node.NodeLinkL().Timestamp(),
                             ETrue ); // Include metadata.
                        
            // We don't want to use a response filter with items because then
            // metadata won't be included in the response unless the filter has
            // metaDataDepth > 0. 
            if ( CNcdNodeFactory::NodeTypeL( node ) != CNcdNodeFactory::ENcdNodeItem ) 
                {
                req->AddResponseFilterL(
                    1, // pageSize: load one child, needed to get child count from Jamba backend
                    0, // pageStart: load the first child
                    1, // structureDepth: load child structure
                    0, // metaDataDepth: don't load metadata
                    0, // metaDataPerLevel: don't load metadata
                    *elements,
                    *elements );
                }
            CleanupStack::PopAndDestroy( elements );
            DLTRACE(("ESingleNode done"))
            break;
            }
        case EChildren:
            {
            DLTRACE(("EChildren"));
            DASSERT( aNodeIdentifier );
            // loading children, use filter params
            // The parameter identifier means the actual node. So,
            // the id may be used to get the node from the manager.
            CNcdNodeFolder& folder = iNodeManager->FolderL( *aNodeIdentifier );
            req->SetNamespaceL( folder.NodeLinkL().MetaDataIdentifier().NodeNameSpace() );
            CDesC16ArrayFlat* elements = new (ELeave) CDesC16ArrayFlat(1);
            CleanupStack::PushL( elements );
            
            // Add the parent folder to the request.
            req->AddEntityL( folder.NodeLinkL().MetaDataIdentifier().NodeId(),
                             folder.NodeLinkL().Timestamp(),
                             EFalse ); // No metadata for parent
            
            switch ( iChildLoadMode )
                {
                case ELoadStructure:
                    {
                    DLTRACE(("ELoadStructure"));
                    // Calculate correct pagesize.
                    TInt pageSize = CalculateStructPageSize(aFilterParams.iPageStart,
                        aFilterParams.iPageSize,
                        folder,
                        iChildLoadMode );
                    // Add response filter to get only the desired amount of children.
                    req->AddResponseFilterL(
                        pageSize, // pageSize
                        aFilterParams.iPageStart, // pageStart
                        1, // structureDepth: load child structure
                        0, // metaDataDepth:  don't load child metadata
                        0, // metaDataPerLevel: don't load child metadata
                        *elements,
                        *elements );
                    DLTRACE(("ELoadStructure done"));
                    break;
                    }
                case ELoadMetadata:
                    {
                    DLTRACE(("ELoadMetadata"));
                    CNcdNodeFolder& folder = iNodeManager->FolderL( *aNodeIdentifier );
                    // Special handling for bundle folders.
                    if ( folder.ClassId() == NcdNodeClassIds::ENcdBundleFolderNodeClassId )
                        {
                        DLTRACE(("Bundle folder -> load children in sub ops."));
                        for ( TInt i = aFilterParams.iPageStart ;
                             i < aFilterParams.iPageStart + aFilterParams.iPageSize ;
                             i++ )
                            {
                            CNcdNode& childNode = iNodeManager->NodeL( folder.ChildByServerIndexL( i ) );
                            
                            if ( !childNode.NodeMetaData() || childNode.NodeLinkL().IsExpired() )
                                {
                                // load node only if it has no metadata or if it's expired
                            
                                CNcdNodeIdentifier* remoteFolderId =
                                    CNcdNodeIdentifier::NewLC( childNode.Identifier() );
                                // Remote folderlist contains actual node ids.
                                iRemoteFolders.AppendL( remoteFolderId );
                                CleanupStack::Pop( remoteFolderId );
                                }
                            }
                        if( iRemoteFolders.Count() > 0 )
                            {
                            DLTRACE(("Only remote folders to load"));
                            User::Leave( KNcdLoadNodeErrRemoteOnly );
                            }
                        else
                            {
                            DLTRACE(("Nothing to do -> complete operation"));
                            User::Leave( KNcdLoadNodeErrNothingToDo );
                            }
                        }
                    else
                        {
                        DLTRACE(("Normal folder"));
                        // Calculate correct pagesize.
                        TInt pageSize = CalculateStructPageSize(aFilterParams.iPageStart,
                            aFilterParams.iPageSize,
                            folder,
                            iChildLoadMode );
                        // Add response filter to get only the desired amount of children.                      
                        req->AddResponseFilterL(
                            pageSize, // pageSize: 
                            aFilterParams.iPageStart, // pageStart: not applicable in this case as pageSize is 0
                            1, // structureDepth: load child structure
                            1, // metaDataDepth: load child metadata
                            aFilterParams.iPageSize,// metaDataPerLevel: load metadata only for the requested page
                            *elements,
                            *elements );
                        }
                    DLTRACE(("ELoadMetadata done"));
                    break;
                    }
                default:
                    {
                    // For debugging purposes
                    DLERROR(("Unidentified case"));
                    DASSERT( EFalse );
                    break;                        
                    }
                }
            CleanupStack::PopAndDestroy( elements );
            DLTRACE(("EChildren done"));
            break;
            }
        default:
            {
            // For debugging purposes
            DLERROR(("Unidentified case"));
            DASSERT( EFalse );
            break;                        
            }
        }
    
    HBufC8* data = NULL;
    
    AddQueryResponsesL( req );
    
    data = iProtocol.ProcessPreminetRequestL(
        iSession.Context(), *req, aUri );
    
    CleanupStack::PopAndDestroy( req );
    CleanupStack::PushL( data );
    return data;
    }

void CNcdLoadNodeOperationImpl::MapIconIdForDataBlockL(
    const TDesC& aIconId, const TDesC& aDataBlockId,
    const CNcdNodeIdentifier& aNodeId ) 
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    // Hold node updates until the datablocks have been received
    iHoldNodeUpdates = ETrue;
    
    // Notice that actually here we use the metadata ids and not
    // the node ids.
    CNcdNodeIconMap* newMap = CNcdNodeIconMap::NewLC( aNodeId, aIconId, aDataBlockId );
    iNodeIconMaps.AppendL( newMap );
    CleanupStack::Pop( newMap );    
    }
    
RPointerArray<CNcdLoadNodeOperationImpl::CNcdNodeIconMap> CNcdLoadNodeOperationImpl::IconsForDataBlockL(
    const TDesC& aDataBlockId ) 
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    RPointerArray<CNcdNodeIconMap> maps;
    CleanupClosePushL( maps );
    
    TInt i = iNodeIconMaps.Count() - 1;
    while ( i > -1 )
        {
        if ( *iNodeIconMaps[i]->iDataBlockId == aDataBlockId ) 
            {
            maps.AppendL( iNodeIconMaps[i] );
            iNodeIconMaps.Remove( i );
            }
        
        i--;
        }
        
    // zero the flag if no more mapped icons exist    
    iHoldNodeUpdates = iNodeIconMaps.Count();        
        
    CleanupStack::Pop( &maps );
    return maps;
    }
    
void CNcdLoadNodeOperationImpl::CreateSubOperationsL()
    {
    DLTRACEIN((("Remote folder count: %d"), iRemoteFolders.Count() ));
    DLINFO(( "this-ptr: %X", this ));
    for ( TInt i = 0 ; i < iRemoteFolders.Count() ; i++ )
        {
        // Note that the remote folder contain the actual node ids.
        CNcdNode& remoteFolder = iNodeManager->NodeL( *iRemoteFolders[i] );        
        CNcdNode& parentNode = iNodeManager->NodeL( remoteFolder.NodeLinkL().ParentIdentifier() );
        CNcdLoadNodeOperationImpl* loadOp =
            CNcdLoadNodeOperationImpl::NewLC(
                *iRemoteFolders[i],
                remoteFolder.NodeLinkL().ParentIdentifier(),
                CNcdNodeFactory::NodePurposeL( parentNode ),
                TNcdResponseFilterParams(),
                iGeneralManager,
                iHttpSession,
                iRemoveHandler,
                iOperationQueue,
                iSession,
                EFalse,
                ELoadStructure,
                ETrue );
        
        loadOp->AddObserverL( this );
        iSubOps.AppendL( loadOp );
        CleanupStack::Pop( loadOp );
        // error code ignored, errors handled via callback
        loadOp->Start();
        }
    }
    
TBool CNcdLoadNodeOperationImpl::IsLoadingNecessaryL()
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    if( iLoadMode == EContentSource )
        {
        DLTRACE(("Content source -> always load"))
        return ETrue;
        }
    DASSERT( iNodeIdentifier );
    CNcdNode& node = iNodeManager->NodeL( *iNodeIdentifier );
    
    // If metadata is loaded for children, it can also be done for the
    // root children. This has been also handled in CreateRequestL.
    // Root loading should only be allowed for root children.
    if ( CNcdNodeFactory::NodeTypeL( node ) == CNcdNodeFactory::ENcdNodeRoot ) 
        {
        DLTRACE(("Root or search root-> never load"))
        return EFalse;
        }
        
    DLTRACE(("load"));
    return ETrue;
    }
    
    
TBool CNcdLoadNodeOperationImpl::IsChildClearingNecessaryL()
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    return iLoadMode == ESingleNode && 
        CNcdNodeFactory::NodeTypeL(
            iNodeManager->NodeL( *iNodeIdentifier ) ) == 
        CNcdNodeFactory::ENcdNodeFolder;
    }

// ---------------------------------------------------------------------------
// From class CNcdBaseOperation.
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CNcdLoadNodeOperationImpl::RunOperation()
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    
    TInt err( KErrNone );
    TRAP( err, DoRunOperationL() );
    
    if ( err != KErrNone )
        {
        DLTRACE(("error: %d", err));
        Cancel();
        iLoadNodeState = EFailed;
        iError = err;
        if ( iPendingMessage )
            {
            // error ignored because operation already failed
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
// 
// 
// ---------------------------------------------------------------------------
//
void CNcdLoadNodeOperationImpl::DoRunOperationL()
    {
    DLTRACEIN(("this: %x", this ));
    TInt err = KErrNone;
    switch ( iLoadNodeState )
        {
        case ESendRequest:
            {
            DLTRACE((_L("->ESendRequest")));
            
            // check that is loading really necessary
            if ( !IsLoadingNecessaryL() )
                {
                // complete operation
                iLoadNodeState = EComplete;
                RunOperation();
                break;
                }
            
            switch ( iLoadMode )
                {
                case EContentSource:
                    {
                    // loading from a content source, get uri from it
                    DASSERT( iContentSource );
                    AssignDesL( iServerUri, iContentSource->Uri() );
                    break;
                    }
                case ESingleNode:
                case EChildren:
                    {
                    // loading from a node, get uri from it
                    DASSERT( iNodeIdentifier );
                    // The member variable contains the actual node identifier.
                    // So, it can be used directly. No need for the parent info.
                    CNcdNode& node = iNodeManager->NodeL( *iNodeIdentifier );                    
                    if ( node.NodeLinkL().RemoteUri() != KNullDesC() )
                        {
                        DLTRACE((_L("Node has remote uri: %S, use it"),
                            &node.NodeLinkL().RemoteUri() ));
                        AssignDesL( iServerUri, node.NodeLinkL().RemoteUri() );
                        }
                    else
                        {
                        AssignDesL( iServerUri, node.NodeLinkL().ServerUri() );
                        }            
                    break;
                    }
                }
            
            
            if ( iNodeIdentifier &&
                 CNcdNodeFactory::NodeTypeL(
                    iNodeManager->NodeL( *iNodeIdentifier ) ) == 
                        CNcdNodeFactory::ENcdNodeFolder )
                {
                CNcdNodeFolder& folder = iNodeManager->FolderL( *iNodeIdentifier );
                // Store previous list only if some children have been previously loaded.
                if( folder.ChildrenPreviouslyLoaded() )
                    {
                    folder.StoreChildrenToPreviousListL();
                    }
                if( IsChildClearingNecessaryL() )
                    {                
                    DLTRACE(("clear children"));
                    RemoveChildrenL( folder );
                    }
                }
            
            DLINFO(( _L("URI: %S"), iServerUri ));
            
            // create a browse request
            HBufC8* request = NULL;
            TRAPD( reqErr,
                {
                request = CreateRequestLC( iNodeIdentifier,
                    iFilterParams, *iServerUri );
                CleanupStack::Pop( request );
                });
            if( reqErr != KErrNone )
                {
                if( reqErr == KNcdLoadNodeErrRemoteOnly )
                    {
                    iLoadNodeState = ERemote;
                    RunOperation();
                    break;
                    }
                else if ( reqErr == KNcdLoadNodeErrNothingToDo )
                    {
                    iLoadNodeState = EComplete;
                    RunOperation();
                    break;
                    }
                else
                    {
                    User::Leave( reqErr );
                    }
                }
            CleanupStack::PushL( request );
            
            
            DLINFO(( "request= %S", request ));
            
            // create transaction
            if ( iLoadMode == EContentSource ) 
                {
                DASSERT( iContentSource );
                if ( iContentSource->NodeId() == KNullDesC ) 
                    {
                    // Note that iTransaction is released & set as null
                    // before new transaction is created. If iTransaction is
                    // not null, then it obviously is not released
                    // Correct accesspoint is set if found, otherwise default
                    // is used
                    iGeneralManager.HttpUtils().CreateTransactionL(
                        iHttpSession,
                        iTransaction,
                        *iServerUri,
                        *this,
                        *request,
                        iContentSource->NameSpace(), 
                        MCatalogsAccessPointManager::EBrowse, 
                        iClientUid );

                    }
                else
                    {
                    // iNodeIdentifier may be NULL here. So, we have to use the information
                    // from the content source. Because the accesspoint requires the node id
                    // we can use the content source that contains the actual node id.
                    CNcdNodeIdentifier* contentIdentifier =
                        CNcdNodeIdentifier::NewLC( iContentSource->NameSpace(), 
                                                   iContentSource->NodeId(), 
                                                   iContentSource->Uri(), 
                                                   iClientUid );

                    
                    iGeneralManager.HttpUtils().CreateTransactionL(
                        iHttpSession,
                        iTransaction,
                        *iServerUri,
                        *this,
                        *request,
                        contentIdentifier->NodeNameSpace(),
                        contentIdentifier->NodeId(),
                        MCatalogsAccessPointManager::EBrowse, 
                        iClientUid );
                        
                    CleanupStack::PopAndDestroy( contentIdentifier );                    
                    }
                }
            else
                {
                DASSERT( iNodeIdentifier )
                DLTRACE((_L("Finding AP: namespace=%S, clientUid=%d"), &iNodeIdentifier->NodeNameSpace(), iClientUid.iUid));
                
                CNcdNodeIdentifier* originIdentifier = NULL;
                if ( NcdNodeIdentifierEditor::IdentifiesTemporaryNodeL( *iNodeIdentifier ) )
                    {
                    // Temporary nodes can be created for already purchased nodes when cache is empty,
                    // therefore we should try to get their origin identifier from purchase history for ap searching.
                    DLTRACE(("Temporary node, try to get origin identifier for it"));
                    originIdentifier =
                        iNodeManager->GetOriginIdentifierL( *iNodeIdentifier );
                    }
                
                if ( originIdentifier ) 
                    {
                    CleanupStack::PushL( originIdentifier );
                    
                    // First try originIdentifier, if it fails, try iNodeIdentifier
                    DLTRACE(("Origin identifier found, trying to get ap with it."));
                    iGeneralManager.HttpUtils().CreateTransactionL(
                        iHttpSession,
                        iTransaction,
                        *iServerUri,
                        *this,
                        *request,
                        *originIdentifier,
                        *iNodeIdentifier,
                        MCatalogsAccessPointManager::EBrowse, 
                        iClientUid );
                        
                    CleanupStack::PopAndDestroy( originIdentifier ); 
                    originIdentifier = NULL;                   
                    }
                else
                    {
                    DLTRACE(("No origin id, just try iNodeIdentifier"));
                    iGeneralManager.HttpUtils().CreateTransactionL(
                        iHttpSession,
                        iTransaction,
                        *iServerUri,
                        *this,
                        *request,
                        *iNodeIdentifier,
                        MCatalogsAccessPointManager::EBrowse, 
                        iClientUid );
                    }
                }
            CleanupStack::PopAndDestroy( request );    

            // create parser  
            if( !iParser )
                {
                iParser = iProtocol.CreateParserL( 
                iSession.Context(), *iServerUri );
                }
            
            // set observers
            MNcdParserObserverBundle& observers = iParser->Observers();
            observers.SetParserObserver( this );
            observers.SetEntityObserver( this );
            observers.SetInformationObserver( this );
            observers.SetDataBlocksObserver( this );
            observers.SetErrorObserver( this );
            observers.SetQueryObserver( this );
            iParser->BeginAsyncL();  
            
            iTransparentChildFolders.ResetAndDestroy();
            // start transaction
            User::LeaveIfError( iTransaction->Start() );
            
            iLoadNodeState = EReceive;
            DLTRACE((_L("->ESendRequest done")));
            break;
            }
            
        case EReceive:
            {
            DLTRACE(("->EReceive, iPendingMessage: %x, nodes: %d", 
                 iPendingMessage,
                 iLoadedNodes.Count() ));
            if ( iPendingMessage && 
                 !iHoldNodeUpdates && 
                 iLoadedNodes.Count() > 0 )
                {
                // send updated nodes identifiers to proxy
                err = CompleteMessage( iPendingMessage,
                    ENCDOperationMessageCompletionNodesUpdated,
                    iProgress,
                    iLoadedNodes,
                    KErrNone );
                User::LeaveIfError( err );
                iLoadedNodes.ResetAndDestroy();
                }
            DLTRACE((_L("->EReceive done")));
            break;
            }
        
        
        case ERemote:
            {
            DLTRACE((_L("->ERemote")));
            
            // create load node op for each remote folder
            iSubOps.ResetAndDestroy();

            // create sub ops for remote folders
            CreateSubOperationsL();
            
            iLoadNodeState = EReceiveRemote;
            
            if ( iFailedSubOps.Count() + iCompletedSubOps.Count() ==
                 iSubOps.Count() )
                {
                // all sub ops have either completed or failed
                // -> go to next state
                iLoadNodeState = EComplete;
                if ( iFailedSubOps.Count() > 0 )
                    {
                    iError = KNcdErrorSomeCatalogsFailedToLoad;
                    }
                RunOperation();
                }
                
            DLTRACE((_L("->ERemote done")));
            break;
            }
        
         case EReceiveRemote:
            {
            DLTRACE((_L("->EReceiveRemote")));
            if( iSubOpQuerys.Count() > 0 )
                {
                // send sub op query to proxy
                CNcdBaseOperation::QueryReceivedL( iSubOpQuerys[0] );
                // release own reference and remove
                iSubOpQuerys[0]->InternalRelease();
                iSubOpQuerys.Remove( 0 );
                }
            else if ( iPendingMessage && 
                      !iHoldNodeUpdates &&
                      iLoadedNodes.Count() > 0 )
                {
                // send updated nodes's identifiers to proxy
                err = CompleteMessage( iPendingMessage,
                    ENCDOperationMessageCompletionNodesUpdated,
                    iProgress,
                    iLoadedNodes,
                    KErrNone );
                User::LeaveIfError( err );
                iLoadedNodes.ResetAndDestroy();
                }
            else if ( iFailedSubOps.Count() + iCompletedSubOps.Count() ==
                 iSubOps.Count() )
                {
                DLTRACE(("Sub ops complete"));
                // all sub ops have either completed or failed
                // -> go to next state
                if ( iFailedSubOps.Count() > 0 ) 
                    {
                    iError = KNcdErrorSomeCatalogsFailedToLoad;
                    }
                iLoadNodeState = EComplete;
                // Continue operation asynchronously, to prevent problems when
                // potentially deleting sub ops in next state
                ContinueOperationL();
                }
                
            DLTRACE((_L("->EReceiveRemote done")));
            break;
            }
            
        
        case EComplete:
            {
            DLTRACE((_L("->EComplete")));
                        
            // Set the children loaded flag so that next refresh stores
            // previous child list (needed for new checking)
            SetChildrenLoadedFlagL();
            
            RefreshSeenStatusL();
            
            if ( IsSubOperation() )
                {
                if( iError == KErrNone )
                    {
                    // call observers 
                    CompleteCallback();
                    }
                else
                    {
                    DLINFO(("Error has occurred, fail operation"));
                    iLoadNodeState = EFailed;
                    RunOperation();
                    break;
                    }
                }
            else
                {
                DLTRACE(("iPendingMessage: %x, loaded nodes: %d", 
                    iPendingMessage, iLoadedNodes.Count() ));
                
                // NOTE: The operation will not complete until it gets a message from proxy
                if ( iPendingMessage && iLoadedNodes.Count() > 0 )
                    {
                    // Send remaining loaded node info to proxy before completing op.
                    DLTRACE(("Sending remaining loaded node info to proxy."));
                    TInt err = CompleteMessage( iPendingMessage,
                        ENCDOperationMessageCompletionNodesUpdated,
                        iProgress,
                        iLoadedNodes,
                        KErrNone );
                    User::LeaveIfError( err );
                    iLoadedNodes.ResetAndDestroy();
                    iHoldNodeUpdates = EFalse;
                    }
                else if ( iPendingMessage )
                    {
                    DLTRACE(("No loaded node info left, complete op."));
                    if( iError == KErrNone )
                        {
                        // Send complete message to proxy.                
                        err = CompleteMessage( iPendingMessage,
                            ENCDOperationMessageCompletionComplete,
                            iProgress,
                            KErrNone );
                        User::LeaveIfError( err );
                        iOperationState = EStateComplete;
                        // call observers 
                        CompleteCallback();
                        }
                    else
                        {
                        DLINFO(("Error has occurred, fail operation"));
                        iLoadNodeState = EFailed;
                        RunOperation();
                        break;
                        }
                    }
                }
                            
            DLTRACE((_L("->EComplete done")));
            break;
            }
            
        case EFailed:
            {
            DLTRACE((_L("->EFailed")));
            // Operation failed, send error message
            Cancel();
            if ( iPendingMessage )
                {
                // error ignored because operation has already failed
                CompleteMessage( iPendingMessage,
                    ENCDOperationMessageCompletionError, iError );
                }
            // call observers
            CompleteCallback();
            DLTRACE((_L("->EFailed done")));
            break;            
            }
        default:
            {
            DLERROR(("default case, should never come here!"));
            DASSERT(0);
            User::Leave( KErrArgument );
            break;
            }
        }
    DLTRACEOUT((""));        
    }
    

void CNcdLoadNodeOperationImpl::ChangeToPreviousStateL()
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    switch ( iLoadNodeState )
        {
        case EReceive:
            {
            // can only go back from this state
            iLoadNodeState = ESendRequest;
            break;
            }
        default:
            {
            DLTRACE(("CAN'T GO BACK FROM THIS STATE, ERROR!"));
            DASSERT(0);
            User::Leave( KErrArgument );
            }
        }
    }

TBool CNcdLoadNodeOperationImpl::QueryCompletedL( CNcdQuery* aQuery )
    {    
    DLTRACEIN(("aQuery=%08x", aQuery));
    DLINFO(( "this-ptr: %X", this ));

    // handle child ops' querys
    for( TInt i = 0 ; i < iSubOps.Count() ; i++ )
        {
        CNcdQuery* query = iSubOps[i]->ActiveQuery();
        if ( aQuery == query )
            {
            // send to subop
            iSubOps[i]->QueryHandledL( aQuery );
            TInt index = iSubOpQuerys.Find( aQuery );
            if ( index != KErrNotFound )
                {
                // remove own reference
                iSubOpQuerys[index]->InternalRelease();
                iSubOpQuerys.Remove( index );
                }
            query->InternalRelease();
            query = NULL;
            return ETrue;
            }
        else if ( query )
            {
            query->InternalRelease();
            }
        }
    
    // own query
    return EFalse;
    }
    
void CNcdLoadNodeOperationImpl::ErrorL( MNcdPreminetProtocolError* aData )
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    CleanupDeletePushL( aData );
    switch ( aData->Code() )
        {
        case 404:
            {
            // requested node not found on server
            if ( iNodeIdentifier && aData->Id() != KNullDesC ) 
                {
                CNcdNode& node = iNodeManager->NodeL( *iNodeIdentifier );
                CNcdNodeLink& nodeLink( node.NodeLinkL() );
                    
                // Checking for empty parent identifier also because of scheme nodes                    
                if( nodeLink.MetaDataIdentifier().NodeId() == aData->Id() &&
                    ( nodeLink.ParentIdentifier().ContainsEmptyFields() ||
                      iNodeManager->NodeL( nodeLink.ParentIdentifier() )
                        .ClassId() == NcdNodeClassIds::ENcdRootNodeClassId ) )
                    {
                    // parent node not found
                    // grandparent is root node -> remove parent
                    iNodeManager->RemoveNodeL( *iNodeIdentifier );
                    }
                else
                    {
                    RPointerArray<CNcdExpiredNode> expiredNodes;
                    CleanupResetAndDestroyPushL( expiredNodes );
                    if( node.NodeLinkL().MetaDataIdentifier().NodeId() == aData->Id() )
                        {
                        // parent node not found
                        // expire grandparent
                        CNcdNode& parent = iNodeManager->NodeL( node.NodeLinkL().ParentIdentifier() );
                        iNodeManager->SetNodeExpiredL( parent, EFalse, EFalse, expiredNodes );
                        }
                    else
                        {                    
                        // child node not found  
                        // expire parent
                        iNodeManager->SetNodeExpiredL( node, EFalse, EFalse, expiredNodes );
                        }
                    ExpirationInfoReceived( this, expiredNodes );
                    CleanupStack::PopAndDestroy( &expiredNodes );
                    }
                iError = KNcdErrorNodeWasRemoved;
                }
            else 
                {
                // something else was not found
                iError = KNcdProtocolErrorBase - aData->Code();
                }
            iLoadNodeState = EFailed;
            CleanupStack::Pop( aData );
            // Default observer deletes aData
            iParser->DefaultObserver().ErrorL( aData );            
            break;
            }
            
        case 416:
            {
            DLTRACE(("session expired"));
            Cancel();
            RemoveServerSessionL();
            DLINFO(("Start operation from initial state"))
            iLoadNodeState = ESendRequest;
            CleanupStack::Pop( aData );
            // Default observer deletes aData
            iParser->DefaultObserver().ErrorL( aData );
            break;
            }
            
        case 401:
            {
            //special case check if we have rejected an auth. query earlier
            for ( TInt i = 0 ; i < iCompletedQuerys.Count() ; i++ )
                {
                if( iCompletedQuerys[i]->Semantics() == MNcdQuery::ESemanticsAuthenticationQuery &&
                    iCompletedQuerys[i]->Response() == MNcdQuery::ERejected )
                    {
                    DLTRACE(("Auth. query was rejected before -> remove session so that we get a new query when this node is requested again"));
                    RemoveServerSessionL();
                    break;
                    }
                }
            // fall trough to default case
            }
            
        default:
            {
            iError = KNcdProtocolErrorBase - aData->Code();
            iLoadNodeState = EFailed;
            CleanupStack::Pop( aData );
            // Default observer deletes aData
            iParser->DefaultObserver().ErrorL( aData );    
            break;
            }
        }    
    
    // continue operation asynchronously to prevent problems with parser
    ContinueOperationL();
    }

void CNcdLoadNodeOperationImpl::DetermineParentTypeL( const TUid& aUid )
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    CNcdNodeIdentifier* root = 
        NcdNodeIdentifierEditor::CreateRootIdentifierForClientLC( aUid );
    DASSERT( iParentIdentifier );
    if ( root->Equals( *iParentIdentifier ) ) 
        {
        DLTRACE(("Parent is root"));
        iParentType = CNcdNodeFactory::ENcdNodeRoot;
        }
    CleanupStack::PopAndDestroy( root );
    }


//////////////////////////////////////////////////////////////////////////////////////////////
// CNcdLoadNodeOperationImpl::CNcdNodeIconMap
//////////////////////////////////////////////////////////////////////////////////////////////

CNcdLoadNodeOperationImpl::CNcdNodeIconMap* CNcdLoadNodeOperationImpl::CNcdNodeIconMap::NewLC(
    const CNcdNodeIdentifier& aMetadataId,
    const TDesC& aIconId,
    const TDesC& aDataBlockId ) 
    {
    CNcdNodeIconMap* self = new ( ELeave ) CNcdNodeIconMap;
    CleanupStack::PushL( self );
    self->ConstructL( aMetadataId, aIconId, aDataBlockId );
    return self;
    }
    
void CNcdLoadNodeOperationImpl::CNcdNodeIconMap::ConstructL(
    const CNcdNodeIdentifier& aMetadataId,
    const TDesC& aIconId,
    const TDesC& aDataBlockId )
    {
    iDataBlockId = aDataBlockId.AllocL();
    iIconId = aIconId.AllocL();
    iMetadataId = CNcdNodeIdentifier::NewL( aMetadataId );
    }
    
CNcdLoadNodeOperationImpl::CNcdNodeIconMap::CNcdNodeIconMap() 
    {
    }
    
CNcdLoadNodeOperationImpl::CNcdNodeIconMap::~CNcdNodeIconMap() 
    {
    delete iDataBlockId;
    delete iIconId;
    delete iMetadataId;
    }
    
void CNcdLoadNodeOperationImpl::RemoveServerSessionL()
    {
    DLTRACEIN((""));
    if( iLoadMode == EContentSource )
        {
        DASSERT( iContentSource );
        iProtocol.SessionHandlerL( iSession.Context() )
            .RemoveSession( *iServerUri, iContentSource->NameSpace() );
        }
    else
        {
        DASSERT( iNodeIdentifier );
        iProtocol.SessionHandlerL( iSession.Context() )
            .RemoveSession( *iServerUri, iNodeIdentifier->NodeNameSpace() );
        }
    }

void CNcdLoadNodeOperationImpl::SetChildrenLoadedFlagL()
    {
    DLTRACEIN((""));
    // Set children loaded flag.
    if( iLoadMode == EChildren )
        {
        CNcdNodeFolder& folder = iNodeManager->FolderL( *iNodeIdentifier );
        folder.SetChildrenPreviouslyLoaded();
        iNodeManager->DbSaveNodeL( folder );
        }
    else if( iLoadMode == EContentSource
        && iContentSource->IsTransparent() )
        {
        RPointerArray<CNcdNodeIdentifier> nodes =
            iContentSourceMap->NodesL( *iContentSource );
        for( TInt i = 0 ; i < nodes.Count() ; i++ )
            {
            CNcdNodeFolder& folder = iNodeManager->FolderL( *nodes[i] );
            folder.SetChildrenPreviouslyLoaded();
            iNodeManager->DbSaveNodeL( folder );
            }
        }
    }
    
void CNcdLoadNodeOperationImpl::RefreshSeenStatusL()
    {
    DLTRACEIN((""));
    if( iLoadMode == EChildren )
        {
        iNodeManager->SeenInfo().RefreshFolderSeenStatusL(  *iNodeIdentifier );
        }
    else if( iLoadMode == EContentSource
        && iContentSource->IsTransparent() )
        {
        RPointerArray<CNcdNodeIdentifier> nodes =
            iContentSourceMap->NodesL( *iContentSource );
        for( TInt i = 0 ; i < nodes.Count() ; i++ )
            {
            iNodeManager->SeenInfo().RefreshFolderSeenStatusL( *nodes[i] );
            }
        }
    }

TInt CNcdLoadNodeOperationImpl::CalculateStructPageSize(
    TInt aPageStart,
    TInt aPageSize,
    CNcdNodeFolder& aNodeFolder,
    TNcdChildLoadMode aChildLoadMode )
    {
    DLTRACEIN((""));
    if( aChildLoadMode == ELoadMetadata )
        {
        DLTRACE(("ELoadMetadata"));
        // Check that how much struct is already loaded.
        TInt count = aNodeFolder.ChildArray().Count();
        if( count > 0 )
            {
            TInt lastLoadedChildIndex = aNodeFolder.ChildArray()[count-1]->Index();
            if( lastLoadedChildIndex >= aPageStart + aPageSize - 1 )
                {
                DLTRACE(("Structure already loaded for this page -> use original page size"));
                return aPageSize;
                }
            }
        }
    DLTRACE((""));
    return KNcdStructPageSize > aPageSize
            ? KNcdStructPageSize : aPageSize; 
    }

TInt CNcdLoadNodeOperationImpl::RemoteFolderCount() const
    {
    DLTRACEIN((""));
    return iRemoteFolders.Count();
    }


void CNcdLoadNodeOperationImpl::RemoveChildrenL( CNcdNodeFolder& aFolder ) 
    {
    DLTRACEIN((""));
    aFolder.ExpireAndRemoveChildrenL();
    }
    

void CNcdLoadNodeOperationImpl::NotifyCompletionOfQueuedOperation(
    TNcdOperationMessageCompletionId aId )
    {
    DLTRACEIN((""));
    if ( IsSubOperation() )
        {
        return;
        }
        
    DASSERT( iOperationQueue );        
    if ( aId == ENCDOperationMessageCompletionComplete ||
         aId == ENCDOperationMessageCompletionError )
        {
        iOperationQueue->QueuedOperationComplete( *this );
        }
    }
    
