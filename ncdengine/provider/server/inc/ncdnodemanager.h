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
* Description:   Contains CNcdNodeManager class
*
*/


#ifndef NCD_NODE_MANAGER_H
#define NCD_NODE_MANAGER_H


#include <e32base.h>
#include <e32cmn.h>
#include <f32file.h>

#include "catalogscommunicable.h"
#include "ncdnodefactory.h"
#include "ncdparserobserver.h"
#include "ncdnodeclassids.h"
#include "ncdprotocoltypes.h"
#include "ncd_pp_entityref.h"
#include "ncddatabaseitems.h"

class CNcdNode;
class CNcdNodeItem;
class CNcdNodeFolder;
class CNcdRootNode;
class CNcdBundleFolder;
class CNcdNodeTransparentFolder;
class CNcdSearchNodeFolder;
class CNcdNodeMetaData;
class CNcdNodeIdentifier;
class CNcdStorageManager;
class CNcdNodeDbManager;
class CNcdPurchaseHistoryDb;
class MCatalogsBaseMessage;
class MNcdPreminetProtocolEntityRef;
class MNcdPreminetProtocolDataEntity;
class MNcdPreminetProtocolDownload;
class MNcdStorageDataItem;
class CNcdSearchNodeFolder;
class MCatalogsContext;
class CNcdNodeCacheCleanerManager;
class CNcdPreviewManager;
class MNcdConfigurationManager;
class CNcdExpiredNode;
class MNcdPreminetProtocolExpiredCachedData;
class CNcdSearchNodeBundle;
class CNcdSearchNodeItem;
class CNcdFavoriteManager;
class CNcdNodeSeenInfo;
class CNcdSearchableNode;
class CNcdGeneralManager;

/**
 *  CNodeManager provides functions to handle creation and deletion
 *  of node related objects.
 */
