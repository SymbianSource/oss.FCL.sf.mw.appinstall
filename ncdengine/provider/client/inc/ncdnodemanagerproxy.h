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
* Description:   Contains CNcdNodeManagerProxy class
*
*/


#ifndef NCD_NODE_MANAGER_PROXY_H
#define NCD_NODE_MANAGER_PROXY_H


#include <e32base.h>
#include <e32cmn.h>

#include "ncdbaseproxy.h"
#include "ncdschemenodetype.h"


class CNcdNodeProxy;
class CNcdRootNodeProxy;
class CNcdNodeFolderProxy;
class CNcdOperationManagerProxy;
class CNcdSubscriptionManagerProxy;
class CNcdNodeIdentifier;
class CNcdSearchRootNodeProxy;
class CNcdProviderProxy;
class CNcdExpiredNode;
class MNcdPurchaseDetails;
class CNcdFavoriteManagerProxy;

/**
 *  CNodeManagerProxy provides functions to ...
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeManagerProxy : public CNcdBaseProxy
    {

public:

    /**
     * NewL
     *
     * @return CNcdNodeManagerProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeManagerProxy* NewL(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdOperationManagerProxy& aOperationManager,
        CNcdSubscriptionManagerProxy& aSubscriptionManager,
        CNcdProviderProxy& aProvider );

    /**
     * NewLC
     *
     * @return CNcdNodeManagerProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeManagerProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdOperationManagerProxy& aOperationManager,
        CNcdSubscriptionManagerProxy& aSubscriptionManager,
        CNcdProviderProxy& aProvider );


    /**
     * Destructor
     *
     * Deletes the nodes from the node cache.
     */
    virtual ~CNcdNodeManagerProxy();


public: // Functions to get nodes if they exist in the server side

    /**
     * Creates a node according to the given id.
     * If corresponding node has already been created, the old
     * one will be returned and new one is not created.
     *
     * @note If server side does not contain the node this function
     * will leave with KErrNotFound and does not return NULL in that case.
     *
     * @param 
     * @return 
     * @exeption Leaves with KErrNotFound If node did not exist.
     */
    CNcdNodeProxy& NodeL( const CNcdNodeIdentifier& aNodeIdentifier );


