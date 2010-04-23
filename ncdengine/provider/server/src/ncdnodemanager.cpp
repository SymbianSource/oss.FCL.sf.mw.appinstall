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
* Description:   Implements CNcdNodeManager class
*
*/


#include "ncdnodemanager.h"

#include <e32err.h>
#include <s32mem.h>
#include <badesca.h>

#include "ncdnodedbmanager.h"
#include "ncdnodecachecleaner.h"
#include "ncdnodecachecleanermanager.h"
#include "ncdnodeidentifier.h"
#include "ncdnodeidentifiereditor.h"
#include "ncdnodeclassids.h"
#include "ncdnodeimpl.h"
#include "ncdnodeitem.h"
#include "ncdnodefolder.h"
#include "ncdbundlefolder.h"
#include "ncdnodetransparentfolder.h"
#include "ncdrootnode.h"
#include "ncdsearchnodefolder.h"
#include "ncdnodelink.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodeitemmetadata.h"
#include "ncdnodefoldermetadata.h"
#include "ncdsearchnodeitem.h"
#include "ncdsearchnodefolder.h"
#include "ncdsearchrootnode.h"
#include "ncdexpirednode.h"
#include "ncdchildentity.h"
#include "ncdpurchasehistorydbimpl.h"
#include "ncdnodepreviewimpl.h"
#include "ncdstoragedescriptordataitem.h"
#include "catalogsbasemessage.h"
#include "catalogssession.h"
#include "ncd_pp_folderref.h"
#include "ncd_pp_itemref.h"
#include "ncd_pp_dataentity.h"
#include "ncd_pp_download.h"
#include "ncdpreviewmanager.h"
#include "ncdstoragemanagerimpl.h"
#include "ncdconfigurationmanager.h"
#include "ncdnodefunctionids.h"
#include "ncdproviderdefines.h"
#include "ncdserverdetails.h"
#include "catalogscontext.h"
#include "catalogsutils.h"
#include "catalogsconstants.h"
#include "ncd_pp_expiredcacheddata.h"
#include "ncdproviderutils.h"
#include "ncdutils.h"
#include "ncdsearchnodebundle.h"
#include "ncdfavoritemanagerimpl.h"
#include "ncdnodeseeninfo.h"
#include "ncdnodeiconimpl.h"
#include "ncderrors.h"
#include "ncdsearchablenode.h"
#include "ncdnodeidentifierutils.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"

// If the values from the provider are given in kilos and other classes
// require them in bytes, this value can be used for conversion.
const TInt KBytesToKilos( 1024 );

const TInt KNodeCacheGranularity = 256;

CNcdNodeManager::CNcdNodeManager( CNcdGeneralManager& aGeneralManager )
: CCatalogsCommunicable(),
  iPurchaseHistory( aGeneralManager.PurchaseHistory() ),  
  iNodeCache( KNodeCacheGranularity ),
  iNodeMetaDataCache( KNodeCacheGranularity ),
  iTempNodeCache( KNodeCacheGranularity ),
  iConfigurationManager( aGeneralManager.ConfigurationManager() ),
  iNodeOrder( CNcdNode::Compare ),
  iGeneralManager( aGeneralManager )
    {
    DLTRACEIN((""));    
    }


void CNcdNodeManager::ConstructL()
    {
    DLTRACEIN((""));
    iGeneralManager.SetNodeManager( *this );
    iDbManager = CNcdNodeDbManager::NewL( iGeneralManager.StorageManager() );
    
    // Make sure that db manager is created before factory is created.
    iNodeFactory = CNcdNodeFactory::NewL( *this );
    
    // Notice that this function uses the objects created above.
    // So, if the creation is moved somewhere else, make sure that
    // NodeDbManager and NodeFactory are also created.
    iNodeCacheCleanerManager = 
        CNcdNodeCacheCleanerManager::NewL( iGeneralManager, 
                                           NodeDbManager(),
                                           NcdProviderDefines::KDbDefaultMaxByteSize,
                                           NodeFactory() );

    iPreviewManager = CNcdPreviewManager::NewL( iGeneralManager, 
                                          NcdProviderDefines::KMaxClientPreviews );
    iPreviewManager->LoadDataL();
    iGeneralManager.SetNodeManager( *this );
    
    iSeenInfo = CNcdNodeSeenInfo::NewL( iGeneralManager );

    iSearchableNode = new ( ELeave ) CNcdSearchableNode( *this );
    DLTRACEOUT((""));
    }


CNcdNodeManager::~CNcdNodeManager()
    {
    DLTRACEIN((""));
    
    // Ensure that all cached objects are closed
    FullCacheCleanup();


    if ( iSearchableNode ) 
        {
        iSearchableNode->Close();        
        }
        
    // Delete member variables in reverse order when comparing
    // to the creation order.

    delete iPreviewManager;
    
    // Delete node cache cleaner.
    delete iNodeCacheCleanerManager;   

    // Delete node factory.
    delete iNodeFactory;

    // Delete the db manager.
    delete iDbManager;
    
    // Delete the seen info.
    delete iSeenInfo;

    iClientDatabaseLocks.Close();
    
    DLTRACEOUT((""));
    }


CNcdNodeManager* CNcdNodeManager::NewL( CNcdGeneralManager& aGeneralManager )
    {
    CNcdNodeManager* self =
        new( ELeave ) CNcdNodeManager( aGeneralManager );
    CleanupClosePushL( *self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// General getter functions
// ---------------------------------------------------------------------------

CNcdPurchaseHistoryDb& CNcdNodeManager::PurchaseHistory() const
    {
    return iPurchaseHistory;
    }

CNcdPreviewManager& CNcdNodeManager::PreviewManager()  const
    {
    return *iPreviewManager;
    }



// ---------------------------------------------------------------------------
// Functions to get node objects from caches or from db. 
// And, functions to create node objects.
// ---------------------------------------------------------------------------

CNcdNode& CNcdNodeManager::NodeL( const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN((""));
    
    CNcdNode* node = NodePtrL( aIdentifier );
    if ( node == NULL )
        {
        User::Leave( KErrNotFound );
        }

    DLTRACEOUT((""));
    return *node;
    }

CNcdNode& CNcdNodeManager::NodeL( const CNcdNodeIdentifier& aParentNodeIdentifier,
                                  const CNcdNodeIdentifier& aNodeMetaDataIdentifier )
    {
    DLTRACEIN((""));
    
    CNcdNodeIdentifier* identifier =
        NcdNodeIdentifierEditor::CreateNodeIdentifierLC( aParentNodeIdentifier, 
                                                         aNodeMetaDataIdentifier );
    CNcdNode& node = NodeL( *identifier );

    CleanupStack::PopAndDestroy( identifier );

    DLTRACEOUT((""));

    return node;
    }


CNcdNode* CNcdNodeManager::NodePtrL( const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN(( _L("Node id: %S, %S, %d"), 
                &aIdentifier.NodeNameSpace(),
                &aIdentifier.NodeId(),
                aIdentifier.ClientUid().iUid ));

    CNcdNode* node( FindNodeFromCacheL( aIdentifier ) );
    if ( node != NULL )
        {
        DLTRACEOUT(("Found node"));
        return node;
        }

    DLINFO(("Start loading the node from storage"));
    // Check if the node can be found from the db because it was not found
    // from the cache.
    // This function leaves with KErrNotFound if the data was not found from
    // the database. If this is the case then the data should be loaded from
    // internet. But it is not job of this function. Let it leave.
    TRAPD( trapError, node = &DbNodeL( aIdentifier ) );
    if ( trapError == KErrNotFound )
        {
        // Node was not found from db. So, return NULL value.
        DLTRACEOUT(("Node was not found anywhere. Return NULL."));
        return NULL;
        }
    // Leave if some other error occurred.
    User::LeaveIfError( trapError );

    DLTRACEOUT(("Node found from db"));
    return node;
    }


CNcdNode* CNcdNodeManager::NodePtrL( const CNcdNodeIdentifier& aParentNodeIdentifier,
                                     const CNcdNodeIdentifier& aNodeMetaDataIdentifier )
    {
    DLTRACEIN((""));
    
    CNcdNodeIdentifier* identifier =
        NcdNodeIdentifierEditor::CreateNodeIdentifierLC( aParentNodeIdentifier, 
                                                         aNodeMetaDataIdentifier );
    CNcdNode* node = NodePtrL( *identifier );

    CleanupStack::PopAndDestroy( identifier );

    DLTRACEOUT((""));

    return node;
    }


CNcdNodeFolder& CNcdNodeManager::FolderL( const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN(( _L("Node id: %S, %S, %d"), 
                &aIdentifier.NodeNameSpace(),
                &aIdentifier.NodeId(),
                aIdentifier.ClientUid().iUid ));

    CNcdNode& node( NodeL( aIdentifier ) );
    CNcdNodeFactory::TNcdNodeType nodeType( NodeFactory().NodeTypeL( node ) );
    if ( nodeType != CNcdNodeFactory::ENcdNodeFolder &&
        nodeType != CNcdNodeFactory::ENcdNodeSearchBundle
         && nodeType != CNcdNodeFactory::ENcdNodeRoot )
        {
        // The node was not of the right type
        DLERROR(("Wrong type of node: %d. Should be folder: %d or %d or %d",
                 nodeType, 
                 CNcdNodeFactory::ENcdNodeFolder,
                 CNcdNodeFactory::ENcdNodeSearchBundle,
                 CNcdNodeFactory::ENcdNodeRoot));

        // For debug purposes
        DASSERT( EFalse );
                
        User::Leave( KErrArgument );
        }

    DLTRACEOUT((""));

    return static_cast<CNcdNodeFolder&>( node );
    }


CNcdRootNode& CNcdNodeManager::RootNodeL( const TUid& aClientUid )
    {
    DLTRACEIN((""));

    // All the search roots have the same namespace, but their ids are
    // the client UIDs.
    CNcdNodeIdentifier* identifier = 
        NcdNodeIdentifierEditor::CreateRootIdentifierForClientLC( aClientUid );

    CNcdNode& node( NodeL( *identifier ) );

    CleanupStack::PopAndDestroy( identifier );

    // Check the type to make sure that the casting is ok in the end.
    CNcdNodeFactory::TNcdNodeType nodeType( NodeFactory().NodeTypeL( node ) );
    if ( nodeType != CNcdNodeFactory::ENcdNodeRoot )
        {
        // The node was not right
        DLERROR(("Wrong node type. Should be root"));

        // For debug purposes
        DASSERT( EFalse );
                
        User::Leave( KErrArgument );
        }

    DLTRACEOUT((""));

    return static_cast<CNcdRootNode&>( node );            
    }


CNcdSearchNodeFolder& CNcdNodeManager::SearchFolderL( const CNcdNodeIdentifier& aParentNodeIdentifier,
                                                      const CNcdNodeIdentifier& aNodeMetaDataIdentifier )
    {
    DLTRACEIN((""));

    CNcdNode& node( NodeL( aParentNodeIdentifier, aNodeMetaDataIdentifier ) );
    
    // Check the type and purpose to make sure that the casting is ok in the end.
    CNcdNodeFactory::TNcdNodeType nodeType( NodeFactory().NodeTypeL( node ) );
    CNcdNodeFactory::TNcdNodePurpose nodePurpose( NodeFactory().NodePurposeL( node ) );
    if ( nodeType != CNcdNodeFactory::ENcdNodeFolder
         && nodeType != CNcdNodeFactory::ENcdNodeRoot
         || nodePurpose != CNcdNodeFactory::ENcdSearchNode )
        {
        // The node was not right
        DLERROR(("Wrong node class."));

        DASSERT( EFalse );

        // The node was not of the right type
        User::Leave( KErrArgument );
        }

    DLTRACEOUT((""));

    return static_cast<CNcdSearchNodeFolder&>( node );
    }


CNcdSearchNodeFolder& CNcdNodeManager::SearchFolderL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));

    CNcdNode& node( NodeL( aNodeIdentifier ));
    
    // Check the type and purpose to make sure that the casting is ok in the end.
    CNcdNodeFactory::TNcdNodeType nodeType( NodeFactory().NodeTypeL( node ) );
    CNcdNodeFactory::TNcdNodePurpose nodePurpose( NodeFactory().NodePurposeL( node ) );
    if ( nodeType != CNcdNodeFactory::ENcdNodeFolder
         && nodeType != CNcdNodeFactory::ENcdNodeRoot
         && nodeType != CNcdNodeFactory::ENcdNodeSearchBundle
         || nodePurpose != CNcdNodeFactory::ENcdSearchNode )
        {
        // The node was not right
        DLERROR(("Wrong node class."));

        DASSERT( EFalse );

        // The node was not of the right type
        User::Leave( KErrArgument );
        }

    DLTRACEOUT((""));

    return static_cast<CNcdSearchNodeFolder&>( node );
    }
    
    
CNcdSearchNodeItem& CNcdNodeManager::SearchNodeItemL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));

    CNcdNode& node( NodeL( aNodeIdentifier ));
    
    if ( node.ClassId() != NcdNodeClassIds::ENcdSearchItemNodeClassId )
        {
        // The node was not right
        DLERROR(("Wrong node class."));

        DASSERT( EFalse );

        // The node was not of the right type
        User::Leave( KErrArgument );
        }

    DLTRACEOUT((""));

    return static_cast<CNcdSearchNodeItem&>( node );
    }


CNcdNode& CNcdNodeManager::CreateNodeL( CNcdNodeFactory::TNcdNodeType aNodeType,
                                        CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                        const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN(("Nodetype: %d, nodepurpose: %d", aNodeType, aNodePurpose));
    DLINFO((_L("Identifier ns: %S, id: %S"), 
            &aNodeIdentifier.NodeNameSpace(),
            &aNodeIdentifier.NodeId()));

    // The node identifier can not be empty
    DASSERT( !aNodeIdentifier.ContainsEmptyFields() );

    // Get the class id of the node that has the given purpose.
    // The data type parameter informs if an item or a folder should be created.
    // Set the class id to be some item as a default.
    NcdNodeClassIds::TNcdNodeClassId classId =
            NodeFactory().NodeClassIdL( aNodeType, aNodePurpose );

    DLINFO(("classid of node: %d", classId));

    CNcdNode* node( NodePtrL( aNodeIdentifier ) );
       
    if ( node == NULL )
        {
        DLINFO(("Create node"));
        // Create the node according to the class id
        node = NodeFactory().CreateNodeLC( aNodeIdentifier,
                                           classId );

        DASSERT( node );
        
        // and insert node to the cache.
        AppendNodeToCacheL( node );
        
        // cache takes ownership of the node
        CleanupStack::Pop( node );
        
        // NodePtr inserts the metadata if node was found there.
        // There is no reason to insert the metadata here, because the
        // node has been just created, and it does not contains the
        // link data where the metadata id info is inserted.
        }
    else if ( NodeFactory().NodeTypeL( *node ) != aNodeType
              || NodeFactory().NodePurposeL( *node  ) != aNodePurpose )
        {
        // Node was already in the list. But, the node is not
        // of the expected type or purpose.
        // Notice, that if the requested node purpose was scheme node and the
        // node that already exists is of the different purpose, then we use the
        // original node. So, schemes do not require this checking.
        DLINFO(("classid of real node: %d", node->ClassId() ));
        DLERROR(("Wrong node type or purpose."));
        DLERROR(("Type: %d, should be: %d",
                 NodeFactory().NodeTypeL( *node ), 
                 aNodeType));
        DLERROR(("Purpose: %d, should be: %d",
                 NodeFactory().NodePurposeL( *node ), 
                 aNodePurpose));
        DASSERT( EFalse );
        User::Leave( KErrArgument );
        }

    DLTRACEOUT((""));

    return *node;     
    }


