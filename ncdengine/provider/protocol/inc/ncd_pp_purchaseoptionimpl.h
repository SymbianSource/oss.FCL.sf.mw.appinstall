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


#ifndef NCD_PROTOCOL_ELEMENT_PURCHASEOPTION_IMPL_H
#define NCD_PROTOCOL_ELEMENT_PURCHASEOPTION_IMPL_H

#include "ncd_pp_purchaseoption.h"
#include "ncdprotocoltypes.h"

class MNcdPreminetProtocolDownload;
class CNcdPreminetProtocolSubscriptionDetailsImpl;

class CNcdPreminetProtocolPurchaseOptionImpl 
: public CBase, public MNcdPreminetProtocolPurchaseOption
    {
    
public:

    static CNcdPreminetProtocolPurchaseOptionImpl* NewL();
    static CNcdPreminetProtocolPurchaseOptionImpl* NewLC();

    virtual ~CNcdPreminetProtocolPurchaseOptionImpl();
    
    void ConstructL();


    // From base class MNcdPreminetProtocolPurchaseOption
    /**
     * Entity name, localized.
     * @return Name, never KNullDesC
     */
    virtual const TDesC& Name() const;

    /**
     * Returns the ID of this entity.
     * @return Id
     */
    virtual const TDesC& DependencyId() const;

    /**
     * Returns the dependency ID of this entity.
     * @return Id
     */
    virtual const TDesC& Id() const;

    /**
     * Retuns the description for this entity, localized.
     * @return Description or KNullDesC
     */
    virtual TNcdPurchaseType Purchase() const;

    /**
     * Retuns true if this item is free.
     * @return true or false
     */
    virtual TBool IsFree() const;

    /**
     * Retuns true if the item requires a purchase process.
     * This is not needed for free items.
     * @return true or false
     */
    virtual TBool RequirePurchaseProcess() const;

    /**
     * Retuns true if the item can be purchased as a gift.
     * @return true or false
     */
    virtual TBool CanBeGift() const;

    /**
     * Retuns the description for this entity, localized.
     * @return Description or KNullDesC
     */
    virtual const TDesC& Description() const;

    virtual const MNcdPreminetProtocolDownload& 
        DownloadDetailsL(TInt aIndex) const;
    virtual TInt DownloadDetailsCount() const;

    /**
     * Subscription details of this purchase.
     */
    virtual const MNcdPreminetProtocolSubscriptionDetails*
        SubscriptionDetails() const;
    
    /**
     * Retuns a pointer to the subscription details of this purchase.
     * @return Pointer or NULL
     */
    virtual const TDesC& ParentSubscriptionEntityId() const;
    virtual const TDesC& ParentSubscriptionPurchaseOptionId() const;

    virtual const TDesC& PriceText() const;
    virtual TReal32 Price() const;
    virtual const TDesC& PriceCurrency() const;
    virtual TReal32 CreditPrice() const;

private:
    CNcdPreminetProtocolPurchaseOptionImpl();
    
public:

    
    HBufC* iName;
    HBufC* iDependencyId;
    HBufC* iId;
    TNcdPurchaseType iPurchase;
    TBool iFree;
    TBool iRequirePurchaseProcess;
    TBool iCanBeGift;
    
    HBufC* iDescription;
    
    RPointerArray<MNcdPreminetProtocolDownload> iDownloadDetails;
    
    // subscription details
    CNcdPreminetProtocolSubscriptionDetailsImpl* iSubscriptionDetails;
    
    HBufC* iParentSubscriptionEntityId;
    HBufC* iParentSubscriptionPurchaseOptionId;

    HBufC* iPriceText;
    
    TReal32 iPrice;
    HBufC* iPriceCurrency;
    TReal32 iCreditPrice;
    };

#endif // NCD_PROTOCOL_ELEMENT_PURCHASEOPTION_IMPL_H