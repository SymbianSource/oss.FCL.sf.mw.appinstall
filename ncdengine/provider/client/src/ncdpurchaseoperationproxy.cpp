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


#include <badesca.h>
#include <s32strm.h>

#include "ncdpurchaseoperationproxy.h"
#include "ncdpurchaseoperationobserver.h"
#include "ncdoperationfunctionids.h"
#include "catalogsdebug.h"
#include "catalogsclientserver.h"
#include "ncdpurchaseoptionproxy.h"
#include "ncdoperationproxyremovehandler.h"
#include "catalogsconstants.h"
#include "catalogsutils.h"
#include "ncdqueryimpl.h"
#include "ncdqueryselectionitemimpl.h"
#include "ncdsubscriptionmanagerproxy.h"
#include "ncdnodeproxy.h"
#include "ncdnodemanagerproxy.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdPurchaseOperationProxy* CNcdPurchaseOperationProxy::NewL( 
    MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    CNcdPurchaseOptionProxy* aSelectedPurchaseOption,
    MNcdPurchaseOperationObserver* aObserver,
    CNcdSubscriptionManagerProxy* aSubscriptionManager,
    CNcdNodeManagerProxy* aNodeManager,
    MNcdClientLocalizer* aLocalizer )
    {
    CNcdPurchaseOperationProxy* self = CNcdPurchaseOperationProxy::NewLC( 
        aSession,
        aHandle,
        aRemoveHandler,
        aNode,
        aSelectedPurchaseOption,
        aObserver,
        aSubscriptionManager,
        aNodeManager,
        aLocalizer );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdPurchaseOperationProxy* CNcdPurchaseOperationProxy::NewLC( 
    MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    CNcdPurchaseOptionProxy* aSelectedPurchaseOption,
    MNcdPurchaseOperationObserver* aObserver,
    CNcdSubscriptionManagerProxy* aSubscriptionManager,
    CNcdNodeManagerProxy* aNodeManager,
    MNcdClientLocalizer* aLocalizer )
    {
    CNcdPurchaseOperationProxy* self =
        new( ELeave ) CNcdPurchaseOperationProxy( aLocalizer );
    
    self->AddRef();
    CleanupReleasePushL( *self );    
    self->ConstructL( 
        aSession, 
        aHandle, 
        aRemoveHandler, 
        aNode, 
        aSelectedPurchaseOption,
        aObserver,
        aSubscriptionManager,
        aNodeManager );

    return self;
    }



// ---------------------------------------------------------------------------
// From MNcdPurchaseOperation
// ---------------------------------------------------------------------------
//
const MNcdPurchaseOption& CNcdPurchaseOperationProxy::PurchaseOption()
    {
    DASSERT( iPurchaseOption );
    return *iPurchaseOption;
    }
	


// ---------------------------------------------------------------------------
// From MNcdOperation
// ?implementation_description
// ---------------------------------------------------------------------------
//
TNcdInterfaceId CNcdPurchaseOperationProxy::OperationType() const
    {
    return static_cast<TNcdInterfaceId>(MNcdPurchaseOperation::KInterfaceUid);
    }




// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdPurchaseOperationProxy::CNcdPurchaseOperationProxy(
    MNcdClientLocalizer* aLocalizer ) : 
    CNcdOperation< MNcdPurchaseOperation >( aLocalizer )
    {
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdPurchaseOperationProxy::~CNcdPurchaseOperationProxy()
    {    
    DLTRACEIN( ( "" ) );
    
    if ( iPurchaseOption != NULL )
        {
        iPurchaseOption->InternalRelease();
        }
    
    DASSERT( iRemoveHandler );
    if ( iRemoveHandler )
        {        
        DLTRACE(("Removing proxy from remove handler"));
        iRemoveHandler->RemoveOperationProxy( *this );
        }    
    DLTRACEOUT( ( "" ) );    
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CNcdPurchaseOperationProxy::ConstructL( MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    CNcdPurchaseOptionProxy* aSelectedPurchaseOption,
    MNcdPurchaseOperationObserver* aObserver,
    CNcdSubscriptionManagerProxy* aSubscriptionManager,
    CNcdNodeManagerProxy* aNodeManager )
    {
    DLTRACEIN( ( "" ) );
    CNcdBaseOperationProxy::ConstructL( aSession, 
                                        aHandle, 
                                        aRemoveHandler,
                                        aNode,
                                        aNodeManager );
    
    iObserver = aObserver;
    iSubscriptionManager = aSubscriptionManager;
    
    // We increment the reference count of the used purchaseoption
    // to ensure that it exists as long as the operation.
    iPurchaseOption = aSelectedPurchaseOption;
    if ( iPurchaseOption != NULL )
        {
        iPurchaseOption->InternalAddRef();
        }
    
    
    // Initialize the operation
//    InitializeOperationL();
    DLTRACEOUT( ( "" ) );
    }



// ---------------------------------------------------------------------------
// HandleCompletedMessage
// ---------------------------------------------------------------------------
//

void CNcdPurchaseOperationProxy::HandleCompletedMessage(
    TNcdOperationMessageCompletionId aCompletionId,
    RReadStream& aReadStream,
    TInt aDataLength )
    {
    DLTRACEIN((_L("aCompletionId =%d, aDataLength =%d"), aCompletionId,
        aDataLength));
    switch ( aCompletionId )
        {
        case ENCDOperationMessageCompletionProgress:
            {
            TRAPD( err, SendContinueMessageL() );
            if ( err != KErrNone ) 
                {
                CompleteCallback( err );
                }
            break;
            }
        case ENCDOperationMessageCompletionComplete:
            {
            DLTRACE( ( "ENCDOperationMessageCompletionComplete" ) );
            // Should get errorcode from the received message
            CompleteCallback( KErrNone );
            break;
            }
                                
        default:
            {
            // Call base class
            CNcdBaseOperationProxy::HandleCompletedMessage( aCompletionId,
                aReadStream,
                aDataLength );
            break;
            }
        }
    }

    
// ---------------------------------------------------------------------------
// Handle progress callback
// ---------------------------------------------------------------------------
//
void CNcdPurchaseOperationProxy::ProgressCallback()
    {
    DLTRACEIN( ( "" ) );
    DASSERT( 0 );
    DLTRACEOUT( ( "" ) );
    }
    
    
// ---------------------------------------------------------------------------
// Handle query received callback
// ---------------------------------------------------------------------------
//
void CNcdPurchaseOperationProxy::QueryReceivedCallback( CNcdQuery* aQuery )
    {
    DLTRACEIN((""));
    aQuery->AddRef();
    
    // Following special case concerns only one query received during
    // the purchase process.
    // The query is received in the first phase of purchase process.
    // It is not received from the server but it is constructed on
    // the server side when the information response is received
    // from the server. The information response contains
    // updated price for the purchase. The updated price is set
    // on the server side into the used purchase option. This is
    // why the purchase option has to be reinternalized.
    if ( aQuery->Id() == KQueryIdPaymentMethod() )
        {
        // If this fails, then possibly updated price
        // cannot be shown to user and purchase process should
        // not be continued
        TRAPD( priceError, iPurchaseOption->InternalizeL() );
        if ( priceError )
            {
            // This error handling is copied and altered from the base
            // operation's DoCancelOperation because call for
            // DoCancelOperation would complete the operation with
            // KErrCancel
            
            
            DLTRACE(("Price update failed. Canceling operation."));
            Cancel();
            
            // Send cancel message to server-side        
            TInt tmpNum( 0 );
            ClientServerSession().
                SendSync( ENCDOperationFunctionCancel,
                          KNullDesC8(),
                          tmpNum,
                          Handle() );     
            SetState( MNcdOperation::EStateCancelled );
            
            DLTRACE(( "Calling complete callback with error: %d",
                      priceError ));
            CompleteCallback( priceError );
            return;
            }
        }    
    
    iObserver->QueryReceived( *this, aQuery );
    DLTRACEOUT((""));
    }
    
    
// ---------------------------------------------------------------------------
// Handle operation complete
// ---------------------------------------------------------------------------
//
void CNcdPurchaseOperationProxy::CompleteCallback( TInt aError )
    {
    DLTRACEIN( ( "Error: %d", aError ) );
    TInt error = aError;

	AddRef();

    DASSERT( iPurchaseOption );
    MNcdPurchaseOption::TType purchaseOptionType =
        iPurchaseOption->PurchaseOptionType();

    DLINFO(("Update purchase history"));
    TRAP_IGNORE( UpdateOperationInfoToPurchaseHistoryL( aError ) );        

    // Subscription manager on server side handles the informing
    // of all relevant proxies so no internalization for
    // subscription manager proxy here is needed.

    // Actually node internalization is probably not needed
    // in case we buy subscription, or subscription upgrade.
    // Then again, a updated price can be received in the
    // purchase operation. Should because of this internalize
    // be done always to update purchase options? Or can the
    // prices change?

    TRAPD( err,
           NodeManager()->InternalizeRelatedNodesL( *NodeProxy() ) );	
    if ( err != KErrNone ) 
	    {
	    error = err;
	    }
	
    iObserver->OperationComplete( *this, error );
    Release();
    DLTRACEOUT( ( "" ) );
    }    