CNcdNode& CNcdNodeManager::CreateNodeL( CNcdNodeFactory::TNcdNodeType aNodeType,
                                        CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                        const CNcdNodeIdentifier& aParentNodeIdentifier,
                                        const CNcdNodeIdentifier& aMetaDataIdentifier )
    {
    DLTRACEIN((""));

    // Check the metadata identifier because it has to contain values. These values
    // are used when the node is created. The parent node may contain empty values.
    // because the root does not have parent.
    if ( aMetaDataIdentifier.ContainsEmptyFields() )
        {
        DLERROR(("Cannot create node with empty identifier meta values"));
        
        // For debug purposes
        DASSERT( EFalse );
        
        User::Leave( KErrArgument );
        }

    // Try to find the node before creating it.
    // Use the correct form of identifier by asking it from the factory
    CNcdNodeIdentifier* identifier =
        NcdNodeIdentifierEditor::CreateNodeIdentifierLC( aParentNodeIdentifier, 
                                                         aMetaDataIdentifier );
    CNcdNode& node( CreateNodeL( aNodeType, aNodePurpose, *identifier ) );
    CleanupStack::PopAndDestroy( identifier );

    DLTRACEOUT((""));

    return node; 
    }


CNcdNodeItem& CNcdNodeManager::CreateNodeItemL( CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                                const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));

    CNcdNode& node =
        CreateNodeL( CNcdNodeFactory::ENcdNodeItem, aNodePurpose, 
                     aNodeIdentifier );
    
    // Now it is safe to do casting.

    DLTRACEOUT((""));

    return static_cast<CNcdNodeItem&>(node);    
    }

CNcdNodeItem& CNcdNodeManager::CreateNodeItemL( CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                                const CNcdNodeIdentifier& aParentNodeIdentifier,
                                                const CNcdNodeIdentifier& aMetaDataIdentifier )
    {
    DLTRACEIN((""));

    CNcdNode& node =
        CreateNodeL( CNcdNodeFactory::ENcdNodeItem, aNodePurpose, 
                     aParentNodeIdentifier, aMetaDataIdentifier );
    
    // Now it is safe to do casting.

    DLTRACEOUT((""));

    return static_cast<CNcdNodeItem&>(node);   
    }


CNcdNodeFolder& CNcdNodeManager::CreateNodeFolderL( CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                                    const CNcdNodeIdentifier& aNodeIdentifier,
                                                    CNcdNodeFactory::TNcdNodeType aFolderType )
    {
    DLTRACEIN((""));

    CNcdNode& node =
        CreateNodeL( aFolderType, aNodePurpose, 
                     aNodeIdentifier );
    
    // Now it is safe to do casting.

    DLTRACEOUT((""));

    return static_cast<CNcdNodeFolder&>(node);    
    }
    
CNcdNodeFolder& CNcdNodeManager::CreateNodeFolderL( CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                                    const CNcdNodeIdentifier& aParentNodeIdentifier,
                                                    const CNcdNodeIdentifier& aMetaDataIdentifier,
                                                    CNcdNodeFactory::TNcdNodeType aFolderType )
    {
    DLTRACEIN((""));

    CNcdNode& node =
        CreateNodeL( aFolderType, aNodePurpose, 
                     aParentNodeIdentifier, aMetaDataIdentifier );
    
    // Now it is safe to do casting.

    DLTRACEOUT((""));

    return static_cast<CNcdNodeFolder&>(node);    
    }    


CNcdRootNode& CNcdNodeManager::CreateRootL( const TUid& aClientUid )
    {
    DLTRACEIN((""));
    
    CNcdNodeIdentifier* rootIdentifier( 
        NcdNodeIdentifierEditor::CreateRootIdentifierForClientLC( aClientUid ) );
        
    CNcdNode& node( CreateNodeL( CNcdNodeFactory::ENcdNodeRoot,
                                 CNcdNodeFactory::ENcdNormalNode,
                                 *rootIdentifier ) );

    CleanupStack::PopAndDestroy( rootIdentifier );

    DLTRACEOUT((""));

    return static_cast<CNcdRootNode&>( node );            
    }


CNcdRootNode& CNcdNodeManager::CreateRootL( const MCatalogsContext& aContext )
    {
    DLTRACEIN((""));
    return CreateRootL( aContext.FamilyId() );
    }
    
    
CNcdNodeFolder& CNcdNodeManager::CreateSearchRootL( const MCatalogsContext& aContext )
    {
    DLTRACEIN((""));
    
    CNcdNodeIdentifier* rootIdentifier( 
        NcdNodeIdentifierEditor::CreateSearchRootIdentifierForClientLC( aContext.FamilyId() ) );
        
    CNcdNode& node( CreateNodeL( CNcdNodeFactory::ENcdNodeRoot,
                                 CNcdNodeFactory::ENcdSearchNode,
                                 *rootIdentifier ) );

    CleanupStack::PopAndDestroy( rootIdentifier );

    DLTRACEOUT((""));

    return static_cast<CNcdNodeFolder&>( node );
    }


CNcdNodeFolder& CNcdNodeManager::CreateBundleFolderL( const CNcdNodeIdentifier& aMetaDataIdentifier )
    {
    DLTRACEIN((""));
    // The parent of the bundle is always the root folder
    CNcdNodeIdentifier* rootIdentifier = 
        NcdNodeIdentifierEditor::CreateRootIdentifierForClientLC( aMetaDataIdentifier.ClientUid() );

    CNcdNodeFolder& folder =
        CreateNodeFolderL( CNcdNodeFactory::ENcdBundleNode, 
                           *rootIdentifier, 
                           aMetaDataIdentifier );
                           
    CleanupStack::PopAndDestroy( rootIdentifier );

    DLTRACEOUT((""));
    
    return folder;
    }


CNcdNode& CNcdNodeManager::CreateTemporaryNodeOrSupplierL( const CNcdNodeIdentifier& aMetaDataIdentifier )
    {
    DLTRACEIN((""));

    CNcdNode* node( NULL );

    // First check if the given temporary node already exists in RAM or in databse.
    // We can use NodeL also for this operation.
    CNcdNodeIdentifier* tempNodeIdentifier =
        NcdNodeIdentifierEditor::CreateTemporaryNodeIdentifierLC( aMetaDataIdentifier );
        
    // Note that if the node is found it is also inserted into the node cache
    // but it is not saved into the database.
    TRAP_IGNORE( node = &NodeL( *tempNodeIdentifier ) );

    // If the temporary node did not exist. Then create supplier, which also checks
    // if the corresponding node already exists in RAM or in database.
    // CreateNodeL can handle this.
    // If it does not exist then there is still possibility that the corresponding
    // metadata exists. If metadata exists, then it can be used to create the correct
    // temporary node
    if ( node == NULL )
        {
        // First check the metadata.
        // The ownership of the meta will be in cache.
        CNcdNodeMetaData* meta( NULL );
        TRAP_IGNORE( meta = &NodeMetaDataL( aMetaDataIdentifier ) );
        
        if ( meta != NULL )
            {
            DLINFO(("Meta was found for the temp node"));
            
            // Metadata was found. So, create right type temporary node.
            if ( meta->ClassId() 
                    == NcdNodeClassIds::ENcdFolderNodeMetaDataClassId )
                {
                DLINFO(("Create temp folder"));
                node = &CreateNodeFolderL( CNcdNodeFactory::ENcdNormalNode,
                                           *tempNodeIdentifier );
                }
            else if ( meta->ClassId() 
                        == NcdNodeClassIds::ENcdItemNodeMetaDataClassId )
                {
                DLINFO(("Create temp item"));
                node = &CreateNodeItemL( CNcdNodeFactory::ENcdNormalNode,
                                         *tempNodeIdentifier );                
                }
            else
                {
                DLINFO(("Unknown meta type"));
                DASSERT( EFalse );
                User::Leave( KErrUnknown );
                }
            
            DLINFO(("Setting node metadata"))
            node->SetNodeMetaDataL( *meta );
            }
        else
            {
            DLINFO(("Create node supplier"));
            node = &CreateNodeL( CNcdNodeFactory::ENcdNodeSupplier,
                                 CNcdNodeFactory::ENcdNormalNode,
                                 *tempNodeIdentifier );
            }
        }
    // Notice that if the node creation worked correctly, it will
    // be added to the cache list. So the ownership is in cache, so
    // no need to keep the node in cleanup stack.
    
    // Now that we have the node, make sure that all the necessary information
    // is inserted to its link. This includes metadata identifier and the
    // server uri.
    CNcdNodeLink& link( node->CreateAndSetLinkL() );
    link.SetMetaDataIdentifierL( aMetaDataIdentifier );
    link.SetServerUriL( aMetaDataIdentifier.ServerUri() );
    
    CleanupStack::PopAndDestroy( tempNodeIdentifier );

    // Now that everything is ok, save the node into the db.
    DbSaveNodeL( *node );
    
    DLTRACEOUT((""));
    
    return *node;
    }
    

CNcdNode* CNcdNodeManager::CreateTemporaryNodeIfMetadataExistsL(
    const CNcdNodeIdentifier& aMetadataIdentifier ) 
    {
    DLTRACEIN((""));
    
    // Check if the metadata exists.
    CNcdNodeMetaData* metadata( NULL );
    TRAPD( err, metadata = &NodeMetaDataL( aMetadataIdentifier ) );
    if ( err == KErrNotFound ) 
        {
        // Metadata not found, cannot do anything.
        return NULL;
        }
    
    User::LeaveIfError( err );
    DASSERT( metadata );
    
    // Metadata exists, create temporary node of correct type.   

    // First check if the given temporary node already exists in RAM or in databse.
    // We can use NodeL also for this operation.
    CNcdNodeIdentifier* tempNodeIdentifier =
        NcdNodeIdentifierEditor::CreateTemporaryNodeIdentifierLC( aMetadataIdentifier );

    CNcdNode* node( NULL );
    TRAP( err, node = &NodeL( *tempNodeIdentifier ) );
    LeaveIfNotErrorL( err, KErrNotFound );
    
    if ( node == NULL )
        {
        // If the temporary node did not exist. Create one according to the type of
        // the metadata.            
        if ( metadata->ClassId() == NcdNodeClassIds::ENcdFolderNodeMetaDataClassId )
            {
            DLINFO(("Create temp folder"));
            node = &CreateNodeFolderL( CNcdNodeFactory::ENcdNormalNode,
                                       *tempNodeIdentifier );
            }
        else if ( metadata->ClassId() 
                    == NcdNodeClassIds::ENcdItemNodeMetaDataClassId )
            {
            DLINFO(("Create temp item"));
            node = &CreateNodeItemL( CNcdNodeFactory::ENcdNormalNode,
                                     *tempNodeIdentifier );                
            }
        else
            {
            DLINFO(("Unknown meta type"));
            DASSERT( EFalse );
            User::Leave( KErrUnknown );
            }
        
        DLINFO(("Setting node metadata"))
        node->SetNodeMetaDataL( *metadata );
        }
    else
        {
        // Temporary node exists. Check that it has the metadata.
        CNcdNodeMetaData* existingMeta = node->NodeMetaData();
        if ( existingMeta )
            {
            DASSERT( existingMeta == metadata );
            }
        else 
            {
            // Temporary node did not have metadata, install it.
            node->SetNodeMetaDataL( *metadata );
            }
        }
        
    DASSERT( node );
    DASSERT( node->NodeMetaData() );
    
    // Notice that if the node creation worked correctly, it will
    // be added to the cache list. So the ownership is in cache, so
    // no need to keep the node in cleanup stack.
    
    // Now that we have the node, make sure that all the necessary information
    // is inserted to its link. This includes metadata identifier and the
    // server uri.
    CNcdNodeLink& link( node->CreateAndSetLinkL() );
    link.SetMetaDataIdentifierL( aMetadataIdentifier );
    link.SetServerUriL( aMetadataIdentifier.ServerUri() );
    
    CleanupStack::PopAndDestroy( tempNodeIdentifier );

    // Now that everything is ok, save the node into the db.
    DbSaveNodeL( *node );
    
    DLTRACEOUT((""));
    
    return node;
    }
    
    
CNcdSearchNodeBundle& CNcdNodeManager::CreateSearchBundleL( const CNcdNodeIdentifier& aMetaDataIdentifier,
    const CNcdNodeIdentifier& aParentIdentifier )
    {
    DLTRACEIN((""));
    

    CNcdNodeFolder& folder =
        CreateNodeFolderL( CNcdNodeFactory::ENcdSearchNode, 
                           aParentIdentifier, 
                           aMetaDataIdentifier,
                           CNcdNodeFactory::ENcdNodeSearchBundle );
                           
    // Now it is safe to do casting.

    DLTRACEOUT((""));

    return static_cast<CNcdSearchNodeBundle&>(folder);
    }

CNcdSearchNodeFolder& CNcdNodeManager::CreateSearchFolderL( const CNcdNodeIdentifier& aMetaDataIdentifier,
    const CNcdNodeIdentifier& aParentIdentifier )
    {
    DLTRACEIN((""));
    

    CNcdNodeFolder& folder =
        CreateNodeFolderL( CNcdNodeFactory::ENcdSearchNode, 
                           aParentIdentifier, 
                           aMetaDataIdentifier,
                           CNcdNodeFactory::ENcdNodeFolder );
                           
    // Now it is safe to do casting.

    DLTRACEOUT((""));

    return static_cast<CNcdSearchNodeBundle&>(folder);
    }


CNcdNodeMetaData& CNcdNodeManager::NodeMetaDataL(  
    const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN(( _L("Metadata id: %S, %S, %d"), 
                &aIdentifier.NodeNameSpace(),
                &aIdentifier.NodeId(),
                aIdentifier.ClientUid().iUid ));

    CNcdNodeMetaData* metaData( 
        FindNodeMetaDataFromCache( aIdentifier ) );

    if ( metaData == NULL )
        {
        // Check if the node can be found from the db because it was not found
        // from the cache.
        // This function leaves with KErrNotFound if the data was not found from
        // the database. If this is the case then the data should be loaded from
        // internet. But it is not job of this function. Let it leave.
        metaData = &DbMetaDataL( aIdentifier );        
        }

    DLTRACEOUT((""));
    
    return *metaData;
    }


CNcdNodeMetaData& CNcdNodeManager::CreateNodeMetaDataL(
    const CNcdNodeIdentifier& aMetaDataIdentifier,
    CNcdNodeFactory::TNcdNodeType aMetaType ) 
    {
    
    DLTRACEIN(( _L("Metadata id: %S, %S, %d"), 
                &aMetaDataIdentifier.NodeNameSpace(),
                &aMetaDataIdentifier.NodeId(),
                aMetaDataIdentifier.ClientUid().iUid ));
    
    CNcdNodeMetaData* metaData( NULL );
    TRAPD( err, metaData = &NodeMetaDataL( aMetaDataIdentifier ) );
    if ( err != KErrNone ) 
        {
        if ( err == KErrNotFound ) 
            {
            metaData = NodeFactory().CreateMetaDataLC( aMetaDataIdentifier, aMetaType );
            iNodeMetaDataCache.AppendL( metaData );
            // Cache takes ownership of the metadata
            CleanupStack::Pop( metaData );
            }
        else 
            {
            User::Leave( err );
            }
        }
    
    return *metaData;
    }



// ---------------------------------------------------------------------------
// Functions that are used to insert data information
// gotten from the data parsers into the correct node
// objects. These functions are provided for operations.
// The updated information may also be inserted from the 
// purchase history.
// ---------------------------------------------------------------------------

