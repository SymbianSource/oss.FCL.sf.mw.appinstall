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
* Description:   Contains CNcdSubscriptionManagerProxy class
*
*/


#ifndef C_NCD_SUBSCRIPTION_MANAGER_PROXY_H
#define C_NCD_SUBSCRIPTION_MANAGER_PROXY_H


// For the streams
#include <s32mem.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdsubscriptionmanager.h"

class CNcdOperationManagerProxy;
class CNcdSubscriptionGroupProxy;
class CNcdSubscriptionProxy;
class MNcdSubscriptionOperation;
class MNcdSubscriptionOperationObserver;
class CNcdKeyValuePair;
class CNcdSubscriptionManagerListener;
class CNcdNodeManagerProxy;

/**
 *  This class implements the functionality for the
 *  MNcdSubscriptionManager interface. The interface is provided for
 *  API users.
 *
 *  @since S60 v3.2
 */
class CNcdSubscriptionManagerProxy : public CNcdInterfaceBaseProxy, 
                                     public MNcdSubscriptionManager
    {

public:

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aOperationManager This class uses operation manager to get the
     * operations for certain tasks.
     * @param aParent Parent whose reference count AddRef of this
     *                object increments.
     * @return CNcdNodeManagerProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdSubscriptionManagerProxy* NewL(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CCatalogsInterfaceBase* aParent,
        CNcdOperationManagerProxy& aOperationManager );

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aParent Parent whose reference count AddRef of this
     *                object increments.
     * @param aOperationManager This class uses operation manager to get the
     * operations for certain tasks. 
     * @return CNcdSubscriptionManagerProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdSubscriptionManagerProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CCatalogsInterfaceBase* aParent,
        CNcdOperationManagerProxy& aOperationManager );


    /**
     * Destructor.
     */
    virtual ~CNcdSubscriptionManagerProxy();
     
    /**
     * Setter for node manager. Needed so that pointer to node manager
     * can be set after construction of this class.
     */
    void SetNodeManager( CNcdNodeManagerProxy* aManager );

    /**
     * Gets the data for descriptors from the server side. This function is
     * called to update proxy data. The function uses the protected virtual
     * internalize functions. So, the child classes may provide their own
     * implementations for internalizations of certain metadata.
     */
    void InternalizeL();
    

    /**
     * Checks if a subscription with given identification is active.
     */    
    TBool ActiveSubscriptionExists( const TDesC& aEntityId,
                                    const TDesC& aNamespace,
                                    const TDesC& aPurchaseOptionId );

    /**
     * @return pointer to specified subscription if such a subscription
     *         exists. (In other words if a subscription such a
     *         subscription has been made at some point.) Returns NULL
     *         if not found.
     *         NOTICE: Ownership is NOT transferred and reference count
     *                 is NOT incremented.
     */    
    CNcdSubscriptionProxy* Subscription( const TDesC& aEntityId,
                                         const TDesC& aNamespace,
                                         const TDesC& aPurchaseOptionId );

    /**
     * This is called by the subscription manager listener when it is notified that
     * the server side subscriptions has been altered.
     */                                         
    void SubscriptionsChangedL();                                         

public: // MNcdSubscriptionManager

    /**
     * @see MNcdSubscriptionManager::SubscriptionsL
     */ 
    virtual RCatalogsArray<MNcdSubscription> SubscriptionsL() const;

    /**
     * @see MNcdSubscriptionManager::RefreshSubscriptionsL
     */ 
    virtual MNcdSubscriptionOperation* RefreshSubscriptionsL( 
        MNcdSubscriptionOperationObserver& aObserver );


protected:

    /**
     * Constructor
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aParent Parent whose reference count AddRef of this
     *                object increments.
     * @param aOperationManager This class uses operation manager to get the
     * operations for certain tasks. 
     * @return CNcdSubscriptionManagerProxy* Pointer to the created object 
     * of this class.
     */
    CNcdSubscriptionManagerProxy(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CCatalogsInterfaceBase* aParent,
        CNcdOperationManagerProxy& aOperationManager );

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
    
    /**
     * Request the identifiers of all the subscription groups in the server side
     * and creates an array containing them.
     *
     * @return The array.
     */
    RPointerArray<CNcdKeyValuePair> SubscriptionGroupIdentifiersL() const;
    
    /**
     * Deletes the subscription groups that don't exist in the given array.
     *
     * @param aGroupIdentifiers Array of subscription group identifiers.
     */
    void DeleteMissingSubscriptionGroups(
        const RPointerArray<CNcdKeyValuePair>& aGroupIdentifiers );
        
    /**
     * Calls InternalizeL to the existing subscription groups.
     */
    void InternalizeSubscriptionGroupsL() const;


private:

    // Prevent if not implemented
    CNcdSubscriptionManagerProxy( 
        const CNcdSubscriptionManagerProxy& aObject );
    CNcdSubscriptionManagerProxy& operator=( 
        const CNcdSubscriptionManagerProxy& aObject );
    
    
    /**
     * @return pointer to specified subscriptiongroup if such a group
     *         exists. (In other words if a subscription from such a group
     *         is made.)
     *         NOTICE: Ownership is NOT transferred and reference count
     *                 is NOT incremented. Although this method
     *                 is internal, it is good to remember to mention these
     *                 things if this method would ever become a public method.
     */    
    CNcdSubscriptionGroupProxy* SubscriptionGroup( const TDesC& aEntityId,
                                                   const TDesC& aNamespace );
    
private: // data

    CNcdOperationManagerProxy& iOperationManager;
    
    /* Pointer to node manager.
     * Not own.
     */
    CNcdNodeManagerProxy* iNodeManager;
    
    RPointerArray<CNcdSubscriptionGroupProxy> iSubscriptionGroups;
    
    /**
     * Active object which listens for update events from the server side subscription
     * manager.
     */
    CNcdSubscriptionManagerListener* iListener;
    
    RBuf8 iInputBuf;
    RBuf8 iOutputBuf;
    
    };


#endif // C_NCD_SUBSCRIPTION_MANAGER_PROXY_H
