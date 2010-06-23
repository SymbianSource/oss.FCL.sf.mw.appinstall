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
* Description:   Contains MNcdNodeSearch interface
*
*/


#ifndef M_NCD_NODE_SEARCH_H
#define M_NCD_NODE_SEARCH_H


#include "catalogsbase.h"
#include "ncdinterfaceids.h"


class MNcdSearchOperation;
class MNcdLoadNodeOperationObserver;
class MNcdSearchFilter;

/**
 *  Provides methods to start search operation for searching contents within
 *  containers. A container node offers this interface to enable searching within it.
 *
 *  
 */
class MNcdNodeSearch : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeSearchUid };


    /**
     * Creates a search operation.
     *
     * @param aObserver Observer for the search operation.
     * @param aSearchFilter A filter object defining the data to search for.
     * @return Pointer to the search operation, used for starting and following
     *  the progress of the search, and receiving the results. Counted, Release()
     *  must be called after use.
     * @exception Leave System wide error codes.
     *  KNcdErrorParallelOperationNotAllowed if a parallel client is running
     *  an operation for the same metadata. See MNcdOperation for full explanation.
     */
    virtual MNcdSearchOperation* SearchL( MNcdLoadNodeOperationObserver& aObserver,
        MNcdSearchFilter& aSearchFilter ) = 0;

protected:

    /**
     * Destructor
     * 
     * @see MCatalogsBase::~MCatalogsBase()
     */
    virtual ~MNcdNodeSearch() {}

    };


#endif // M_NCD_NODE_SEARCH_H