CNcdNode& CNcdNodeManager::RefHandlerL( 
    const CNcdNodeIdentifier& aParentNodeIdentifier,
    MNcdPreminetProtocolEntityRef& aData,
    const TUid& aClientUid,
    TNcdRefHandleMode aMode,
    TInt aIndex,
    CNcdNodeFactory::TNcdNodeType aParentNodeType,
    CNcdNodeFactory::TNcdNodePurpose aParentNodePurpose,
    CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
    TBool aCreateParent )
    {
    DLTRACEIN(("aData: %X, nodepurpose: %d, aCreateParent: %d", &aData, aNodePurpose, aCreateParent));
    
    DLINFO((_L("parent node ns: %S, id: %S"), 

            &aParentNodeIdentifier.NodeNameSpace(), &aParentNodeIdentifier.NodeId()));
    DLINFO((_L("parent node from entity ns: %S, id: %S"), 
            &aData.ParentNamespace(), &aData.ParentId()));

    DLINFO((_L("data ns: %S, id: %S"), 
            &aData.Namespace(), &aData.Id()));
            
    DLINFO((_L("insert index: %d, mode: %d"), aIndex, aMode ));
    DPROFILING_BEGIN( x );

    // Get the type of the node -- folder or item.
    CNcdNodeFactory::TNcdNodeType nodeType = CNcdNodeFactory::ENcdNodeItem;
    if ( aData.Type() == MNcdPreminetProtocolEntityRef::EFolderRef )
        {
        nodeType = CNcdNodeFactory::ENcdNodeFolder;
        }
    else if ( aData.Type() != MNcdPreminetProtocolEntityRef::EItemRef )
        {                
        // Wrong type
        DLINFO(( "Wrong node data type: %d", aData.Type() ));
        DASSERT( EFalse );
        User::Leave( KErrArgument );
        }   

    // Notice that the aClientUid is used here instead of the parent identifier uid,
    // because the parent identifier may be empty in some cases.
    CNcdNodeIdentifier* metaIdentifier = 
        CNcdNodeIdentifier::NewLC( aData.Namespace(), 
                                   aData.Id(),
                                   aData.ServerUri(), 
                                   aClientUid );

        
    // Check if the node already exists in RAM cache or in the db.
    // Also, check the node type. This function replaces old node
    // in RAM cache (not in db) if the node type and purpose do
    // not match with the ones gotten from the parser.
    CNcdNode& node( CheckAndCreateNodeL( nodeType, 
                                         aNodePurpose,
                                         aParentNodeIdentifier,
                                         *metaIdentifier ) );



    if ( aCreateParent ||
         !aParentNodeIdentifier.ContainsEmptyFields() &&
         NodePtrL( aParentNodeIdentifier ) ) 
        {        
        // Insert the node to the parent if it does not already exist there.
        // Note that here we insert it into the actual parent. Not into the parent
        // that is set for the proxy parent.
        // Note that the node link is internalized after the node has been inserted into the
        // parent. This way, the link will finally contain the correct info, for example
        // the correct parent info.
        // Make sure that the parent node type is correct in case root is used
        AddToParentL( aParentNodeIdentifier, 
                      *metaIdentifier, 
                      aParentNodeType, 
                      aParentNodePurpose, 
                      aNodePurpose, 
                      aMode, 
                      aIndex );
        DLINFO(("AddToParentL done, aData: %X", &aData));
        }

    // Metaid is not needed anymore. So, delete it.
    CleanupStack::PopAndDestroy( metaIdentifier );
    metaIdentifier = NULL;

    // Internalize data to the link of the node
    DLINFO(("Internalize link data"));
    
    // Note that the parent identifier should be checked if the parent is transparent in
    // other words if this node should be child of transparent.
    // Then the actual parent for the proxy transmission should be set to be the
    // grand parent node.
    if ( aNodePurpose == CNcdNodeFactory::ENcdChildOfTransparentNode )
        {
        DLINFO(("Transparent child. Get parent of parent"));
        // Parse the correct grandparent identifier because the proxy side will get
        // the grandparent identifier for the parent identifier info in case of transparent
        // nodes.
        CNcdNodeIdentifier* grandParentIdentifier( 
            NcdNodeIdentifierEditor::ParentOfLC( aParentNodeIdentifier ) );
        node.InternalizeLinkL( 
            aData, aParentNodeIdentifier, *grandParentIdentifier, aClientUid );
        CleanupStack::PopAndDestroy( grandParentIdentifier );
        }
    else 
        {
        node.InternalizeLinkL( 
            aData, aParentNodeIdentifier, aParentNodeIdentifier, aClientUid );
        }

    // Metadata of the node may exists in cache at this point already, but it is not
    // installed to the node, because otherwise the state of the node would be
    // "initialized" and UI would not reload the metadata. This is a problem if
    // user wants to force refresh level3 view.
    
    // Save the node now after it has been inserted into its parent.
    // Because the node has been updated and contains at least new
    // link data, we should save the new data to the database.
    DLINFO(("Save node"));

    DbSaveNodeL( node );
    DPROFILING_END( x );
    DLTRACEOUT((""));
    
    return node;
    }


CNcdNode& CNcdNodeManager::DataHandlerL( 
    const CNcdNodeIdentifier& aParentNodeIdentifier,
    MNcdPreminetProtocolDataEntity& aData,
    const TUid& aClientUid )
    {
    DLTRACEIN((_L("parent ns: %S, id: %S, data ns: %S, id: %S, name: %S"), 
                &aParentNodeIdentifier.NodeNameSpace(), &aParentNodeIdentifier.NodeId(), 
                &aData.Namespace(), &aData.Id(),&aData.Name()));
    DPROFILING_BEGIN( x );
    // Create the metadata according to the entity information
    
    // Notice:
    // The data entity namespace information is conditional accroding to the protocol. 
    // But the parser should always set the correct namespace. The parent node namespace
    // can not be used here because the parent may be in different namespace than
    // the child. At least if the root is used.

    // Notice that if the parent is for example root,
    // then the parent server uri is KNullDesC which is not the right server.
    // Use the serveruri that is gotten from the aData interface object.
    // Also, notice that aClientUid is used here. Because in some special cases,
    // the parent node identifier may be empty.
    CNcdNodeIdentifier* metaId = 
        CNcdNodeIdentifier::NewLC( aData.Namespace(), 
                                   aData.Id(), 
                                   aData.ServerUri(),
                                   aClientUid );

    // The node should always exist in the database if metadata exist.
    CNcdNode& node( NodeL( aParentNodeIdentifier, *metaId ) );


    DLINFO(("Create metadata"));
    CNcdNodeFactory::TNcdNodeType metaType( CNcdNodeFactory::ENcdNodeItem );
    if ( aData.Type() == EFolderEntity )
        {
        metaType = CNcdNodeFactory::ENcdNodeFolder;
        }
    else if ( aData.Type() != EItemEntity )
        {
        DLERROR(("Wrong entity type"));
        DASSERT( EFalse );
        User::Leave( KErrArgument );
        }
    
    // Checks that the old metadata is of the right type if it already
    // exists. If the types do not match, then the old metadata in 
    // RAM cache is replaced by the new metadata object that is of the
    // correct type. Notice, that the db is not updated by this function call.    
    CNcdNodeMetaData& metaData( 
        CheckAndCreateMetaDataL( *metaId, metaType ) );    
    
    CleanupStack::PopAndDestroy( metaId );

    DLINFO(("Internalize meta"));

    // Set all the data from the entity to the metadata itself
    metaData.InternalizeL( aData );

    DLINFO(( "Set meta for the node"));

    // Insert the metadata information to the node. Metadata's timestamp
    // is also updated to link in here
    node.SetNodeMetaDataL( metaData );
    
    DLINFO(("Save meta to db"));        
    
    // Do not save the node here, because it already contains uptodate info
    // or if it was just created, then nothing worth saving.
    // Just save the metadata to the database,
    // because it contains new info.
    DbSaveNodeMetaDataL( metaData );
    DPROFILING_END( x );
    DLTRACEOUT((""));
    
    return node;
    }


void CNcdNodeManager::PreviewHandlerL( const CNcdNodeIdentifier& aNodeIdentifier,
                                       const TDesC& aFileName,
                                       TInt aIndex,
                                       const TDesC& aMimeType )
    {
    DLTRACEIN(("Check old meta from the cache"));
    
    CNcdNode& node( NodeL( aNodeIdentifier ) );    
    CNcdNodeMetaData& metadata( node.NodeMetaDataL() );
    CNcdNodePreview& preview( metadata.PreviewL() );

    iPreviewManager->AddPreviewL( metadata.Identifier(), 
                                  preview.Uri( aIndex ), 
                                  aFileName,
                                  aMimeType );

    // Updates aMimeType to CNcdNodePreview if the MIME type
    // was not received in protocol responses
    preview.UpdateMimesFromPreviewManagerL();
    
    // Notice that the metadata does not need to be saved after this
    // because the preview manager handles all the necessary info.
    
    DLTRACEOUT((""));
    }


void CNcdNodeManager::PurchaseHandlerL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN(("Update installation information to the node"));
    
    CNcdNode& node( NodeL( aNodeIdentifier ) );
    CNcdNodeMetaData& metadata( node.NodeMetaDataL() );

    CNcdPurchaseDetails* details = metadata.PurchaseDetailsLC();

    // Try to internalize URI content from purchase history
    metadata.InternalizeUriContentL( *details );                    
     
    // Try to internalize node download from purchase history.
    metadata.InternalizeDownloadL( *details );
    
    metadata.InternalizeInstallFromContentInfoL();
    
    // Try to internalize node install from purchase history
    metadata.InternalizeInstallL( *details );    
    
    CleanupStack::PopAndDestroy( details );


    // Notice that the metadata does not need to be saved after this
    // because the purchase history contains all the necessary info.
            
    DLTRACEOUT((""));
    }


void CNcdNodeManager::DownloadDataHandlerL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));

    CNcdNode& node( NodeL( aNodeIdentifier ) );
    CNcdNodeMetaData& metadata( node.NodeMetaDataL() );


    CNcdPurchaseDetails* details = metadata.PurchaseDetailsLC();
        
    metadata.InternalizeDependencyL( *details );
     
    // Try to internalize node download from purchase history.
    metadata.InternalizeDownloadL( *details );
    
    metadata.InternalizeInstallFromContentInfoL();
    
    // Try to internalize node install from purchase history
    metadata.InternalizeInstallL( *details );    
    
    CleanupStack::PopAndDestroy( details );

    // Notice that the metadata does not need to be saved after this
    // because the purchase history contains all the necessary info.
            
    DLTRACEOUT((""));
    }


void CNcdNodeManager::InstallHandlerL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN(("Update installation information to the node"));
    
    CNcdNode& node( NodeL( aNodeIdentifier ) );
    CNcdNodeMetaData& metadata( node.NodeMetaDataL() );
        
    CNcdPurchaseDetails* details = metadata.PurchaseDetailsLC();
        
    metadata.InternalizeDependencyL( *details );
    
    // Try to internalize node download from purchase history.
    metadata.InternalizeDownloadL( *details );
    
    metadata.InternalizeInstallFromContentInfoL();
    
    // Try to internalize node install from purchase history
    metadata.InternalizeInstallL( *details );    
    
    metadata.HandleContentUpgradeL();
    
    CleanupStack::PopAndDestroy( details );

    // Notice that the metadata does not need to be saved after this
    // because the purchase history contains all the necessary info.
            
    DLTRACEOUT((""));
    }


void CNcdNodeManager::SetNodeExpiredL(
    const CNcdNodeIdentifier& aNodeIdentifier,
    TBool aRecursive,
    TBool aForceUpdate,
    RPointerArray<CNcdExpiredNode>& aFoundNodes )
    {
    DLTRACEIN((""));

    CNcdNode* node( NULL );
    TRAPD( err, node = &NodeL( aNodeIdentifier ) );
    if ( err == KErrNotFound )
        {
        // No need to do anything if node was not found.
        return;
        }
    else if ( err != KErrNone )
        {
        User::Leave( err );
        }
        
    SetNodeExpiredL( *node, aRecursive, aForceUpdate, aFoundNodes );
    DLTRACEOUT((""));
    }

void CNcdNodeManager::SetNodeExpiredL(
    CNcdNode& aNode,
    TBool aRecursive,
    TBool aForceUpdate,
    RPointerArray<CNcdExpiredNode>& aFoundNodes )
    {
    DLTRACEIN((""));
    CNcdNodeLink& nodeLink = aNode.CreateAndSetLinkL();
    nodeLink.SetValidUntilDelta( 0 );
    DASSERT( nodeLink.IsExpired() );
    DbSaveNodeL( aNode );
    aFoundNodes.AppendL( CNcdExpiredNode::NewL( aNode.Identifier(), aForceUpdate ) );
    
    if ( aRecursive 
         && ( aNode.ClassId() == NcdNodeClassIds::ENcdFolderNodeClassId
              || aNode.ClassId() == NcdNodeClassIds::ENcdSearchFolderNodeClassId
              || aNode.ClassId() == NcdNodeClassIds::ENcdRootNodeClassId
              || aNode.ClassId() == NcdNodeClassIds::ENcdBundleFolderNodeClassId 
              || aNode.ClassId() == NcdNodeClassIds::ENcdTransparentFolderNodeClassId 
              || aNode.ClassId() == NcdNodeClassIds::ENcdSearchBundleNodeClassId ) ) 
        {
        CNcdNodeFolder& folder = static_cast<CNcdNodeFolder&>( aNode );
        const RPointerArray<CNcdChildEntity>& children = folder.ChildArray();
        for ( TInt i = 0; i < children.Count(); i++ )
            {
            SetNodeExpiredL( children[i]->Identifier(), ETrue, aForceUpdate, aFoundNodes );
            }
        }
    DLTRACEOUT((""));
    }

void CNcdNodeManager::SetNodesExpiredByMetadataL(
    const MNcdPreminetProtocolExpiredCachedData& aData,
    const TUid& aClientUid,
    const TDesC& aNameSpace,
    RPointerArray<CNcdExpiredNode>& aFoundNodes )
    {
    DLTRACEIN((""));
    
    RPointerArray<CNcdNodeIdentifier> identifiers;
    CleanupResetAndDestroyPushL( identifiers );
    CDesCArrayFlat* nameSpaces = new (ELeave) CDesCArrayFlat( KListGranularity );
    CleanupStack::PushL( nameSpaces );
    RArray<NcdNodeClassIds::TNcdNodeClassType> types;
    CleanupClosePushL( types );
    types.AppendL( NcdNodeClassIds::ENcdNode );
    iDbManager->GetAllClientItemIdentifiersL( identifiers, aClientUid,
        *nameSpaces, types );
        
    for ( TInt i = 0 ; i < identifiers.Count() ; i++ )
        {
        for ( TInt j = 0 ; j < aData.ExpiredEntityCount() ; j++ )
            {
            if ( NcdNodeIdentifierEditor::DoesMetaDataIdentifierMatchL(
                *identifiers[i],
                aData.ExpiredEntityL( j ).EntityId(),
                aNameSpace,
                aClientUid ) )
                {
                // need to create a temp identifier because identifiers
                // from db have encoded namespace
                CNcdNodeIdentifier* realIdentifier = CNcdNodeIdentifier::NewLC(
                    aNameSpace,
                    identifiers[i]->NodeId(),
                    identifiers[i]->ClientUid() );
                SetNodeExpiredL( *realIdentifier,
                    aData.ExpiredEntityL( j ).Recursive(),
                    aData.ExpiredEntityL( j ).ForceUpdate(),
                    aFoundNodes );
                CleanupStack::PopAndDestroy( realIdentifier );
                break;
                }
            }
        }
        
    CleanupStack::PopAndDestroy( &types );
    CleanupStack::PopAndDestroy( nameSpaces );
    CleanupStack::PopAndDestroy( &identifiers );
    }


