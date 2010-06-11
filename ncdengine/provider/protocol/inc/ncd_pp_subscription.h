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
* Description:   MNcdPreminetProtocolPurchaseOption declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_SUBSCRIPTION_H
#define NCD_PREMINET_PROTOCOL_SUBSCRIPTION_H

#include <e32base.h>
#include "ncdprotocoltypes.h"


class MNcdPreminetProtocolSubscriptionDetails;
class MNcdPreminetProtocolPayment;
class MNcdConfigurationProtocolQuery;

class MNcdPreminetProtocolSubscription
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolSubscription() {}
    
    /**
     * Namespace.
     * @return The namespace.
     */
    virtual const TDesC& Namespace() const = 0;

    /**
     * Entity id.
     * @return Entity id or KNullDesC
     */
    virtual const TDesC& EntityId() const = 0;

    /**
     * Purchase option id.
     * @return Purchase option id or KNullDesC
     */
    virtual const TDesC& PurchaseOptionId() const = 0;

    /**
     * Subscription timestamp.
     * @return Timestamp or KNullDesC
     */
    virtual const TDesC& Timestamp() const = 0;

    /**
     * Subsctiption type.
     * @return Type
     */
    virtual TNcdSubscriptionType Type() const = 0;

    /**
     * Retuns true if this subscription is cancelled.
     * @return true or false
     */
    virtual TBool Cancelled() const = 0;

    /**
     * Total usage rights of this subscription.
     * @return Subscription details pointer or NULL
     */
    virtual const MNcdPreminetProtocolSubscriptionDetails* 
        TotalUsageRights() const = 0;

    /**
     * Remaining usage rights of this subscription.
     * @return Subscription details pointer or NULL
     */
    virtual const MNcdPreminetProtocolSubscriptionDetails*
        RemainingUsageRights() const = 0;

    /**
     * Timestamp of expiration
     * @return Timestamp or KNullDesC
     */
    virtual const TDesC& ExpiredOn() const = 0;
    
    /**
     * Usage rights of this subscription.
     * Wrapping method for old subscriptions, uses TotalUsageRights().
     * @return Subscription details pointer or NULL
     */
    virtual const MNcdPreminetProtocolSubscriptionDetails* 
        UsageRights() const = 0;
    };


#endif //NCD_PREMINET_PROTOCOL_SUBSCRIPTION_H