class CNcdNodeManager : public CCatalogsCommunicable                  
    {

public:

    /**
     * Defines how ref data is handled.
     */
    enum TNcdRefHandleMode
        {
        EUpdate,    // Updates a previously existing node
        EInsert,    // Inserts node to a specific index, increases node count
        EReplace,   // Replaces the node at a specific index
        EAppend     // Appends the node
        };

    /**
     * Different types of temporary nodes.
     */        
    enum TNcdTemporaryNodeType
        {
        ENcdTemporaryNodeItem,
        ENcdTemporaryNodeFolder,
        ENcdTemporaryBundleFolder
        };

    /**
     * NewL
     *
     * @param aStorageManager Storage manager provides means for
     * the manager to save, load, and remove data in db.
     * @param aPurchaseHistory Purchase history contains information
     * about node purchase phases.
     * @param aConfigurationManager Gives access to server capabilities etc.
     * @return CNcdNodeManager* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeManager* NewL(
        CNcdGeneralManager& aGeneralManager );

    /**
     * Destructor
     *
     * Deletes the object from caches and releases objects that are
     * owned by this class object.
     */
    virtual ~CNcdNodeManager();


public: // General getter and setter functions

    /**
     * @return CNcdPurchaseHistoryDb& The purchase history class object.
     */    
    CNcdPurchaseHistoryDb& PurchaseHistory() const;

    CNcdPreviewManager& PreviewManager() const;
    
    MNcdConfigurationManager& ConfigurationManager() const;

    /**
     * @return CNcdNodeCacheCleanerManager& Cleaner Manager may be used 
     * to get db cache cleaners that can be used to clean data
     * from db.
     */
    CNcdNodeCacheCleanerManager& NodeCacheCleanerManager() const;

    /**
     * @return CNcdNodeDbManager& Reference to the db manager
     * that can be used to save, load, and remove data in db.
     */
    CNcdNodeDbManager& NodeDbManager() const;
    
    /**
     * @return CNcdNodeSeenInfo& Reference to the seen info
     * which is used to keep track of seen nodes.
     */
    CNcdNodeSeenInfo& SeenInfo() const;
    
    /**
     * Sets the favorite manager.
     */
    void SetFavoriteManager( CNcdFavoriteManager& aManager );


public: // Functions to get node objects from caches or from db. 
        // And, functions to create node objects.
        
    /**
     * Checks if the node is found from the cache or from the database.
     * If the node is found from the database then the metadata is also 
     * searched for the node and if it is found then it is inserted 
     * for the node.
     *
     * @note This uses the actual node identifier which may differ from
     * the metadata identifier. Be sure that the identifier is correct
     * when using this function.
     *
     * @param aIdentifier Identifies the node that is searched for.
     * @return CNcdNode& Reference to the node that was found.
     * @excpetion KErrNotFound if the node was not found from the
     * cache or from the database.
     */
    CNcdNode& NodeL( const CNcdNodeIdentifier& aIdentifier );

    /**
     * Instead of taking the actual nodeidentifier, uses the parent identifier
     * and the metadata identifier to create the correct identifier and after that
     * calls the NodeL above.
     *
     * @see CNcdNodeManager::NodeL
     *
     * @param aParentIdentifier Identifier of parent node of the node that is searched.
     * @param aNodeMetaDataIdentifier Metadata identifier of the node that is
     * searched for.
     */
    CNcdNode& NodeL( const CNcdNodeIdentifier& aParentIdentifier,
                     const CNcdNodeIdentifier& aNodeMetaDataIdentifier );
 
    
    /**
     * Checks if the node is found from the cache or from the database.
     * If the node is found from the database then the metadata is also 
     * searched for the node and if it is found then it is inserted 
     * for the node.
     *
     * @note Ownership of the node is not transferred.
     *
     * @note This uses the actual node identifier which may differ from
     * the metadata identifier. Be sure that the identifier is correct
     * when using this function.
     *
     * @param aIdentifier
     * @return CNcdNode* Node if it is found. Else NULL.
     */
    CNcdNode* NodePtrL( const CNcdNodeIdentifier& aIdentifier );

    /**
     * Instead of taking the actual nodeidentifier, uses the parent identifier
     * and the metadata identifier to create the correct identifier and after that
     * calls the NodeL above.
     *
     * @see CNcdNodeManager::NodePtrL
     *
     * @param aParentIdentifier Identifier of parent node of the node that is searched.
     * @param aNodeMetaDataIdentifier Metadata identifier of the node that is
     * searched for.
     */
    CNcdNode* NodePtrL( const CNcdNodeIdentifier& aParentIdentifier,
                        const CNcdNodeIdentifier& aNodeMetaDataIdentifier );
    

    /**
     * Uses NodeL to get the folder from the cache or from the db. 
     * If the node is of the folder type, then the folder object is 
     * returned.
     *
     * @see CNcdNodeManager::NodeL
     */    
    CNcdNodeFolder& FolderL(
        const CNcdNodeIdentifier& aIdentifier );

    /**
     * Uses NodeL to get the folder from the cache or from the db. 
     *
     * @see CNcdNodeManager::NodeL
     */    
    CNcdRootNode& RootNodeL( const TUid& aClientUid );

    /**
     * Uses NodeL to get the folder from the cache or from the db. 
     *
     * @see CNcdNodeManager::NodeL
     */    
    CNcdSearchNodeFolder& SearchFolderL( const CNcdNodeIdentifier& aParentIdentifier,
                                         const CNcdNodeIdentifier& aNodeMetaDataIdentifier );

    /**
     * Uses NodeL to get the folder from the cache or from the db.
     *
     * @see CNcdNodeManager::NodeL
     */    
    CNcdSearchNodeFolder& SearchFolderL( const CNcdNodeIdentifier& aNodeIdentifier );
    
    /**
     * Uses NodeL to get the folder from the cache or from the db.
     *
     * @see CNcdNodeManager::NodeL
     */    
    CNcdSearchNodeItem& SearchNodeItemL( const CNcdNodeIdentifier& aNodeIdentifier );

    /**
     * Creates a node accroding to the given type information. 
     * Adds the created node into the cache. If node already exists
     * in the cache or in the database, then the old node is used.
     *
     * @note Be carefull when using this function. Make sure that the node identifier
     * actually contains all the parts. Should you use CreateNodeL with parent and
     * metadata identifiers instead?
     *
     * @param aNodeType informs if the node should be item or folder
     * @param aNodePurpose gives the information if the normal or some specific kind
     * of node should be created.
     * @param aNodeIdentfier The identifier that identifies the parent of this node.
     * @return CNcdNode& Reference to the node that was found.
     */
    CNcdNode& CreateNodeL( CNcdNodeFactory::TNcdNodeType aNodeType,
                           CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                           const CNcdNodeIdentifier& aNodeIdentifier );


    /**
     * Creates a node accroding to the given type information. 
     * Adds the created node into the cache. If node already exists
     * in the cache or in the database, then the old node is used.
     *
     * @param aNodeType informs if the node should be item or folder
     * @param aNodePurpose gives the information if the normal or some specific kind
     * of node should be created.
     * @param aParentNodeIdentfier The identifier that identifies the parent of this node.
     * @param aMetaDataIdentifier The identifier of metadata of this node.
     * @return CNcdNode& Reference to the node that was found.
     */
    CNcdNode& CreateNodeL( CNcdNodeFactory::TNcdNodeType aNodeType,
                           CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                           const CNcdNodeIdentifier& aParentNodeIdentifier,
                           const CNcdNodeIdentifier& aMetaDataIdentifier );


    /**
     * Creates an item node accroding to the given type information. 
     * Adds the created node into the cache. If node already exists
     * in the cache or in the database, then the old node is used.
     *
     * @note Be carefull when using this function. Make sure that the node identifier
     * actually contains all the parts. Should you use CreateNodeItemL with parent and
     * metadata identifiers instead?
     *
     * @param aNodePurpose gives the information if the normal or some specific kind
     * of node should be created.
     * @param aNodeIdentfier The identifier that identifies the parent of this node.
     * @return CNcdNode& Reference to the node that was found.
     */
    CNcdNodeItem& CNcdNodeManager::CreateNodeItemL( CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                                    const CNcdNodeIdentifier& aNodeIdentifier );


    /**
     * Creates an item node accroding to the given type information. 
     * Adds the created node into the cache. If node already exists
     * in the cache or in the database, then the old node is used.
     *
     * @param aNodePurpose gives the information if the normal or some specific kind
     * of node should be created.
     * @param aParentNodeIdentfier The identifier that identifies the parent of this node.
     * @param aMetaDataIdentifier The identifier of metadata of this node.
     * @return CNcdNode& Reference to the node that was found.
     */
    CNcdNodeItem& CNcdNodeManager::CreateNodeItemL( CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                                    const CNcdNodeIdentifier& aParentNodeIdentifier,
                                                    const CNcdNodeIdentifier& aMetaDataIdentifier );


    /**
     * Creates a folder node accroding to the given type information. 
     * Adds the created node into the cache. If node already exists
     * in the cache or in the database, then the old node is used.
     *
     * @note Be carefull when using this function. Make sure that the node identifier
     * actually contains all the parts. Should you use CreateNodeFolderL with parent and
     * metadata identifiers instead?
     *
     * @param aNodePurpose gives the information if the normal or some specific kind
     * of node should be created.
     * @param aNodeIdentfier The identifier that identifies the parent of this node.
     * @param aFolderType May be used to inform what kind of special folder is used. For example
     * if root folder should be created.
     * @return CNcdNode& Reference to the node that was found.
     */
    CNcdNodeFolder& CNcdNodeManager::CreateNodeFolderL( CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                                        const CNcdNodeIdentifier& aNodeIdentifier,
                                                        CNcdNodeFactory::TNcdNodeType aFolderType
                                                            = CNcdNodeFactory::ENcdNodeFolder );    
    /**
     * Creates a folder node accroding to the given type information. 
     * Adds the created node into the cache. If node already exists
     * in the cache or in the database, then the old node is used.
     *
     * @param aNodePurpose gives the information if the normal or some specific kind
     * of node should be created.
     * @param aParentNodeIdentfier The identifier that identifies the parent of this node.
     * @param aMetaDataIdentifier The identifier of metadata of this node.
     * @param aFolderType May be used to inform what kind of special folder is used. For example
     * if root folder should be created.
     * @return CNcdNode& Reference to the node that was found.
     */
    CNcdNodeFolder& CNcdNodeManager::CreateNodeFolderL( CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                                        const CNcdNodeIdentifier& aParentNodeIdentifier,
                                                        const CNcdNodeIdentifier& aMetaDataIdentifier,
                                                        CNcdNodeFactory::TNcdNodeType aFolderType
                                                            = CNcdNodeFactory::ENcdNodeFolder );    

    /**
     * Uses FolderL to search an existing folder. If the folder is not
     * found then new transparent folder will be created and retured.
     *
     * @see CNcdNodeManager::FolderL
     */        
    CNcdNodeTransparentFolder& CreateTransparentFolderL(
        const CNcdNodeIdentifier& aNodeIdentifier );

    /**
     * Getter for root.
     * Uses FolderL to get the root from the cache or from the db.
     */
    CNcdRootNode& CreateRootL( const TUid& aClientUid );
                                                     
    /**
     * Getter for root.
     * Uses CreateRootL above to get the root from the cache or from the db.
     */
    CNcdRootNode& CreateRootL( const MCatalogsContext& aContext );

    /**
     * Getter for search root.
     * Uses FolderL to get the search root from the cache or from the db.
     */
    CNcdNodeFolder& CreateSearchRootL( const MCatalogsContext& aContext );

    /**
     * Getter for the bundle node.
     * The parent of the bunlde is always root.
     *
     * @note the use of the metadata identifier instead of the node identifier
     * @note Also, note that this function does not add the created folder to be the child
     * of the root. It has to be done separately.
     *
     * @param aMetaDataIdentifier The metadata identifier of the bundle node is used
     * when the bundle folder is created. So, note that you have to give meta data identifier
     * here instead of the node identifier.
     */    
    CNcdNodeFolder& CreateBundleFolderL( const CNcdNodeIdentifier& aMetaDataIdentifier );

    /**
     * This function can be used to create temporary nodes.
     * If corresponding temporary node already exists in RAM cache or 
     * in database, it will be returned. If not, then metadata is checked.
     * If the metadata for the node exists, a temporary node is created directly 
     * for the node, and the server uri for the node link is set from 
     * the metadata identifier. The type of the node is gotten from the metadata.
     * If the temporary node could not be created, then the supplier node that will
     * act as a wrapper is created.
     *
     * @note aMetaDataIdentifier has to contain server URI if the node should be able
     * to load its data from the web.
     * @note According to the way the node is created. The node identifier will be an
     * identifier of the temporary node or the identifier of the supplier node.
     * @note If the metadata is found, it is set to the node 
     *
     * @param aMetaDataIdentifier Identifies the metadata that the node should contain.
     * @return CNcdNode& Reference to the node that has been created.
     */
    CNcdNode& CreateTemporaryNodeOrSupplierL( const CNcdNodeIdentifier& aMetaDataIdentifier );
    
    
    /**
     * Creates temporary node if the given metadata exists already. The type
     * of the created node is concluded from the type of the metadata. The
     * created node has no parent. The metadata is set automatically for the
     * created node. NULL is returned if the node cannot be created.
     *
     * @param aMetaDataIdentifier Identifies the metadata that the node should contain.
     * @return The node or NULL, ownership is NOT transferred.
     */
    CNcdNode* CreateTemporaryNodeIfMetadataExistsL(
        const CNcdNodeIdentifier& aMetadataIdentifier );


    /**
     * Creates temporary node. The created node has no parent. The metadata is 
     * set automatically for the created node if it exists or can be created. 
     * NULL is returned if the node cannot be created.
     *
     * @param aTempNodeIdentifier Temp node identifier
     * @param aType Type of the temp node
     * @param aCreateMetaData If ETrue metadata is created if it doesn't exist
     * @return The node or NULL, ownership is NOT transferred.
     */
    CNcdNode& CreateTemporaryNodeL(
        CNcdNodeIdentifier& aTempNodeIdentifier,
        TNcdTemporaryNodeType aType,
        TBool aCreateMetaData );

    /**
     * Creates a search bundle folder.
     *
     * @note the use of the metadata identifier instead of the node identifier
     * @note Also, note that this function does not add the created node as a child
     * for the parent. It has to be done separately.
     *
     * @param aMetaDataIdentifier The metadata identifier of the node is used
     * when the folder is created. So, note that you have to give meta data identifier
     * here instead of the node identifier.
     * @param aParentIdentifier The full identifier of the parent, NOT a metadata identifier.
     */    
    CNcdSearchNodeBundle& CreateSearchBundleL( const CNcdNodeIdentifier& aMetaDataIdentifier,
        const CNcdNodeIdentifier& aParentIdentifier );

    /**
     * Creates a search folder.
     *
     * @note the use of the metadata identifier instead of the node identifier
     * @note Also, note that this function does not add the created node as a child
     * for the parent. It has to be done separately.
     *
     * @param aMetaDataIdentifier The metadata identifier of the node is used
     * when the folder is created. So, note that you have to give meta data identifier
     * here instead of the node identifier.
     * @param aParentIdentifier The full identifier of the parent, NOT a metadata identifier.
     */
    CNcdSearchNodeFolder& CreateSearchFolderL( const CNcdNodeIdentifier& aMetaDataIdentifier,
        const CNcdNodeIdentifier& aParentIdentifier );

    /**
     * Checks if the node metadata is found from the cache or from the database.
     *
     * @param aIdentifier Identifies the node metadata that is searched for.
     * @return CNcdNode& Reference to the node metadata that was found.
     * @exception KErrNotFound if the node metadata was not found from the
     * cache or from the database.
     */
    CNcdNodeMetaData& NodeMetaDataL( const CNcdNodeIdentifier& aIdentifier );

    /**
     * Creates an empty node metadata.
     *
     * @note That this function does not set the metadata to any node because
     * the metadata may not be initialized with proper values if it has only
     * been created after it was not found from db or from cache.
     * The caller of this function has to set the metadata to the node
     * itself when it is wanted.
     *
     * @param aMetaDataIdentifier Identifies the name of the metadata that is created.
     * @param aMetaType Gives the type of metadata that will be created.
     * @return CNcdNodeFolderMetaData& reference to the metadata that was created.
     * @exception KErrNotFound If the node was not found from the cache or from the
     *                         database.
     */
    CNcdNodeMetaData& CreateNodeMetaDataL(
        const CNcdNodeIdentifier& aMetaDataIdentifier,
        CNcdNodeFactory::TNcdNodeType aMetaType );


public: // Functions that are used to insert data information
        // gotten from the data parsers into the correct node
        // objects. These functions are provided for operations
        // to update the nodes. The updated information may also be
        // inserted from the purchase history.

    /**
     * This function updates the node and its link information by
     * using the MNcdPreminetProtocolEntityRef data. A new node will
     * be created if it does not exists. Also, metadata is inserted into the
     * node if the corresponding metadata is found.
     *
     * @param aParentNodeIdentifier Identifies the parent of the node that is updated.
     * @param aData Contains the data that is used to internalize the node link.
     * @param aClientUid This parameter provides the client uid information. Usually,
     * the information is also included in the parent node identifier but in some
     * cases the identifier may be empty. So, the client uid is provided here.
     * @param aMode Identifies the purpose of this function call.
     * @param aIndex Tells the index that is used when the node is inserted
     * into its parent's child array. If mode is EAppend, the index is ignored.
     * @param aParentNodeType Informs what type the parent is. This is used to check
     * if the parent is normal folder or root.
     * @param aNodePurpose Informs what kind of node should get this parser data.
     * @param aCreateParent Creates the parent node if it does not exist.
     * @return CNcdNode& Node that was updated.
     */
    CNcdNode& RefHandlerL( 
                const CNcdNodeIdentifier& aParentNodeIdentifier,
                MNcdPreminetProtocolEntityRef& aData,
                const TUid& aClientUid,
                TNcdRefHandleMode aMode,
                TInt aIndex,
                CNcdNodeFactory::TNcdNodeType aParentNodeType
                = CNcdNodeFactory::ENcdNodeFolder,
                CNcdNodeFactory::TNcdNodePurpose aParentNodePurpose
                = CNcdNodeFactory::ENcdNormalNode,
                CNcdNodeFactory::TNcdNodePurpose aNodePurpose 
                = CNcdNodeFactory::ENcdNormalNode,
                TBool aCreateParent = ETrue );

    /**
     * This function updates the node metadata by
     * using the MNcdPreminetProtocolDataEntity data. A new node metadata will
     * be created if it does not exists. Also, metadata is inserted into the
     * node. The node has always been created for the metadata before this function
     * is called because RefHandlerL is called when the reference data has been loaded
     * before metadata.
     *
     * @param aParentNodeIdentifier Identifies the parent of the node that is updated.
     * @param aData Contains the data that is used to internalize the node link.
     * @param aClientUid This parameter provides the client uid information. Usually,
     * the information is also included in the parent node identifier but in some
     * cases the identifier may be empty. So, the client uid is provided here.
     * @return CNcdNode& Node that contains the metadata that was updated.
     */
    CNcdNode& DataHandlerL( 
                const CNcdNodeIdentifier& aParentNodeIdentifier,
                MNcdPreminetProtocolDataEntity& aData,
                const TUid& aClientUid );


    /**
     * Sets the preview data for the node.
     * The metadata of the node actually contains the information.
     * So, the info is updated eventually into the metadata.
     *
     * @param aIdentifier Identifies the node that will be updated.
     * @param aFileName Name of the file that contains the preview data.
     * @param aIndex Node may contain multiple previews. Index identifies
     * the preview of the node.
     * @param aMimeType MIME type received in HTTP headers. Used only if 
     * none was received in protocol responses
     */
    void PreviewHandlerL( const CNcdNodeIdentifier& aNodeIdentifier,
                          const TDesC& aFileName,
                          TInt aIndex,
                          const TDesC& aMimeType );
 
    /**
     * Updates the node 
     * The metadata of the node actually contains the information.
     * So, the info is updated eventually into the metadata.
     *
     * @param aIdentifier Identifies the node that will be updated.
     */
    void PurchaseHandlerL( const CNcdNodeIdentifier& aNodeIdentifier );


    /**
     * Sets the download information of a content into the metadata of a node. 
     * This is called when the data has been downloaded.
     * The metadata of the node actually contains the information.
     * So, the info is updated eventually into the metadata.
     *
     * @param aIdentifier Identifies the node whose content may be downloaded.
     */
    void DownloadDataHandlerL( const CNcdNodeIdentifier& aNodeIdentifier );


    /**
     * Sets the install information for the node. This is called when the downloaded
     * data has been installed.
     * The metadata of the node actually contains the information.
     * So, the info is updated eventually into the metadata.
     *
     * @param aIdentifier Identifies the node that will be updated.
     */
    void InstallHandlerL( const CNcdNodeIdentifier& aNodeIdentifier );

    
    /**
     * Sets the node expired.
     *
     * @param aNodeIdentifier The node identifier.
     * @param recursive If true, all the child nodes are set expired too.
     * @param aFoundNodes All found nodes are added to this array.
     */
    void SetNodeExpiredL( const CNcdNodeIdentifier& aNodeIdentifier, 
                          TBool aRecursive,
                          TBool aForceUpdate,
                          RPointerArray<CNcdExpiredNode>& aFoundNodes );
    
    /**
     * Sets the node expired.
     *
     * @param aNodeIdentifier The node identifier.
     * @param recursive If true, all the child nodes are set expired too.
     * @param aFoundNodes All found nodes are added to this array.
     */
    void SetNodeExpiredL( CNcdNode& aNode, 
                          TBool aRecursive,
                          TBool aForceUpdate,
                          RPointerArray<CNcdExpiredNode>& aFoundNodes );
    
    /**
     * Sets all nodes that use the given metadata expired.
     *
     * @param aData Expired cached data info.
     * @param aClientUid Uid of the client.
     * @param aNameSpace Namespace of the expired nodes.
     * @param aFoundNodes All found nodes are added to this array.
     */
    void SetNodesExpiredByMetadataL(
        const MNcdPreminetProtocolExpiredCachedData& aData, 
        const TUid& aClientUid,
        const TDesC& aNameSpace,
                          RPointerArray<CNcdExpiredNode>& aFoundNodes );


    /**
     * Removes the given node.
     * This function can be used if the actua node identifier is not known
     * but the parent identifier and the node metadata are.
     *
     * @param aParentIdentifier
     * @param aNodeMetaDataIdentifier
     * @ Remove the node from database.
     */
    void RemoveNodeL( const CNcdNodeIdentifier& aParentIdentifier,
                      const CNcdNodeIdentifier& aNodeMetaDataIdentifier );

    
    /**
     * Removes the given node.
     *
     * @param aIdentifier The node.
     * @ Remove the node from database.
     */
    void RemoveNodeL( const CNcdNodeIdentifier& aIdentifier );
    
     
    /**
     * Clears all search results. 
     */     
    void ClearSearchResultsL( const MCatalogsContext& aContext );
    
    /**
     * Moves all the child nodes (recursively) of the given root in the RAM cache to temporary cache.
     * Note! The nodes are not searched from temporary cache ever, but they
     * can be returned to the main RAM cache if wanted. The children are removed from
     * the root node's child list. Node database must not be locked when
     * calling this function.
     *
     * @param aRootNode The root node.
     */
    void BackupAndClearCacheL(
        const CNcdNodeIdentifier& aRootNode );
    
    /**
     * Reverts the child nodes of the given root from the temporary cache to the main 
     * RAM cache. Internalizes the root node's child list from database.
     *
     * @param aRootNode The root node.
     */
    void RevertNodeCacheL(
        const CNcdNodeIdentifier& aRootNode ); 
    
    /**
     * Reverts the given node from the temp cache to the main RAM cache.
     *
     * @param aNodeIdentifier The node identifier.
     */
    void RevertNodeFromTempCacheL( const CNcdNodeIdentifier& aNodeIdentifier );
    
    /**
     * Removes and closes the child nodes of the given root in the temporary
     * RAM cache.
     *
     * @param aRootIdentifier The root node.
     */
    void ClearTempCacheL( const CNcdNodeIdentifier& aRootIdentifier );


public: // Cache cleanup functions.
        // These functions are called by the node objects when
        // they are released by the corresponding proxy object.

    /**
     * This function is called by the node when the
     * release request has been made from the proxy node.
     * When the node is release its reference count is decreased
     * by one. If the reference count has reached one. It means
     * that no proxy side object needs the node anymore. So, the
     * node may be closed if necessary.
     *
     * @param aNode The node which got the release request.
     */
    void NodeReleased( CNcdNode& aNode );

    /**
     * This function is called by the metadata when the
     * release request has been made from the proxy metadata.
     * When the metadata is release its reference count is decreased
     * by one. If the reference count has reached one. It means
     * that no proxy side object needs the node anymore. So, the
     * metadata may be closed if necessary.
     *
     * @param aNode The node which got the release request.
     */
    void MetaDataReleased( CNcdNodeMetaData& aMetaData );


    /**
     * Clears a client's cache (nodes, icons, previews etc.)
     *
     * @param aContext Client's context
     * @param aClearDownloads If ETrue incomplete downloads are deleted
     */
    void ClearClientCacheL( 
        const MCatalogsContext& aContext, 
        TBool aClearDownloads );

    
public: // Db functions that are needed from other class objects.
        // The db functions are provided here instead of directly providing
        // db manager because node manager may want to do some additional checking
        // before db actions are allowed.

   
    /**
     * Removes the node from the db.
     *
     * @param aIdentifier Identifies the node that will be removed from db.
     */
    void DbRemoveNodeL( const CNcdNodeIdentifier& aIdentifier );


    /**
     * Saves the userdata into the db.
     *
     * @param aUserDataIdentifier Identifier for the db object.
     * @param aDataItem UserData class object should implement MNcdStorageDataItem
     * interface whose externalzie function is called when the data is saved. 
     */
    void DbSaveUserDataL( const CNcdNodeIdentifier& aUserDataIdentifier,
                          MNcdStorageDataItem& aDataItem );
    
    /**
     * Removes the user data corresponding the given identifier from the db.
     * @param aUserDataIdentifier Identifies the user data from the db.
     */
    void DbRemoveUserDataL( const CNcdNodeIdentifier& aUserDataIdentifier );
    
    /**
     * Loads the user data from db.
     *
     * @param aUserDataIdentifier Identfier for the db object.
     * @param aDataItem UserData class object should implement MNcdStorageDataItem
     * interface whose internalize function is called when the data is laoded.
     */
    void DbLoadUserDataL( const CNcdNodeIdentifier& aUserDataIdentifier,
                          MNcdStorageDataItem& aDataItem );

    /**
     * Gets the screenshot data from the db.
     *
     * @param aScreenshotIdentifier Identifier for the db object.
     * @return HBufC8* The descriptor contains the screenshot data.
     */
    HBufC8* DbScreenshotDataLC( const CNcdNodeIdentifier& aScreenshotIdentifier );

    /**
     * Gets the icon data from the db.
     *
     * @param aIconIdentifier Identifier for the db object.
     * @return HBufC8* The descriptor contains the icon data.
     */
    HBufC8* DbIconDataLC( const CNcdNodeIdentifier& aIconIdentifier );

    /**
     * Saves the given icon data into the db.
     *
     * @param aIconIdentifier Identifier for the db object.
     * @param aIconData Data that will be saved into the db.
     */
    void DbSaveIconDataL( const CNcdNodeIdentifier& aIconIdentifier, 
                          const TDesC8& aIconData );


    /**
     * Saves the node information to the storage.
     * The node implements MNcdStorageDataItem interface.
     * So, it can internalize its data into the db.
     *
     * @param aNode
     */
    void DbSaveNodeL( CNcdNode& aNode );
    
    /**
     * Saves all the child nodes of the given root in the RAM cache to the storage.
     * Does not save scheme nodes.
     *
     * @param aRootNode The root node.
     */
    void DbSaveNodesL( const CNcdNodeIdentifier& aRootNode );
    
    /**
     * Saves the node metadata information to the storage.
     * The node metadata implements MNcdStorageDataItem interface.
     * So, it can internalize its data into the db.
     *
     * @param aMetaData
     */
    void DbSaveNodeMetaDataL( CNcdNodeMetaData& aMetaData );


    /**
     * Sets the max db byte size for the given client.
     * If db exeeds this size, then some cleaning should be done.
     *
     * @notice This function takes the max db size value as kilobytes.
     * Function converts the kB value to byte value 
     * (by multiplying by 1024) and
     * sets the converted value for the cache cleaner.
     *
     * @param aUid the client uid
     * @param aMaxDbKiloByteSize The maximum kilo byte size of the db.
     */
    void DbSetMaxSizeL( const TUid& aClientUid, 
                       const TInt aMaxDbKiloByteSize );

    /**
     * Locks the node database from the given client. That is, nodes are never
     * stored or searched from database. Only RAM cache is used.
     * 
     * @param aClientUid The client uid.
     */
    void LockNodeDbL( TUid aClientUid );
    
    /**
     * Unlocks the node database.
     *
     * @see CNcdNodeManager::LockNodeDb
     * @param aClientUid The client uid.
     */
    void UnlockNodeDb( TUid aClientUid );
    
    
public: // General tool functions   

    /**
     * Inserts the given child into the given parents child array.
     * Also, updates the cache if new items are created.
     *
     * @note This function also inserts the parent identifier into the
     * child node if the child is added to a parent.SetParentIdentifierL of
     * the link is used, which means that also the request parent identifier of the node link
     * is updated. Be careful when calling this function because problems may occur
     * if the request parent identifier of the transparent child node is not
     * updated correctly after this function. When RefHandlerL calls this function
     * it also updates the request parent identifier correctly. So, if possible
     * use RefHandlerL instead of this function.
     *
     * @note Parent id is node identifier and the child id is metadata identifier.
     *
     * @param 
     * @param aChildNodeMetaDataIdentifier The metadata identifier of the child
     * is used to add the child information to the parent node. 
     */
    void AddToParentL( const CNcdNodeIdentifier& aParentNodeIdentifier,
                       const CNcdNodeIdentifier& aChildNodeMetaDataIdentifier,
                       CNcdNodeFactory::TNcdNodeType aParentNodeType
                        = CNcdNodeFactory::ENcdNodeFolder,
                       CNcdNodeFactory::TNcdNodePurpose aParentNodePurpose
                        = CNcdNodeFactory::ENcdNormalNode,
                       CNcdNodeFactory::TNcdNodePurpose aNodePurpose 
                        = CNcdNodeFactory::ENcdNormalNode, 
                       TNcdRefHandleMode aMode = EUpdate,
                       TInt aIndex = 0,
                       TBool aTransparent = EFalse );
    
    TBool IsCapabilitySupportedL( const CNcdNodeIdentifier& aNodeIdentifier,
        const TDesC& aCapability, MCatalogsContext& aContext );

    /**
     * Tries to get an origin identifier from purchase history for a given node.
     *
     * @param aNodeIdentifier The node to get origin identifier for.
     * @return An origin identifier for the node, NULL if not found.
     */
    CNcdNodeIdentifier* GetOriginIdentifierL( const CNcdNodeIdentifier& aNodeIdentifier );


    /**
     * Removes folder's children from db unless they are favorites,
     * removes all (immediate) children from RAM cache and empties
     * the child array and saves the folder to disk
     */
    void RemoveChildrenL( CNcdNodeFolder& aFolder );

    /**
     * Removes folder's children's metadatas from disk
     */
    void RemoveChildrenMetadataL( CNcdNodeFolder& aFolder );
    
    
public: // CCatalogsCommunicable

    /**
     * @see CCatalogsCommunicable::ReceiveMessage
     */
    virtual void ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                 TInt aFunctionNumber );

    /**
     * @see CCatalogsCommunicable::CounterPartLost
     */
    virtual void CounterPartLost( const MCatalogsSession& aSession );