void CNcdNodeManager::RemoveNodeL( const CNcdNodeIdentifier& aParentIdentifier,
                                   const CNcdNodeIdentifier& aNodeMetaDataIdentifier ) 
    {
    DLTRACEIN((""));
    CNcdNodeIdentifier* identifier =
        NcdNodeIdentifierEditor::CreateNodeIdentifierLC( aParentIdentifier,
                                                         aNodeMetaDataIdentifier );
    RemoveNodeL( *identifier );
    CleanupStack::PopAndDestroy( identifier );
    }

void CNcdNodeManager::RemoveNodeL( const CNcdNodeIdentifier& aIdentifier ) 
    {
    DLTRACEIN((""));
    CNcdNode* node = NodePtrL( aIdentifier );
    if ( node == NULL ) 
        {
        DLINFO(("Node did not exist"));
        return;
        }
            
    CNcdNodeLink& nodeLink = node->CreateAndSetLinkL();
    const CNcdNodeIdentifier& parentId = nodeLink.ParentIdentifier();
    
    CNcdNodeFolder* parentFolder = NULL;
    // Parent node doesn't necessarily exists, eg. "floating" favorite nodes
    // If the parent node identifier is empty, then FolderL will leave
    // with KErrArgument
    TRAPD( err, parentFolder = &FolderL( parentId ) );
    LeaveIfNotErrorL( err, KErrNotFound, KErrArgument );
    
    NodeCacheCleanerManager().
        CacheCleanerL( 
            aIdentifier.ClientUid() ).
                AddCleanupIdentifierL( aIdentifier );
    
    if ( parentFolder ) 
        {
        DLTRACE(("Removing from parent"));
        // NOTE: After next function aIdentifier may not be valid anymore as
        // node owning it has been deleted.
        parentFolder->RemoveChild( aIdentifier );
        DbSaveNodeL( *parentFolder );
        }
    }
    

void CNcdNodeManager::ClearSearchResultsL( const MCatalogsContext& aContext )
    {
    DLTRACEIN((""));
    
    CNcdNodeFolder& searchRoot = CreateSearchRootL( aContext );
    
    // Maybe this should be recursive?
    const RPointerArray<CNcdChildEntity>& children = searchRoot.ChildArray();
    for (TInt i = 0 ; i < children.Count() ; i++ )
        {
        const CNcdNodeIdentifier& child = children[i]->Identifier();
        CNcdNodeFolder* childFolder = NULL;
        TRAPD(err, childFolder = &FolderL( child ) );
        if( err == KErrNone )
            {
            childFolder->RemoveChildrenL();            
            }
        }
    searchRoot.RemoveChildrenL();  
         
    }
    
    
void CNcdNodeManager::BackupAndClearCacheL(
    const CNcdNodeIdentifier& aRootNode )
    {
    DLTRACEIN((_L("aRootNode ns: %S, id: %S"), &aRootNode.NodeNameSpace(), &aRootNode.NodeId()));

    CNcdNode* root = NodePtrL( aRootNode );
   
    // Remove children from the list of root node.
    if ( root ) 
        {
        CNcdNodeFolder* rootFolder = static_cast<CNcdNodeFolder*>( root );
        // Note that RemoveChildren() doesn't remove nodes from the database like
        // RemoveChildrenL()        
        rootFolder->RemoveChildren();
        }    
    
    
    // Move all the children of the root node ( recursively ).
    for ( TInt i = iNodeCache.Count() - 1; i >= 0; i-- ) 
        {
        CNcdNode* node = iNodeCache[i];
        if ( NcdNodeIdentifierEditor::ParentOf( aRootNode, node->Identifier() ) ) 
            {

            if ( CNcdNodeFactory::NodeTypeL( *node ) == CNcdNodeFactory::ENcdNodeFolder )
                {
                DLTRACE(("Clearing child list"));
                
                CNcdNodeFolder* folder = static_cast<CNcdNodeFolder*>( node );
                // Do not use RemoveChildrenL method, since it saves the parent node to
                // db --> original child list is lost, reverting is impossible
                folder->RemoveChildren();
                }
                
            User::LeaveIfError( InsertNodeInOrder( node, iTempNodeCache ) );
            DLINFO((_L("Moved node: %S, %S"), &node->Identifier().NodeNameSpace(), &node->Identifier().NodeId() ));                
            iNodeCache.Remove( i );
            }
        }          
        
    }

    
void CNcdNodeManager::RevertNodeCacheL(
    const CNcdNodeIdentifier& aRootNode ) 
    {
    DLTRACEIN((""));
    
    RPointerArray<CNcdNode> schemeNodes;
    
    // Remove all the children of the given root from the main cache.
    for ( TInt i = iNodeCache.Count() - 1; i >= 0; i-- ) 
        {
        CNcdNode* node = iNodeCache[i];
        if ( NcdNodeIdentifierEditor::ParentOf( aRootNode, node->Identifier() ) ) 
            {
            node->Close();
            iNodeCache.Remove( i );
            }
        }
        
    // Move the nodes from temp cache to main RAM cache
    for ( TInt i = iTempNodeCache.Count() - 1; i >= 0; i-- ) 
        {
        CNcdNode* node = iTempNodeCache[i];
        if ( NcdNodeIdentifierEditor::ParentOf( aRootNode, node->Identifier() ) ) 
            {
            User::LeaveIfError( InsertNodeInOrder( node, iNodeCache ) );
            iTempNodeCache.Remove( i );
            }
        }
        
    // Internalize the root and its children from database to get correct child lists.
    for ( TInt i = 0; i < iNodeCache.Count(); i++ ) 
        {
        CNcdNode* node = iNodeCache[ i ];
        if ( NcdNodeIdentifierEditor::ParentOf( aRootNode, node->Identifier() ) ||
             node->Identifier().Equals( aRootNode ) ) 
            {
            HBufC8* data( 
                NodeDbManager().ReadDataFromDatabaseLC(
                    node->Identifier(), NcdNodeClassIds::ENcdNode ) );
            if ( data != NULL && data->Length() > 0 ) 
                {
                NodeFactory().InternalizeNodeL( *node, *data );
                }
            else 
                {
                // Data was not in database, clear the child list.
                CNcdNodeFactory::TNcdNodeType nodeType = CNcdNodeFactory::NodeTypeL( *node );
                if ( nodeType == CNcdNodeFactory::ENcdNodeFolder || 
                     nodeType == CNcdNodeFactory::ENcdNodeRoot ) 
                    {
                    CNcdNodeFolder* folder = static_cast<CNcdNodeFolder*>( node );
                    folder->RemoveChildrenL();
                    }                                
                }
            CleanupStack::PopAndDestroy( data );
            }
        }
    }        

void CNcdNodeManager::RevertNodeFromTempCacheL( const CNcdNodeIdentifier& aNodeIdentifier ) 
    {
    DLTRACEIN((""));
    // Find the node from temp cache
    TInt index = FindNodeFromArray( aNodeIdentifier, iTempNodeCache );
    
    if ( index != KErrNotFound ) 
        {
        CNcdNode* nodeFromMainCache = FindNodeFromMainCache( aNodeIdentifier );
        if ( nodeFromMainCache ) 
            {
            DASSERT( nodeFromMainCache == iTempNodeCache[ index ] );
            DLINFO(("No need to revert, the node was in main cache already"));            
            iTempNodeCache[ index ]->Close();
            }
        else 
            {            
            User::LeaveIfError( InsertNodeInOrder( iTempNodeCache[ index ], iNodeCache ) );            
            }
            
        iTempNodeCache.Remove( index );
        }
    }
    
void CNcdNodeManager::ClearTempCacheL( const CNcdNodeIdentifier& aRootNode ) 
    {
    DLTRACEIN((""));
    for ( TInt i = iTempNodeCache.Count() - 1; i >= 0; i-- ) 
        {
        CNcdNode* node = iTempNodeCache[i];
        if ( NcdNodeIdentifierEditor::ParentOf( aRootNode, node->Identifier() ) ) 
            {
            
            // Also remove from db if not present in main cache.
            if ( FindNodeFromMainCache( node->Identifier() ) == NULL ) 
                {
                RemoveNodeL( node->Identifier() );
                }
                
            iTempNodeCache.Remove( i );
            node->Close();
            }
        }
    }

// ---------------------------------------------------------------------------
// Cache cleanup functions.
// These functions are called by the node objects when
// they are released by the corresponding proxy object.
// ---------------------------------------------------------------------------

void CNcdNodeManager::NodeReleased( CNcdNode& aNode )
    {
    if ( aNode.AccessCount() == 1 )
        {
        // We need to release the node immediately if its metadata needs to be
        // released as soon as possible
        CNcdNodeMetaData* metadata = aNode.NodeMetaData();
        if ( metadata && 
             metadata->DeleteSoon() )
            {
            DLTRACE(( _L("Remove node from cache: %S"), 
                &aNode.Identifier().NodeId() ));
            
            if ( iClientDatabaseLocks.Find(
                aNode.Identifier().ClientUid().iUid ) == KErrNotFound )
                {                
                // Because this node will not be in the cache anymore, we can remove
                // it from the block list of the cleaner if it exists there. 
                TRAP_IGNORE(
                    NodeCacheCleanerManager().
                        CacheCleanerL( aNode.Identifier().ClientUid() ).
                            RemoveDoNotRemoveIdentifierL( aNode.Identifier() ) );        
                }
                
            TInt index = iNodeCache.Find( &aNode );
            if ( index != KErrNotFound ) 
                {                
                // Remove unreferenced node from the cache and destroy it.
                iNodeCache.Remove( index );
                }
            
            aNode.Close();
            
            // Ensure that metadata is released if possible
            MetaDataReleased( *metadata );
            }
            
        // Node is ready for closing.
        NodeCacheCleanup();
        }
    }
    
    
void CNcdNodeManager::MetaDataReleased( CNcdNodeMetaData& aMetaData )
    {
    DLTRACEIN((""));
    if ( aMetaData.AccessCount() == 1 )
        {
        if ( aMetaData.DeleteSoon() && !IsMetadataUsed( &aMetaData ) ) 
            {            
            DLTRACE(("Metadata wants to be released as soon as possible"));
            TInt index = iNodeMetaDataCache.Find( &aMetaData );
            DASSERT( index != KErrNotFound );
            aMetaData.Close();            

            iNodeMetaDataCache.Remove( index );            
            }
            
        // Metadata is ready for closing.
        MetaDataCacheCleanup();
        }
    }


void CNcdNodeManager::ClearClientCacheL( 
    const MCatalogsContext& aContext,
    TBool aClearDownloads )
    {    
    DLTRACEIN(("Clearing previews"));        
    
    TRAP_IGNORE( iPreviewManager->RemoveAllPreviewsL() );
        
    DLTRACE(("Close cached objects"));
    FullCacheCleanup();
        
    DLTRACE(("Clearing nodes and icons"));
    
    // Clear namespaces that contain favorites so that favorites are
    // left intact. Also checks that favorite nodes can be loaded from db
    TRAPD( err, ClearNamespacesWithFavoritesL( aContext.FamilyId() ) );
            
    // Now clear rest of the namespaces (except subscriptions)
    // Favorites are deleted if they are corrupted
    ClearClientNamespacesL( 
        aContext.FamilyId(), 
        err != KErrNone, 
        aClearDownloads );
    
    // Clear seen info.
    iSeenInfo->ClearInfoL( aContext.FamilyId() );
    
    // Unset previously loaded flag to prevent all children from being interpreted
    // as new on next refresh.
    CreateRootL( aContext.FamilyId() ).SetChildrenPreviouslyLoaded( EFalse );
    
    DLTRACEOUT(("All cleared"));
    }


void CNcdNodeManager::ClearClientNamespacesL( 
    const TUid& aClientUid, 
    TBool aClearFavorites,
    TBool aClearDownloads )
    {
    DLTRACEIN((""));
    // Insert the subscription namespace into the skip array. Because
    // subscriptions should not be deleted from the db.
    CPtrCArray* doNotCleanNameSpaces = 
        new(ELeave) CPtrCArray( KListGranularity );
    CleanupStack::PushL( doNotCleanNameSpaces );
    
    doNotCleanNameSpaces->AppendL( 
        NcdProviderDefines::KSubscriptionNamespace() );
    
    doNotCleanNameSpaces->AppendL(
        NcdProviderDefines::KProviderStorageNamespace() );
    
    if ( !aClearDownloads )
        {
        DLTRACE(("Do not delete downloads"));
        doNotCleanNameSpaces->AppendL( 
            NcdProviderDefines::KDownloadNamespace() );
        
        doNotCleanNameSpaces->AppendL( 
            NcdProviderDefines::KDataNamespace() );
        }
    
    if ( !aClearFavorites ) 
        {        
        // Insert the namespaces from which nodes are in 
        // favorites list to skip list.
        const RPointerArray<CNcdNodeIdentifier>& favorites = 
            iFavoriteManager->FavoriteNodesL( aClientUid );
        TInt favoriteCount = favorites.Count();
        TInt count = 0;
        for ( TInt i = 0; i < favoriteCount; i++ ) 
            {
            const TDesC& nameSpace = favorites[ i ]->NodeNameSpace();
            
            count = doNotCleanNameSpaces->MdcaCount();
            while( count-- ) 
                {
                if ( doNotCleanNameSpaces->MdcaPoint( count ) != nameSpace ) 
                    {
                    doNotCleanNameSpaces->AppendL( nameSpace );
                    break;
                    }
                }
            }        
        }
    else // favorites need to be removed from the favorite manager too
        {        
        iFavoriteManager->RemoveFavoritesL( aClientUid );
        }

    NodeDbManager().ClearClientL( aClientUid, *doNotCleanNameSpaces );
    CleanupStack::PopAndDestroy( doNotCleanNameSpaces );    
    }


void CNcdNodeManager::ClearNamespacesWithFavoritesL( const TUid& aClientUid ) 
    {
    DLTRACEIN((""));
    const RPointerArray<CNcdNodeIdentifier>& favorites( 
        iFavoriteManager->FavoriteNodesL( aClientUid ) );
    
    if ( !favorites.Count() ) 
        {
        DLTRACEOUT(("No favorites so nothing to do"));
        return;
        }
        
    // order by uid & namespace
    TLinearOrder<CNcdNodeIdentifier> sort( 
        CNcdNodeIdentifier::CompareOrderByUid );
    RPointerArray<CNcdNodeIdentifier> sortedArray;
    
    // array won't own the identifiers so just close
    CleanupClosePushL( sortedArray );
    sortedArray.ReserveL( favorites.Count() );
        
    TInt count = favorites.Count();
    DLTRACE(("Sorting %d favorites", count));
    while ( count-- ) 
        {
        sortedArray.InsertInOrderL( favorites[ count ], sort );
        }
    
    count = sortedArray.Count();
    TInt index = 0;
    // iterate through favorites one namespace at a time
    while ( index < count )
        {        
        index = ClearNamespaceL( sortedArray, index );
        }    
    
    CleanupStack::PopAndDestroy( &sortedArray );    
    
    DLTRACEOUT((""));
    }
    

