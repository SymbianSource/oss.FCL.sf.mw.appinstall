/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ncdpurchaseoperationimpl.h"

#include <badesca.h>
#include <s32mem.h>
#include <exterror.h> // for KErrGsmSMSShortMessageTransferRejected

#include "ncdoperationfunctionids.h"
#include "catalogsbasemessage.h"
#include "catalogssmssession.h"
#include "catalogshttpincludes.h"
#include "ncdrequestgenerator.h"
#include "ncdrequestbase.h"
#include "ncdrequestpurchase.h"
#include "ncdrequestconfigurationdata.h"
#include "ncdprotocol.h"
#include "ncdparser.h"
#include "ncdnodemanager.h"
#include "ncdnodeidentifier.h"
#include "ncdnodeimpl.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodecontentinfoimpl.h"
#include "ncdparserfactory.h"
#include "ncdpaymentmethod.h"
#include "ncd_pp_purchaseinformationimpl.h"
#include "ncd_pp_purchaseprocessedimpl.h"
#include "ncd_pp_information.h"
#include "ncd_pp_error.h"
#include "ncd_cp_queryresponse.h"
#include "ncd_cp_query.h"
#include "ncd_cp_queryimpl.h"
#include "ncd_cp_queryelement.h"
#include "ncd_cp_detail.h"
#include "ncdstring.h"

#include "ncdpurchaseoptionimpl.h"

#include "ncdpurchasehistorydbimpl.h"
#include "ncdutils.h"
#include "ncddelay.h"
#include "ncdnodelink.h"
#include "catalogsconstants.h"
#include "catalogsutils.h"
#include "ncd_pp_download.h"
#include "ncd_pp_descriptor.h"
#include "ncd_pp_rights.h"
#include "ncdqueryimpl.h"
#include "ncdqueryselectionitemimpl.h"
#include "ncdnodeiconimpl.h"
#include "ncdoperationremovehandler.h"
#include "ncdnodedownloadimpl.h"
#include "ncddownloadinfo.h"

#include "ncdsubscriptionmanagerimpl.h"
#include "ncderrors.h"
#include "ncdsessionhandler.h"
#include "ncdprotocoldefaultobserver.h"
#include "ncdnodedependencyimpl.h"
#include "ncdnodeupgradeimpl.h"

