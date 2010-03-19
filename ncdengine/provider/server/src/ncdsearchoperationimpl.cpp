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

#include "ncdsearchoperationimpl.h"
#include "ncdoperationfunctionids.h"
#include "catalogsbasemessage.h"
#include "catalogshttpsession.h"
#include "catalogshttpoperation.h"
#include "catalogshttpconfig.h"
#include "catalogsdebug.h"
#include "catalogsbigdes.h"
#include "ncdrequestgenerator.h"
#include "catalogsaccesspointmanager.h"

#include "ncdrequestbase.h"
#include "ncdrequestbrowsesearch.h"
#include "ncdrequestconfiguration.h"
#include "ncd_pp_itemref.h"
#include "ncd_pp_folderref.h"
#include "ncd_pp_dataentity.h"
#include "ncd_pp_error.h"
// remove these
#include "ncd_pp_itemrefimpl.h"
#include "ncd_pp_folderrefimpl.h"
#include "ncd_pp_dataentityimpl.h"
#include "ncd_pp_datablock.h"
//
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
#include "ncdstoragedescriptordataitem.h"
#include "ncdnodeiconimpl.h"
#include "ncdnodelink.h"
#include "catalogsconstants.h"
#include "ncdnodeclassids.h"
#include "ncdnodecontentinfoimpl.h"
#include "ncditempurpose.h"
#include "ncdutils.h"
#include "ncdsearchnodefolder.h"
#include "ncdnodeclassids.h"
#include "ncdnodefactory.h"
#include "ncdnodeidentifiereditor.h"
#include "ncdsearchnodefolder.h"
#include "ncdcapabilities.h"
#include "ncdsearchnodebundle.h"
#include "ncdchildentity.h"


