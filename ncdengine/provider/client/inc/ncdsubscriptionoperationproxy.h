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


#ifndef C_NCDSUBSCRIPTIONOPERATIONPROXY_H	
#define C_NCDSUBSCRIPTIONOPERATIONPROXY_H

#include "ncdbaseoperationproxy.h"
#include "ncdoperationimpl.h"
#include "ncdsubscriptionoperation.h"
#include "ncdoperationdatatypes.h"

class MNcdSubscriptionOperationObserver;
class CNcdSubscriptionManagerProxy;

class CNcdSubscriptionOperationProxy : 
    public CNcdOperation< MNcdSubscriptionOperation >
    {
public:

    /**
     * Constructor.
     *
     * @param aSession is the session that is used between the proxy and the
     * server.
     * @param aSubscriptionType Subscription type.
     * @param aHandle is the handle that identifies the serverside object that
     * this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.
     * @param aObserver Subscription operation observer.
     * @param aPurchaseOptionId ID of the purchase option used to purchase the
     * subscription.
     * @param aClientLocalizer The client localizer.
     */    
    static CNcdSubscriptionOperationProxy* NewL(
        MCatalogsClientServer& aSession,
        MNcdSubscriptionOperation::TType aSubscriptionType,
        TInt aHandle,
        MNcdOperationProxyRemoveHandler* aRemoveHandler,
        MNcdSubscriptionOperationObserver& aObserver,
        CNcdSubscriptionManagerProxy* aSubscriptionManager,
        CNcdNodeManagerProxy* aNodeManager,
        MNcdClientLocalizer* aClientLocalizer );

    /**
     * Constructor.
     *
     * @param aSession is the session that is used between the proxy and the
     * server.
     * @param aSubscriptionType Subscription type.
     * @param aHandle is the handle that identifies the serverside object that
     * this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.
     * @param aObserver Subscription operation observer.
     * @param aPurchaseOptionId ID of the purchase option used to purchase the
     * subscription.
     * @param aClientLocalizer The client localizer.
     */    
    static CNcdSubscriptionOperationProxy* NewLC(
        MCatalogsClientServer& aSession,
        MNcdSubscriptionOperation::TType aSubscriptionType,
        TInt aHandle,
        MNcdOperationProxyRemoveHandler* aRemoveHandler,
        MNcdSubscriptionOperationObserver& aObserver,
        CNcdSubscriptionManagerProxy* aSubscriptionManager,
        CNcdNodeManagerProxy* aNodeManager,
        MNcdClientLocalizer* aClientLocalizer );

        
public: // From MNcdSubscriptionOperation
    
    /**
     * @see MNcdSubscriptionOperation::SubscriptionOperationType
     */
    MNcdSubscriptionOperation::TType SubscriptionOperationType() const;


public: // From MNcdOperation

    /**
     * @see MNcdOperation::OperationType
     */
    TNcdInterfaceId OperationType() const;


protected: // Constructor and destructor

    /**
     * Constructor.
     *
     * @param aSubscriptionType Subscription type.
     */
    CNcdSubscriptionOperationProxy(
        MNcdSubscriptionOperationObserver& aObserver,
        MNcdClientLocalizer* aClientLocalizer );
    
    virtual ~CNcdSubscriptionOperationProxy();


    /**
     * ConstructL.
     *
     * @param aSession is the session that is used between the proxy and the
     * server.
     * @param aSubscriptionType Subscription type.
     * @param aHandle is the handle that identifies the serverside object that
     * this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aObserver Subscription operation observer.
     * @param aPurchaseOptionId ID of the purchase option used to purchase the
     * subscription.
     */
    void ConstructL(
        MCatalogsClientServer& aSession,
        MNcdSubscriptionOperation::TType aSubscriptionType,
        TInt aHandle,
        MNcdOperationProxyRemoveHandler* aRemoveHandler,
        CNcdSubscriptionManagerProxy* aSubscriptionManager,
        CNcdNodeManagerProxy* aNodeManager );

    
    
protected: // From CNcdBaseOperationProxy
  

    /**
     * @see CNcdBaseOperationProxy::ProgressCallback
     */
    void ProgressCallback();
    
    
    /**
     * @see CNcdBaseOperationProxy::QueryReceivedCallback
     */
    void QueryReceivedCallback( CNcdQuery* aQuery );
    
    
    /**
     * @see CNcdBaseOperationProxy::CompleteCallback
     */
    void CompleteCallback( TInt aError );    
    
    
private: // data

    /**
     * Operation observer, for callbacks.
     * Observer not own.
     */    
    MNcdSubscriptionOperationObserver& iObserver;

    /**
     * Subscription type.
     */
    MNcdSubscriptionOperation::TType iSubscriptionOperationType;

    /**
     * Subscription manager. Not own.
     */
    CNcdSubscriptionManagerProxy* iSubscriptionManager;

    };

#endif // C_NCDSUBSCRIPTIONOPERATIONPROXY_H
