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
* Description:   Implementation of CCatalogsHttpStack
*
*/


#include "catalogshttpstack.h"

#include <uri8.h>
#include <http.h>
#include <chttpformencoder.h>
#include <httpstringconstants.h>
#include <http/rhttptransaction.h>
#include <http/rhttpsession.h>
#include <http/rhttpheaders.h>
#include <es_enum.h>
#include <in_sock.h>


// HTTP headers

#include <httpfilteracceptheaderinterface.h>
#include <uaproffilter_interface.h>
#include <deflatefilterinterface.h>
#include <httpfiltercommonstringsext.h>
#include <httpfilterproxyinterface.h>
#include <cookiefilterinterface.h>
    
#include "catalogs_device_config.h"
#include "catalogshttptypes.h"
#include "catalogshttpconnectioncreator.h"
#include "catalogshttpsessionmanagerimpl.h"
#include "catalogsnetworkmanager.h"
#include "catalogserrors.h"
#include "catalogsconnectionmethod.h"
#include "catalogshttputils.h"
#include "catalogsconnection.h"
#include "catalogsconstants.h"

#include "catalogsdebug.h"

// Uncomment this to enable CHttpDeflateFilter in ARM-builds (accepts zipped input)
#define ENABLE_DEFLATE_FILTER


// ----------------------------------------------------------------------------
// CCatalogsHttpStack::NewL()
//
// Creates instance of CCatalogsHttpStack.
// ----------------------------------------------------------------------------
CCatalogsHttpStack* CCatalogsHttpStack::NewL( 
    MCatalogsHttpStackObserver* aObserver,
    CCatalogsConnection& aConnection,
    CCatalogsHttpConnectionCreator& aConnectionCreator )
    {
    CCatalogsHttpStack* self = CCatalogsHttpStack::NewLC( 
        aObserver, 
        aConnection,
        aConnectionCreator );
    CleanupStack::Pop( self );
    return self;
    }