//_LIT8(KRequestBody, "<?xml version=\"1.0\" encoding=\"UTF-8\"?> <preminetRequest xmlns=\"http://nokia.com/preminet/protocol/v/2/0\"  xmlns:cp=\"http://nokia.com/preminet/protocol/configuration/v/1/0\" xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" version=\"2.0\" namespace=\"http://lynx.ionific.com:9086/ntp-cgw/catalogs\"> <configuration> <client> <cp:software version=\"3.2\" type=\"S60\"/> </client> </configuration> <search> <entity xsi:type=\"Folder\" id=\"cgw_normal\" timestamp=\"2007-12-17T09:30:47.0Z\"/> <entityFilter subscribableContent=\"false\"> <keywords all=\"true\" caseSensitive=\"false\"> <keyword exclude=\"false\">a</keyword> </keywords> <reviewScore>1</reviewScore> </entityFilter> <responseFilter structureDepth=\"2\" metadataDepth=\"2\" metadataPerLevel=\"5\" pageStart=\"2\" pageSize=\"2\"/> </search> </preminetRequest>");

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdSearchOperation* CNcdSearchOperation::NewL(    
    const CNcdNodeIdentifier& aNodeIdentifier,
    const CNcdNodeIdentifier& aParentIdentifier,
    const CNcdSearchFilter& aSearchFilter,
    TNcdResponseFilterParams aFilterParams,    
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler* aRemoveHandler,
    MNcdOperationQueue* aOperationQueue,
    MCatalogsSession& aSession,
    TBool aLoadChildren,
    TNcdChildLoadMode aMode,
    TInt aRecursionLevels,
    TBool aIsSubOperation )
    {
    CNcdSearchOperation* self = CNcdSearchOperation::NewLC(
        aNodeIdentifier,
        aParentIdentifier,
        aSearchFilter,
        aFilterParams,        
        aGeneralManager,        
        aHttpSession,        
        aRemoveHandler,
        aOperationQueue,        
        aSession,
        aLoadChildren,
        aMode,
        aRecursionLevels,
        aIsSubOperation );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdSearchOperation* CNcdSearchOperation::NewLC(
    const CNcdNodeIdentifier& aNodeIdentifier,
    const CNcdNodeIdentifier& aParentIdentifier,
    const CNcdSearchFilter& aSearchFilter,
    TNcdResponseFilterParams aFilterParams,
    CNcdGeneralManager& aGeneralManager,    
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler* aRemoveHandler,
    MNcdOperationQueue* aOperationQueue,
    MCatalogsSession& aSession,
    TBool aLoadChildren,
    TNcdChildLoadMode aMode,
    TInt aRecursionLevels,
    TBool aIsSubOperation )
    {
    CNcdSearchOperation* self =
        new( ELeave ) CNcdSearchOperation( aFilterParams, aMode,
            aLoadChildren, aGeneralManager, aHttpSession,
            aRemoveHandler, aOperationQueue, aSession, aRecursionLevels,
            aIsSubOperation );
    CleanupClosePushL( *self );
    self->ConstructL( aNodeIdentifier, aParentIdentifier, aSearchFilter );
    return self;
    }
    
// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdSearchOperation::~CNcdSearchOperation()
    {    
    DLTRACEIN((""));
    delete iSearchFilter;
    iRemoteFoldersChildOfTransparent.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdSearchOperation::FolderRefL(
    MNcdPreminetProtocolFolderRef* aData )
    {
    DLTRACEIN(("%X",aData));

    if ( aData == NULL )
        {
        return;
        }
    TRAPD( err, DoFolderRefL( aData ) );

    if( err != KErrNone )
        {
        Cancel();
        iError = err;
        iLoadNodeState = EFailed;
        }
    RunOperation();
    
    }

void CNcdSearchOperation::DoFolderRefL( MNcdPreminetProtocolFolderRef* aData )    
    {
    DLTRACEIN((""));        

    // Normal PushL causes USER 42
    CleanupDeletePushL( aData );
    
    DLTRACE(( _L("folder id=%S, ns=%S"), 
        &aData->Id(), &aData->Namespace() ));
    DLTRACE(( _L("Parent id=%S, ns=%S"), 
        &aData->ParentId(), &aData->ParentNamespace() ));

#ifdef CATALOGS_BUILD_CONFIG_DEBUG    
    if ( iParentIdentifier ) 
        {
        DLNODEID( (*iParentIdentifier) );
        }
#endif
    
    TBool nodeAdded = EFalse;
    
    // This will contain pointer to the iNodeIdentifier or to the iParentIdentifier
    // So, we will know in the end which one is the parent of the current data item.
    // Do not delete this in the end because this just points to the member
    // variables.
    CNcdNodeIdentifier* tmpSearchParentIdentifier( iParentIdentifier );
    CNcdNodeIdentifier* metaIdentifier =
                    NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( *iNodeIdentifier );
    
    switch ( iLoadMode )
        {
        case EContentSource:
            {
            DLTRACE(("Should never come here, ERROR"));
            DASSERT(0);
            User::Leave( KErrGeneral );
            break;
            }
        case ESingleNode:
            {
            DASSERT( iNodeIdentifier );
            DLINFO(( _L("iNodeIdentifier: ns= %S, id= %S, aData: ns= %S, id= %S"),
                &iNodeIdentifier->NodeNameSpace(), &iNodeIdentifier->NodeId(),
                &aData->Namespace(), &aData->Id() ));
            // Because aData contains metadata ids, we have to get
            // the metadata id from the iNodeIdentifier
            
            if ( aData->Id() == metaIdentifier->NodeId() && 
                aData->Namespace() == metaIdentifier->NodeNameSpace() )
                {
                DLTRACE(("ESingleNode"));
                // Because the data id is same as the meta id, we know
                // that this can not be the root search. Root search namespace
                // and id are defined locally here in the client, not in the 
                // web server.
                
                CNcdNodeManager::TNcdRefHandleMode mode;
                if( CNcdNodeFactory::NodePurposeL(
                    iNodeManager->NodeL( *iNodeIdentifier ) ) ==
                        CNcdNodeFactory::ENcdSearchNode )
                    {
                    mode = CNcdNodeManager::EUpdate;
                    }
                else
                    {
                    mode =  CNcdNodeManager::EInsert;
                    }
                
                iNodeManager->RefHandlerL( *iParentIdentifier,
                    *aData,
                    iClientUid,
                    mode,
                    0,
                    iParentType,
                    CNcdNodeFactory::ENcdSearchNode,
                    CNcdNodeFactory::ENcdSearchNode );
                nodeAdded = ETrue;
                }
            else if( aData->ParentId() == metaIdentifier->NodeId() &&
                aData->ParentNamespace() == metaIdentifier->NodeNameSpace() )
                {
                DLINFO(("ESingleNode, adding children"));
                // A child of the node that we are loading is received
                
                                    
                CNcdNode& node = 
                    iNodeManager->RefHandlerL( *iNodeIdentifier,
                                                *aData,
                                                iClientUid,
                                                CNcdNodeManager::EInsert, // use insertion because child count is not known yet
                                                iNodeIndex++,
                                                CNcdNodeFactory::ENcdNodeFolder,
                                                CNcdNodeFactory::ENcdSearchNode,
                                                CNcdNodeFactory::ENcdSearchNode );
                                                
                // Before saving the node information make sure that the node remote info is
                // set correctly. Because nodeAdded will be set true below the node will be 
                // saved in the end of this function.
                if ( aData->RemoteUri() != KNullDesC )
                    {
                    DLINFO((_L("Remote search node: %S"), &node.Identifier().NodeId()));
                    node.NodeLinkL().SetRemoteFlag( ETrue );
                    if ( iNodeManager->SearchFolderL( *iNodeIdentifier ).IsTransparent() )
                        {
                        DLINFO(("Parent is transparent folder, add child to remote folder array"));
                        iRemoteFoldersChildOfTransparent.AppendL( CNcdNodeIdentifier::NewLC( node.Identifier() ) );
                        CleanupStack::Pop(); // the nodeidentifier
                        }
                    else if ( iRecursionLeft ) 
                        {
                        // Sub operations should not go any further in recursion
                        DLINFO(("Recursion left"));
                        iRemoteFolders.AppendL( CNcdNodeIdentifier::NewLC( node.Identifier() ) );
                        CleanupStack::Pop(); // the nodeidentifier
                        }
                     
                    }
                else
                    {
                    DLINFO((_L("Normal search node: %S"), &node.Identifier().NodeId()));
                    node.NodeLinkL().SetRemoteFlag( EFalse );
                    }
                           
                // The parent is not the node identified by the iParentIdentifier
                tmpSearchParentIdentifier = iNodeIdentifier;
                nodeAdded = ETrue;
                }
            break;
            }
            
        case EChildren:
            {
            DLTRACE(("EChildren"));
            DASSERT( iNodeIdentifier );

            // Get the parent node. So, we can use its link to get the metadataidentifier.
            // The parent always has the link information set here.    
            CNcdNode& currentNode = iNodeManager->NodeL( *iNodeIdentifier );
            DLNODEID( currentNode.NodeLinkL().MetaDataIdentifier() );
            if ( aData->Id() == currentNode.NodeLinkL().MetaDataIdentifier().NodeId() && 
                aData->Namespace() == currentNode.NodeLinkL().MetaDataIdentifier().NodeNameSpace() )
                {
                DLINFO(("EChildren Add parent"));
                // Get the actual parent for this operation. This is required to check
                // if the parent is root or some other kind of folder.
                CNcdNode& addParent = iNodeManager->NodeL( *iParentIdentifier );                    
                // add parent
                iNodeManager->RefHandlerL( *iParentIdentifier,
                    *aData,
                    iClientUid,
                    CNcdNodeManager::EUpdate,
                    0, // Should give no index in this case
                    CNcdNodeFactory::NodeTypeL( addParent ),
                    CNcdNodeFactory::ENcdSearchNode,
                    CNcdNodeFactory::ENcdSearchNode );
                nodeAdded = ETrue;
                // Structure loaded for parent -> send update notification
                // for parent node so that it gets internalized.
                CNcdNodeIdentifier* loadedNodeId = 
                    CNcdNodeIdentifier::NewLC( currentNode.Identifier() );
                iLoadedNodes.AppendL( loadedNodeId );
                CleanupStack::Pop( loadedNodeId );
                }
            else if ( aData->ParentId() == metaIdentifier->NodeId() ||
                aData->ParentNamespace() == metaIdentifier->NodeNameSpace() )
                {
                DLINFO(("EChildren"));
                // add child
                // We are loading the children of the current node that is
                // identified by the iNodeIdentifier.
                CNcdNode& node = 
                    iNodeManager->RefHandlerL( *iNodeIdentifier,
                                                *aData,
                                                iClientUid,
                                                CNcdNodeManager::EReplace,
                                                iNodeIndex++,
                                                CNcdNodeFactory::ENcdNodeFolder,
                                                CNcdNodeFactory::ENcdSearchNode,
                                                CNcdNodeFactory::ENcdSearchNode );

                // Before saving the node information make sure that the node remote info is
                // set correctly. Because nodeAdded will be set true below the node will be 
                // saved in the end of this function.
                if ( aData->RemoteUri() != KNullDesC )
                    {
                    DLINFO((_L("Remote search node: %S"), &node.Identifier().NodeId()));
                    node.NodeLinkL().SetRemoteFlag( ETrue );
                    CNcdNodeIdentifier* identifier = CNcdNodeIdentifier::NewLC( node.Identifier() );
                    iRemoteFolders.AppendL( identifier );
                    CleanupStack::Pop( identifier );
                    }
                else
                    {
                    DLINFO((_L("Normal search node: %S"), &node.Identifier().NodeId()));
                    node.NodeLinkL().SetRemoteFlag( EFalse );
                    }
                            
                // The parent is not the node identified by the iParentIdentifier
                tmpSearchParentIdentifier = iNodeIdentifier;
                nodeAdded = ETrue;
                }
            break;
            }
        }
    CleanupStack::PopAndDestroy( metaIdentifier );
            
    // add search filter to folder, this is needed so that consecutive
    // search ops (started while paging results) can get the filter
    if ( nodeAdded )
        {
        CNcdNodeIdentifier* searchMetaIdentifier = 
            CNcdNodeIdentifier::NewLC( aData->Namespace(),
                                       aData->Id(), 
                                       aData->ServerUri(),
                                       iClientUid );
        CNcdSearchNodeFolder& searchFolder = iNodeManager->SearchFolderL(
            *tmpSearchParentIdentifier,
            *searchMetaIdentifier );
        searchFolder.SetSearchFilterL( *iSearchFilter );
        if( iLoadMode == ESingleNode && CNcdNodeFactory::NodePurposeL(
            iNodeManager->NodeL( *iNodeIdentifier ) ) !=
                CNcdNodeFactory::ENcdSearchNode )
            {
            // search op started for a normal node, add origin identifier
            searchFolder.SetOriginIdentifierL( *iNodeIdentifier );
            }
        iNodeManager->DbSaveNodeL( searchFolder );
        CleanupStack::PopAndDestroy( searchMetaIdentifier );
        }
    
    // Delete data because ownership has been transferred.
    CleanupStack::PopAndDestroy( aData );
    
    DLTRACEOUT((""));
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//    
void CNcdSearchOperation::FolderDataL(
    MNcdPreminetProtocolDataEntity* aData )
    {
    DLTRACEIN((""));
    if ( aData != NULL )
        {
        TInt err( KErrNone );
        TRAP( err,
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
                parentIdentifier = CNcdNodeIdentifier::NewLC( *iNodeIdentifier );
                }
            delete metaIdentifier;
            metaIdentifier = NULL;
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
            
            DLINFO(("node loaded, id: %S", &node.Identifier().NodeId() ));

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
        else if ( parentIdentifier )
            {
            CleanupStack::PopAndDestroy( parentIdentifier );
            }

        // Delete data because ownership has been transferred.
        CleanupStack::PopAndDestroy( aData );
        ); //TRAPD
        if( err != KErrNone )
            {
            iError = err;
            iLoadNodeState = EFailed;
            }
        }
    RunOperation();
    
    DLTRACEOUT((""));
    }
    
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdSearchOperation::ItemRefL( MNcdPreminetProtocolItemRef* aData )
    {
    DLTRACEIN((""));

    // Item references are handled only when children are loaded or if the 
    // search mode is not ENormal
    //
    // If all item refs were handled recursive searching would be veeery
    // slow and we might run out of memory due to very high number of 
    // nodes (800 and more)
    if ( ( iSearchFilter->SearchMode() == MNcdSearchFilter::ENormal && 
           iLoadMode != EChildren &&
           !iNodeManager->SearchFolderL( *iNodeIdentifier ).IsTransparent() ) || 
         !aData )
        {
        delete aData;
        DLTRACEOUT(("Not loading children or no data"));
        return;
        }
            
    TRAPD( err, DoItemRefL( aData ) );

    // Delete data because ownership has been transferred.
    delete aData;     
    aData = NULL;    
 
    if( err != KErrNone )
        {
        DLERROR(("Error: %d", err));
        iError = err;
        iLoadNodeState = EFailed;
        }
        
    RunOperation();    
    }    
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdSearchOperation::DoItemRefL( MNcdPreminetProtocolItemRef* aData )
    {
    DLTRACEIN((""));
    // Because aData contains metadata ids, we have to get
    // the metadata id from the iNodeIdentifier
    CNcdNodeIdentifier* metaIdentifier =
        NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( *iNodeIdentifier );
    
    switch ( iLoadMode )
        {
        case EContentSource:
            {                    
            DLTRACE(("Should never come here, ERROR"));
            DASSERT(0);
            break;
            }
            
        case ESingleNode:
            {
            DLINFO(("Search single node"));
            DASSERT( iNodeIdentifier );
            
            if ( aData->Id() == metaIdentifier->NodeId() && 
                aData->Namespace() == metaIdentifier->NodeNameSpace() )
                {
                iNodeManager->RefHandlerL( *iParentIdentifier,
                                            *aData,
                                            iClientUid,
                                            CNcdNodeManager::EUpdate,
                                            0,
                                            iParentType,
                                            CNcdNodeFactory::ENcdSearchNode,
                                            CNcdNodeFactory::ENcdSearchNode );
                }
            else if( aData->ParentId() == metaIdentifier->NodeId() &&
                aData->ParentNamespace() == metaIdentifier->NodeNameSpace() )
                {
                // add child
                iNodeManager->RefHandlerL( *iNodeIdentifier,
                    *aData,
                    iClientUid,
                    CNcdNodeManager::EInsert, // use insertion because child count is not known yet
                    iNodeIndex++,
                    CNcdNodeFactory::ENcdNodeFolder,
                    CNcdNodeFactory::ENcdSearchNode,
                    CNcdNodeFactory::ENcdSearchNode );
                }
            break;
            }
            
        case EChildren:
            {
            DLINFO(("Search children"));
            DASSERT( iNodeIdentifier );

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
                    CNcdNodeFactory::ENcdSearchNode,
                    CNcdNodeFactory::ENcdSearchNode );
                }
            else if ( aData->ParentId() == metaIdentifier->NodeId() &&
                aData->ParentNamespace() == metaIdentifier->NodeNameSpace() )
                {
                // add child
                iNodeManager->RefHandlerL( *iNodeIdentifier,
                    *aData,
                    iClientUid,
                    CNcdNodeManager::EReplace,
                    iNodeIndex++,
                    CNcdNodeFactory::ENcdNodeFolder,
                    CNcdNodeFactory::ENcdSearchNode,
                    CNcdNodeFactory::ENcdSearchNode );
                }
            break;
            }
        }   
        
    CleanupStack::PopAndDestroy( metaIdentifier );
    DLTRACEOUT((""));
    }
    

