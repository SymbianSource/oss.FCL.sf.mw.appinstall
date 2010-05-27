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


#ifndef C_NCDPURCHASEOPTIONIMPL_H
#define C_NCDPURCHASEOPTIONIMPL_H
 
// For streams
#include <s32mem.h>

#include "catalogscommunicable.h"
#include "ncdstoragedataitem.h"
#include "ncdnodeclassids.h"
#include "ncdprotocoltypes.h"
#include "ncdpurchaseoption.h" // To get TType enum for purchaseoption

class MNcdPreminetProtocolPurchaseOption;
class CNcdPurchaseDownloadInfo;

class CNcdServerSubscription;
class CNcdServerPartOfSubscription;
class CNcdServerUpgrade;
class CNcdNodeMetaData;
class CNcdNodeIdentifier;
class CNcdServerSubscribableContent;
class CNcdNodeIcon;


/**
 *  Server-side purchaseoption-implementation
 *
 *  This class implements server-side functionality related to
 *  each node's purchaseoptions.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdPurchaseOptionImpl : public CCatalogsCommunicable,
                               public MNcdStorageDataItem
{

public:

    
    static CNcdPurchaseOptionImpl* NewL(
        const CNcdNodeMetaData& aParentMetaData );
    
    static CNcdPurchaseOptionImpl* NewLC(
        const CNcdNodeMetaData& aParentMetaData );
    
    virtual ~CNcdPurchaseOptionImpl();

    /**
     * Retrieves the data type that informs what class the data is for.
     * By checking the data type information, an InternalizeL function
     * of a right class can be called when the object data is set
     * from the storage.
     * The data type may be decided and set in a object that creates this 
     * class object.
     *
     * @return NcdNodeClassIds::TNcdNodeClassId Describes the data type. 
     */
    NcdNodeClassIds::TNcdNodeClassId ClassId() const;


    /**
     * This function is called when the owner of this object
     * wants to internalize the content according to the data
     * that has been received from the parser.
     * 
     * @param aOption The data is set in the protocol parser and can
     *                be used to initialize this class object.
     */
    void InternalizeL( const MNcdPreminetProtocolPurchaseOption& aOption );


    /**
     * Returns reference to parent meta data's node identifier.
     * 
     * @return Node identifier of parent meta data.
     */
    const CNcdNodeIdentifier& ParentMetaIdentifier() const;

    /**
     * Returns pointer to parent node's subscribable content.
     * 
     * @return Subscribable content info of parent node. NULL if
     *         not found.
     */
    const CNcdServerSubscribableContent* ParentSubscribableContent() const;

    /**
     * Returns reference to parent node's icon.
     * 
     * @return Icon of parent node.
     */
    const CNcdNodeIcon& ParentIconL() const;

    /**
     * Getter for the name of the purchaseoption.
     *
     * @note This function is not named simply Name, because such
     *       function allready exists in CObject. Only the return value
     *       would be different.
     * 
     * @return Name of the purchase option
     */
    const TDesC& PurchaseOptionName() const;

    /**
     * Setter for the id of the purchaseoption. Intended to be used
     * only when creting a new purchase option.
     *
     * @return Id of the purchase option
     */
    void SetIdL( const TDesC& aId );
    
    /**
     * Getter for the id of the purchaseoption.
     *
     * @return Id of the purchase option
     */
    const TDesC& Id() const;

    /**
     * Getter for the type of the purchaseoption.
     *
     * @return Type of the purchase option
     */
    MNcdPurchaseOption::TType PurchaseOptionType() const;

    /**
     * Tells whether this purchase option is free.
     *
     * @return ETrue if the purchase option is free, EFalse otherwise.
     */
    TBool IsFree() const;

    /**
     * Tells whether purchase process is reuqired when using
     * this option.
     *
     * @return ETrue if the purchase process is required, EFalse
     *         otherwise.
     */    
    TBool RequirePurchaseProcess() const;

    /**
     * Setter for price text.
     *
     * @param aPriceText New price text for this purchase option. 
     */
    void SetPriceTextL( const TDesC& aPriceText );

    /**
     * Getter for price text.
     * 
     * @return Textual representation of the price
     */
    const TDesC& PriceText() const;

    /**
     * Getter for amount of download infos.
     *
     * @return Amount of download infos. 
     */
    TInt DownloadInfoCount() const;
    /**
     * Getter for amount of download info with given index.
     *
     * @return Download info.
     */
    const CNcdPurchaseDownloadInfo& DownloadInfo( TInt aInfoIndex ) const;



    /**
     * Getter for the subscription info of this purchaseoption.
     * If subscription is purchased, this tells details of the
     * subscription.
     *
     * @return Object representing subscription info of this purchase option.
     *         NULL if not found.
     *
     */
    const CNcdServerSubscription* SubscriptionInfo() const;

    /**
     * Getter for the partOfSubscription info of this purchaseoption.
     * These things are used to purchase something using a subscription.
     *
     *
     * @return Object representing partOfSubscription info of this
     *         purchase option. NULL if not found.
     *
     */
    const CNcdServerPartOfSubscription* PartOfSubscriptionInfo() const;

    /**
     * Getter for the upgrade info of this purchaseoption. These
     * things are used to upgrade a subscription.
     *
     *
     * @return Object representing upgrade info of this
     *         purchase option. NULL if not found.
     *
     */
    const CNcdServerUpgrade* UpgradeInfo() const;


    /**
     * This function is called to set internal flag of the purchase
     * option to inform whether the purchase option has been recently
     * updated or not.
     *
     * @param aNewState New state for the recently updated flag.
     *
     */
    void SetRecentlyUpdated( TBool aNewState );

    /**
     * This function returns the information whether the purchase
     * option has been recently updated or not.
     *
     * @return State for the recently updated flag.
     */    
    TBool RecentlyUpdated() const;


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

    CNcdPurchaseOptionImpl(
        NcdNodeClassIds::TNcdNodeClassId aClassId,
        const CNcdNodeMetaData& aParentMetaData );

    void ConstructL();

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
     * This function writes the object data to the stream. 
     * The stream content will be sent to the proxy that requested the data.
     * Child classes should add their own data after this parent data.
     * @param aStream The data content of this class object will be written
     * into this stream.
     */
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream ) const;

    /**
     * This function is called from the proxy side. When the proxy
     * is deleted.
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;

private:

    // Prevent these two if they are not implemented
    CNcdPurchaseOptionImpl( const CNcdPurchaseOptionImpl& aObject );
    CNcdPurchaseOptionImpl& operator =( const CNcdPurchaseOptionImpl& aObject );


    /**
     * This function is called during internalization of this object to
     * internalize subscriptions related info.
     * 
     * @param aOption The data is set in the protocol parser and can
     *                be used to initialize this class object.
     */
    void InternalizeSubscriptionsInfoL( 
        const MNcdPreminetProtocolPurchaseOption& aOption );

    /**
     * This function is called during internalization of this object to
     * internalize downloaddetails.
     * 
     * @param aOption The data is set in the protocol parser and can
     *                be used to initialize this class object.
     */
    void InternalizeDownloadDetailsL(
        const MNcdPreminetProtocolPurchaseOption& aOption );
    

    /**
     * Deletes all allocated member variables and sets the pointers to NULL.
     * Also sets other variables to their initial values.
     *
     * @since S60 ?S60_version
     */    
    void ResetMemberVariables();    

