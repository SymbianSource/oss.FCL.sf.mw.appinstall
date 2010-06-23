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


#ifndef C_NCDPURCHASEOPERATIONPROXY_H
#define C_NCDPURCHASEOPERATIONPROXY_H

#include "ncdoperationimpl.h"
#include "ncdbaseoperationproxy.h"
#include "ncdpurchaseoperation.h"

class CNcdPurchaseOptionProxy;
class CNcdSubscriptionManagerProxy;

/**
 *  Purchase operation proxy.
 *
 *  Client side implementation for purchase operation.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdPurchaseOperationProxy : 
    public CNcdOperation< MNcdPurchaseOperation >
    {
    
public:

    /**
     * Constructor
     *
     * @param aSession Session that is used between the proxy and the server.
     * @param aHandle Handle that identifies the serverside object that this 
     *                proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.
     * @param aObserver.
     * @param aLocalizer The client localizer.
     */    
    static CNcdPurchaseOperationProxy* NewL( 
        MCatalogsClientServer& aSession,
        TInt aHandle,
        MNcdOperationProxyRemoveHandler* aRemoveHandler,
        CNcdNodeProxy* aNode,
        CNcdPurchaseOptionProxy* aSelectedPurchaseOption,
        MNcdPurchaseOperationObserver* aObserver,
        CNcdSubscriptionManagerProxy* aSubscriptionManager,
        CNcdNodeManagerProxy* aNodeManager,
        MNcdClientLocalizer* aLocalizer );
    /**
     * Constructor
     *
     * @param aSession Session that is used between the proxy and the server.
     * @param aHandle Handle that identifies the serverside object that this 
     *                proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.
     * @param aObserver 
     * @param aLocalizer The client localizer.
     */    
    static CNcdPurchaseOperationProxy* NewLC(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        MNcdOperationProxyRemoveHandler* aRemoveHandler,
        CNcdNodeProxy* aNode,
        CNcdPurchaseOptionProxy* aSelectedPurchaseOption,
        MNcdPurchaseOperationObserver* aObserver,
        CNcdSubscriptionManagerProxy* aSubscriptionManager,
        CNcdNodeManagerProxy* aNodeManager,
        MNcdClientLocalizer* aLocalizer );




public: // From MNcdPurchaseOperation

    /**
     * @see MNcdPurchaseOperation::PurchaseOption
     */
    virtual const MNcdPurchaseOption& PurchaseOption();
	



public: // From MNcdOperation

    /**
     * @see MNcdOperation::OperationType()
     */
    TNcdInterfaceId OperationType() const;




protected:
    
    /**
     * Constructor
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.
     */
    CNcdPurchaseOperationProxy( MNcdClientLocalizer* aLocalizer );
    
    virtual ~CNcdPurchaseOperationProxy();
    
    void ConstructL( MCatalogsClientServer& aSession,
                     TInt aHandle,
                     MNcdOperationProxyRemoveHandler* aRemoveHandler,
                     CNcdNodeProxy* aNode,
                     CNcdPurchaseOptionProxy* aSelectedPurchaseOption,
                     MNcdPurchaseOperationObserver* aObserver,
                     CNcdSubscriptionManagerProxy* aSubscriptionManager,
                     CNcdNodeManagerProxy* aNodeManager );
    




protected: // From CNcdBaseOperationProxy

    /**
     * @see CNcdBaseOperationProxy::HandleCompletedMessage()
     */
    void HandleCompletedMessage( TNcdOperationMessageCompletionId aCompletionId,
        RReadStream& aReadStream, TInt aDataLength );

    /**
     * @see CNcdBaseOperationProxy::ProgressCallback()
     */
    void ProgressCallback();
    
    
    /**
     * @see CNcdBaseOperationProxy::QueryReceivedCallback()
     */
    void QueryReceivedCallback( CNcdQuery* aQuery );
    
    
    /**
     * @see CNcdBaseOperationProxy::CompleteCallback()
     */
    void CompleteCallback( TInt aError );


    /**
     * @see CNcdBaseOperationProxy::DoCompleteQueryL()
     */    
    void DoCompleteQueryL( MNcdQuery& aQuery );


private: // Data

    /**
     * Operation observer, for callbacks.
     * Observer not own.
     */    
    MNcdPurchaseOperationObserver* iObserver;


    /**
     * Subscription manager proxy.
     * Not own.
     */
    CNcdSubscriptionManagerProxy* iSubscriptionManager;

    /**
     * Pointer to used purchase option.
     * Not own.
     */
    CNcdPurchaseOptionProxy* iPurchaseOption;
	
    };

#endif // C_NCDPURCHASEOPERATIONPROXY_H
