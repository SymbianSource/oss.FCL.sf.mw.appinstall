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
* Description:   Implements CNcdSubscriptionOperationProxy
*
*/


#include "ncdsubscriptionoperationproxy.h"
#include "ncdsubscriptionoperationobserver.h"
#include "ncdsubscriptionmanagerproxy.h"
#include "ncdqueryimpl.h"
#include "catalogsutils.h"
#include "ncdoperationproxyremovehandler.h"

CNcdSubscriptionOperationProxy* CNcdSubscriptionOperationProxy::NewL(
    MCatalogsClientServer& aSession,
    MNcdSubscriptionOperation::TType aSubscriptionType,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    MNcdSubscriptionOperationObserver& aObserver,
    CNcdSubscriptionManagerProxy* aSubscriptionManager,
    CNcdNodeManagerProxy* aNodeManager,
    MNcdClientLocalizer* aLocalizer )
    {
    CNcdSubscriptionOperationProxy* self =
        CNcdSubscriptionOperationProxy::NewLC(
            aSession,
            aSubscriptionType,
            aHandle,
            aRemoveHandler,
            aObserver,
            aSubscriptionManager,
            aNodeManager,
            aLocalizer );

    CleanupStack::Pop( self );

    return self;
    }

CNcdSubscriptionOperationProxy* CNcdSubscriptionOperationProxy::NewLC(
    MCatalogsClientServer& aSession,
    MNcdSubscriptionOperation::TType aSubscriptionType,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    MNcdSubscriptionOperationObserver& aObserver,
    CNcdSubscriptionManagerProxy* aSubscriptionManager,
    CNcdNodeManagerProxy* aNodeManager,
    MNcdClientLocalizer* aLocalizer )
    {
    CNcdSubscriptionOperationProxy* self =
        new( ELeave ) CNcdSubscriptionOperationProxy( aObserver, aLocalizer );
    
    self->AddRef();
    CleanupReleasePushL( *self );    
    self->ConstructL(
        aSession,
        aSubscriptionType,
        aHandle,
        aRemoveHandler,
        aSubscriptionManager,
        aNodeManager );

    return self;
    }

MNcdSubscriptionOperation::TType
    CNcdSubscriptionOperationProxy::SubscriptionOperationType() const
    {
    return iSubscriptionOperationType;
    }

TNcdInterfaceId CNcdSubscriptionOperationProxy::OperationType() const
    {
    return static_cast<TNcdInterfaceId>(MNcdSubscriptionOperation::KInterfaceUid);
    }

CNcdSubscriptionOperationProxy::CNcdSubscriptionOperationProxy(
    MNcdSubscriptionOperationObserver& aObserver,
    MNcdClientLocalizer* aClientLocalizer)
    : CNcdOperation< MNcdSubscriptionOperation >( aClientLocalizer ), 
      iObserver( aObserver )
    {
    }

CNcdSubscriptionOperationProxy::~CNcdSubscriptionOperationProxy()
    {
    DASSERT( iRemoveHandler );
    if ( iRemoveHandler )
        {        
        DLTRACE(("Removing proxy from remove handler"));
        iRemoveHandler->RemoveOperationProxy( *this );
        }
    }

void CNcdSubscriptionOperationProxy::ConstructL(
    MCatalogsClientServer& aSession,
    MNcdSubscriptionOperation::TType aSubscriptionType,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdSubscriptionManagerProxy* aSubscriptionManager,
    CNcdNodeManagerProxy* aNodeManager )
    {
    DLTRACEIN( ( "" ) );
    CNcdBaseOperationProxy::ConstructL(
        aSession,
        aHandle,
        aRemoveHandler,
        NULL,
        aNodeManager );
    
    iSubscriptionOperationType = aSubscriptionType;

    iSubscriptionManager = aSubscriptionManager;

    DLTRACEOUT( ( "" ) );    
    }


void CNcdSubscriptionOperationProxy::ProgressCallback()
    {
    DLTRACEIN( ( "" ) );

    TNcdSendableProgress& sendableProgress( SendableProgress() );
    TNcdProgress progress( sendableProgress.iProgress, 
                           sendableProgress.iMaxProgress );

    DLTRACE(("Progress: %d/%d", sendableProgress.iProgress,
        sendableProgress.iMaxProgress ));

    iObserver.SubscriptionProgress( *this, progress );

    DLTRACEOUT( ( "" ) );
    }

void CNcdSubscriptionOperationProxy::QueryReceivedCallback(
    CNcdQuery* aQuery )
    {
    DLTRACEIN( ( "" ) );

    aQuery->AddRef();
    iObserver.QueryReceived( *this, aQuery );

    DLTRACEOUT( ( "" ) );
    }

void CNcdSubscriptionOperationProxy::CompleteCallback( TInt aError )
    {
    DLTRACEIN( ( "Error: %d", aError ) );

    // Subscription manager on server side handles the informing
    // of all relevant proxies so no internalization here is needed
    // for the subscription manager proxy.
    
    iObserver.OperationComplete( *this, aError );

    DLTRACEOUT( ( "" ) );
    }
