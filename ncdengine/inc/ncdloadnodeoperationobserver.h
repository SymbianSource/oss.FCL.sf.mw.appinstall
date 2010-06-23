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
* Description:   ?Description
*
*/
	

#ifndef M_NCD_LOAD_NODE_OPERATION_OBSERVER_H
#define M_NCD_LOAD_NODE_OPERATION_OBSERVER_H

#include <e32cmn.h>
#include "catalogsarray.h"

class MNcdLoadNodeOperation;
class MNcdQuery;
class MNcdNode;

/**
 *  Observer interface for node load operations
 *
 *  Node load operation users should implement this interface to be able to
 *  receive callbacks.
 *
 *  The observer can show progress via either the progress variable or by
 *  loaded nodes count / total count of nodes to load or both.
 *
 *  
 *  @see MNcdLoadNodeOperation
 */
class MNcdLoadNodeOperationObserver
    {
    
public:

    /**
     * Called when nodes have been updated. This may be called numerous 
     * times during one operation. The nodes can now be queried for name,
     * description etc. information.
     *
     * 
     * @param aOperation Reference to the operation reporting the updates.
     *  Uncounted; if the observer implementation wants to save
     *  the operation pointer for later use, it must call AddRef() for
     *  it first, and Release() after use.
     * @param aNodes Array of pointers to updated nodes. The references
     *  in the array are uncounted; if the observer implementation wants to
     *  save a node pointer for later use, it must call AddRef() for it first,
     *  and Relese() after is no longer needed.
     */
    virtual void NodesUpdated( MNcdLoadNodeOperation& aOperation,
                               RCatalogsArray< MNcdNode >& aNodes ) = 0;
    
    /**
     * Called when a query has been received for an operation. The query
     * object can be obtained from the operation object with
     * MNcdOperation::QueryL().
     *
     * @note Querys need to be completed with MNcdOperation::CompleteQueryL()
     *       to continue the operation! Alternatively, the operation must be
     *       cancelled.
     * 
     * 
     * @see MNcdOperation
     * @see MNcdQuery
     * @param aOperation Uncounted reference to the operation object that sent
     *  the event.
     * @param aQuery Pointer to the query that must be completed. Counted, Release()
     *        must be called after use.
     * @see MNcdOperation::CompleteQueryL()
     */
    virtual void QueryReceived( MNcdLoadNodeOperation& aOperation,
                                MNcdQuery* aQuery ) = 0;

    /**
     * Called when an operation has been completed.
     *
     * 
     * @param aOperation Reference to the operation that sent the event.
     *  Uncounted; if the observer implementation wants to save
     *  the operation pointer for later use, it must call AddRef() for
     *  it first, and Release() after use.
     * @param aError Error code for operation completion. 
     *               KErrNone for successful completion, 
     *               KErrNoMasterServerUri if the master server URI has not been set 
     *               (possible only when loading root node),
     *               otherwise a system wide error code.
     */
    virtual void OperationComplete( MNcdLoadNodeOperation& aOperation,
                                    TInt aError ) = 0;
    };
    
#endif //  M_NCD_LOAD_NODE_OPERATION_OBSERVER_H
