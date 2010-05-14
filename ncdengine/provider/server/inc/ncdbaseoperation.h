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


#ifndef C_NCDBASEOPERATION_H
#define C_NCDBASEOPERATION_H

#include <e32base.h>

#include "catalogscommunicable.h"
#include "ncdsendableprogress.h"
#include "ncdoperationfunctionids.h"
#include "catalogshttpobserver.h"
#include "catalogshttptypes.h"
#include "catalogshttpsession.h"
#include "ncdparserobserver.h"
#include "catalogsdebug.h"
#include "ncdoperationobserver.h"
#include "ncdoperationtype.h"

class MCatalogsBaseMessage;
class MNcdSendable;
class CNcdQuery;
class MNcdConfigurationProtocolQueryResponse;
class MNcdOperationRemoveHandler;
class CNcdNodeManager;
class MNcdParser;
class CNcdAsyncRunner;
class CNcdNodeIdentifier;
class CNcdExpiredNode;
class CNcdRequestBase;
class CNcdGeneralManager;

/**
 *  Base class for all operations.
 *
 *  Implements common operation functionality.
 *
 *  @lib ?library
 *  @since S60 ?S60_version
 */
class CNcdBaseOperation : public CCatalogsCommunicable,
                          public MNcdParserQueryObserver,
                          public MNcdParserInformationObserver,
                          public MNcdParserErrorObserver,
                          public MNcdOperationObserver
    {

public:

    /** Operation states */
    enum TOperationState
        {
        EStateStopped, // Op not started, initial state.
        EStateRunning, // Op started and running.
        EStateQuery,   // Op started but waiting query completion.
        EStateSendExpirationInfo, // Expiration info needs to be sent next.
        EStateComplete,// Op is completed
        EStateCancelled
        };

    virtual ~CNcdBaseOperation();

    /**
     * Starts the operation.
     */
    virtual TInt Start();
    
    /**
     * Cancels the operation. Returns the object to it's initial
     * state, made progress is lost.
     */
    virtual void Cancel() = 0;
    
    /**
     * Sets an observer for this operation.
     *
     * @param aObserver An observer that will receive progress and completion
     *                  callbacks from this operation.
     */
    virtual void AddObserverL( MNcdOperationObserver* aObserver );
    
    /**
     * Removes an observer from this operation.
     *
     * @param aObserver An observer to remove.
     * @return KErrNotFound if the observer is not found, otherwise KErrNone.
     */
    virtual TInt RemoveObserver( MNcdOperationObserver* aObserver );
    
    /**
     * Retrieves the type of this operation
     *
     * @return the type of this operation
     */
    virtual TNcdOperationType Type() const;
    
    /**
     * Retrieves the state of this operation
     *
     * @return the type of this operation
     */
    //virtual TOperationState State();
    
    /**
     * Handler for start messages.
     *
     * @param aMessage The received message.
     */
    virtual void HandleStartMessage( MCatalogsBaseMessage* aMessage );
    
    /**
     * Handler for cancel messages.
     *
     * @param aMessage The received message.
     */
    virtual void HandleCancelMessage( MCatalogsBaseMessage* aMessage );
    
    /**
     * Handler for continue messages.
     *
     * @param aMessage The received message.
     */
    virtual void HandleContinueMessage( MCatalogsBaseMessage* aMessage );
    
    /**
     * Handler for query response messages.
     *
     * @param aMessage The received message.
     */
    virtual void HandleQueryResponseMessage( MCatalogsBaseMessage* aMessage );
    
    
    /**
     * Handler for initialization messages
     *
     * @param aMessage The received message
     */
    virtual void HandleInitMessage( MCatalogsBaseMessage* aMessage );
    
    
    /**
     * Handler for release messages
     *
     * @param aMessage The received message
     */
    virtual void HandleReleaseMessage( MCatalogsBaseMessage* aMessage );
    
    
    /**
     * Completes a message with a completion id and a MNcdSendable 
     * object.
     *
     * NOTE: aMessage is always released and set to null.
     *
     * @param aMessage The message to complete.
     * @param aId Id for the message e.g. progress.
     * @param aSendableObject A object to send.
     * @param aStatus Error code.
     * @return System-wide error code.
     */
    virtual TInt CompleteMessage( MCatalogsBaseMessage* & aMessage,
        TNcdOperationMessageCompletionId aId,
        const MNcdSendable& aSendableObject,
        TInt aStatus );
        
    /**
     * Completes a message with a completion id.
     *
     * NOTE: aMessage is always released and set to null.
     *
     * @param aMessage The message to complete.
     * @param aId Id for the message e.g. progress.
     * @param aStatus Error code.
     * @return System-wide error code.
     */
    virtual TInt CompleteMessage( MCatalogsBaseMessage* & aMessage,
        TNcdOperationMessageCompletionId aId,
        TInt aStatus );
        
    /**
     * Completes a message with a completion id, sendable object and an array
     * of node identifiers.
     *
     * NOTE: aMessage is always released and set to null.
     *
     * @param aMessage The message to complete.
     * @param aId Id for the message e.g. progress.
     * @param aSendableObject A object to send.
     * @param aArray The descriptors to send.
     * @param aStatus Error code.
     * @return System-wide error code.
     */
    virtual TInt CompleteMessage( MCatalogsBaseMessage*& aMessage,
        TNcdOperationMessageCompletionId aId,
        const MNcdSendable& aSendableObject,
        RPointerArray<CNcdNodeIdentifier>& aNodes,
        TInt aStatus );
        
    /**
     * Completes a message with a completion id, and an array
     * of sendable objects.
     *
     * NOTE: aMessage is always released and set to null.
     *
     * @param aMessage The message to complete.
     * @param aId Id for the message e.g. progress.
     * @param aExpiredNodes expired nodes to send.
     * @param aStatus Error code.
     * @return System-wide error code.
     */
    virtual TInt CompleteMessage( MCatalogsBaseMessage*& aMessage,
        TNcdOperationMessageCompletionId aId,
        RPointerArray<CNcdExpiredNode>& aExpiredNodes,
        TInt aStatus );
        
    /**
     * Getter for the session from which this operation was started from.
     * Session (or specifically MCatalogsContext retrieved via it) can be used
     * to identify that which client of the engine started a specific operation.
     *
     * @return session.
     */
    MCatalogsSession& Session();
    
    /**
     * Checks whether this operation is a sub-operation.
     */
    TBool IsSubOperation();
    
    
    /**
     * Progress getter
     * @return Operation progress
     */
    virtual const TNcdSendableProgress& Progress() const;
    
    
    /**
     * Handle setter
     *
     * @param aHandle
     */
    virtual void SetHandle( TInt aHandle );
    
    
    /**
     * Handle getter
     *
     * @return Handle
     */
    virtual TInt Handle() const;
    
    /**
     * Runs operation specific code.
     *
     * @return System wide error-code.
     */
    virtual TInt RunOperation() = 0;
    
    /**
     * Continues operation asynchronously 
     */
    void ContinueOperationL();
    
public: // from base class CCatalogsCommunicable
    
    /**
     * @see CCatalogsCommunicable
     */
    virtual void ReceiveMessage( 
        MCatalogsBaseMessage* aMessage,
        TInt aFunctionNumber );
    
    /**     
     * @see CCatalogsCommunicable
     */
    virtual void CounterPartLost( const MCatalogsSession& aSession );

public: // from MNcdParserQueryObserver

    /**
     * @see MNcdParserQueryObserver
     */
    virtual void QueryL( MNcdConfigurationProtocolQuery* aData );

public: // from MNcdParserInformationObserver

    /**
     * @see MNcdParserInformationObserver::InformationL
     */
    virtual void InformationL( MNcdPreminetProtocolInformation* aData );
    
public: // from MNcdParserErrorObserver

    /**
     * @see MNcdParserInformationObserver::ErrorL
     */
    virtual void ErrorL( MNcdPreminetProtocolError* aData );

public: // from MNcdOperationObserver

    /**
     * @see MNcdOperationObserver::Progress
     */
    virtual void Progress( CNcdBaseOperation& aOperation );
    
    /**
     * @see MNcdOperationObserver::QueryReceived
     */
    virtual void QueryReceived( CNcdBaseOperation& aOperation,
                                CNcdQuery* aQuery );

    /**
     * @see MNcdOperationObserver::OperationComplete
     */
    virtual void OperationComplete( CNcdBaseOperation* aOperation,
                                    TInt aError );
                                    
    /**
     * @see MNcdOperationObserver::ExpirationInfoReceived
     */
    virtual void ExpirationInfoReceived( CNcdBaseOperation* aOperation,
        RPointerArray<CNcdExpiredNode>& aExpiredNodes );

protected:

    /**
     * Constructor.
     *
     * @param aNodeManager Pointer to the node manager.
     * @param aIsSubOperation Indicates that the operation
     * is a sub-operation and has no proxy. Querys, progress-
     * and completion events will only be sent to the parent operation.
     */
    CNcdBaseOperation(
        CNcdGeneralManager& aGeneralManager,        
        MNcdOperationRemoveHandler* aRemoveHandler,
        TNcdOperationType aOperationType,
        MCatalogsSession& aSession,
        TBool aIsSubOperation = EFalse );
    
    void ConstructL();
    
    /**
     * Initializes the operation
     *
     * Must be implemented by the operation if it's proxy uses 
     * InitializeOperationL().
     *
     * The implementation must complete the currently pending message
     *
     * The base implementation does nothing
     */
    virtual TInt Initialize();
        
    /**
     * Changes to previous state.
     */
    virtual void ChangeToPreviousStateL();
    
    /**
     * Responds to query items that need to be handled inside
     * the engine e.g. IMEI -query.
     */
    void HandleEngineQueryItemsL( CNcdQuery* aQuery );
    
    /**
     * Creates a query response protocol element from this query object.
     *
     * @exception Leaves with KErrNotReady if non-optional items are not set
     * or if the query has not been accepted.
     * @return Query response element.
     */
    MNcdConfigurationProtocolQueryResponse* CreateResponseL( 
        CNcdQuery& aQuery );

    /**
     * Checks whether an information response has been received.
     *
     */
     TBool InformationReceived();
    
    /**
     * Handles an information response. Information may contain querys
     * so this method should not be called when there isn't already a
     * query pending.
     *
     * @exception KErrNotFound if no information response present,
     *            KErrNotReady if there is another query pending
     */
     void HandleInformationResponseL();

     void CompleteCallback();

     /**
      * Sends expiration info to proxy for normal ops and to observer for subops.
      *
      * @param aExpiredNodes The expired nodes.
      * @return KErrNone if everything went ok, KErrNotFound if no iPendingMessage 
      * otherwise system-wide error code.
      */
     TInt SendExpirationInfo( RPointerArray<CNcdExpiredNode>& aExpiredNodes );
    
public:
    /**
     * Stores the query and sends it to proxy.
     * Calls AddRef to the query.
     *
     * @param aQuery The query to send.
     */
    void QueryReceivedL( CNcdQuery* aQuery );
    
    /**
     * Handles a completed query. Called when proxy
     * has completed a query.
     *
     * Inheriting classes need to implement this if they need
     * to handle querys.
     *
     * @note Default implementation panics!
     *
     * @param aQuery The query that has been completed.
     * @return ETrue if query was handled EFalse otherwise
     */
    virtual TBool QueryCompletedL( CNcdQuery* aQuery );
    
    /**
     * Getter for current query.
     * Calls AddRef().
     */
    CNcdQuery* ActiveQuery();
    
    /**
     * Getter for query entity count.
     */
    TInt QueryEntityCount();
    
    /**
     * Getter for received protocol query entitys.
     */
    const MNcdConfigurationProtocolQuery& QueryEntityL( const TDesC& aId );
    
    void ContinueOperation();
    
    /**
     * Fails the operation.
     */
    void FailOperation( TInt aError );
    
    /**
     * Adds query responses to request
     */
    void AddQueryResponsesL( CNcdRequestBase* aRequest );
    
    /**
     * Sends all current querys to proxy side (all information- and needed embedded -querys)
     *
     */
    void HandleQuerysL();
    
    /**
     * A query has been handled 
     */
    void QueryHandledL( CNcdQuery* aQuery );
    
    /**
     * Returns the amount of pending queries.
     *
     * @return the amount of pending queries.
     */
    TInt QueriesPending();
    
    /**
     * Resends previous request. Used for resending requests when queries need responding.
     *
     */
    void ResendRequestL();
    
    /**
     * Clears completed queries.
     */
    void ClearCompletedQueries();
    
       
protected: // data

    CNcdGeneralManager& iGeneralManager;
    
    /**
     * Last error.
     */
    TInt iError;
    
    MNcdOperationRemoveHandler* iRemoveHandler;
    
    /**
     * Type of the operation.
     */
    TNcdOperationType iOperationType;
    
    /**
     * State of the operation.
     */
    TOperationState iOperationState;
    
    /**
     * Progress of the operation. Sub-classes need to initialize this
     * object to correct values before use.
     */
    TNcdSendableProgress iProgress;

    /**
     * Operation observers, for callbacks.
     * Not own.
     */
    RPointerArray<MNcdOperationObserver> iObservers;
    
    /**
     * A pending message. This will be completed when progress
     * has been made.
     * Own.
     */
    MCatalogsBaseMessage* iPendingMessage;
    
    MCatalogsSession& iSession;

    /**
     * Parser. Construction and deletion is handled in derived classes.
     */
    MNcdParser* iParser;
    
    /**
     * Currently active query.     
     */
    CNcdQuery* iActiveQuery;
    
    /**
     * Protocol query entitys received from parser.
     */
    RPointerArray<MNcdConfigurationProtocolQuery> iEmbeddedDataQuerys;
    
    /**
     * Querys waiting sending (to proxy or parent op)
     */
    RPointerArray<CNcdQuery> iPendingQuerys;
    
    /**
     * Completed querys which have not yet been sent to server.
     * These can be added to a request via AddQuerysL.
     */
    RPointerArray<CNcdQuery> iCompletedQuerys;
    
    TBool iIsSubOperation;
        
    /**
     * Pointer to the node manager. May be NULL if the operation
     * class does not need it. Not own.
     */
    CNcdNodeManager* iNodeManager;

    /**
     * Handle used for client-server -communication
     */
    TInt iHandle;
    
    /**
     * Used to continue operation asynchronously after a callback.
     * (Used because of problems with parser callbacks)
     */
    CNcdAsyncRunner* iRunner;
    
    /**
     * Identifiers of nodes, that are expired as a result of an expiredCachedData response 
     * are stored here.
     */
    RPointerArray<CNcdExpiredNode> iExpiredNodes;
    
    };


class CNcdAsyncRunner : public CActive
    {
public:
    static CNcdAsyncRunner* NewL( CNcdBaseOperation* aOperation )
        {
        CNcdAsyncRunner* self = new (ELeave) CNcdAsyncRunner( aOperation );
        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop( self );
        return self;
        }
        
    ~CNcdAsyncRunner()
        {
        DLTRACEIN((""));
        Cancel();
        }
        
    void Start()
        {
        DLTRACEIN((""));
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
protected:
    virtual void RunL()
        {
        DLTRACEIN((""));
        iOperation->ContinueOperation();
        }
    virtual void DoCancel()
        {        
        DLTRACEIN((""));
        }
        
private:
    CNcdAsyncRunner::CNcdAsyncRunner( CNcdBaseOperation* aOperation)
    : CActive(EPriorityStandard), iOperation( aOperation )
    	{
    	CActiveScheduler::Add(this);
    	}    
    void ConstructL()
        {        
        }
private: // data
    CNcdBaseOperation* iOperation;
    };

#endif // C_NCDBASEOPERATION_H
