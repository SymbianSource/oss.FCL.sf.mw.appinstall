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
* Description:   Contains MNcdNodeDependency interface
*
*/


#ifndef M_NCD_NODE_DEPENDENCY_H
#define M_NCD_NODE_DEPENDENCY_H


#include <e32cmn.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"


class MNcdNodeDependencyInfo;



/**
 * Node dependency states
 *
 * @note Do not change the order of these enumerations
 */
enum TNcdDependencyState
    {
    /**
     * Dependency has not been installed or is not present
     */
    ENcdDependencyMissing = 0, 
    
    /**
     * Dependency is present but there is an upgrade available
     * on the server
     */
    ENcdDependencyUpgradeAvailable, 
    
    /**
     * Dependency is installed
     */
    ENcdDependencyInstalled    
    };


/**
 *  This interface provides functions for the node that depends on
 *  another nodes. The functions can be used to check what nodes
 *  or node content this node depends on.
 *
 *  @see MNcdNode
 *  @see MNcdNodeDependencyInfo
 *
 *  
 */
class MNcdNodeDependency : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeDependencyUid };


    /**
     * Returns the dependency information about the nodes or content
     * that this object depends from.
     *
     * @return Array of pointers to dependency information objects that 
     *         this node depends on.
     */
    virtual const RPointerArray< MNcdNodeDependencyInfo >& DependencyInfos() const = 0;


    /**
     * Returns the collective state of the dependencies.
     *
     * The returned value will be ENcdDependencyMissing if any of the node's
     * dependencies is missing, ENcdDependencyUpgradeAvailable if all of
     * node's dependencies are installed but at least one has an upgrade 
     * available or ENcdDependencyInstalled if all are installed and there are
     * no upgrades available
     *
     * @return Node's dependencies' state
     */
    virtual TNcdDependencyState State() const = 0;


protected:

    /**
     * Destructor.
     * 
     * @see MCatalogsBase::~MCatalogsBase
     */
    virtual ~MNcdNodeDependency() {}

    };


#endif // M_NCD_NODE_DEPENDENCY_H
