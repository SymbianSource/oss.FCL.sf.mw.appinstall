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
* Description:   Contains MNcdNodeContainer interface
*
*/


#ifndef M_NCD_NODE_CONTAINER_H
#define M_NCD_NODE_CONTAINER_H


#include <e32cmn.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"
#include "ncdchildloadmode.h"
#include "ncdcontainertype.h"


class MNcdNode;
class MNcdLoadNodeOperation;
class MNcdLoadNodeOperationObserver;

/**
 *  Container may contain other nodes.
 *  This interface provides functions that can be used to
 *  get the nodes of the container. Also, a convenience function
 *  to load the content for the child nodes is provided.
 *
 *  This interface can be queried from node objects that are containers,
 *  after the node has been initialized.
 *
 *  
 */
class MNcdNodeContainer : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeContainerUid };
    
    /**
     * Returns the child node count for this container. The count may be
     * used to get the upper limit for indexing the contained child nodes
     * with the ChildL() method.
     *
     * @return The number of the children in this container.
     */
    virtual TInt ChildCount() const = 0;


    /**
     * Returns a child (contained node) of this container.
     *
     * @note The reference count of the node is increased by one. So,
     * Release function of the node should be called when the node
     * is not needed anymore.
     *
     * @param aIndex is the index to the child of this container.
     * @return Pointer to the child node, may be an item or another container.
     *  Counted, Release() must be called after use.
     * @exception Leave System wide error code.
     * @exception Panic ENcdPanicIndexOutOfRange Index out of range.
     */
    virtual MNcdNode* ChildL( TInt aIndex ) = 0;


    /**
     * Loads node information from the content provider for the
     * children of this container.
     *
     * The loading of children is started from the given index.
     * Size informs the maximum number of children that are loaded.
     *
     * @param aIndex The index of the child that will be the first one
     *  to be loaded.
     * @param aSize The maximum number of children to load.
     *  For special cases: if KMaxTInt is given, then it will be
     *  interpreted so that all the children after aIndex should be loaded. 
     *  By using KMaxTInt, a parent container refresh may be skipped and 
     *  all the children may be requested from server even if parent's 
     *  child count information may not be valid. In normal cases, usage of 
     *  KMaxTInt should be avoided. Instead, refresh the parent first and 
     *  use the correct child count value. KMaxTInt may not be supported 
     *  for all type of containers. If not supported, then a leave will occur.  
     * @param aMode Load mode, determines whether structure, metadata or both
     *  will be loaded.
     * @param aObserver Observer for the operation.
     * @return Pointer to an operation that can 
     *  be used to check the progressing of the loading. Counted,
     *  Release() must be called after use.
     * @exception Leave System wide error code.
     *  KErrNotFound if trying to load metadata and no structure is present.
     *  KNcdErrorParallelOperationNotAllowed if a parallel client is running
     *  an operation for the same metadata, see MNcdOperation for full explanation.
     */
    virtual MNcdLoadNodeOperation* LoadChildrenL( 
        TInt aIndex, TInt aSize, TNcdChildLoadMode aMode,
        MNcdLoadNodeOperationObserver& aObserver ) = 0;
        
    /**
     * Tells the type of the folder. It may be a normal folder or a catalog.
     *
     * @return The type.
     */
    virtual TNcdContainerType ContainerType() const = 0;


protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdNodeContainer() {}

    };


#endif // M_NCD_NODE_CONTAINER_H
