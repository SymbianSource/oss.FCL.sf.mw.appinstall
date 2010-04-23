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


#ifndef C_PURCHASE_OPTION_PROXY
#define C_PURCHASE_OPTION_PROXY

#include <s32mem.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdpurchaseoption.h"

class CNcdClientSubscription;
class CNcdClientPartOfSubscription;
class CNcdClientUpgrade;
class CNcdNodePurchaseProxy;

/**
 *  Class to represent purchaseoption.
 *
 *  Proxy-class to represent a purchaseoption.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdPurchaseOptionProxy : public CNcdInterfaceBaseProxy,
                                public MNcdPurchaseOption
    {

public:

    virtual ~CNcdPurchaseOptionProxy();

    static CNcdPurchaseOptionProxy* NewL( 
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodePurchaseProxy& aParentNodePurchase );

    static CNcdPurchaseOptionProxy* NewLC( 
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodePurchaseProxy& aParentNodePurchase );

    /**
     * This function is called to update proxy data.
     * The function uses the protected virtual internalize functions. 
     * So, the child classes may provide their own implementations 
     * for internalizations of certain data.
     *
     * @since S60 ?S60_version
     */
    void InternalizeL();
    
    /**
     * Getter for id of this purchaseoption.
     *
     * @since S60 ?S60_version
     * @return Id of this purchaseoption.
     */    
    const TDesC& Id() const;
    

    /**
     * Getter for pointer to the NodePurchase that owns this
     * purchaseoption proxy.
     *
     * @since S60 ?S60_version
     * @return Pointer to owning node. NULL If this purchaseoption
     *         is obsolete.
     */
    CNcdNodePurchaseProxy* ParentNodePurchase() const;

    /**
     * Getter for partofsubscription info. As const pointer implies
     * ownership is not transferred.
     *
     * @since S60 ?S60_version
     * @return Pointer to object containin part of subscription info.
     *         If NULL, does not exist.
     */    
    const CNcdClientPartOfSubscription* PartOfSubscription() const;

    /**
     * Sets purchaseoption into obsolete state and it should
     * not be used anymore.
     *
     * @since S60 ?S60_version
     * @param aIsObsolete New value for obsolete info of the
     *                    purchaseoption.
     */ 
    void SetObsolete();
    
public: // from base class MNcdPurchaseOption

    /**
     * From MNcdPurchaseOption.
     * @see MNcdPurchaseOption::Name
     */
    virtual const TDesC& Name() const;


    /**
     * From MNcdPurchaseOption.
     * @see MNcdPurchaseOption::Price
     */
    virtual const TDesC& Price() const;


    /**
     * From MNcdPurchaseOption.
     * @see MNcdPurchaseOption::IsFree
     */
    virtual TBool IsFree() const;


    /**
     * From MNcdPurchaseOption.
     * @see MNcdPurchaseOption::PurchaseOptionType
     */
    virtual MNcdPurchaseOption::TType PurchaseOptionType() const;

    /**
     * From MNcdPurchaseOption.
     * @see MNcdPurchaseOption::IsObsolete
     */
    virtual TBool IsObsolete() const;

    /**
     * From MNcdPurchaseOption.
     * @see MNcdPurchaseOption::IsUsableL
     */
    virtual TBool IsUsableL() const;

    /**
     * From MNcdPurchaseOption.
     * @see MNcdPurchaseOption::ParentSubscriptionL
     */ 
    virtual MNcdSubscription* ParentSubscriptionL() const;
 
     /**
     * From MNcdPurchaseOption.
     * @see MNcdPurchaseOption::ParentSubscriptionNodeL
     */   
    virtual MNcdNode* ParentSubscriptionNodeL() const;

     /**
     * From MNcdPurchaseOption.
     * @see MNcdPurchaseOption::ParentSubscriptionPurchaseOptionId
     */       
    virtual const TDesC& ParentSubscriptionPurchaseOptionIdL() const;


protected:

    
    CNcdPurchaseOptionProxy( 
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodePurchaseProxy& aParentNodePurchase );

    void ConstructL();

    /**
     * Function that does the actual internalization of
     * data for this proxy.
     *
     * @since S60 ?S60_version
     * @param aStream Stream where from data is internalized.
     */
    virtual void InternalizeDataL( RReadStream& aStream );


private:




private: // data

    /**
     * Purchase interface into which this purchaseoptionproxy is
     * related to.
     * Not own.
     */
    CNcdNodePurchaseProxy* iParentNodePurchase;

    /**
     * Pointer to possible subscription-component of purchase-option.
     * Null if not present.
     * Own.
     */
    CNcdClientSubscription* iSubscriptionComponent;

    /**
     * Pointer to possible partofsubscription-component of purchase-option.
     * Null if not present.
     * Own.
     */
    CNcdClientPartOfSubscription* iPartOfSubscriptionComponent;

    /**
     * Pointer to possible upgrade-component of purchase-option.
     * Null if not present.
     * Own.
     */    
    CNcdClientUpgrade* iUpgradeComponent;


    /**
     * Name of the option. For example "Try" or "Full". 
     */  
    HBufC* iName;
    /**
     * Textual representation of price when using this purchase option.
     */      
    HBufC* iPriceText;
    /**
     * If iPriceText is not used then this tells the price.
     */     
    TReal32 iPrice;
    /**
     * Currency of iPriceText.
     * Own. 
     */     
    HBufC* iPriceCurrency;
    /**
     * If target is free using this purchase option.
     */ 
    TBool iIsFree;


    /**
     * Id of this purchase option.
     */ 
    HBufC* iPurchaseOptionId;
    
    /**
     * Type for this entity.
     * Own. 
     */   
    MNcdPurchaseOption::TType iType;
    
    /**
     * If purchase process is needed when buying target with this
     * purchase option.
     */    
    TBool iRequirePurchaseProcess;

    /**
     * If this purchaseoption is obsolete and should not be used
     * anymore. This can happen if the node is reinternalized
     * and someone still has handle to any of the purchaseoptions
     * of the node.
     */ 
    TBool iObsolete;

    };



#endif // C_PURCHASE_OPTION_PROXY
