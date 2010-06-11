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
* Description:  
*
*/


#ifndef M_NCDOPERATION_H
#define M_NCDOPERATION_H

#include <e32cmn.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"

class TNcdProgress;
class MNcdQuery;
class MNcdNode;

/**
 *  Main operation interface.
 *
 *  Operation objects are used in the Catalogs engine for potentially
 *  time consuming operations e.g. downloading items or loading node data
 *  from server. 
 *
 *  Operation objects function asynchronously. All operations have their own 
 *  observer interface that operation users should implement. Callbacks to
 *  observers occur at key points in an operation's execution. These points
 *  include progress events, query receival and operation completion.
 *
 *  All operations related to a node that they are started from. The node can
 *  be aqcuired via this interface. The intention is that operations mostly 
 *  just notify their observers of progress, completion etc. and that the 
 *  observers can get what they need via the node.
 *
 *  Operations can send querys to observers at any time. Querys always need
 *  to be completed before the operation can continue! Querys are mostly sent
 *  in situations where user interaction is needed to continue the operation
 *  (e.g. filling credit card information for a purchase operation).
 *
 *  @note Parallel operations from different clients for one node metadata are
 *  not supported. Requests for an operation for a node metadata that already
 *  has an active operation for another client, will fail with the error code
 *  KNcdErrorParallelOperationNotAllowed.
 *
 *  This affects only clients using the same family id, and only operations that
 *  are node-specific. Operation request methods that can cause this have this 
 *  potential exception in their comments.
 *
 *  Clients can handle this situation e.g. by waiting a moment and trying again
 *  to see if the other operation has completed. However, as a general rule
 *  situations where clients run in parallel and excecute operations on the
 *  same content should be avoided to allow smooth operation of said clients.
 *
 *  @see MNcdNode
 *  @see MNcdQuery
 *  
 */
class MNcdOperation : public virtual MCatalogsBase
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdOperationUid };
    
    /** 
     * Operation states.
     *
     * 
     */
    enum TState
        {
        /** Nothing done yet, initial state. */
        EStateStopped,
        
        /** Operation is running. */
        EStateRunning,
        
        /** Waiting query completion. */
        EStateQuery,
        
        /** Operation complete. */
        EStateComplete,
        
        /** Operation cancelled */
        EStateCancelled
        
        };

    /**
     * Getter for operation state. It can be useful to find
     * out the operations state in situations where the operation
     * is not actively monitored (via observer) or where an operation
     * is left in an unactive state and later resumed.
     *
     * @return State of the operation.
     */
    virtual TState OperationStateL() const = 0;
    
    /**
     * Type of the operation. This can be used to easily get the 
     * correct interface via QueryInterfaceL.
     *
     * @return Interface id of the operation interface.
     */
    virtual TNcdInterfaceId OperationType() const = 0;
    
    /**
     * Starts operation, an asynchronous function.
     *
     * @exception KErrInUse if operation already started.
     * @exception KErrCancel if operation has been cancelled
     */
    virtual void StartOperationL() = 0;
    
    /**
     * Cancel operation, a synchronous function.
     *
     * OperationComplete() will be called for each observer with KErrCancel
     * as the error parameter.
     *
     * @note Operation can not be started again.
     */
    virtual void CancelOperation() = 0;

    /**
     * Returns current progress information immediately.
     * Can be safely used during an ui update. Users should always use this
     * method when displaying progress information for operations.
     *
     * Both made progress and maximum progress can change during an operation
     * e.g. when file size information is received for a download operation.
     *
     * Progress information should never be used to decide whether an 
     * operation has been completed! Operation completion is always explicitly
     * notified via an observer callback!
     * 
     * @return Structure describing operation progress.
     */
    virtual TNcdProgress Progress() const = 0;

    /**
     * Notifies the operation that the current query has been completed. This
     * must be called to continue the operation if a query is pending! 
     * The query is acquired in the operation callback.
     *
     * Before completion, the query object needs to be set to a valid state.
     * This means that all its non-optional items need to be set to valid
     * values and the query is accepted, OR the query is declined/cancelled.
     *
     * @exception KErrNotFound No query object pending for the operation.
     *            KErrNotReady Query object is not in a valid state.
     */
    virtual void CompleteQueryL( MNcdQuery& aQuery ) = 0;
    
    /**
     * Getter for the node that this operation was started from.
     * This can be used in callbacks to easily access the related node.
     *
     * @note AddRef() is called to the node so the user must call Release()
     *       when no longer needed.
     * @return Pointer to the node object that originated this operation.
     *         Counted, Release() must be called after use.
     */
    virtual MNcdNode* Node() = 0;

protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdOperation() {}

    };
	
	
#endif //  M_NCDOPERATION_H
