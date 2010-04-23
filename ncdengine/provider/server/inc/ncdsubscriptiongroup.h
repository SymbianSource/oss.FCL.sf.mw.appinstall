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


#ifndef C_NCD_SUBSCRIPTION_GROUP_H
#define C_NCD_SUBSCRIPTION_GROUP_H
 
// For streams
#include <s32mem.h>
#include <badesca.h>

#include "catalogscommunicable.h"
#include "ncdstoragedataitem.h"
#include "ncdnodeclassids.h"
#include "ncdprotocoltypes.h"

class CNcdSubscription;
class MNcdPreminetProtocolSubscription;
class CNcdPurchaseOptionImpl;
class CNcdNodeIdentifier;


/**
 *  Server side subscriptiongroup implementation
 *
 *  Server side implementation for purchased or old subscriptions
 *  that have been purchased from one certain dataentity.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdSubscriptionGroup : public CCatalogsCommunicable,
                              public MNcdStorageDataItem
{

public:

    
    static CNcdSubscriptionGroup* NewL();
    
    static CNcdSubscriptionGroup* NewLC();
    
    virtual ~CNcdSubscriptionGroup();


    /**
     * This function is called when subscription info should be 
     * constructed from a purchase option. If subscription info
     * of given subscription is already found, it is overwritten. If not,
     * it is created.
     *
     * Notice that this function internalizes only given subscription info
     * to this object. This means that entityid, namespace, server uri or 
     * other possible subscriptions are not read into this object.
     * 
     * @param aData Purchase option used to buy the subscription. Info
     *              for the subscription is read from the purchase option.
     */
    void InternalizeSubscriptionL(
        const CNcdPurchaseOptionImpl& aData );


    /**
     * This function is called when subscription info should be updated
     * or constructed from a protocol element. If subscription info
     * of given subscription is already found, it is updated. If not,
     * it is created.
     *
     * Notice that this function internalizes only given subscription info
     * to this object. This means that entityid, namespace, server uri or 
     * other possible subscriptions are not read into this object.
     * 
     * @param aSubscription The data is set in the protocol parser and can
     *                      be used to update or create subscription info.
     */
    void InternalizeSubscriptionL(
        MNcdPreminetProtocolSubscription& aSubscription );


    
    /**
     * This function is called when a subscription in the group should
     * be removed.
     *
     * @param aPurchaseOptionImpl The purchase option that identifies
     *                            the subscription.
     * @exception KErrNotFound If there is no such subscription.
     */
    void RemoveSubscriptionL( const TDesC& aPurchaseOptionId );

    /**
     * This function is called to set internal flag of the subscription
     * to inform whether the subscription has been recently updated or
     * not.
     *
     * @param aNewState New state for the recently updated flag.
     * @param aPurchaseOptionId The purchase option id to identify
     *                          the desired subscription.
     *
     * @exception KErrNotFound If there is no such subscription.
     */
    void SetRecentlyUpdatedL( TBool aNewState,
                              const TDesC& aPurchaseOptionId );

    /**
     * This function returns the information whether the subscription
     * has been recently updated or not.
     *
     * @param aPurchaseOptionId The purchase option id to identify
     *                          the desired subscription.
     * @return aNewState New state for the recently updated flag.
     *
     * @exception KErrNotFound If there is no such subscription.
     */    
    TBool RecentlyUpdatedL( const TDesC& aPurchaseOptionId ) const;

    /** 
     * Removes all subscriptions that are not marked as recently updated.
     * Returns boolean value to indicate if something was removed.
     *
     * @return Boolean value to indicate if something was removed. ETrue
     *         means that something was removed.
     */
    TBool RemoveUnmarkedSubscriptionsAndUnmarkL();

    /**
     * When updating/constructing this object after a subscription
     * purchase this function is used to set info of entity where from
     * the subscription was bought.
     * This is not needed when initializing from database.
     * Notice that icon is left as it was. 
     */
    virtual void SetEntityInfoL( const TDesC& aEntityId,
                                 const TDesC& aNamespace,
                                 const TDesC& aServerUri,
                                 const TUid aClientUid );

    /**
     * Gets icon of this group's subscriptions. 
     *
     * @return Icon of this group
     */
    const TDesC8& Icon();

    /**
     * Sets icon for subscriptions of this group. 
     */
    virtual void SetIconL( const TDesC8& aIcon );


    // Getters for entity information

    virtual const TDesC& EntityId() const;

    virtual const TDesC& Namespace() const;
    
    virtual const TDesC& ServerUri() const;

    CNcdNodeIdentifier& OriginNodeIdentifier() const;
    
    /** 
     * Returns the subscription which has the given purchase option id.
     *
     * @param aPurchaseOptionId The purchase option id.
     * @exception KErrNotFound If the subscription does not exist.
     */
    CNcdSubscription& SubscriptionIfExistsL(
        const TDesC& aPurchaseOptionId ) const;

    /** 
     * Returns the amount of subscription under the group.
     *
     * @return Amount of subscriptions under the group.
     */
    TInt SubscriptionCount() const;

    /** 
     * Function that checks if this group contains subscriptions
     * whose data is incomplete. (Name or icon missing) If such
     * subscriptions are found appends a copy of corresponding
     * node identifier and purchase option id to the arrays.
     *
     * @param aNodeIds Array for node identifier appending.
     * @param aPurchaseOptionIDs Array for purchase option id appending.
     */    
    void AppendIncompleteSubscriptionIDsL(
        RPointerArray<CNcdNodeIdentifier>& aNodeIds,
        CDesCArrayFlat& aPurchaseOptionIDs );
    
