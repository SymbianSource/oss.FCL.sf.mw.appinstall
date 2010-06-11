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
* Description:   Contains CNcdSubscriptionProxy class
*
*/


#ifndef C_NCD_SUBSCRIPTION_PROXY_H
#define C_NCD_SUBSCRIPTION_PROXY_H


// For the streams
#include <s32mem.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdsubscription.h"

class CNcdOperationManagerProxy;
class CNcdSubscriptionGroupProxy;
class CNcdNodeManagerProxy;

/**
 *  This class implements the functionality for the
 *  MNcdSubscription
 *  These interfaces are provided for API users.
 *
 *  @since S60 v3.2
 */
class CNcdSubscriptionProxy : public CNcdInterfaceBaseProxy, 
                              public MNcdSubscription
    {

public:

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aOperationManager This class uses operation manager to get the
     * operations for certain tasks.
     * @param aNodeManager This class uses node manager to get certain
     *                     nodes.
     * @param aParentGroup The parent subscription group.
     * @return CNcdSubscriptionProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdSubscriptionProxy* NewL(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdOperationManagerProxy& aOperationManager,
        CNcdNodeManagerProxy& aNodeManager,
        CNcdSubscriptionGroupProxy& aParentGroup );

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aOperationManager This class uses operation manager to get the
     * operations for certain tasks.
     * @param aNodeManager This class uses node manager to get certain
     *                     nodes.
     * @param aParentGroup The parent subscription group.
     * @return CNcdSubscriptionProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdSubscriptionProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdOperationManagerProxy& aOperationManager,
        CNcdNodeManagerProxy& aNodeManager,
        CNcdSubscriptionGroupProxy& aParentGroup );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns this object may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdSubscriptionProxy();
     

    /**
     * Gets the data for descriptors from the server side. This function is
     * called to update proxy data. The function uses the protected virtual
     * internalize functions. So, the child classes may provide their own
     * implementations for internalizations of certain metadata.
     */
    void InternalizeL();


    /**
     * Sets subscription into obsolete state and it should
     * not be used anymore.
     *
     * @since S60 ?S60_version
     * @param aIsObsolete New value for obsolete info of the
     *                    subscription.
     */     
    void SetObsolete();

    /**
     * Id of the purchaseoption where from this subscription was
     * bought.
     */    
    virtual TDesC& PurchaseOptionId() const;
    

public: // MNcdSubscription

// from base class MNcdSubscription

    /**
     * From MNcdPurchaseOption.
     * @see MNcdPurchaseOption::Name
     */
    virtual const TDesC& Name() const;

    /**
     * From MNcdPurchaseOption.
     * @see MNcdPurchaseOption::IconL
     */
    virtual HBufC8* IconL() const;
    
    /**
     * From MNcdPurchaseOption.
     * @see MNcdPurchaseOption::SubscriptionStatus
     */
    virtual MNcdSubscription::TStatus SubscriptionStatus() const;

    /**
     * From MNcdPurchaseOption.
     * @see MNcdPurchaseOption::IsObsolete
     */     
    TBool IsObsolete() const;


    /**
     * From MNcdSubscription.
     * @see MNcdSubscription::Unsubscribed
     */
    virtual TBool Unsubscribed() const;

    /**
     * From MNcdSubscription.
     * @see MNcdSubscription::UnsubscribeL
     */
    virtual MNcdSubscriptionOperation* UnsubscribeL( 
        MNcdSubscriptionOperationObserver& aObserver );
    

    /**
     * From MNcdSubscription.
     * @see MNcdSubscription::SubscriptionType
     */
    virtual MNcdSubscription::TType SubscriptionType() const;
    

    /**
     * From MNcdSubscription.
     * @see MNcdSubscription::ValidityTime
     */
    virtual TBool ValidityTime( TTime& aValidUntil ) const;


    /**
     * From MNcdSubscription.
     * @see MNcdSubscription::CreditLimit
     */
    virtual TBool CreditLimit( TReal& aCreditsLeft, TReal& aTotalCredits ) const;


    /**
     * From MNcdSubscription.
     * @see MNcdSubscription::DownloadLimit
     */
    virtual TBool DownloadLimit( TInt& aDownloadsLeft, TInt& aTotalDownloads) const;

    /**
     * From MNcdSubscription.
     * @see MNcdSubscription::OriginNodeL
     */
    virtual MNcdNode* OriginNodeL() const;

    /**
     * From MNcdSubscription.
     * @see MNcdSubscription::OriginPurchaseOptionId
     */
    virtual const TDesC& OriginPurchaseOptionId() const;
    

protected:

    /**
     * Constructor
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aOperationManager This class uses operation manager to get the
     * operations for certain tasks.
     * @param aNodeManager This class uses node manager to get certain
     *                     nodes.
     * @param aParentGroup The parent subscription group.
     * @return CNcdSubscriptionProxy* Pointer to the created object 
     * of this class.
     */
    CNcdSubscriptionProxy( MCatalogsClientServer& aSession,
                           TInt aHandle,
                           CNcdOperationManagerProxy& aOperationManager,
                           CNcdNodeManagerProxy& aNodeManager,
                           CNcdSubscriptionGroupProxy& aParentGroup );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    
    
    /**
     * @return CNcdOperationManagerProxy& Gives the operation manager proxy.
     * Operation manager may be asked to create new operations for this class
     * object.
     */
    CNcdOperationManagerProxy& OperationManager() const;



    // These functions are used to update the data of this class object

    /**
     * @param aStream This stream will contain all the data content for
     * this class object. The stream is gotten from the server side. The
     * memeber variables will be updated according to the data from
     * the stream. 
     */
    virtual void InternalizeDataL( RReadStream& aStream );


private:

    // Prevent if not implemented
    CNcdSubscriptionProxy( const CNcdSubscriptionProxy& aObject );
    CNcdSubscriptionProxy& operator =( const CNcdSubscriptionProxy& aObject );

    void ResetSubscriptionVariables();    
        
    
private: // data

    CNcdOperationManagerProxy& iOperationManager;
    CNcdNodeManagerProxy& iNodeManager;
    
    /**
     * If this subscription is obsolete and should not be used
     * anymore. This can happen if the group is reinternalized
     * and someone still has handle to any of the subscriptions
     * of the group.
     */ 
    TBool iObsolete;

    /*
     * Subscription data.
     * Own.
     */
    HBufC* iName;    
    HBufC* iExpiredOn;
    
    TBool iCancelled;
    
    MNcdSubscription::TType iSubscriptionType;
    
    TBool iValidityTimeSet;
    TTime iValidUntil;
    
    TBool iCreditLimitSet;
    TReal32 iCreditsLeft;
    TReal32 iTotalCredits;
    
    TBool iDownloadLimitSet;
    TInt iDownloadsLeft;
    TInt iTotalDownloads;
    
    HBufC* iPurchaseOptionId;
        
    /**
     * Reference to the parent subscription group.
     */
    CNcdSubscriptionGroupProxy& iParentGroup;

    };


#endif // C_NCD_SUBSCRIPTION_PROXY_H
