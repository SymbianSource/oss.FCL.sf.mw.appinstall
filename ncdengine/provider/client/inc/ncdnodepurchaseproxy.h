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
* Description:   ?description
*
*/


#ifndef C_NCD_NODE_PURCHASE_PROXY
#define C_NCD_NODE_PURCHASE_PROXY

#include <s32mem.h>
#include <badesca.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdnodepurchase.h"

class CNcdNodeMetadataProxy;
class CNcdPurchaseOptionProxy;
class CNcdClientSubscribableContent;

/**
 *  Implementation of purchase-interface
 *
 *  @see MNcdNodePurchase
 *  Implementation of purchase-interface which acts also as
 *  a proxy.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodePurchaseProxy : public CNcdInterfaceBaseProxy,
                              public MNcdNodePurchase
    {



public:


    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodePurchaseProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodePurchaseProxy* NewL(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodePurchaseProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodePurchaseProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );

    /**
     * Destructor.
     * The destructor is set public. So, the node that owns the PurchaseProxy may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodePurchaseProxy();


    /**
     * This function is called to update proxy data.
     * The function uses the internalize functions.
     *
     * @since S60 ?S60_version
     */
    void InternalizeL();

    /**
     * This function is called to update proxy data related to
     * purchaseoptions.
     * The function uses the protected virtual internalize functions. 
     * So, the child classes may provide their own implementations 
     * for internalizations of certain data.
     *
     * @since S60 ?S60_version
     */    
    void InternalizeMeansL();

    /**
     * This function is called to update proxy data related to
     * purchasehistory.
     * The function uses the protected virtual internalize functions. 
     * So, the child classes may provide their own implementations 
     * for internalizations of certain data.
     *
     * @since S60 ?S60_version
     */        
    void InternalizeHistoryL();
    
    /**
     * @return CNcdNodeMetadataProxy& Gives the proxy that owns this class object.
     */
    CNcdNodeMetadataProxy& Metadata() const;

    /**
     * Function to get subscribable content related to this
     * NodePurchase. 
     *
     * @return Subscribable content if node is subscribable.
     *         NULL otherwise.
     *
     * @since S60 ?S60_version
     */ 
    const CNcdClientSubscribableContent* SubscribableContent() const;
    
    

public: // from base class MNcdNodePurchase

    /**
     * From MNcdNodePurchase.
     * @see MNcdNodePurchase::PurchaseOptionsL
     */
    virtual RCatalogsArray< MNcdPurchaseOption > PurchaseOptionsL() const;
    

    /**
     * From MNcdNodePurchase.
     * @see MNcdNodePurchase::PurchaseOptionL
     */
    virtual MNcdPurchaseOption* PurchaseOptionL( 
                const TDesC& aPurchaseOptionId ) const;
    

    /**
     * From MNcdNodePurchase.
     * @see MNcdNodePurchase::PurchaseL
     */
    virtual MNcdPurchaseOperation* PurchaseL(
                MNcdPurchaseOption& aPurchaseOption,
                MNcdPurchaseOperationObserver& aObserver );

    /**
     * From MNcdNodePurchase.
     * @see MNcdNodePurchase::IsPurchased
     */
    virtual TBool IsPurchased() const;

    
    /**
     * From MNcdNodePurchase.
     * @see MNcdNodePurchase::PurchasedOptionL
     */
    virtual MNcdPurchaseOption* PurchasedOptionL() const;


    /**
     * From MNcdNodePurchase.
     * @see MNcdNodePurchase::TimeOfPurchaseL
     */
    virtual TTime TimeOfPurchaseL() const;


    /**
     * From MNcdNodePurchase.
     * @see MNcdNodePurchase::PurchasedPriceL
     */
    virtual const TDesC& PurchasedPriceL() const;


protected:

    /**
     * Constructor
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodePurchaseProxy* Pointer to the created object 
     * of this class.
     */
    CNcdNodePurchaseProxy(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );

    virtual void ConstructL();

    /**
     * Function that does the actual internalization of
     * purchaseoption-related data for this proxy.
     *
     * @since S60 ?S60_version
     * @param aStream Stream where from data is internalized.
     */    
    virtual void InternalizeMeansDataL( RReadStream& aStream );

    /**
     * Function that does the actual internalization of
     * purchasehistory-related data for this proxy.
     *
     * @since S60 ?S60_version
     * @param aStream Stream where from data is internalized.
     */
    virtual void InternalizeHistoryDataL( RReadStream& aStream );


private: // new methods

    /**
     * Calls delete to purchaseoptions stored into this
     * class if there are no references left to them.
     * If references are left changes purchaseoption's state
     * to obsolete and removes the purchaseoption from the
     * purchaseoptions stored to this class. After this
     * the one that holds the reference to the
     * purchaseoption should destroy it by calling release.
     *
     * In the end resets the purchaseoption array.
     *
     * @since S60 ?S60_version
     */   
    void DeletePurchaseOptions();

    /**
     * Resets purchasehistory related member variables.
     *
     * @since S60 ?S60_version
     */     
    void ResetHistoryData();
    
    /**
     * Request the purchase option ids that exist in server side.
     *
     * @return Array of the purchase option ids.
     * @since S60 ?S60_version
     */
    CDesCArray* RequestPurchaseOptionIdsL();
    
    /**
     * Releases the purchase options that don't exist in the given
     * array.
     *
     * @param aPurchaseOptionIds Array of purchase option ids.
     * @since S60 ?S60_version
     */
    void ReleaseMissingPurchaseOptions(
        const CDesCArray& aPurchaseOptionIds );
    
    /**
     * Calls InternalizeL() for the current purchase options.
     *
     * @since S60 ?S60_version
     */
    void InternalizeExistingPurchaseOptionsL();
    
    /**
     * Internalizes the subscribable content and purchase options
     * defined in the given array.
     *
     * @param aPurchaseOptionIds Array of purchase option ids.
     * @since S60 ?S60_version
     */
    void InternalizeMeansL( const CDesCArray& aPurchaseOptionIds );
    
    /**
     * Tells whether there is a purchase option with the given id.
     *
     * @param aId The purchase option id.
     * @return True if the id exists, otherwise false.
     * @since S60 ?S60_version
     */
    TBool HasPurchaseOption( const TDesC& aId ) const;

private: // data

    /**
     * Array that holds purchaseOptions of related dataentity.
     * Because we use internal reference counting, we
     * don't use CatalogsArray which calls Release for the
     * objects stored in it when ResetAndRelease is called.
     */
    RPointerArray<CNcdPurchaseOptionProxy> iPurchaseOptions;

    /**
     * Container for possible subscribable content info. Found
     * if this node has a purchase option for buying a subscription.
     * Used when buying a subscription.
     * Own.
     */    
    CNcdClientSubscribableContent* iSubscribableContent;

    /**
     * Whether the node is purchased or not.
     */
    TBool iIsPurchased;

    /**
     * Purchase option id that identifies the purchase option
     * that was used to make the most recent purchase.
     */
    HBufC* iPurchasedOptionId;
    
    /**
     * The time of the purchase if the node has been purchased.
     */
    TTime iTimeOfPurchase;  
      
    /**
     * The final price which was paid when the node was
     * purchased. That is, if it was purchased.
     */
    HBufC* iPurchasedPrice;

    CNcdNodeMetadataProxy& iMetadata;

    };



#endif // C_NCD_NODE_PURCHASE_PROXY
