/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains MNcdNodeChildOfTransparent interface
*
*/


#ifndef M_NCD_NODE_CHILD_OF_TRANSPARENT_H
#define M_NCD_NODE_CHILD_OF_TRANSPARENT_H


#include <e32cmn.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"


class MNcdNode;

/**
 * This interface provides function to obtain the transparent parent of
 * a child of a transparent node. Transparent nodes are not normally
 * present in proxy side.
 *
 * This interface can be queried from node objects that are direct children of
 * transparent nodes.
 *
 * 
 */
class MNcdNodeChildOfTransparent : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeChildOfTransparentUid };
    
    /**
     * Returns the transparent parent of the node, or NULL if the parent
     * is not found from cache.
     *
     * @return The parent.
     * @leave System wide error code.
     */
    virtual MNcdNode* TransparentParentL() const = 0;

protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdNodeChildOfTransparent() {}

    };


#endif // M_NCD_NODE_CHILD_OF_TRANSPARENT_H
