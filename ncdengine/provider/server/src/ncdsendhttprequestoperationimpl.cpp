/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ncdsendhttprequestoperationimpl.h"

#include <s32mem.h>
#include <apmstd.h>

#include "catalogsbasemessage.h"
#include "catalogshttpincludes.h"
#include "catalogsutils.h"
#include "catalogscontext.h"
#include "ncdproviderdefines.h"
#include "catalogshttprequestadapter.h"
#include "catalogshttpresponsecomposer.h"
#include "ncdproviderutils.h"
#include "ncdhttputils.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdSendHttpRequestOperation* CNcdSendHttpRequestOperation::NewL( 
    HBufC8* aUri,
    HBufC8* aRequest,        
    const TNcdConnectionMethod& aMethod,
    CNcdGeneralManager& aGeneralManager,
    MNcdOperationRemoveHandler& aRemoveHandler, 
    MCatalogsHttpSession& aHttpSession,
    MCatalogsSession& aSession )
    {
    CNcdSendHttpRequestOperation* self = new( ELeave ) CNcdSendHttpRequestOperation(      
        aGeneralManager,
        aRemoveHandler, 
        aHttpSession,
        aSession );
    CleanupClosePushL( *self );
    self->ConstructL( aMethod, aUri, aRequest );
    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdSendHttpRequestOperation::~CNcdSendHttpRequestOperation()
    {
    DLTRACEIN( ( "" ) );    
    if ( iTransaction ) 
        {
        iTransaction->Release();
        iTransaction = NULL;
        }
    delete iUri;
    delete iRequest;
    delete iResponse;
    delete iAdapter;
    delete iBody;
    }


// ---------------------------------------------------------------------------
// HTTP event handler
// ---------------------------------------------------------------------------
//
void CNcdSendHttpRequestOperation::HandleHttpEventL( 
    MCatalogsHttpOperation& aOperation, 
    TCatalogsHttpEvent aEvent )
    {
    DLTRACEIN((""));
    
    if ( aEvent.iOperationState == ECatalogsHttpOpInProgress &&
         aEvent.iProgressState == ECatalogsHttpResponseBodyReceived ) 
        {
        // Append the body
        iBody->InsertL( iBody->Size(), aOperation.Body() );
        }
    else if ( aEvent.iOperationState == ECatalogsHttpOpCompleted ) 
        {
        DLTRACE(("Operation complete"));
        // Compose the response here
        TCatalogsHttpResponseComposer composer;
        iResponse = composer.ComposeResponseL( 
            aOperation, 
            iBody->Ptr( 0 ) );
        
        delete iBody;
        iBody = NULL;
        aOperation.Release();
        iTransaction = NULL;
        iOperationState = EStateComplete;
        RunOperation();
        }
    }
    

// ---------------------------------------------------------------------------
// HTTP error handler
// ---------------------------------------------------------------------------
//
TBool CNcdSendHttpRequestOperation::HandleHttpError(
    MCatalogsHttpOperation& aOperation,
    TCatalogsHttpError aError )
    {
    DLTRACEIN((""));
    aOperation.Cancel();
    iTransaction = NULL;
    iError = aError.iError;
    iOperationState = EStateComplete;
    
    RunOperation();
        
    return ETrue;
    }
    

// ---------------------------------------------------------------------------
// Cancel
// ---------------------------------------------------------------------------
//
void CNcdSendHttpRequestOperation::Cancel() 
    {    
    DLTRACEIN(( "" ));
    if ( iTransaction ) 
        {
        iTransaction->Cancel();
        iTransaction = NULL;
        }
    }


// ---------------------------------------------------------------------------
// ReceiveMessage
// ---------------------------------------------------------------------------
//
void CNcdSendHttpRequestOperation::ReceiveMessage( 
    MCatalogsBaseMessage* aMessage,
    TInt aFunctionNumber )
    {
    DLTRACEIN(( "Function: %d", aFunctionNumber ));
    
    TInt err = KErrNone;
        
    // Response to pause and resume messages, other messages are handled
    // by the base class
    switch ( aFunctionNumber )
        {
        case ENCDOperationFunctionGetData: 
            {
            DLTRACE(( "ENCDOperationFunctionGetData "));
            // Only error will come from CompleteMessageL
            TRAP( err, GetResponseL( *aMessage ) );
            break;
            }
                        
        default:
            {
            DLTRACE(("Calling baseclass"));
            // Call implementation in the base class
            CNcdBaseOperation::ReceiveMessage( aMessage, aFunctionNumber );
            DLTRACEOUT(( "Called baseclass" ));
            return;
            }
        }                    
    
    HandleError( err );
    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// RunOperation
// ---------------------------------------------------------------------------
//
TInt CNcdSendHttpRequestOperation::RunOperation()
    {
    DLTRACEIN(( "Pending message: %X", iPendingMessage ));
    
    
    if ( !iPendingMessage ) 
        {      
        DLTRACE(("No pending message"));  
        return KErrNotReady;
        }
    
    TRAPD( err, HandleStateL() );
    HandleError( err );
    
    DLTRACEOUT(("err: %d", err));
    return err;
    }



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdSendHttpRequestOperation::HandleStateL()
    {
    switch ( iOperationState ) 
        {
        case EStateRunning: 
            {
            DLTRACE(("Creating request"));
            DASSERT( iRequest );            
            iTransaction = iAdapter->CreateTransactionL( 
                *iUri,
                *iRequest,
                *this );
            
            iTransaction->Config().SetConnectionMethod( 
                iConnectionMethod );
                
            TInt err = iTransaction->Start();
            if ( err != KErrNone ) 
                {
                DLERROR(("Start failed with %d", err));
                iTransaction->Cancel();
                iTransaction = NULL;
                User::Leave( err );
                }
                        
            break;
            }
            
        case EStateComplete:
            {
            // Returns false if there was no error
            if ( !HandleError( iError ) ) 
                {
                DLTRACE(("Completing the operation"));                
                CNcdBaseOperation::CompleteMessage( 
                    iPendingMessage,
                    ENCDOperationMessageCompletionComplete, 
                    iProgress,
                    KErrNone );
                
                }
            break;
            }
        
        default: 
            {
            DASSERT( 0 );
            }
        }
    
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CNcdSendHttpRequestOperation::HandleError( TInt aError ) 
    {
    DLTRACEIN(("aError: %d", aError ));
    if ( aError != KErrNone ) 
        {
        DLERROR(("Error %d occurred", aError ));
        iError = aError;
        iOperationState = EStateCancelled;
        Cancel();
        if ( iPendingMessage )
            {
            // ignoring error because operation already failed
            CNcdBaseOperation::CompleteMessage( iPendingMessage,
                ENCDOperationMessageCompletionError, iError );
            }
        return ETrue;
        }    
    return EFalse;
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CNcdSendHttpRequestOperation::GetResponseL( 
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    if ( !iResponse ) 
        {
        User::Leave( KErrCorrupt );
        }
                
    aMessage.CompleteAndReleaseL(
        *iResponse, 
        KErrNone );
    
    }
    
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdSendHttpRequestOperation::CNcdSendHttpRequestOperation(
    CNcdGeneralManager& aGeneralManager,
    MNcdOperationRemoveHandler& aRemoveHandler,
    MCatalogsHttpSession& aHttpSession,
    MCatalogsSession& aSession )
    :
    CNcdBaseOperation( aGeneralManager, &aRemoveHandler, ESendHttpRequestOperation,
        aSession ), 
    iHttpSession( aHttpSession )
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdSendHttpRequestOperation::ConstructL(
    const TNcdConnectionMethod& aMethod,
    HBufC8* aUri,
    HBufC8* aRequest )
    {
    DLTRACEIN( ( "" ) );

    // Call ConstructL for the base class
    CNcdBaseOperation::ConstructL();

    iAdapter = CCatalogsHttpRequestAdapter::NewL( iHttpSession );
    iBody = CBufFlat::NewL( NcdProviderDefines::KNcdBufferExpandSize );
    iGeneralManager.HttpUtils().ConvertConnectionMethod( 
        aMethod,
        iConnectionMethod );
    iUri = aUri;
    iRequest = aRequest;
    }
    