public: // Functions that will create nodes in the server side if they do not
        // already exist.

    /**
     * Creates the root node.
     * If corresponding node has already been created, the old
     * one will be returned and new one is not created.
     *
     * @return 
     */
    CNcdRootNodeProxy& RootNodeL();

    /**
     * Creates the search root node.
     * If corresponding node has already been created, the old
     * one will be returned and new one is not created.
     *
     * @return CNcdSearchRootNodeProxy& 
     */
    CNcdSearchRootNodeProxy& SearchRootNodeL();


    /**
     * When aForceCreate is ETrue, function forces the server side to create a node 
     * with the given id if the node is not found from the cache.
     * The node will not have any parent. If aForceCreate is EFalse, then
     * scheme node is returned only if it already exists or if the metadata exists
     * in RAM or database cache. The scheme node is added to favourites.
     *
     * @note If the corresponding node already exists, then instead of creating a
     * new node, the existing node is returned.
     *
     * @param aMetadataIdentifier
     * @param aRemoveOnDisconnect If true, the node is removed from favorites when the
     * client-server session is closed.
     * @param aType Type of the scheme node to create.
     * @param aForceCreate Determines if the node will always be created.
     * @return CNcdNodeProxy&
     * @exception Leave KErrNotFound if aForceCreate has been set as EFalse,
     * and the node was not created.
     */
    CNcdNodeProxy& CNcdNodeManagerProxy::CreateSchemeNodeL(
        const CNcdNodeIdentifier& aMetadataIdentifier,
        TNcdSchemeNodeType aType,
        TBool aRemoveOnDisconnect,
        TBool aForceCreate );
                                                  

    /**
     * Forces the server side to create a temporary node by using the purchase details
     * information. If the node already exists in the cache then that node will be
     * returned.
     * This node will not be a child of any other node. So, it is up to user
     * what to do with the node.
     * 
     * @note This function assumes that purchase details contain the information that
     * is required to initialize the metadata if it can not be found from the database or
     * from the cache.
     *
     * @param aDetails Purchase details are used to create correct form of the node.
     * @return CNcdNodeProxy&
     */
    CNcdNodeProxy& CreateTemporaryNodeL( const MNcdPurchaseDetails& aDetails );

    /**
     * Forces the server side to create a temporary node folder and inserts a metadata for it.
     * This node will not be a child of any other node. So, it is up to user
     * what to do with the node.
     * 
     * @param aMetadataIdentifier
     * @param aCreateMetaData If true, this function will create a dummy metadata if the
     * metadata does not already exist. The dummy metadata initializes itself from the
     * purchase history if the purchase history contains corresponding info. 
     * @return CNcdNodeProxy&
     */
    CNcdNodeProxy& CreateTemporaryNodeFolderL(
        const CNcdNodeIdentifier& aMetadataIdentifier,
        TBool aCreateMetaData );

    /**
     * This function works like CreateTemporaryNodeFolderL but cretes a bundle folder
     * instead.
     *
     * @see CNcdNodeManagerProxy::CreateTemporaryNodeFolderL
     */
    CNcdNodeProxy& CreateTemporaryBundleFolderL(
        const CNcdNodeIdentifier& aMetadataIdentifier,
        TBool aCreateMetaData );
    
    /**
     * This function works like CreateTemporaryNodeFolderL but cretes an item
     * instead.
     *
     * @see CNcdNodeManagerProxy::CreateTemporaryNodeFolderL
     */
    CNcdNodeProxy& CreateTemporaryNodeItemL(
        const CNcdNodeIdentifier& aMetadataIdentifier,
        TBool aCreateMetaData );
    
    /**
     * This function checks the node cache for the old temporary node. If the node exists it
     * will be used. If the node does not exist, the node will be created to server side
     * if the metadata exists there already. Correct type of the node is concluded from the
     * type of the metadata. If the metadata does not exist, the node is not created and
     * the function will leave.
     *
     * @param aMetadataIdentifier The metadata identifier.
     * @return The node.
     * @leave KErrNotFound If the node cannot be created since metadata does not exist.
     */
    CNcdNodeProxy& CreateTemporaryNodeIfMetadataExistsL(
        const CNcdNodeIdentifier& aMetadataIdentifier );
    
    /**
     * This function checks the node cache for the old node. If the node exists it will be used.
     * If the node does not exist, the node will be asked from the server side node manager.
     * Then, supplier node or temporary item or temporary folder proxy will be created.
     *
     * @note This function creates the supplier if the temporary node or corresponding metadata
     * did not exist in the server side cache or in db. Unlike temporary node functions above,     
     * this function does not create the metadata if it does not exist in database.
     *
     * @note Supplier node will use CNcdNodeManagerProxy::ReplaceCacheNodeL to replace
     * the supplier node in cache, when temporary node can be created and used. 
     *
     * @param aMetadataIdentifier
     */
    CNcdNodeProxy& CreateTemporaryOrSupplierNodeL( const CNcdNodeIdentifier& aMetadataIdentifier );

    /** 
     * This function can be used if some node in cache list is
     * wanted to be replaced by the new proxy node.
     * This may the case if the original node type was not known 
     * when the node was created as a supplier node. 
     * After loading the node data from the web server,
     * a temporary node of the correct type is created in server side. 
     * Then, that temporary node can be used instead.
     *
     * @note If the node is found from the server side, old node object is removed
     * from the proxy ram cache and replaced by the new node object of the same
     * node identifier. UI may still use the old node object
     * if it owns it but after calling this function the manager will
     * return pointer directly to the new node object instead. 
     * 
     * @param aNodeIdentifier The identifier of the node that will be replaced by the new
     * node object.
     * @return CNcdNodeProxy&
     * @exception Leaves with KErrNotFound if the node did not exist in 
     * the server side.
     */
    CNcdNodeProxy& ReplaceCacheNodeL( const CNcdNodeIdentifier& aNodeIdentifier );


