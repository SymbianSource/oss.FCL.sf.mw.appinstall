/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains CNcdSubscriptionDataCompleterDataCompleter class
*
*/


#ifndef NCD_SUBSCRIPTION_DATA_COMPLETER_H
#define NCD_SUBSCRIPTION_DATA_COMPLETER_H

#include <e32base.h>
#include <bamdesca.h>

class CNcdSubscriptionManager;
class CNcdNodeIdentifier;
class MNcdSubscriptionManagerObserver;
class CNcdNodeManager;
class CNcdSubscription;
class CNcdNodeMetaData;
class CNcdOperationManager;
class MCatalogsContext;

/**
 *  This server side class contains the functionality to complete
 *  the data of subscriptions whose data is incomplete.
 *
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdSubscriptionDataCompleter : public CBase
    {

public:
    /**
     * NewL
     *
     * @param aManager Reference to subscription manager which started
     *                 this completer.
     * @param aNodeManager Reference to node manager.
     * @param aOperationManager Reference to operation manager.
     * @return CNcdSubscriptionDataCompleter* Pointer to the
     *         created object of this class.
     */
    static CNcdSubscriptionDataCompleter* NewL(
        CNcdSubscriptionManager& aSubscriptionManager,
        CNcdNodeManager& aNodeManager,
        CNcdOperationManager& aOperationManager );

    /**
     * NewLC
     *
     * @param aManager Reference to subscription manager which started
     *                 this completer.
     * @param aNodeManager Reference to node manager.
     * @param aOperationManager Reference to operation manager.
     * @return CNcdSubscriptionDataCompleter* Pointer to the 
     *         created object of this class.
     */
    static CNcdSubscriptionDataCompleter* NewLC(
        CNcdSubscriptionManager& aSubscriptionManager,
        CNcdNodeManager& aNodeManager,
        CNcdOperationManager& aOperationManager );

    /**
     * Destructor
     *
     */
    virtual ~CNcdSubscriptionDataCompleter();

    /**
     * Function for checking that subscriptions of given goups have
     * all needed data and if not get the data from the corresponding
     * nodes. Does also the needed callback of observer and notifying
     * of subscription manager's listeners
     *
     * @note Given groups are thought to be obtained from one server
     *
     * @param aClientUid Id of the client that should be notified
     *                   of subscription manager content change.
     * @param aNodeIds Identifiers of nodes which were used to buy
     *                 the subscriptions.
     * @param aPurchaseOptionIDs Purchase option ids which were
     *                           used to buy the subscriptions.
     * @param aContext Not used.
     * @param aObserver Observer that is informed of internalisation
     *                  completion.
     * @return Symbian error code.
     */
    TInt CompleteSubscriptions( TUid aClientUid,
                                RPointerArray<CNcdNodeIdentifier> aNodeIds,
                                MDesCArray* aPurchaseOptionIDs,
                                MCatalogsContext* aContext,
                                MNcdSubscriptionManagerObserver* aObserver );

protected:

    /**
     * Constructor
     * Is set in the NewLC function.
     * @param aSubscriptionManager Reference to subscription manager
     *                             which started this completer.
     * @param aNodeManager Reference to node manager.
     * @param aOperationManager Reference to operation manager.
     */
    CNcdSubscriptionDataCompleter(
        CNcdSubscriptionManager& aSubscriptionManager,
        CNcdNodeManager& aNodeManager,
        CNcdOperationManager& aOperationManager );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    


private:

    // Prevent these two if they are not implemented
    CNcdSubscriptionDataCompleter( 
        const CNcdSubscriptionDataCompleter& aObject );
    CNcdSubscriptionDataCompleter& 
        operator=( const CNcdSubscriptionDataCompleter& aObject );

    void ProceedCompletion();

    /**
     * Function for completing given subscription from the given node.
     * Also saves updated node to database.
     *
     * @param aCurrentSubscription Subscription to update.
     * @param aCurrentNode Node which should be used in the completion.
     * @param aPurchaseOptionId Purchase option id which was
     *                          used to buy the subscription.
     * @param aClientUid Uid of the client whose subscription we
     *                   are updating.
     */
    void CompleteSubscriptionL( CNcdSubscription& aCurrentSubscription,
                                const CNcdNodeMetaData& aCurrentNodeMetaData,
                                const TDesC& aPurchaseOptionId,
                                TUid aClientUid ) const;

    /**
     * Returns identified subscription if found.
     *
     * @param aNodeIdentifier Identifier of the node which was used
     *                        to buy the subscription.
     * @param aPurchaseOptionId Purchase option id which was
     *                          used to buy the subscription.
     */
    CNcdSubscription& SubscriptionL(
        const CNcdNodeIdentifier& aNodeIdentifier,
        const TDesC& aPurchaseOptionId ) const;

    /**
     * Function for checking whether given subscription is incomplete
     * or not.
     *
     * @param aSubscription Subscription to check.
     */
    TBool SubscriptionIncomplete(
        const CNcdSubscription& aSubscription ) const;

    /**
     * Function for setting name of the given subscription from
     * the purchase option of given metadata identified by the
     * purchase option id.
     *
     * @param aSubscription Subscription whose name has to be set.
     * @param aMetaData Meta data which contains purchase option
     *                  whose name is used as the name of the
     *                  subscription.
     * @param aPurchaseOptionId Purchase option whose name is 
     *                          set to be the name of the subscription.
     */
    void CompleteSubscriptionNameL(
        CNcdSubscription& aSubscription,
        const CNcdNodeMetaData& aMetaData,
        const TDesC& aPurchaseOptionId ) const;

    /**
     * Function for setting icon of the given subscription from
     * the given metadata.
     *
     * @param aSubscription Subscription whose icon has to be set.
     * @param aMetaData Meta data which contains the icon for the
     *                  subscription.
     */
    void CompleteSubscriptionIconL(
        CNcdSubscription& aSubscription,
        const CNcdNodeMetaData& aMetaData ) const;

private: // data

    /**
     * Reference to subscription manager which started this completer.
     */
    CNcdSubscriptionManager& iSubscriptionManager;

    /**
     * Reference to node manager for node creation/retrieval purposes.
     */
    CNcdNodeManager& iNodeManager;

    /**
     * Reference to operation manager for operation creation purposes.
     */
    CNcdOperationManager& iOperationManager;

    /**
     * Client uid of the client that should be notified of
     * subscription manager state change after the internalization.
     */
    TUid iClientUid;

    /**
     * Indexer to index subscriptions which are being completed.
     * Member variable to enable possible asynchronous operations.
     * (although such operations are not needed at the moment)
     */     
    TInt iIndexer;

    /**
     * Array of node ids identifying the nodes which contained
     * the purchase options which were used to buy the
     * subscriptions whose data should be completed.
     */    
    RPointerArray<CNcdNodeIdentifier> iNodeIds;
    
    /**
     * Array of purchase option ids which were used to buy the
     * subscriptions whose data should be completed.
     * Own.
     */
    MDesCArray* iPurchaseOptionIds;

    /**
     * Not used at the moment.
     * Context to identify client if node loads would ever be
     * implemented here.
     * Not own.
     */ 
    MCatalogsContext* iContext;

    /**
     * Observer that should be notified when internalization of
     * subscriptions has ended.
     * Not own.
     */    
    MNcdSubscriptionManagerObserver* iInternalizationObserver;


    };
    
#endif // NCD_SUBSCRIPTION_DATA_COMPLETER_H 
