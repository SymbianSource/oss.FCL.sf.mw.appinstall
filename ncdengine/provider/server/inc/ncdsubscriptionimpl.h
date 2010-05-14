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
* Description:   Contains CNcdSubscription class
*
*/


#ifndef NCD_SUBSCRIPTION_IMPL_H
#define NCD_SUBSCRIPTION_IMPL_H


// For streams
#include <s32mem.h>

#include "catalogscommunicable.h"
#include "ncdstoragedataitem.h"
#include "ncdnodeclassids.h"
#include "ncdsubscription.h"

class MNcdPreminetProtocolSubscription;
class CNcdPurchaseOptionImpl;
class CNcdSubscriptionGroup;

/**
 *  This server side class contains the data and the functionality
 *  that the proxy objects will use to internalize itself.
 *
 *  This object should be added to the session. So, it will be usable
 *  in the proxy side by using the handle gotten during addition. 
 *  The handle is used to identify to what object the proxy directs 
 *  the function call.  When objects are added to sessions, 
 *  multiple handles may be gotten for the same object if addition is 
 *  done multiple times.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdSubscription : public CCatalogsCommunicable,
                         public MNcdStorageDataItem
    {

public:
    /**
     * NewL
     *
     * @param aParentGroup The parent subscription group.
     * @return CNcdSubscription* Pointer to the created object 
     * of this class.
     */
    static CNcdSubscription* NewL( CNcdSubscriptionGroup& aParentGroup);

    /**
     * NewLC
     *
     * @param aParentGroup The parent subscription group.
     * @return CNcdSubscription* Pointer to the created object 
     * of this class.
     */
    static CNcdSubscription* NewLC( CNcdSubscriptionGroup& aParentGroup);

    /**
     * Destructor
     *
     * @note Because this is CCatalogsCommunicable function the
     * session that owns this object should delete this class object.
     * So, instead of directly deleting this object from some other
     * class. Close-method should be used instead.
     */
    virtual ~CNcdSubscription();

    /**
     * Function to query name of the subscription.
     *
     * @return Name of the subscription.
     */    
    const TDesC& SubscriptionName() const;
    
    /**
     * Function to set a new name for the subscription.
     *
     * @param aNewName New name for the subscription.
     */    
    void SetSubscriptionNameL( const TDesC& aNewName );

    /**
     * Function to query purchaseoption that identifies subscription
     * from other subscriptions of same dataentity. 
     *
     * @return Purchaseoption that identifies this subscription.
     */
    const TDesC& PurchaseOptionId() const;

    
    /**
     * Returns the parent subscription group of this subscription.
     *
     * @return The parent group.
     */
    CNcdSubscriptionGroup& ParentGroup() const;

    /**
     * Return the type of the subscription.
     *
     * @return The subscription type.
     */
    MNcdSubscription::TType SubscriptionType() const;

    /**
     * This function is called when the owner of this object
     * wants to internalize the content according to the
     * purchaseoption used to buy this subscription.
     *
     * @note Assumed to be used after purchase of a new subscription.
     *       Some data acquired earlier, for example by using
     *       InternalizeL with protocol parameter, is lost if this
     *       function is called. That is why this function should
     *       be called first.
     * 
     * @param aData Data from purchaseoption that is used to initialize
     *              this subscription.
     *
     * @exception KErrArgument Given purchaseoption does not
     *                         contain subscription information.
     * @exception Leave System wide error code.
     */
    void InternalizeL( const CNcdPurchaseOptionImpl& aData );


    /**
     * This function is called when the owner of this object
     * wants to internalize the content according to the
     * updated subscription data that has been received from
     * the parser.
     *
     * @note This function cannot be used to internalize fully
     *       subsription object because the protocol-object does
     *       not contain all info. At the moment such info is
     *       the name of the subscription that does not exist
     *       in the protocol element.
     * 
     * @param aData The data is set in the protocol parser and can
     *              be used to initialize this class object.
     */
    void InternalizeL( MNcdPreminetProtocolSubscription& aData );


    /**
     * This function is called to set internal flag of the subscription
     * to inform whether the subscription has been recently updated or
     * not.
     *
     * @param aNewState New state for the recently updated flag.
     *
     */
    void SetRecentlyUpdated( TBool aNewState );

    /**
     * This function returns the information whether the subscription
     * has been recently updated or not.
     *
     * @return aNewState New state for the recently updated flag.
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

    /**
     * Constructor
     * Is set in the NewLC function.
     *
     * @param aParentGroup The parent subscription group.
     */
    CNcdSubscription( CNcdSubscriptionGroup& aParentGroup );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    

    // These functions are called from the ReceiveMessage when
    // the given function id has matched to the function.

    /**
     * This function is called when the proxy wants to get the
     * data from the serverside. This function calls the
     * InternalizeDataForRequestL which may be overloaded in the
     * child classes
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void InternalizeRequestL( MCatalogsBaseMessage& aMessage );
        
    /**
     * This function writes the object data to the stream. 
     * The stream content will be sent to the proxy that requested the data.
     * Child classes should add their own data after this parent data.
     * @param aStream The data content of this class object will be written
     * into this stream.
     */
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream );

    /**
     * This function is called from the proxy side. When the proxy
     * is deleted.
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;


private:

    // Prevent these two if they are not implemented
    CNcdSubscription( const CNcdSubscription& aObject );
    CNcdSubscription& operator =( const CNcdSubscription& aObject );


    /**
     * Called when this subscription should be used to
     * buy given purchaseoption.
     *
     * @since S60 ?S60_version
     * @param aData Purchase option which was used to do the
     *              purchase.
     * @exception Leave KErrArgument if the purchase option
     *            does not have needed info.
     */ 
    void UseL( const CNcdPurchaseOptionImpl& aData );
    
    /**
     * Called when this subscription should be upgraded
     * according to given purchase option.
     *
     * @since S60 ?S60_version
     * @param aData Purchase option which was used to do the
     *              purchase of upgrade.
     * @exception Leave KErrArgument if the purchase option
     *            does not have needed info.
     */ 
    void UpgradeL( const CNcdPurchaseOptionImpl& aData );

    /**
     * Externalizes data of this class into stream. Does not
     * externalize objects owned by this class.
     *
     * @since S60 ?S60_version
     * @param aStream The data content of this class object will be written
     *                into this stream.
     */ 
    void ExternalizeOwnDataL( RWriteStream& aStream );

    /**
     * Deletes all allocated member variables that can be
     * reconstructed from a subscriptions protocol object.
     * Sets related pointers to NULL.
     *
     * @since S60 ?S60_version
     */ 
    void ResetProtocolMemberVariables();


    /**
     * Deletes all allocated member variables and sets the pointers
     * to NULL.
     *
     * @since S60 ?S60_version
     */    
    void ResetMemberVariables();