public:

    /**
     * Checks if the node exists in the cache.
     * @param aNodeIdentifier identifies the node that is looked for.
     * @return CNcdNodeProxy* pointer to the node that can be found from
     * the cache. NULL if the node was not found. Ownership is NOT transferred.
     */
    CNcdNodeProxy* NodeExists( const CNcdNodeIdentifier& aNodeIdentifier ) const;


    /**
     * This informs the manager that the given node is deleted.
     * Because the node is deleted, it has to be removed from the cache.
     * 
     * @param aNode The node that was or will be deleted.
     */
    void NodeDeleted( CNcdNodeProxy* aNode );

    /**
     * This function internalizes the given node and all the nodes
     * whose metadata part of the identifier matches the given node.
     * The related nodes are gotten from the node cache of this
     * node manager proxy. This function is provided to make sure
     * that if operations use temporary nodes and the API user may 
     * still have the handle to the original node, the operation may 
     * also update the original nodes instead of just the temporary 
     * one.
     *
     * @param aNode Node that will be internalized and used to 
     * search for other related nodes for internalization.
     */
    void InternalizeRelatedNodesL( CNcdNodeProxy& aNode ) const;

    /**
     * Getter for subscription manager.
     * 
     * @return Reference to subscription manager.
     */
    CNcdSubscriptionManagerProxy& SubscriptionManager() const;
    
    /**
     * Setter for favorite manager. This must be set always!
     *
     * @param aFavoriteManager The favorite manager.
     */
    void SetFavoriteManager( CNcdFavoriteManagerProxy& aFavoriteManager );
    
    /**
     * Clears all search result nodes.
     */
    void ClearSearchResultsL();


    /**
     * 
     * @param aServerUri Server URI
     * @param aNamespace Namespace
     */
    TBool IsCapabilitySupportedL( const CNcdNodeIdentifier& aNode, 
        const TDesC& aCapability );

    /**
     * Internalizes expired nodes and notifies api users via provider.
     *
     * @param aExpiredNodes A list of expired nodes in need of re-loading.
     */        
    void HandleExpiredNodesL( RPointerArray< CNcdExpiredNode >& aExpiredNodes );


    /**
     * @return CNcdProviderProxy& Reference to the provider.
     */
    CNcdProviderProxy& Provider() const;    
    
    
protected:

    /**
     * Constructor
     */
    CNcdNodeManagerProxy( 
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdOperationManagerProxy& aOperationManager,
        CNcdSubscriptionManagerProxy& aSubscriptionManager,
        CNcdProviderProxy& aProvider );

    /**
     * ConstructL
     */
    virtual void ConstructL();


private:

    // Prevent if not implemented
    CNcdNodeManagerProxy( const CNcdNodeManagerProxy& aObject );
    CNcdNodeManagerProxy& operator =( const CNcdNodeManagerProxy& aObject );

/*
    CNcdNodeProxy& CNcdNodeManagerProxy::CreateSchemeNodeL( 
        const CNcdNodeIdentifier& aMetadataIdentifier,
        TInt aFunctionId );
*/
    CNcdNodeProxy& CreateNodeL( const CNcdNodeIdentifier& aNodeIdentifier );


private: // data

    // This manager is required when nodes are created.
    // Because they should now the operation manager which
    // they use to ask and create operations.
    CNcdOperationManagerProxy& iOperationManager;

    // This manager is required when nodes are created.
    // Because their purchase should know the manager.
    CNcdSubscriptionManagerProxy& iSubscriptionManager;
    
    // This manager is required when nodes are create since
    // it must be given to all the nodes. Not own.
    CNcdFavoriteManagerProxy* iFavoriteManager;

    // This array contains all the nodes that have been created.
    RPointerArray<CNcdNodeProxy> iNodeCache;

    // Root node under which everything else is located.    
    CNcdRootNodeProxy* iRootNode;
    
    // Search root node under which all search results are located.
    CNcdSearchRootNodeProxy* iSearchRootNode;
    
    // Needed for expiration callback
    CNcdProviderProxy& iProvider;
    };


#endif // NCD_NODE_MANAGER_PROXY_H