TInt CNcdNodeManager::ClearNamespaceL( 
    const RPointerArray<CNcdNodeIdentifier>& aSortedArray, 
    TInt aIndex )
    {
    DLTRACEIN((""));
    TPtrC currentNamespace( aSortedArray[ aIndex ]->NodeNameSpace() );

    // Array for metadata id's that must not be removed
    CDesCArrayFlat* doNotRemoveMetadataArray = 
        new( ELeave ) CDesCArrayFlat( KListGranularity );
        
    // owns the array
    RNcdDatabaseItems unremovableMetadata(         
        KErrNotFound, // ignores type so all types that match the id are left
        doNotRemoveMetadataArray );
    CleanupClosePushL( unremovableMetadata );

    // Array for node id's that must not be removed
    CPtrCArray* doNotRemoveArray = 
        new( ELeave ) CPtrCArray( KListGranularity );

    // owns the array
    RNcdDatabaseItems unremovableItem( 
        NcdNodeClassIds::ENcdNode,
        doNotRemoveArray );
    
    CleanupClosePushL( unremovableItem );

    // Array for icon id's that must not be removed
    CDesCArrayFlat* doNotRemoveIconArray = 
        new( ELeave ) CDesCArrayFlat( KListGranularity );

    // owns the array
    RNcdDatabaseItems unremovableIcon( 
        NcdNodeClassIds::ENcdIconData,
        doNotRemoveIconArray );
    
    CleanupClosePushL( unremovableIcon );

    TInt pos = 0; // needed for CDesCArray::Find
    TInt err = KErrNone;            
    
    TInt index = aIndex; 
    TInt count = aSortedArray.Count();
    // Generate array of id's that must not be removed from current namespace    
    while( index < count && 
           aSortedArray[ index ]->NodeNameSpace() == currentNamespace )          
        {
        doNotRemoveArray->AppendL( aSortedArray[ index ]->NodeId() );                              

        CNcdNodeIdentifier* metaId = 
            NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( 
                *aSortedArray[ index ] );
    
        if ( doNotRemoveMetadataArray->Find( 
                metaId->NodeId(), pos, ECmpNormal ) )
            {            
            doNotRemoveMetadataArray->AppendL( metaId->NodeId() );
            
            const CNcdNodeMetaData* metadata( NULL );
            TRAP( err, metadata = &NodeMetaDataL( *metaId ) );
            LeaveIfNotErrorL( err, KErrNotFound );
        
            if ( metadata ) 
                {                                                                       
                // Icons are checked only when new metadata is added
                // IconL leaves only with KErrNotFound
                CNcdNodeIcon* icon( NULL );
                TRAP_IGNORE( icon = &metadata->IconL() );
                
                if ( icon && 
                     doNotRemoveIconArray->Find( 
                        icon->IconId(), pos, ECmpNormal ) ) 
                    {
                    DLTRACE(("Adding icon id to do not remove list"));
                    doNotRemoveIconArray->AppendL( icon->IconId() );
                    }

                // close the opened metadata    
                CloseMetadatas();
                }
            }
        CleanupStack::PopAndDestroy( metaId );          
        ++index;  
        }

    RArray<RNcdDatabaseItems> items;
    CleanupClosePushL( items );
    
    // note: ownership is not transferred
    items.AppendL( unremovableItem );
    items.AppendL( unremovableMetadata );
    items.AppendL( unremovableIcon );
    
    DLTRACE(("Removing from database"));
    // commits and compacts data     
    NodeDbManager().RemoveDataFromDatabaseL( 
        *aSortedArray[ index - 1 ], items );
            
    CleanupStack::PopAndDestroy( &items );
    CleanupStack::PopAndDestroy( &unremovableIcon );
    CleanupStack::PopAndDestroy( &unremovableItem );
    CleanupStack::PopAndDestroy( &unremovableMetadata );    
    
    CheckNodesL( aSortedArray, aIndex, index );
    return index;
    }


void CNcdNodeManager::CheckNodesL( 
    const RPointerArray<CNcdNodeIdentifier>& aNodeIds,
    TInt aStart,
    TInt aEnd )
    {
    DLTRACEIN(("aStart: %d, aEnd: %d", aStart, aEnd ));
    for ( ; aStart < aEnd; ++aStart ) 
        {        
        CNcdNode& node = NodeL( *aNodeIds[ aStart ] );
        node.NodeMetaDataL();
        FullCacheCleanup();
        }
    }

// ---------------------------------------------------------------------------
// Db functions that are needed from other class objects.
// The db functions are provided here instead of directly providing
// db manager because node manager may want to do some additional checking
// before db actions are allowed.
// ---------------------------------------------------------------------------


void CNcdNodeManager::DbRemoveNodeL( const CNcdNodeIdentifier& aIdentifier )
    {
    if ( iClientDatabaseLocks.Find( aIdentifier.ClientUid().iUid ) == KErrNotFound ) 
        {        
        NodeDbManager().RemoveDataFromDatabaseL( aIdentifier,
                                                 NcdNodeClassIds::ENcdNode );
        }
    }


void CNcdNodeManager::DbSaveUserDataL( const CNcdNodeIdentifier& aUserDataIdentifier,
                                       MNcdStorageDataItem& aDataItem )
    {
    NodeDbManager().SaveDataIntoDatabaseL( aUserDataIdentifier,
                                           aDataItem,
                                           NcdNodeClassIds::ENcdNodeUserData );    
    }

    
void CNcdNodeManager::DbRemoveUserDataL( const CNcdNodeIdentifier& aUserDataIdentifier )
    {
    NodeDbManager().RemoveDataFromDatabaseL( aUserDataIdentifier,
                                             NcdNodeClassIds::ENcdNodeUserData );     
    }

    
void CNcdNodeManager::DbLoadUserDataL( const CNcdNodeIdentifier& aUserDataIdentifier,
                                       MNcdStorageDataItem& aDataItem )
    {
    NodeDbManager().StartStorageLoadActionL( aUserDataIdentifier,
                                             aDataItem,
                                             NcdNodeClassIds::ENcdNodeUserData );
    }


HBufC8* CNcdNodeManager::DbScreenshotDataLC( const CNcdNodeIdentifier& aScreenshotIdentifier )
    {
     return NodeDbManager().ReadDataFromDatabaseLC( aScreenshotIdentifier,
                                                    NcdNodeClassIds::ENcdScreenshotData );     
    }


HBufC8* CNcdNodeManager::DbIconDataLC( const CNcdNodeIdentifier& aIconIdentifier )
    {
    DLTRACEIN((""));
     return NodeDbManager().ReadDataFromDatabaseLC( aIconIdentifier,
                                                    NcdNodeClassIds::ENcdIconData );     
    }


void CNcdNodeManager::DbSaveIconDataL( const CNcdNodeIdentifier& aIconIdentifier, 
                                       const TDesC8& aIconData )
    {
    DLTRACEIN((""));
    CNcdStorageDescriptorDataItem* iconDataItem = 
        CNcdStorageDescriptorDataItem::NewLC( aIconData );
    NodeDbManager().SaveDataIntoDatabaseL( aIconIdentifier, 
                                           *iconDataItem,
                                           NcdNodeClassIds::ENcdIconData );
    CleanupStack::PopAndDestroy( iconDataItem );

    // Because node data was saved into the db. Check if the max size has
    // been exceeded.
    // This is propably good place to do checking because icons are one of
    // the biggest datablobs that are saved into the db.
    NodeCacheCleanerManager().
        CacheCleanerL( aIconIdentifier.ClientUid() ).CheckDbSizeL();
    DLTRACEOUT(("Data saved successfully"));
    }



// ---------------------------------------------------------------------------
// General tool functions
// ---------------------------------------------------------------------------

void CNcdNodeManager::AddToParentL( const CNcdNodeIdentifier& aParentNodeIdentifier,
                                    const CNcdNodeIdentifier& aChildNodeMetaDataIdentifier,
                                    CNcdNodeFactory::TNcdNodeType aParentNodeType,
                                    CNcdNodeFactory::TNcdNodePurpose aParentNodePurpose,
                                    CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                    TNcdRefHandleMode aMode,
                                    TInt aIndex,
                                    TBool aTransparent )
    {
    DLTRACEIN(( _L("Child: %S::%S Parent %S::%S"),
              &aChildNodeMetaDataIdentifier.NodeNameSpace(), 
              &aChildNodeMetaDataIdentifier.NodeId(), 
              &aParentNodeIdentifier.NodeNameSpace(), 
              &aParentNodeIdentifier.NodeId() ));
    DLINFO(("Node purpose: %d", aNodePurpose));
    DPROFILING_BEGIN( x );    
    
    if ( aParentNodeIdentifier.ContainsEmptyFields() )
        {
        // In some situations the parent may be empty. For example when temporary
        // nodes are used. Because we do not create nodes with empty identifiers,
        // we do not try to add the child to one either here. 
        DLINFO(("Empty parent. Do not add child."));
        return;
        }

    if ( aParentNodeType != CNcdNodeFactory::ENcdNodeRoot
         && aParentNodeType != CNcdNodeFactory::ENcdNodeFolder
         && aParentNodeType != CNcdNodeFactory::ENcdNodeSearchBundle )
        {
        DLERROR(("Only root or folder can be a parent!"));
        DASSERT( EFalse );
        User::Leave( KErrArgument );
        }
        
    DLINFO(("Parent node purpose: %d", aParentNodePurpose));
    CNcdNodeFolder& folder( 
        static_cast<CNcdNodeFolder&>( 
            CheckAndCreateNodeL( aParentNodeType, 
                                 aParentNodePurpose,
                                 aParentNodeIdentifier ) ) );

    TBool childAdded = EFalse;
    
    CNcdNodeIdentifier* childIdentifier =
        NcdNodeIdentifierEditor::CreateNodeIdentifierLC( aParentNodeIdentifier,
                                                         aChildNodeMetaDataIdentifier );

    DASSERT( childIdentifier != NULL );    
    
    // Ensure that transparent nodes are set as transparent
    aTransparent = aTransparent || 
                 ( aNodePurpose == CNcdNodeFactory::ENcdTransparentNode ); 
                 
    CNcdNode* child = NodePtrL( *childIdentifier );          
    DASSERT( child != NULL );
    CNcdNodeFactory::TNcdNodeType nodeType = CNcdNodeFactory::NodeTypeL( *child );
    
    if( aMode == EInsert )
        {
        childAdded = folder.InsertChildL( *childIdentifier, aIndex,
            aTransparent, nodeType );
        }
    else if ( aMode == EReplace )
        {
        childAdded = folder.ReplaceChildL( *childIdentifier, aIndex,
            aTransparent, nodeType );
        }
    else if ( aMode == EAppend )
        {
        childAdded = folder.AppendChildL( *childIdentifier, aTransparent,
            nodeType );
        }
    else 
        {
        DASSERT( aMode == EUpdate );
        // Notice that here we do not set the childAdded value.
        // So, the parent node will not be saved below.
        // For example, when scheme nodes are loaded, they will be loaded by using the
        // EUpdate value, which means that the root node will not be saved into the
        // database and the scheme information is contained in root node only while
        // the root node is in RAM. 
        }

    if( childAdded )
        {
        DLINFO(( _L("Updating child: %S::%S to parent %S::%S in db"),
                  &childIdentifier->NodeNameSpace(), 
                  &childIdentifier->NodeId(), 
                  &aParentNodeIdentifier.NodeNameSpace(), 
                  &aParentNodeIdentifier.NodeId() ));       

        // Notice, that the RefHandler should Internalize the link after this operation
        // because the transparent folder children should have the different parent set for
        // requests than just the real parent.
        child->CreateAndSetLinkL().SetParentIdentifierL( aParentNodeIdentifier );
        
        
        // Check new status.
        iSeenInfo->CheckChildNewStatusL( folder, aIndex );
        
        // The child was added to the list because it did not exist there before.
        // This is reason enought to save the node the db. So, it is upto date.
        DbSaveNodeL( folder );
        DLINFO(("Parent updated"));
        }

    CleanupStack::PopAndDestroy( childIdentifier );
    DPROFILING_END( x );        
    DLTRACEOUT((""));
    }



// ---------------------------------------------------------------------------
// CCatalogsCommunicable
// ---------------------------------------------------------------------------