private: // data

    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;
    
    /*
     * Internal flag of the subscription to inform whether the
     * subscription has been recently updated or not. This has
     * to be set by user.
     */    
    TBool iRecentlyUpdated;
    
    /**
     * Name of the subscription.
     * Own.
     */     
    HBufC* iName;
    
    /**
     * Time of expiration.
     * Own.
     */    
    HBufC* iExpiredOn;
    
    
    /**
     * Tells whether subscription is cancelled. In other words
     * usubscribed.
     */    
    TBool iCancelled;
    
    
    /**
     * Type of the subscription.
     */  
    MNcdSubscription::TType iSubscriptionType;



    /**
     * Tells if validity time is used in this subscription.
     */     
    TBool iValidityTimeSet;
    /**
     * Tells time until this subscription is valid.
     */
    TTime iValidUntil;
    
    /**
     * Total validity time that subscription had in the
     * beginning. Notice that like other values this value can
     * also grow for example because of upgrades. This value
     * has to be stored also because in case of renew (when
     * subscription is for example periodic) we have
     * to know how much to increase the current validity.
     */    
    TInt iTotalValidityDelta;


    /**
     * Tells if credit limit is used in this subscription.
     */
    TBool iCreditLimitSet;
    /**
     * Credits that are available using the subscription.
     */ 
    TReal32 iCreditsLeft;
    /**
     * Total credits that were available in the beginning of the
     * subscription.
     */ 
    TReal32 iTotalCredits;


    /**
     * Tells if download limit is used in this subscription.
     */
    TBool iDownloadLimitSet;    
    /**
     * Downloads that are available using the subscription.
     */ 
    TInt iDownloadsLeft;
    /**
     * Total downloads that were available in the beginning of the
     * subscription.
     */    
    TInt iTotalDownloads;
    
    
    /**
     * Id of the purchase option where this subscription was
     * bought from.
     * Own. 
     */
    HBufC* iPurchaseOptionId;
        
    /**
     * The parent subscription group.
     */
    CNcdSubscriptionGroup& iParentGroup;
    };
    
#endif // NCD_SUBSCRIPTION_IMPL_H 
