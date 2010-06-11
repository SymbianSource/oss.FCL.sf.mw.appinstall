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


#ifndef C_NCD_SUBSCRIPTION_OPERATION_H
#define C_NCD_SUBSCRIPTION_OPERATION_H

#include "ncdbaseoperation.h"
#include "ncdoperationobserver.h"
#include "ncdsubscriptionoperation.h"
#include "ncdsubscriptionssourceidentifier.h"
#include "ncdsubscriptionmanagerobserver.h"

class MCatalogsAccessPointManager;
class MNcdProtocol;
class CNcdSubscriptionManager;
class CNcdSubscription;
class CNcdPurchaseOperationImpl;
class MNcdConfigurationManager;


/** 
 * Subscription operation.
 */
class CNcdSubscriptionOperation : public CNcdBaseOperation,
                                  public MNcdParserObserver,
                                  public MNcdParserSubscriptionObserver,
                                  public MCatalogsHttpObserver,
                                  public MNcdSubscriptionManagerObserver
    {
public:

    /**
     *  Sub states of subscription operation 
     */
    enum TSubscriptionOperationState
        {
        EBegin,
        ERunning,
        EHandlingQueries,
        EComplete
        };

    
    /**
     * NewL
     * Used for subscription refreshing.
     *
     * @since S60 v3.2
     * @param aSubscriptionOperationType Type of the subscription operation.
     * @param aAccessPointManager Access point manager.
     * @param aSubscriptionManager Subscription manager.
     * @param aNodeManager Node manager.
     * @param aConfigurationManager Configuration manager.
     * @param aHttpSession HTTP session used for creating subscription
     * operations.
     * @param aProtocol Protocol is used to get parser.
     * @param aRemoveHandler Remove handler.
     * @param aSession Session.
     * @return Subscription operation.
     */
    static CNcdSubscriptionOperation* NewL(
        MNcdSubscriptionOperation::TType aSubscriptionOperationType,
        CNcdGeneralManager& aGeneralManager,
        CNcdSubscriptionManager& aSubscriptionManager,        
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler& aRemoveHandler,
        MCatalogsSession& aSession );

    /**
     * NewLC
     * Used for subscription refreshing.
     *
     * @since S60 v3.2
     * @param aSubscriptionOperationType Type of the subscription operation.
     * @param aAccessPointManager Access point manager.
     * @param aSubscriptionManager Subscription manager.
     * @param aNodeManager Node manager.
     * @param aConfigurationManager Configuration manager.
     * @param aHttpSession HTTP session used for creating subscription
     * operations.
     * @param aProtocol Protocol is used to get parser.
     * @param aRemoveHandler Remove handler.
     * @param aSession Session.
     * @return Subscription operation.
     */
    static CNcdSubscriptionOperation* NewLC(
        MNcdSubscriptionOperation::TType aSubscriptionOperationType,
        CNcdGeneralManager& aGeneralManager,
        CNcdSubscriptionManager& aSubscriptionManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler& aRemoveHandler,
        MCatalogsSession& aSession );

        
    /**
     * NewL
     * Used for unsubscribing subscription.
     *
     * @since S60 v3.2
     * @param aSubscriptionOperationType Type of the subscription operation.
     * @param aPurchaseOptionId Subscription purchase option ID.
     * @param aEntityId Subscription entity ID.
     * @param aNamespace Subscription namespace.
     * @param aServerUri Subscription server URI.
     * @param aAccessPointManager Access point manager.
     * @param aSubscriptionManager Subscription manager.
     * @param aNodeManager Node manager.
     * @param aConfigurationManager Configuration manager.
     * @param aHttpSession HTTP session used for creating subscription
     * operations.
     * @param aProtocol Protocol is used to get parser.
     * @param aRemoveHandler Remove handler.
     * @param aSession Session.
     * @return Subscription operation.
     */
    static CNcdSubscriptionOperation* NewL(
        MNcdSubscriptionOperation::TType aSubscriptionOperationType,
        const TDesC& aPurchaseOptionId,
        const TDesC& aEntityId,
        const TDesC& aNamespace,
        const TDesC& aServerUri,
        CNcdGeneralManager& aGeneralManager,
        CNcdSubscriptionManager& aSubscriptionManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler& aRemoveHandler,
        MCatalogsSession& aSession );

    /**
     * NewLC
     * Used for unsubscribing subscription.
     *
     * @since S60 v3.2
     * @param aSubscriptionOperationType Type of the subscription operation.
     * @param aPurchaseOptionId Subscription purchase option ID.
     * @param aEntityId Subscription entity ID.
     * @param aNamespace Subscription namespace.
     * @param aServerUri Subscription server URI.
     * @param aAccessPointManager Access point manager.
     * @param aSubscriptionManager Subscription manager.
     * @param aNodeManager Node manager.
     * @param aConfigurationManager Configuration manager.
     * @param aHttpSession HTTP session used for creating subscription
     * operations.
     * @param aProtocol Protocol is used to get parser.
     * @param aRemoveHandler Remove handler.
     * @param aSession Session.
     * @return Subscription operation.
     */
    static CNcdSubscriptionOperation* NewLC(
        MNcdSubscriptionOperation::TType aSubscriptionOperationType,
        const TDesC& aPurchaseOptionId,
        const TDesC& aEntityId,
        const TDesC& aNamespace,
        const TDesC& aServerUri,
        CNcdGeneralManager& aGeneralManager,
        CNcdSubscriptionManager& aSubscriptionManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler& aRemoveHandler,
        MCatalogsSession& aSession );

    /**
     * Destructor
     */
    ~CNcdSubscriptionOperation();

public: // From CNcdBaseOperation

    TInt RunOperation();

    /**
     * @see CNcdBaseOperation::Cancel
     */
    void Cancel();
    
    
    /**
     * @see CNcdBaseOperation::QueryCompletedL
     */
    TBool QueryCompletedL( CNcdQuery* aQuery );

public: // From MCatalogsHttpObserver

    /**     
     * @see MCatalogsHttpObserver::HandleHttpEventL
     */
    void HandleHttpEventL( 
        MCatalogsHttpOperation& aOperation, 
        TCatalogsHttpEvent aEvent );
        
    /**
     * @see MCatalogsHttpObserver::HandleHttpError()
     */
    TBool HandleHttpError(
        MCatalogsHttpOperation& aOperation,
        TCatalogsHttpError aError );
        
public: // From MNcdParserObserver

    /**
     * @see MNcdParserObserver::ParseError
     */
    void ParseError( TInt aErrorCode );

    /**
     * @see MNcdParserObserver::ParseCompleteL
     */
    void ParseCompleteL( TInt aError );

public: // From MNcdParserSubscriptionObserver

    /**
     * see MNcdParserSubscriptionObserver::ValidSubscriptionL
     */
    void ValidSubscriptionL( MNcdPreminetProtocolSubscription* aData );

    /**
     * see MNcdParserSubscriptionObserver::OldSubscriptionL
     */
    void OldSubscriptionL( MNcdPreminetProtocolSubscription* aData );
    
public: // From MNcdOperationObserver
    
    /**
     * @see MNcdOperationObserver::Progress
     */
    void Progress( CNcdBaseOperation& aOperation );
    
    /**
     * @see MNcdOperationObserver::QueryReceived
     */
    void QueryReceived( CNcdBaseOperation& aOperation,
                                CNcdQuery* aQuery );

    /**
     * @see MNcdOperationObserver::OperationComplete
     */
    void OperationComplete( CNcdBaseOperation* aOperation,
                                    TInt aError );

public: // From MNcdParserErrorObserver

    /**
     * @see MNcdParserErrorObserver::ErrorL
     */
    void ErrorL( MNcdPreminetProtocolError* aData );

public: // From MNcdSubscriptionManagerObserver

    /**
     * @see MNcdSubscriptionManagerObserver::
     *          SubscriptionsInternalizeComplete
     */
    void SubscriptionsInternalizeComplete( TInt aError );

private:

    /**
     * Creates operation for getting all subscriptions from one server.
     *
     * @since S60 v3.2
     * @param aSubscriptionOperationType Type of the subscription.
     * @param aAccessPointManager Access point manager.
     * @param aSubscriptionManager Subscription manager.
     * @param aNodeManager Node manager.
     * @param aConfigurationManager Configuration manager.
     * @param aHttpSession HTTP session used for creating subscription
     * operations.
     * @param aProtocol Protocol is used to get parser.
     * @param aRemoveHandler Remove handler.
     * @param aSession Session.
     * @return Subscription operation.
     */
    static CNcdSubscriptionOperation* CreateSubOperationLC(
        CNcdSubscriptionsSourceIdentifier* aSource,
        CNcdGeneralManager& aGeneralManager,
        CNcdSubscriptionManager& aSubscriptionManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler& aRemoveHandler,
        MCatalogsSession& aSession );

    /**
     * Start refresh operation.
     *
     * @since S60 v3.2
     */
    void RunRefreshOperationL();
    

    /**
     * Start unsubscribe operation.
     *
     * @since S60 v3.2
     */
    void RunUnsubscribeOperationL();

    /**
     * This must be called, when completing the operation.
     *
     * @since S60 v3.2
     * @param aError Error information.
     * @return Error code.
     */
    TInt SubscriptionOperationComplete( TInt aError );

    /**
     * Completes current iPendingMessage with error value from
     * iCompletionErrorCode. Sets EStateComplete into
     * iOperationState. 
     *
     * @since S60 v3.2
     * @return Error code.
     */    
    TInt CompletePendingMessage();    
    
    /**
     * Removes the given subscription from phone cache and database.
     *
     * @param aSubscripion The subscription.
     */
    void RemoveSubscriptionL(const CNcdSubscription& aSubscription );
    
    
    /**
     * @see CNcdBaseOperation::ChangeToPreviousStateL();
     */
    void CNcdSubscriptionOperation::ChangeToPreviousStateL();
    
    /**
     * Cancels the suboperations.
     */
    void CancelSuboperations();

protected:

    /**
     * Contsructor.
     *
     * @since S60 v3.2
     * @param aSubscriptionOperationType Type of the subscription.
     * @param aAccessPointManager Access point manager.
     * @param aSubscriptionManager Subscription manager.
     * @param aNodeManager Node manager.
     * @param aConfigurationManager Configuration manager.
     * @param aHttpSession HTTP session used for creating subscription
     * operations.
     * @param aProtocol Protocol is used to get parser.
     * @param aRemoveHandler Remove handler.
     * @param aSession Session.
     */
    CNcdSubscriptionOperation(
        MNcdSubscriptionOperation::TType aSubscriptionOperationType,
        CNcdGeneralManager& aGeneralManager,
        CNcdSubscriptionManager& aSubscriptionManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler& aRemoveHandler,
        MCatalogsSession& aSession );

    /**
     * Contsructor.
     *
     * @param aSource Subscription source information.
     */
    void ConstructL(
        CNcdSubscriptionsSourceIdentifier* aSource,
        const TDesC& aPurchaseOptionId,
        const TDesC& aEntityId,
        const TDesC& aNamespace,
        const TDesC& aServerUri );
    

private:

    /**
     * A sub-state of this operation's execution.
     */
    TSubscriptionOperationState iSubscriptionOperationState;

    MNcdSubscriptionOperation::TType iSubscriptionOperationType;
    MCatalogsAccessPointManager& iAccessPointManager;
    CNcdSubscriptionManager& iSubscriptionManager;
    CNcdNodeManager& iNodeManager;
    MNcdConfigurationManager& iConfigurationManager;
    MCatalogsHttpSession& iHttpSession;
    MNcdProtocol& iProtocol;

    // Not own.
    const CNcdSubscription* iSubscription;

    MCatalogsHttpOperation* iTransaction;
    
    CNcdSubscriptionsSourceIdentifier* iSource;
    HBufC* iPurchaseOptionId;
    HBufC* iEntityId;
    HBufC* iNamespace;
    HBufC* iServerUri;

    RPointerArray<MNcdPreminetProtocolSubscription> iServersSubscriptions;

    RPointerArray<CNcdSubscriptionOperation> iSubOps;
    RPointerArray<CNcdSubscriptionOperation> iFailedSubOps;
    RPointerArray<CNcdSubscriptionOperation> iCompletedSubOps;

    TUid iClientUid;
    
    TInt iCompletionErrorCode;

    };

#endif // C_NCD_SUBSCRIPTION_OPERATION_H