public: // MNcdStorageDataItem 

    // These functions are used to get the data from and to insert the data
    // into the database using by the given stream.

    /**
     * @see MNcdStorageDataItem::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see MNcdStorageDataItem::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );


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

    CNcdSubscriptionGroup();

    void ConstructL();
    
    /**
     * This function is called when the proxy wants to get the
     * purchase option ids of the subscription belonging to this
     * subscription group.
     *
     * @param aMessage Used to send data back to proxy.
     */
    void PurchaseOptionIdsRequestL( MCatalogsBaseMessage& aMessage ) const;

    /**
     * This function is called when the proxy wants to get the
     * data from the serverside. This function calls the
     * InternalizeDataForRequestL which may be overloaded in the
     * child classes
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void InternalizeRequestL( MCatalogsBaseMessage& aMessage ) const;

    /**
     * This function is called when the proxy wants to get the
     * icon data of the subscriptions belonging to this
     * subscription group.
     *
     * @param aMessage Used to send data back to proxy.
     */
    void CNcdSubscriptionGroup::IconDataRequestL(
        MCatalogsBaseMessage& aMessage ) const;
        
    /**
     * This function writes the object data to the stream. The given array
     * contains the purchase option ids of the subscription that are externalized.
     * The stream content will be sent to the proxy that requested the data.
     * Child classes should add their own data after this parent data.
     * @param aStream The data content of this class object will be written
     * into this stream.
     */
    virtual void ExternalizeDataForRequestL(
        MCatalogsSession& aSession,
        const CDesCArray& aPurchaseOptionIds,
        RWriteStream& aStream ) const;

    /**
     * This function is called from the proxy side. When the proxy
     * is deleted.
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;

private:

    // Prevent these two if they are not implemented
    CNcdSubscriptionGroup( const CNcdSubscriptionGroup& aObject );
    CNcdSubscriptionGroup& operator=( const CNcdSubscriptionGroup& aObject );


    /**
     * Function to return index pointing to a subscription
     * with given purchaseoption id.
     *
     * @param aPurchaseOptionId Purchaseoption id to identify subscription.
     * @return Index of purchaseoption.
     * @exception Leave KErrNotFound if not found.
     */
    TInt FindSubscriptionL( const TDesC& aPurchaseOptionId ) const;

    /**
     * Function to return reference to subscription identified
     * by purchase option id.
     * If subscription object is not found it is created.
     *
     * @return Reference to found or created subscription.
     */
    CNcdSubscription& SubscriptionL( const TDesC& aPurchaseOptionId );

    /**
     * Deletes all allocated member variables and sets the pointers to NULL.
     *
     * @since S60 ?S60_version
     */    
    void ResetMemberVariables();

private:


    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;



    // Info related to the dataentity where from the subscriptions
    // contained in this group are bought.

    /**
     * NodeIdentifier to identify the node where from the
     * subscriptions within this group are bought from.
     * Own.
     */
    CNcdNodeIdentifier* iOriginNodeIdentifier;

    /**
     * Icon for subscriptions.
     * Own.
     */  
    HBufC8* iIcon;

 
    /**
     * Array of subscriptions bought from the dataentity
     */
    RPointerArray<CNcdSubscription> iSubscriptions;
    
};


#endif // C_NCD_SUBSCRIPTION_GROUP_H
