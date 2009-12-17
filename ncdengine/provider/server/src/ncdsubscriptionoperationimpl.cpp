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
#include <apmstd.h>

#include "ncdsubscriptionoperationimpl.h"
#include "catalogsbasemessage.h"
#include "catalogshttpincludes.h"
#include "ncdsubscriptionmanagerimpl.h"
#include "catalogsutils.h"
#include "catalogscontext.h"
#include "ncdproviderdefines.h"
#include "ncddescriptordownloadsuboperation.h"
#include "ncdrequestmanagesubscriptions.h"
#include "ncdrequestgenerator.h"
#include "ncdrequestconfigurationdata.h"
#include "ncdprotocol.h"
#include "ncdparser.h"
#include "ncdprotocoldefaultobserver.h"
#include "ncd_pp_error.h"
#include "ncd_pp_subscription.h"
#include "ncdsubscriptionimpl.h"
#include "ncdpurchaseoperationimpl.h"
#include "ncdsubscriptiongroup.h"
#include "ncdnodeidentifier.h"
#include "ncdserverdetails.h"
#include "ncdconfigurationmanager.h"
#include "ncdcapabilities.h"
#include "ncderrors.h"
#include "ncdsessionhandler.h"
#include "ncdproviderutils.h"
#include "ncdhttputils.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdSubscriptionOperation* CNcdSubscriptionOperation::NewL( 
    MNcdSubscriptionOperation::TType aSubscriptionOperationType,   
    CNcdGeneralManager& aGeneralManager,
    CNcdSubscriptionManager& aSubscriptionManager,        
    MCatalogsHttpSession& aHttpSession,    
    MNcdOperationRemoveHandler& aRemoveHandler,
    MCatalogsSession& aSession )
    {
    CNcdSubscriptionOperation* self = CNcdSubscriptionOperation::NewLC(
        aSubscriptionOperationType,
        aGeneralManager,
        aSubscriptionManager,
        aHttpSession,
        aRemoveHandler,
        aSession );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CNcdSubscriptionOperation* CNcdSubscriptionOperation::NewLC( 
    MNcdSubscriptionOperation::TType aSubscriptionOperationType,
    CNcdGeneralManager& aGeneralManager,
    CNcdSubscriptionManager& aSubscriptionManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler& aRemoveHandler,
    MCatalogsSession& aSession )
    {
    CNcdSubscriptionOperation* self =
        new( ELeave ) CNcdSubscriptionOperation( 
            aSubscriptionOperationType,
            aGeneralManager,
            aSubscriptionManager,
            aHttpSession,
            aRemoveHandler,
            aSession );
    CleanupClosePushL( *self );
    self->ConstructL( NULL, KNullDesC, KNullDesC, KNullDesC, KNullDesC );
    return self;
    }
    

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdSubscriptionOperation* CNcdSubscriptionOperation::NewL(
    MNcdSubscriptionOperation::TType aSubscriptionOperationType,
    const TDesC& aPurchaseOptionId,
    const TDesC& aEntityId,
    const TDesC& aNamespace,
    const TDesC& aServerUri,
    CNcdGeneralManager& aGeneralManager,
    CNcdSubscriptionManager& aSubscriptionManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler& aRemoveHandler,
    MCatalogsSession& aSession )
    {
    CNcdSubscriptionOperation* self = CNcdSubscriptionOperation::NewLC(
        aSubscriptionOperationType,
        aPurchaseOptionId,
        aEntityId,
        aNamespace,
        aServerUri,
        aGeneralManager,
        aSubscriptionManager,
        aHttpSession,
        aRemoveHandler,
        aSession );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CNcdSubscriptionOperation* CNcdSubscriptionOperation::NewLC(
    MNcdSubscriptionOperation::TType aSubscriptionOperationType,
    const TDesC& aPurchaseOptionId,
    const TDesC& aEntityId,
    const TDesC& aNamespace,
    const TDesC& aServerUri,
    CNcdGeneralManager& aGeneralManager,
    CNcdSubscriptionManager& aSubscriptionManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler& aRemoveHandler,
    MCatalogsSession& aSession )
    {
    CNcdSubscriptionOperation* self =
        new( ELeave ) CNcdSubscriptionOperation( 
            aSubscriptionOperationType,
            aGeneralManager,
            aSubscriptionManager,
            aHttpSession,
            aRemoveHandler,
            aSession );
    CleanupClosePushL( *self );
    self->ConstructL(
        NULL,
        aPurchaseOptionId,
        aEntityId,
        aNamespace,
        aServerUri );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdSubscriptionOperation::~CNcdSubscriptionOperation()
    {
    DLTRACEIN((""));    

    DLTRACE(("Closing suboperations"));
    // Close operations
    for ( TInt i = 0; i < iSubOps.Count(); i++ )
        {
        iSubOps[i]->Close();        
        }
                
    DLTRACE(("Suboperations closed"));
    iSubOps.Reset();
    iFailedSubOps.Reset();
    iCompletedSubOps.Reset();

    if ( iTransaction )
        {
        iTransaction->Release();
        iTransaction = NULL;
        }

    iServersSubscriptions.ResetAndDestroy();

    delete iSource;
    delete iPurchaseOptionId;
    delete iEntityId;
    delete iNamespace;
    delete iServerUri;

    delete iParser;

    DLTRACEOUT((""));
    }


TInt CNcdSubscriptionOperation::RunOperation()
    {
    DLTRACEIN(( "Pending message: %X", iPendingMessage ));

    // Cannot determine from iOperationState whether we are
    // just starting this operation or continuing it after
    // for example expiration info has been handled.
    // This is because iOperationState will be EStateRunning
    // In both cases. This is why we use our own internal
    // variable.
    if ( iSubscriptionOperationState == EBegin )
        {
        // Operation is being started and we come here for
        // the first time
        iSubscriptionOperationState = ERunning;
        }
    else if ( iSubscriptionOperationState == EComplete )
        {
        // It is possible that the operation has completed while we have
        // been processing for example expired nodes. So we send completion
        // message immediately.
        if ( iPendingMessage )
            {
            return CompletePendingMessage();
            }
        return KErrGeneral;
        }    
    else if ( iSubscriptionOperationState == EHandlingQueries ) 
        {
        DLTRACE(("Queries handled"));
        SubscriptionOperationComplete( iError );        
        }
    else
        {
        // We don't want to start the operation again as it
        // is already running. Something should eventually trigger
        // completion of iMessage
        return KErrNone;
        }

    TRAPD( err,
        {
        switch ( iSubscriptionOperationType ) 
            {
            case MNcdSubscriptionOperation::EUnsubscribe:
                RunUnsubscribeOperationL();
                break;
            case MNcdSubscriptionOperation::ERefreshSubscriptions:
                RunRefreshOperationL();
                break;
            default:            
                User::Leave( KErrNotSupported );
                break;
            }
        } );

    if ( err != KErrNone )
        {
        SubscriptionOperationComplete( err );
        }

    DLTRACEOUT(("err: %d", err));
    return err;
    }

void CNcdSubscriptionOperation::Cancel()
    {
    DLTRACEIN( ( "" ) );

    if ( iTransaction )
        {
        iTransaction->Cancel();
        iTransaction = NULL;
        }
    if ( iParser )
        {
        iParser->CancelParsing();
        //delete iParser;
        //iParser = NULL;
        }
        
    CancelSuboperations();            

    DLTRACEOUT(( "" ));
    }
    

TBool CNcdSubscriptionOperation::QueryCompletedL( CNcdQuery* /* aQuery */ )
    {
    DLTRACEIN((""));
    return EFalse;
    }
    

void CNcdSubscriptionOperation::HandleHttpEventL( 
        MCatalogsHttpOperation& aOperation, 
        TCatalogsHttpEvent aEvent )
    {
    DLTRACEIN((""));
    DASSERT( &aOperation == iTransaction );
    DASSERT( aOperation.OperationType() == ECatalogsHttpTransaction );

    TCatalogsTransportProgress progress( iTransaction->Progress() );
    
    switch( aEvent.iOperationState ) 
        {
        // Handle completed operation
        case ECatalogsHttpOpCompleted:
            {
            // Inform parser that no more data will be sent
            iParser->EndL();
            break;
            }        
        // Handle operation in progress
        case ECatalogsHttpOpInProgress:
            {
            if( aEvent.iProgressState == ECatalogsHttpResponseBodyReceived )
                {
                // send received data to parser
                iParser->ParseL( aOperation.Body() );
                }      
            break;
            }
                    
        default:
            {
            break;
            }
        }
        
    DLTRACEOUT((""));    
    }

TBool CNcdSubscriptionOperation::HandleHttpError(
        MCatalogsHttpOperation& aOperation,
        TCatalogsHttpError aError )
    {
    DLTRACEIN(("Error type: %d, code: %d", aError.iType, aError.iError ));    
    
    DASSERT( &aOperation == iTransaction );
    
    aOperation.Release();
    iTransaction = NULL;

    SubscriptionOperationComplete( aError.iError );
        
    DLTRACEOUT((""));
    return ETrue;
    }

void CNcdSubscriptionOperation::ParseError( TInt aErrorCode )
    {
    DLTRACEIN(("error:%d", aErrorCode ));

    // Handle error only if operation is not completed already
    // (cancellation of parsing may cause an unnecessary call to this function).
    if ( iSubscriptionOperationState != EComplete )
        {
            if ( iTransaction )
            {
            iTransaction->Cancel();        
            iTransaction = NULL;
            }

        SubscriptionOperationComplete( aErrorCode );

        }
    
    DLTRACEOUT((""));
    }

void CNcdSubscriptionOperation::ParseCompleteL( TInt aError )
    {
    DLTRACEIN((_L("error:%d"), aError ));

    if ( iParser )
        {
        delete iParser;
        iParser = NULL;
        }

    // HandleQuerysL will continue the operation in RunOperation
    iSubscriptionOperationState = EHandlingQueries;
    iError = aError;
    HandleQuerysL();

    DLTRACEOUT((""));
    }

void CNcdSubscriptionOperation::ValidSubscriptionL(
    MNcdPreminetProtocolSubscription* aData )
    {
    DLTRACEIN((""));

    iServersSubscriptions.AppendL( aData );

    DLTRACEOUT((""));
    }

void CNcdSubscriptionOperation::OldSubscriptionL(
    MNcdPreminetProtocolSubscription* aData )
    {
    DLTRACEIN((""));

    iServersSubscriptions.AppendL( aData );

    DLTRACEOUT((""));
    }


void CNcdSubscriptionOperation::Progress( CNcdBaseOperation& /*aOperation*/ )
    {
    DLTRACEIN((""));
    DLTRACEOUT((""));
    }

void CNcdSubscriptionOperation::QueryReceived( CNcdBaseOperation& /*aOperation*/,
                                CNcdQuery* /*aQuery*/ )
    {
    DLTRACEIN((""));
    DLTRACEOUT((""));
    }

void CNcdSubscriptionOperation::OperationComplete(
    CNcdBaseOperation* aOperation,
    TInt aError )
    {
    DLTRACEIN(("error=%d", aError));

    if ( iSubscriptionOperationType == MNcdSubscriptionOperation::EUnsubscribe )
        {
        SubscriptionOperationComplete( aError );
        }
    else if ( iSubscriptionOperationType ==
        MNcdSubscriptionOperation::ERefreshSubscriptions )
        {
        DLINFO(("Refresh subscription op complete."));
        
        TRAPD( err,
            {
            CNcdSubscriptionOperation* subOp =
                static_cast<CNcdSubscriptionOperation*>( aOperation );
                
            if ( aError != KErrNone )
                {
                DLINFO(("Refresh sub operation failed."));
                iFailedSubOps.AppendL( subOp );
                if ( iCompletionErrorCode == KErrNone )
                    {
                    iCompletionErrorCode = aError;
                    }
                }
            else
                {
                DLINFO(("Refresh sub operation successfull."));
                iCompletedSubOps.AppendL( subOp );
                }
            } ); //TRAPD

        if ( err != KErrNone )
            {
            DLERROR(("Internal subscription operation error"));
            SubscriptionOperationComplete( err );
            }
        else if ( iFailedSubOps.Count() + iCompletedSubOps.Count() ==
            iSubOps.Count() )
            {
            // All sub operations completed, the operation is complete.
            if ( iFailedSubOps.Count() && iCompletedSubOps.Count() )
                {
                // Some suboperations has failed, but not all.
                SubscriptionOperationComplete( KNcdErrorSomeSubscriptionsFailedToUpdate );
                }
            else if ( iFailedSubOps.Count() )
                {
                // All suboperations has failed.
                SubscriptionOperationComplete( iCompletionErrorCode );
                }
            else
                {                
                SubscriptionOperationComplete( KErrNone );
                }
            }
        }

    DLTRACEOUT((""));
    }

void CNcdSubscriptionOperation::ErrorL( MNcdPreminetProtocolError* aData )
    {
    DLTRACEIN((""));

    CleanupDeletePushL( aData );

    // Map error codes to correct enumeration values.
    switch ( aData->Code() ) 
        {
        case 404:
            {
            iError = KNcdErrorNotFound;
            DASSERT( iSubscription );
            if ( iSubscription )
                {
                RemoveSubscriptionL( *iSubscription );
                iSubscription = NULL;
                }                        
            break;
            }
        case 416:
            {
            DLTRACE(("session expired"));
            Cancel();
            
            switch ( iSubscriptionOperationType ) 
                {
                case MNcdSubscriptionOperation::EUnsubscribe:
                    {
                    iProtocol.SessionHandlerL( iSession.Context() ).
                        RemoveSession( *iServerUri, *iNamespace );
                    break;
                    }
                case MNcdSubscriptionOperation::ERefreshSubscriptions:
                    {
                    iProtocol.SessionHandlerL( iSession.Context() ).
                        RemoveSession( iSource->Uri(), iSource->Namespace() );
                    break;
                    }                    
                default:            
                    DASSERT(0);
                    break;
                }
            
            DLINFO(("Start operation from the beginning"));            
            // continue operation asynchronously to prevent problems with parser
            ContinueOperationL();
            break;
            }
        case 426:
            iError = KNcdErrorSubscriptionPaymentAlreadyDone;
            break;
        case 427:
            iError = KNcdErrorSubscriptionNotSubscribed;
            DASSERT( iSubscription );
            if ( iSubscription )
                {
                RemoveSubscriptionL( *iSubscription );
                iSubscription = NULL;
                }  
            break;
        case 428:
            iError = KNcdErrorSubscriptionInvalid;
            DASSERT( iSubscription );
            if ( iSubscription )
                {
                RemoveSubscriptionL( *iSubscription );
                iSubscription = NULL;
                }  
            break;
        case 429:
            iError = KNcdErrorSubscriptionNotEnoughCredits;
            break;
        default:
            iError = KNcdProtocolErrorBase - aData->Code();
            break;
        }

    //CNcdBaseOperation::ErrorL( aData );        

    // Default observer deletes the data.
    CleanupStack::Pop( aData );

    iParser->DefaultObserver().ErrorL( aData );
    
    if( iError != KErrNone )
        {
        SubscriptionOperationComplete( iError );
        }    

    DLTRACEOUT((""));
    }
    
void CNcdSubscriptionOperation::SubscriptionsInternalizeComplete(
    TInt /*aError*/ )
    {
    DLTRACEIN((""));
    
    // call observers
    DLINFO(("Calling observers."));
    for ( TInt i = 0 ; i < iObservers.Count() ; i++ )
        {
        DLINFO(("Calling observer: %d .", i ));
        iObservers[i]->OperationComplete( this, KErrNone );
        }
    }

CNcdSubscriptionOperation* CNcdSubscriptionOperation::CreateSubOperationLC(
    CNcdSubscriptionsSourceIdentifier* aSource,
    CNcdGeneralManager& aGeneralManager,
    CNcdSubscriptionManager& aSubscriptionManager,    
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler& aRemoveHandler,
    MCatalogsSession& aSession )
    {
    CNcdSubscriptionOperation* self =
        new( ELeave ) CNcdSubscriptionOperation( 
            MNcdSubscriptionOperation::ERefreshSubscriptions,
            aGeneralManager,
            aSubscriptionManager,
            aHttpSession,
            aRemoveHandler,
            aSession );
    CleanupClosePushL( *self );
    self->ConstructL( aSource, KNullDesC, KNullDesC, KNullDesC, KNullDesC );
    return self;
    }

void CNcdSubscriptionOperation::RunRefreshOperationL()
    {
    DLTRACEIN((""));

    if ( iSource )
        {
        DLTRACE(( "Sub operation" ));
        // This is a sub operation. Send request to the given source.

        CNcdRequestManageSubscriptions* request =
            NcdRequestGenerator::CreateManageSubscriptionsRequestLC();

        // Set namespace from the source.
        request->SetNamespaceL( iSource->Namespace() );

        MCatalogsContext& context( iSession.Context() );

        HBufC8* data =
            iProtocol.ProcessPreminetRequestL(
                context,
                *request,
                iSource->Uri() );

        CleanupStack::PopAndDestroy( request );
        CleanupStack::PushL( data );

        // create transaction
        iGeneralManager.HttpUtils().CreateTransactionL(
            iHttpSession,
            iTransaction,
            iSource->Uri(),
            *this,
            *data,
            iSource->Namespace(),
            MCatalogsAccessPointManager::EBrowse,
            iClientUid );
        
        CleanupStack::PopAndDestroy( data );
        
        // create parser  
        delete iParser;
        iParser = NULL;
        iParser = iProtocol.CreateParserL( context, iSource->Uri() );

        // Set observers
        MNcdParserObserverBundle& observers = iParser->Observers();
        observers.SetParserObserver( this );
        observers.SetSubscriptionObserver( this );
        observers.SetInformationObserver( this );
        observers.SetErrorObserver( this );

        iParser->BeginAsyncL();

        // start transaction
        User::LeaveIfError( iTransaction->Start() );
        }
    else
        {
        DLTRACE(( "Main operation" ));
        // This is the main operation. Sub operations are started
        // from here.

        // Get sources for sub operations.
        RPointerArray<CNcdSubscriptionsSourceIdentifier> sources =
            iSubscriptionManager.SubscriptionsSourcesL(
                iClientUid );
        CleanupResetAndDestroyPushL( sources );
        
        TInt count = sources.Count();
        
        DLTRACE(( "Source count: %d", count ));
        
        if ( count == 0 )
            {
            // No sources found.
            CleanupStack::PopAndDestroy( &sources );
            
            SubscriptionOperationComplete( KErrNone );

            DLTRACEOUT((""));
            return;
            }

        // Start all sub operations.
        for ( TInt i = 0; i < count; i++ )
            {
            DLTRACE(( _L("Source URI: %S"), &sources[0]->Uri() ));
            DLTRACE(( _L("Source namespace: %S"), &sources[0]->Namespace() ));

            const MCatalogsContext& context( iSession.Context() );
            MNcdServerDetails& serverDetails =
                iConfigurationManager.ServerDetailsL(
                    context,
                    sources[0]->Uri(),
                    sources[0]->Namespace() );

            // Capabilities are checked only if it is required
            if ( sources[0]->RequiresCapabilityCheck() &&
                     serverDetails.IsCapabilitySupported(
                         NcdCapabilities::KSubscriptions )
                 || !sources[0]->RequiresCapabilityCheck() )
                {
                DLTRACE(( "This source supports subscriptions!" ));
                DLINFO(( "Capability check was required: %d",
                         sources[0]->RequiresCapabilityCheck() ));

                CNcdSubscriptionOperation* subOp =
                    CNcdSubscriptionOperation::CreateSubOperationLC(
                        sources[0],
                        iGeneralManager,
                        iSubscriptionManager,
                        iHttpSession,
                        *iRemoveHandler,
                        iSession );
                // CreateSubOperationLC takes the ownership of the first
                // object in the sources array. Remove it from sources.
                sources.Remove( 0 );
                subOp->AddObserverL( this );
                User::LeaveIfError( subOp->Start() );
                DLTRACE(( "Sub operation started!" ));
                iSubOps.AppendL( subOp );
                CleanupStack::Pop( subOp );
                }
            else
                {
                DLTRACE(( "This source does not support subscriptions!" ));

                // Do not use this source.
                sources.Remove( 0 );
                }
            }

        CleanupStack::PopAndDestroy( &sources );

        // Any sub operations started
        if ( iSubOps.Count() == 0 )
            {
            DLTRACE(( "None of the sources did not support subscriptions!" ));
            SubscriptionOperationComplete( KErrNone );
            }
        }

    DLTRACEOUT((""));
    }
    
    
void CNcdSubscriptionOperation::RunUnsubscribeOperationL()
    {
    DLTRACEIN((""));

    CNcdRequestManageSubscriptions* request =
        NcdRequestGenerator::CreateManageSubscriptionsRequestLC();

    request->SetNamespaceL( *iNamespace );
    request->AddSubscriptionL(
        *iEntityId,
        *iPurchaseOptionId,
        EUnsubscribe );

    MCatalogsContext& context( iSession.Context() );

    HBufC8* data =
        iProtocol.ProcessPreminetRequestL(
            context,
            *request,
            *iServerUri );

    CleanupStack::PopAndDestroy( request );
    CleanupStack::PushL( data );

    // create transaction
    iGeneralManager.HttpUtils().CreateTransactionL(
        iHttpSession,
        iTransaction,
        *iServerUri,
        *this,
        *data,
        *iNamespace,
        MCatalogsAccessPointManager::EBrowse,
        iClientUid );
        
    CleanupStack::PopAndDestroy( data );
            
    // create parser  
    delete iParser;
    iParser = NULL;
    iParser = iProtocol.CreateParserL( context, *iServerUri );

    // Set observers
    MNcdParserObserverBundle& observers = iParser->Observers();
    observers.SetParserObserver( this );
    observers.SetSubscriptionObserver( this );
    observers.SetInformationObserver( this );
    observers.SetErrorObserver( this );

    iParser->BeginAsyncL();

    // start transaction
    User::LeaveIfError( iTransaction->Start() );

    DLTRACEOUT((""));
    }

TInt CNcdSubscriptionOperation::SubscriptionOperationComplete( TInt aError )
    {
    DLTRACEIN((""));

    // iSubscriptionOperationState is used in RunOperation to determine
    // whether this operation has already completed (this function has been
    // called).
    // We should complete incoming message in RunOperation if it was not
    // available in this function.
    
    // Notice that we cannot automatically set iOperationState to be
    // EStateComplete as iOperationState is used when we receive
    // continue message. State EStateComplete would produce an error in that
    // case because it means that we are not waiting any messages anymore.
    // This is why we have to use two state variables to inform of completion.
    
    iSubscriptionOperationState = EComplete;

    // Also given error code has to be stored if iPendingMessage is not
    // currently available    
    iCompletionErrorCode = aError; 


    if ( aError != KErrNone )
        {
        DLTRACE((_L("->Operation failed")));
        // Operation failed, send error message.
        Cancel();
        if ( iPendingMessage )
            {
            CompletePendingMessage();
            }
        // call observers
        for ( TInt i = 0 ; i < iObservers.Count() ; i++ )
            {
            iObservers[i]->OperationComplete( this, aError );
            }
        }
    else
        {
        DLTRACE((_L("->Operation complete")));
        // Operation has completed.

        if ( iPendingMessage )
            {
            TInt err = CompletePendingMessage();
            if ( err != KErrNone )
                {
                // call observers
                for ( TInt i = 0 ; i < iObservers.Count() ; i++ )
                    {
                    iObservers[i]->OperationComplete( this, aError );
                    }
                return err;
                }
            }
        
        TInt err = KErrNone;
        if ( iSource && iSubscriptionOperationType ==
            MNcdSubscriptionOperation::ERefreshSubscriptions )
            {
            TRAP_IGNORE(
                {
                iSubscriptionManager.InternalizeSubscriptionsFromServerL(
                    iClientUid,
                    iSource->Uri(),
                    iServersSubscriptions,
                    &iSession.Context(),
                    this );
                } );
                
            // Do not call operation complete here if this is
            // suboperation. Instead do it after internalize in
            // SubscriptionsInternalizeComplete()
            DLTRACEOUT((""));
            return KErrNone;
            }
            
        // call observers
        DLINFO(("Calling observers."));
        for ( TInt i = 0 ; i < iObservers.Count() ; i++ )
            {
            DLINFO(("Calling observer: %d .", i ));
            iObservers[i]->OperationComplete( this, err );
            }
        }

    DLTRACEOUT((""));
    return KErrNone;
    }

TInt CNcdSubscriptionOperation::CompletePendingMessage()
    {
    DASSERT( iPendingMessage );
    DLTRACEIN(( "Pending message: %X", iPendingMessage ));
    
    TInt err( 0 );
    if ( iCompletionErrorCode != KErrNone )
        {
        err = CNcdBaseOperation::CompleteMessage(
                       iPendingMessage,
                       ENCDOperationMessageCompletionError,
                       iCompletionErrorCode );
        }
    else
        {
        err = CNcdBaseOperation::CompleteMessage( iPendingMessage,
            ENCDOperationMessageCompletionComplete,
            iProgress,
            KErrNone );
        }            
    // iPendingMessage = NULL set by CompleteMessage
      
    // Is this ok?
    iOperationState = EStateComplete;
    
    return err;
    }
    
void CNcdSubscriptionOperation::RemoveSubscriptionL(
    const CNcdSubscription& aSubscription ) 
    {
    CNcdNodeIdentifier* nodeId = CNcdNodeIdentifier::NewLC(
        aSubscription.ParentGroup().Namespace(),
        aSubscription.ParentGroup().EntityId(), iClientUid );
    iSubscriptionManager.RemoveSubscriptionL(
        *nodeId, aSubscription.PurchaseOptionId() );
    CleanupStack::PopAndDestroy( nodeId );
    }
    


void CNcdSubscriptionOperation::ChangeToPreviousStateL()
    {
    DLTRACEIN((""));
    // Nothing to do. This is needed for query handling, ResendRequestL uses this
    }
    
void CNcdSubscriptionOperation::CancelSuboperations()
    {
    DLTRACEIN((""));
    
    for ( TInt i = 0; i < iSubOps.Count(); i++ )
        {
        CNcdSubscriptionOperation* operation = iSubOps[ i ];
        if ( iCompletedSubOps.Find( operation ) == KErrNotFound &&
             iFailedSubOps.Find( operation ) == KErrNotFound )
            {
            DLINFO(("operation not completed yet, cancel it"));
            operation->Cancel();
            }
        }
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdSubscriptionOperation::CNcdSubscriptionOperation(
    MNcdSubscriptionOperation::TType aSubscriptionOperationType,
    CNcdGeneralManager& aGeneralManager,
    CNcdSubscriptionManager& aSubscriptionManager,
    MCatalogsHttpSession& aHttpSession,
    MNcdOperationRemoveHandler& aRemoveHandler,
    MCatalogsSession& aSession )
    :
    CNcdBaseOperation( aGeneralManager, &aRemoveHandler, ESubscriptionOperation, aSession ),
    iSubscriptionOperationState( EBegin ),
    iSubscriptionOperationType( aSubscriptionOperationType ),
    iAccessPointManager( aGeneralManager.AccessPointManager() ),
    iSubscriptionManager( aSubscriptionManager ),
    iNodeManager( aGeneralManager.NodeManager() ),
    iConfigurationManager( aGeneralManager.ConfigurationManager() ),
    iHttpSession( aHttpSession ),
    iProtocol( aGeneralManager.ProtocolManager() ),
    iCompletionErrorCode( KErrNone )
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdSubscriptionOperation::ConstructL(
    CNcdSubscriptionsSourceIdentifier* aSource,
    const TDesC& aPurchaseOptionId,
    const TDesC& aEntityId,
    const TDesC& aNamespace,
    const TDesC& aServerUri )

    {
    DLTRACEIN(("aSource=%08x", aSource));

    // Call ConstructL for the base class
    CNcdBaseOperation::ConstructL();

    iSource = aSource;
    iPurchaseOptionId = aPurchaseOptionId.AllocL();
    iEntityId = aEntityId.AllocL();
    iNamespace = aNamespace.AllocL();
    iServerUri = aServerUri.AllocL();
    
    iClientUid = iSession.Context().FamilyId();

    if ( iPurchaseOptionId->Length() > 0 &&
        iEntityId->Length() > 0 &&
        iNamespace->Length() > 0 )
        {
        iSubscription = &iSubscriptionManager.SubscriptionL(
            *iEntityId,
            *iNamespace,
            *iPurchaseOptionId,
            iClientUid );
        }

    DLTRACEOUT((""));
    }