void CNcdNodeManager::ReceiveMessage( MCatalogsBaseMessage* aMessage,
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
        case NcdNodeFunctionIds::ENcdRootNodeHandle:
            DLINFO(("Getting root node"));
            TRAP( trapError, RootNodeRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdSearchRootNodeHandle:
            DLINFO(("Getting search root node"));
            TRAP( trapError, SearchRootNodeRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdNodeHandle:
            DLINFO(("Getting node"));
            TRAP( trapError, NodeRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdTemporaryNodeFolderHandle:
            DLINFO(("Getting node"));
            TRAP( trapError, TemporaryNodeRequestL( *aMessage, ENcdTemporaryNodeFolder, EFalse ) );
            break;

        case NcdNodeFunctionIds::ENcdTemporaryNodeFolderWithMetaDataHandle:
            DLINFO(("Getting node"));
            TRAP( trapError, TemporaryNodeRequestL( *aMessage, ENcdTemporaryNodeFolder, ETrue ) );
            break;

        case NcdNodeFunctionIds::ENcdTemporaryNodeItemHandle:
            DLINFO(("Getting node"));
            TRAP( trapError, TemporaryNodeRequestL( *aMessage, ENcdTemporaryNodeItem, EFalse ) );
            break;
            
        case NcdNodeFunctionIds::ENcdTemporaryNodeItemWithMetaDataHandle:
            DLINFO(("Getting node"));
            TRAP( trapError, TemporaryNodeRequestL( *aMessage, ENcdTemporaryNodeItem, ETrue ) );
            break;

        case NcdNodeFunctionIds::ENcdTemporaryBundleFolderHandle:
            DLINFO(("Getting bundle folder"));
            TRAP( trapError, TemporaryNodeRequestL( *aMessage, ENcdTemporaryBundleFolder, EFalse ) );
            break;
            
        case NcdNodeFunctionIds::ENcdTemporaryBundleFolderWithMetaDataHandle:
            DLINFO(("Getting bundle folder"));
            TRAP( trapError, TemporaryNodeRequestL( *aMessage, ENcdTemporaryBundleFolder, ETrue ) );
            break;

        case NcdNodeFunctionIds::ENcdCreateTemporaryOrSupplierNode:
            DLINFO(("Getting temporary or supplier node"));
            TRAP( trapError, TemporaryOrSupplierNodeRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdCreateTemporaryNodeIfMetadataExists:
            DLINFO(("Getting temporary node if metadata exists"));
            TRAP( trapError, TemporaryNodeIfMetadataExistsRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdRelease:
            ReleaseRequest( *aMessage );
            break;
            
        case NcdNodeFunctionIds::ENcdClearSearchResults:
            TRAP( trapError, ClearSearchResultsRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdIsCapabilitySupported:
            TRAP( trapError, IsCapabilitySupportedRequestL( *aMessage ));
            break;
            
        default:
            break;
        }

    if ( trapError != KErrNone )
        {
        // Because something went wrong the complete has not been
        // yet called for the message.
        // So, inform the client about the error.
        DLINFO(("ERROR, Complete and release %d", trapError));
        
        aMessage->CompleteAndRelease( trapError );
        }

    // Because the message should not be used after this, set it NULL.
    // So, CounterPartLost function will know that no messages are
    // waiting the response at the moment.
    iMessage = NULL;        
            
    DLTRACEOUT((""));
    }


void CNcdNodeManager::CounterPartLost( const MCatalogsSession& aSession )
    {
    DLTRACEIN((""));    
    CommitSeenChanges( const_cast<MCatalogsSession&>( aSession ).Context() );
    
    // This function may be called whenever -- when the message is waiting
    // response or when the message does not exist.
    // iMessage may be NULL here, because in the end of the
    // ReceiveMessage it is set to NULL. The life time of the message
    // ends shortly after CompleteAndRelease is called.
    if ( iMessage != NULL )
        {
        iMessage->CounterPartLost( aSession );
        }    

    DLTRACEOUT((""));    
    }


// ---------------------------------------------------------------------------
// Public:
// General getter and setter functions.
// ---------------------------------------------------------------------------

CNcdNodeDbManager& CNcdNodeManager::NodeDbManager() const
    {
    return *iDbManager;
    }
    
CNcdNodeSeenInfo& CNcdNodeManager::SeenInfo() const
    {
    return *iSeenInfo;
    }

CNcdNodeFactory& CNcdNodeManager::NodeFactory() const
    {
    return *iNodeFactory;
    }    

CNcdNodeCacheCleanerManager& CNcdNodeManager::NodeCacheCleanerManager() const
    {
    return *iNodeCacheCleanerManager;
    }    

void CNcdNodeManager::SetFavoriteManager( CNcdFavoriteManager& aManager ) 
    {
    iFavoriteManager = &aManager;
    }

void CNcdNodeManager::SetNodeMetaDataL( CNcdNode& aNode )
    {
    DLTRACEIN(("Check metadata for node"));
    CNcdNodeMetaData* metadata( NULL );
    CNcdNodeLink* link = aNode.NodeLink();    
    if ( link )
        {
        const CNcdNodeIdentifier& metaDataIdentifier( link->MetaDataIdentifier() );
        if ( !metaDataIdentifier.ContainsEmptyFields() )
            {
            TRAP_IGNORE(
                   metadata = &NodeMetaDataL( metaDataIdentifier ) );        
            if ( metadata != NULL )
                {
                DLINFO(("Setting metadata for the created node"));
                aNode.SetNodeMetaDataL( *metadata );            
                }                    
            }
        }    
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Protected:
// Db functions
// ---------------------------------------------------------------------------

CNcdNode& CNcdNodeManager::DbNodeL( const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN(( _L("Node id: %S, %S, %d"), 
                &aIdentifier.NodeNameSpace(),
                &aIdentifier.NodeId(),
                aIdentifier.ClientUid().iUid ));
    DPROFILING_BEGIN( x );
    // If the database is locked, leave immediately.
    if ( iClientDatabaseLocks.Find( aIdentifier.ClientUid().iUid ) != KErrNotFound ) 
        {
        DLINFO(("Database locked -> leave."));
        User::Leave( KErrNotFound );
        }

    HBufC8* data( 
        NodeDbManager().
            ReadDataFromDatabaseLC( aIdentifier,
                                    NcdNodeClassIds::ENcdNode ) );

    if ( data == NULL
         || data->Length() == 0 )
        {
        // Node was not found from db. So, leave.
        DLINFO(("Node was not found from db."));
        User::Leave( KErrNotFound );
        }

    // Create and internalize node according to the data gotten from the
    // database.
    CNcdNode* node( 
        NodeFactory().
            CreateNodeLC( aIdentifier,
                          *data ) );   
 
     // Insert node to the cache.
    AppendNodeToCacheL( node );        

    // Cache took ownership of the node.
    CleanupStack::Pop( node );

    // Delete the node data because new node has been created. 
    CleanupStack::PopAndDestroy( data );    

    // Now that the node has been created from the db, the metadata should
    // be also set if it can be found. If it can, it should be set when the
    // metadata is gotten from the internet and handled with DataHandlerL
    SetNodeMetaDataL( *node );    
    DPROFILING_END( x );
    DLTRACEOUT((""));    
    return *node;
    }
    
    
CNcdNodeMetaData& CNcdNodeManager::DbMetaDataL( const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN(( _L("Metadata id: %S, %S, %d"), 
                &aIdentifier.NodeNameSpace(),
                &aIdentifier.NodeId(),
                aIdentifier.ClientUid().iUid ));

    HBufC8* data( 
        NodeDbManager().
        ReadDataFromDatabaseLC( aIdentifier,
                                NcdNodeClassIds::ENcdMetaData ) );

    if ( data == NULL )
        {
        // Node was not found from db. So, leave.
        DLINFO(("Node metadata was not found from db"));
        User::Leave( KErrNotFound );
        }
    else if ( data->Length() == 0 )
        {
        // Node was not found from db. So, leave.
        DLINFO(("Node metadata from db was empty"));
        User::Leave( KErrNotFound );        
        }

    // Use factory to create and to initialize the correct node from the data.
    CNcdNodeMetaData* metaData( NodeFactory().CreateMetaDataLC( aIdentifier, *data ) );

    // Insert node to the cache.
    iNodeMetaDataCache.AppendL( metaData );
    
    // Cache took ownership of the node.
    CleanupStack::Pop( metaData );

    CleanupStack::PopAndDestroy( data );
        
    DLTRACEOUT((""));    
    return *metaData;    
    }


void CNcdNodeManager::DbSaveNodeL( CNcdNode& aNode )
    {
    DLTRACEIN((_L("Node id: %S, %S, %d"), 
               &aNode.Identifier().NodeNameSpace(),
               &aNode.Identifier().NodeId(),
               aNode.Identifier().ClientUid().iUid));
    DPROFILING_BEGIN( x );
    // If the database is locked, do not do anything.
    if ( iClientDatabaseLocks.Find( aNode.Identifier().ClientUid().iUid ) == 
         KErrNotFound ) 
        {
        // Save the node information to the database
        // The node implements MNcdStorageDataItem interface.
        // So, the externalize function will insert the data to the stream
        // and the database handler will save the stream to the database.
        NodeDbManager().SaveDataIntoDatabaseL( aNode.Identifier(), 
                                               aNode,
                                               NcdNodeClassIds::ENcdNode );
        }
        

    // Inform cache cleaner about the node saving.
    // Because the node has been updated into the db. Make sure
    // that it will not be in the cleanup list anymore.
    NodeCacheCleanerManager().
        CacheCleanerL( aNode.Identifier().ClientUid() ).
            RemoveCleanupIdentifier( aNode.Identifier() );
    DPROFILING_END( x );
    DLTRACEOUT((""));
    }
    
    
void CNcdNodeManager::DbSaveNodesL( const CNcdNodeIdentifier& aRootNode ) 
    {
    DLTRACEIN((""));
    
    for ( TInt i = 0; i < iNodeCache.Count(); i++ ) 
        {
        CNcdNode* node = iNodeCache[i];
        if ( NcdNodeIdentifierEditor::ParentOf( aRootNode, node->Identifier() ) ||
             node->Identifier().Equals( aRootNode ) )
            {
            DLINFO((_L("Saving node ns: %S, id: %S"), &node->Identifier().NodeNameSpace(), &node->Identifier().NodeId() ));
            DbSaveNodeL( *node );
            }
        }
    }


void CNcdNodeManager::DbSaveNodeMetaDataL( CNcdNodeMetaData& aMetaData )
    {
    DLTRACEIN(( _L("Metadata id: %S, %S, %d"), 
                &aMetaData.Identifier().NodeNameSpace(),
                &aMetaData.Identifier().NodeId(),
                aMetaData.Identifier().ClientUid().iUid ));
    DPROFILING_BEGIN( x );
    // Save the node information to the database
    // The node implements MNcdStorageDataItem interface.
    // So, the externalize function will insert the data to the stream
    // and the database handler will save the stream to the database.
    NodeDbManager().SaveDataIntoDatabaseL( aMetaData.Identifier(), 
                                           aMetaData,
                                           NcdNodeClassIds::ENcdMetaData );

    // Because node data was saved into the db. Check if the max size has
    // been exceeded.
    // This is propably good enough place to do checking because most of the
    // data is gotten from the metadata.
    NodeCacheCleanerManager().
        CacheCleanerL( aMetaData.Identifier().ClientUid() ).CheckDbSizeL();
    
    DPROFILING_END( x );    
    DLTRACEOUT((""));
    }


void CNcdNodeManager::DbSetMaxSizeL( const TUid& aClientUid, 
                                    const TInt aMaxDbKiloByteSize )
    {
    DLTRACEIN(("Setting max db size: %d for client: %d", 
               aMaxDbKiloByteSize, aClientUid));
    
    // The cache cleaner uses this information so forward the info for it.
    NodeCacheCleanerManager().
        CacheCleanerL(aClientUid).
            SetDbMaxSize( aMaxDbKiloByteSize * KBytesToKilos );

    DLTRACEOUT((""));
    }
    
void CNcdNodeManager::LockNodeDbL( TUid aClientUid ) 
    {
    DLTRACEIN((""));
    iClientDatabaseLocks.AppendL( aClientUid.iUid );
    }
    
void CNcdNodeManager::UnlockNodeDb( TUid aClientUid ) 
    {
    DLTRACEIN((""));
    TInt index = iClientDatabaseLocks.Find( aClientUid.iUid );
    if ( index != KErrNotFound ) 
        {
        iClientDatabaseLocks.Remove( index );
        }
    }

// ---------------------------------------------------------------------------
// Protected:
// Functions that are called from the ReceiveMessageL, 
// which is meant to be used by the client side.
// ---------------------------------------------------------------------------

void CNcdNodeManager::RootNodeRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    // Creates the root if does not exist yet.
    CNcdNodeFolder& root = CreateRootL( requestSession.Context() );

    // Add the node to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 rootHandle( requestSession.AddObjectL( &root ) );

    DLINFO(("Root handle: %d", rootHandle ));

    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( rootHandle, KErrNone );

    DLTRACEOUT((""));
    }

void CNcdNodeManager::SearchRootNodeRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    // Creates the search root if does not exist yet.
    CNcdNodeFolder& searchRoot = CreateSearchRootL( requestSession.Context() );

    // Add the node to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 searchRootHandle( requestSession.AddObjectL( &searchRoot ) );

    DLINFO(("Search root handle: %d", searchRootHandle ));

    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( searchRootHandle, KErrNone );

    DLTRACEOUT((""));
    }


void CNcdNodeManager::NodeRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    CNcdNodeIdentifier* nodeIdentifier( RequestNodeIdentifierLC( aMessage ) );

    // Get the node from the cache or from the db.
    // Notice that new node is not created if it is not found from the
    // cache or from the db.
    CNcdNode& node( NodeL( *nodeIdentifier ) );

    CleanupStack::PopAndDestroy( nodeIdentifier );
    
    DLINFO(("Node found"));

    // Add the node to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 nodeHandle( aMessage.Session().AddObjectL( &node ) );

    DLINFO(("Node handle: %d", nodeHandle ));

    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( nodeHandle, KErrNone );

    DLTRACEOUT((""));
    }
    

void CNcdNodeManager::TemporaryNodeRequestL(
    MCatalogsBaseMessage& aMessage,
    TNcdTemporaryNodeType aType,
    TBool aCreateMetaData )
    {
    DLTRACEIN((""));
    
    CNcdNodeIdentifier* nodeIdentifier( RequestNodeIdentifierLC( aMessage ) );

    CNcdNode* node = &CreateTemporaryNodeL( 
        *nodeIdentifier, 
        aType, 
        aCreateMetaData );
    
    CleanupStack::PopAndDestroy( nodeIdentifier );
    
    // Add the node to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 nodeHandle( aMessage.Session().AddObjectL( node ) );

    DLINFO(("Node handle: %d", nodeHandle ));

    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( nodeHandle, KErrNone );

    DLTRACEOUT((""));
    }


CNcdNode& CNcdNodeManager::CreateTemporaryNodeL(
    CNcdNodeIdentifier& aTempNodeIdentifier,
    TNcdTemporaryNodeType aType,
    TBool aCreateMetaData )
    {
    DLTRACEIN((""));
    // Check if the metadata exists already, and is of correct type.
    CNcdNodeIdentifier* metaIdentifier =
        NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( 
                aTempNodeIdentifier );
    
    CNcdNodeMetaData* meta( NULL );
    TRAPD( err, meta = &NodeMetaDataL( *metaIdentifier ) );
    if ( err == KErrNone ) 
        {
        // Metadata was found, check the type.
        NcdNodeClassIds::TNcdNodeClassId acceptedMetaClassId = 
            NcdNodeClassIds::ENcdNullObjectClassId;
            
        switch ( aType ) 
            {
            case ENcdTemporaryBundleFolder:
            case ENcdTemporaryNodeFolder:
                acceptedMetaClassId = NcdNodeClassIds::ENcdFolderNodeMetaDataClassId;
                break;
            
            case ENcdTemporaryNodeItem:
                acceptedMetaClassId = NcdNodeClassIds::ENcdItemNodeMetaDataClassId;
                break;
            
            default:
                DASSERT( EFalse );
                break;
            }
                
        if ( meta->ClassId() != acceptedMetaClassId ) 
            {
            DLINFO(("Wrong metadata type, leave"));
            User::Leave( KErrArgument );
            }
        }

            
    // Get the node from the cache or from the db.
    // Notice that new node is not created if it is not found from the
    // cache or from the db.
    CNcdNode* node( NULL );
    switch( aType )
        {
        case ENcdTemporaryNodeFolder:        
            node = &CreateNodeFolderL(
                CNcdNodeFactory::ENcdNormalNode, aTempNodeIdentifier );
            break;
            
        case ENcdTemporaryNodeItem:
            node = &CreateNodeItemL(
                CNcdNodeFactory::ENcdNormalNode, aTempNodeIdentifier );
            break;
        
        case ENcdTemporaryBundleFolder:
            node = &CreateNodeFolderL(
                CNcdNodeFactory::ENcdBundleNode, aTempNodeIdentifier );
            break;
        
        default:
            DASSERT( EFalse );
            break;
        }

    DASSERT( node );
    if (!node )
        {
        User::Leave( KErrGeneral );
        }
            
    // Because this is temporary node, we will create or get the metadata for the node.
    // This way the metadata will be at least initialized with the purchase history data.
    if ( !meta && aCreateMetaData ) 
        {
        switch( aType )
            {
            case ENcdTemporaryBundleFolder:
            case ENcdTemporaryNodeFolder:
                meta = &CreateNodeMetaDataL(
                    *metaIdentifier, CNcdNodeFactory::ENcdNodeFolder );
                break;
                
            case ENcdTemporaryNodeItem:
                meta = &CreateNodeMetaDataL(
                    *metaIdentifier, CNcdNodeFactory::ENcdNodeItem );
                break;

            default:
                DASSERT( EFalse );
                break;
            }
        }


    // Also to be sure that the node will be in initialized mode, we set the
    // link for the node here.

    // Set the server and metadata information for the link.
    // These are required, so the temp node may also be loaded from web.
    node->CreateAndSetLinkL().SetServerUriL( node->Identifier().ServerUri() );
    node->CreateAndSetLinkL().SetMetaDataIdentifierL( *metaIdentifier );

    CleanupStack::PopAndDestroy( metaIdentifier );    

    if ( meta )
        {
        // Set the metadata for the node.
        node->SetNodeMetaDataL( *meta );
        DLINFO(("Node and meta created"));
        }

    // Also, save the node now that it has been updated
    DbSaveNodeL( *node );
    return *node;
    }

    
void CNcdNodeManager::TemporaryOrSupplierNodeRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    // This will use the helper function to get the actual node identifier that is included
    // into the message data.
    CNcdNodeIdentifier* nodeIdentifier( RequestNodeIdentifierLC( aMessage ) );

    // The creator function requires metadata identifier in this case. 
    // So, create correct identifier here.    
    CNcdNodeIdentifier* metadataIdentifier( 
        NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( *nodeIdentifier ) );
    CNcdNode& node = CreateTemporaryNodeOrSupplierL( *metadataIdentifier );
    CleanupStack::PopAndDestroy( metadataIdentifier );

    CleanupStack::PopAndDestroy( nodeIdentifier );

    // NOTICE: We do not get the handle here, but the proxy side has to request it
    // separately by using the normal node request. This just created the node.    

    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    // Notice that we just return zero here, because we do not have anything 
    // special to give back.
    aMessage.CompleteAndReleaseL( 0, KErrNone );

    DLTRACEOUT((""));
    }
    

void CNcdNodeManager::TemporaryNodeIfMetadataExistsRequestL(
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    // This will use the helper function to get the actual node identifier that is included
    // into the message data.
    CNcdNodeIdentifier* nodeIdentifier( RequestNodeIdentifierLC( aMessage ) );

    // The creator function requires metadata identifier in this case. 
    // So, create correct identifier here.    
    CNcdNodeIdentifier* metadataIdentifier( 
        NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( *nodeIdentifier ) );
        
    CNcdNode* node = CreateTemporaryNodeIfMetadataExistsL( *metadataIdentifier );
    CleanupStack::PopAndDestroy( metadataIdentifier );
    CleanupStack::PopAndDestroy( nodeIdentifier );
    
    // NOTICE: We do not get the handle here, but the proxy side has to request it
    // separately by using the normal node request. This just created the node.    

    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    // Return the information whether the node was created or not.
    TBool retValue = node != NULL;
    aMessage.CompleteAndReleaseL( retValue, KErrNone );
    }


void CNcdNodeManager::ReleaseRequest( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    // Commit the changes in seen info.
    // Unable to handle the possible error.
    CommitSeenChanges( aMessage.Session().Context() );

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );
    aMessage.CompleteAndRelease( KErrNone );
    requestSession.RemoveObject( handle );
            
    DLTRACEOUT((""));
    }


void CNcdNodeManager::ClearSearchResultsRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    ClearSearchResultsL( aMessage.Session().Context() );
    aMessage.CompleteAndReleaseL( KErrNone, KErrNone );
    }

