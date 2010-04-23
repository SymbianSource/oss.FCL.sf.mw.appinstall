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
* Description:   Contains CNcdSubscriptionGroupProxy class
*
*/


#ifndef C_NCD_SUBSCRIPTION_GROUP_PROXY_H
#define C_NCD_SUBSCRIPTION_GROUP_PROXY_H


// For the streams
#include <s32mem.h>
#include <badesca.h>

#include "ncdbaseproxy.h"

class CNcdSubscriptionProxy;
class CNcdOperationManagerProxy;
class CNcdNodeManagerProxy;
class CNcdNodeIdentifier;

/**
 *  This class implements the subscription group.
 *
 *  @since S60 v3.2
 */
class CNcdSubscriptionGroupProxy : public CNcdBaseProxy
    {

public:

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aOperationManager This class uses operation manager to get the
     * operations for certain tasks.
     * @param aNodeManager This class uses node manager to get certain
     *                     nodes.
     * @return CNcdSubscriptionGroupProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdSubscriptionGroupProxy* NewL(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdOperationManagerProxy& aOperationManager,
        CNcdNodeManagerProxy& aNodeManager );

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aOperationManager This class uses operation manager to get the
     * operations for certain tasks. 
     * @param aNodeManager This class uses node manager to get certain
     *                     nodes.
     * @return CNcdSubscriptionGroupProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdSubscriptionGroupProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdOperationManagerProxy& aOperationManager,
        CNcdNodeManagerProxy& aNodeManager );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns this object may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdSubscriptionGroupProxy();
     

    /**
     * Gets the data for descriptors from the server side. This function is
     * called to update proxy data. The function uses the protected virtual
     * internalize functions. So, the child classes may provide their own
     * implementations for internalizations of certain metadata.
     */
    void InternalizeL();
    

    /**
     * Gets all subscriptions related to this subscriptiongroup.
     * No reference count changes are done to contents of returned
     * array before returning it.
     */
    const RPointerArray<CNcdSubscriptionProxy>& Subscriptions();

    /**
     * @return pointer to specified subscription if such a subscription
     *         exists. (In other words if a subscription is made.)
     *         NOTICE: Ownership is NOT transferred and reference
     *                 count is NOT incremented.
     */        
    CNcdSubscriptionProxy* Subscription( const TDesC& aPurchaseOptionId );

    /**
     * Identifier of the meta data where from subscriptions of this
     * group have been bought from.
     *
     * @return Node identifier of the meta data.
     */
    const CNcdNodeIdentifier& Identifier() const;

    /**
     * EntityId of the meta data where from subscriptions of this
     * group are bought from.
     */
    const TDesC& EntityId() const;

    /**
     * Namespace of the meta data where from subscriptions of this
     * group are bought from.
     */
    const TDesC& Namespace() const;

    /**
     * Server URI of the meta data where from subscriptions of this
     * group are bought from.
     */
    const TDesC& ServerUri() const;

    /**
     * Icon of the meta data where from subscriptions of this
     * group are bought from. Notice that the ownership is
     * transferred. Uses RequestIconDataL function which retrieves the
     * icon from the server side.
     */
    HBufC8* IconL() const;

protected:

    /**
     * Constructor
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aOperationManager This class uses operation manager to get the
     * operations for certain tasks. 
     * @param aNodeManager This class uses node manager to get certain
     *                     nodes.
     */
    CNcdSubscriptionGroupProxy(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdOperationManagerProxy& aOperationManager,
        CNcdNodeManagerProxy& aNodeManager );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    
    /**
     * @return CNcdOperationManagerProxy& Gives the operation manager proxy.
     * Operation manager may be asked to create new operations for this class
     * object.
     */
    CNcdOperationManagerProxy& OperationManager() const;

    
    // These functions are used to update the data of this class object

    /**
     * @param aStream This stream will contain all the data content for
     * this class object. The stream is gotten from the server side. The
     * memeber variables will be updated according to the data from
     * the stream. 
     */
    virtual void InternalizeDataL( RReadStream& aStream );
    

private:

    // Prevent if not implemented
    CNcdSubscriptionGroupProxy(
        const CNcdSubscriptionGroupProxy& aObject );
    CNcdSubscriptionGroupProxy& operator=(
        const CNcdSubscriptionGroupProxy& aObject );
    
    /**
     * Function that goes through subscriptions-array and
     * calls internal release to each one of them. If
     * references are still left, it is the one's, that holds
     * the reference, duty to release it.
     *
     * @since S60 ?S60_version
     */    
    void DeleteSubscriptions();
    
    /**
     * Request all the purchase option ids of the subscriptions
     * from server side and creates an array containing them.
     *
     * @return The array of purchase option ids.
     */
    CDesCArray* RequestPurchaseOptionIdsL() const;
    
    /**
     * Deletes the subscriptions of which purchase option id
     * is not in the given descriptor array.
     *
     * @param aPurchaseOptionIds The purchase option ids.
     */
    void DeleteMissingSubscriptions(
        const CDesCArray& aPurchaseOptionIds );
    
    /**
     * Releases the given subscription.
     *
     * @param aSubscription The subscription.
     */    
    void DeleteSubscription( CNcdSubscriptionProxy* aSubscription );
    
    /**
     * Internalizes the subscription group with the subscriptions of 
     * which purchase option id is in the given array.
     * Those subscriptions must not exist in the proxy side yet.
     *
     * @param aPurchaseOptionIds The array of purchase option ids.
     */
    void InternalizeL(
        const CDesCArray& aPurchaseOptionIds );

    /**
     * Request the icon data of the subscriptions related to this
     * subscription group.
     *
     * @return The icon data descriptor.
     */
    HBufC8* RequestIconDataL() const;
    
private: // data

    CNcdOperationManagerProxy& iOperationManager;
    CNcdNodeManagerProxy& iNodeManager;


    RPointerArray<CNcdSubscriptionProxy> iSubscriptions;
    
    /**
     * NodeIdentifier to identify the node where from the
     * subscriptions within this group are bought from.
     * Own.
     */
    CNcdNodeIdentifier* iOriginNodeIdentifier;
        
    };


#endif // C_NCD_SUBSCRIPTION_GROUP_PROXY_H
