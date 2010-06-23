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

#include "ncdbaseoperation.h"
#include "catalogsbasemessage.h"
#include "catalogsdebug.h"
#include "ncdqueryimpl.h"
#include "ncd_cp_queryresponseimpl.h"
#include "ncd_cp_query.h"
#include "catalogsutils.h"
#include "ncdoperationobserver.h"
#include "ncd_pp_information.h"
#include "catalogssession.h"
#include "ncdoperationremovehandler.h"
#include "ncd_pp_error.h"
#include "ncd_pp_expiredcacheddata.h"
#include "ncdnodeidentifier.h"
#include "ncdnodemanager.h"
#include "catalogscontext.h"
#include "ncddeviceinteractionfactory.h"
#include "ncddeviceservice.h"
#include "ncdquerytextitem.h"
#include "ncdparser.h"
#include "ncdprotocoldefaultobserver.h"
#include "catalogsconstants.h"
#include "ncdexpirednode.h"
#include "ncdrequestbase.h"
#include "ncdproviderutils.h"
#include "ncderrors.h"
#include "ncdgeneralmanager.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdBaseOperation::~CNcdBaseOperation()
    {
    DLTRACEIN((""));
    
    if ( iRemoveHandler ) 
        {
        iRemoveHandler->RemoveOperation( *this );
        }
    
    iObservers.Close();    
    if ( iActiveQuery )
        {
        iActiveQuery->InternalRelease();
        }
    if ( iPendingMessage )
        {
        DLTRACE(("Message pending"));
        // Operation destroyed unexpectedly when a message was still pending
        iPendingMessage->CompleteAndRelease( KErrDied );
        }    
    
    iEmbeddedDataQuerys.ResetAndDestroy();
    for( TInt i = 0 ; i < iPendingQuerys.Count() ; i++ )
        {
        iPendingQuerys[i]->InternalRelease();
        }
    iPendingQuerys.Close();
    delete iRunner;
    iExpiredNodes.ResetAndDestroy();
    for ( TInt i = 0 ; i < iCompletedQuerys.Count() ; i++ )
        {
        iCompletedQuerys[i]->InternalRelease();
        }
    iCompletedQuerys.Reset();
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CNcdBaseOperation::Start()
    {
    DLTRACEIN((""));
    if ( iOperationState == EStateStopped )
        {
        // Op not yet running, start it
        iOperationState = EStateRunning;
        TInt err = RunOperation();
        DLTRACE(("Returned from RunOperation()"));
        DLTRACEOUT((""));
        return err;
        }
    else
        {
        // Op already started.
        DLTRACEOUT((""));
        return KErrInUse;
        }
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperation::AddObserverL( 
    MNcdOperationObserver* aObserver )
    {
    DLTRACEIN((""));
    DASSERT( aObserver );
    iObservers.AppendL( aObserver );
    }


// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CNcdBaseOperation::RemoveObserver( 
    MNcdOperationObserver* aObserver )
    {
    DLTRACEIN((""));
    TInt index = iObservers.Find( aObserver );
    if ( index != KErrNotFound )
        {
        iObservers.Remove( index );
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
TNcdOperationType CNcdBaseOperation::Type() const
    {
    DLTRACEIN((""));
    return iOperationType;
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperation::HandleStartMessage( 
    MCatalogsBaseMessage* aMessage )    
    {
    DLTRACEIN((""));
    DASSERT( ! iPendingMessage );
    if ( iOperationState == EStateStopped )
        {        
        iPendingMessage = aMessage;
        TInt err = Start();
        DLTRACE(( "Start returned with: %d", err ));
        }
    else
        {
        DLTRACE(("Operation already started"));
        // Operation already started.
        TInt err = CompleteMessage( aMessage, ENCDOperationMessageCompletionError,
            KErrInUse );
        if ( err != KErrNone )
            {
            FailOperation( err );
            }
        }       
    DLTRACEOUT(("")); 
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperation::HandleCancelMessage( 
    MCatalogsBaseMessage* aMessage )
    {
    DLTRACEIN((""));
    // release pending message if any
    if ( iPendingMessage )
        {
        DLTRACE(("Completing pending message"));
       
        // Completing the message so no memory is pending in the
        // framework because of it.            
        iPendingMessage->CompleteAndRelease( KErrCancel );            
        iPendingMessage = NULL;
        }
    
    // iOperationState has to be set to EStateCancelled before
    // cancelling purchase operation.
    // See CNcdPurchaseOperationImpl::HandleSmsEvent()
    iOperationState = EStateCancelled;
    Cancel();

    aMessage->CompleteAndRelease( KErrNone );

    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperation::HandleContinueMessage( 
    MCatalogsBaseMessage* aMessage )
    {
    DLTRACEIN(( "iOperationState: %d", iOperationState ));
    DASSERT( ! iPendingMessage );
    if ( iOperationState == EStateRunning )
        {
        DLTRACE(("iOperationState == EStateRunning"));        
        // Message is just stored here, it will be completed 
        // when progress has been made.
        iPendingMessage = aMessage;
        iOperationState = EStateRunning;
        
        RunOperation();
        DLTRACE(("Returned from RunOperation()"));
        }
    else if ( iOperationState == EStateQuery )
        {
        DLTRACE(("iOperationState == EStateQuery"));
        // A query is pending and has not yet been sent to proxy        
        TInt err = CompleteMessage( aMessage, ENCDOperationMessageCompletionQuery,
            *iActiveQuery,
            KErrNone );
        if( err != KErrNone )
            {
            FailOperation( err );
            }
        }
    else if ( iOperationState == EStateSendExpirationInfo )
        {
        DLTRACE(("iOperationState == EStateSendExpirationInfo"));
        iOperationState = EStateRunning;
        iPendingMessage = aMessage;
        TInt err = SendExpirationInfo( iExpiredNodes );
        if( err != KErrNone )
            {
            FailOperation( err );            
            }
        else
            {
            iExpiredNodes.ResetAndDestroy();
            if ( iActiveQuery )
                {
                // there's a query pending, change state accordingly
                iOperationState = EStateQuery;
                }
            }
        }
    else
        {
        DLTRACE(("Not waiting for a continue message!"));
        // Fail operation can't complete the message if it's not set
        // to iPendingMessage
        iPendingMessage = aMessage;
        
        // Not waiting for a continue message.
        FailOperation( KErrNotReady );
        }
    DLTRACEOUT((""));
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperation::HandleQueryResponseMessage( 
    MCatalogsBaseMessage* aMessage )
    {
    DLTRACEIN((""));
    DASSERT( ! iPendingMessage );
    if ( iOperationState == EStateQuery )
        {        
        DASSERT( iActiveQuery );
        // store message, it will be completed later
        iPendingMessage = aMessage;
        iOperationState = EStateRunning;
        
        DLINFO(("input length= %d",iPendingMessage->InputLength()));
        
        TRAPD( err,
            {            
            HBufC8* des = HBufC8::NewLC( iPendingMessage->InputLength() );
            TPtr8 ptr = des->Des();
            iPendingMessage->ReadInput( ptr );
            RDesReadStream stream( *des );
            CleanupReleasePushL( stream );
            iActiveQuery->InternalizeL( stream );
            CleanupStack::PopAndDestroy( &stream );
            CleanupStack::PopAndDestroy( des );
            QueryHandledL( iActiveQuery );
            }); // TRAP
            
        if( err != KErrNone )
            {
            FailOperation( err );
            }
        
            
        
        // Should handle cases where both expired info and querys have been received
        /*else if( iExpiredNodes.Count() > 0 )
            {
            DLTRACE(("Expired nodes received previously, send to proxy"));
            TInt err = SendExpirationInfo( iExpiredNodes );
            if( err != KErrNone )
                {
                FailOperation( err );
                }
            }*/
        }
    else
        {
        DLTRACE(("Not waiting for a query"));
        // Not waiting for a query response message.
        TInt err = CompleteMessage( aMessage, ENCDOperationMessageCompletionError,
            KErrNotReady );
        if ( err != KErrNone )
            {
            FailOperation( err );
            }
        }
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Handle initialization messages
// ---------------------------------------------------------------------------
//
void CNcdBaseOperation::HandleInitMessage( MCatalogsBaseMessage* aMessage )
    {
    DLTRACEIN( ( "" ) );
    if ( iOperationState == EStateStopped )
        {
        // this will probably cause an error in the proxy, remove?
        if ( iPendingMessage )
            {
            DLTRACE( ( "MESSAGE PENDING! THIS CAN NOT BE!" ) );
            iPendingMessage->CompleteAndRelease( KErrAlreadyExists );
            DASSERT( 0 );
            }
        iPendingMessage = aMessage;
        Initialize();
        }
    else
        {
        // Operation already started.
        TInt err = CompleteMessage( aMessage, 
            ENCDOperationMessageCompletionError,
            KErrInUse );
        if ( err != KErrNone )
            {
            FailOperation( err );
            }
        }        
    DLTRACEOUT((""));
    }
    
    

// ---------------------------------------------------------------------------
// Handles release messages
// ---------------------------------------------------------------------------
//
void CNcdBaseOperation::HandleReleaseMessage( MCatalogsBaseMessage* aMessage )    
    {
    DLTRACEIN((""));
    if ( iPendingMessage ) 
        {
        
        // Notice that because we complete the iPendingMessage here the
        // client has to say AsyncMessageSenderDown() to the
        // ClientServer so the client won't receive this
        // iPendingMessage. iPendingMessage is released here so no memory
        // is pending because of it.
        DLTRACE(("Releasing pending message"));
        iPendingMessage->CompleteAndRelease( KErrCancel );            
        iPendingMessage = NULL;        
        DLTRACE(("Message released"));
        }
        
    MCatalogsSession& requestSession( aMessage->Session() );
    DLTRACE(("Getting handle"));
    TInt handle( aMessage->Handle() );
    
    DLTRACE(("Completing message"));
    aMessage->CompleteAndRelease( KErrNone );
    DLTRACE(("Removing object"));
    requestSession.RemoveObject( handle );
    }
        


// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CNcdBaseOperation::CompleteMessage( MCatalogsBaseMessage* & aMessage,
        TNcdOperationMessageCompletionId aId,
        const MNcdSendable& aSendableObject,
        TInt aStatus )
    {
    DLTRACEIN((""));
    DLTRACE(("Handle: %d", aMessage->Handle()));
    TRAPD(err,    
        {
        DLTRACE(("new buf"));
        CBufBase* buf = CBufFlat::NewL( 30 );
        CleanupStack::PushL( buf );
        RBufWriteStream stream( *buf );
        CleanupClosePushL( stream );
        stream.WriteInt32L( aId );
        aSendableObject.ExternalizeL( stream );
        CleanupStack::PopAndDestroy( &stream );
        TPtrC8 ptr = buf->Ptr( 0 );
        DLTRACE(("complete msg"));
        DASSERT((aMessage != NULL));
        aMessage->CompleteAndReleaseL( ptr, aStatus );
        DLTRACE(("pop buf"));
        CleanupStack::PopAndDestroy( buf );
        DLTRACE(("done"));
        });
    if( err != KErrNone )
        {
        aMessage->CompleteAndRelease( aStatus );
        }
    aMessage = NULL;
    DLTRACEOUT(("error=%d", err));
    return err;
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CNcdBaseOperation::CompleteMessage(MCatalogsBaseMessage* & aMessage,
        TNcdOperationMessageCompletionId aId,
        TInt aStatus )
    {
    DLTRACEIN((""));
    DASSERT((aMessage));
    DLTRACE(("Handle: %d", aMessage->Handle()));
    TRAPD(err,
        {
        HBufC8* buffer = HBufC8::NewLC( sizeof(TInt) );
        TPtr8 bufferPtr( buffer->Des() );        
        RDesWriteStream desWriteStream( bufferPtr );
        CleanupClosePushL( desWriteStream );
        desWriteStream.WriteInt32L( aId );
        CleanupStack::PopAndDestroy( &desWriteStream );
        aMessage->CompleteAndReleaseL( *buffer, aStatus );
        CleanupStack::PopAndDestroy( buffer );
        });
    if( err != KErrNone )
        {
        aMessage->CompleteAndRelease( aStatus );
        }
    aMessage = NULL;
    DLTRACEOUT(("error=%d", err));
    return err;
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CNcdBaseOperation::CompleteMessage( MCatalogsBaseMessage*& aMessage,
        TNcdOperationMessageCompletionId aId,
        const MNcdSendable& aSendableObject,
        RPointerArray<CNcdNodeIdentifier>& aNodes,
        TInt aStatus )
    {
    DLTRACEIN(("Handle: %i", aMessage->Handle()));
    
    TRAPD(err,    
        {
        CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
        CleanupStack::PushL( buf );
        RBufWriteStream stream( *buf );
        CleanupClosePushL( stream );
        stream.WriteInt32L( aId );
        aSendableObject.ExternalizeL( stream );
        stream.WriteInt32L( aNodes.Count() );
        DLINFO(("Node loop: %d", aNodes.Count()));
        for ( TInt i = 0 ; i < aNodes.Count() ; i++ )
            {
            aNodes[i]->ExternalizeL( stream );
            }
        CleanupStack::PopAndDestroy( &stream );
        TPtrC8 ptr = buf->Ptr( 0 );
        aMessage->CompleteAndReleaseL( ptr, aStatus );
        CleanupStack::PopAndDestroy( buf );
        });
    if( err != KErrNone )
        {
        aMessage->CompleteAndRelease( aStatus );
        }
    aMessage = NULL;
    DLTRACEOUT((""));
            
    return err;
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CNcdBaseOperation::CompleteMessage( MCatalogsBaseMessage*& aMessage,
        TNcdOperationMessageCompletionId aId,
        RPointerArray<CNcdExpiredNode>& aExpiredNodes,
        TInt aStatus )
    {
    DLTRACEIN(("Handle: %i", aMessage->Handle()));
    
    TRAPD(err,    
        {
        CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
        CleanupStack::PushL( buf );
        RBufWriteStream stream( *buf );
        CleanupClosePushL( stream );
        stream.WriteInt32L( aId );
        stream.WriteInt32L( aExpiredNodes.Count() );
        for ( TInt i = 0 ; i < aExpiredNodes.Count() ; i++ )
            {
            aExpiredNodes[i]->ExternalizeL( stream );
            }
        CleanupStack::PopAndDestroy( &stream );
        TPtrC8 ptr = buf->Ptr( 0 );
        aMessage->CompleteAndReleaseL( ptr, aStatus );
        CleanupStack::PopAndDestroy( buf );
        });
    if( err != KErrNone )
        {
        aMessage->CompleteAndRelease( aStatus );
        }
    DLINFO(("message null"));    
    aMessage = NULL;
    DLTRACEOUT((""));
            
    return err;
    }

MCatalogsSession& CNcdBaseOperation::Session()
    {
    return iSession;
    }

TBool CNcdBaseOperation::IsSubOperation()
    {
    return iIsSubOperation;
    }


// ---------------------------------------------------------------------------
// Progress getter
// ---------------------------------------------------------------------------
//
const TNcdSendableProgress& CNcdBaseOperation::Progress() const
    {
    return iProgress;
    }


// ---------------------------------------------------------------------------
// Handle setter
// ---------------------------------------------------------------------------
//
void CNcdBaseOperation::SetHandle( TInt aHandle )
    {
    iHandle = aHandle;
    }


// ---------------------------------------------------------------------------
// Handle getter
// ---------------------------------------------------------------------------
//
TInt CNcdBaseOperation::Handle() const
    {
    return iHandle;
    }

// ---------------------------------------------------------------------------
// From class CCatalogsCommunicable
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperation::ReceiveMessage( 
        MCatalogsBaseMessage* aMessage,
        TInt aFunctionNumber )
    {
    DLTRACEIN((_L("Handle: %i, aFunctionNumber=%d"), aMessage->Handle(),
        aFunctionNumber));
    
    
    switch ( aFunctionNumber )
        {
        case ENCDOperationFunctionStart:
            {
            HandleStartMessage( aMessage );
            break;
            }
        case ENCDOperationFunctionCancel:
            {
            HandleCancelMessage( aMessage );
            break;
            }
        case ENCDOperationFunctionContinue:
            {
            HandleContinueMessage( aMessage );
            break;
            }
        case ENCDOperationFunctionQueryResponse:
            {
            HandleQueryResponseMessage( aMessage );
            break;
            }
        case ENCDOperationFunctionInit:
            {
            HandleInitMessage( aMessage );
            break;
            }
            
        case ENCDOperationFunctionRelease:
            {
            HandleReleaseMessage( aMessage );
            break;
            }
            
        }
    DLTRACEOUT((""));
    }
    
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdBaseOperation::CounterPartLost( const MCatalogsSession& aSession ) 
    {
    DLTRACEIN((""));
    Cancel();
    if ( iPendingMessage )
        {
        // This function may be called whenever -- when the message is waiting
        // response or when the message does not exist. The life time of the message
        // ends shortly after CompleteAndRelease is called.
        iPendingMessage->CounterPartLost( aSession );        
        }
    }

void CNcdBaseOperation::QueryL( MNcdConfigurationProtocolQuery* aData )
    {
    DLTRACEIN((""));    
    // querys from embedded data end up here, just store them
	
    // maybe this should not be added to the array if it's an autenthication
    // query? on the other hand it could be referred from somewhere else.
    CleanupDeletePushL( aData );
    iEmbeddedDataQuerys.AppendL( aData );    
    CleanupStack::Pop( aData );
    // special case: authentication query, should actually come in
    // information/Messages according to protocol but CGW sends it in
    // embeddedData/Queries.
    // 
    if( aData->Semantics() == MNcdQuery::ESemanticsAuthenticationQuery )
        {
        iPendingQuerys.AppendL( CNcdQuery::NewLC( *aData ) );
        CleanupStack::Pop();
        }
    DLTRACEOUT((""));
    }

void CNcdBaseOperation::InformationL(
    MNcdPreminetProtocolInformation* aData )
    {
    DLTRACEIN(("aData: %X", aData));    
    CleanupDeletePushL( aData );
    DLTRACE(("Handling queries"));
    // store querys from information response
    // the inheriting operation will handle these if needed
    for ( TInt i = 0 ; i < aData->MessageCount() ; i++ )
        {
        CNcdQuery* query = CNcdQuery::NewLC( aData->MessageL(i) );
        iPendingQuerys.AppendL( query );
        CleanupStack::Pop( query );
        }
    
    DLTRACE(("Checking for expired cached data"));
    
    // handle expired cached data
    const MNcdPreminetProtocolExpiredCachedData* expiredData = 
        aData->ExpiredCachedData();
    
    if ( expiredData )
        {
        DLTRACE(("Handling expired data"));
        RPointerArray<CNcdExpiredNode> expiredNodes;
        CleanupResetAndDestroyPushL( expiredNodes );
        iNodeManager->SetNodesExpiredByMetadataL( *expiredData,
            iSession.Context().FamilyId(),
            aData->Namespace(),
            expiredNodes );
        ExpirationInfoReceived( this, expiredNodes );
        CleanupStack::PopAndDestroy( &expiredNodes );
                    }
                
    DLTRACE(("Should handle stuff"));
    
    DASSERT( iParser );
    CleanupStack::Pop( aData );
    iParser->DefaultObserver().InformationL( aData );
    DLTRACEOUT((""));
    }

void CNcdBaseOperation::ErrorL( MNcdPreminetProtocolError* /* aData */ )
    {
    DLTRACEIN((""));
    //switch 
    
    }

void CNcdBaseOperation::Progress( CNcdBaseOperation& /*aOperation*/ )
    {
    DLTRACEIN(("default implementation, does nothing"));
    }
    
void CNcdBaseOperation::QueryReceived( CNcdBaseOperation& /*aOperation*/,
    CNcdQuery* /*aQuery*/ )
    {
    DLTRACEIN(("default implementation, does nothing"));
    }

void CNcdBaseOperation::OperationComplete( CNcdBaseOperation* /*aOperation*/,
    TInt /*aError*/ )
    {
    DLTRACEIN(("default implementation, does nothing"));
    }
                                    
void CNcdBaseOperation::ExpirationInfoReceived( CNcdBaseOperation* /*aOperation*/,
        RPointerArray<CNcdExpiredNode>& aExpiredNodes )
    {
    DLTRACEIN((""));
    if ( iIsSubOperation )
        {
        DLINFO(( "Subop, inform observers of expiration info" ));
        // send to observer
        for( TInt i = 0 ; i < iObservers.Count() ; i++ )
            {
            iObservers[i]->ExpirationInfoReceived( this, aExpiredNodes );
            }
        }
    else
        {
        if( iPendingMessage )
            {
            DLINFO(( "Message exists, send expiration info" ));
            // message available, send
            TInt err = SendExpirationInfo( aExpiredNodes );
            if( err != KErrNone )
                {
                FailOperation( err );
                }
            }
        else
            {
            DLINFO(( "Message does not exist, cannot send expiration info" ));
            // no message, store
            iOperationState = EStateSendExpirationInfo;
            for( TInt i = 0 ; i < aExpiredNodes.Count() ; i++ )
                {
                TRAPD( err, iExpiredNodes.AppendL( aExpiredNodes[i] ) );//TRAPD
                if( err != KErrNone )
                    {
                    FailOperation( err );
                    }
                }
            aExpiredNodes.Reset();
            }
        }
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdBaseOperation::CNcdBaseOperation( 
    CNcdGeneralManager& aGeneralManager,    
    MNcdOperationRemoveHandler* aRemoveHandler,
    TNcdOperationType aOperationType, 
    MCatalogsSession& aSession, 
    TBool aIsSubOperation )
    : iGeneralManager( aGeneralManager ),
      iError( KErrNone ), 
      iRemoveHandler( aRemoveHandler ),
      iOperationType( aOperationType ), 
      iOperationState( EStateStopped ),
      iSession( aSession ),
      iIsSubOperation( aIsSubOperation ), 
      iNodeManager( &aGeneralManager.NodeManager() )      
    {
    DLTRACEIN((""));    
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CNcdBaseOperation::ConstructL()
    {
    DLTRACEIN((""));
    }
    
// ---------------------------------------------------------------------------
// Empty initializer
// ---------------------------------------------------------------------------
//
TInt CNcdBaseOperation::Initialize()
    {
    DLTRACEIN( ( "THIS SHOULD NOT BE CALLED" ) );
    DASSERT( 0 );
    return KErrNone;
    }

void CNcdBaseOperation::ChangeToPreviousStateL()
    {
    DLTRACEIN(("SHOULD NEVER BE CALLED, ERROR!"))
    DASSERT(0);
    }

void CNcdBaseOperation::HandleEngineQueryItemsL( CNcdQuery* aQuery )
    {
    DLTRACEIN((""));
    aQuery->InternalAddRef();
    for ( TInt i = 0 ; i < aQuery->ItemCount() ; i++ )
        {
        CNcdQueryItem& queryItem = aQuery->QueryItemL( i );
        switch ( queryItem.Semantics() )
            {
            case MNcdQueryItem::ESemanticsImei:
                {
                MNcdQueryTextItem* textQueryItem = NULL;
                textQueryItem = queryItem.QueryInterfaceLC< MNcdQueryTextItem >();
                DASSERT( textQueryItem );
                // get imei from device service
                MNcdDeviceService* deviceService =
                    NcdDeviceInteractionFactory::CreateDeviceServiceLC();
                textQueryItem->SetTextL( deviceService->ImeiL() );
                CleanupStack::PopAndDestroy( deviceService );
                CleanupStack::PopAndDestroy( textQueryItem );
                break;
                }
            default:
                {
                // not an engine query item
                break;
                }
            }
        }
    aQuery->InternalRelease();
    }

MNcdConfigurationProtocolQueryResponse* CNcdBaseOperation::CreateResponseL(
    CNcdQuery& aQuery )
    {
    DLTRACEIN((""));
    if ( aQuery.Response() == MNcdQuery::ENoResponse )
        {
        User::Leave( KErrNotReady );
        }
    CNcdConfigurationProtocolQueryResponseImpl* queryResponse = 
        CNcdConfigurationProtocolQueryResponseImpl::NewLC();
    AssignDesL( queryResponse->iId, aQuery.Id() );
    DLINFO((_L("Added query id=%S"), queryResponse->iId ));
    
    if( aQuery.Response() == MNcdQuery::ERejected )
        {
        // query has been cancelled
        queryResponse->iCancel = ETrue;
        }
    else
        {        
        // query has been accepted, add response values
        for ( TInt i = 0 ; i < aQuery.ItemCount() ; i++ )
            {
            CNcdQueryItem& item = aQuery.QueryItemL( i );
            if ( item.IsOptional() && ! item.IsSet() )
                {
                continue;
                }
            if ( !item.IsSet() )
                {
                User::Leave( KErrNotReady );
                }
            CNcdConfigurationProtocolQueryResponseValueImpl* value = 
                CNcdConfigurationProtocolQueryResponseValueImpl::NewLC();
            AssignDesL( value->iId, item.Id() );
            HBufC* valueDes = item.ValueL().AllocLC();
            value->iValues.AppendL( valueDes );
            CleanupStack::Pop( valueDes );
            queryResponse->iResponses.AppendL( value );
            CleanupStack::Pop( value );
            DLINFO((_L("Added query item response id=%S, value=%S"),
                value->iId, valueDes ));
            }
        }
    
    CleanupStack::Pop( queryResponse );

    return queryResponse;
    }


void CNcdBaseOperation::QueryReceivedL( CNcdQuery* aQuery )
    {
    DLTRACEIN((""));
    aQuery->InternalAddRef();
    iOperationState = EStateQuery;
    DASSERT( ! iActiveQuery );
    iActiveQuery = aQuery;
    HandleEngineQueryItemsL( iActiveQuery );
    if ( iActiveQuery->ItemCount() > 0 && iActiveQuery->AllItemsSet() )
        {
        // all items set by engine, accept and complete
        DLTRACEIN(("all items set by engine -> complete"));
        iActiveQuery->SetResponseL( MNcdQuery::EAccepted );
        iOperationState = EStateRunning;
        QueryHandledL( iActiveQuery );
        DLTRACEOUT((""));
        return;
        }
    else if ( iIsSubOperation )
        {
        DASSERT( iObservers.Count() > 0 )
        // this is a sub operation send the query to observer
        // maybe there should be only one observer/parent
        iObservers[0]->QueryReceived( *this, aQuery );
        }
    else if ( iPendingMessage )
        {
        TInt err = CompleteMessage( iPendingMessage, ENCDOperationMessageCompletionQuery,
            *iActiveQuery, KErrNone );
        if ( err != KErrNone )
            {
            FailOperation( err );
            }
        }
    DLTRACEOUT((""));
    }
    
void CNcdBaseOperation::CompleteCallback()
    {
    for ( TInt i = 0 ; i < iObservers.Count() ; i++ )
        {
        iObservers[i]->OperationComplete( this, iError );
        }
    }
    
void CNcdBaseOperation::ContinueOperationL()
    {
    DLTRACEIN((""));
    if( !iRunner )
        {
        iRunner = CNcdAsyncRunner::NewL( this );
        }
    iRunner->Start();
    }

TInt CNcdBaseOperation::SendExpirationInfo(
    RPointerArray<CNcdExpiredNode>& aExpiredNodes )
    {
    DLTRACEIN((""));
    if( iPendingMessage )
        {
        return CompleteMessage( iPendingMessage, 
            ENCDOperationMessageCompletionExpirationInfo,
            aExpiredNodes,
            iError );
        }
    return KErrNotFound;
    }

TBool CNcdBaseOperation::QueryCompletedL( CNcdQuery* /*aQuery*/ )
    {
    DLTRACEIN(("THIS SHOULD NOT BE CALLED! ERROR!"));
    DASSERT( 0 );
    return EFalse;
    }

CNcdQuery* CNcdBaseOperation::ActiveQuery()
    {
    if( iActiveQuery )
        {
        iActiveQuery->InternalAddRef();
        return iActiveQuery;
        }
    return NULL;
    }

TInt CNcdBaseOperation::QueryEntityCount()
    {
    return iEmbeddedDataQuerys.Count();
    }

const MNcdConfigurationProtocolQuery& CNcdBaseOperation::QueryEntityL(
    const TDesC& aId )
    {    
    MNcdConfigurationProtocolQuery* query = NULL;
    for ( TInt i = 0 ; i < iEmbeddedDataQuerys.Count() ; i++ )
        {
        if( iEmbeddedDataQuerys[i]->Id() == aId )
            {
            query = iEmbeddedDataQuerys[i];
            break;
            }
        }
    if ( query == NULL )
        {
        User::Leave( KErrNotFound );
        }   
    return *query;
    }

void CNcdBaseOperation::ContinueOperation()
    {
    RunOperation();
    }

void CNcdBaseOperation::FailOperation( TInt aError )
    {
    DLTRACEIN(("aError: %d",aError));
    iError = aError;
    Cancel();
    if( iIsSubOperation )
        {
        // send to observer
        for( TInt i = 0 ; i < iObservers.Count() ; i++ )
            {
            iObservers[i]->OperationComplete( this, iError );
            }
        }
    else if ( iPendingMessage )
        {
        // complete message with error code, possible error message is ignored
        // as the operation has already failed
        CompleteMessage( iPendingMessage, 
            ENCDOperationMessageCompletionError,
            iError );
        }
    }
    
void CNcdBaseOperation::AddQueryResponsesL( CNcdRequestBase* aRequest )
    {
    DLTRACEIN((""));
    // add querys        
    for ( TInt i = 0 ; i < iCompletedQuerys.Count() ; i++ )
        {
        CNcdQuery* query = iCompletedQuerys[i];
        DASSERT(( query->Response() == MNcdQuery::EAccepted || 
            query->Response() == MNcdQuery::ERejected ));
        MNcdConfigurationProtocolQueryResponse* queryResponse =
            CreateResponseL( *query );
        CleanupStack::PushL( queryResponse );
        aRequest->AddQueryResponseL( queryResponse );
        CleanupStack::Pop( queryResponse );
        }
    }
    
void CNcdBaseOperation::HandleQuerysL()
    {
    DLTRACEIN((""));
    if ( iPendingQuerys.Count() > 0 )
        {
        // still a query left, send it to proxy
        CNcdQuery* query = iPendingQuerys[0];
        // Should set "is secure uri" flag
        // remove from array
        iPendingQuerys.Remove(0);
        // handle query
        CNcdBaseOperation::QueryReceivedL( query );
        // release own reference
        query->InternalRelease();
        }
    else
        {
        if( iCompletedQuerys.Count() > 0 )
            {
            // there are completed querys that need to be responded to
            // -> resend request with the query responses
            ResendRequestL();
            }
        else
            {
            // all queries handled, continue operation normally
            ContinueOperationL();
            }
        }
    }
    
void CNcdBaseOperation::QueryHandledL( CNcdQuery* aQuery )
    {
    DLTRACEIN((""));
    DASSERT( aQuery == iActiveQuery )
    DASSERT( aQuery->Response() == MNcdQuery::EAccepted ||
    aQuery->Response() == MNcdQuery::ERejected )

    // this may leave (e.g. in some cases when the query has been rejected )
    TBool handled = QueryCompletedL( aQuery );
    if ( !handled && aQuery->ItemCount() > 0 )
        {
        if( aQuery->Response() == MNcdQuery::EAccepted )
            {
            // query was not handled in the inheriting op, but was accepted
            // add it to the list for later retrieval
            iCompletedQuerys.AppendL( aQuery );
            }
        else if( aQuery->Semantics() == MNcdQuery::ESemanticsAuthenticationQuery &&
            aQuery->Response() == MNcdQuery::ERejected )
            {
            // special case; server want's query responses for rejected authentication
            // queries, so add it to the list
            iCompletedQuerys.AppendL( aQuery );
            }
        else if( aQuery->Response() == MNcdQuery::ERejected 
            && !aQuery->IsOptional() )
            {
            // a rejected, and mandatory, query was not handled -> fail the operation
            User::Leave( KNcdErrorMandatoryQueryRejected );
            }
        else
            {
            // optional query rejected, release own reference to it
            iActiveQuery->InternalRelease();
            }
        }
    else
        {
        // query has been handled or has no items (i.e. server message)
        // release own reference to it
        iActiveQuery->InternalRelease();
        }
    // active query is now handled, set to null
    iActiveQuery = NULL;
    // handle querys if some are still left
    HandleQuerysL();
    }
    
TInt CNcdBaseOperation::QueriesPending()
    {
    DLTRACEIN((""));
    return iPendingQuerys.Count();
    }
    
void CNcdBaseOperation::ResendRequestL()
    {
    DLTRACEIN((""));
    ChangeToPreviousStateL();
    ContinueOperationL();
    }

void CNcdBaseOperation::ClearCompletedQueries()
    {
    DLTRACEIN((""));
    for ( TInt i = 0 ; i < iCompletedQuerys.Count() ; i++ )
        {
        iCompletedQuerys[i]->InternalRelease();
        }
    iCompletedQuerys.Reset();
    }