void CNcdNodeManager::IsCapabilitySupportedRequestL(
        MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));        
      
    HBufC8* des = HBufC8::NewLC( aMessage.InputLength() );
    TPtr8 ptr = des->Des();
    aMessage.ReadInput( ptr );
    RDesReadStream stream( *des );
    CleanupReleasePushL( stream );
    
    CNcdNodeIdentifier* nodeId = CNcdNodeIdentifier::NewLC( stream );
    CNcdNode& node = NodeL( *nodeId );
    CleanupStack::PopAndDestroy( nodeId );
    const TDesC* serverUri = NULL;
    if( node.NodeLinkL().RemoteUri() != KNullDesC )
        {
        DLTRACE(("Using Remote URI as ServerURI"));
        serverUri = &node.NodeLinkL().RemoteUri();
        }
    else
        {
        DLTRACE(("Using Server URI as ServerURI"));
        serverUri = &node.NodeLinkL().ServerUri();
        }
    
    HBufC* capability = NULL;
    InternalizeDesL( capability, stream );
    CleanupStack::PushL( capability );
    
    MNcdServerDetails& serverDetails =
        iConfigurationManager.ServerDetailsL(
            aMessage.Session().Context(),
            *serverUri,
            node.NodeLinkL().MetaDataIdentifier().NodeNameSpace() );
            
    TBool isCapabilitySupported = 
        serverDetails.IsCapabilitySupported( *capability );
        
    DLINFO(( _L("server uri: %S, namespace: %S, capability: %S, is supported: %d"),
        serverUri, &node.NodeLinkL().MetaDataIdentifier().NodeNameSpace(),
        capability, isCapabilitySupported ));
        
    CleanupStack::PopAndDestroy( capability );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( des );
    
    aMessage.CompleteAndReleaseL( isCapabilitySupported, KErrNone );
    }


TBool CNcdNodeManager::IsCapabilitySupportedL( const CNcdNodeIdentifier& aNodeIdentifier,
    const TDesC& aCapability, MCatalogsContext& aContext )
    {
    DLTRACEIN((""));
    
    CNcdNode& node = NodeL( aNodeIdentifier );
    const TDesC* serverUri = NULL;
    if( node.NodeLinkL().RemoteUri() != KNullDesC )
        {
        DLTRACE(("Using Remote URI as ServerURI"));
        serverUri = &node.NodeLinkL().RemoteUri();
        }
    else
        {
        DLTRACE(("Using Server URI as ServerURI"));
        serverUri = &node.NodeLinkL().ServerUri();
        }
    
    MNcdServerDetails& serverDetails =
        iConfigurationManager.ServerDetailsL(
            aContext,
            *serverUri,
            node.NodeLinkL().MetaDataIdentifier().NodeNameSpace() );
            
    TBool isCapabilitySupported = 
        serverDetails.IsCapabilitySupported( aCapability );
        
    DLINFO(( _L("server uri: %S, namespace: %S, capability: %S, is supported: %d"),
        serverUri, &node.NodeLinkL().MetaDataIdentifier().NodeNameSpace(),
        &aCapability, isCapabilitySupported ));
    return isCapabilitySupported;
    }


CNcdNodeIdentifier* CNcdNodeManager::GetOriginIdentifierL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));
    CNcdNode& node = NodeL( aNodeIdentifier );
    
    CNcdNodeMetaData* nodeMeta( NULL );
    DLTRACE(("Try to get metadata for node."));
    TRAPD( err, nodeMeta = &node.NodeMetaDataL() );
    if ( err == KErrNotFound ) 
        {
        DLTRACE(("Metadata not found, return NULL"));
        return NULL;
        }
    User::LeaveIfError( err );
    DASSERT( nodeMeta );
    
    CNcdPurchaseDetails* purchaseDetails = NULL;
    DLTRACE(("Try to get purchase details for node."));
    TRAP( err,
        purchaseDetails = nodeMeta->PurchaseDetailsLC();
        CleanupStack::Pop( purchaseDetails );
        );
    if ( err == KNcdErrorNoPurchaseInformation )
        {
        DLTRACE(("Purchase details not found for node, return NULL."))
        return NULL;
        }
    else if ( err != KErrNone )
        {
        User::Leave( err );
        }
    // Create origin identifier
    CleanupStack::PushL( purchaseDetails );
    CNcdNodeIdentifier* originIdentifier = CNcdNodeIdentifier::NewL(
        aNodeIdentifier.NodeNameSpace(), purchaseDetails->OriginNodeId(),
        aNodeIdentifier.ClientUid() );
    CleanupStack::PopAndDestroy( purchaseDetails );
    DLTRACE(( _L("Purchase details found, origin node id: %S"), &originIdentifier->NodeId() ));
    return originIdentifier;
    }


void CNcdNodeManager::RemoveChildrenL( CNcdNodeFolder& aFolder )
    {
    DLTRACEIN((""));

    const RPointerArray<CNcdChildEntity>& childArray( aFolder.ChildArray() );
    TInt count = childArray.Count();

    const RPointerArray<CNcdNodeIdentifier>& favorites( 
        iFavoriteManager->FavoriteNodesL( aFolder.Identifier().ClientUid() ) );

    while ( count-- ) 
        {        
        RemoveNodeFromRamCache( childArray[ count ]->Identifier() );

        if ( !NcdNodeIdentifierUtils::ContainsIdentifier( 
            childArray[ count ]->Identifier(),
            favorites )  )
            {
            DbRemoveNodeL( childArray[ count ]->Identifier() );
            }
        }
        
    // Empties folder's child array
    aFolder.RemoveChildren();
        
    DbSaveNodeL( aFolder );
    }


void CNcdNodeManager::RemoveChildrenMetadataL( CNcdNodeFolder& aFolder )
    {
    DLTRACEIN((""));
    const RPointerArray<CNcdChildEntity>& childArray( aFolder.ChildArray() );
    TInt count = childArray.Count();
    
    if ( !count ) 
        {
        DLTRACEOUT(("No children"));
        return;
        }
    
    RPointerArray<CNcdNodeIdentifier> metaIdArray;
    CleanupResetAndDestroyPushL( metaIdArray );
    metaIdArray.ReserveL( count );    
    
    const RPointerArray<CNcdNodeIdentifier>& favorites( 
        iFavoriteManager->FavoriteNodesL( aFolder.Identifier().ClientUid() ) );
    
    RPointerArray<CNcdNodeIdentifier> favoriteMetas;
    CleanupResetAndDestroyPushL( favoriteMetas );
            
    // We need to convert node ids of favorite nodes to metadata ids so that
    // we can be absolutely sure that we don't delete anything we are not
    // supposed to delete
    TInt favoriteCount = favorites.Count();
    favoriteMetas.ReserveL( favoriteCount );
    while( favoriteCount-- )
        {        
        CNcdNodeIdentifier* metaId = 
            NcdNodeIdentifierEditor::CreateMetaDataIdentifierL(
                *favorites[ favoriteCount ] );
        favoriteMetas.Append( metaId );
        }
    
    while( count-- ) 
        {
        CNcdNodeIdentifier* metaId = 
            NcdNodeIdentifierEditor::CreateMetaDataIdentifierL(
                childArray[ count ]->Identifier() );
        
        // Ensure that we don't remove favorite nodes
        if ( !NcdNodeIdentifierUtils::ContainsIdentifier( 
                *metaId, 
                favoriteMetas ) )
            {
            DLTRACE(("Meta not favorite"));
            CNcdNodeMetaData* metadata = FindNodeMetaDataFromCache( *metaId );
            if ( metadata ) 
                {
                DLTRACE(("Setting metadata to be deleted as soon as possible from cache"));
                metadata->SetDeleteSoon( ETrue );
                }            
            
            // We don't have to worry about running out of memory because the
            // array has already enough space reserved
            metaIdArray.Append( metaId );
            }
        else 
            {
            delete metaId;
            }
        }
    
    CleanupStack::PopAndDestroy( &favoriteMetas );
    
    RArray<NcdNodeClassIds::TNcdNodeClassType> classTypes;
    CleanupClosePushL( classTypes );   
    classTypes.AppendL( NcdNodeClassIds::ENcdMetaData );        
    
    if ( iClientDatabaseLocks.Find( 
            aFolder.Identifier().ClientUid().iUid ) == KErrNotFound ) 
        {        
        DLTRACE(("Removing %d metadatas from disk", metaIdArray.Count() ));
        // Delete from database but don't compact since it's veeeery slow
        NodeDbManager().RemoveDataFromDatabaseL( 
            metaIdArray, classTypes, EFalse );
        }
        
    CleanupStack::PopAndDestroy( 2, &metaIdArray ); // classTypes, metaIdArray
    }



// ---------------------------------------------------------------------------
// Protected:
// Functions that are called from functions that handle received messages.
// ---------------------------------------------------------------------------

CNcdNodeIdentifier* CNcdNodeManager::RequestNodeIdentifierLC( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));
    
    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    DLINFO(("Message, length: %X", aMessage.InputLength() ));

    // Get the node object
    RBuf8 nodeIdentifierData;
    nodeIdentifierData.CreateL( aMessage.InputLength() );
    CleanupClosePushL( nodeIdentifierData );
    User::LeaveIfError( aMessage.ReadInput( nodeIdentifierData ) );

    // Get the node information from the stream
    CNcdNodeIdentifier* nodeIdentifier = 
        CNcdNodeIdentifier::NewLC( nodeIdentifierData );
        
    // Check if we should update the nodeidentifier with the correct uid info
    if ( nodeIdentifier->ClientUid() == TUid::Null() )
        {        
        // Nodeidentifier was created in the proxy side without knowing
        // the uid value of the application.
        // Create a new nodeidentifier here that will contain the actual UID
        CNcdNodeIdentifier* uidNodeIdentifier =
            CNcdNodeIdentifier::NewL( nodeIdentifier->NodeNameSpace(),
                                      nodeIdentifier->NodeId(),
                                      nodeIdentifier->ServerUri(),
                                      requestSession.Context().FamilyId() );
        CleanupStack::PopAndDestroy( nodeIdentifier );
        nodeIdentifier = uidNodeIdentifier;
        CleanupStack::PushL( nodeIdentifier );

        DLINFO(( ("Null uid. New uid: %d"), nodeIdentifier->ClientUid().iUid ));
        }

    DLINFO((_L("Node namespace: %S, node id: %S, server uri: %S, node uid: %d"), 
            &nodeIdentifier->NodeNameSpace(), 
            &nodeIdentifier->NodeId(), 
            &nodeIdentifier->ServerUri(),
            nodeIdentifier->ClientUid()));

    CleanupStack::Pop( nodeIdentifier );
    CleanupStack::PopAndDestroy( &nodeIdentifierData );
    CleanupStack::PushL( nodeIdentifier );
    
    DLTRACEOUT((""));
    
    return nodeIdentifier;    
    }
        


// ---------------------------------------------------------------------------
// Private:
// Cache functions
// ---------------------------------------------------------------------------

CNcdNode* CNcdNodeManager::FindNodeFromCacheL(
    const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN(( _L("Node id: %S, %S, %d"), 
                &aIdentifier.NodeNameSpace(),
                &aIdentifier.NodeId(),
                aIdentifier.ClientUid().iUid ));
    
    if ( aIdentifier.ContainsEmptyFields() ) 
        {
        DLERROR(("Identifier contains empty fields, leaving with KErrArgument (%d)", 
            KErrArgument ));
        User::Leave( KErrArgument );
        }
        
    // Check from main cache at first.
    CNcdNode* node = FindNodeFromMainCache( aIdentifier );
    if ( node ) 
        {
        return node;
        }
 
        
    // Check from temp cache too. If node is found there, copy it to the main cache to
    // keep the main cache up to date.
    TInt index = FindNodeFromArray( aIdentifier, iTempNodeCache );
    if ( index != KErrNotFound ) 
        {
        User::LeaveIfError( InsertNodeInOrder( 
            iTempNodeCache[ index ], iNodeCache ) );
        iTempNodeCache[ index ]->Open();
        return iTempNodeCache[ index ];
        }

    DLTRACEOUT((""));
        
    return NULL;    
    }


// ---------------------------------------------------------------------------
// Private:
// Cache functions
// ---------------------------------------------------------------------------

CNcdNode* CNcdNodeManager::FindNodeFromMainCache(
    const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN((""));
    // Check if the node already exists in the cache.
    // If it does, do not create it.
    TInt index = FindNodeFromArray( aIdentifier, iNodeCache );
    if ( index != KErrNotFound ) 
        {
        return iNodeCache[ index ];
        }
    return NULL;
    
    }


TInt CNcdNodeManager::FindNodeFromArray( 
    const CNcdNodeIdentifier& aIdentifier,
    const RPointerArray<CNcdNode>& aArray ) const
    {            
    
    iSearchableNode->SetIdentifier( aIdentifier );
    return aArray.FindInOrder( 
        iSearchableNode, 
        iNodeOrder );
    
    /*
    for ( TInt i = 0; i < aArray.Count(); ++i )
        {
        if ( aArray[ i ]->Identifier().Equals( aIdentifier ) )
            {
            // The node has already been created.
            // Return the old node.
            return i;
            }
        }
    return KErrNotFound; 
     */
    }


TInt CNcdNodeManager::InsertNodeInOrder( 
    CNcdNode* aNode,
    RPointerArray<CNcdNode>& aArray )
    {    
    return aArray.InsertInOrder( aNode, iNodeOrder );
    //return aArray.Append( aNode );
    }


CNcdNodeMetaData* CNcdNodeManager::FindNodeMetaDataFromCache( 
    const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN(( _L("Metadata id: %S, %S, %d"), 
                &aIdentifier.NodeNameSpace(),
                &aIdentifier.NodeId(),
                aIdentifier.ClientUid().iUid ));
    
    // Check if the metadata already exists in the cache.
    // If it does, do not create it.
    for ( TInt i = 0; i < iNodeMetaDataCache.Count(); ++i )
        {
        if ( iNodeMetaDataCache[ i ]->Identifier().Equals( aIdentifier ) )
            {
            // The node has already been created.
            // Return the old node.
            return iNodeMetaDataCache[ i ];
            }
        }

    DLTRACEOUT((""));
        
    return NULL;    
    }