// ----------------------------------------------------------------------------
// CCatalogsHttpStack::NewLC()
//
// Creates instance of CCatalogsHttpStack.
// ----------------------------------------------------------------------------
CCatalogsHttpStack* CCatalogsHttpStack::NewLC( 
    MCatalogsHttpStackObserver* aObserver,
    CCatalogsConnection& aConnection,
    CCatalogsHttpConnectionCreator& aConnectionCreator )
    {
    CCatalogsHttpStack* self = new (ELeave) CCatalogsHttpStack( 
        aObserver, 
        aConnection,
        aConnectionCreator );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ----------------------------------------------------------------------------
// CCatalogsHttpStack::CCatalogsHttpStack()
//
// First phase constructor.
// ----------------------------------------------------------------------------
CCatalogsHttpStack::CCatalogsHttpStack( 
    MCatalogsHttpStackObserver* aObserver,
    CCatalogsConnection& aConnection,
    CCatalogsHttpConnectionCreator& aConnectionCreator ) :
    CActive( EPriorityNormal ),
    iTable( RHTTPSession::GetTable() ),
    iObserver( aObserver ),
    iConnection( aConnection ),
    iConnectionCreator( aConnectionCreator )
    {
    CActiveScheduler::Add( this );
    }


// ----------------------------------------------------------------------------
// CCatalogsHttpStack::~CCatalogsHttpStack()
//
// Destructor.
// ----------------------------------------------------------------------------
CCatalogsHttpStack::~CCatalogsHttpStack()
    {
    DLTRACEIN( ( "" ) );
    CancelTransaction();        
    
    iTimer.Close();
    iSession.Close();  // closes also iTransaction if necessary   
    delete iPostData;
    iRunning = EFalse;
    // The headers are deleted when the transaction configuration is deleted
    iUserHeaders.Reset();
    DLTRACEOUT( ( "" ) );
    }


// ----------------------------------------------------------------------------
// CCatalogsHttpStack::ConstructL()
//
// Second phase construction.
// ----------------------------------------------------------------------------
void CCatalogsHttpStack::ConstructL()
    {
    DLTRACE( ( "CCatalogsHttpStack::ConstructL(): %X", this ) );

    iConnectionOpened = ETrue;
    
    // Open RHTTPSession with default protocol ( "HTTP/TCP" )    
    // Most common error for leaves: no access point configured, 
    // and session creation
    // leaves with KErrNotFound.    

    iSession.OpenL();
    iPool = iSession.StringPool();

    // Initialize the HTTP to use the created socket server. Without these
    // settings, the HTTP protocol would open a connection of its own; this is not
    // desired, because the GPRS connection would then be broken after every query.

    RHTTPConnectionInfo info = iSession.ConnectionInfo();

    info.SetPropertyL( iPool.StringF( HTTP::EHttpSocketServ, iTable ),
                       iConnection.SocketServer().Handle() );

    info.SetPropertyL( iPool.StringF( HTTP::EHttpSocketConnection, iTable ),
                       ( TInt )&iConnection.Connection() );

    // Open tables to enable HTTP filters
    iPool.OpenL( HttpFilterCommonStringsExt::GetTable() );
    iPool.OpenL( HttpFilterCommonStringsExt::GetLanguageTable() );


    // Install this class as the callback for authentication requests. When 
    // page requires authentication the framework calls GetCredentialsL to get 
    // user name and password.
    InstallAuthenticationL( iSession );


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Note: these filters do not seem to work on the E60 phone
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // Install accept filters
    // This crashes on 3.0 week 42 and earlier
    DLTRACE(( "->installing AcceptHeader filter" ));
    TRAPD( err, CHttpFilterAcceptHeaderInterface::InstallFilterL( iSession ) );
    if ( err != KErrNone )
        {
        DLTRACE(("->CHttpFilterAcceptHeaderInterface::InstallFilterL() ERROR: %d", 
            err ) );
        }


// Don't install UAprof filter if user agent is overridden
#ifndef CATALOGS_OVERRIDE_USERAGENT
    
    // This does not crash on 3.0. Wonders! 
    // Install UAprof filters, ignore errors
    DLTRACE(( "->installing UAProf filter" ));
    TRAP( err, CHttpUAProfFilterInterface::InstallFilterL( iSession ) );
    if ( err != KErrNone )
        {
        
        DLTRACE( ("->CHttpUAProfFilterInterface::InstallFilterL() ERROR: %d", 
            err ) );
        }
#endif
// This is available on 3.0 but crashes on E60 wk40 and earlier.
    

#ifdef ENABLE_DEFLATE_FILTER
    // Accept zipped input
    DLTRACE( (_L( "->installing deflate filter" ) ) );
    TRAP( err, CHttpDeflateFilter::InstallFilterL( iSession ) );
    if ( err != KErrNone )
        {
        DLTRACE(("->CHttpDeflateFilter::InstallFilterL() ERROR: %d", err ));
        }
#endif // ENABLE_DEFLATE_FILTER


    // Install proxy filter, ignore errors
    DLTRACE( ( "->installing proxy filter" ) );
    TRAP( err, CHttpFilterProxyInterface::InstallFilterL( iSession ) );
    if ( err != KErrNone )
        {
        DLTRACE( ("CHttpFilterProxyInterface::InstallFilterL() ERROR: %d", err ) );
        }    

    // Install cookie filter, ignore errors
    DLTRACE( ( "->installing cookie filter" ) );
    TRAP( err, CHttpCookieFilter::InstallFilterL( iSession ) );
    if ( err != KErrNone )
        {
        DLTRACE( ("CHttpCookieFilter::InstallFilterL() ERROR: %d", err ) );
        }
    
    User::LeaveIfError( iTimer.CreateLocal() );

    DLTRACE( ("->CCatalogsHttpStack::ConstructL() ok") );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpStack::SetConnectionMethodL( 
    const TCatalogsConnectionMethod& aMethod )
    {
    DLTRACEIN((("ap type: %d id: %u"), aMethod.iType, aMethod.iId ));
    
    iConnectionMethod = aMethod;        
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint32 CCatalogsHttpStack::AccessPointID() const
    {
    return iConnectionMethod.iApnId;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpStack::SetObserver( MCatalogsHttpStackObserver* aObserver )
    {
    iObserver = aObserver;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpStack::SetConnectionManager( 
    CCatalogsHttpConnectionManager* aManager )
    {
    iConnectionManager = aManager;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpStack::ClearHeaders()
    {
    DLTRACEIN((""));    
    // iUserHeaders doesn't own the headers
    iUserHeaders.Reset();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpStack::AddHeaderL( const CCatalogsKeyValuePair* aPair )
    {
    DLTRACEIN((""));
    User::LeaveIfError( iUserHeaders.Append( aPair ) );
    }


// ----------------------------------------------------------------------------
// CCatalogsHttpStack::SetHeaderL()
//
// Used to set header value to HTTP request
// ----------------------------------------------------------------------------
void CCatalogsHttpStack::SetHeaderL( RHTTPHeaders aHeaders, 
                                     TInt aHdrField, 
                                     const TDesC8& aHdrValue )
    {
    DLTRACEIN((""));
    RStringF valStr = iSession.StringPool().OpenFStringL( aHdrValue );
    CleanupClosePushL( valStr );

    THTTPHdrVal val( valStr );

    aHeaders.SetFieldL( 
        iSession.StringPool().StringF( aHdrField, iTable ), val );

    CleanupStack::PopAndDestroy( &valStr );
    }


// ----------------------------------------------------------------------------
// CCatalogsHttpStack::IssueHTTPRequestL()
//
// Start a new HTTP transaction.
// ----------------------------------------------------------------------------
void CCatalogsHttpStack::IssueHttpRequestL( const TDesC8& aMethod,
                                       const TDesC8& aURI,
                                       const TDesC8& aContentType,
                                       const TDesC8& aBody )
    {
    DLTRACEIN( ( "body size: %d, this: %X", 
        aBody.Size(), this ) );
    
    DLINFO( ( "-> URI: %S", &aURI ) );
    
    // Parse string to URI
    TUriParser8 uri; 
    uri.Parse( aURI );

    // Using post as default HTTP method
    RStringF method;
    if ( aMethod.Length() == 0 )
        {
        // Get request method string for HTTP POST
        method = iSession.StringPool().StringF( HTTP::EPOST, iTable );
        }
    else
        {
        method = iSession.StringPool().OpenFStringL( aMethod );
        }
    
    CleanupClosePushL( method );
    iTransaction = iSession.OpenTransactionL( uri, *this, method );
    CleanupStack::PopAndDestroy( &method );
    
    RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
    
    TInt count = iUserHeaders.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CCatalogsKeyValuePair* pair = iUserHeaders[i];

        RStringF headerStr = iSession.StringPool().OpenFStringL( pair->Key() );
        CleanupClosePushL( headerStr );
        
        RStringF valueStr = iSession.StringPool().OpenFStringL( pair->Value() );
        CleanupClosePushL( valueStr );
        
        DLTRACE(("Header: %S, data: %S", &headerStr.DesC(), &valueStr.DesC() ));
        hdr.SetFieldL( headerStr, valueStr );

        CleanupStack::PopAndDestroy( 2, &headerStr ); // headerStr, valueStr

        }

    // Copy data to be posted into member variable; iPostData is used later in
    // methods inherited from MHTTPDataSupplier.
    delete iPostData;
    iPostData = 0;
    iPostData = aBody.AllocL();
    

#ifdef CATALOGS_OVERRIDE_USERAGENT

    // Override user agent string
    DLINFO(("Overriding user agent string with: %S", &KCatalogsOverrideUserAgent ));
    SetHeaderL( hdr, HTTP::EUserAgent, KCatalogsOverrideUserAgent );

#endif    

    if ( aBody.Length() > 0 )
        {
        if ( aContentType.Length() ) 
            {            
            SetHeaderL( hdr, HTTP::EContentType, aContentType );
            }
        else 
            // default to "text/xml"
            // Content-type must be sent in POST requests because some
            // proxies will reject the request if it's not present and doesn't
            // contain something
            {
            SetHeaderL( hdr, HTTP::EContentType, KMimeTypeXml );
            }

        // Set this class as an data supplier. Inherited MHTTPDataSupplier methods
        // are called when framework needs to send body data.
        MHTTPDataSupplier* dataSupplier = this;
        iTransaction.Request().SetBody( *dataSupplier );
        }
    
    iRunning = ETrue;

    iTransaction.SubmitL();
    if ( iObserver ) 
        {
        iObserver->RequestSubmitted();
        }
        
    SetTimeoutTimer();

    }


// ----------------------------------------------------------------------------
// CCatalogsHttpStack::CancelTransaction()
//
// Cancels currently running transaction and frees resources related to it.
// ----------------------------------------------------------------------------
void CCatalogsHttpStack::CancelTransaction()
    {
    DLTRACEIN( ("iActive: %d, iState: %d, this: %X", 
        IsActive(), 
        TInt( iState ), 
        this ) ); // cast required for armcc
    
    // Close() also cancels transaction ( Cancel() can also be used but 
    // resources allocated by transaction must be still freed with Close())
    if ( iRunning )
        {
        iTransaction.Close();

        // Not running anymore
        iRunning = EFalse;
        }

    Cancel();

    iState = ENone;
    }


// ----------------------------------------------------------------------------
// CCatalogsHttpStack::MHFRunL()
//
// Inherited from MHTTPTransactionCallback
// Called by framework to pass transaction events.
// ----------------------------------------------------------------------------
void CCatalogsHttpStack::MHFRunL( RHTTPTransaction aTransaction,
                             const THTTPEvent& aEvent )
    {
    DLTRACEIN( ( "CCatalogsHttpStack::MHFRunL(), event: %d, this: %X", 
        aEvent.iStatus, this ) );
    
    // Cancel timeout
    CancelTimeoutTimer();

    switch ( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseHeaders:
            {
            // HTTP response headers have been received. Use
            // aTransaction.Response() to get the response. However, it's not
            // necessary to do anything with the response when this event occurs.

            // Get HTTP status code from header
            RHTTPResponse resp = aTransaction.Response();
            iLastError = resp.StatusCode();

            if ( !iObserver ) 
                {      
                DLTRACEOUT(("No observer"));
                return;          
                }
                        
            // Report response status code
            if ( iObserver->ResponseReceived( iLastError, resp.StatusText().DesC() ) ) 
                {
                return;
                }

            // what to do in case of redirect etc.?
            switch ( iLastError )
                {
                case 100:   // Continue
                case 101:   // Switching Protocols
                    {
                    // Informational
                    break;
                    }
                
                case 200:   // OK
                case 201:   // Created
                case 202:   // Accepted
                case 203:   // Non-Authoritative Information
                case 204:   // No Content
                case 205:   // Reset Content
                case 206:   // Partial Content
                    {
                    // Success
                    break;
                    }
                
                case 300:  // Multiple Choices   
                case 301:  // Moved Permanently  
                case 302:  // Found              
                case 303:  // See Other          
                case 304:  // Not Modified       
                case 305:  // Use Proxy          
                case 306:  // ( Unused )           
                case 307:  // Temporary Redirect 
                    {
                    break;
                    }

                case 400:   // Bad Request
                case 401:   // Unauthorized
                case 402:   // Payment Required
                case 403:   // Forbidden
                case 404:   // Not Found
                case 405:   // Method Not Allowed
                case 406:   // Not Acceptable
                case 407:   // Proxy Authentication Required
                case 408:   // Request Timeout
                case 409:   // Conflict
                case 410:   // Gone
                case 411:   // Length Required
                case 412:   // Precondition Failed
                case 413:   // Request Entity Too Large
                case 414:   // Request-URI Too Long
                case 415:   // Unsupported Media Type
                case 416:   // Requested Range Not Satisfiable
                case 417:   // Expectation Failed
                    {
                    // Client error
                    break;
                    }
                case 500:   // Internal Server Error
                case 501:   // Not Implemented
                case 502:   // Bad Gateway
                case 503:   // Service Unavailable
                case 504:   // Gateway Timeout
                case 505:   // HTTP Version Not Supported 
                    {
                    // Server error
                    break;
                    }
                }

            // Parse response headers
            RHTTPHeaders headers = resp.GetHeaderCollection();
            THTTPHdrFieldIter iter = headers.Fields();

            RStringTokenF name = iter();
            while ( !name.IsNull() )
                {
                DLTRACE(( "Start adding next header" ));
                // Header
                RStringF stringF;
                stringF = iSession.StringPool().StringF( name );
                const TDesC8& header = stringF.DesC();

                DLTRACE( ( "Header: %S" , &header ) );
                
                TPtrC8 rawField( KNullDesC8 );
                TInt err2 = headers.GetRawField( stringF, rawField );
                
                if ( err2 == KErrNone ) 
                    {                    
                    DLTRACE( ( "Raw value: %S", &rawField ) );
                    iObserver->ResponseHeaderReceived( header, rawField );
                    }                

                if ( header.CompareF( KCatalogsHeaderErrorNumber ) == 0 )
                    {
                    iLastError = KCatalogsErrorHeaderPresent;
                    }
                ++iter;
                name = iter();
                DLTRACE(( "Added header" ));    
                }
            DLTRACE(( "EGotResponseHeaders done" ));
            }
            break;
            

        case THTTPEvent::EGotResponseBodyData:
            {
            // Part ( or all ) of response's body data received. Use 
            // aTransaction.Response().Body()->GetNextDataPart() to get the actual
            // body data.

            DLTRACE(("Got response body"));
            // Get the body data supplier
            iBody = aTransaction.Response().Body();

            if ( !iObserver ) 
                {
                DLTRACEOUT(("No observer. Releasing data"));
                iBody->ReleaseData();
                iBody = NULL;
                return;
                }

            switch ( iLastError )
                {
                /// Add further study what codes are acceptable
                case 200:   // OK
                case 201:   // Created
                case 202:   // Accepted
                case 203:   // Non-Authoritative Information
                case 204:   // No Content
                case 205:   // Reset Content
                case 206:   // Partial Content
                    {

                    // GetNextDataPart() returns ETrue, if the received part is the last 
                    // one.
                    TPtrC8 dataChunk;
                    iBody->GetNextDataPart( dataChunk );
                    TBool releaseData = iObserver->ResponseBodyReceived( dataChunk );


                    // Check if still processing the request
                    // Not running if the client cancelled the transaction
                    if ( iRunning )
                        {
                        iBody->ReleaseData();
                        iBody = 0;
                        }

                    // Issue timer for timeout handling
                    SetTimeoutTimer();

                    break;
                    }
                default:
                    {
                    // Just release the data, not interested in the error messages
                    // Should there be a separate callback for these?
                    iBody->ReleaseData();
                    iBody = 0;
                    break;
                    }
                }
            }
            break;

        case THTTPEvent::EResponseComplete:
            {
            // Indicates that header & body of response is completely received.
            // No further action here needed.
            DLTRACE(("Response complete"));
            }
            break;

        case THTTPEvent::ESucceeded:
            {
            DLTRACE( ( "ESucceeded" ) );
            // Transaction can be closed now. It's not needed anymore.
            aTransaction.Close();
            iRunning = EFalse;
            if ( iObserver ) 
                {
                
                if ( iLastError == KCatalogsErrorHeaderPresent )
                    {
                    iObserver->RequestCompleted( KErrUnknown );
                    }
                else
                    {
                    iObserver->RequestCompleted( KErrNone );
                    }
                }
            }
            break;

        case THTTPEvent::EFailed:
            {
            DLTRACE( ( "EFailed" ) );
            // Transaction completed with failure.             
            
            aTransaction.Close();    
            iRunning = EFalse;
            
            if ( iObserver ) 
                {                
                iObserver->RequestCompleted( iLastError );
                }
            }

            break;

        case THTTPEvent::EGotResponseTrailerHeaders:
        case THTTPEvent::EMoreDataReceivedThanExpected:
    
        case THTTPEvent::ERedirectedTemporarily:
        case THTTPEvent::ERedirectedPermanently:
            // Should the client change it's URI?

        case THTTPEvent::ERedirectRequiresConfirmation:
            // ?

        case THTTPEvent::ETooMuchRequestData:
        case THTTPEvent::EUnrecoverableError:
            // ?

        default:
            // There are more events in THTTPEvent, but they are not usually 
            // needed. However, event status smaller than zero should be handled 
            // correctly since it's an error.
            {
            if ( aEvent.iStatus < 0 )
                {
                DLTRACE(( "-> ERROR: %d", aEvent.iStatus ) );
                
                // Just close the transaction on errors
                aTransaction.Close();
                iRunning = EFalse;
                
                if ( iObserver ) 
                    {                    
                    iObserver->RequestCompleted( aEvent.iStatus );
                    }
                }
            else if ( aEvent == THTTPEvent::ERedirectRequiresConfirmation )
                {
                DLTRACE( ("-> Redirection confirmation requested, confirming..." ) );
                
                TRAPD( err, iTransaction.SubmitL() );

                if ( err == KErrNone && iObserver ) 
                    {
                    iObserver->RequestSubmitted();
                    }
                else if ( err != KErrNone )
                    {
                    DLTRACE( ( "-> Error resubmitting the request: %d", err ) );

                    // Just close the transaction on errors
                    aTransaction.Close();
                    iRunning = EFalse;
                    if ( iObserver ) 
                        {                        
                        iObserver->RequestCompleted( aEvent.iStatus );
                        }
                    }
                }
            else
                {
                DLTRACE(( "-> UNHANDLED EVENT: %d", aEvent.iStatus ));

                // Other events are not errors (e.g. permanent and temporary
                // redirections )
                }
            }
            break;
        }
    }


// ----------------------------------------------------------------------------
// CCatalogsHttpStack::MHFRunError()
//
// Inherited from MHTTPTransactionCallback
// Called by framework when *leave* occurs in handling of transaction event.
// These errors must be handled, or otherwise HTTP-CORE 6 panic is thrown.
// ----------------------------------------------------------------------------
TInt CCatalogsHttpStack::MHFRunError( TInt aError, 
                                 RHTTPTransaction /*aTransaction*/, 
                                 const THTTPEvent& /*aEvent*/ )
    {
    DLTRACEIN( ( "CCatalogsHttpStack::MHFRunError( %d )", aError ) ); 
    
    // Just notify about the error and return KErrNone.
    if ( iObserver ) 
        {        
        iObserver->RequestCompleted( aError );
        }

    return KErrNone;
    }


// ----------------------------------------------------------------------------
// CCatalogsHttpStack::GetNextDataPart()
//
// Inherited from MHTTPDataSupplier
// Called by framework when next part of the body is needed. In this example 
// this provides data for HTTP post.
// ----------------------------------------------------------------------------
TBool CCatalogsHttpStack::GetNextDataPart( TPtrC8& aDataPart )
    {
    DLTRACEIN( ("CCatalogsHttpStack::GetNextDataPart()") );
    
    if ( iPostData )
        {
        // Provide pointer to next chunk of data ( return ETrue, if last chunk )
        // Usually only one chunk is needed, but sending big file could require
        // loading the file in small parts.

        // body in multiple parts?
        aDataPart.Set( iPostData->Des() );
        }
    return ETrue;
    }


// ----------------------------------------------------------------------------
// CCatalogsHttpStack::ReleaseData()
//
// Inherited from MHTTPDataSupplier
// Called by framework. Allows us to release resources needed for previous
// chunk. (e.g. free buffers )
// ----------------------------------------------------------------------------
void CCatalogsHttpStack::ReleaseData()
    {
    DLTRACE(( "CCatalogsHttpStack::ReleaseData()" ));
    
    // When sending data in multiple parts we must notify the framework here:
    // ( the framework can call GetNextDataPart() when we notify it by calling
    //  NotifyNewRequestBodyPartL())
    
    // Doesn't anything since we send only one part and that is released in
    // the destructor and also before we send a new request
    }


// ----------------------------------------------------------------------------
// CCatalogsHttpStack::Reset()
//
// Inherited from MHTTPDataSupplier
// Called by framework to reset the data supplier. Indicates to the data 
// supplier that it should return to the first part of the data.    
// In practise an error has occured while sending data, and framework needs to 
// resend data.
// ----------------------------------------------------------------------------
TInt CCatalogsHttpStack::Reset()
    {
    DLTRACE(( "CCatalogsHttpStack::Reset()" ));
    
    // Nothing needed since iPostData still exists and contains all the data. 
    // ( If a file is used and read in small parts we should seek to beginning 
    // of file and provide the first chunk again in GetNextDataPart() )
    return KErrNone;
    }


// ----------------------------------------------------------------------------
// CCatalogsHttpStack::OverallDataSize()
//
// Inherited from MHTTPDataSupplier
// Called by framework. We should return the expected size of data to be sent.
// If it's not know we can return KErrNotFound ( it's allowed and does not cause
// problems, since HTTP protocol allows multipart bodys without exact content 
// length in header ).
// ----------------------------------------------------------------------------
TInt CCatalogsHttpStack::OverallDataSize()
    {
    DLTRACEIN(("this-ptr: %X", this));
    if ( iPostData )
        {
        DLTRACE( ( "CCatalogsHttpStack::OverallDataSize(), data size: %d", 
            iPostData->Length() ) );
        
        return iPostData->Length();
        }
    else
        {
        DLTRACE( ( "CCatalogsHttpStack::OverallDataSize(), ERROR, no body" ) );

        return KErrNotFound;
        }
    }

// ----------------------------------------------------------------------------
// CCatalogsHttpStack::GetCredentialsL()
//
// Inherited from MHTTPAuthenticationCallback
// Called by framework when we requested authenticated page and framework 
// needs to know username and password.
// ----------------------------------------------------------------------------
TBool CCatalogsHttpStack::GetCredentialsL( const TUriC8& /*aURI*/,
                                      RString /*aRealm*/, 
                                      RStringF /*aAuthenticationType*/,
                                      RString& /*aUsername*/, 
                                      RString& /*aPassword*/ )
    {
    DLTRACE( ( "CCatalogsHttpStack::GetCredentialsL()" ) );
    
    // aURI, aRealm and aAuthenticationType are informational only. We only need 
    // to set aUsername and aPassword and return ETrue, if aUsername and 
    // aPassword are provided by user.
    return EFalse;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpStack::ReleaseBody()
    {
    // If a received body part exists, release it
    if ( iBody )
        {
        iBody->ReleaseData();
        iBody = 0;
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpStack::RunL()
    {
    DLTRACEIN( ( "iState: %d, iStatus: %d, this: %X",
                     TInt( iState ), // cast required for armcc
                     iStatus.Int(),
                     this ) );

    switch ( iState )
        {
        case ENone:
            {
            break;
            }

        case ETimeout:
            {
            DLTRACE(("Timeout"));
            if ( iStatus != KErrCancel )
                {                
                DLTRACE(( "Request timed out, cancelling" ));
                // Request has timed out, close transaction
                CancelTransaction();

                DLERROR(("Couldn't reach the host. Reset AP"));
                iConnectionMethod.iApnId = 0;
                    
                if ( iObserver ) 
                    {                    
                    iObserver->UpdateAccessPoint( iConnectionMethod );
                    iObserver->RequestCompleted( KErrTimedOut );
                    }
                }
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpStack::DoCancel()
    {
    DLTRACEIN(( "iStatus != KRequestPending: %d", 
        iStatus != KRequestPending  ));
    
    switch ( iState )
        {
        case ETimeout:
            {
            DLTRACE( ( "ETimeout" ) );
            iTimer.Cancel();
            break;
            }
            
            
        default:
            {
            // Crash fix: instead of Stop(), call Close() and then Open().
            // When calling Stop() in early phases of connection building
            // the phone will crash to white screen. ESock IP will crash first.
            // Close()+Open() circumvents and seems to work.
            break;
            }
        }
    DASSERT( iStatus != KRequestPending );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpStack::SetTimeoutTimer()
    {
    Cancel();
    iTimer.After( iStatus, KRequestTimeoutMicroseconds );

    iState = ETimeout;

    SetActive();
    DASSERT( iStatus.Int() == KRequestPending );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpStack::CancelTimeoutTimer()
    {
    Cancel();
    }