#include "ncdproviderutils.h"
#include "ncdengineconfiguration.h"
#include "catalogscontext.h"
#include "ncdsearchnodefolder.h"
#include "ncdhttputils.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// Resend after timer is not needed if iResendAfter is -1
const TInt KResendAfterDisabled = -1;

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdPurchaseOperationImpl* CNcdPurchaseOperationImpl::NewL(
    const TDesC& aNameSpace,
    const TDesC& aNodeId,
    const TUid& aClientUid,
    const TDesC& aPurchaseOptionId,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MCatalogsSmsSession& aSmsSession,
    CNcdSubscriptionManager& aSubscriptionManager,
    MNcdOperationRemoveHandler* aRemoveHandler,
    MCatalogsSession& aSession )
    {
    CNcdPurchaseOperationImpl* self = 
        CNcdPurchaseOperationImpl::NewLC(
            aNameSpace,
            aNodeId,
            aClientUid,
            aPurchaseOptionId,
            aGeneralManager,
            aHttpSession,
            aSmsSession,
            aSubscriptionManager,
            aRemoveHandler,
            aSession );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdPurchaseOperationImpl* CNcdPurchaseOperationImpl::NewLC(
    const TDesC& aNameSpace,
    const TDesC& aNodeId,
    const TUid& aClientUid,
    const TDesC& aPurchaseOptionId,
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MCatalogsSmsSession& aSmsSession,
    CNcdSubscriptionManager& aSubscriptionManager,
    MNcdOperationRemoveHandler* aRemoveHandler,
    MCatalogsSession& aSession )
    {
    CNcdPurchaseOperationImpl* self = 
        new( ELeave ) CNcdPurchaseOperationImpl(
            aGeneralManager,
            aHttpSession,
            aSmsSession,
            aSubscriptionManager,
            aRemoveHandler,
            aSession );
    CleanupClosePushL( *self );
    self->ConstructL( aNameSpace, aNodeId, aClientUid, aPurchaseOptionId );
    return self;
    }
    

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdPurchaseOperationImpl::CNcdPurchaseOperationImpl(
    CNcdGeneralManager& aGeneralManager,
    MCatalogsHttpSession& aHttpSession,
    MCatalogsSmsSession& aSmsSession,
    CNcdSubscriptionManager& aSubscriptionManager,
    MNcdOperationRemoveHandler* aRemoveHandler,
    MCatalogsSession& aSession )
    : CNcdBaseOperation( aGeneralManager, aRemoveHandler, EPurchaseOperation,
        aSession ),
      iAccessPointManager( aGeneralManager.AccessPointManager() ),
      iPurchaseHistory( &aGeneralManager.PurchaseHistory() ),
      iHttpSession( aHttpSession ),
      iSmsSession( &aSmsSession ),
      iProtocol( aGeneralManager.ProtocolManager() ),
      iSubscriptionManager( aSubscriptionManager ),
      iLocked( EFalse )
    {
    iPurchaseOperationState = EBegin;
    
    }
    

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CNcdPurchaseOperationImpl::ConstructL(
    const TDesC& aNameSpace,
    const TDesC& aNodeId,
    const TUid& aClientUid,
    const TDesC& aPurchaseOptionId )
    {
    DLTRACEIN((""));

    CNcdBaseOperation::ConstructL();

    iNodeIdentifier = CNcdNodeIdentifier::NewL( aNameSpace, aNodeId, aClientUid );

    // Get the node where the purchase operation was started from
    iNode = &iNodeManager->NodeL( *iNodeIdentifier );

    // Get the selected purchase option
    RPointerArray<CNcdPurchaseOptionImpl> purchaseOptions = 
        iNode->NodeMetaDataL().PurchaseOptions();

    for( TInt i = 0; i < purchaseOptions.Count(); i++ )
        {
        if( purchaseOptions[i]->Id().CompareF( aPurchaseOptionId ) == 0 )
            {
            iSelectedPurchaseOption = purchaseOptions[i];
            }
        }
    
    // Purchase option is needed so leave if not found    
    if ( !iSelectedPurchaseOption )
        {
        User::Leave( KErrNotFound );
        }
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CNcdPurchaseOperationImpl::~CNcdPurchaseOperationImpl()
    {
    delete iPurchaseOptionId;
    delete iNodeIdentifier;
    delete iParser;
    if ( iHttpTransaction ) 
        {
        // Cancel also releases the operation
        iHttpTransaction->Cancel();        
        }
    
    delete iSmsRegistrationRequest;
    delete iPurchaseInformationData;
    delete iPurchaseProcessedData;
    if( iPurchaseQuery )
        {
        iPurchaseQuery->InternalRelease();
        }
    delete iRedirectUri;
    delete iDelay;
    iPendingSmsOperations.Reset();
    }

// ---------------------------------------------------------------------------
// Node Id getter
// ---------------------------------------------------------------------------
//
const CNcdNodeIdentifier& CNcdPurchaseOperationImpl::NodeIdentifier() const
    {
    return *iNodeIdentifier;
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdPurchaseOperationImpl::Cancel()
    {
    DLTRACEIN( ( "" ) );
    if ( iHttpTransaction )
        {
        iHttpTransaction->Cancel();
        iHttpTransaction = NULL;
        }
    
    if ( iParser )
        {
        iParser->CancelParsing();        
        }
    
    DeletePtr( iDelay );
    
    TInt i = iPendingSmsOperations.Count();
    while( i-- )
        {
        iPendingSmsOperations[i]->Cancel();
        // The previous cancel will result with current implementation
        // in HandleSmsEvent() call. Although this happens release
        // and removal is done here to be sure that they are done.
        // (This way this function is not dependant whether the
        // HandleSmsEvent callback occurs or not)                    
	    iPendingSmsOperations[i]->Release();
        iPendingSmsOperations.Remove( i );
        }
    	
    }
    
// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdPurchaseOperationImpl::HandleHttpEventL( 
        MCatalogsHttpOperation& aOperation, 
        TCatalogsHttpEvent aEvent )
    {
    DLTRACEIN((""));

    DASSERT( aOperation.OperationType() == ECatalogsHttpTransaction );
    
    TInt err = KErrNone;
    switch( aEvent.iOperationState )
        {
            
        case ECatalogsHttpOpInProgress:
            {
            // Don't bother to send empty bodies because this event 
            // is sent also for received headers etc.
            if ( aOperation.Body().Length() )
                {                
                // Send received data to the parser
                TRAP(err, iParser->ParseL( aOperation.Body() ));
                }

            break;
            }

        case ECatalogsHttpOpCompleted:
            {
            TRAP( err, iParser->EndL() );
            break;
            }
        }

    if( err != KErrNone )
        {
        iError = err;
        iPurchaseOperationState = EFailed;
        Cancel();
        RunOperation();
        }
        
    DLTRACEOUT((""));
    }
    



// ---------------------------------------------------------------------------
// From class CNcdBaseOperation.
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CNcdPurchaseOperationImpl::RunOperation()
    {
    DLTRACEIN((""));
    
    if ( !iPendingMessage )
        {
        DLTRACEOUT(("No pending message -> return"));
        return KErrNone;
        }

    DLTRACE(("Purchase operation state: %d", iPurchaseOperationState));
    
    if ( iLocked ) 
        {
        DLINFO(("Operation locked"));
        return KErrNone;
        }
    
    TRAPD( err, DoRunOperationL() );
     
     if ( err != KErrNone )
        {
        DLTRACE(("error: %d", err));
        Cancel();
        iPurchaseOperationState = EFailed;
        iError = err;
        if ( iPendingMessage )
            {
            // error ignored because operation already failed
            CNcdBaseOperation::CompleteMessage( iPendingMessage,
                ENCDOperationMessageCompletionError, err );
            }
        }
    DLTRACEOUT((""));
    return err;
    }


void CNcdPurchaseOperationImpl::DoRunOperationL()
    {
    DLTRACEIN((""));
    switch ( iPurchaseOperationState )
        {
        case EBegin:
            {
            DLTRACE( _L( "->EBegin" ) );
            
            // At first check if there is enough disk space for the downloadable
            // content. Stop the purchase process if this is not the case.
            TInt contentSize = 0;
            TRAP_IGNORE( contentSize = iNode->NodeMetaDataL().ContentInfoL().TotalContentSize() );          
            DLINFO((("contentSize=%d"), contentSize ));
            if ( contentSize > 0 ) 
                {                
                HBufC* clientDataPath = CNcdProviderUtils::EngineConfig().ClientDataPathLC( 
                    iGeneralManager.FamilyName(), EFalse );    
                DLINFO(( _L("Client data path: %S"), clientDataPath )); 
                WouldDiskSpaceGoBelowCriticalLevelL(
                    *clientDataPath, CNcdProviderUtils::FileSession(), contentSize );
                CleanupStack::PopAndDestroy( clientDataPath );                
                }
            
            // Skip purchase process for free items that doesn't implicitely
            // require going through it
            // Notice that we don't support purchasing of a subscription
            // that is free and does not require purchase process. Purchase
            // process is used as no downloadinfo is received!!
            if( iSelectedPurchaseOption->IsFree() && 
                !iSelectedPurchaseOption->RequirePurchaseProcess() &&
                iSelectedPurchaseOption->DownloadInfoCount() > 0 ) 
                {
                iPurchaseOperationState = EDownloadDetailsReceived;
                
                CNcdBaseOperation::CompleteMessage( 
                    iPendingMessage, 
                    ENCDOperationMessageCompletionProgress,
                    iProgress,
                    KErrNone );
                }
            else
                {

                // Create purchase request and send it. Next callback will be
                // HandleHttpEventL or HandleHttpError.
                HBufC8* purchaseRequest = CreatePurchaseRequestLC(
                    EPurchaseRequest,
                    iNode->NodeLinkL().ServerUri() );

                SendRequestL( iNode->NodeLinkL().ServerUri(), *purchaseRequest );
                CleanupStack::PopAndDestroy( purchaseRequest );

                iPurchaseOperationState = EPurchaseRequestSent;
                }
            DLTRACE(( "EBegin done" ));
            break;
            }

        case EPurchaseInformationReceived:
            {
            DLTRACE( _L( "->EPurchaseInformationReceived" ) );
            
            if( !iPurchaseInformationData )
                {
                User::Leave( KNcdErrorNoPurchaseInformationReceived );
                }

            // Handle redirection if server requested for it.
            if( iPurchaseInformationData->Uri() != KNullDesC )
                {
                // store redirect uri
                delete iRedirectUri;
                iRedirectUri = NULL;
                iRedirectUri = iPurchaseInformationData->Uri().AllocL();
                
                HBufC8* purchaseRequest;

                // Redirection with new session
                if( iPurchaseInformationData->InitiateSession() )
                    {
                    // remove session if there is one
                    iProtocol.SessionHandlerL( iSession.Context() ).RemoveSession(
                    iPurchaseInformationData->Uri(), iNodeIdentifier->NodeNameSpace() );
                    purchaseRequest = 
                        CreatePurchaseRequestLC( 
                            EPurchaseRequest,
                            iPurchaseInformationData->Uri() );
                    }
                
                // Redirection without new session
                else
                    {
                    purchaseRequest = 
                        CreatePurchaseRequestLC( 
                            EPurchaseRequest,
                            iNode->NodeLinkL().ServerUri() ); 
                    }
                
                SendRequestL( 
                    *iRedirectUri, *purchaseRequest );
                CleanupStack::PopAndDestroy( purchaseRequest );
                iPurchaseOperationState = EPurchaseRequestResent;
                }
            else if( iPurchaseInformationData->Disclaimer() )
                {
                DLTRACE(("Sending purchase disclaimer"));
                if ( iPurchaseQuery )
                    {
                    iPurchaseQuery->InternalRelease();
                    iPurchaseQuery = NULL;
                    }
                iPurchaseQuery = 
                    CNcdQuery::NewL( *iPurchaseInformationData->Disclaimer() );
                CNcdBaseOperation::QueryReceivedL( iPurchaseQuery );
                
                iPurchaseOperationState = EDisclaimerQueried;
                }
            else
                {
                iPurchaseOperationState = EDisclaimerQueried;

                // Send progress info to the proxy
                CNcdBaseOperation::CompleteMessage( iPendingMessage,
                         ENCDOperationMessageCompletionProgress,
                         iProgress,
                         KErrNone );
                }
            DLTRACE(( "EPurchaseInformationReceived done" ));
            break;
            }
            
        case EDisclaimerQueried:
            {
            DLTRACE(( "EDisclaimerQueried" ));
                      
            // purchase option is free but payment methods exist
            // -> there may be querys that need showing
            if( iSelectedPurchaseOption->IsFree() &&
                iPurchaseInformationData->PaymentCount() == 1 )
                {
                // automatically select the purchase option
                iSelectedPaymentMethod = &iPurchaseInformationData->PaymentL( 0 );
                // continue operation so that possible querys get shown
                iPurchaseOperationState = EPaymentMethodsQueried;
                
                // Send progress info to the proxy
                CNcdBaseOperation::CompleteMessage( iPendingMessage,
                         ENCDOperationMessageCompletionProgress,
                         iProgress,
                         KErrNone );
                }
            // purchase option is free but no payment methods exist
            else if( iSelectedPurchaseOption->IsFree() &&
                iPurchaseInformationData->PaymentCount() < 1 )
                {
                // send purchase confirmation next
                iPurchaseOperationState = EPaymentProcessed;
                // Send progress info to the proxy
                CNcdBaseOperation::CompleteMessage( iPendingMessage,
                         ENCDOperationMessageCompletionProgress,
                         iProgress,
                         KErrNone );
                }
            // Send received payment methods to the proxy
            else
                {
                // Set received price text to the purchase option.
                iSelectedPurchaseOption->SetPriceTextL( 
                    iPurchaseInformationData->EntityL( 0 ).PriceText() );

                // Now that the purchase option has been updated (new price)
                // on the server side, it has to be reinternalized into
                // proxy side. This is done in
                // CNcdPurchaseOperationProxy::QueryReceivedCallback.
				//
                // It might be nicer to pass the updated price
                // some way in the query instead of updating the
                // purchase option. This way the same updated price
                // would not (in update situations) be in two places
                // in purchase history (final price and the price of
                // purchase option).


                RPointerArray<CNcdString> paymentMethodNames( KListGranularity );
                CleanupResetAndDestroyPushL( paymentMethodNames );

                CDesC8ArrayFlat* paymentMethodTypes =
                    new ( ELeave ) CDesC8ArrayFlat( KListGranularity );

                CleanupStack::PushL( paymentMethodTypes );

                HBufC8* paymentMethodType;

                TInt count = iPurchaseInformationData->PaymentCount();
                
                // must have at least one payment method
                if( count < 1 )
                    {
                    DLERROR(("No payment methods!"));
                    DASSERT(0);
                    User::Leave( KNcdErrorNoPurchaseInformation );
                    }
                
                for( TInt i = 0; i < count; i++ )
                    {
                    CNcdString* paymentMethodName = CNcdString::NewLC( 
                        iPurchaseInformationData->PaymentL( i ).Name() );
                    paymentMethodNames.AppendL( paymentMethodName );
                    CleanupStack::Pop( paymentMethodName );

                    paymentMethodType = IntToDes8LC( iPurchaseInformationData->PaymentL( i ).Method() );
                    paymentMethodTypes->AppendL( *paymentMethodType );
                    CleanupStack::PopAndDestroy( paymentMethodType );
                    }

                
                if ( iPurchaseQuery )
                    {
                    iPurchaseQuery->InternalRelease();
                    iPurchaseQuery = NULL;
                    }
                iPurchaseQuery = 
                    CNcdQuery::NewL( paymentMethodNames, *paymentMethodTypes );
                CNcdBaseOperation::QueryReceivedL( iPurchaseQuery );
                
                CleanupStack::PopAndDestroy( paymentMethodTypes );
                CleanupStack::PopAndDestroy( &paymentMethodNames );
                iPurchaseOperationState = EPaymentMethodsQueried;

                }
            DLTRACE(( "EDisclaimerQueried done" ));
            break;
            }

        case EPaymentMethodsQueried:
            {
            DLTRACE( _L( "->EPaymentMethodsQueried" ) );
            
            // Query exists for the selected payment method. 
            if( iSelectedPaymentMethod->QueryId() != KNullDesC() )
                {
                if ( iPurchaseQuery )
                    {
                    iPurchaseQuery->InternalRelease();
                    iPurchaseQuery = NULL;
                    }
                
                TBool secureConnection = EFalse;
                if ( iRedirectUri )
                    {
                    secureConnection = IsHttpsUri( *iRedirectUri );
                    }
                else
                    {
                    secureConnection = IsHttpsUri( iNode->NodeLinkL().ServerUri() );
                    }
                    
                iPurchaseQuery = 
                    CNcdQuery::NewL( CNcdBaseOperation::QueryEntityL( 
                        iSelectedPaymentMethod->QueryId() ), secureConnection );
                        
                CNcdBaseOperation::QueryReceivedL( iPurchaseQuery );
                
                iPurchaseOperationState = EPaymentInformationQueried;
                }

            // No query available. Continue purchase process.
            else
                {
                iPurchaseOperationState = EPaymentInformationQueried;

                // Send progress info to the proxy
                CNcdBaseOperation::CompleteMessage( iPendingMessage, 
                         ENCDOperationMessageCompletionProgress,
                         iProgress,
                         KErrNone );
                }
            DLTRACE(("EPaymentMethodsQueried done"));
            break;
            }

        case EPaymentInformationQueried:
            {
            DLTRACE(("EPaymentInformationQueried"));
            // Send payment SMSs if SMS payment method was selected.
            if( iSelectedPaymentMethod->Method() == 
                MNcdPaymentMethod::EPaymentSms
                && iSelectedPaymentMethod->SmsDetailsCount() > 0 )
                {
                // Sms payments are processed if there is something to process.
                // If sms details were not given, then this operation should be
                // continued in the else clause below as in normal case.
                ProcessSmsPaymentL();
                }
            else
                {
                iPurchaseOperationState = EPaymentProcessed;

                // Send progress info to the proxy
                CNcdBaseOperation::CompleteMessage( iPendingMessage, 
                         ENCDOperationMessageCompletionProgress,
                         iProgress,
                         KErrNone );
                }
            DLTRACE(("EPaymentInformationQueried"));
            break;
            }

        case EPaymentProcessed:
            {
            DLTRACE( _L( "->EPaymentProcessed" ) );
            
            // Create purchase confirmation and send it.
            
            // Handle redirection if server requested for it.
            if( iPurchaseInformationData->Uri() != KNullDesC )
                {
                DLTRACE(( "Redirection." ));
                delete iRedirectUri;
                iRedirectUri = NULL;
                iRedirectUri = iPurchaseInformationData->Uri().AllocL();
                
                HBufC8* purchaseRequest;

                // Redirection with new session
                if( iPurchaseInformationData->InitiateSession() )
                    {
                    DLTRACE(( "Redirection with a new session." ));
                    // remove session if there is one
                    iProtocol.SessionHandlerL( iSession.Context() ).RemoveSession(
                    iPurchaseInformationData->Uri(), iNodeIdentifier->NodeNameSpace() );
                    purchaseRequest = 
                        CreatePurchaseRequestLC( 
                            EPurchaseConfirmation,
                            iPurchaseInformationData->Uri() );
                    }
                
                // Redirection without new session
                else
                    {
                    DLTRACE(( "Redirection without a new session." ));
                    purchaseRequest = 
                        CreatePurchaseRequestLC( 
                            EPurchaseConfirmation,
                            iNode->NodeLinkL().ServerUri() ); 
                    }
                SendRequestL( iPurchaseInformationData->Uri(), *purchaseRequest );
                CleanupStack::PopAndDestroy( purchaseRequest );
                }
            // previously redirected
            else if ( iRedirectUri )
                {
                DLTRACE(( "Previously redirected" ));
                HBufC8* purchaseRequest = 
                    CreatePurchaseRequestLC( EPurchaseConfirmation, *iRedirectUri );
                SendRequestL( *iRedirectUri, *purchaseRequest );
                CleanupStack::PopAndDestroy( purchaseRequest );
                }
            // Otherwise use original server URI.
            else
                {
                DLTRACE(( "No redirection, using original server URI." ));
                HBufC8* purchaseRequest = 
                    CreatePurchaseRequestLC( EPurchaseConfirmation, iNode->NodeLinkL().ServerUri() );
                SendRequestL( iNode->NodeLinkL().ServerUri(), *purchaseRequest );
                CleanupStack::PopAndDestroy( purchaseRequest );
                }

            iPurchaseOperationState = EPurchaseConfirmationSent;
            DLTRACE(( "EPaymentProcessed done" ));
            break;
            }

        case EDownloadDetailsReceived:
            {
            DLTRACE( _L( "->EDownloadDetailsReceived" ) );

            MNcdPurchaseOption::TType selectedOptionType = 
                iSelectedPurchaseOption->PurchaseOptionType();

            if( selectedOptionType ==
                    MNcdPurchaseOption::EPurchase ||
                selectedOptionType ==
                    MNcdPurchaseOption::ESubscriptionPurchase )
                {
                DLTRACE(("Purchase done normally or with subscription."));
                
                // Update purchase information to the purchase history and notify
                // node to refresh itself.
                UpdatePurchaseHistoryL();
                
                DLTRACE(("Purchase history updated."));
                
                // Update node from the purchase db
                iNodeManager->PurchaseHandlerL( iNode->Identifier() );
                
                DLTRACE(("Node download internalized."));                
                }
                

            iPurchaseOperationState = EPurchaseComplete;
            
            if( iPurchaseProcessedData && iPurchaseProcessedData->Information() )
                {
                if ( iPurchaseQuery )
                    {
                    iPurchaseQuery->InternalRelease();
                    iPurchaseQuery = NULL;
                    }
                iPurchaseQuery = 
                    CNcdQuery::NewL( *iPurchaseProcessedData->Information() );
                CNcdBaseOperation::QueryReceivedL( iPurchaseQuery );
                }
            else 
                {                
                // Send info about operation completion to the proxy
                CNcdBaseOperation::CompleteMessage( iPendingMessage, 
                         ENCDOperationMessageCompletionComplete,
                         iProgress,
                        KErrNone );
                }
            break;
            }
            
        case EPurchaseComplete:
            {
            DLTRACE(("EPurchaseComplete"));
            if ( iPendingMessage )
                {                
                // Send info about operation completion to the proxy
                CNcdBaseOperation::CompleteMessage( iPendingMessage, 
                         ENCDOperationMessageCompletionComplete,
                         iProgress,
                        KErrNone );
                }
            DLTRACE(("EPurchaseComplete done"));
            break;
            }
        
        case EFailed:
            {
            DLTRACE(( "->EFailed" ));
            Cancel();
            if ( iPendingMessage )                                 
                {                
                User::LeaveIfError( CNcdBaseOperation::CompleteMessage(
                    iPendingMessage, ENCDOperationMessageCompletionError, iProgress, iError ) );
                }
            DLTRACE(("EFailed done"));
            break;
            }
        
        default:
            {
            // RunOperation should not be called for other states.
            DASSERT( 0 );
            }

        }     
    }


void CNcdPurchaseOperationImpl::ChangeToPreviousStateL()
    {
    DLTRACEIN((""));
    switch( iPurchaseOperationState )
        {
        case EPurchaseRequestSent:
        case EPurchaseInformationReceived:
            {
            iPurchaseOperationState = EBegin;
            break;
            }
        case EPurchaseRequestResent:
            {
            iPurchaseOperationState = EPurchaseInformationReceived;
            break;
            }
        case EPurchaseConfirmationSent:
            {
            iPurchaseOperationState = EPaymentProcessed;
            break;
            }
        default:
            {
            DLTRACE(("CAN'T GO BACK FROM THIS STATE: %d, ERROR!", iPurchaseOperationState));
            DASSERT(0);
            User::Leave( KErrArgument );
            break;
            }
        }
    }



HBufC8* CNcdPurchaseOperationImpl::CreatePurchaseRequestLC( 
    TPurchaseRequestType aType,
    const TDesC& aServerUri )
    {
    DLTRACEIN((""));
    // Create a purchase request
    CNcdRequestPurchase* req =
        NcdRequestGenerator::CreatePurchaseRequestLC();

    req->SetNamespaceL( iNodeIdentifier->NodeNameSpace() );

    // Fill necessary information to the purchase request.
    switch( aType )
        {
        case EPurchaseRequest:
            {
            DLTRACE(("EPurchaseRequest"));
            req->AddEntityDetailsL( iNode->NodeLinkL().MetaDataIdentifier().NodeId(),
            						iNode->NodeMetaDataL().TimeStamp(),
                                    iSelectedPurchaseOption->Id() );
            DLTRACE(("EPurchaseRequest done"));
            break;
            }
        case EPurchaseConfirmation:
            {
            DLTRACE(("EPurchaseConfirmation"));
            // Set transaction id
            if( iPurchaseInformationData->TransactionId() != KNullDesC() )
            	{
                DLTRACE(("set transaction id"));
	            req->SetTransactionIdL( iPurchaseInformationData->TransactionId() );
            	}
            	
            DLTRACE(("set purchase confirmation"));
            if ( iSelectedPaymentMethod ) 
                {
                if( iPurchaseQuery )
                    {
                    DLTRACE(("adding query id"));
                    req->SetPurchaseConfirmationL( iSelectedPaymentMethod->QueryId(),
            							   iSelectedPaymentMethod->Method() );
                    }
                else
                    {
                    DLTRACE(("not adding query id"));
                    req->SetPurchaseConfirmationL( iSelectedPaymentMethod->Method() );
                    }
                }
            else 
                {
                req->SetPurchaseConfirmationL( KNullDesC, // free item -> no query id
            							   MNcdPaymentMethod::EPaymentDirect );
                }
            
            if( iPurchaseInformationData->EntityCount() > 0 &&
                iPurchaseInformationData->EntityL( 0 ).Ticket() != KNullDesC )
				{
                DLTRACE(("add entity with ticket"));
                req->AddPurchaseConfirmationEntityL( iNode->NodeLinkL().MetaDataIdentifier().NodeId(),
                                                     iNode->NodeMetaDataL().TimeStamp(),
                                                     iSelectedPurchaseOption->Id(),
                                                     iPurchaseInformationData->EntityL( 0 ).Ticket() );
				}
            else
				{
                DLTRACE(("add entity"));
                req->AddPurchaseConfirmationEntityL( iNode->NodeLinkL().MetaDataIdentifier().NodeId(),
                                                     iNode->NodeMetaDataL().TimeStamp(),
                                                     iSelectedPurchaseOption->Id() );
				}
		    DLTRACE(("EPurchaseConfirmation done"));
            break;
            }
        }

    
    AddQueryResponsesL( req );
    
    if( iPurchaseQuery )
        {
        DLTRACE(("Adding query response"));
        req->AddQueryResponseL( CreateResponseL( *iPurchaseQuery ) );
        }
    
    HBufC8* data = iProtocol.ProcessPreminetRequestL( iSession.Context(), *req, aServerUri );
    DLINFO( ( "Request: %S", data  ) );
    CleanupStack::PopAndDestroy( req );
    CleanupStack::PushL( data );
    return data;
    }


void CNcdPurchaseOperationImpl::SendRequestL( const TDesC& aServerUri,
                                              const TDesC8& aData )
{
    DLTRACEIN((""));
	// Create a parser to handle the response
	if( iParser )
	    {
	    iParser->CancelParsing();
	    delete iParser;
	    iParser = NULL;
	    }
	
	iParser = iProtocol.CreateParserL( iSession.Context(), aServerUri );
    
    iParser->Observers().SetParserObserver( this );
    iParser->Observers().SetPurchaseObserver( this );
    iParser->Observers().SetQueryObserver( this );
    iParser->Observers().SetInformationObserver( this );
    iParser->Observers().SetErrorObserver( this );

    iParser->BeginAsyncL();

	
    // Create a HTTP transaction
    iGeneralManager.HttpUtils().CreateTransactionL( 
        iHttpSession,
        iHttpTransaction,
        aServerUri,
        *this,
        aData,
        *iNodeIdentifier,
        MCatalogsAccessPointManager::EPurchase,
        iNodeIdentifier->ClientUid() );
    
    // Start transaction
    User::LeaveIfError( iHttpTransaction->Start() );    
    }


TBool CNcdPurchaseOperationImpl::HandleHttpError( 
    MCatalogsHttpOperation& /* aOperation */, 
    TCatalogsHttpError aError )
    {
    DLTRACEIN(("Error type: %d, code: %d", aError.iType, aError.iError ));
    iError = aError.iError;
    iPurchaseOperationState = EFailed;
    Cancel();
    RunOperation();
    return ETrue;
    }


// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdPurchaseOperationImpl::ParseError( TInt aErrorCode )
    {
    DLTRACEIN(("error:%d", aErrorCode ));
    // Handle only if not handling an error already.
    if( iError == KErrNone )
        {
        iPurchaseOperationState = EFailed;
        iError = aErrorCode;
        Cancel();
        RunOperation();
        }
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CNcdPurchaseOperationImpl::ParseCompleteL( TInt /*aError*/ )
    {
    DLTRACEIN((""));
    
    // Should the error cases be handled here or in the ParseError 
    // callback? Probably not in both.
    if ( iLocked ) 
        {
        // do not complete the message in case the operation is locked
        return;
        }
    

    switch( iPurchaseOperationState )
        {
        case EPurchaseRequestSent:
        case EPurchaseRequestResent:
            {
            iPurchaseOperationState = EPurchaseInformationReceived;
            break;
            }
        case EPurchaseConfirmationSent:
            {
            iPurchaseOperationState = EDownloadDetailsReceived;
            break;
            }
        default:
            {
            DLERROR(( "Unexpected state!" ));
            DASSERT( EFalse );
            break;
            }
        }
    
    // Clear the completed queries from base op, so that we don't send them
    // again in the next request.
    ClearCompletedQueries();
    
    // handle querys if any, calls RunOperation after querys have been handled
    HandleQuerysL();
    }

//
void CNcdPurchaseOperationImpl::InformationL(
    MNcdPreminetProtocolPurchaseInformation* aData )
    {
    delete iPurchaseInformationData;
    iPurchaseInformationData = aData;
    }

void CNcdPurchaseOperationImpl::ProcessedL(
    MNcdPreminetProtocolPurchaseProcessed* aData )
    {
    delete iPurchaseProcessedData;
    iPurchaseProcessedData = aData;
    if ( iPurchaseProcessedData->ResultCode() != 0 )
        {
        DLTRACE(("Error: %d, Stop operation!",
            iPurchaseProcessedData->ResultCode() ));
        iPurchaseOperationState = EFailed;
        iError = iPurchaseProcessedData->ResultCode();
        Cancel();
        RunOperation();
        }
    }
    
void CNcdPurchaseOperationImpl::InformationL(
    MNcdPreminetProtocolInformation* aData ) 
    {
    DLTRACEIN((""));
    
    // handle resend after
    iResendAfter = aData->ResendAfter();
    if ( iResendAfter > KResendAfterDisabled ) 
        {
        DLINFO(( "Resend after %d requested", iResendAfter ));
        switch ( iPurchaseOperationState ) 
            {
            case EPurchaseRequestSent:
                iPurchaseOperationState = EBegin;
                break;
                
            case EPurchaseConfirmationSent:
                iPurchaseOperationState = EPaymentProcessed;
                break;
                
            default:
                // Resend after should not be requested in other cases.
                DASSERT( EFalse );
                break;
            }

        StartResendAfterTimer();
        }
        
    CNcdBaseOperation::InformationL( aData );    
    }


void CNcdPurchaseOperationImpl::ErrorL( MNcdPreminetProtocolError* aData ) 
    {
    DLTRACEIN((""));
    // Map error codes to correct enumeration values.
    
    CleanupDeletePushL( aData );
    
    switch ( aData->Code() ) 
        {
        case 404:
            {
            iError = KNcdErrorNotFound;
            iSubscriptionManager.RemoveSubscriptionL(
                *iNodeIdentifier, *iPurchaseOptionId );
            break;
            }
        case 416:
            {
            DLTRACE(("session expired"));
            Cancel();
            if( iPurchaseInformationData && iPurchaseInformationData->Uri() != KNullDesC )
                {
                iProtocol.SessionHandlerL( iSession.Context() ).RemoveSession( 
                    iPurchaseInformationData->Uri(), iNodeIdentifier->NodeNameSpace() );
                }
            iProtocol.SessionHandlerL( iSession.Context() ).RemoveSession(
                iNode->NodeLinkL().ServerUri(), iNodeIdentifier->NodeNameSpace() );
            DLINFO(("Continue operation from previous state"));
            ChangeToPreviousStateL();
            // continue operation asynchronously to prevent problems with parser
            ContinueOperationL();
            break;
            }
        case 426:
            iError = KNcdErrorSubscriptionPaymentAlreadyDone;
            break;
        case 427:
            iError = KNcdErrorSubscriptionNotSubscribed;
            iSubscriptionManager.RemoveSubscriptionL(
                *iNodeIdentifier, *iPurchaseOptionId );
            break;
        case 428:
            iError = KNcdErrorSubscriptionInvalid;
            iSubscriptionManager.RemoveSubscriptionL(
                *iNodeIdentifier, *iPurchaseOptionId );
            break;
        case 429:
            iError = KNcdErrorSubscriptionNotEnoughCredits;
            break;
        default:
            iError = KNcdProtocolErrorBase - aData->Code();
            break;
        }
    
    CleanupStack::Pop( aData );
    
    // Default observer deletes aData
    iParser->DefaultObserver().ErrorL( aData );
    
    if ( iError != KErrNone )
        {
        iPurchaseOperationState = EFailed;
        Cancel();
        RunOperation();
        }
    }    


void CNcdPurchaseOperationImpl::QueryL( MNcdConfigurationProtocolQuery* aData )
    {
    DLTRACEIN(("query: sem=%d", aData->Semantics()));    
    // check if we need to do something special here,
    // otherwise forward the query to the base operation
    
    if ( aData->Semantics() == MNcdQuery::ESemanticsRegistrationQuery ) 
        {
        DLTRACE(("received sms registration query"));
        CleanupDeletePushL( aData );        
        
        TInt err( KErrNone );
        TRAP( err, 
        for ( TInt i = 0; i < aData->QueryElementCount(); ++i ) 
            {
            const MNcdConfigurationProtocolQueryElement& element = 
                aData->QueryElementL(i);

            if ( element.Type() == MNcdConfigurationProtocolQueryElement::ESms ) 
                {
                DLTRACE(("sms query element"));    
                const MNcdConfigurationProtocolDetail* detail = 
                    element.Detail();
                _LIT(KValueSmsAddress, "smsAddress");
                _LIT(KValueSmsMessage, "smsMessage");

                delete iSmsRegistrationRequest;
                iSmsRegistrationRequest = NULL;

                const RPointerArray<MNcdConfigurationProtocolContent>& contents(
                    detail->Contents() );

                TInt addressKey = KErrNotFound;
                TInt messageKey = KErrNotFound;
                for ( TInt k = 0; k < contents.Count(); ++k ) 
                    {
                    MNcdConfigurationProtocolContent* content = contents[ k ];

                    if ( content->Key() == KValueSmsAddress ) 
                        {
                        DLTRACE(( _L("got sms address: %S"), &content->Value() ));                            
                        addressKey = k;
                        }
                    else if ( content->Key() == KValueSmsMessage ) 
                        {
                        DLTRACE(( _L("got sms message: %S"), &content->Value() ));    
                        messageKey = k;
                        }
                    }

                if ( addressKey != KErrNotFound &&
                     messageKey != KErrNotFound )
                    {
                    iSmsRegistrationRequest = CNcdKeyValuePair::NewL(
                        contents[ addressKey ]->Value(),
                        contents[ messageKey ]->Value() );
                    DLTRACE(("sms address and messager ok"));    
                        
                    // both values are now set, create query
                    CNcdConfigurationProtocolQueryImpl* cpQuery = 
                        CNcdConfigurationProtocolQueryImpl::NewLC();

                    cpQuery->iTitle->SetDataL( aData->Title().Data() );
                    cpQuery->iTitle->SetKeyL( aData->Title().Key() );
                    cpQuery->iBodyText->SetDataL( aData->BodyText().Data() );
                    cpQuery->iBodyText->SetKeyL( aData->BodyText().Key() );
                    cpQuery->iSemantics = MNcdQuery::ESemanticsRegistrationQuery;
                    if ( iPurchaseQuery )
                        {
                        iPurchaseQuery->InternalRelease();
                        iPurchaseQuery = NULL;
                        }
                    iPurchaseQuery  = CNcdQuery::NewL( *cpQuery );

                    CleanupStack::PopAndDestroy( cpQuery );
                    cpQuery = NULL;
                    
                    // Canceling resendAfter timer since there's no point
                    // sending a request until the query has been handled
                    DeletePtr( iDelay );
                    CNcdBaseOperation::QueryReceivedL( iPurchaseQuery );

                    DLTRACE(("created sms registration request ok"));    
                    break;
                    }
                else 
                    {
                    DLTRACE(("incomplete"));    
                    break;
                    }
                }
            }
        ); // TRAPD 
        CleanupStack::PopAndDestroy( aData );
        if( err != KErrNone )
            {
            iPurchaseOperationState = EFailed;
            iError = err;
            Cancel();
            RunOperation();            
            return;
            }
        }
    else
        {
        CNcdBaseOperation::QueryL(aData);
        }
    DLTRACEOUT((""));
    }


void CNcdPurchaseOperationImpl::ProcessSmsPaymentL()
    {
    // Create a SMS for every SMS details received from the server.
    for( TInt i = 0; i < iSelectedPaymentMethod->SmsDetailsCount(); i++ )
	    {
	    const MNcdPreminetProtocolSmsDetails* smsDetails = 
	        &iSelectedPaymentMethod->SmsDetailsL( i );
	    
        // Store ongoing SMS operations to the array. They are removed in the 
        // HandleSmsEvent callback.
        iPendingSmsOperations.AppendL( 
	    	iSmsSession->CreateSmsL( smsDetails->Address(),
	    				             smsDetails->Message(),
                                     this ) );
	    }
    }


TInt CNcdPurchaseOperationImpl::HandleSmsEvent(
	MCatalogsSmsOperation& aOperation, 
	TCatalogsSmsEvent aEvent )
	{
    DLTRACEIN(("sms event: %d", aEvent));
    
    if ( iOperationState == EStateCancelled )
        {
        // If cancellation is originating from this operation let's
        // not call runoperation with state EFailed as it calls the
        // cancel again.
        // Notice also that the runoperation is not called and therefore
        // no observers are called in there. If necessary this has to
        // be done in the cancel.
        // No sms operation removal is done here. It is done in cancel.
        return KErrNone;
        }
    
	switch ( aEvent )
	    {
	    case ECatalogsSmsSent:
	        {
            // Remove completed SMS operation
            RemovePendingSmsOp( aOperation.OperationId() );
        	break;    
	        }
	    case ECatalogsSmsSending:
	        {
	        // Nothing to do
	        break;
	        }
	        
	    // Cancel here is a cancel that is not originating from 
	    // this operation.
	    // This is highly improbable, but the handling is here if anyone
	    // would ever do such an implementation.	    
	    case ECatalogsSmsCancelled: // Flow through
	    case ECatalogsSmsSendingFailed:
	        {
            // Remove SMS operation
        	RemovePendingSmsOp( aOperation.OperationId() );
	        iPurchaseOperationState = EFailed;
	        iError = KErrGsmSMSShortMessageTransferRejected;
            break;
	        }
        default:
            {
            break;
            }
	    }
	
    // All SMS operations have been completed. Continue purchase process.
    if( iPendingSmsOperations.Count() == 0 )
	    {
	    if ( iSmsRegistrationRequest ) 
	        {	        
	        // If the operation is already in failed state, let's not change
	        // the state.
	        if ( iPurchaseOperationState != EFailed )
                {
    	        iPurchaseOperationState = EBegin;
                }
    	    
    	    delete iSmsRegistrationRequest;
    	    iSmsRegistrationRequest = NULL;
    	    
    	    if ( iResendAfter != KResendAfterDisabled ) 
    	        {
    	        StartResendAfterTimer();
    	        // we don't want to call RunOperation now
    	        return KErrNone;
    	        }
	        }
        else 
            {
            // If the operation is already in failed state, let's not change
	        // the state.
	        if ( iPurchaseOperationState != EFailed )
                {
    	        iPurchaseOperationState = EPaymentProcessed;
                }
            }
        // continue operation
        RunOperation();
        }
        
        
	DLTRACEOUT((""));
	return KErrNone;
	}



void CNcdPurchaseOperationImpl::UpdatePurchaseHistoryL()
    {
    DLTRACEIN((""));
    // Create purchase details and fill necessary information to it.
    CNcdPurchaseDetails* purchaseDetails = CNcdPurchaseDetails::NewLC();
    
    // Fill purchase info

    purchaseDetails->SetClientUid( iNode->NodeLinkL().MetaDataIdentifier().ClientUid() );
    purchaseDetails->SetNamespaceL( iNode->NodeLinkL().MetaDataIdentifier().NodeNameSpace() );
    purchaseDetails->SetEntityIdL( iNode->NodeLinkL().MetaDataIdentifier().NodeId() );
    purchaseDetails->SetItemNameL( iNode->NodeMetaDataL().NodeName() );
    
    purchaseDetails->SetCatalogSourceNameL( iNode->NodeLinkL().CatalogsSourceName() );
    purchaseDetails->SetPurchaseOptionIdL( iSelectedPurchaseOption->Id() );
    purchaseDetails->SetServerUriL( iNode->NodeLinkL().ServerUri() );
    purchaseDetails->SetItemType( MNcdPurchaseDetails::EItem ); // Should set this from node!
    
    TRAPD( err, SetContentInfoToPurchaseDetailsL( 
        *purchaseDetails, iNode->NodeMetaDataL() ) );
    
    LeaveIfNotErrorL( err, KErrNotFound );
         
    
    if( iNode->ClassId() == NcdNodeClassIds::ENcdSearchItemNodeClassId )
        {
        // this is a search node, get the origin id from parent node
        CNcdSearchNodeFolder& searchFolder = iNodeManager->SearchFolderL(
            iNode->NodeLinkL().ParentIdentifier() );
        // NOTE: This sets some parent node's origin id as origin id.
        // This is ok as the origin id is used only for retrieving the correct
        // acccess point and ap:s are set only for catalogs/folders. Thus using 
        // a parent catalog's/folder's origin id will result in correct results.
        DLTRACE((_L("Search node, set origin node id from parent, resulting id=: %S"),
            &searchFolder.OriginIdentifierL().NodeId() ));
        purchaseDetails->SetOriginNodeIdL( searchFolder.OriginIdentifierL().NodeId() );
        }
    else
        {
        // this is a normal node, use it's id directly
        DLTRACE((_L("Normal node use own id as origin node id=: %S"),
            &iNode->Identifier().NodeId() ));
        DLTRACE((_L("iNodeIdentifier=: %S"),
            &iNodeIdentifier->NodeId() ));
        purchaseDetails->SetOriginNodeIdL( iNode->Identifier().NodeId() );
        }

    DLINFO(("Node info set."));
    
    TRAP_IGNORE( HandleDependenciesL( *purchaseDetails ) );
    
    DLINFO(("Dependencies handled"));
    
            
    CNcdPurchaseDownloadInfo* downloadInfo( NULL );
        
    TInt count( 0 );
    if( !iSelectedPurchaseOption->IsFree() ||
        iSelectedPurchaseOption->RequirePurchaseProcess() ||
        iSelectedPurchaseOption->DownloadInfoCount() < 1 )
        {
        DLINFO(("Setting download details according to info received."));
        
        count = iPurchaseProcessedData->EntityL( 0 ).DownloadDetailsCount();
        for( TInt i = 0; i < count; i++ )
            {
            downloadInfo = 
                CNcdDownloadInfo::NewLC( 
                    iPurchaseProcessedData->EntityL( 0 ).DownloadDetailsL( i ) );
            purchaseDetails->AddDownloadInfoL( downloadInfo );           
            CleanupStack::Pop( downloadInfo );
            }
            
        // Only the price of one item is needed so it has to be taken from purchase option.
        // Total price of iPurchaseInformationData is a price of a group of items if
        // such a group is bought.
        purchaseDetails->SetFinalPriceL( iSelectedPurchaseOption->PriceText() );
        }
    else
        {
        DLINFO(("Setting download details according to purchase option."));
        
        count = iSelectedPurchaseOption->DownloadInfoCount();
        for( TInt i = 0; i < count; i++ )
            {
            downloadInfo = 
                CNcdPurchaseDownloadInfo::NewLC( 
                    iSelectedPurchaseOption->DownloadInfo( i ) );            
            purchaseDetails->AddDownloadInfoL( downloadInfo );            
            CleanupStack::Pop( downloadInfo );
            }
        purchaseDetails->SetFinalPriceL( iSelectedPurchaseOption->PriceText() );
        }
    DLINFO(("Download details set."));

    // Add the correct number of empty file paths
    for ( TInt i = 0; i < purchaseDetails->DownloadInfoCount(); ++i ) 
        {
        purchaseDetails->AddDownloadedFileL( KNullDesC );
        }

    purchaseDetails->SetDescriptionL( iNode->NodeMetaDataL().Description() );
    DLINFO(("Description set."));
    purchaseDetails->SetPurchaseOptionNameL( iSelectedPurchaseOption->PurchaseOptionName() );
    purchaseDetails->SetPurchaseOptionPriceL( iSelectedPurchaseOption->PriceText() );
    DLINFO(("Purchase option info set."));
    
    // Set the purchase time. Note that this is universal time, not local time.
    // This helps in syncing the purchase history with pc client front end.
    // UI must convert this to local time before showing it.
    TTime time;
    time.UniversalTime();
    purchaseDetails->SetPurchaseTime( time );
    DLINFO(("Purchase time set."));
    
   
    // If icon can not be found, set null. Done because at the moment
    // we don't have icons for all items.
    HBufC8* iconData( NULL );
    TRAP_IGNORE( iconData = iNode->NodeMetaDataL().IconL().IconDataL() );
    
    if ( iconData ) 
        {        
        purchaseDetails->SetIcon( iconData );
    
        DLINFO(("Icon set."));
        }
    
    purchaseDetails->SetLastUniversalOperationTime();
    purchaseDetails->SetLastOperationErrorCode( KErrNone );    
    
    // Save it to the purchase history
    iPurchaseHistory->SavePurchaseL( *purchaseDetails );
    
    CleanupStack::PopAndDestroy( purchaseDetails );
    DLTRACEOUT((""));
    }


void CNcdPurchaseOperationImpl::SetContentInfoToPurchaseDetailsL( 
    CNcdPurchaseDetails& aDetails, 
    const CNcdNodeMetaData& aMetadata ) const
    {
    DLTRACEIN((""));
    const CNcdNodeContentInfo& info( aMetadata.ContentInfoL() );
        
    aDetails.SetTotalContentSize( info.TotalContentSize() );
    aDetails.SetVersionL( info.Version() );
    aDetails.SetItemPurpose( info.Purpose() );  
    aDetails.SetAttributeL( 
        MNcdPurchaseDetails::EPurchaseAttributeContentUid, 
        info.Uid().iUid );

    aDetails.SetAttributeL( 
        MNcdPurchaseDetails::EPurchaseAttributeContentMimeType, 
        info.MimeType() );
    
    }


void CNcdPurchaseOperationImpl::HandleDependenciesL( 
    CNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN((""));
    CNcdNodeMetaData& metadata( iNode->NodeMetaDataL() );
    
    const CNcdNodeDependency& dep( metadata.DependencyL() );
    AddDownloadInfosToDetailsL( aDetails, dep.ContentTargets() );
    DLTRACEOUT(("Dependencies handled successfully"));
    }


void CNcdPurchaseOperationImpl::AddDownloadInfosToDetailsL( 
    CNcdPurchaseDetails& aDetails, 
    const RPointerArray<CNcdDownloadInfo>& aInfos )
    {
    DLTRACEIN((""));
    CNcdPurchaseDownloadInfo* downloadInfo( NULL );
    CNcdPurchaseInstallInfo* installInfo( NULL );
    for( TInt i = 0; i < aInfos.Count(); i++ )
        {
        downloadInfo = 
            CNcdPurchaseDownloadInfo::NewLC( 
                *aInfos[ i ] );
        aDetails.AddDownloadInfoL( downloadInfo );           
        CleanupStack::Pop( downloadInfo );        
        
        DLTRACE(( _L("Adding install info, uid: %x, version: %S"), 
            aInfos[i]->ContentId(), &aInfos[i]->ContentVersion() ));
            
        installInfo = CNcdPurchaseInstallInfo::NewLC();
        installInfo->SetApplicationUid( aInfos[i]->ContentId() );
        installInfo->SetApplicationVersionL( aInfos[i]->ContentVersion() );
        aDetails.AddInstallInfoL( installInfo );
        CleanupStack::Pop( installInfo );
        }
    DLTRACEOUT(("Download infos added successfully"));
    }


TInt CNcdPurchaseOperationImpl::RemovePendingSmsOp(
    const TCatalogsTransportOperationId& aId )
    {
	for( TInt i = 0; i < iPendingSmsOperations.Count(); i++ )
	    {
	    if( iPendingSmsOperations[i]->OperationId() == aId )
	        {
    	    iPendingSmsOperations[i]->Release();
	        iPendingSmsOperations.Remove( i );
	        return KErrNone;
	        }
	    }
	return KErrNotFound;
    }

TBool CNcdPurchaseOperationImpl::QueryCompletedL( CNcdQuery* aQuery )
    {
    DLTRACEIN((""));
    DLINFO(("query response: %d", aQuery->Response() ));
    TBool handled = EFalse;
    aQuery->InternalAddRef();
    CleanupInternalReleasePushL( *aQuery );
    
    if( iPurchaseQuery && iPurchaseQuery == aQuery )
        {
        // this is a purchase query (i.e. initiated directly from purchase op)
        if( iPurchaseOperationState == EPurchaseComplete )
            {
            // Operation is already complete.
            // This is needed for purchase information, which is sent last 
            // just before completing the operation.
            iPurchaseQuery->InternalRelease();
            iPurchaseQuery = NULL;
            }
        else if ( aQuery->Semantics() == MNcdQuery::ESemanticsRegistrationQuery &&
                    iSmsRegistrationRequest ) 
            {
            if ( aQuery->Response() == MNcdQuery::EAccepted ) 
                {
                iPendingSmsOperations.AppendL( 
        	    	iSmsSession->CreateSmsL( iSmsRegistrationRequest->Key(),
        	    				             iSmsRegistrationRequest->Value(),
                                             this ) );
                }
            else 
                {
                delete iSmsRegistrationRequest;
                iSmsRegistrationRequest = 0;
                }
            // query is now handled, release it
            iPurchaseQuery->InternalRelease();
            iPurchaseQuery = NULL;
            }
        // in the rest of the cases the query must not be rejected or op will fail
        else if ( aQuery->Response() == MNcdQuery::ERejected )
            {
            DLTRACE(("Query rejected -> fail operation"));
            iError = KNcdErrorMandatoryQueryRejected;
            iPurchaseOperationState = EFailed;
            // query is now handled, release it
            iPurchaseQuery->InternalRelease();
            iPurchaseQuery = NULL;
            User::Leave( KNcdErrorMandatoryQueryRejected );
            }
        else if( aQuery->Id() == KQueryIdPaymentMethod )
            {
            CNcdQuerySelectionItem* item = static_cast<CNcdQuerySelectionItem*>( &aQuery->QueryItemL( 0 ) );
            iSelectedPaymentMethod = &iPurchaseInformationData->PaymentL( item->Selection() );
            // query is now handled, release it
            iPurchaseQuery->InternalRelease();
            iPurchaseQuery = NULL;
            }
        else if ( aQuery->Semantics() == MNcdQuery::ESemanticsDisclaimer )
            {
            // disclaimer does not require a response
            // query is now handled, release it
            iPurchaseQuery->InternalRelease();
            iPurchaseQuery = NULL;
            }
        else
            {
            // payment info query, don't release, response needs to be added to
            // confirmation request
            }
        handled = ETrue;
        }
    CleanupStack::PopAndDestroy( aQuery );
    return handled;
    }

TInt CNcdPurchaseOperationImpl::ResendAfterCallBack( TAny* aOperation ) 
    {
    DLTRACEIN((""));
    CNcdPurchaseOperationImpl* purchaseOp = static_cast<CNcdPurchaseOperationImpl*>( aOperation );
    purchaseOp->iLocked = EFalse;
    purchaseOp->iResendAfter = KResendAfterDisabled;
    return purchaseOp->RunOperation();
    }


TInt CNcdPurchaseOperationImpl::StartResendAfterTimer()
    {
    DLTRACEIN((""));
    TCallBack cb( ResendAfterCallBack, this );
    delete iDelay;
    iDelay = NULL;
    
    TRAPD( err, iDelay = CNcdDelay::NewL( cb ) );
    if ( err == KErrNone ) 
        {
        iDelay->After( iResendAfter * 1000000 );
        iLocked = ETrue;
        }
    else
        {
        iPurchaseOperationState = EFailed;
        iError = err;
        Cancel();
        RunOperation();
        }
    
    return err;
    }