protected:

    /**
     * Constructor
     *
     * @param aPurchaseHistory Purchase history contains information
     * about node purchase phases.
     * @param aConfigurationManager Gives access to server capabilities etc.
     */
    CNcdNodeManager( CNcdGeneralManager& aGeneralManager );

    /**
     * ConstructL
     *
     * @param aStorageManager Storage manager provides means for
     * the manager to save, load, and remove data in db.
     */
    void ConstructL();


protected: // General getter and setter functions
    
    /**
     * @return CNcdNodeFactory& Reference to the node factory
     * that is used to create node objects.
     */
    CNcdNodeFactory& NodeFactory() const;

    /**
     * Sets the metadata for the given node if metadata can be found
     * from cache or from db.
     *
     * @param aNode Node that will have the metadata if it is found.
     */ 
    void SetNodeMetaDataL( CNcdNode& aNode );


protected: // Db functions

    /**
     * Searches the db for the given node. And creates the node if it is found.
     *
     * @return CNcdNode& Node whose data was found from the db.
     * @exception KErrNotFound if the node was not found from the db.
     */
    CNcdNode& DbNodeL( const CNcdNodeIdentifier& aIdentifier );

    /**
     * Searches the db for the given node metadata. 
     * And creates the node metadata if it is found.
     *
     * @return CNcdNodeMetaData& Node metadata whose data was found from the db.
     * @exception KErrNotFound if the node metadata was not found from the db.
     */
    CNcdNodeMetaData& DbMetaDataL( const CNcdNodeIdentifier& aIdentifier );