void CNcdNodeManager::NodeCacheCleanup()
    {
    DLTRACEIN((""));

    if ( iNodeCache.Count() < NcdProviderDefines::KNodeRamCacheMaxCount )
        {
        // Node cache has not reached the maximum size yet.
        // So, no need to do cleanup yet.
        return;
        }

    CNcdNode* node( NULL );
    TBool nodeRemoved( EFalse );    
        
    // Because the unreferenced item has the access count 1,
    // we remove them from the cache and call Close, which will
    // delete the item itself.
    // The cache does not need to be totally cleaned. So, if the
    // delimiter value is reached then the cleanup can be stopped.
    for( TInt i = 0; 
         i < iNodeCache.Count() 
            && iNodeCache.Count() >= NcdProviderDefines::KNodeRamCacheDelimiterCount; 
         ++i )
        {
        node = iNodeCache[ i ];
        if( node->AccessCount() == 1 &&
            iClientDatabaseLocks.Find(
                node->Identifier().ClientUid().iUid ) == KErrNotFound )
            {
            DLINFO(( _L("Remove node from cache: %S"), 
                &node->Identifier().NodeId() ));
                
            // Because this node will not be in the cache anymore, we can remove
            // it from the block list of the cleaner if it exists there. 
            TRAP_IGNORE(
                NodeCacheCleanerManager().
                    CacheCleanerL( node->Identifier().ClientUid() ).
                        RemoveDoNotRemoveIdentifierL( node->Identifier() ) );        

            // Remove unreferenced node from the cache and destroy it.
            iNodeCache.Remove( i );
            i--;
            node->Close();
            node = NULL;

            nodeRemoved = ETrue;
            
            DLINFO(("node removed from the cache"));
            }
        }

    if ( nodeRemoved )
        {
        // Notice that we will come here only if the RAM cache max count has been
        // reached. Otherwise the beginning of this function will return immediately.
        // So, we will not come here every time some node is released. Therefore,
        // this db check can be done here. It is run only once in a while.
        DLINFO(("Nodes were removed from RAM cache. So, check also database."));
        TRAP_IGNORE( NodeCacheCleanerManager().CheckAllL() );
        }

    DLTRACEOUT((""));
    }
    
    
void CNcdNodeManager::MetaDataCacheCleanup()
    {
    DLTRACEIN(("Metadata cache count: %d", iNodeMetaDataCache.Count() ));
    
    if ( iNodeMetaDataCache.Count() < NcdProviderDefines::KNodeRamCacheMaxCount )
        {
        // Node cache has not reached the maximum size yet.
        // So, no need to do cleanup yet.
        DLTRACEOUT(("No need for cleanup"));
        return;
        }            
    
    // Check if metadatas are used in some node
    // Go through all the metadata info

    for ( TInt i = 0; 
          i < iNodeMetaDataCache.Count()
            && iNodeMetaDataCache.Count() >= NcdProviderDefines::KNodeRamCacheDelimiterCount; 
          ++i )
        {        
        DLTRACEIN(("Going through %d nodes for metadata in index: %i", 
            iNodeCache.Count(), i ));            
                    
        if( !IsMetadataUsed( iNodeMetaDataCache[ i ] ) )
            {
            DLTRACE(("Removing metadata"));
            // Because none of the nodes needed this metadata,
            // we may close the metadata and remove it from the cache.
            CNcdNodeMetaData* metaData( iNodeMetaDataCache[ i ] );
            iNodeMetaDataCache.Remove( i );
            metaData->Close();
            metaData = NULL;
            // Because one item was removed also update the index
            // for the next round.
            --i;
            DLINFO(("metadata removed from the cache"));
            }
        }        

    DLTRACEOUT((""));
    }


TBool CNcdNodeManager::IsMetadataUsed( const CNcdNodeMetaData* aMetadata ) const
    {        
    DASSERT( aMetadata );
    
    // Compare the metadata against the metadata info that
    // nodes contain.
    for ( TInt j = 0; j < iNodeCache.Count(); ++j )
        {
        // Use non-leaving metadata getter            
        if ( iNodeCache[ j ]->NodeMetaData() == aMetadata )
            {                
            DLTRACE(("Metadata in use, index: %d", j));
            // Metadata was used in some node
            return ETrue;            
            }
        }

    for ( TInt j = 0; j < iTempNodeCache.Count(); ++j )
        {
        // Use non-leaving metadata getter            
        if ( iTempNodeCache[ j ]->NodeMetaData() == aMetadata )
            {                
            DLTRACE(("Metadata in use, index: %d", j));
            // Metadata was used in some node            
            return ETrue;
            }
        }
    
    return EFalse;
    }
    

// Closes all nodes and metadata objects     
void CNcdNodeManager::FullCacheCleanup()
    {
    DLTRACEIN((""));
    // Here we call the Close-function of the nodes which are CObjects.
    // When the access count of CObject is decreased to zero, it will
    // be destroyed. Because, the initial access number of the node is one,
    // this manager has to call the Close method, so the node will be 
    // deleted after nobody is using it.
    DLINFO(("Closing node-objects"));
    for ( TInt i = 0; i < iNodeCache.Count(); ++i )
        {
        // The element should always be deleted here because
        // its access count should reach zero after this close.
        // So, the access count should always print 1 to the debug log here.
        DLINFO(("Close node %d access count: %d", 
                i, iNodeCache[ i ]->AccessCount()));

        // Because this node will not be in the cache anymore, we can remove
        // it from the block list of the cleaner if it exists there.
        TRAP_IGNORE(
            NodeCacheCleanerManager().
                CacheCleanerL( iNodeCache[ i ]->Identifier().ClientUid() ).
                    RemoveDoNotRemoveIdentifierL( 
                        iNodeCache[ i ]->Identifier() ) );
        
        // Note that the element is most likely deleted after this close
        // call. But, it does not matter here. Because the array is also
        // reset after all the elements here have been closed. So, it does
        // not matter that pointers to the deleted elements are left to the
        // array.             
        iNodeCache[ i ]->Close();
        }
    // Also, close the cache array.
    iNodeCache.Reset();
    
    // Close the objects of temp cache too.
    for ( TInt i = 0; i < iTempNodeCache.Count(); i++ ) 
        {
        // Because this node will not be in the cache anymore, we can remove
        // it from the block list of the cleaner if it exists there.
        TRAP_IGNORE(
            NodeCacheCleanerManager().
                CacheCleanerL( iTempNodeCache[ i ]->Identifier().ClientUid() ).
                    RemoveDoNotRemoveIdentifierL( 
                        iTempNodeCache[ i ]->Identifier() ) );
        iTempNodeCache[i]->Close();
        }
    iTempNodeCache.Reset();        
    
    DLINFO(("Closing nodemetadata-objects"));
    CloseMetadatas();
    }


void CNcdNodeManager::CloseMetadatas() 
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iNodeMetaDataCache.Count(); ++i )
        {
        DLINFO(("Close metadata %d access count: %d", 
                i, iNodeMetaDataCache[ i ]->AccessCount()));                
        iNodeMetaDataCache[i]->Close();
        }
    iNodeMetaDataCache.Reset();

    DLTRACEOUT(("Nodemetadata-objects closed"));    
    }
    

void CNcdNodeManager::AppendNodeToCacheL( CNcdNode* aNode )
    {
    DLTRACEIN((""));
    
    DASSERT( aNode );

    User::LeaveIfError( InsertNodeInOrder( aNode, iNodeCache ) );

    // Because this node was appended to the list, inform the cleaner that
    // this node or its parents should not be removed from the database
    NodeCacheCleanerManager().
        CacheCleanerL( aNode->Identifier().ClientUid() ).
            AddDoNotRemoveIdentifierL( aNode->Identifier() );
        
    DLTRACEOUT(("Nodes in cache: %d", iNodeCache.Count() ));
    }


CNcdNode& CNcdNodeManager::CheckAndCreateNodeL( CNcdNodeFactory::TNcdNodeType aNodeType,
                                                CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                                const CNcdNodeIdentifier& aParentNodeIdentifier,
                                                const CNcdNodeIdentifier& aMetaIdentifier )
    {
    DLTRACEIN((""));
    
    CNcdNodeIdentifier* nodeIdentifier( 
        NcdNodeIdentifierEditor::CreateNodeIdentifierLC( aParentNodeIdentifier,
                                                         aMetaIdentifier ) );
    CNcdNode& node( CheckAndCreateNodeL( aNodeType, aNodePurpose,
                                         *nodeIdentifier ) );
    CleanupStack::PopAndDestroy( nodeIdentifier );
    
    DLTRACEOUT((""));
    
    return node;
    }


CNcdNode& CNcdNodeManager::CheckAndCreateNodeL( CNcdNodeFactory::TNcdNodeType aNodeType,
                                                CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                                const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));

    // Check if the node can be found from the RAM cache or from the db.
    CNcdNode* node( NodePtrL( aNodeIdentifier) );

    // Get the class id of the node that has the given purpose.
    // The data type parameter informs if an item or a folder should be created.
    // Set the class id to be some item as a default.
    NcdNodeClassIds::TNcdNodeClassId classId =
            NodeFactory().NodeClassIdL( aNodeType, aNodePurpose );

    if ( node != NULL )
        {
        DLINFO(("Node was found"));
        DLINFO(("Class comparison. Old: %d, new: %d",
                node->ClassId(), classId));

        // Check if we should replace the old node by new one because its type or purpose has changed.
        if ( node->ClassId() != classId )
            {
            DLWARNING(("Be sure that the server has changed the type and there is no bug in the code!"));
            // Because the original type is wrong type,
            // remove the node from the RAM cache.
            for ( TInt i = 0; i < iNodeCache.Count(); ++i )
                {
                if ( iNodeCache[ i ] == node )
                    {
                    DLINFO(("Remove node from db"));
                    // Because node is removed from cache.
                    // Call Close, so the access count will be correct
                    // for the hanging node. Most likely it will be
                    // deleted soon.                
                    iNodeCache.Remove( i );
                    // Because the node is removed from the cache
                    // also inform cleaner that the node may be cleaned
                    // from the database if wanted.
                    NodeCacheCleanerManager().
                        CacheCleanerL( node->Identifier().ClientUid() ).
                                       RemoveDoNotRemoveIdentifierL( 
                                        node->Identifier() );
                    // Finally close the node. Because we do not own it
                    // anymore.
                    node->Close();
                    node = NULL;
                    break;
                    }
                }
                
            // Because the node was removed from the cache and we do not
            // want to use the node that is saved into the db,
            // we have to create the node directly here by using
            // node factory. This way we get the new uninitialized node
            // that is of the correct type.

            DLINFO(("Create node"));
            // Create the node according to the class id
            node = NodeFactory().CreateNodeLC( aNodeIdentifier,
                                               classId );

            DASSERT( node );
            
            // Insert node to the cache.
            // This will also insert the node back into do not remove list of
            // the cleaner.
            AppendNodeToCacheL( node );
            
            // Cache takes ownership of the node
            CleanupStack::Pop( node );
            }
        }
    else
        {
        DLINFO(("Node has to be created because it was not found"));
        // Create the node according to the class id
        node = NodeFactory().CreateNodeLC( aNodeIdentifier,
                                           classId );

        
        DASSERT( node );
        
        // Insert node to the cache.
        // This will also insert the node back into do not remove list of
        // the cleaner.
        AppendNodeToCacheL( node );
        
        // Cache takes ownership of the node
        CleanupStack::Pop( node );
        }
    
    DLTRACEOUT((""));
    
    return *node;
    }


CNcdNodeMetaData& CNcdNodeManager::CheckAndCreateMetaDataL( 
    const CNcdNodeIdentifier& aMetaIdentifier,
    CNcdNodeFactory::TNcdNodeType aMetaType )
    {
    DLTRACEIN((""));

    // Check if the metadata can be found from the RAM cache or from the db.
    CNcdNodeMetaData* metaData( NULL );
    TRAPD( metaDataError, metaData = &NodeMetaDataL( aMetaIdentifier ) );

    // Accept leave with KEreNotFound because next we can create it
    if( metaDataError != KErrNone && metaDataError != KErrNotFound )
        {
        DLERROR(( "metaDataError: %d", metaDataError ));
        User::Leave( metaDataError );
        }
    DLINFO((""));

    // Get the class id of the node that has the given purpose.
    // The data type parameter informs if an item or a folder should be created.
    // Set the class id to be some item as a default.
    NcdNodeClassIds::TNcdNodeClassId classId =
            NodeFactory().MetaDataClassId( aMetaType );

    if ( metaData != NULL )
        {
        DLINFO(("Metadata was found"));
        DLINFO(("Class comparison. Old: %d, new: %d",
                metaData->ClassId(), classId));

        if ( metaData->ClassId() != classId )
            {
            DLWARNING(("Be sure that the server has changed the type and there is no bug in the code!"))
            // Because the original type is wrong type,
            // remove the node from the RAM cache.
            for ( TInt i = 0; i < iNodeMetaDataCache.Count(); ++i )
                {
                if ( iNodeMetaDataCache[ i ] == metaData )
                    {
                    DLINFO(("Remove meta from db"));
                    // Because metadata is removed from cache.
                    // Call Close, so the access count will be correct
                    // for the hanging metadata. Most likely it will be
                    // deleted soon.                
                    iNodeMetaDataCache.Remove( i );
                    // Finally close the metadata. Because we do not own it
                    // anymore.
                    metaData->Close();
                    metaData = NULL;
                    break;
                    }
                }
                
            // Because the metadata was removed from the cache and we do not
            // want to use the metadata that is saved into the db,
            // we have to create the metadata directly here by using
            // node factory. This way we get the new uninitialized node
            // that is of the correct type.

            DLINFO(("Create metadata"));
            // Create the node according to the class id
            metaData = NodeFactory().CreateMetaDataLC( aMetaIdentifier,
                                                       classId );

            if ( metaData == NULL )
                {
                // Node was not be created.
                DLERROR(("Metadata was not created."));
                DASSERT( EFalse );
                User::Leave( KErrNotFound );
                }
            
            // Insert node to the cache.
            // This will also insert the node back into do not remove list of
            // the cleaner.
            iNodeMetaDataCache.AppendL( metaData );
            
            // Cache takes ownership of the node
            CleanupStack::Pop( metaData );
            }
        }
    else
        {
        DLINFO(("Metadata has to be created because it was not found"));
        // Create the node according to the class id
        metaData = NodeFactory().CreateMetaDataLC( aMetaIdentifier,
                                                   classId );

        if ( metaData == NULL )
            {
            // Node was not be created.
            DLERROR(("Node was not created."));
            DASSERT( EFalse );
            User::Leave( KErrNotFound );
            }
        
        // Insert node to the cache.
        // This will also insert the node back into do not remove list of
        // the cleaner.
        iNodeMetaDataCache.AppendL( metaData );
        
        // Cache takes ownership of the node
        CleanupStack::Pop( metaData );
        }
    
    DLTRACEOUT((""));
    
    return *metaData;
    }
    

MNcdConfigurationManager& CNcdNodeManager::ConfigurationManager()  const
    {
    return iConfigurationManager;
    }
    

void CNcdNodeManager::RemoveNodeFromRamCache( 
    const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN((""));
    TInt index = FindNodeFromArray( aIdentifier, iNodeCache );
    if ( index != KErrNotFound )
        {
        DLTRACE(("Removing node from RAM cache"));
        iNodeCache[ index ]->Close();
        iNodeCache.Remove( index );        
        }
    }


void CNcdNodeManager::CommitSeenChanges( const MCatalogsContext& aContext )
    {
    TRAP_IGNORE( iSeenInfo->CommitChangesL( aContext.FamilyId() ) );
    }

