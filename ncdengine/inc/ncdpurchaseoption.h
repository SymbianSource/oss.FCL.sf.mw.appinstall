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
* Description:   ?Description
*
*/


#ifndef M_NCD_PURCHASE_OPTION_H
#define M_NCD_PURCHASE_OPTION_H

#include <e32cmn.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"

class MNcdNode;
class MNcdSubscription;

/**
 *  Describes one purchase option. 
 *
 *  Each purchasable node may have one or more of these.
 *  A purchase option can be for example "1$ Trial version", or "10$ Full version".
 *
 *  
 */
class MNcdPurchaseOption : public virtual MCatalogsBase
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdPurchaseOptionUid };

    /**
     * Type of the purchase option.
     */
    enum TType
        {
        
        /** Purchase option to make a normal purchase. */
        EPurchase,

        /** Purchase option to purchase a subscription. */
        ESubscription,

        /** Purchase option to do a purchase using an existing subscription. */
        ESubscriptionPurchase,
        
        /** Purchase option to do an upgrade to an existing subscription. */
        ESubscriptionUpgrade
        
        };

    /**
     * Name getter.
     *
     * 
     * @return Name of the purchase option.
     */ 
    virtual const TDesC& Name() const = 0;


    /**
     * Price getter.
     *
     * 
     * @return String indicating the price of the purchase option.
     */
    virtual const TDesC& Price() const = 0;


    /**
     * Checks whether this purchase option is free.
     *
     * 
     * @return ETrue if this purchase option is free.
     */
    virtual TBool IsFree() const = 0;


    /**
     * Get type of the purchase option.
     *
     * 
     * @return Subscription type.
     */
    virtual TType PurchaseOptionType() const = 0;

    /**
     * Checks whether this purchase option is obsolete or not.
     * Purchase option becomes obsolete if it is removed while
     * user still has reference to it. Removal can be done for
     * example because the purchase option was removed from the
     * server.
     * Obsolete purchase option should not be used and valid purchase
     * option instances should be retrieved by using PurchaseOptionsL
     * method from MNcdNodePurchase interface.
     *
     * 
     * @return ETrue if this purchase option is obsolete.
     */
    virtual TBool IsObsolete() const = 0;

    /**
     * Checks whether this purchase option can be used.
     * The purchase option cannot be used if the type of
     * this purchase option is ESubscriptionPurchase or
     * ESubscriptionUpgrade and the parentsubscription is
     * not yet bought.
     *
     * 
     * @return ETrue if this purchase option can be used.
     * @exception Leave with KNcdErrorObsolete if this purchase option
     *            is obsolete. Otherwise leave with System wide error code.
     */
    virtual TBool IsUsableL() const = 0;
 
    /**
     * If this purchase option is of type ESubscriptionPurchase,
     * this function returns the subscription that the purchase option
     * uses if the subscription exists in subscription manager.
     *
     * @note Valid only when purchase option is a valid
     *       ESubscriptionPurchase and this purchase option is 
     *       not obsolete.
     *
     * @return Pointer to the subscription that this purchase option
     *         uses. The subscription can be expired or not, but if
     *         the subscription does not exist in the subscription
     *         manager, NULL is returned.
     *         Counted, Release() must be called after use.
     *
     * @exception Leave with KNcdErrorObsolete if this purchase option
     *            is obsolete. If this purchase option is not valid
     *            ESubscriptionPurchase then leave with
     *            KNcdErrorPurchaseOptionNotValidSubscriptionPurchase.
     *            Otherwise leave with System wide error code.
     */
    virtual MNcdSubscription* ParentSubscriptionL() const = 0;

    /**
     * Returns the node that has the purchase option to buy
     * the subscription needed to use this purchase option.
     *
     * @note Valid only when purchase option is a valid
     *       ESubscriptionPurchase and this purchase option is 
     *       not obsolete.
     *
     * @return Pointer to the node that has the purchase option to buy
     *         the subscription needed to use this purchase option.
     *         Note that the node may be in an uninitialized state,
     *         or in an initialized state, depending on whether the
     *         node has previously been known to the engine. Counted,
     *         Release() must be called after use.
     *
     * @exception Leave with KNcdErrorObsolete if this purchase option
     *            is obsolete. If this purchase option is not valid
     *            ESubscriptionPurchase then leave with
     *            KNcdErrorPurchaseOptionNotValidSubscriptionPurchase.
     *            Otherwise leave with System wide error code.
     */
    virtual MNcdNode* ParentSubscriptionNodeL() const = 0;

    /**
     * Returns an id that identifies the purchaseoption that can be
     * used to buy the subscription needed to use this purchase option.
     * Node that has the purchase option identified by the id returned
     * from this function can be queried by using method
     * ParentSubscriptionNodeL().
     *
     * @note Valid only when purchase option is a valid
     *       ESubscriptionPurchase.
     *
     * @return Id that identifies the purchaseoption that can be
     *         used to buy the subscription needed to use this 
     *         purchase option. Node where from this purchase option is
     *         found can be queried by using method
     *         ParentSubscriptionNodeL().
     *
     * @exception If this purchase option is not valid
     *            ESubscriptionPurchase then leave with
     *            KNcdErrorPurchaseOptionNotValidSubscriptionPurchase.
     *            Otherwise leave with System wide error code.
     */
    virtual const TDesC& ParentSubscriptionPurchaseOptionIdL() const = 0;


protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdPurchaseOption() {}

    };
	
	
#endif //  M_NCD_PURCHASE_OPTION_H
