/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains MNcdNodeSeenFolder
*
*/


#ifndef M_NCDNODESEENFOLDER_H
#define M_NCDNODESEENFOLDER_H

#include "catalogsbase.h"
#include "ncdinterfaceids.h"
#include "catalogsarray.h"

class MNcdNode;

/**
 * This interface provides functions to obtain unseen child nodes of a folder and
 * to set them seen. This interface can be queried from a folder node.
 *
 * 
 */
class MNcdNodeSeenFolder : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeSeenFolderUid };
    
    /**
     * Sets the child nodes of the folder as seen. Only the immediate children are set.
     *
     * @leave System wide error code.
     */
    virtual void SetContentsSeenL() = 0;    
    
    /**
     * Returns the number of new child nodes. All the nodes that are unseen are
     * regarded as new.
     *
     * @param aLevels The number of node hierarchy levels the new nodes are searched from.
     * 1 means only the children of this node are searched.
     * @return The number of new items.
     * @leave System wide error code.
     */
    virtual TInt NewCountL( TInt aLevels ) const = 0;
    
    /**
     * Returns an array of the new child nodes. All the nodes that are unseen are
     * regarded as new.
     *
     * The nodes in the array are counted, they must be released after use.
     *
     * @param aLevels The number of node hierarchy levels the new nodes are searched from.
     * 1 means only the children of this node are searched.
     */
    virtual RCatalogsArray<MNcdNode> NewNodesL( TInt aLevels ) const = 0;
    
protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdNodeSeenFolder() {}

    };


#endif // M_NCDNODESEENFOLDER_H
