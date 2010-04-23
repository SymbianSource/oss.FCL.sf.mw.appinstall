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
* Description:  
*
*/


#include "catalogshttptransaction.h"

#include "catalogshttptransactionmanager.h"
#include "catalogshttpobserver.h"
#include "catalogshttpconfigimpl.h"
#include "catalogskeyvaluepair.h"
#include "catalogshttpheadersimpl.h"
#include "catalogshttpstack.h"
#include "catalogsutils.h"
#include "catalogserrors.h"
#include "catalogshttpconnectionmanager.h"
#include "catalogsconnection.h"
#include "catalogshttputils.h"

#include "catalogsdebug.h"

// HTTP header strings
_LIT8( KHttpContentRangeHeader, "Content-Range" );
_LIT8( KHttpContentLengthHeader, "Content-Length" );
_LIT8( KHttpContentTypeHeader, "Content-Type" );


const TInt KTransactionRetries = 3;

// ======== MEMBER FUNCTIONS ========

TInt CCatalogsHttpTransaction::StartCallBack( TAny* aPtr )
    {
    DLTRACEIN((""));
    CCatalogsHttpTransaction* transaction = 
        static_cast<CCatalogsHttpTransaction*>( aPtr );
        
    TInt err = transaction->Start();
    if ( err != KErrNone ) 
        {
        transaction->iObserver->HandleHttpError( 
            *transaction, 
            TCatalogsHttpError( 
                ECatalogsHttpErrorGeneral, 
                err ) );            
        }
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	
CCatalogsHttpTransaction* CCatalogsHttpTransaction::NewL( 
    MCatalogsHttpTransactionManager& aOwner, 
    const CCatalogsHttpConfig& aConfig,
    const TCatalogsTransportOperationId& aId,
    TCatalogsHttpTransactionType aType )
    {
    CCatalogsHttpTransaction* self = NewLC( aOwner, aConfig, aId, aType );
    CleanupStack::Pop( self );      
    return self;
    }


// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	
CCatalogsHttpTransaction* CCatalogsHttpTransaction::NewLC( 
    MCatalogsHttpTransactionManager& aOwner, 
    const CCatalogsHttpConfig& aConfig,
    const TCatalogsTransportOperationId& aId,
    TCatalogsHttpTransactionType aType )
    {
    CCatalogsHttpTransaction* self = new( ELeave ) CCatalogsHttpTransaction( 
        aOwner, aId, aType );
        
    CleanupStack::PushL( self );
    self->ConstructL( &aConfig );    
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//	
CCatalogsHttpTransaction::~CCatalogsHttpTransaction()
    {    
    DLTRACEIN(( "Deleting Operation: %i, id: %i", 
        reinterpret_cast<TInt>( this ), OperationId().Id() ));
        
    SetTransferring( EFalse );    
    
    ReleasePtr( iConnection );
    
    DLTRACE(( "Removing from owner" ));
    delete iHttp;

    iOwner.RemoveOperation( this ); 

    iOwner.CompleteOperation( this );    

        
    DLTRACE(( "Deleting config" ));    
    delete iConfig;
    
    DLTRACE(( "Deleting response headers" ));
    delete iResponseHeaders;    
    
    DLTRACE(( "Deleting request body" ));
    delete iRequestBody;
    
    DLTRACE(( "Deleting content type" ));
    delete iContentType;   
    
    delete iUri;

    delete iEncodedUri;
    
    delete iResponseStatusText;
    
    delete iCallBack;
    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// Add a reference
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpTransaction::AddRef()
    {
    iRefCount++;
    return iRefCount;
    }


// ---------------------------------------------------------------------------
// Release a reference
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpTransaction::Release()
    {
    DLTRACEIN(( "" ));
    iRefCount--;
    if ( !iRefCount ) 
        {
        delete this;
        return 0;
        }
    return iRefCount;
    }
        

// ---------------------------------------------------------------------------
// Reference count
// ---------------------------------------------------------------------------
//        
TInt CCatalogsHttpTransaction::RefCount() const
    {
    return iRefCount;
    }


// ---------------------------------------------------------------------------
// Cancel transaction
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpTransaction::Cancel()
    {
    DLTRACEIN(( "" )); 
    iState.iOperationState = ECatalogsHttpOpDeleting;
    SetTransferring( EFalse );
    if ( iHttp ) 
        {        
        iHttp->CancelTransaction(); 

        DeletePtr( iHttp );        
        }
        
    if ( iState.iProgressState != ECatalogsHttpNone && 
         iState.iProgressState != ECatalogsHttpDone ) 
        {        
        iOwner.CompleteOperation( this );
        }
    DLTRACE(("Calling release"));
    return Release();
    }


// ---------------------------------------------------------------------------
// Transaction progress
// ---------------------------------------------------------------------------
//	
TCatalogsTransportProgress CCatalogsHttpTransaction::Progress() const
    {
    return TCatalogsTransportProgress( iState.iOperationState, iTransferredLength, 
        iContentLength );
    }


// ---------------------------------------------------------------------------
// Start download
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpTransaction::Start( TResumeStartInformation /*aResumeOrigin*/ )
    {
    DLTRACEIN(( "" ));    
    
    if ( !iEncodedUri || !iEncodedUri->Length() ) 
        {
        DLTRACEOUT(( "No URI, return KErrNotReady" ));
        return KErrNotReady;
        }

    DASSERT( iConfig );
    
    // Update the member variable
    iObserver = iConfig->Observer();
    
    if ( !iObserver ) 
        {
        DLTRACEOUT(( "iObserver = NULL, return KErrNotReady" ));
        return KErrNotReady;
        }
    
    TInt err = KErrNone;
    if ( iState.iOperationState == ECatalogsHttpOpCreated 
        || iState.iOperationState == ECatalogsHttpOpQueued )
        {        
        UpdateConfiguration();
        
        TInt err = iOwner.StartOperation( this );

        if ( err == KErrNone ) 
            {
            TRAP( err, DoStartL() );
            }
        else if ( err == KCatalogsHttpOperationQueued ) 
            {
            DLTRACE(( "Transaction queued" ));
            err = KErrNone;
            
            // Set as queued and notify observer if any
            iState.iOperationState = ECatalogsHttpOpQueued;            
            if ( iObserver ) 
                {
                // Start errors are returned so no NotifyObserver
                TRAP( err, iObserver->HandleHttpEventL( *this, iState ) );
                }
            }
        }
    DLTRACEOUT(("err: %d", err));
    return err;
    }


// ---------------------------------------------------------------------------
// Pause download
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpTransaction::Pause()
    {
    DLTRACEIN(( "" ));
    return KErrNotSupported;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::NotifyCancel()
    {
    DLTRACEIN((""));
    if ( iObserver )
        {
        iObserver->HandleHttpError( *this, TCatalogsHttpError(
            ECatalogsHttpErrorGeneral, KErrCancel ) );
        }
    else 
        {
        Cancel();        
        }
    }


// ---------------------------------------------------------------------------
// Set body
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::SetBodyL( const TDesC8& aBody )
    {
    DLTRACEIN(( "" ));
    delete iRequestBody;
    iRequestBody = NULL;
    iRequestBody = aBody.AllocL();
    }


// ---------------------------------------------------------------------------
// Set body
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::SetBodyL( const TDesC16& aBody )
    {
    DLTRACEIN(( "" ));
    delete iRequestBody;
    iRequestBody = NULL;
    iRequestBody = ConvertUnicodeToUtf8L( aBody );
    }


// ---------------------------------------------------------------------------
// Body getter
// ---------------------------------------------------------------------------
//	
const TDesC8& CCatalogsHttpTransaction::Body() const
    {
    return iResponseBody;
    }


// ---------------------------------------------------------------------------
// URI setter
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::SetUriL( const TDesC8& aUri )
    {
    delete iUri;
    iUri = NULL;
    iUri = aUri.AllocL();
    EncodeUriL();
    }


// ---------------------------------------------------------------------------
// URI setter
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::SetUriL( const TDesC16& aUri )
    {
    delete iUri;
    iUri = NULL;
    iUri = ConvertUnicodeToUtf8L( aUri );
    EncodeUriL();    
    }


// ---------------------------------------------------------------------------
// URI getter
// ---------------------------------------------------------------------------
//	
const TDesC8& CCatalogsHttpTransaction::Uri() const
    {
    if( iUri ) 
        {        
        return *iUri;
        }
    return KNullDesC8();
    }


// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------
//	
MCatalogsHttpConfig& CCatalogsHttpTransaction::Config() const
    {
    return *iConfig;
    }


// ---------------------------------------------------------------------------
// Request headers
// ---------------------------------------------------------------------------
//	
MCatalogsHttpHeaders& CCatalogsHttpTransaction::RequestHeadersL() const
    {
    return iConfig->RequestHeaders();
    }


// ---------------------------------------------------------------------------
// Response headers
// ---------------------------------------------------------------------------
//	
const MCatalogsHttpHeaders& CCatalogsHttpTransaction::ResponseHeadersL() const
    {
    return *iResponseHeaders;
    }


// ---------------------------------------------------------------------------
// Operation type
// ---------------------------------------------------------------------------
//	
TCatalogsHttpOperationType CCatalogsHttpTransaction::OperationType() const
    {
    return ECatalogsHttpTransaction;    
    }


// ---------------------------------------------------------------------------
// Operation ID
// ---------------------------------------------------------------------------
//	
const TCatalogsTransportOperationId& 
    CCatalogsHttpTransaction::OperationId() const
    {
    return iId;
    }


// ---------------------------------------------------------------------------
// Content type setter
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::SetContentTypeL( const TDesC8& aContentType )
    {
    delete iContentType;
    iContentType = NULL;
    iContentType = aContentType.AllocL();
    }


// ---------------------------------------------------------------------------
// Content type setter
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::SetContentTypeL( const TDesC16& aContentType )
    {
    delete iContentType;
    iContentType = NULL;
    iContentType = ConvertUnicodeToUtf8L( aContentType );
    }


// ---------------------------------------------------------------------------
// Content type getter
// ---------------------------------------------------------------------------
//	
const TDesC8& CCatalogsHttpTransaction::ContentType() const
    {
    if ( iContentType )
        {
        return *iContentType;
        }
    return KNullDesC8();
    }


// ---------------------------------------------------------------------------
// Content size getter
// ---------------------------------------------------------------------------
//	
TInt32 CCatalogsHttpTransaction::ContentSize() const
    { 
    return iContentLength;
    }


// ---------------------------------------------------------------------------
// Downloaded size getter
// ---------------------------------------------------------------------------
//	
TInt32 CCatalogsHttpTransaction::TransferredSize() const
    {
    return iTransferredLength;
    }


// ---------------------------------------------------------------------------
// Is download pausable
// ---------------------------------------------------------------------------
//	
TBool CCatalogsHttpTransaction::IsPausable() const
    {
    return EFalse;
    }


// ---------------------------------------------------------------------------
// Sets header mode
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::SetHeaderMode( TCatalogsHttpHeaderMode 
    /*aMode*/ )
    {
    // not supported
    }


// ---------------------------------------------------------------------------
// Transaction status code
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpTransaction::StatusCode() const
    {
    return iResponseStatusCode;
    }


// ---------------------------------------------------------------------------
// Transaction status text
// ---------------------------------------------------------------------------
//	
const TDesC8& CCatalogsHttpTransaction::StatusText() const
    {
    if ( iResponseStatusText ) 
        {
        return *iResponseStatusText;
        }
    return KNullDesC8();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::SetConnection( CCatalogsConnection& aConnection )
    {
    DLTRACEIN((""));
    ReleasePtr( iConnection );
    
    iConnection = &aConnection;
    iConnection->AddRef();
    Config().SetConnectionMethod( aConnection.ConnectionMethod() );
    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::ReportConnectionError( TInt aError )
    {
    DLTRACEIN(("aError: %d", aError ));
    iObserver->HandleHttpError( *this, 
        TCatalogsHttpError( ECatalogsHttpErrorGeneral, aError ) );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
CCatalogsHttpConnectionManager& CCatalogsHttpTransaction::ConnectionManager()
    {
    return iOwner.ConnectionManager();
    }


// ---------------------------------------------------------------------------
// Not supported
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpTransaction::ExternalizeL( RWriteStream& /* aStream */ ) const
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------------------------
// Not supported
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpTransaction::InternalizeL( RReadStream& /* aStream */ )
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// Operation state
// ---------------------------------------------------------------------------
//  
TCatalogsHttpOperationState CCatalogsHttpTransaction::State() const
    {
    return iState.iOperationState;
    }


// ---------------------------------------------------------------------------
// Handles events from the configuration
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpTransaction::HandleHttpConfigEvent( 
    MCatalogsHttpConfig* /* aConfig */,
    const TCatalogsHttpConfigEvent& aEvent )
    {
    if ( aEvent == ECatalogsHttpCfgPriorityChanged ) 
        {
        return iOwner.OperationPriorityChanged( this );
        }
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// Response received
// ---------------------------------------------------------------------------
//	
TBool CCatalogsHttpTransaction::ResponseReceived( TInt aResponseStatusCode, 
    const TDesC8& aResponseStatusText )
    {    
    DLTRACEIN(( "%d, text: %S", aResponseStatusCode, 
        &aResponseStatusText ));

    iResponseStatusCode = aResponseStatusCode;
    
    iResponseStatusText = aResponseStatusText.Alloc();
    
    if ( !iResponseStatusText )
        {
        return HandleHttpError( ECatalogsHttpErrorGeneral, KErrNoMemory );        
        }

    
    if ( aResponseStatusCode >= 400 ) 
        {
        iState.iOperationState = ECatalogsHttpOpFailed;
        
        return iObserver->HandleHttpError( *this, TCatalogsHttpError( 
            ECatalogsHttpErrorHttp, 
            KCatalogsErrorHttpBase - aResponseStatusCode ) );
        }
    else 
        {
        iState.iOperationState = ECatalogsHttpOpInProgress;
        iState.iProgressState = ECatalogsHttpConnected;        
        NotifyObserver();
        }
    DLTRACEOUT((""));
    return EFalse;
    }


// ---------------------------------------------------------------------------
// Received a response header
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::ResponseHeaderReceived( const TDesC8& aHeader, 
    const TDesC8& aValue )
    {
    DLTRACEIN( ("") );
    DASSERT( iObserver );
    
    TRAPD( err, iResponseHeaders->AddHeaderL( aHeader, aValue ) );        
    if ( err != KErrNone )
        {
        HandleHttpError( ECatalogsHttpErrorGeneral, err );
        return;
        }
        
    iState.iOperationState = ECatalogsHttpOpInProgress;
    iState.iProgressState = ECatalogsHttpResponseHeaderReceived;
         
    
    if ( aHeader.CompareF( KHttpContentRangeHeader ) == 0 )
        {
        //
        // Content-Range, bytes x-y/z
        // Extract 'z' and use it as the total content length
        //
        TPtrC8 ptr( aValue );
        TInt offset = ptr.Locate( '/' );
        if ( offset != KErrNotFound )
            {
            TLex8 val;
            val.Assign( ptr.Mid( offset + 1 ) );

            TInt value;
            TInt err = val.Val( value );
            if ( err == KErrNone )
                {
                iContentLength = value;
                DLTRACE(( "Content length: %i", iContentLength ));
                }
            }
        }
    else if ( aHeader.CompareF( KHttpContentLengthHeader ) == 0 )
        {
        //
        // If content length for this request has not been already set
        // e.g. from a Content-Range header, extract from Content-Length header
        //
        if ( iContentLength == 0 )
            {
            TLex8 val;
            val.Assign( aValue );

            TInt value;
            TInt err = val.Val( value );
            if ( err == KErrNone )
                {
                iContentLength = value;
                DLTRACE(( "Content length: %i", iContentLength ));
                }                
            }
        else
            {
            DLTRACE(( "-> ContentLength set, ignoring" ));
            }
        }
    else if ( aHeader.CompareF( KHttpContentTypeHeader ) == 0 ) 
        {
        // Content type from the header
        DLTRACE( ( "Content type received" ) );
        
        TRAPD( err, SetContentTypeL( aValue ) );
        if ( err != KErrNone ) 
            {
            DLTRACE( ( "Content type setting failed with err: %i",
                err ) );
            HandleHttpError( ECatalogsHttpErrorGeneral, err );
            return;
            }          
        else 
            {
            iState.iProgressState = ECatalogsHttpContentTypeReceived;
            }
        }
         
    // Report the observer that a header has been received
    NotifyObserver();
    }



// ---------------------------------------------------------------------------
// Received a part of the response
// ---------------------------------------------------------------------------
//	
TBool CCatalogsHttpTransaction::ResponseBodyReceived( const TDesC8& aData )
    {
    DLTRACEIN( ( "" ) );
    DASSERT( iObserver );
    
    iState.iOperationState = ECatalogsHttpOpInProgress;    
    iState.iProgressState = ECatalogsHttpResponseBodyReceived;
    
    // Set body to point to response body
    iResponseBody.Set( aData );
    
    iTransferredLength += iResponseBody.Length();
       
    DLTRACE( ( "Calling observer" ) );
    NotifyObserver();
    
    DLTRACE( ( "Observer finished" ) );
    
    // Response body back to empty
    iResponseBody.Set( KNullDesC8 );
    return ETrue;
    }
    

// ---------------------------------------------------------------------------
// Request submitted
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::RequestSubmitted()
    {
    DLTRACEIN((""));
    SetTransferring( ETrue );
    }


// ---------------------------------------------------------------------------
// Request completed
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::RequestCompleted( TInt aError )
    {
    DLTRACEIN( ("Error: %d", aError ) );
    DASSERT( iObserver );
    AddRef();

    SetTransferring( EFalse );
    if ( aError != KErrNone ) 
        {
        iOwner.ConnectionManager().ReportConnectionError( 
            iConfig->ConnectionMethod(),
            aError );
        }
    
    // If reference count is 1, then the operation has already been 
    // cancelled/released
    if ( iRefCount > 1 )
        {        
        TRAPD( err, DoRequestCompletedL( aError ) );
        if ( err != KErrNone ) 
            {
            DLERROR(("DoRequestCompletedL leaved with: %d", err));
            // Symbian error codes
            iObserver->HandleHttpError( *this, TCatalogsHttpError( 
                ECatalogsHttpErrorGeneral, err) );            
            }
        }

    Release();
    DLTRACEOUT((""));    
    }
    
    
// ---------------------------------------------------------------------------
// Request completed
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::DoRequestCompletedL( TInt aError )
    {
    DLTRACEIN( ("Error: %d", aError ) );

    iOwner.CompleteOperation( this );    
    
    if ( ( aError == KErrDisconnected ) && 
         iRetryCount && 
         iState.iProgressState < ECatalogsHttpResponseHeaderReceived )
        {
        DLTRACE(("Retrying"));
        iRetryCount--;
        
        // This ensures that connection is reset for this op
        ReleasePtr( iConnection );
        
        iState.iOperationState = ECatalogsHttpOpCreated;
        iState.iProgressState = ECatalogsHttpNone;
        
        // Start asynchronously so that CCatalogsHttpStack is not deleted
        // inside an MHFRunL-callback
        AsyncStartL();        
        return;
        }

    
    if ( aError == KErrNone ) 
        {        
        iState.iOperationState = ECatalogsHttpOpCompleted;
        iState.iProgressState = ECatalogsHttpDone;
        // Report to the observer
        NotifyObserver();
        
        }
    else 
        {        
        DLTRACE(("Handling error"));
        iState.iOperationState = ECatalogsHttpOpFailed;        
        
        // HandleHttpError would also Cancel the operation but it's
        // already been complete by CompleteOperation
        if ( aError < KErrNone ) 
            {
            UpdateAccessPoint( TCatalogsConnectionMethod() );
            
            // Symbian error codes
            iObserver->HandleHttpError( *this, TCatalogsHttpError( 
                ECatalogsHttpErrorGeneral, aError ) );

            }
        else 
            {
            
            // HTTP errors
            iObserver->HandleHttpError( *this, TCatalogsHttpError( 
                ECatalogsHttpErrorHttp, 
                KCatalogsErrorHttpBase - aError ) );
            }
        }

    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Update accesspoint
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::UpdateAccessPoint( 
    const TCatalogsConnectionMethod& aMethod )
    {
    DLTRACEIN(("AP, type: %d, id: %u, apn: %u", 
        aMethod.iType, aMethod.iId, aMethod.iApnId ));
    
    iConfig->SetConnectionMethod( aMethod );

    DLTRACE(("Setting new default AP"));
        
    // Update default ap to connection manager if it's not set
        
    iOwner.ConnectionManager().SetAccessPointForDefaultConnectionMethod( 
        aMethod );
    
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//	
CCatalogsHttpTransaction::CCatalogsHttpTransaction( 
    MCatalogsHttpTransactionManager& aOwner,
    const TCatalogsTransportOperationId& aId,
    TCatalogsHttpTransactionType aType ) :
    iOwner( aOwner ), iId( aId ), iState( ECatalogsHttpOpCreated,
    ECatalogsHttpNone ),
    iRefCount( 1 ),
    iType( aType ),
    iRetryCount( KTransactionRetries ),
    iStartCallBack( &StartCallBack, this )
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpTransaction::ConstructL( const CCatalogsHttpConfig* aConfig )
    {    
    iResponseBody.Set( KNullDesC8 );
    iResponseHeaders = CCatalogsHttpHeaders::NewL();

    
    // Create a copy of the configuration 
    if ( aConfig ) 
        {
        iConfig = aConfig->CloneL();
        }
    else 
        {
        // Or create a new configuration
        iConfig = CCatalogsHttpConfig::NewL();
        iConfig->SetHttpMethod( ECatalogsHttpPost );
        }        
    }
    

// ---------------------------------------------------------------------------
// Updates the http stack to match the configuration
// ---------------------------------------------------------------------------
//	    
void CCatalogsHttpTransaction::UpdateConfiguration()
    {
    DLTRACEIN( ("" ) );
    DASSERT( iConfig );

    const TCatalogsConnectionMethod& method( iConfig->ConnectionMethod() );
    
    // If no APN id or method id is set, we need to use the default  
    if ( !method.iApnId &&
         !method.iId )
        {        
        iConfig->SetConnectionMethod( 
            iOwner.ConnectionManager().DefaultConnectionMethod() );
        }    
    }
    

// ---------------------------------------------------------------------------
// Updates the request headers to the HTTP stack
// ---------------------------------------------------------------------------
//	        
void CCatalogsHttpTransaction::UpdateRequestHeadersL()
    {
    DLTRACEIN( ("") );
    RPointerArray<CCatalogsKeyValuePair>& headers( 
        iConfig->RequestHeaders().Headers() );
    
    // Clear previously declared headers
    iHttp->ClearHeaders();
       
    // Iterate through headers and add them to the request headers
    // in HTTP stack
    for ( TInt i = 0; i < headers.Count(); ++i )
        {
        iHttp->AddHeaderL( headers[i] );
        }
        
    DLTRACEOUT( ("" ) );
    }


// ---------------------------------------------------------------------------
// Handle HTTP error
// ---------------------------------------------------------------------------
//	            
TBool CCatalogsHttpTransaction::HandleHttpError( TCatalogsHttpErrorType aType, 
    TInt aErr )
    {
    DLTRACE( ( "Error: %d ", aErr ) );
    iState.iOperationState = ECatalogsHttpOpFailed;
    
    // Start processing next transactions        
    iOwner.CompleteOperation( this );

    return iObserver->HandleHttpError( 
        *this, TCatalogsHttpError( aType, aErr ) );
    }


// ---------------------------------------------------------------------------
// Actually starts the transaction
// ---------------------------------------------------------------------------
//	            
void CCatalogsHttpTransaction::DoStartL()
    {
    DLTRACEIN((""));        
    
    DASSERT( iConnection );

    if ( !iOwner.ConnectionManager().AskConnectionConfirmation( 
        Config().ConnectionMethod() ) )
        {
        DLINFO(("Connection denied!"));
        User::Leave( KCatalogsErrorHttpConnectionDenied );
        }

    // If the operation disconnected prematurely it will be restarted
    // so we need to delete the old iHttp
    DeletePtr( iHttp );    
    iHttp = CCatalogsHttpStack::NewL( 
        this,
        *iConnection,
        iOwner.ConnectionCreatorL() );
    
    iHttp->SetConnectionManager( &iOwner.ConnectionManager() );
                
    UpdateRequestHeadersL();    
    const TDesC8* method = &KCatalogsHttpPost;
    // Choose the correct method string
    switch( Config().HttpMethod() ) 
        {
        case ECatalogsHttpGet:
            {
            method = &KCatalogsHttpGet;
            break;
            }
            
        case ECatalogsHttpHead:
            {
            method = &KCatalogsHttpHead;
            }
            
        default:                   
            {
            }                   
        }
    
    iState.iOperationState = ECatalogsHttpOpInProgress;
    iState.iProgressState = ECatalogsHttpStarted;    
    
    if ( iRequestBody ) 
        {
        // Send the request
        iHttp->IssueHttpRequestL( *method, EncodedUri(), ContentType(), 
            *iRequestBody );                    
        }
    else 
        {
        // Send the request with empty body
        iHttp->IssueHttpRequestL( *method, EncodedUri(), ContentType(), 
            KNullDesC8 );
        }        
    
    // Notify observer about operation start    
    // This method can leave so not using NotifyObserver
    iObserver->HandleHttpEventL( *this, iState );    
    
    DLTRACEOUT(("successful"));
    }


void CCatalogsHttpTransaction::NotifyObserver()
    {
    DLTRACEIN((""));
    if ( iObserver ) 
        {        
        TRAPD( err, iObserver->HandleHttpEventL( *this, iState ) );
        if ( err != KErrNone ) 
            {
            DLERROR(("Observer's HandleHttpEventL leaved with: %d", err));
            // Symbian error codes
            iObserver->HandleHttpError( *this, TCatalogsHttpError( 
                ECatalogsHttpErrorGeneral, err) );            
            }
        }
    }
    

void CCatalogsHttpTransaction::SetTransferring( TBool aTransferring )
    {
    if ( iTransferring != aTransferring ) 
        {
        iTransferring = aTransferring;
        iOwner.ReportConnectionStatus( iTransferring );
        }
    }


// ---------------------------------------------------------------------------
// Starts the operation asynchronously
// ---------------------------------------------------------------------------
//              
void CCatalogsHttpTransaction::AsyncStartL()
    {
    DLTRACEIN((""));
    DeletePtr( iCallBack );
    iCallBack = new ( ELeave ) CAsyncCallBack( 
        iStartCallBack, CActive::EPriorityStandard );
    iCallBack->CallBack();
    }


// ---------------------------------------------------------------------------
// Encodes the URI
// ---------------------------------------------------------------------------
//              
void CCatalogsHttpTransaction::EncodeUriL()
    {
    DeletePtr( iEncodedUri );
    iEncodedUri = CatalogsHttpUtils::EncodeUriL( Uri() );
    }


// ---------------------------------------------------------------------------
// Returns the encoded URI
// ---------------------------------------------------------------------------
//              
const TDesC8& CCatalogsHttpTransaction::EncodedUri() const
    {
    DASSERT( iEncodedUri );
    return *iEncodedUri;
    }

