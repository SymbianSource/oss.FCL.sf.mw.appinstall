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


#include <s32mem.h>

#include "ncdbaseoperationproxy.h"
#include "ncdoperationfunctionids.h"
#include "ncdsendableprogress.h"
#include "ncdsendable.h"
#include "catalogsclientserver.h"
#include "ncdoperationproxyremovehandler.h"
#include "ncdnode.h"
#include "ncdnodeproxy.h"
#include "ncdqueryimpl.h"
#include "catalogsconstants.h"
#include "catalogsutils.h"
#include "ncdnodemanagerproxy.h"
#include "ncdnodeidentifier.h"
#include "ncdexpirednode.h"
#include "ncdutils.h"
#include "ncdpurchasehistory.h"
#include "ncdnodemetadataproxy.h"
#include "ncdproviderproxy.h"
#include "ncdpanics.h"
#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// From class MNcdOperation
// ?implementation_description
// ---------------------------------------------------------------------------
//

MNcdOperation::TState CNcdBaseOperationProxy::DoOperationStateL() const
    {
    return iState;
    }
    
// ---------------------------------------------------------------------------
// From class MNcdOperation
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperationProxy::DoStartOperationL()
    {
    DLTRACEIN((""));
    
    if ( iState == MNcdOperation::EStateCancelled )
        {
        User::Leave( KErrCancel );
        }
    
    if ( iState != MNcdOperation::EStateStopped )
        {
        DLTRACE(("Already running"));
        // Operation is already running.
        //User::Leave( KErrInUse );
        return;
        }
    
    InitBuffersL( 0, 0 );
    
    ClientServerSession().SendAsyncAlloc( ENCDOperationFunctionStart,
        SendBuf8L(),
        iReceiveBuffer,
        Handle(),
        iStatus,
        0 );
    DLTRACE(("Setting active"));
    SetActive();
    DASSERT( iStatus.Int() == KRequestPending );
    SetState( MNcdOperation::EStateRunning );
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// From class MNcdOperation
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperationProxy::DoCancelOperation()
    {
    DLTRACEIN((""));
    if ( iState != MNcdOperation::EStateCancelled &&
        iState != MNcdOperation::EStateComplete )
        {
        DLTRACE(("Sending cancel message to server"));

        Cancel();
        
        // Send cancel message to server-side        
        TInt tmpNum( 0 );
        ClientServerSession().
            SendSync( ENCDOperationFunctionCancel,
                      KNullDesC8(),
                      tmpNum,
                      Handle() );     
        iState = MNcdOperation::EStateCancelled;
        
        
        DLTRACE(("Calling complete callback with KErrCancel"));
        CompleteCallback( KErrCancel );
        }
    }

// ---------------------------------------------------------------------------
// From class MNcdOperation
// ?implementation_description
// ---------------------------------------------------------------------------
//
TNcdProgress CNcdBaseOperationProxy::DoProgress() const
    {
    DLTRACEIN((""));
    TNcdProgress progress( iProgress.iProgress,
                           iProgress.iMaxProgress );
    return progress;
    }


// ---------------------------------------------------------------------------
// From class MNcdOperation
// Completes a query
// ---------------------------------------------------------------------------
//
void CNcdBaseOperationProxy::DoCompleteQueryL( MNcdQuery& aQuery )
    {
    DLTRACEIN((""));
    DASSERT( static_cast<CNcdQuery&>(aQuery).Id() == iQuery->Id() );
    (void)aQuery; // to suppress compiler warning
    delete iSendBuffer;
    iSendBuffer = NULL;
    iSendBuffer = CBufFlat::NewL( KBufExpandSize );
    RBufWriteStream stream( *iSendBuffer );
    CleanupClosePushL( stream );
    iQuery->ExternalizeL( stream );
    iQuery->InternalRelease();
    iQuery = NULL;
    iSendPtr.Set(iSendBuffer->Ptr( 0 ) );    
    DLINFO(("buf length= %d", iSendPtr.Length()));
    
    delete iReceiveBuffer;
    iReceiveBuffer = NULL;

    ClientServerSession().SendAsyncAlloc( ENCDOperationFunctionQueryResponse,
                                     iSendPtr,
                                     iReceiveBuffer,
                                     Handle(),
                                     iStatus,
                                     0 );
    CleanupStack::PopAndDestroy( &stream );    
    SetActive();
    
    }

// ---------------------------------------------------------------------------
// From class MNcdOperation
// ?implementation_description
// ---------------------------------------------------------------------------
//
MNcdNode* CNcdBaseOperationProxy::DoNode()
    {
    if ( iNode )
        {
        // Increase the reference counter for the node if it exists.
        iNode->AddRef();        
        }
        
    return iNode;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdBaseOperationProxy::CNcdBaseOperationProxy(
    MNcdClientLocalizer* aLocalizer ):
        iSendPtr( NULL, 0),
        iStringLocalizer( aLocalizer )
    {
    
    DLTRACEIN((""));
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CNcdBaseOperationProxy::ConstructL( MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    CNcdNodeManagerProxy* aNodeManager )
    {
    DLTRACEIN((""));    
    iSession = &aSession;
    iHandle = aHandle;
    iRemoveHandler = aRemoveHandler;
    iNode = aNode;
    iNodeManager = aNodeManager;

    if ( iNode != NULL )
        {
        // Because we own the node for a while, increase the internal
        // reference counter by one to be sure that is will be alive
        iNode->InternalAddRef();
        }

    // Because this is an active object, we need to inform
    // scheduler about it.    
    CActiveScheduler::Add( this );    
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdBaseOperationProxy::~CNcdBaseOperationProxy()
    {
    DLTRACEIN((""));   

    Cancel(); 

    // Release the server-side operation
    SendReleaseMessage();
    
    delete iReceiveBuffer;
    delete iSendBuffer;
    delete iSendHeapBuf8;        
        
    if ( iNode != NULL )
        {
        iNode->InternalRelease();
        }
        
    if ( iQuery != NULL )
        {
        iQuery->InternalRelease();
        }
    
    iNodeManager = NULL;  
    
    // NOTE: Operation classes MUST remove themselves from
    // the operation manager through the RemoveHandler
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperationProxy::HandleCompletedMessage(
    TNcdOperationMessageCompletionId aCompletionId,
    RReadStream& aReadStream,
    TInt aDataLength )
    {
    DLTRACEIN((_L("aCompletionId =%d, aDataLength =%d"), aCompletionId,
        aDataLength));
    
    TRAPD(err,
    CleanupClosePushL( aReadStream );
    switch ( aCompletionId )
        {
        case ENCDOperationMessageCompletionProgress:
            {
            DLTRACE(("Completion progress"));
            // Read progress info
            iProgress.InternalizeL( aReadStream );                        
            
            
            // Progress callback is last so that the op is resumed 
            // as soon as possible
            ProgressCallback();            

            SendContinueMessageL();            
            break;
            }
        case ENCDOperationMessageCompletionQuery:
            {
            DLTRACE(("Completion query"));
            // A call to complete query is needed after this to
            // continue operation.
            iState = MNcdOperation::EStateQuery;            
            DASSERT( ! iQuery );
            iQuery = CNcdQuery::NewL( aReadStream );
            iQuery->SetClientLocalizer( iStringLocalizer );
            if( iQuery->MessageTitle() == KNullDesC &&
                iQuery->MessageBody() == KNullDesC && 
                iQuery->ItemCount() < 1 )
                {
                DLTRACE(("No localizations for query without items, don't show"));
                iQuery->SetResponseL( MNcdQuery::EAccepted );
                DoCompleteQueryL( *iQuery );
                }
            else
                {
                QueryReceivedCallback( iQuery );
                }
            break;
            }
        case ENCDOperationMessageCompletionError:
            {
            DLTRACE(("Completion error: %d", iStatus.Int() ));
            iState = MNcdOperation::EStateComplete;
            CompleteCallback( iStatus.Int() );
            break;
            }
        case ENCDOperationMessageCompletionComplete:
            {
            DLTRACE(("Completion complete"));
            iState = MNcdOperation::EStateComplete;
            CompleteCallback( KErrNone );
            break;
            }
            
        case ENCDOperationMessageCompletionInit:
            {
            DLTRACE(("Completion init"));
            iState = MNcdOperation::EStateStopped;
            InitializationCallback( aReadStream, aDataLength );
            break;
            }
            
        case ENCDOperationMessageCompletionExpirationInfo:
            {
            DLTRACE(("ENCDOperationMessageCompletionExpirationInfo"));
            TInt nodeCount = aReadStream.ReadInt32L();
            RPointerArray<CNcdExpiredNode> expiredNodes;
            CleanupResetAndDestroyPushL( expiredNodes );
            for ( TInt i = 0 ; i < nodeCount ; i++ )
                {
                CNcdExpiredNode* expiredNode = CNcdExpiredNode::NewLC(
                    aReadStream );
                DLINFO( (_L("Expired node: id=%S, ns=%S, force update=%d"),
                    &expiredNode->NodeIdentifier().NodeId(),
                    &expiredNode->NodeIdentifier().NodeNameSpace(),
                    expiredNode->ForceUpdate() ) );
                DLINFO(( "Appending nodes." ));
                expiredNodes.AppendL( expiredNode );
                DLINFO(( "Popping expired one." ));
                CleanupStack::Pop( expiredNode );
                }
            // send expiration info to provider
            DLINFO(( "Send exp info." ));
            iNodeManager->HandleExpiredNodesL( expiredNodes );
            DLINFO(( "Pop & destroy expired nodes." ));
            CleanupStack::PopAndDestroy( &expiredNodes );
            DLINFO(( "Continue message" ));
            SendContinueMessageL();
            DLTRACE(("ENCDOperationMessageCompletionExpirationInfo done"));
            break;
            }
                
        default:
            {
            // Unknown completion id -> corrupt data
            iState = MNcdOperation::EStateStopped;
            CompleteCallback( KErrCorrupt );
            }
        }
    
    CleanupStack::PopAndDestroy( &aReadStream );
    ); // TRAPD
    if ( err != KErrNone )
        {
        DLERROR(("Error %d! Calling CompleteCallback", err));
        CompleteCallback( err );
        }
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperationProxy::SendContinueMessageL()
    {
    DLTRACEIN(("IsActive: %d", IsActive() ));
    DASSERT( iState != MNcdOperation::EStateCancelled );
    
    // Prevents continue message sending when the server's response to
    // an asyncronous message has not yet been received but some
    // synchronous operation wants to send a continue msg because it
    // thinks that there isn't any.
    // 
    // Basically this fixes NCDALTCI-369 where client-side crashed when a 
    // download was quickly paused and resumed and the server had just 
    // completed a progress message before the download was paused but
    // it got handled after the op was resumed again
    if ( !IsActive() ) 
        {            
        InitBuffersL( 0, 0 );
        ClientServerSession().SendAsyncAlloc( 
            ENCDOperationFunctionContinue,
            SendBuf8L(),
            iReceiveBuffer,
            Handle(),
            iStatus,
            0 );
        DLTRACE(("Setting active"));
        SetActive();        
        }
    }
    

// ---------------------------------------------------------------------------
// Sends a message that releases the operation on the server side
// ---------------------------------------------------------------------------
//    
void CNcdBaseOperationProxy::SendReleaseMessage()
    {
    DLTRACEIN((""));
    
    TInt tmpNum( 0 );
    ClientServerSession().
        SendSync( ENCDOperationFunctionRelease,                  
                  KNullDesC8(),
                  tmpNum,
                  Handle() );     
                  
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperationProxy::InitBuffersL( TInt aSendSize, TInt /*aReceiveSize*/)
    {
    DLTRACEIN((""));
    
    // Make sure that the send buffer exists.
    TRAPD( sendError, SendBuf8L() );
    if( sendError == KErrNotFound )
        {
        SetSendBuf8( HBufC8::NewL( aSendSize ) );
        }
    
    delete iReceiveBuffer;
    iReceiveBuffer = NULL;
    }


// ---------------------------------------------------------------------------
// Initializes the operation
// ---------------------------------------------------------------------------
//
void CNcdBaseOperationProxy::InitializeOperationL()
    {
    DLTRACEIN((""));
    
    // Get the initialization buffer from the implementing class    
    HBufC8* initBuf = CreateInitializationBufferL();    

    SetSendBuf8( initBuf );

    delete iReceiveBuffer;
    iReceiveBuffer = NULL;

    DLTRACE(("Sending init message"));
    // Initialization is done synchronously
    User::LeaveIfError( ClientServerSession().SendSyncAlloc( 
        ENCDOperationFunctionInit,
        SendBuf8L(),
        iReceiveBuffer,
        Handle(),
        0 ) );

    DLTRACE(("Received response, &iReceiveBuffer: %X, length: %d", iReceiveBuffer,
        iReceiveBuffer->Length() ));
    if ( ! iReceiveBuffer || iReceiveBuffer->Length() < sizeof(TInt) )
        {
        DLTRACE(("Corrupt data"));
        // Corrupt data.
        iState = MNcdOperation::EStateStopped;
        User::Leave( KErrCorrupt );
        }
    
    DLTRACE(("Reading completion id"));    
    // Let's read the completion id
    TInt dataLength( iReceiveBuffer->Length() );
    RDesReadStream desReadStream( *iReceiveBuffer );
    CleanupClosePushL( desReadStream );
    
    TNcdOperationMessageCompletionId completionId = 
        static_cast<TNcdOperationMessageCompletionId>( 
        desReadStream.ReadInt32L() );
    
    DASSERT( completionId == ENCDOperationMessageCompletionInit );

    CleanupStack::Pop(); // RDesReadStream
    
    DLTRACE(("Handling completed message"));
    // Handler functions will take care of the rest.
    HandleCompletedMessage( completionId, desReadStream,
        dataLength );
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Empty implementation
// ---------------------------------------------------------------------------
//
HBufC8* CNcdBaseOperationProxy::CreateInitializationBufferL()
    {
    return HBufC8::NewL( 0 );
    }


// ---------------------------------------------------------------------------
// Empty implementation
// ---------------------------------------------------------------------------
//
void CNcdBaseOperationProxy::InitializationCallback( 
    RReadStream& /* aReadStream */, TInt aDataLength )
    {
    DLTRACEIN( ( "Data length: %d", aDataLength ) );
    (void)aDataLength;   // to suppress compiler warning
    }


// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperationProxy::SetState( MNcdOperation::TState aState )
    {
    iState = aState;
    }
    
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
TNcdSendableProgress& CNcdBaseOperationProxy::SendableProgress()
    {
    return iProgress;
    }


CNcdNodeManagerProxy* CNcdBaseOperationProxy::NodeManager() const
    {
    return iNodeManager;
    }

CNcdNodeProxy* CNcdBaseOperationProxy::NodeProxy() const
    {
    return iNode;
    }


// ---------------------------------------------------------------------------
// UpdateOperationInfoToPurchaseHistoryL
// ---------------------------------------------------------------------------
//
void CNcdBaseOperationProxy::UpdateOperationInfoToPurchaseHistoryL( TInt aErrorCode )
    {
    DLTRACEIN((""));
    
    CNcdNodeProxy* node( NodeProxy() );
    NCD_ASSERT_ALWAYS( node, ENcdPanicNoData );
        
    CNcdNodeMetadataProxy* metadata( node->Metadata() );
    NCD_ASSERT_ALWAYS( metadata, ENcdPanicNoData );

    // Notice, that release must be called, when history is not used anymore.
    MNcdPurchaseHistory* purchaseHistory( 
        NodeManager()->Provider().PurchaseHistoryL() );
    NCD_ASSERT_ALWAYS( purchaseHistory, ENcdPanicNoData );
    
    CleanupReleasePushL( *purchaseHistory );

    CNcdNodeIdentifier& identifier =
        metadata->Identifier();


    DLINFO(("Create filter"));
    // Create the filter. So, we will get
    // the purchase history details we want.
    CNcdPurchaseHistoryFilter* filter =
        CNcdPurchaseHistoryFilter::NewLC();

    // Add client Uids to the filter
    RArray< TUid > clientUids;
    CleanupClosePushL( clientUids );
    clientUids.AppendL( identifier.ClientUid() );
    filter->SetClientUids( clientUids.Array() );
    CleanupStack::PopAndDestroy( &clientUids );
    
    // Add other filter values
    filter->SetNamespaceL( identifier.NodeNameSpace() );
    filter->SetEntityIdL( identifier.NodeId() );


    // Get the ids. So, we can next get all the corresponding
    // details and update them into the purchase history.
    RArray< TUint > ids =
        purchaseHistory->PurchaseIdsL( *filter );

    CleanupStack::PopAndDestroy( filter );

    CleanupClosePushL( ids );
    
    // Get all the details and add history items into the
    // array.    
    if ( ids.Count() > 0 )
        {
        DLINFO(("At least one purchase id existed: %d", ids.Count() ));
        
        // Operations always use the newest details. So, use it here also.
        // We do not want to load icons. So, use EFalse.
            
        CNcdPurchaseDetails* details(
            purchaseHistory->PurchaseDetailsL( ids[ 0 ] , EFalse ) );
            
        if ( details != NULL )
            {
            CleanupStack::PushL( details );
            
            DLINFO(("Details was found"));
                            
            // Set the error code and the lates operation time
            details->SetLastOperationErrorCode( aErrorCode );
            details->SetLastUniversalOperationTime();
                
            // Save purchase details into the purchase history.
            // This will replace the old detail.
            // But, do not replace old icon, because we did not load it
            // for the details above.
            purchaseHistory->SavePurchaseL( *details, EFalse );
            
            CleanupStack::PopAndDestroy( details );
            details = NULL;            
            }
        }
    
    CleanupStack::PopAndDestroy( &ids );  
    
    CleanupStack::PopAndDestroy( purchaseHistory );  
    
    DLTRACEOUT((""));
    }
    
    
// ---------------------------------------------------------------------------
// From CActive
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperationProxy::RunL()
    {
    DLTRACEIN(("iReceiveBuffer=%X, status: %d, state: %d", 
        iReceiveBuffer, iStatus.Int(), iState ));

    
    if ( iStatus.Int() == KErrCancel ) 
        {
        DLINFO(("The operation was cancelled by engine"));
        // Do not complete with KErrCancel since it is used when the operation
        // is cancelled by UI. These cases has to be distinguished somehow.
        iState = MNcdOperation::EStateCancelled;
        CompleteCallback( KErrAbort );
        return;
        }
    else if ( iState == MNcdOperation::EStateCancelled )
        {
        DLINFO(("The operation has already been cancelled"));
        // Ignore message
        return;
        }
    
    if ( ! iReceiveBuffer || iReceiveBuffer->Length() < sizeof(TInt) )
        {
        DLTRACE(("corrupt data"));
        // Corrupt data.
        iState = MNcdOperation::EStateStopped;
        // error callback: KErrCorrupt
        CompleteCallback( KErrCorrupt );
        return;
        }

    // Let's read the completion id
    TInt dataLength( iReceiveBuffer->Length() );
    RDesReadStream desReadStream( *iReceiveBuffer );
    TNcdOperationMessageCompletionId completionId = ENCDOperationMessageCompletionError;
    TRAPD( err, completionId =
        static_cast<TNcdOperationMessageCompletionId>
        ( desReadStream.ReadInt32L() ) );
    if ( err != KErrNone )
        {
        DLTRACE(("corrupt"));
        CompleteCallback( KErrCorrupt );
        return;
        }
    DLTRACE(("id read"));
    // Handler functions will take care of the rest.
    HandleCompletedMessage( completionId, desReadStream,
        dataLength );
    DLTRACEOUT((""));
    }
    
// ---------------------------------------------------------------------------
// From CActive
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperationProxy::DoCancel()
    {
    DLTRACEIN((""));
    
    DLTRACE(("Cancel async requests"));
    ClientServerSession().AsyncMessageSenderDown( iStatus ); 
    }
    

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
TInt CNcdBaseOperationProxy::RunError( TInt aError )
    {
    DLTRACEIN(("Error: %d in RunL()", aError ));
    return aError;
    }
    
MCatalogsClientServer& CNcdBaseOperationProxy::ClientServerSession() const
    {
    DLTRACEIN((""));    
    return *iSession;
    }    
    
TInt CNcdBaseOperationProxy::Handle() const
    {
    return iHandle;
    }


const TDesC8& CNcdBaseOperationProxy::SendBuf8L()
    {
    if( iSendHeapBuf8 == NULL )
        {
        User::Leave( KErrNotFound );
        }
        
    return *iSendHeapBuf8;
    }

    
void CNcdBaseOperationProxy::SetSendBuf8( HBufC8* aBuffer )
    {
    delete iSendHeapBuf8;
    iSendHeapBuf8 = aBuffer;
    }