protected: // Functions that are called from the ReceiveMessageL, 
           // which is meant to be used by the client side.


    /**
     * Creates a root node according to the identifier that has
     * included into the message.
     * If corresponding node has already been created, the old
     * one will be returned and new one is not created.
     *
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy.
     */
    void RootNodeRequestL( MCatalogsBaseMessage& aMessage );

    /**
     * Creates a search root node according to the identifier that has
     * included into the message.
     * If corresponding node has already been created, the old
     * one will be returned and new one is not created.
     *
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy.
     */
    void SearchRootNodeRequestL( MCatalogsBaseMessage& aMessage );

    /**
     * Check RAM cache and DB for the node.
     * If corresponding node has already been created in cache, the old
     * one will be returned and new one is not created.
     *
     * @note This does not create the node if it is not found from the 
     * db.
     *
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy.
     */
    void NodeRequestL( MCatalogsBaseMessage& aMessage );

    /** 
     * Creates the temporary node that corresponds to the given identifier.
     * This node will not be inserted as child to some other folder.
     *
     * 
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy.
     * @param aType Type of the node to create.
     * @param aCreateMetaData If true, this function will create the dummy metadata
     * for the node if it does not already exist in RAM cache or in db.
     * The metadata is initialized normally from the purchase details.
     * So, the node created by this function will be initialized even if
     * its metadata would not contain any real info.
     */ 
    void TemporaryNodeRequestL( 
        MCatalogsBaseMessage& aMessage,
        TNcdTemporaryNodeType aType,
        TBool aCreateMetaData );
            
    /**
     * Creates temporary folder or item that has metadata that corresponds
     * to the given metadata identifier. This node will not be inserted as child
     * to some other folder. The node is not created if suitable one already exists.
     * If the metadata does not exist yet, it cannot be known whether a node or folder
     * should be created, so a supplier node is created.
     *
     * @param aMessage Contains the node identifier data from the proxy. Notice,
     * that this function only returns zero in message and not any usable value. 
     * NodeRequestL has to be called after this request to get the handle for 
     * the created node.
     */
    void TemporaryOrSupplierNodeRequestL( MCatalogsBaseMessage& aMessage );
    
    /**
     * Creates temporary folder or item that has metadata that corresponds to
     * the given metadata identifier. If the metadata does not exist already,
     * the node is not created.
     *
     * @param aMessage Contains the node identifier data from the proxy. This
     * function returns ETrue or EFalse in the message, depending on whether the
     * node was created or not.
     */ 
    void TemporaryNodeIfMetadataExistsRequestL(
        MCatalogsBaseMessage& aMessage );


    /**
     * This function is called from the proxy side when the proxy
     * is deleted.
     *
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy.
     */
    void ReleaseRequest( MCatalogsBaseMessage& aMessage );
    
    /**
     * Clears all search result nodes.
     *
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy.
     */
    void ClearSearchResultsRequestL( MCatalogsBaseMessage& aMessage );
        
    /**
     * Checks via conf manager whether a capability is supported by a server.
     * 
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy.
     */
     void IsCapabilitySupportedRequestL( MCatalogsBaseMessage& aMessage );