private:

    // The class id is identifies this class. The id may be used to
    // identify what kind of class object is created when data is gotten
    // from the db.
    NcdNodeClassIds::TNcdNodeClassId  iClassId;

    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;


    // Parent meta data.
    const CNcdNodeMetaData& iParentMetaData;


    /*
     * Internal flag of the purchase option to inform whether the
     * purchase option has been recently updated or not. This has
     * to be set by user.
     */    
    TBool iRecentlyUpdated;


    /**
     * Name of the option. For example "Try" or "Full".
     * Own. 
     */  
    HBufC* iName;
    /**
     * Textual representation of price when using this purchase option.
     * Own. 
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
     * Own. 
     */
    HBufC* iPurchaseOptionId;

    /**
     * Type of this entity.
     */    
    MNcdPurchaseOption::TType iType;
 
    /**
     * If purchase process is needed when buying target with this
     * purchase option.
     */    
    TBool iRequirePurchaseProcess;

    /**
     * Array to store downloadinfo related to this purchase option
     */     
    RPointerArray<CNcdPurchaseDownloadInfo> iDownloadInfo;
    
    
    // NOTICE: purchaseoption can contain more than one of the
    //         following components. For example it could be
    //         possible that a subscription is a part of subscription
    //         although in reality this is probably never the case.
    
    /**
     * Subscription info if this is a subscription.
     * Own. 
     */    
    CNcdServerSubscription* iSubscription;

    /**
     * Part of subscription info if this is a part of subscription.
     * Own. 
     */    
    CNcdServerPartOfSubscription* iPartOfSubscription;

    /**
     * Upgrade info if this is a upgrade.
     * Own. 
     */
    CNcdServerUpgrade* iUpgrade;
    
    
};


#endif // C_NCDPURCHASEOPTIONIMPL_H
