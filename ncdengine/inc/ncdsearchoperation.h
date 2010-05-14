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
* Description:   Search operation interface definition
*
*/
	

#ifndef M_NCDSEARCHOPERATION_H
#define M_NCDSEARCHOPERATION_H

#include "ncdloadnodeoperation.h"
#include "ncdinterfaceids.h"
#include "ncdnodecontainer.h"

class MNcdSearchFilter;

/**
 *  Search operation interface.
 *
 *  Search operation searches for nodes according to a set search filter. The search
 *  is done starting from the folder from which the MNcdNodeSearch interface was
 *  queried. All items and folders below the start folder are searched for recursively, 
 *  the results are returned as a flat list under the virtual search result folder that
 *  can be queried with MNcdSearchOperation::SearchRootNodeL()
 * 
 *  The search is conducted on the node hierarchy below the node that the
 *  operation is started from. The search results are represented as a separate
 *  temporary node hierarchy under the provider.
 *
 *  
 *  @see MNcdNodeSearch
 *  @see MNcdSearchFilter
 */
class MNcdSearchOperation : public MNcdLoadNodeOperation

    {

public: // New functions

    
    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdSearchOperationUid };


    /**
     * Gets the search result hierarchy root node.
     *
     * 
     * @return Search result root node. The search results will be loaded here.
     *  Counted, Release() must be called after use.
     * @exception Leave System wide error code
     */
    virtual MNcdNodeContainer* SearchRootNodeL() = 0;


protected:


    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdSearchOperation() {}

    };

#endif //  M_NCDSEARCHOPERATION_H
