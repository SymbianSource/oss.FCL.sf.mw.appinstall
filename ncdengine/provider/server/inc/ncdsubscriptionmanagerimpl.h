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
* Description:   Contains CNcdSubscriptionManager class
*
*/


#ifndef NCD_SUBSCRIPTION_MANAGER_IMPL_H
#define NCD_SUBSCRIPTION_MANAGER_IMPL_H


#include <e32base.h>
#include <e32cmn.h>
#include <badesca.h>

#include "catalogscommunicable.h"
#include "ncdparserobserver.h"
#include "ncdnodeclassids.h"

class RWriteStream;
class RReadStream;

class CNcdStorageManager;
class CNcdNodeIdentifier;
class MCatalogsBaseMessage;
class MNcdStorage;
class CNcdSubscriptionsSourceIdentifier;
class MNcdStorageDataItem;
class CNcdPurchaseOptionImpl;
class CNcdSubscriptionGroup;
class CNcdClientsSubscriptions;
class CNcdNodeManager;
class CNcdSubscription;

class CNcdKeyValuePair;

class MNcdSubscriptionManagerObserver;
class CNcdSubscriptionDataCompleter;

class CNcdOperationManager;
class MCatalogsContext;

/**
 *  CSubscriptionManager manages all subscription info.
 *
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdSubscriptionManager : public CCatalogsCommunicable                  
    {

public:

    /**
     * NewL
     *
     * @return CNcdSubscriptionManager* Pointer to the created object 
     * of this class.
     */
    static CNcdSubscriptionManager* NewL( 
        CNcdStorageManager& aStorageManager,
        CNcdNodeManager& aNodeManager );

    /**
     * NewLC
     *
     * @return CNcdSubscriptionManager* Pointer to the created object 
     * of this class.
     */
    static CNcdSubscriptionManager* NewLC( 
        CNcdStorageManager& aStorageManager,
        CNcdNodeManager& aNodeManager );


    /**
     * Destructor
     *
     *
     */
    virtual ~CNcdSubscriptionManager();

    /**
     * Setter for operation manager. Needed to create operations for
     * subscription completion.
     */
    void SetOperationManager( CNcdOperationManager* aManager );

    /**
     * Function to return sources of subscriptions for given client.
     * Can be used for example when querying subscriptions info from
     * servers. Returns info to identify where the queries should be sent
     * etc.
     *
     * @note If same subscription could exist behind different URI:s
     *       only one URI is returned.
     *
     * @param aClientUid Uid to identify client whose subscriptions
     *                   sources are needed.
     * @return Array containing objects to identify sources for
     *         subscriptions.
     * @exception Leave KErrNotFound if nothing is found. Otherwise
     *            System wide error code.
     */
    RPointerArray<CNcdSubscriptionsSourceIdentifier> 
        SubscriptionsSourcesL( TUid aClientUid ) const;

    /**
     * This function is called when subscription info of one server
     * should be reset from given protocol elements. Server is
     * identified by the server URI which is given as a parameter.
     * If subscription info of given subscription is already found
     * from the client's data structures, it is updated. If not,
     * it is created. If some subscriptions are found locally but
     * not from the received list of subscriptions then those
     * local subscriptions are deleted.
     *
     * @note If existing subscription is not found and it is created
     *       the created subscription will have incomplete info.
     *       This is because protocol element does not contain all
     *       info. That is why this function will start the retrieval
     *       of the missing data from corresponding node if present.
     *
     * @note Array given as parameter is not closed or reset in
     *       this function.
     *
     * 
     * @param aClientUid Uid to identify client whose subscriptions
     *                   are given
     * @param aUri Uri of the server that is used to retrieve info
     *             related to given subscription.
     * @param aServersSubscriptions The data is set in the protocol parser
     *                              and can be used to update or create
     *                              subscription info.
     * @param aContext Not used.
     * @param aObserver Observer which should be called with
     *                  InternalizeComplete when internalization
     *                  completes.
     *
     */    
    void InternalizeSubscriptionsFromServerL(
        TUid aClientUid,
        const TDesC& aUri,
        RPointerArray<MNcdPreminetProtocolSubscription>&
            aServersSubscriptions,
        MCatalogsContext* aContext,
        MNcdSubscriptionManagerObserver* aObserver );


    /**
     * This function is called when subscription info should be updated
     * or constructed from a protocol element. If subscription info
     * of given subscription is already found, it is updated. If not,
     * it is created.
     *
     * @note If existing subscription is not found and it is created
     *       the created subscription will have incomplete info.
     *       This is because protocol element does not contain all
     *       info. That is why this function will start the retrieval
     *       of the missing data from corresponding node if present.
     *
     * 
     * @param aClientUid Uid to identify client whose subscription
     *                   is passed in aSubscription parameter
     * @param aUri Uri of the server that is used to retrieve info
     *             related to given subscription. (Same as of the node's
     *             uri.)
     * @param aSubscription The data is set in the protocol parser and can
     *                      be used to update or create subscription info.
     * @param aContext Not used.
     * @param aObserver Observer which should be called with
     *                  InternalizeComplete when internalization
     *                  completes.
     */
    void InternalizeSubscriptionL( 
        TUid aClientUid,
        const TDesC& aUri,
        MNcdPreminetProtocolSubscription& aSubscription,
        MCatalogsContext* aContext,
        MNcdSubscriptionManagerObserver* aObserver );


    /**
     * This function is called when a subscription should be removed
     * from the cache and from the database
     *
     * @exception Leave KErrNotFound if used subscription is not found.
     * @exception Otherwise leave System wide error code.
     */  
    void RemoveSubscriptionL( const CNcdNodeIdentifier& aData,
                              const TDesC& aPurchaseOptionId );
                                   
    /**
     * Returns the subscription defined by parameters.
     *
     * @param aNodeId The node id.
     * @param aNodeNameSpace The node namespace.
     * @param aPurchaseOptionId The purchase option id.
     * @param aClientUid The client uid.
     * @return The subscription.
     * @exception KErrNotFound If there is no such subscription group.
     */
    CNcdSubscription& SubscriptionL( const TDesC& aNodeId,
                                     const TDesC& aNodeNameSpace,
                                     const TDesC& aPurchaseOptionId,
                                     const TUid& aClientUid );
                                     
    /**
     * Clears the subscription database completely.
     *
     * @aContext The context identifying the client.
     */
    void ClearSubscriptionDbL( const MCatalogsContext& aContext );                                     

    /**
     * Notifies the enrolled listeners by completing the pending messages.
     *
     * @param aClientUid The client of which listeners are notified.
     */
    void NotifyListeners( TUid aClientUid );
    
    /**
     * Removes finished completer from the list and deletes it.
     *
     * @param aFinishedCompleter Pointer to the finished completer.
     */
    void SubscriptionCompleted(
        CNcdSubscriptionDataCompleter* aFinishedCompleter );

    /** 
     * This function stores (and replaces if exists) data of given
     * group into database with given clientuid.
     * This function uses SaveDataIntoDatabaseL
     *
     */  
    void SaveGroupIntoDatabaseL( TUid aClientUid,
                                 CNcdSubscriptionGroup& aGroup );

