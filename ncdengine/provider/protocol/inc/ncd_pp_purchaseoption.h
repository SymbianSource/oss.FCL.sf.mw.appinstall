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


#ifndef NCD_PROTOCOL_ELEMENT_PURCHASEOPTION_H
#define NCD_PROTOCOL_ELEMENT_PURCHASEOPTION_H

#include <e32base.h>
#include "ncdprotocoltypes.h"


class MNcdPreminetProtocolDownload;
class MNcdPreminetProtocolSubscriptionDetails;

class MNcdPreminetProtocolPurchaseOption
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolPurchaseOption() {}

    /**
     * Entity name, localized.
     * @return Name, never KNullDesC
     */
    virtual const TDesC& Name() const = 0;

    /**
     * Returns the purchase option dependency ID.
     * @return Id
     */
    virtual const TDesC& DependencyId() const = 0;

    /**
     * Returns the purchase option ID.
     * @return Id
     */
    virtual const TDesC& Id() const = 0;

    /**
     * Retuns the description for this entity, localized.
     * @return Description or KNullDesC
     */
    virtual TNcdPurchaseType Purchase() const = 0;

    /**
     * Retuns true if this item is free.
     * @return true or false
     */
    virtual TBool IsFree() const = 0;

    /**
     * Retuns true if the item requires a purchase process.
     * This is not needed for free items.
     * @return true or false
     */
    virtual TBool RequirePurchaseProcess() const = 0;

    /**
     * Retuns true if the item can be purchased as a gift.
     * @return true or false
     */
    virtual TBool CanBeGift() const = 0;

    /**
     * Retuns the description for this entity, localized.
     * @return Description or KNullDesC
     */
    virtual const TDesC& Description() const = 0;

    /**
     * Retuns a reference to the download details of this purchase.
     * Leaves if index is out of bounds. Check DownloadDetailsCount().
     * @return Download details object
     */
    virtual const MNcdPreminetProtocolDownload& 
        DownloadDetailsL(TInt aIndex) const = 0;

    /**
     * Retuns amoutn of download details of this purchase.
     * @return Amount of download details
     */
    virtual TInt DownloadDetailsCount() const = 0;

    /**
     * Retuns a pointer to the subscription details of this purchase.
     * @return Pointer or NULL
     */
    virtual const MNcdPreminetProtocolSubscriptionDetails* 
        SubscriptionDetails() const = 0;
    
    /**
     * Entity ID of parent subscription (if available)
     * @return ID or KNullDesC
     */
    virtual const TDesC& ParentSubscriptionEntityId() const = 0;

    /**
     * Purchase option ID of parent subscription (if available)
     * @return ID or KNullDesC
     */
    virtual const TDesC& ParentSubscriptionPurchaseOptionId() const = 0;

    /**
     * Combined price text
     * @return Price text or KNullDesC
     */
    virtual const TDesC& PriceText() const = 0;

    /**
     * Purchase price
     * @return Price
     */
    virtual TReal32 Price() const = 0;

    /**
     * Purchase price currency string
     * @return Currency string or KNullDesC
     */
    virtual const TDesC& PriceCurrency() const = 0;

    /**
     * Purchase price in credits
     * @return Credit price
     */
    virtual TReal32 CreditPrice() const = 0;
    
    };


#endif //NCD_PROTOCOL_ELEMENT_PURCHASEOPTION_H
