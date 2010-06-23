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
* Description:   CNcdPreminetProtocolPurchaseOptionImpl declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_SUBSCRIPTION_IMPL_H
#define NCD_PREMINET_PROTOCOL_SUBSCRIPTION_IMPL_H

#include "ncd_pp_subscription.h"
#include "ncdprotocoltypes.h"

class MNcdPreminetProtocolSubscriptionDetails;
class MNcdPreminetProtocolPayment;
class MNcdConfigurationProtocolQuery;

class CNcdPreminetProtocolSubscriptionImpl 
: public CBase, public MNcdPreminetProtocolSubscription
    {
    
public:

    static CNcdPreminetProtocolSubscriptionImpl* NewL();
    static CNcdPreminetProtocolSubscriptionImpl* NewLC();

    virtual ~CNcdPreminetProtocolSubscriptionImpl();
    
    void ConstructL();


    // From base class MNcdPreminetProtocolPurchaseOption
    /**
     * Namespace.
     * @return The namespace.
     */
    virtual const TDesC& Namespace() const; 

    /**
     * Entity id.
     * @return Entity id or KNullDesC
     */
    virtual const TDesC& EntityId() const;

    /**
     * Purchase option id.
     * @return Purchase option id or KNullDesC
     */
    virtual const TDesC& PurchaseOptionId() const;

    /**
     * Subscription timestamp.
     * @return Timestamp or KNullDesC
     */
    virtual const TDesC& Timestamp() const;

    /**
     * Subsctiption type.
     * @return Type
     */
    virtual TNcdSubscriptionType Type() const;

    /**
     * Retuns true if this subscription is cancelled.
     * @return true or false
     */
    virtual TBool Cancelled() const;

    /**
     * Total usage rights of this subscription.
     * @return Subscription details pointer or NULL
     */
    virtual const MNcdPreminetProtocolSubscriptionDetails* 
        TotalUsageRights() const;

    /**
     * Remaining usage rights of this subscription.
     * @return Subscription details pointer or NULL
     */
    virtual const MNcdPreminetProtocolSubscriptionDetails*
        RemainingUsageRights() const;


    /**
     * Timestamp of expiration
     * @return Timestamp or KNullDesC
     */
    virtual const TDesC& ExpiredOn() const;
    
    /**
     * Usage rights of this subscription.
     * Wrapping method for old subscriptions, uses TotalUsageRights().
     * @return Subscription details pointer or NULL
     */
    virtual const MNcdPreminetProtocolSubscriptionDetails* 
        UsageRights() const;
        
private:
    CNcdPreminetProtocolSubscriptionImpl();
    
public:
    HBufC* iNamespace;
    HBufC* iEntityId;
    HBufC* iPurchaseOptionId;
    HBufC* iTimestamp;
    TNcdSubscriptionType iType;
    TBool iCancelled;

    MNcdPreminetProtocolSubscriptionDetails* iTotalUsageRights;
    MNcdPreminetProtocolSubscriptionDetails* iRemainingUsageRights;
        
    HBufC* iExpiredOn;    
    };

#endif // NCD_PREMINET_PROTOCOL_SUBSCRIPTION_IMPL_H