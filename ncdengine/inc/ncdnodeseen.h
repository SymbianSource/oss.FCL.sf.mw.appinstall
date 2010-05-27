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
* Description:   Contains MNcdNodeSeen
*
*/


#ifndef M_NCDNODESEEN_H
#define M_NCDNODESEEN_H

#include "catalogsbase.h"
#include "ncdinterfaceids.h"


/**
 * This interface can be used to set a node as seen, or ask the 'seen' state
 * of the node. This is useful if new nodes should be separated from the seen
 * ones somehow. This interface can be queried from a node. If the node is
 * set as seen, all the nodes that have the same metadata are automatically
 * set as seen.
 *
 * 
 */
class MNcdNodeSeen : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeSeenUid };
    
    /**
     * Tells whether the metadata is seen.
     */
    virtual TBool IsSeen() const = 0;
    
    /**
     * Sets the node as seen. Note, the effect takes place when the client-server session
     * is closed. All the other nodes having the same metadata are set as seen too.
     */
    virtual void SetSeenL() = 0;
    
protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdNodeSeen() {}

    };


#endif // M_NCDNODESEEN_H
