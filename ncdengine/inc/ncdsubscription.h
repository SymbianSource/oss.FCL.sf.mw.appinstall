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
* Description:   Definition of MNcdSubscription interface
*
*/


#ifndef M_NCDSUBSCRIPTION_H
#define M_NCDSUBSCRIPTION_H

#include "catalogsbase.h"
#include "ncdinterfaceids.h"

class MNcdSubscriptionOperation;
class MNcdSubscriptionOperationObserver;
class TTime;
class MNcdNode;

/**
 *  Subscription interface.
 *
 *  This interface describes a subscription and provides methods for
 *  subscription management.
 *
 *  
 */
class MNcdSubscription : public virtual MCatalogsBase
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdSubscriptionUid };

    
    /**
     * Type of the subscription renewal method.
     *
     */
    enum TType
        {
        
        /** Requires an explicit renewal after expiration. */
        EPeriodic,

        /** Renewed automatically on server side after expiration. */
        EAutomaticContinous
        };

    /**
     * Status of the subscription.
     *
     * 
     */
    enum TStatus
        {

        /**
         * Subscription has been purchased and is still active
         * (validity time/credits/downloads left).
         */
        ESubscriptionActive,

        /**
         * Subscription has been purchased, but has expired
         * (out of validity time/credits/downloads).
         */
        ESubscriptionExpired
        };

    /**
     * Name getter.
     *
     * 
     * @return Name of the subscription.
     */ 
    virtual const TDesC& Name() const = 0;

    /**
     * Icon getter.
     *
     * 
     * @return Icon data. Ownership is transferred. Empty descriptor
     *         returned if icon data is not set.
     * @exception Leave with KNcdErrorObsolete if this subscription
     *            is obsolete. Otherwise leave with System wide error code.
     */
    virtual HBufC8* IconL() const = 0;

    /**
     * Returns the current status for the subscription.
     *
     * 
     * @return Current status of the subscription.
     */
    virtual TStatus SubscriptionStatus() const = 0;

    /**
     * Checks whether this subscription is obsolete or not.
     * Subscription becomes obsolete if it is removed while
     * user still has reference to it. Removal can be done for
     * example because the subscription was removed from the
     * server.
     * Obsolete subscription should not be used and valid subscription
     * instances should be retrieved by using SubscriptionsL
     * method from MNcdSubscriptionManager interface.
     *
     * 
     * @return ETrue if this purchase option is obsolete.
     */
    virtual TBool IsObsolete() const = 0;

    /**
     * Returns whether the subscription has been unsubscribed or not.
     *
     * 
     * @return ETrue, if subscription has been unsubscribed. EFalse, if the subcription
     *  has not been unsubscribed.
     */
    virtual TBool Unsubscribed() const = 0;

    /**
     * Unsubscribe a currently valid subscription. 
     *
     * 
     * @param aObserver Operation observer interface.
     * @return Subscription operation object. Counted, Release() must be called after use.
     * @exception Leave with KNcdErrorObsolete if this subscription
     *            is obsolete. Otherwise leave with System wide error code.
     */
    virtual MNcdSubscriptionOperation* UnsubscribeL( 
        MNcdSubscriptionOperationObserver& aObserver ) = 0;
    

    /**
     * Get type of the subscription.
     *
     * 
     * @return Subscription type.
     */
    virtual TType SubscriptionType() const = 0;
    

    /**
     * Get subscription validity time information.
     *
     * 
     * @param aValidUntil Set to time when the subscription expires, if
     *  ETrue is returned, otherwise not used. Time given is in local time.
     * @return ETrue, if validity time is set. EFalse, if the subcription
     *  has no validity time limit set (stays valid indefinitely).
     */
    virtual TBool ValidityTime( TTime& aValidUntil ) const = 0;


    /**
     * Get subscription credit limit information.
     *
     * 
     * @param aCreditsLeft If ETrue is returned, set to current number of
     *  credits left in the subscription.
     * @param aTotalCredits If ETrue is returned, set to total number of
     *  credits the subscription originally had after purchase.
     * @return ETrue if subscription has a credit limit, EFalse otherwise.
     */
    virtual TBool CreditLimit( TReal& aCreditsLeft, TReal& aTotalCredits ) const = 0;


    /**
     * Get subscription download limit information.
     *
     * 
     * @param aDownloadsLeft If ETrue is returned, set to current number of
     *  downloads left in the subscription.
     * @param aTotalDownloads If ETrue is returned, set to total number of
     *  downloads the subscription originally had after purchase.
     * @return ETrue if subscription has a download limit, EFalse otherwise.
     */
    virtual TBool DownloadLimit( TInt& aDownloadsLeft, TInt& aTotalDownloads) const = 0;


    /**
     * Returns the node that has the purchase option which was used
     * to buy this subscription.
     *
     * @return Pointer to the node that has the purchase option to buy
     *         this subscription. Note that the node may be in an 
     *         uninitialized state, or in an initialized state, depending
     *         on whether the node has previously been known to the engine.
     *         Counted, Release() must be called after use.
     *
     * @exception Leave with KNcdErrorObsolete if this subscription
     *            is obsolete. Otherwise leave with System wide error code.
     */
    virtual MNcdNode* OriginNodeL() const = 0;

    /**
     * Returns an id that identifies the purchaseoption that was used to buy
     * this subscription. Node that has the purchase option identified by
     * the id returned from this function can be queried by using method
     * OriginNodeL() of this class.
     *
     * @return Id that identifies the purchaseoption that was used
     *         to buy this subscription. Node where from this purchase
     *         option is found can be queried by using method
     *         OriginNodeL() of this class. Returns KNullDesC on error
     *         situations.
     */
    virtual const TDesC& OriginPurchaseOptionId() const = 0;



protected: // Destruction

    /**
     * Destructor
     * 
     * Protected so that the implementing object cannot be deleted through
     * this interface by using delete. The destructor of an interface should
     * be set virtual to make sure that the destructors of derived classes
     * will be called when object is destroyed.
     */
    virtual ~MNcdSubscription() {}

    };

#endif // M_NCDSUBSCRIPTION_H
