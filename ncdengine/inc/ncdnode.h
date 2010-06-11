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
* Description:   Contains MNcdNode interface
*
*/


#ifndef M_NCD_NODE_H
#define M_NCD_NODE_H


#include <e32cmn.h>
#include <e32std.h>

#include "catalogsbase.h"
#include "catalogsarray.h"
#include "ncdinterfaceids.h"


class MNcdNodeContainer;
class MNcdLoadNodeOperation;
class MNcdLoadNodeOperationObserver;
class MNcdOperation;


/**
 *  Node contains the basic information about the content.
 *  The interface also provides a load function that must be used to
 *  initialize the node, before accessing other information.
 *
 *  When the UI gets the node, the node may not contain any information
 *  except from the node state. The state may be checked by calling
 *  MNcdNode::State(). MNcdNode::LoadL() can then be called to load
 *  the content for the node, if the state is uninitialized or expired.
 *
 *  @note The actual content information, such as name, description etc. are
 *  available through the MNcdNodeMetadata interface that can be queried from
 *  node objects. There are various other additional node interfaces as well
 *  that all offer different functionality and information about the node. The
 *  interfaces are not available at all times, they have different criteria for
 *  becoming available. For example, the MNcdNodeMetadata interface becomes
 *  available for a node when its metadata is loaded, so if it is not available
 *  at first, it can be made available by calling the node's MNcdNode::LoadL()
 *  method.
 *
 *  
 *  @see MNcdNodeMetadata
 *  @see MNcdNodeContainer
 *  @see MNcdNodeContent
 *  @see MNcdNodeDependency
 *  @see MNcdNodeDownload
 *  @see MNcdNodeIcon
 *  @see MNcdNodeInstall
 *  @see MNcdNodePreview
 *  @see MNcdNodePurchase
 *  @see MNcdNodeScreenshot
 *  @see MNcdNodeSearch
 *  @see MNcdNodeSkin
 *  @see MNcdNodeSubscribe
 *  @see MNcdNodeUpgrade
 *  @see MNcdNodeUriContent
 *  @see MNcdNodeUserData
 */
class MNcdNode : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeUid };


    /**
     * TNcdNode::TState gives information in what state the node is at the moment.
     * This information can be used to check if the node information should be
     * loaded over network or from local cache.
     *
     * 
     */
    enum TState
        {
        /**
         * Node is not initialized and the data should
         * be loaded from either local cache, or over network.
         */
        EStateNotInitialized,

        /** Node data is up to date. */
        EStateInitialized,

        /** Node data is expired and should be reloaded over network. */
        EStateExpired
        };


    /**
     * Gives information about the state of the node. This information can be used to
     * check whether the item data has been loaded or if the data is expired.
     * 
     * 
     * @return The state of the node.
     */
    virtual TState State() const = 0;


    /**
     * Gets the node id.
     *
     * 
     * @return Id of the node.
     */
    virtual const TDesC& Id() const = 0;


    /**
     * All nodes and their content are part of a namespace. Within the
     * namespace, node identifiers are unique. This function returns the namespace
     * of this node and its contents.
     *
     * 
     * @return The namespace identifier. If the protocol has not defined
     *         any value, an empty descriptor is returned.
     */
    virtual const TDesC& Namespace() const = 0;


    /**
     * Gets the name of the catalog source for the node.
     *
     * 
     * @return Name of the catalog source where the node is loaded from. An
     *  empty string is returned if the source name is not defined.
     */
    virtual const TDesC& CatalogSourceName() const = 0;


    /**
     * Retrieves the reference to the parent of this node.
     *
     * 
     * @return The parent of this node. NULL if the node
     *  has no parent. Counted, Release() must be called
     *  after use.
     * @exception Leave System wide error code, 
     *  KErrNotFound if the parent node is not loaded.
     */
    virtual MNcdNodeContainer* ParentL() const = 0;
    

    /**
     * Loads node information from the content provider.
     *
     * @note Should be called when a folder node needs to be refreshed, clears all
     *  child nodes for folders!
     *
     * 
     * @param aObserver Observer for the operation.
     * @return Pointer to an operation object that can be
     *  used to check the loading progress. Counted, Release() must be called
     *  after use.     
     * @exception Leave System wide error code.
     *  KNcdErrorParallelOperationNotAllowed if a parallel client is running
     *  an operation for the same metadata. See MNcdOperation for full explanation.
     */
    virtual MNcdLoadNodeOperation* LoadL( MNcdLoadNodeOperationObserver& aObserver ) = 0;


    /**
     * Returns a list of currently active operations of this node for progress
     * monitoring and operation management.
     *
     * @note Operations may also be other than just MNcdLoadNodeOperations.
     *
     * @return List of currently active operations for the node. Counted,
     *  each pointer must be released after use e.g. with a call to the array's
     *  RCatalogsArray::ResetAndDestroy().
     * @exception Leave System wide error code
     */
    virtual RCatalogsArray< MNcdOperation > OperationsL() const = 0;
    
    
    /**
     * Adds the node to favorite nodes. If the node is already
     * a favorite node, does nothing.
     *
     * @exception Leave System wide error code 
     */
    virtual void AddToFavoritesL() = 0;
        
    
protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdNode() {}
    
    };


#endif // M_NCD_NODE_H