protected: // Functions that are called from the functions that receive messages.

    /**
     * Creates the identifier from the message that has been gotten from the proxy.
     * If the identifier does not already contain the UID value, it will be set here.
     *
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy. This function uses the message to get the identifier
     * that has been sent from the proxy side.
     */
    CNcdNodeIdentifier* RequestNodeIdentifierLC( MCatalogsBaseMessage& aMessage ) const;

    
private:

    // Prevent if not implemented
    CNcdNodeManager( const CNcdNodeManager& aObject );
    CNcdNodeManager& operator =( const CNcdNodeManager& aObject );


private: // Cache functions

    // Searches the cache for the node that corresponds to the given identifier.
    CNcdNode* FindNodeFromCacheL( const CNcdNodeIdentifier& aIdentifier );
    
    // Searches the main cache for the given node.
    CNcdNode* FindNodeFromMainCache( const CNcdNodeIdentifier& aIdentifier );

    // Searches the cache for the metadata that corresponds to the given identifier.
    CNcdNodeMetaData* FindNodeMetaDataFromCache( const CNcdNodeIdentifier& aIdentifier );

    inline TInt FindNodeFromArray( 
        const CNcdNodeIdentifier& aIdentifier,
        const RPointerArray<CNcdNode>& aArray ) const;

    inline TInt InsertNodeInOrder( 
        CNcdNode* aNode,
        RPointerArray<CNcdNode>& aArray );
        
    // Closes all the nodes that are not used any more.
    // This function is used when node manager things that it is
    // a good time to clean unnecessary nodes from the cache.
    // ( If access count of node is 1 the manager things that
    // the node is not used anymore. )
    void NodeCacheCleanup();

    // Checks if there are unnecesary meta data objects
    // ( no node is using the metadata ) in the cache.
    // Deletes unused metadata. 
    void MetaDataCacheCleanup();
    
    // Checks if any cached node uses the metadata    
    TBool IsMetadataUsed( const CNcdNodeMetaData* aMetadata ) const;

    // Closes all nodes and metadata objects     
    void FullCacheCleanup();

    // Closes all metadata objects
    void CloseMetadatas();
    
    // Appends the given node into the array.
    // The cache takes the ownership. So, remember to pop the
    // node from the cleanupstack after using this function.
    // This function should be used instead of directly appending data
    // to the cache because this function also updates info to 
    // the db cache cleaner.
    void AppendNodeToCacheL( CNcdNode* aNode );

    // This function should be used only in rare cases.
    // Usually only if the server has changed the types of the nodes.
    // Function will check if the node exists in RAM cache or in db.
    // If the node already exists, the type of the old node is compared
    // to the values that have been given as parameter. If the old node
    // is not of the right type, then the old node is removed from the RAM
    // cache and replaced by the new node that will be of the required type.
    // Notice, that this function does not replace the old node data in db.
    // If db should be updated, it has to be done separately.
    CNcdNode& CheckAndCreateNodeL( CNcdNodeFactory::TNcdNodeType aNodeType,
                                   CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                   const CNcdNodeIdentifier& aParentNodeIdentifier,
                                   const CNcdNodeIdentifier& aMetaIdentifier );

    // See description above.
    CNcdNode& CheckAndCreateNodeL( CNcdNodeFactory::TNcdNodeType aNodeType,
                                   CNcdNodeFactory::TNcdNodePurpose aNodePurpose,
                                   const CNcdNodeIdentifier& aNodeIdentifier );

    // Same as CheckAndCreateNodeL but for the metadata
    CNcdNodeMetaData& CheckAndCreateMetaDataL( const CNcdNodeIdentifier& aMetaIdentifier,
                                               CNcdNodeFactory::TNcdNodeType aMetaType );


    // Clears all client namespaces except subscriptions 
    // Favorites are not cleared if aClearFavorites is false
    // Downloads are not cleared if aClearDownloads is false
    void ClearClientNamespacesL( 
        const TUid& aClientUid, 
        TBool aClearFavorites,
        TBool aClearDownloads );

    // Clears namespaces that have favorites so that favorites are not cleared
    void ClearNamespacesWithFavoritesL( const TUid& aClientUid );
    
    // Clears the namespace aSortedArray[aIndex]->NodeNamespace() but
    // ignores nodes that are in aSortedArray. Also their metadatas and icons are
    // left intact.
    // aSortedArray must be sorted by namespaces
    // Client UID is taken from a CNcdNodeIdentifier
    TInt ClearNamespaceL(         
        const RPointerArray<CNcdNodeIdentifier>& aSortedArray, 
        TInt aIndex );
    
    // Loads and closes nodes and their metadatas using the identifiers
    // in array aNodeIds from index aStart to aEnd - 1
    // Leaves if either node or metadata load fails
    void CheckNodesL( 
        const RPointerArray<CNcdNodeIdentifier>& aNodeIds,
        TInt aStart,
        TInt aEnd );

    
    void RemoveNodeFromRamCache( 
        const CNcdNodeIdentifier& aIdentifier );

    void CommitSeenChanges( const MCatalogsContext& aContext );
    