void CNcdSearchOperation::ItemDataL(
    MNcdPreminetProtocolDataEntity* aData )
    {
    DLTRACEIN((""));

    // Item metadata is handled only when children are loaded or if the 
    // search mode is not ENormal
    //
    // If all item refs were handled recursive searching would be veeery
    // slow and we might run out of memory due to very high number of 
    // nodes (800 and more)
    if ( ( iSearchFilter->SearchMode() == MNcdSearchFilter::ENormal && 
           iLoadMode != EChildren &&
           !iNodeManager->SearchFolderL( *iNodeIdentifier ).IsTransparent() ) || 
         !aData )
        {
        delete aData;
        DLTRACEOUT(("Not loading children or no data"));
        return;
        }
            
    TRAPD( err, DoItemDataL( aData ) );

    // Delete data because ownership has been transferred.
    delete aData;     
    aData = NULL;    
 
    if( err != KErrNone )
        {
        DLERROR(("Error: %d", err));
        iError = err;
        iLoadNodeState = EFailed;
        }
        
    RunOperation();    
    }    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdSearchOperation::DoItemDataL(
    MNcdPreminetProtocolDataEntity* aData )
    {
    DLTRACEIN((""));
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
            parentIdentifier = CNcdNodeIdentifier::NewLC( *iNodeIdentifier );
            }
        delete metaIdentifier;
        metaIdentifier = NULL;
        }

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
    
    DLTRACEOUT((""));
    }
    
// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdSearchOperation::CNcdSearchOperation(
    TNcdResponseFilterParams aFilterParams,
    TNcdChildLoadMode aMode,
    TBool aLoadChildren,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler* aRemoveHandler,
    MNcdOperationQueue* aOperationQueue,
    MCatalogsSession& aSession,
    TInt aRecursionLevels,
    TBool aIsSubOperation )
    : CNcdLoadNodeOperationImpl( CNcdNodeFactory::ENcdSearchNode, 
        aFilterParams, aMode, aLoadChildren,
        aGeneralManager, aHttpSession,
        aRemoveHandler, aOperationQueue, aSession, aIsSubOperation, ETrue ), 
      iParentType( CNcdNodeFactory::ENcdNodeFolder ),
      iRecursionLeft( aRecursionLevels )
    {
    iOperationType = ESearchOperation;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CNcdSearchOperation::ConstructL(
    const CNcdNodeIdentifier& aNodeIdentifier,
    const CNcdNodeIdentifier& aParentIdentifier,
    const CNcdSearchFilter& aFilter )
    {
    DLTRACEIN((""));
    CNcdLoadNodeOperationImpl::ConstructL( aNodeIdentifier,
        aParentIdentifier );
    iSearchFilter = CNcdSearchFilter::NewL( aFilter );
    DetermineParentTypeL( aNodeIdentifier.ClientUid() );
    CNcdNode& node = iNodeManager->NodeL( *iNodeIdentifier );
    if( node.ClassId() == NcdNodeClassIds::ENcdBundleFolderNodeClassId )
        {
        // Search op started for a bundle.
        // Search op started from a non-search node (i.e. starting a new search via search api).
        // Create a search folder skeleton and replace iNodeIdentifier with the
        // resulting search folder's identifier. This makes stuff easier elsewhere in the op.
        CNcdNodeIdentifier* originalIdentifier = iNodeIdentifier;
        CNcdNode& searchNode = CreateSearchBundleSkeletonL();
        iNodeIdentifier = CNcdNodeIdentifier::NewL( searchNode.Identifier() );
        delete originalIdentifier;
        }
    else if ( CNcdNodeFactory::NodePurposeL( node.ClassId() ) !=
        CNcdNodeFactory::ENcdSearchNode )
        {
        // Search op started from a non-search node (i.e. starting a new search via search api).
        // Create a search folder skeleton and replace iNodeIdentifier with the
        // resulting search folder's identifier. This makes stuff easier elsewhere in the op.
        CNcdNodeIdentifier* originalIdentifier = iNodeIdentifier;
        CNcdNode& searchNode = CreateSearchFolderSkeletonL();
        iNodeIdentifier = CNcdNodeIdentifier::NewL( searchNode.Identifier() );
        delete originalIdentifier;
        }
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
HBufC8* CNcdSearchOperation::CreateRequestLC(
    CNcdNodeIdentifier* aNodeIdentifier,    
    TNcdResponseFilterParams aFilterParams,
    const TDesC& aUri )
    {
    //DLTRACEIN(("pagesize=%d, pagestart=%d, depth=%d",aPageSize,aPageStart,aDepth));
    DLTRACEIN((""));
    
    CNcdRequestBrowseSearch* req =
        NcdRequestGenerator::CreateSearchRequestLC();
        
    // create textual representations for content purposes
    CDesC16ArrayFlat* contentPurposes = new ( ELeave ) CDesC16ArrayFlat(
        KListGranularity );
    CleanupStack::PushL( contentPurposes );
    TUint purposes = iSearchFilter->ContentPurposes();
    if ( purposes & ENcdItemPurposeMusic )
        {
        contentPurposes->AppendL( KNcdContentPurposeMusic );
        }
    if ( purposes & ENcdItemPurposeRingtone )
        {
        contentPurposes->AppendL( KNcdContentPurposeRingtone );
        }
    if ( purposes & ENcdItemPurposeWallpaper )
        {
        contentPurposes->AppendL( KNcdContentPurposeWallpaper );
        }
    if ( purposes & ENcdItemPurposeVideo )
        {        
        contentPurposes->AppendL( KNcdContentPurposeVideo );
        }
    if ( purposes & ENcdItemPurposeTheme )
        {
        contentPurposes->AppendL( KNcdContentPurposeTheme );
        }
    if ( purposes & ENcdItemPurposeApplication )
        {
        contentPurposes->AppendL( KNcdContentPurposeApplication );
        }
    if ( purposes & ENcdItemPurposeHtmlPage )
        {        
        contentPurposes->AppendL( KNcdContentPurposeHtmlPage );
        }
    if ( purposes & ENcdItemPurposeGame )
        {
        contentPurposes->AppendL( KNcdContentPurposeGame );
        }
    if ( purposes & ENcdItemPurposeScreensaver )
        {
        contentPurposes->AppendL( KNcdContentPurposeScreensaver );
        }
    if ( purposes & ENcdItemPurposeStream )
        {
        contentPurposes->AppendL( KNcdContentPurposeStream );
        }
        
    // The parameter is the actual search node identifier. So, it can be used
    // to get the node from the manager.
    DLINFO((_L("Search node ns: %S, Id: %S"), 
            &aNodeIdentifier->NodeNameSpace(), 
            &aNodeIdentifier->NodeId()));
    CNcdNode& node = iNodeManager->NodeL( *aNodeIdentifier );

    req->SetNamespaceL( aNodeIdentifier->NodeNameSpace() );
    CDesC16ArrayFlat* elements = new (ELeave) CDesC16ArrayFlat(1);
    CleanupStack::PushL( elements );

    switch ( iLoadMode )
        {        
        case ESingleNode:
            {
            DLTRACE(("ESingleNode"))
            DASSERT( aNodeIdentifier );
            DASSERT( node.ClassId() != NcdNodeClassIds::ENcdItemNodeClassId );
            if ( node.ClassId() == NcdNodeClassIds::ENcdSearchItemNodeClassId )
                {
                // search op started for item, can't do a search
                req->SetSearch( EFalse );
                }
            else if ( node.ClassId() == NcdNodeClassIds::ENcdSearchBundleNodeClassId  )
                {
                DLTRACE(("Searching from bundle, load children as sub-operations"));
                CNcdSearchNodeFolder& folder = iNodeManager->SearchFolderL( node.Identifier() );
                const RPointerArray<CNcdChildEntity>& children = folder.ChildArray();
                for ( TInt i = 0 ; i < children.Count() ; i++ )
                    {
                    CNcdNode& childNode = iNodeManager->NodeL( children[i]->Identifier() );
                    // remote folders are loaded separately
                    CNcdNodeIdentifier* remoteFolderId =
                        CNcdNodeIdentifier::NewLC(
                            childNode.Identifier() );
                    iRemoteFolders.AppendL( remoteFolderId );
                    CleanupStack::Pop( remoteFolderId );
                    }
                DLTRACE(("Only search bundle children to load"));
                User::Leave( KNcdLoadNodeErrRemoteOnly );
                }
            else
                {
                // search op started for folder, add search filter
                req->AddEntityFilterL( iSearchFilter->Keywords(),
                    *contentPurposes,
                    ETrue,
                    ETrue);
                }            

            req->AddEntityL( 
                node.NodeLinkL().MetaDataIdentifier().NodeId(),
                node.NodeLinkL().Timestamp(),
                ETrue );

            // Use response filter only for non-transparent folders            
            if( node.ClassId() == NcdNodeClassIds::ENcdSearchFolderNodeClassId &&
                !iNodeManager->SearchFolderL( node.Identifier() ).IsTransparent() )
                {
                // If not using recursion or we are on the last level of recursion
                // we don't need to load the children. This ensures that
                // with 0 recursion depth search works just like in 3.1.50
                
                TInt childCount = INT_MAX; // load all the children
                if ( !iRecursionLeft ) 
                    {
                    childCount = 1; // load just one child
                    }
                    
                req->AddResponseFilterL(
                    childCount, 
                    0, // pageStart: load from the first child
                    1, // structureDepth: load child structure
                    0, // metaDataDepth: don't load child metadata
                    1, // metaDataPerLevel: only load one metadata per level
                    *elements,
                    *elements );
                }
            
            DLTRACE(("ESingleNode done"))
            break;
            }
            
        case EChildren:
            {
            DLTRACE(("EChildren"));
            // load children can only be done for folders
            DASSERT( node.ClassId() == NcdNodeClassIds::ENcdSearchFolderNodeClassId ||
                node.ClassId() == NcdNodeClassIds::ENcdFolderNodeClassId ||
                node.ClassId() == NcdNodeClassIds::ENcdSearchBundleNodeClassId );
            
            // add search filter
            req->AddEntityFilterL( iSearchFilter->Keywords(),
                *contentPurposes,
                ETrue,
                ETrue);

            req->AddEntityL( 
                node.NodeLinkL().MetaDataIdentifier().NodeId(),
                node.NodeLinkL().Timestamp(),
                EFalse ); // Don't load parent metadata.
            
            // loading children, use filter params
            switch ( iChildLoadMode )
                {
                case ELoadStructure:
                    {
                    DLTRACE(("ELoadStructure"));
                    // Calculate correct pagesize.
                    TInt pageSize = CalculateStructPageSize(aFilterParams.iPageStart,
                        aFilterParams.iPageSize,
                        iNodeManager->FolderL( node.Identifier() ),
                        iChildLoadMode );
                    // Add response filter to get only the desired amount of children.
                    req->AddResponseFilterL(
                        pageSize,
                        aFilterParams.iPageStart,
                        1, // structureDepth: load child structure
                        0, // metaDataDepth:  don't load child metadata
                        0, // metaDataPerLevel: don't load child metadata
                        *elements,
                        *elements );
                    break;
                    }
                    
                case ELoadMetadata:
                    {
                    DLTRACE(("ELoadMetadata"));
                    // search op should never load metadata for non-search folders
                    // as in this case no search is conducted 
                    DASSERT( node.ClassId() == 
                        NcdNodeClassIds::ENcdSearchFolderNodeClassId ||
                        node.ClassId() == NcdNodeClassIds::ENcdSearchBundleNodeClassId);
                    CNcdNodeFolder& folder = iNodeManager->FolderL( *aNodeIdentifier );
                    // Special handling for bundle folders.
                    if ( folder.ClassId() == NcdNodeClassIds::ENcdSearchBundleNodeClassId )
                        {
                        DLTRACE(("Search bundle -> load children in sub ops."));
                        for ( TInt i = aFilterParams.iPageStart ;
                             i < aFilterParams.iPageStart + aFilterParams.iPageSize ;
                             i++ )
                            {
                            CNcdNode& childNode = iNodeManager->NodeL( folder.ChildL( i ) );
                            
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
                        DLTRACE(("Normal search folder"));
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
                }
            DLTRACE(("EChildren done"));
            break;
            }
            
        case EContentSource:
        default:
            {
            // this should never happen
            DASSERT( 0 );
            break;
            }
        }
    CleanupStack::PopAndDestroy( elements );
    CleanupStack::PopAndDestroy( contentPurposes );
    
    AddQueryResponsesL( req );
    
    HBufC8* data = NULL;
    
    data = iProtocol.ProcessPreminetRequestL(
        iSession.Context(), *req, aUri );
    
    CleanupStack::PopAndDestroy( req );
    CleanupStack::PushL( data );
    return data;
    }
    
void CNcdSearchOperation::CreateSubOperationsL()
    {
    DLTRACEIN(("remote folder count: %d", iRemoteFolders.Count()));
    
    for ( TInt i = 0 ; i < iRemoteFolders.Count() ; i++ )
        {
        CNcdNode& remoteFolder = iNodeManager->NodeL( *iRemoteFolders[i] );
        CNcdSearchOperation* loadOp =
            CNcdSearchOperation::NewLC(
                *iRemoteFolders[i],
                remoteFolder.NodeLinkL().ParentIdentifier(),
                *iSearchFilter,
                TNcdResponseFilterParams(),
                iGeneralManager,
                iHttpSession,
                iRemoveHandler,
                iOperationQueue,
                iSession,
                EFalse, // Results in  ESingleNode load mode.
                ELoadStructure, // Not used in ESingleNode mode.
                iRecursionLeft - 1,
                ETrue );
        
        loadOp->AddObserverL( this );
        User::LeaveIfError( loadOp->Start() );
        iSubOps.AppendL( loadOp );
        CleanupStack::Pop( loadOp );
        }
        
    DLINFO(("Child of transparent count: %d", iRemoteFoldersChildOfTransparent.Count() ));
    
    // Create load operations for children of transparent folders.
    for ( TInt i = 0 ; i < iRemoteFoldersChildOfTransparent.Count() ; i++ )
        {
        CNcdNode& remoteFolder = iNodeManager->NodeL( *iRemoteFoldersChildOfTransparent[i] );
        CNcdSearchOperation* loadOp =
            CNcdSearchOperation::NewLC(
                *iRemoteFoldersChildOfTransparent[i],
                remoteFolder.NodeLinkL().ParentIdentifier(),
                *iSearchFilter,
                TNcdResponseFilterParams(),
                iGeneralManager,
                iHttpSession,
                iRemoveHandler,
                iOperationQueue,
                iSession,
                EFalse, // Results in  ESingleNode load mode.
                ELoadStructure, // Not used in ESingleNode mode.
                iRecursionLeft, // Don't decrease the recursion level for child of transparent
                ETrue );
        
        loadOp->AddObserverL( this );
        User::LeaveIfError( loadOp->Start() );
        iSubOps.AppendL( loadOp );
        CleanupStack::Pop( loadOp );
        }
    }
    
TBool CNcdSearchOperation::IsLoadingNecessaryL()
    {
    DLTRACEIN((""));
    DASSERT( iNodeIdentifier );
    CNcdNode& node = iNodeManager->NodeL( *iNodeIdentifier );
    
    if ( node.ClassId() == NcdNodeClassIds::ENcdSearchRootNodeClassId )
        {
        DLTRACE(("Search root -> don't load"));
        return EFalse;
        }
    else if ( iLoadMode == EChildren && iChildLoadMode == ELoadMetadata && 
              node.ClassId() != NcdNodeClassIds::ENcdSearchBundleNodeClassId )
        {
        DLTRACE(("ELoadMetadata for normal folder -> load"));
        return ETrue;
        }
    else if( iLoadMode == ESingleNode && node.ClassId() == NcdNodeClassIds::ENcdSearchBundleNodeClassId )
        {
        DLTRACE(("Search bundle and ESingleNode -> load"));
        return ETrue;
        }
    else if( node.ClassId() == NcdNodeClassIds::ENcdSearchBundleNodeClassId ||
        node.ClassId() == NcdNodeClassIds::ENcdBundleFolderNodeClassId )
        {
        DLTRACE(("Bundle or search bundle -> don't load"));
        return EFalse;
        }
    else
        {
        DLTRACE(("load"));
        return ETrue;
        }
    }
    
TBool CNcdSearchOperation::IsChildClearingNecessaryL()
    {
    DLTRACEIN((""));
    CNcdNode& node = iNodeManager->NodeL( *iNodeIdentifier );
    return node.ClassId() == NcdNodeClassIds::ENcdSearchFolderNodeClassId && 
           iLoadMode == ESingleNode;
    }


void CNcdSearchOperation::DetermineParentTypeL( const TUid& aUid )
    {
    DLTRACEIN((""));
    CNcdNodeIdentifier* searchRoot = 
        NcdNodeIdentifierEditor::CreateSearchRootIdentifierForClientLC( aUid );
    DASSERT( iNodeIdentifier );
    DASSERT( iParentIdentifier );
    // don't do parent type check for search root as it has no parent
    if ( !searchRoot->Equals( *iNodeIdentifier ) ) 
        {
        iParentType = CNcdNodeFactory::NodeTypeL(
        iNodeManager->NodeL( *iParentIdentifier ) );
        }
    CleanupStack::PopAndDestroy( searchRoot );
    }

CNcdNode& CNcdSearchOperation::CreateSearchBundleSkeletonL()
    {
    DLTRACEIN((""));
    
    CNcdNodeFolder& bundle = iNodeManager->FolderL( *iNodeIdentifier );
    
    CNcdNodeMetaData& bundleMeta = bundle.NodeMetaDataL();
    
    CNcdSearchNodeBundle& searchBundle = 
        iNodeManager->CreateSearchBundleL( bundleMeta.Identifier(),
        *iParentIdentifier );
        
    // Create a link for the bundle
    CNcdNodeLink& link = searchBundle.CreateAndSetLinkL();
    
    // Also, notice that it is essential to insert the metadata identifier into the
    // link info. So, the right metadata will be found when the bundle is opened
    // from the database. For example when application has been started.
    link.SetMetaDataIdentifierL( bundleMeta.Identifier() );
    
    // Set the metadata to the search bundle because it was not set during creation.
    searchBundle.SetNodeMetaDataL( bundleMeta );
    
    searchBundle.SetSearchFilterL( *iSearchFilter );
    
    // Set the search bundle as child of the parent node.
    iNodeManager->AddToParentL( *iParentIdentifier, bundleMeta.Identifier(), 
                                iParentType,
                                CNcdNodeFactory::ENcdSearchNode,
                                CNcdNodeFactory::ENcdSearchNode,
                                CNcdNodeManager::EInsert, 0 );
    
    // Add child folders to search bundle
    const RPointerArray<CNcdChildEntity>& children = bundle.ChildArray();
    for( TInt i = 0 ; i < children.Count() ; i++ )
        {
        // bundle's children can only be folders
        CNcdNodeFolder& bundleChild = iNodeManager->FolderL( children[i]->Identifier() );
        
        // add only children with search capability
        if( iNodeManager->IsCapabilitySupportedL( bundleChild.Identifier(),
            NcdCapabilities::KSearch,
            iSession.Context() ) )
            {
            // Bunlde child's metadata may be unavailable so just use the 
            // metadata identifier from link
            //CNcdNodeMetaData& bundleChildMeta = bundleChild.NodeMetaDataL();
            CNcdNodeLink& bundleChildLink = bundleChild.NodeLinkL();
            
            CNcdSearchNodeFolder& searchBundleChild = 
                iNodeManager->CreateSearchFolderL( bundleChildLink.MetaDataIdentifier(),
                searchBundle.Identifier() );
                
            // Create a link for the child
            CNcdNodeLink& childLink = searchBundleChild.CreateAndSetLinkL();
            
            // Also, notice that it is essential to insert the metadata identifier into the
            // link info. So, the right metadata will be found when the node is opened
            // from the database. For example when application has been started.
            childLink.SetMetaDataIdentifierL( bundleChildLink.MetaDataIdentifier() );
            
            // copy server uri from original node
            if( bundleChild.NodeLinkL().RemoteUri() != KNullDesC )
                {
                childLink.SetServerUriL( bundleChild.NodeLinkL().RemoteUri() );
                }
            else
                {
                childLink.SetServerUriL( bundleChild.NodeLinkL().ServerUri() );
                }
            
            // Set the metadata to the bundle folder because it was not set during creation.
            //searchBundleChild.SetNodeMetaDataL( bundleChildMeta );
            
            searchBundleChild.SetSearchFilterL( *iSearchFilter );

            TBool transparent = 
                ( bundleChild.ClassId() == NcdNodeClassIds::ENcdTransparentFolderNodeClassId );
                        
            searchBundleChild.SetTransparent( transparent );
                
            
            // Set the folder as child of search bundle.
            iNodeManager->AddToParentL( searchBundle.Identifier(), bundleChildLink.MetaDataIdentifier(), 
                                        CNcdNodeFactory::ENcdNodeSearchBundle,
                                        CNcdNodeFactory::ENcdSearchNode,
                                        CNcdNodeFactory::ENcdSearchNode, //This parameter is unused
                                        CNcdNodeManager::EAppend, 
                                        0,
                                        transparent );
                                        
            searchBundleChild.SetOriginIdentifierL( bundleChild.Identifier() );
            iNodeManager->DbSaveNodeL( searchBundleChild );
            }
        }
    iNodeManager->DbSaveNodeL( searchBundle );
    return searchBundle;
    }

CNcdNode& CNcdSearchOperation::CreateSearchFolderSkeletonL()
    {
    DLTRACEIN((""));
    
    CNcdNodeFolder& folder = iNodeManager->FolderL( *iNodeIdentifier );
    
    CNcdNodeMetaData& folderMeta = folder.NodeMetaDataL();
    
    CNcdSearchNodeFolder& searchFolder = 
        iNodeManager->CreateSearchFolderL( folderMeta.Identifier(),
        *iParentIdentifier );
        
    // Create a link for the folder
    CNcdNodeLink& link = searchFolder.CreateAndSetLinkL();
    
    // Also, notice that it is essential to insert the metadata identifier into the
    // link info. So, the right metadata will be found when the folder is opened
    // from the database. For example when application has been started.
    link.SetMetaDataIdentifierL( folderMeta.Identifier() );
    
    // copy server uri from original node
    if( folder.NodeLinkL().RemoteUri() != KNullDesC )
        {
        link.SetServerUriL( folder.NodeLinkL().RemoteUri() );
        }
    else
        {
        link.SetServerUriL( folder.NodeLinkL().ServerUri() );
        }
    
    // Set the metadata to the search bundle because it was not set during creation.
    searchFolder.SetNodeMetaDataL( folderMeta );
    
    searchFolder.SetSearchFilterL( *iSearchFilter );
    
    TBool transparent = 
        ( folder.ClassId() == NcdNodeClassIds::ENcdTransparentFolderNodeClassId );
        
    // Set the search folder as child of the parent node.
    iNodeManager->AddToParentL( *iParentIdentifier, 
                                folderMeta.Identifier(), 
                                iParentType,
                                CNcdNodeFactory::ENcdSearchNode,
                                CNcdNodeFactory::ENcdSearchNode,
                                CNcdNodeManager::EInsert, 
                                0,
                                transparent );
    
    searchFolder.SetOriginIdentifierL( *iNodeIdentifier );
    
    // Because AddToParentL only saves the parent information after child is added to it
    // we should always save the bundle node here. So, it will be usable after client is
    // restarted.
    iNodeManager->DbSaveNodeL( searchFolder );
    
    return searchFolder;
    }
    
void CNcdSearchOperation::ErrorL( MNcdPreminetProtocolError* aData )
    {
    DLTRACEIN((""));
    switch ( aData->Code() )
        {
        case 404:
            {
            // Removed/missing nodes need no special handling in search op,
            // so just fail the operation.
            iError = KNcdErrorNodeWasRemoved;
            iLoadNodeState = EFailed;
            Cancel();
            delete aData;
            RunOperation();
            break;
            }
        default:
            {
            CNcdLoadNodeOperationImpl::ErrorL( aData );
            break;
            }
        }    
    }

TInt CNcdSearchOperation::RemoteFolderCount() const
    {
    DLTRACEIN((""));
    return iRemoteFolders.Count() + iRemoteFoldersChildOfTransparent.Count();
    }
    
    
void CNcdSearchOperation::RemoveChildrenL( CNcdNodeFolder& aFolder ) 
    {
    DLTRACEIN((""));
    aFolder.RemoveChildren();
    }
    