public: // CCatalogsCommunicable

    /**
     * @see CCatalogsCommunicable::ReceiveMessage
     */
    virtual void ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                 TInt aFunctionNumber );

    /**
     * @see CCatalogsCommunicable::CounterPartLost
     */
    virtual void CounterPartLost( const MCatalogsSession& aSession );

    
protected:

    /**
     * Constructor
     */
    CNcdSubscriptionManager( CNcdStorageManager& aStorageManager,
                             CNcdNodeManager& aNodeManager );

    /**
     * ConstructL
     */
    virtual void ConstructL();


    // Functions that are called from the ReceiveMessageL, which is meant
    // to be used by the client side.
    
    void ReleaseRequest( MCatalogsBaseMessage& aMessage );

    /**
     * This function is called when the proxy wants to get the
     * data from the serverside. This function calls the
     * ExternalizeClientsSubscriptionsL which may be overloaded in the
     * child classes
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void InternalizeRequestL( MCatalogsBaseMessage& aMessage );
    
    /** 
     * This function is called when proxy wants to get the subscription
     * group identifiers from server side.
     *
     * @param aMessage Used to send data to proxy.
     */
    void SubscriptionGroupIdentifiersRequestL(
        MCatalogsBaseMessage& aMessage );


private:

    // Prevent if not implemented
    CNcdSubscriptionManager( 
        const CNcdSubscriptionManager& aObject );
    CNcdSubscriptionManager& operator=( 
        const CNcdSubscriptionManager& aObject );

    /**
     * Function for checking that subscriptions of given goups have
     * all needed data and if not get the data from the corresponding
     * nodes. Does also the needed callback of observer and notifying
     * of subscription manager's listeners
     *
     * @note Given groups should be of the same client
     *
     * @param aClientUid Id of the client that should be notified
     *                   of manager content change.
     * @param aServersGroups Groups that should be checked.
     * @param aObserver Observer that is informed of internalisation
     *                  completion.
     * @param aContext Not used.
     * @exception Leave with System wide error code.
     */
    void CNcdSubscriptionManager::CompleteSubscriptionsDataL(
        TUid aClientUid,
        RPointerArray<CNcdSubscriptionGroup>& aServersGroups,
        MCatalogsContext* aContext,
        MNcdSubscriptionManagerObserver* aObserver );

    /**
     * Function that goes through given subscription groups and
     * checks whether they have subscriptions whose data is
     * incomplete. If such subscriptions are found, appends copy of
     * node identifier and purchase option id where from the
     * subscription is bought into the given arrays.
     *
     * @param aGroups Groups that should be checked.
     * @param aNodeIds Array for Node Id appending.
     * @param aPurchaseOptionIDs Array for purchase option id
     *        appending.
     * @exception Leave with System wide error code.
     */
    void IdentifiersForCompletionL(
        RPointerArray<CNcdSubscriptionGroup>& aGroups,
        RPointerArray<CNcdNodeIdentifier>& aNodeIds,
        CDesCArrayFlat& aPurchaseOptionIDs );

    /**
     * Function to return index pointing to subscriptions of a
     * client with given uid.
     *
     * @param aUid Uid to identify client whose subscriptions
     *             are needed.
     * @return Index of subscriptions of searched client.
     * @exception Leave KErrNotFound if not found.
     */
    TInt IsInCacheL( TUid aUid ) const;

    /**
     * Function to return index pointing to subscriptions of a
     * client with given uid. If it is not found, a object to
     * contain subscriptions of a client with given uid is created.
     *
     * @param aUid Uid to identify client whose subscriptions
     *             are needed.
     * @return Index of subscriptions of searched or created client.
     */
    TInt ClientsSubscriptionsL( TUid aClientUid );

    /**
     * Function to return index pointing to a group searched from
     * groups given as array parameter.
     *
     * @param aGroups Array to search for certain subscription group.
     * @param aEntityId Id info that should be found from the group
     * @param aNamespace Namespace info that should be found from the
     *                   group.
     * @return Index of searched group if found.
     * @exception Leave KErrNotFound if not found.
     */
    TInt FindGroupL(
        const RPointerArray<CNcdSubscriptionGroup>& aGroups,
        const TDesC& aEntityId,
        const TDesC& aNamespace ) const;

    /**
     * Function to return reference to subscription group identified
     * by given parameters. Client is identified with given uid.
     *
     * @param aUid Uid to identify client whose subscriptions
     *             are needed.
     * @param aEntityId Id info that should be found from the group
     * @param aNamespace Namespace info that should be found from the
     *                   group.
     * @return Reference to subscription group found.
     *
     * @exception Leave KErrNotFound if not found.
     */
    CNcdSubscriptionGroup& FindGroupL(
        TUid aClientUid,
        const TDesC& aEntityId,
        const TDesC& aNamespace ) const;


    /**
     * Function to return reference to subscription group identified
     * by given uri and purchase option.
     * If object containing clients subscription groups is not found it
     * is created. Also if searched subscription group is not found,
     * it is created. Created objects are inserted to corresponding
     * arrays under subscription manager.
     *
     * @param aUri Uri of the server that is used to retrieve info
     *             related to given subscription. (Same as of the node's
     *             uri.)
     * @param aData Purchaseoption that was used to buy the subscription.
     *              It is used to construct the subscription info.
     * @return Reference to subscription group found or created.
     */
    CNcdSubscriptionGroup& ClientsSubscriptionGroupL(
        const TDesC& aUri,
        const CNcdPurchaseOptionImpl& aData );

    /**
     * Function to return reference to subscription group identified
     * by given parameters. Client is identified with given uid.
     * If object containing clients subscription groups is not found it
     * is created. Also if searched subscription group is not found,
     * it is created. Created objects are inserted to corresponding
     * arrays under subscription manager.
     *
     * @param aUid Uid to identify client whose subscriptions
     *             are needed.
     * @param aEntityId Id info that should be found from the group
     * @param aNamespace Namespace info that should be found from the
     *                   group.
     * @param aUri Uri of the server that is used to retrieve info
     *             related to searched subscription. (Same as of the node's
     *             uri.)
     * @return Reference to subscription group found or created.
     */
    CNcdSubscriptionGroup& ClientsSubscriptionGroupL(
        TUid aClientUid,
        const TDesC& aEntityId,
        const TDesC& aNamespace,
        const TDesC& aUri );

    /**
     * Function to return subscriptiongroups that are received
     * from given server.
     *
     * @param aUid Uid to identify client whose subscriptions
     *             are needed.
     * @param aUri Uri of the server that is used to retrieve info
     *             related to searched subscription.
     * @return Array of subscription groups found.
     */
    RPointerArray<CNcdSubscriptionGroup> ServersGroupsL(
        TUid aClientUid,
        const TDesC& aUri ) const;

    /**
     * This function internalizes subscriptions and marks them recently
     * updated. Also writes the altered group into database.
     *
     * @note If existing subscription is not found and it is created
     *       the created subscription will have incomplete info.
     *       This is because protocol element does not contain all
     *       info. (name of the subscription)
     *
     * @note Arrays given as parameter are not closed or reset in
     *       this function.
     *
     * 
     * @param aClientUid Uid to identify client whose subscriptions
     *                   are given
     * @param aUri Uri of the server that is used to retrieve info
     *             related to given subscription.
     * @param aSubscriptions The data is set in the protocol parser and can
     *                       be used to update or create subscription info.
     *
     */
     void InternalizeAndMarkSubscriptionsL(
        TUid aClientUid,
        const TDesC& aUri,
        RPointerArray<MNcdPreminetProtocolSubscription>& 
            aServersSubscriptions );

    /**
     * This function internalizes subscription and marks it recently
     * updated. Also writes the altered group into database.
     *
     * @note If existing subscription is not found and it is created
     *       the created subscription will have incomplete info.
     *       This is because protocol element does not contain all
     *       info. (name of the subscription)
     *
     * 
     * @param aClientUid Uid to identify client whose subscriptions
     *                   are given
     * @param aUri Uri of the server that is used to retrieve info
     *             related to given subscription.
     * @param aSubscription The data is set in the protocol parser and can
     *                      be used to update or create subscription info.
     *
     */            
    void InternalizeAndMarkSubscriptionL(
        TUid aClientUid,
        const TDesC& aUri,
        MNcdPreminetProtocolSubscription& aSubscription );
        
    /**
     * This function is called when subscription info should be updated
     * or constructed from a protocol element. If subscription info
     * of given subscription is already found, it is updated. If not,
     * it is created.
     *
     * @note If existing subscription is not found and it is created
     *       the created subscription will have incomplete info.
     *       This is because protocol element does not contain all
     *       info. 
     *
     * 
     * @param aClientUid Uid to identify client whose subscription
     *                   is passed in aSubscription parameter
     * @param aUri Uri of the server that is used to retrieve info
     *             related to given subscription. (Same as of the node's
     *             uri.)
     * @param aSubscription The data is set in the protocol parser and can
     *                      be used to update or create subscription info.
     */
    void InternalizeSubscriptionL( 
        TUid aClientUid,
        const TDesC& aUri,
        MNcdPreminetProtocolSubscription& aSubscription );
        
    /**
     * Function that goes through given subscription groups (array)
     * and checks if there are subscriptions that have not been
     * marked as recently updated. If such are found they are removed.
     * They are also removed from the database.
     *
     * @param aUid Uid to identify client whose subscriptions
     *             are handled.
     * @param aGroups Array of subscription groups.
     */
    void RemoveUnmarkedSubscriptionsL(
        TUid aClientUid,
        RPointerArray<CNcdSubscriptionGroup>& aGroups );

    /**
     * Function to return sources of subscriptions for indexed client.
     * Can be used for example when querying subscriptions info from
     * servers. Returns info to identify where the queries should be sent
     * etc.
     *
     * @note Given index has to be valid and contain a valid object.
     *
     * @param aClientIndex Index to identify client whose subscriptions
     *                   sources are needed.
     * @return Array containing objects to identify sources for
     *         subscriptions.
     * @exception Leave KErrNotFound if no subscriptions found from indexed
     *            client. Otherwise system wide error code.
     */
    RPointerArray<CNcdSubscriptionsSourceIdentifier> 
        SubscriptionsSourcesL( TInt aClientIndex ) const;


    /**
     * This function writes client's (identified with familyid from context)
     * subscription info to the given stream. If info is not found it is
     * loaded from database. If still not found zero is returned as the
     * amount of subscription groups.
     *
     *
     * Uses ExternalizeDataForRequestL to write the received data
     * to the given stream.
     */
    virtual void ExternalizeClientsSubscriptionsL(
        MCatalogsSession& aSession,
        const RPointerArray<CNcdKeyValuePair>& aGroupIdentifiers,
        RWriteStream& aStream );

    /** 
     * This function is used to start the database action
     * that will get all data for given client.
     *
     */    
    virtual void LoadClientsInfoFromStorageL( TUid aUid );

    /**
     * Writes subscription info of given client (identified with index)
     * into given stream. If index is less than 0, writes zero
     * to the stream as an indication of no subscription info.
     *
     * @param aStream The data content of this class object will be written
     * into this stream.
     */
    virtual void ExternalizeDataForRequestL(
        TInt aIndex,
        MCatalogsSession& aSession,
        const RPointerArray<CNcdKeyValuePair>& aGroupIdentifiers,
        RWriteStream& aStream );
                                             
    /** 
     * This function creates data id for given subscription group
     * that can be used to identify the group in database.
     *
     */      
    HBufC* GenerateGroupDataIdL( CNcdSubscriptionGroup& aGroup );

    /** 
     * This function removes data of given group from database with
     * given clientuid. This function uses SaveGroupIntoDatabaseL.
     *
     */  
    void RemoveGroupFromDatabaseL( TUid aClientUid,
                                   CNcdSubscriptionGroup& aGroup );
    
    /** 
     * This function removes data of given dataitem from database.
     *
     */    
    void RemoveDataFromDatabaseL( 
        TUid aUid,
        const TDesC& aNamespace,
        const TDesC& aDataId,
        NcdNodeClassIds::TNcdNodeClassType aClassType );
        

    /** 
     * This function stores (and replaces if exists) data of given
     * dataitem into database by calling externalize.
     *
     */      
    void SaveDataIntoDatabaseL( 
        TUid aUid,
        const TDesC& aNamespace,
        const TDesC& aDataId,
        MNcdStorageDataItem& aDataItem,
        NcdNodeClassIds::TNcdNodeClassType aClassType );
    

    // Returns a storage matching the namespace or creates a new one
    // if necessary    
    MNcdStorage* StorageL( TUid aUid, const TDesC& aNamespace ) const;
    
    /**
     * Called when a listener wants to enroll.
     * @param aMessage The message from the listener.
     */
    void ListenerEnrolledL( MCatalogsBaseMessage& aMessage );
    
    /**
     * Notifies all the enrolled listeners by completing the pending messages 
     * with the given error code.
     *
     * @param aError The error code.
     */
    void NotifyAllListeners( TInt aError );
    
    
private: // data

    // Storage manager is used to load and to save data from
    // databases.
    CNcdStorageManager& iStorageManager;
    
    // Node manager is used to access nodes.
    CNcdNodeManager& iNodeManager;
    
    // Operation manager used in subscription completion
    CNcdOperationManager* iOperationManager;
        
    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;

    // Subscription groups
    RPointerArray<CNcdClientsSubscriptions> iClientSubscriptions;
    
    // Pending messages waiting for change in subscription states.
    RPointerArray<MCatalogsBaseMessage> iPendingMessages;
    
    // Objects that handle data completion of subscriptions
    RPointerArray<CNcdSubscriptionDataCompleter> iSubscriptionDataCompleters;
    
    };


#endif // NCD_SUBSCRIPTION_MANAGER_IMPL_H