private: // data

    // Db manager is used to save, load, and remove data in database.
    CNcdNodeDbManager* iDbManager;
    
    // Factory is used to create the nodes. The purpose of the factory
    // is to provide one class where all the nodes can be created. Thus,
    // when new nodes are implemented they can be always created by
    // using one factory class instead of creating nodes all around 
    // the code.
    CNcdNodeFactory* iNodeFactory;    

    // This object is used to manage db cache cleaners.
    CNcdNodeCacheCleanerManager* iNodeCacheCleanerManager;

    CNcdPreviewManager* iPreviewManager;

    CNcdFavoriteManager* iFavoriteManager;
    
    // Node seen info is used to keep track of seen nodes. Owned by this class.
    CNcdNodeSeenInfo* iSeenInfo;

    // Purchase history is used to load and to save purchase data from
    // database.
    CNcdPurchaseHistoryDb& iPurchaseHistory;    

    // This array contains all the nodes that have been created.
    // By checking the id of the nodes, the same node, that contains
    // the metadata, can be included to multiple containers without
    // copying it.
    RPointerArray<CNcdNode> iNodeCache;

    // This array contains all the nodes that have been created.
    // By checking the id of the nodes, the same node, that contains
    // the metadata, can be included to multiple containers without
    // copying it.
    RPointerArray<CNcdNodeMetaData> iNodeMetaDataCache;
    
    // This array is used to store nodes temporarily.
    RPointerArray<CNcdNode> iTempNodeCache;
    
    // Array of client uid whose database is locked.
    RArray<TInt32> iClientDatabaseLocks;

    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;
    
    MNcdConfigurationManager& iConfigurationManager;
    
    TLinearOrder<CNcdNode> iNodeOrder;
    CNcdSearchableNode* iSearchableNode;
    CNcdGeneralManager& iGeneralManager;
    };


#endif // NCD_NODE_MANAGER_H
