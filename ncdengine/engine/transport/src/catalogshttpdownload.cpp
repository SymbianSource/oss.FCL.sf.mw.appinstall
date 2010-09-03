/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "catalogshttpdownload.h"

#include <bautils.h>
#include <e32err.h>
#include <apmstd.h> // TDataType
#include <escapeutils.h>
#include <downloadmanager.h>
#include <cmmanager.h>
#include <cmdestination.h>
#include <uri8.h> 
#include <QtGlobal>
#include <QStringList>
#include <xqconversions.h>

#include "catalogshttpdownloadmanagerimpl.h"
#include "catalogshttpobserver.h"
#include "catalogshttpconfigimpl.h"
#include "catalogskeyvaluepair.h"
#include "catalogshttpheadersimpl.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "catalogsconstants.h"
#include "catalogserrors.h"
#include "catalogshttpconnectionmanager.h"
#include "catalogsconnectionmethod.h"
#include "catalogsconnection.h"
#include "catalogshttputils.h"

// Number of retries before giving up the download
const TInt KReconnectAttempts = 3;

const TInt KHttpErrorStatus = 400;
const TInt KGenericSymbianHttpError = -20000;

#define CATALOGS_DISABLE_DOWNLOAD_RESPONSE_HEADERS

// HTTP header strings
_LIT8( KHttpRequestAcceptHeader, "Accept" );
_LIT8( KHttpRequestAcceptCharSetHeader, "Accept-Charset" );
_LIT8( KHttpRequestAcceptLanguageHeader, "Accept-Language" );
_LIT8( KHttpRequestExpectHeader, "Expect" );
_LIT8( KHttpRequestFromHeader, "From" );
_LIT8( KHttpRequestHostHeader, "Host" );
_LIT8( KHttpRequestMaxForwardsHeader, "Max-Forwards" );
_LIT8( KHttpRequestPragmaHeader, "Pragma" );
_LIT8( KHttpRequestRefererHeader, "Referer" );
_LIT8( KHttpRequestUserAgentHeader, "User-Agent" );
_LIT8( KHttpRequestVaryHeader, "Vary" );

_LIT8( KHttpGeneralCacheControlHeader, "Cache-Control" );
_LIT8( KHttpGeneralDateHeader, "Date" );
_LIT8( KHttpGeneralPragmaHeader, "Pragma" );
_LIT8( KHttpGeneralViaHeader, "Via" );
_LIT8( KHttpGeneralWarningHeader, "Warning" );


_LIT8( KHttpEntityAllowHeader, "Allow" );
_LIT8( KHttpEntityContentEncodingHeader, "Content-Encoding" );
_LIT8( KHttpEntityContentLanguageHeader, "Content-Language" );
_LIT8( KHttpEntityContentLocationHeader, "Content-Location" );
_LIT8( KHttpEntityExpiresHeader, "Expires" );
_LIT8( KHttpEntityLastModifiedHeader, "Last-Modified" );


#ifndef CATALOGS_DISABLE_DOWNLOAD_RESPONSE_HEADERS
    _LIT8( KHttpResponseCharSet, "Charset" );
    _LIT8( KHttpResponseAge, "Age" );
    _LIT8( KHttpResponseETag, "ETag" );
    _LIT8( KHttpResponseLocation, "Location" );
    _LIT8( KHttpResponseRetryAfter, "Retry-After" );
    _LIT8( KHttpResponseServer, "Server" );
    _LIT8( KHttpResponseVary, "Vary" );
#endif    



// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	
CCatalogsHttpDownload* CCatalogsHttpDownload::NewLC( 
    CCatalogsHttpDownloadManager& aOwner, 
    Download* aDownload,
    const CCatalogsHttpConfig& aConfig )
    {
    CCatalogsHttpDownload* self = new( ELeave ) CCatalogsHttpDownload( 
        aOwner,
        aDownload );
        
    CleanupStack::PushL( self );
    self->ConstructL( &aConfig );    
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//	
CCatalogsHttpDownload::~CCatalogsHttpDownload()
    {    
    DLTRACEIN(( "this-ptr: %X", this ));

    SetTransferring( EFalse );
        
    if ( iTransaction ) 
        {
        DLTRACE(("Cancelling transaction"));
        iTransaction->Cancel();
        }
    
    // Normally, iDownload is not deleted since it would also remove the
    // incomplete files etc.
    if ( iDownload )     
        {
        
        DLTRACE(("RHttpDownload-ptr: %x", iDownload ));
        if ( iNormalDelete && 
             iState.iOperationState != ECatalogsHttpOpCompleted &&
             iState.iOperationState != ECatalogsHttpOpFailed ) 
            {
            DLTRACE(("Setting download as paused"));
            SetDeleteState( EDownloadPaused );
            }
        else 
            {
            DLTRACE(( "Deleting download" ));
             iOwner.GetDownloadManager()->removeOne(iDownload);                      
            
            if ( iState.iOperationState != ECatalogsHttpOpCompleted ) 
                {
                // Ensure that temp files are really deleted
                DeleteFiles();
                }
            }                   
        }
    

    ReleasePtr( iConnection );
    
    DLTRACE(( "Removing from owner" ));
    if ( iState.iOperationState != ECatalogsHttpOpCompleted &&
         iState.iOperationState != ECatalogsHttpOpFailed ) 
        {
        iOwner.CompleteOperation( this );
        }
    else if ( iState.iOperationState == ECatalogsHttpOpFailed
        && iState.iProgressState != ECatalogsHttpDone )
        {
        iOwner.CompleteOperation( this );
        }
    iOwner.RemoveDownload( this );        
    
            
    delete iConfig;
    delete iResponseHeaders;
    delete iAddedRequestHeaders;   
    delete iUri;
    delete iEncodedUri;
    delete iTempFilename;
    iDdFile.Close();
    }


// ---------------------------------------------------------------------------
// Add a reference
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpDownload::AddRef()
    {
    iRefCount++;
    return iRefCount;    
    }


// ---------------------------------------------------------------------------
// Release a reference
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpDownload::Release()
    {
    DLTRACEIN(( "" ));
    iRefCount--;
    if ( !iRefCount ) 
        {
        // Ensures that iDownload is not deleted and therefore paused/ongoing
        // downloads will be handled as determined by the quit operation
        iNormalDelete = ETrue;
        delete this;
        return 0;
        }
    return iRefCount;
    }
        

// ---------------------------------------------------------------------------
// Reference count
// ---------------------------------------------------------------------------
//        
TInt CCatalogsHttpDownload::RefCount() const
    {
    return iRefCount;
    }


// ---------------------------------------------------------------------------
// Cancel download
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpDownload::Cancel()
    {
    DLTRACEIN( ( "this: %x, ID: %d, Operation state: %i, Progress state: %d",
        this, iId.Id(), 
        iState.iOperationState, iState.iProgressState ) );
    
    iTransferredSize = 0;
    if ( iTransaction ) 
        {
        iTransaction->Cancel();
        iTransaction = NULL;                        
        }

#ifdef __SERIES60_31__
    
    // Have to pause downloads before deleting. Otherwise progress events
    // will stop coming because DL manager's event queue gets stuck in 3.1.
    iCancelled = ETrue;
    if ( iDownload )
        {
        iObserver = NULL;
        TInt32 state = 0;

         state = (Download::State)iDownload->attribute(State).toInt();
        if ( IsOneOf( 
                static_cast<THttpDownloadState>( state ), 
                EHttpDlCreated,
                EHttpDlPaused,
                EHttpDlCompleted,
                EHttpDlFailed ) )
            {
            DLTRACE(("Deleting download"));
            DeletePlatformDownload();
            }
        else
            {
            DLTRACE(("Pausing download"));
            iDownload->Pause();
            return RefCount();
            }
        }

    DLTRACE(("No iDownload"));
    ReleasePtr( iConnection );
    
    SetTransferring( EFalse );
    DeleteFiles();
    
    iOwner.CompleteOperation( this );
    iState.iOperationState = ECatalogsHttpOpDeleting;
    iState.iProgressState = ECatalogsHttpDone;

    return Release();                    
    
#else    
    
    // DLMAIN-546, if the download was completed before cancel
    // was executed then DL manager's download has already been deleted
    DeletePlatformDownload();
        
    ReleasePtr( iConnection );
    
    SetTransferring( EFalse );
    DeleteFiles();
    
    // call CancelOperation only if the download had been started (or at least
    // tried)
    if ( iState.iProgressState != ECatalogsHttpNone && 
         iState.iProgressState != ECatalogsHttpDone ) 
        {     
        DLTRACE(("Cancelling the operation"));                   
        iOwner.CompleteOperation( this );
        iState.iOperationState = ECatalogsHttpOpDeleting;
        iState.iProgressState = ECatalogsHttpDone;
        }
    return Release();
#endif    
    }


// ---------------------------------------------------------------------------
// Download progress
// ---------------------------------------------------------------------------
//	
TCatalogsTransportProgress CCatalogsHttpDownload::Progress() const
    {
    if ( iState.iOperationState == ECatalogsHttpOpInProgress ) 
        {
        TInt32 contentSize = ContentSize();
        if ( !contentSize ) 
            {
            // Prevent division by zero problems
            contentSize = 1;
            }
        return TCatalogsTransportProgress( iState.iOperationState, TransferredSize(), 
            contentSize );
        }

    return TCatalogsTransportProgress( iState.iOperationState, 0, 1 );
    }


// ---------------------------------------------------------------------------
// Start download
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpDownload::Start( TResumeStartInformation aResumeOrigin )
    {
    DLTRACEIN(( "op state: %d, prog state: %d", iState.iOperationState,
        iState.iProgressState ));    
    
    TInt err = KErrNone;
    if ( iState.iProgressState == ECatalogsHttpNone ) 
        {
        DLTRACE(("Updating dl config"));
        TRAP( err, UpdateDownloadConfigurationL() );
        if ( err != KErrNone ) 
            {
            return err;
            }
        }    
        
    if ( aResumeOrigin == EAutomaticResume && iPaused )
        {
        // Download is continued automatically but
        // it is paused, and we do not resume it
        }
    else if( iPausePending )
        {
        // Pausing is an asynchronous operation therefore we need to enqueue
        // resume while pause is pending (we get an event from dlmgr when pause is done)
        DLTRACE(("Enqueuing resume"));
        iQueuedResume = ETrue;
        }
    // Start from the beginning or continue from paused
    else if ( ( iState.iProgressState == ECatalogsHttpNone || 
         iState.iOperationState == ECatalogsHttpOpPaused ||
         iState.iOperationState == ECatalogsHttpOpQueued ) && 
         !iTransaction ) 
        {        
        DLTRACE(("Starting the operation"));
        // Check if the download can be started or put it to queue
        
        err = iOwner.StartOperation( this ); 
        if ( err == KErrNone ) 
            {                        
            DLTRACE(("Starting the actual download"));         
            err = StartDownload();
            
            if ( err == KErrNone ) 
                {
                DLTRACE(("DL started"));
                iState.iOperationState = ECatalogsHttpOpInProgress;
                iState.iProgressState = ECatalogsHttpStarted;
                iPaused = EFalse;
                
    		    if ( iObserver ) 
    		        {		        
        		    TRAP( err, iObserver->HandleHttpEventL( 
        		        *this, 
        		        iState ) );
    		        }
                }
            else 
                {
                DLTRACE(( "Start failed with: %i", err ));
                }
            }
        else if ( err == KCatalogsHttpOperationQueued ) 
            {            
            // Set state as queued and notify the observer
            err = KErrNone;
            iState.iOperationState = ECatalogsHttpOpQueued;
            iPaused = EFalse;
            
            if ( iObserver ) 
                {
                TRAP( err, iObserver->HandleHttpEventL( *this, iState ) );
                }
            }        
        }
        
    return err;
    }


// ---------------------------------------------------------------------------
// Pause download
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpDownload::Pause()
    {
    DLTRACEIN( ( "" ) );    
        
    if( iQueuedResume )
        {
        DLTRACE(("Removing queued resume"));
        iQueuedResume = EFalse;
        return KErrNone;
        }
    else if ( iTransaction ) 
        {
        DLTRACE(("Was getting the headers"));
        iTransaction->Cancel();
        iTransaction = NULL;
        
        if ( iDdDownload ) 
            {
            DLTRACE(("DD download. Cannot really pause but we fake it"));
            TFileName name;
            iDdFile.FullName( name );
            iDdFile.Close();
            iFs.Delete( name ); 
            
            // Reseting filename so that HTTP headers are taken again
            // and DD download is restarted correctly.
            //
            // This is not the optimal solution but otherwise we would need
            // to handle DD download resuming after client restart 
            // differently from resuming normal downloads
            //
            // SetFilenameL can't actually leave here because it just deletes
            // the old filename without any new memory allocations
            TRAP_IGNORE( iConfig->SetFilenameL( KNullDesC() ) );
            iDdDownload = EFalse;           
            }
        
        ReleasePtr( iConnection );
        // Notify the session manager that there's room for executing
        // another operation
        iOwner.PauseOperation( this );
        iState.iOperationState = ECatalogsHttpOpPaused;
        iState.iProgressState = ECatalogsHttpNone;
        iPaused = ETrue; 
        SetTransferring( EFalse );           
        return KErrNone;
        }
    else 
        {
        // Set pending flag
        if( iState.iOperationState == ECatalogsHttpOpInProgress)
            {
            DLTRACE(("Set pause pending flag"));
            iPausePending = ETrue;
            }
        // It's possible that pause is not supported but then the download is
        // just restarted when it's resumed
        DLTRACE(("Thread semaphore count: %d", RThread().RequestCount() ));
        TInt err = KErrNone;
        if ( iDownload ) 
            {
					    try
							{
							iDownload->pause();
							}
			        catch(const std::exception& exception)
			        	{
							 err = qt_symbian_exception2Error(exception);
			        	}
            }
        
        DLTRACE(("Pause err: %d", err ));
        if ( err == KErrNone ) 
            {
            ReleasePtr( iConnection );
            if ( !iPaused )
                {
                // Notify the session manager that there's room for executing
                // another operation
                iOwner.PauseOperation( this );
                }
            iState.iOperationState = ECatalogsHttpOpPaused;
            iPaused = ETrue;            
            SetTransferring( EFalse );
            }
        DLTRACEOUT(("Operation paused"));
        return err;
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpDownload::NotifyCancel()
    {
    DLTRACEIN((""));
    if ( iObserver )
        {
        iObserver->HandleHttpError( *this, TCatalogsHttpError(
            ECatalogsHttpErrorGeneral, KErrCancel ) );
        }
    else 
        {
        DLINFO(("No observer"));
        }
    }
    
// ---------------------------------------------------------------------------
// NOT SUPPORTED
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpDownload::SetBodyL( const TDesC8& /* aBody */ )
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// NOT SUPPORTED
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpDownload::SetBodyL( const TDesC16& /* aBody */ )
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------------------------
// Body getter
// ---------------------------------------------------------------------------
//	
const TDesC8& CCatalogsHttpDownload::Body() const
    {
    return KNullDesC8;        
    }


// ---------------------------------------------------------------------------
// URI setter
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpDownload::SetUriL( const TDesC8& aUri )
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
void CCatalogsHttpDownload::SetUriL( const TDesC16& aUri )
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
const TDesC8& CCatalogsHttpDownload::Uri() const
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
MCatalogsHttpConfig& CCatalogsHttpDownload::Config() const
    {
    return *iConfig;
    }
    

// ---------------------------------------------------------------------------
// Request headers
// ---------------------------------------------------------------------------
//	
MCatalogsHttpHeaders& CCatalogsHttpDownload::RequestHeadersL() const
    {
    return iConfig->RequestHeaders();
    }


// ---------------------------------------------------------------------------
// Response headers
// ---------------------------------------------------------------------------
//	
const MCatalogsHttpHeaders& CCatalogsHttpDownload::ResponseHeadersL() const
    {
    return *iResponseHeaders;
    }


// ---------------------------------------------------------------------------
// Operation type
// ---------------------------------------------------------------------------
//	
TCatalogsHttpOperationType CCatalogsHttpDownload::OperationType() const
    {
    return ECatalogsHttpDownload;    
    }


// ---------------------------------------------------------------------------
// Operation ID
// ---------------------------------------------------------------------------
//	
const TCatalogsTransportOperationId& 
    CCatalogsHttpDownload::OperationId() const
    {
    return iId;
    }


// ---------------------------------------------------------------------------
// Content type setter
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpDownload::SetContentTypeL( const TDesC8& aContentType  )
    {
    DLTRACEIN(( "Content-type: %S", &aContentType ));
    
    iContentType = aContentType;
    }


// ---------------------------------------------------------------------------
// Content type setter
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpDownload::SetContentTypeL( const TDesC16& aContentType )
    {
    DLTRACEIN(( _L("Content-type: %S"), &aContentType ));
    
    HBufC8* contentType = ConvertUnicodeToUtf8L( aContentType );
    iContentType = *contentType;
    delete contentType;
    }


// ---------------------------------------------------------------------------
// Content type getter
// ---------------------------------------------------------------------------
//	
const TDesC8& CCatalogsHttpDownload::ContentType() const
    {
    return iContentType;
    }

// ---------------------------------------------------------------------------
// Content size getter
// ---------------------------------------------------------------------------
//	
TInt32 CCatalogsHttpDownload::ContentSize() const
    {    
    DLTRACEIN((""));
    // We get the content size only once in order to minimize 
    // client-server communication with the Download manager server
    if ( iDownload && iContentSize <= 0 ) 
        {                
        iContentSize = iDownload->attribute(TotalSize).toInt();
        DLTRACE(("Content size from download: %i", iContentSize ));
        }
    return iContentSize;
    }


// ---------------------------------------------------------------------------
// Downloaded size getter
// ---------------------------------------------------------------------------
//	
TInt32 CCatalogsHttpDownload::TransferredSize() const
    {    
    return iTransferredSize;
    }


// ---------------------------------------------------------------------------
// Is download pausable
// ---------------------------------------------------------------------------
//	
TBool CCatalogsHttpDownload::IsPausable() const
    {    
    TBool pausable = ETrue;
    if ( iDownload ) 
        {        

        pausable=iDownload->attribute(Pausable).toBool();
        }
    return pausable;
    }


// ---------------------------------------------------------------------------
// Download status code
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpDownload::StatusCode() const
    {
    return 0;
    }


// ---------------------------------------------------------------------------
// Download status text
// ---------------------------------------------------------------------------
//	
const TDesC8& CCatalogsHttpDownload::StatusText() const
    {
    return KNullDesC8();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpDownload::SetConnection( CCatalogsConnection& aConnection )
    {
    DLTRACEIN((""));
    ReleasePtr( iConnection );

    iConnection = &aConnection;
    iConnection->AddRef();
    const TCatalogsConnectionMethod& method( aConnection.ConnectionMethod() );
    DLTRACE(("Connection: %d, %u, %u", 
        method.iType, method.iId, method.iApnId ));
        
    Config().SetConnectionMethod( method );
    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpDownload::ReportConnectionError( TInt aError )
    {
    DLTRACEIN(("aError: %d", aError));
    iObserver->HandleHttpError( *this, 
        TCatalogsHttpError( ECatalogsHttpErrorGeneral, aError ) );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
CCatalogsHttpConnectionManager& CCatalogsHttpDownload::ConnectionManager()
    {
    return iOwner.ConnectionManager();
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpDownload::ExternalizeL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));
    iConfig->ExternalizeL( aStream );
    ExternalizeDesL( ContentType(), aStream );    
    ExternalizeDesL( *iTempFilename, aStream );
    ExternalizeEnumL( iMode, aStream );
    iId.ExternalizeL( aStream );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpDownload::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    iConfig->InternalizeL( aStream );
    HBufC8* contentType = NULL;
    InternalizeDesL( contentType, aStream );
    // HLa if ( contentType->Length() > KMaxContentTypeLength ) 
    if ( contentType->Length() > 256 )
        {
        DeletePtr( contentType );
        DLERROR(("Content type was too long, leaving with KErrCorrupt"));
        User::Leave( KErrCorrupt );
        }
    iContentType = *contentType;
    DeletePtr( contentType );
    
    InternalizeDesL( iTempFilename, aStream );
    InternalizeEnumL( iMode, aStream );
    iId.InternalizeL( aStream );
    }


// ---------------------------------------------------------------------------
// Operation state
// ---------------------------------------------------------------------------
//  
TCatalogsHttpOperationState CCatalogsHttpDownload::State() const
    {
    return iState.iOperationState;
    }


// ---------------------------------------------------------------------------
// Handles events from the configuration
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpDownload::HandleHttpConfigEvent( 
    MCatalogsHttpConfig* /* aConfig */,
    const TCatalogsHttpConfigEvent& aEvent )
    {
    if ( aEvent == ECatalogsHttpCfgPriorityChanged ) 
        {
        return iOwner.OperationPriorityChanged( this );
        }
    return KErrNone;
    }

void CCatalogsHttpDownload::HandledownloadEventL(DownloadEvent& aEvent)
{
	
	switch(aEvent.type())
	{
		
		case DownloadEvent::Started:
		
			break;
		
		case DownloadEvent::Error:	
		case DownloadEvent::Failed:
		case DownloadEvent::NetworkLoss:    
		case DownloadEvent::ConnectionNeeded:  
		case DownloadEvent::ConnectionDisconnected:    
			{
		    TInt32 errorId = -1;
		    if ( aEvent.type() == DownloadEvent::NetworkLoss  || 
		         aEvent.type() == DownloadEvent::ConnectionNeeded ||
		         aEvent.type() == DownloadEvent::ConnectionDisconnected )
		        {
		        errorId = WRT::ConnectionFailed;
		        }
		    else
		        {
		        errorId = iDownload->attribute(LastError).toInt();
		        }
            SetTransferring( EFalse );
            if ( errorId > 0 )
                {
                // Symbian error codes are always negative numbers 
                errorId =  -1 * errorId;
                }
                                    
            /*if ( ( errorId == ConnectionFailed || 
                   errorId == TransactionFailed )) 
                {
                DLTRACE(("Try to reconnect"));
                iReconnectWhenFail = EFalse;
                iDownload->start();
                break;
                }
            else if ( errorId == ContentExpired ||
                      errorId == PartialContentModified )
                {
                DLTRACE(("Content has changed, reset and restart"));
                iReconnectWhenFail = EFalse;
                iDownload->cancel();
                iDownload->start();
                break;
                }*/
            iState.iOperationState = ECatalogsHttpOpFailed;
            
            TBool deleted = EFalse;
            if ( iObserver ) 
                {
                // Determine whether failure was due to a HTTP error or some
                // other error. 
                //if ( errorId >= 400 ) 
                //    {
               //     deleted = iObserver->HandleHttpError( 
               //     *this, TCatalogsHttpError(
              //      ECatalogsHttpErrorHttp, 
              //      errorId ) );
              //      }
              //  else 
                    {
                    AddRef();
                    //iOwner.ConnectionManager().ReportConnectionError( 
                    //               TCatalogsConnectionMethod(), errorId );
                                
                    if ( iRefCount > 1 )
                        {                       
                        deleted = iObserver->HandleHttpError( 
                        *this, TCatalogsHttpError( 
                        ECatalogsHttpErrorGeneral, errorId ) );
                        }
                    else 
                        {
                        deleted = ETrue;
                        }
                    Release();
                    }
                }
            if ( !deleted )
                {                
                iOwner.CompleteOperation( this );
                iState.iProgressState = ECatalogsHttpDone;
                }
			}
			break;
		
		case DownloadEvent::Cancelled:
		
			break;
		case DownloadEvent::ContentTypeChanged:
			
			break;
		case DownloadEvent::HeadersReceived:
			{
					// Read the response headers from platform DL manager
			UpdateResponseHeadersL();		
			UpdateContentType();
		  }
			break;
	
	  
		case DownloadEvent::Completed:
			  {            
            // move/rename temp file as the target file
            TRAPD( err, MoveFileL() );
            
            // DLMAIN-546, delete DL manager's download before starting
            // the next one so that downloads don't jam, again
            if ( iDownload ) 
                {   
                	iTransferredSize = iDownload->attribute(DownloadedSize).toInt();             
         
                    
                DLTRACE(("Deleting download"));                
                DeletePlatformDownload();                
                }
            iState.iProgressState = ECatalogsHttpDone;
            iOwner.CompleteOperation( this );
            iState.iOperationState = ECatalogsHttpOpCompleted;
            
            if ( iObserver ) 
                {                
                iObserver->HandleHttpEventL( *this, iState );
                }

	     }
			break;
			
		case DownloadEvent::InProgress:
		    {
		    iTransferredSize  =  iDownload->attribute(DownloadedSize).toInt();
		    if ( iObserver ) 
		        {                
		        iObserver->HandleHttpEventL( *this, iState );
		        }
		    }
		    break;
		  
	  default:
            {
            }		  
	};
}
// ---------------------------------------------------------------------------
// Handles events from the transaction
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpDownload::HandleHttpEventL( 
    MCatalogsHttpOperation& aOperation,
    TCatalogsHttpEvent aEvent )
    {
    DLTRACEIN((""));
    DLINFO( ("OP ID: %i, event op: %i, pr: %i, this-ptr: %X",
        aOperation.OperationId().Id(), aEvent.iOperationState,
        aEvent.iProgressState, this ) );
    
        
    switch ( aEvent.iOperationState )
        {
        case ECatalogsHttpOpCompleted:            
            {
            
            if ( !iDdDownload )
                {
                DLTRACE( ("Op completed") );
                UpdateResponseHeadersL( aOperation );
                
                DLTRACE(("Content type from op: %S", &aOperation.ContentType() ));
                // Copy content type if didn't receive any from protocol
                // or if the type received from HTTP headers is OMA DD when
                // the type from protocol/previous purchase might not be
                if ( !iContentType.Length() ||
                     aOperation.ContentType().MatchF( KMimeTypeMatchOdd8 ) == 0 ) 
                    {
                    DLTRACE(("Updating content type"));
                    SetContentTypeL( aOperation.ContentType() );         
                    }
                
                iTransaction = NULL;

                // Tries to parse the target filename from the URI if it hasn't
                // been set already. Doing this here because if it were done
                // in UpdateDownloadConfigurationL, pausing before HTTP
                // headers are received would prevent new HTTP HEAD request
                // from being sent.
                ParseFilenameFromUriL();                                
                
                TRAPD( err, UpdateFilenameFromContentDispositionL() );
                if ( err != KErrNone ) 
                    {
                    // Failed to update from content disposition, 
                    // use tempfilename unless some name has already been set
                    UpdateFilenameL();
                    }
                
                // No need to get headers again
                iMode = ECatalogsHttpHeaderModeNoHead;

                // Check if the download is a DD and if it is, use a 
                // transaction to download the descriptor so that Download
                // manager doesn't try to mess with things

                if ( iContentType.MatchF( KMimeTypeMatchOdd8 ) == 0 )
                    {
                    DLTRACE(("Content type matches DD download"));
                    iDdDownload = ETrue;
                    
                    StartDescriptorDownloadL();                                    
                    }
                else
                    {                
                    // Start the normal file download
                    User::LeaveIfError( Start() );
                    }

                aOperation.Release();
                }
            else // iDdDownload == ETrue
                {
                
                DLTRACE(("Closing dd"));
                iDdFile.Close();
                iTransaction = NULL;
                iState = aEvent;
                iState.iOperationState = ECatalogsHttpOpCompleted;
                iState.iProgressState = ECatalogsHttpDone;                
                
                iObserver->HandleHttpEventL( *this, iState );
                aOperation.Release();
                }
            break;
            }

        case ECatalogsHttpOpInProgress:
            {
            if ( iDdDownload ) 
                {
                DLTRACE(("DD download"));
                iState = aEvent;
                switch ( aEvent.iProgressState )
                    {
                    case ECatalogsHttpResponseBodyReceived:
                        {
                        DLTRACE(("Received a body part"));
                        User::LeaveIfError( iDdFile.Write( iTransaction->Body() ) );
                        iObserver->HandleHttpEventL( *this, aEvent );
                        break;
                        }
                    
                    default:
                        {
                        DLTRACE(("Default"));
                        
                        }                    
                    }
                }
            break;
            }

        default:
            {
            }
        }
    
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Handles events from the transaction
// ---------------------------------------------------------------------------
//	
TBool CCatalogsHttpDownload::HandleHttpError( 
    MCatalogsHttpOperation& aOperation, TCatalogsHttpError aError )
    {
    DLTRACEIN((""));
    DLERROR( ("Error type: %i, code: %i", aError.iType, 
        aError.iError ) );

    iTransaction = NULL;
    aOperation.Release();
        
    iOwner.CompleteOperation( this );        
    iState.iOperationState = ECatalogsHttpOpFailed;
    iState.iProgressState = ECatalogsHttpDone;
    iObserver->HandleHttpError( *this, aError );
    
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Handles events from the download manager
// ---------------------------------------------------------------------------
//	

/* HLa
void CCatalogsHttpDownload::HandleEventL( THttpDownloadEvent aEvent )
    {
    DLTRACEIN((""));
    DLINFO( ("DL id: %d, Dl state: %i, Pr: %i, this-ptr: %X, iState: %d, iProgress: %d, dl-ptr: %X", 
        OperationId().Id(),
        aEvent.iDownloadState, aEvent.iProgressState,
        this, iState.iOperationState, iState.iProgressState,
        iDownload ) );

        
#ifdef __SERIES60_31__
    if ( IsCancelled() )
        {
        if ( IsOneOf( 
                aEvent.iDownloadState,             
                EHttpDlPaused, 
                EHttpDlCompleted, 
                EHttpDlFailed ) )
            {
            Cancel();               
            }
        return;
        }    
    
#endif   
    
    if ( iState.iProgressState == ECatalogsHttpNone ) 
        {
        DLTRACEOUT(("Not yet in progress"));
        return;
        }
    
	TInt32 statusCode = -1;
	
	 statusCode  = (Download::State)iDownload->attribute(WRT::State).toInt(); 
	 DLINFO( ("Response status: %i", statusCode ) );

#ifdef CATALOGS_BUILD_CONFIG_DEBUG    
    TInt32 errorId = 0;
		errorId=iDownload->attribute(LastError).toInt();

	DLINFO( ("Error id: %i", errorId ) );
	    
#endif

    TInt32 globalErrorId = 0;    
   globalErrorId= iDownload->attribute(LastError).toInt();

	DLINFO( ("Global error id: %i", globalErrorId ) );
	

    // was >=, DL manager should give us proper errors but it doesn't always
    if ( ( statusCode == KHttpErrorStatus ||
           globalErrorId == KGenericSymbianHttpError ||
           globalErrorId == KErrDisMounted) && 
         aEvent.iDownloadState != EHttpDlFailed ) 
        {
        DLTRACE(("Setting download as failed because received a response >= 400" ));
        aEvent.iDownloadState = EHttpDlFailed;
        }

    switch ( aEvent.iDownloadState )
        {
        case EHttpDlCreated:
            {
            iState.iOperationState = ECatalogsHttpOpCreated;
            break;
            }
                        
        case EHttpDlInprogress:
            {
            iState.iOperationState = ECatalogsHttpOpInProgress;
            // Update progress info            
            HandleEventProgressL( aEvent );
            break;
            }
            
        case EHttpDlPaused:
            {
            DLTRACE(("EHttpDlPaused"));
            // For some reason restored downloads receive unwanted EHttpDlPaused
            // event when they are started. Hopefully this doesn't break something else
            if ( iState.iProgressState == ECatalogsHttpStarted ) 
                {                
                DLTRACE(("Something funny happened, skip the event"));
                break;
                }
            
            SetTransferring( EFalse );
            if ( iReconnectWhenFail && 
                 iState.iProgressState == ECatalogsHttpDisconnected ) 
                {
                DLTRACE(("Try to reconnect"));
                iReconnectWhenFail = EFalse;
                iDownload->start();                
                break;
                }
            
            if ( iState.iOperationState != ECatalogsHttpOpQueued ) 
                {                
                iState.iOperationState = ECatalogsHttpOpPaused;            
                iPaused = ETrue;
                }
            ReleasePtr( iConnection );
            
            if( aEvent.iProgressState == EHttpProgNone )
                {
                DLTRACE(("got pause event, resetting pause pending flag"));
                iPausePending = EFalse;
                if( iQueuedResume )
                    {
                    DLTRACE(("Handling queued resume"));
                    iQueuedResume = EFalse;
                    Start();
                    }
                }
            else
                {
                HandleEventProgressL( aEvent );
		        }
            break;
            }	


        case EHttpDlCompleted:
            {
            DLINFO( ("EHttpDlCompleted, progress state: %d", aEvent.iProgressState ) );                      
            SetTransferring( EFalse );
            
            // Release connection as soon as possible so that other APs
            // can be raised in the observer callbacks if necessary
            ReleasePtr( iConnection );
            
            if ( iPaused ) 
                {
                DLTRACE(("iPaused == ETrue, setting state to paused"));
                iState.iOperationState = ECatalogsHttpOpPaused;
                }
            
            // This ensures that CompleteOperation() works correctly if
            // the download was just paused when it completes             
            if ( iState.iOperationState != ECatalogsHttpOpPaused ) 
                {                
                iState.iOperationState = ECatalogsHttpOpCompleted;
                }
            
            // move/rename temp file as the target file
            TRAPD( err, MoveFileL() );
            
            // DLMAIN-546, delete DL manager's download before starting
            // the next one so that downloads don't jam, again
            if ( iDownload ) 
                {   
                	iTransferredSize = iDownload->attribute(DownloadedSize).toInt();             
         
                    
                DLTRACE(("Deleting download"));                
                DeletePlatformDownload();                
                }
            
            iState.iProgressState = ECatalogsHttpDone;
            iOwner.CompleteOperation( this );
            iState.iOperationState = ECatalogsHttpOpCompleted;
            
            if ( err != KErrNone && iObserver ) 
                {
                DLERROR(("Error %d occurred when moving", err));
                iState.iOperationState = ECatalogsHttpOpFailed;
                iObserver->HandleHttpError( 
                    *this, TCatalogsHttpError(
                    ECatalogsHttpErrorGeneral, 
                    err ) );                
                }
            else if ( iObserver ) 
                {                
                iObserver->HandleHttpEventL( *this, iState );
                }
            
            DLINFO(("EHttpDlCompletedDone"));
            break;  
            }
    
            
        case EHttpDlFailed:
            {
            DLTRACE(( "Download failed" ));
            TInt32 errorId = -1;
            SetTransferring( EFalse );
            errorId = iDownload->attribute(LastError).toInt();
        
        	DLINFO( ("Error id: %i", errorId ) );

        	DLINFO( ("Global error id: %i", globalErrorId ) );

            if ( iReconnectWhenFail && 
                 ( errorId == EConnectionFailed || 
                   errorId == ETransactionFailed )) 
                {
                DLTRACE(("Try to reconnect"));
                iReconnectWhenFail = EFalse;
                iDownload->start();
                break;
                }
            else if ( errorId == EContentExpired ||
                      errorId == EPartialContentModified )
                {
                DLTRACE(("Content has changed, reset and restart"));
                iReconnectWhenFail = EFalse;
                iDownload->cancel();
                iDownload->start();
                break;
                }
            
            ReleasePtr( iConnection );    
            iState.iOperationState = ECatalogsHttpOpFailed;


            TBool deleted = EFalse;
            if ( iObserver ) 
                {
                // Determine whether failure was due to a HTTP error or some
                // other error. HTTP errors are mapped to a negative range
                if ( statusCode >= 400 ) 
                    {
                    deleted = iObserver->HandleHttpError( 
                        *this, TCatalogsHttpError(
                        ECatalogsHttpErrorHttp, 
                        KCatalogsErrorHttpBase - statusCode ) );
                    }
                else 
                    {
                    // 5.0 issue: some downloads fail without any error code
                    // so we have to give them some
                	if ( globalErrorId == KErrNone ) 
                	    {
                	    DLERROR(("Download failed without an error"));
                	    globalErrorId = KErrUnknown;
                	    }
                	
                	AddRef();
                	iOwner.ConnectionManager().ReportConnectionError( 
                	    TCatalogsConnectionMethod(), globalErrorId );
                	
                	if ( iRefCount > 1 )
                	    {                	    
                        deleted = iObserver->HandleHttpError( 
                            *this, TCatalogsHttpError( 
                            ECatalogsHttpErrorGeneral, globalErrorId ) );
                	    }
                	else 
                	    {
                	    deleted = ETrue;
                	    }
                    Release();
                    }
                }
            if ( !deleted )
                {                
                iOwner.CompleteOperation( this );
                iState.iProgressState = ECatalogsHttpDone;
                }
            break;
            }


        /// MMC card or other storage media is removed from the phone.
/*
        case EHttpDlMediaRemoved:
            {
            iState.iOperationState = ECatalogsHttpOpMediaRemoved;
            break;
            }
  */          

        /** 
        * MMC card or other storage media inserted and 
        * downloaded content file found on it. 
        * If MMC card inserted, but (partially) downloaded content file 
        * is not found on it, download is failed with error reason
        * EContentFileIntegrity. 
        */            
/*
        case EHttpDlMediaInserted:
            {
            iState.iOperationState = ECatalogsHttpOpMediaInserted;
            break;
            }	
*/
        /** 
        * Download process can be paused again. This event only occurs after
        * EHttpDlNonPausable. 
        */	
/* HLa
        case EHttpDlPausable:
            {
            DLTRACE(( "Pausable" ));
            iState.iOperationState = ECatalogsHttpOpPausable;
            HandleEventProgressL( aEvent );
            break;
            }
        
        /// Download process cannot be paused, or the content will be lost.  
        case EHttpDlNonPausable:
            {
            
            iState.iOperationState = ECatalogsHttpOpNonPausable;
            DLTRACE(( "Nonpausable" ));
            // Update the download UI data.
            HandleEventProgressL( aEvent );
            break;
            }
            
        /// Download is started when it's already progressing
        case EHttpDlAlreadyRunning:
            {
            DLTRACE(( "Already running" ));
            iState.iOperationState = ECatalogsHttpOpAlreadyRunning;
            HandleEventProgressL( aEvent );
            // Nothing to do.
            break;
            }

        default:
            {
            break;
            }	
        }
    }
*/


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
/*
void CCatalogsHttpDownload::HandleEventProgressL( const 
    THttpDownloadEvent& aEvent )
    {
    DLTRACEIN(( "" ));
	switch ( aEvent.iProgressState ) 
		{
		case EHttpStarted: 
		    {
		    DLTRACE(( "Progress: Started" ));
		    // If the download was paused immediately, iPaused flag will be
		    // ETrue but the dl operation is not paused
		    // Try again to pause the download correctly
		    if ( iPaused )
		        {
		        
		        Pause();
		        }
		    else
		        {
    		    iState.iProgressState = ECatalogsHttpStarted;
    		    iPaused = EFalse;		        
		        }
		    break;
		    }
		    
		case EHttpProgCreatingConnection:
		    {
		    DLTRACE(( "Progress: Creating connection" ));
		    iState.iProgressState = ECatalogsHttpCreatingConnection;
		    break;
		    }
		    
		case EHttpProgConnected:
		    {
		    DLTRACE(( "Progress: Connected" ));
		    iState.iProgressState = ECatalogsHttpConnected;	
		    SetTransferring( ETrue );	    
		    break;
		    }
		    
		case EHttpProgConnectionSuspended:
		    {
		    DLTRACE(( "Progress: Connection suspended" ));
		    iState.iProgressState = ECatalogsHttpConnectionSuspended;
		    break;
		    }

		case EHttpProgDisconnected:
		    {
		    DLINFO(( "Progress: Disconnected" ));

		    if ( iState.iProgressState == ECatalogsHttpCreatingConnection ) 
		        {
		        DLERROR(("Connection creation failed, no reconnecting"));
		        iReconnectCount = 0;
		        }

		    iState.iProgressState = ECatalogsHttpDisconnected;
		    DASSERT( iObserver );
		    SetTransferring( EFalse );
		    // Don't restart paused downloads or notify about disconnection
		    if ( !iPaused ) 
		        {		  
                TInt32 globalErrorId = 0;
            	    globalErrorId = iDownload->attribute(LastError).toInt();
            	DLINFO( ("Global error id: %i", globalErrorId ) );

                if ( globalErrorId == KErrCancel ) 
                    {
                    DLTRACE(("Cancelled"));
                    iState.iOperationState = ECatalogsHttpOpFailed;
                	iOwner.ConnectionManager().ReportConnectionError( 
                	    TCatalogsConnectionMethod(), globalErrorId );

                    }
                else //if ( globalErrorId != KErrNone ) 
                    {
                    
                    if ( iReconnectCount ) 
        		        {
        		        DLTRACE(("Reconnect, count left: %d", iReconnectCount ));
        		        iReconnectCount--;
        		        iReconnectWhenFail = ETrue;
        		        }
        		    else if ( iObserver )
        		        {		        
        		        DLTRACE(("No more reconnect attempts"));
        		        TCatalogsHttpError error( ECatalogsHttpErrorGeneral, KErrDisconnected );
            	        iObserver->HandleHttpError( *this, error );
        		        }
                    }
    		    
		        }
		    break;
		    }

		case EHttpProgDownloadStarted:
		    {
		    DLTRACE(( "Progress: Download started" ));
		    iState.iProgressState = ECatalogsHttpDownloadStarted;
		    DASSERT( iObserver );
		    
		    if ( iObserver ) 
		        {		        
    		    iObserver->HandleHttpEventL( *this, 
    		        iState );
		        }
		    break;
		    }

		    
		case EHttpContentTypeReceived:
		// Start download again if content-type is acceptable
		// and UiLib is not installed
			{
		    // State is paused. The observer should continue
		    // the download if the content type is acceptable
		    // otherwise he should cancel it
		    DLTRACE( ( "Progress: Content type received" ) );
		    iState.iProgressState = ECatalogsHttpContentTypeReceived;            
            UpdateContentType();
		    
		    
		    if ( iObserver ) 
		        {
		        
    		    iObserver->HandleHttpEventL( *this, 
    		        iState );
		        }
			break;			
			}


		case EHttpProgSubmitIssued:
			{
			iState.iProgressState = ECatalogsHttpSubmitIssued;
			break;			
			}


		case EHttpProgResponseHeaderReceived:
			{	
			DLTRACE(( "Progress: Response header received" ));
			iState.iProgressState = ECatalogsHttpResponseHeaderReceived;
			
			// Reset reconnect count
		    iReconnectCount = KReconnectAttempts;			
			
			// Read the response headers from platform DL manager
			UpdateResponseHeadersL();		
			UpdateContentType();

			if ( iObserver ) 
			    {			    
    		    iObserver->HandleHttpEventL( *this,
    		        iState );
			    }
			break;
			}


		case EHttpProgResponseBodyReceived:
			{
			DLTRACE(( "Progress: Response body received" ));
			iState.iProgressState = ECatalogsHttpResponseBodyReceived;

		    // Reset reconnect count
		    iReconnectCount = KReconnectAttempts;

		    TInt32 size = 0;
		     size = iDownload->attribute(DownloadedSize).toInt();

	        DLTRACE(("Transferred size from download: %i", size));

	        if ( size != iTransferredSize ) 
	            {
	            DLTRACE(("Updating transferred size"));
	            iTransferredSize = size;	           
                if ( iObserver )
                    {                
                    iObserver->HandleHttpEventL( *this,
                        iState );
                    }
                }

			break;
			}


		case EHttpProgRedirectedPermanently:
			{
			DLTRACE(( "Progress: Redirected permanently" ));
			iState.iProgressState = ECatalogsHttpRedirectedPermanently;
			
			TRAPD( err,
			    {
                UpdateUriL();
			    ParseFilenameFromUriL();
			    } );
			    
			if ( iObserver ) 
			    {
			    if ( err == KErrNone ) 
			        {			        
			        iObserver->HandleHttpEventL( *this, iState );
			        }
			    else 
			        {
			        iObserver->HandleHttpError( *this, TCatalogsHttpError(
			            ECatalogsHttpErrorGeneral, err ) );
			        }
			    }
			
			break;
			}


		case EHttpProgRedirectedTemporarily:
			{			
			DLTRACE(( "Progress: Redirected temporarily" ));
			iState.iProgressState = ECatalogsHttpRedirectedTemporarily;
			
			TRAPD( err,
			    {
			    UpdateUriL();
			    ParseFilenameFromUriL();
			    } );
			    
			if ( iObserver ) 
			    {
			    if ( err == KErrNone ) 
			        {			        
			        iObserver->HandleHttpEventL( *this, iState );
			        }
			    else 
			        {
			        iObserver->HandleHttpError( *this, TCatalogsHttpError(
			            ECatalogsHttpErrorGeneral, err ) );
			        }
			    }
			
			break;
			}

		case EHttpProgContentTypeChanged:
			{			
			iState.iProgressState = ECatalogsHttpContentTypeChanged;
			break;
			}


		case EHttpProgMovingContentFile:
			{
			DLINFO( ("Progress: Moving content file") );
			iState.iProgressState = ECatalogsHttpMovingContentFile;
			break;
			}


		case EHttpProgContentFileMoved:
			{
			DLINFO( ("Progress: Content file moved") );
			iState.iProgressState = ECatalogsHttpContentFileMoved;
            iOwner.CompleteOperation( this );
			break;
			}

		default:
		    DLTRACE(("Default"));
		    DLTRACE(( "state: %d, progress: %d",aEvent.iDownloadState, aEvent.iProgressState));
			break;
		}
    }
 */   


// ---------------------------------------------------------------------------
// Sets file server session
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpDownload::SetFileServerSession( RFs& aFs )
    {
    DLTRACEIN((""));
    iFs = aFs;
    }


#ifdef __SERIES60_31__
TBool CCatalogsHttpDownload::IsCancelled() const
    {
    return iCancelled;
    }
#endif    

    
    
// ---------------------------------------------------------------------------
// Sets header mode
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpDownload::SetHeaderMode( TCatalogsHttpHeaderMode aMode )
    {
    iMode = aMode;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//	
CCatalogsHttpDownload::CCatalogsHttpDownload( 
    CCatalogsHttpDownloadManager& aOwner, 
    Download* aDownload ) :
    iOwner( aOwner ), 
    iDownload( aDownload ), 
    iState( ECatalogsHttpOpCreated, ECatalogsHttpNone ),
    iRefCount( 1 ), 
    iNormalDelete( EFalse ),
    iReconnectCount( KReconnectAttempts )    
    {
    DLTRACEIN(( "this-ptr: %X", this ));
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpDownload::ConstructL( const CCatalogsHttpConfig* aConfig )
    {
    // Get the id for the download and update iUri
    if ( iDownload ) 
        {        
        // id part will be updated from disk
        iId = TCatalogsTransportOperationId( iOwner.SessionId(), KErrNotFound );
				iQTDownload = new CCatalogsHttpQTDownload(this,iDownload);
				 
        UpdateSecondaryIdL();
        
        User::LeaveIfError( SetDeleteState( EDownloadCanBeDeleted ) );
        // Update iUri to match the current URI
        UpdateUriL();
        iResponseHeaders = CCatalogsHttpHeaders::NewL();
        }
    else
        {
        iId = TCatalogsTransportOperationId( 
            iOwner.SessionId(), 
            iOwner.NewDownloadId() );        
        }
    
    // Create a copy of the configuration 
    if ( aConfig ) 
        {
        iConfig = aConfig->CloneL();
        }
    else 
        {
        // Or create a new configuration
        iConfig = CCatalogsHttpConfig::NewL();
        }    
    
    AssignDesL( iTempFilename, KNullDesC() );    
    }
    

// ---------------------------------------------------------------------------
// Updates the content type
// ---------------------------------------------------------------------------
//	    
void CCatalogsHttpDownload::UpdateContentType()
    {
    DLTRACEIN(( "" ));
    if ( !iContentType.Length() ) 
        {
        // Download always exists when this is called so no need to check
        DLTRACE(("Updating the content type"));
        QString contentType;
        contentType= iDownload->attribute(WRT::ContentType).toString();
        // HLa TBuf<KMaxContentTypeLength>  ContentType(contentType.utf16());
        TBuf<256>  ContentType(contentType.utf16());
        	iContentType.Copy(ContentType);
   
        }
    DLTRACEOUT(("Content-type: %S", &iContentType ));
    }


// ---------------------------------------------------------------------------
// Updates the filename either from content-disposition header or DL manager
// ---------------------------------------------------------------------------
//	    
void CCatalogsHttpDownload::UpdateFilenameFromContentDispositionL()
    {
    DLTRACEIN(( "" ));
    const TDesC8& value = iResponseHeaders->HeaderByKeyL( 
        KCatalogsHttpHeaderContentDisposition );
        
    DLTRACE( ( "Content-disposition: %S", &value ) );

    TCatalogsContentDispositionParser parser( value );

    //   
    HBufC* filename = parser.FilenameLC();
    
    iConfig->SetFilenameL( *filename );
    CleanupStack::PopAndDestroy( filename );
    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	    
void CCatalogsHttpDownload::UpdateFilenameL()
    {  
    DLTRACEIN( ("") );
    if ( ContainsData( iTempFilename ) &&        
         // We don't want to overwrite existing name
         !iConfig->Filename().Length() ) 
        {
        DLTRACE(( _L("Setting filename as temp filename: %S"), iTempFilename ));
        TParsePtrC parse( *iTempFilename );        
        iConfig->SetFilenameL( parse.NameAndExt() );
        }
    }


// ---------------------------------------------------------------------------
// Updates the platform download object to match the configuration
// ---------------------------------------------------------------------------
//	    
void CCatalogsHttpDownload::UpdateDownloadConfigurationL()
    {
    DASSERT( iEncodedUri );
    DLTRACEIN(( "URI: %S", iEncodedUri ));
    DASSERT( iConfig );
    
    TCatalogsConnectionMethod currentAp = iConfig->ConnectionMethod();

    // If no APN id is set, we need to use the default  
    if ( !currentAp.iApnId &&
         !currentAp.iId )
        {
        currentAp = iOwner.ConnectionManager().DefaultConnectionMethod();
        iConfig->SetConnectionMethod( currentAp );
        }

    // Ask for confirmation
    if ( !iOwner.ConnectionManager().AskConnectionConfirmation( currentAp ) )
        {
        DLINFO(("Connection denied by the user"));
        User::Leave( KCatalogsErrorHttpConnectionDenied );        
        }

    // note that actual connection setting is done by the 
    // session manager according to operation's config
    // so we don't set the accesspoint to DL manager here anymore
        
    // Update the member variable
    iObserver = iConfig->Observer();
           
    // Directory MUST be set
    if ( !iObserver || !iConfig->Directory().Length() ) 
        {
        DLERROR(("No directory path or observer set, leaving with KErrArgument"));
        User::Leave( KErrArgument );
        }
    
    // Check if HEAD is required
    UpdateHeadRequirement();
    
    // Update the filename only if it has not been set yet and header mode
    // is forced (like content downloads and previews) and
    // HEAD is not prohibited by download options
    if ( iMode == ECatalogsHttpHeaderModeForceHead &&
         !iConfig->Filename().Length() &&
         !( iConfig->Options() & ECatalogsHttpDisableHeadRequest ) )
        {        
        DLTRACE(( "Forcing header getting with transactions" ) );

        iTransaction = iOwner.CreateDlTransactionL( Uri(), 
            *this, *iConfig );
        iTransaction->Config().SetHttpMethod( ECatalogsHttpHead );
        iTransaction->Start();
        DLTRACE(( "Transaction started" ));
        }
    }


// ---------------------------------------------------------------------------
// Updates the URI from the DL manager to iUri
// ---------------------------------------------------------------------------
//	    
void CCatalogsHttpDownload::UpdateUriL()
    {
    DLTRACEIN( ("") );
    // Get the current url of the download
    RBuf8 buf;
    CleanupClosePushL( buf );
    const TInt KMaxUrlLength = 2048;
    buf.CreateL( KMaxUrlLength );
    QString string;
	try
	{
    string = iDownload->attribute(SourceUrl).toString();
	}
    catch(const std::exception& exception)
    	{
			 qt_symbian_exception2LeaveL(exception);
    	}

    
    // DL manager has the encoded URI
    AssignDesL( iEncodedUri, buf );
    CleanupStack::PopAndDestroy( &buf );
    
    DeletePtr( iUri );
    
    iUri = EscapeUtils::EscapeDecodeL( EncodedUri() );
    DLTRACE(( "URI: %S", &EncodedUri() ));
    }
    

// ---------------------------------------------------------------------------
// Updates the request headers to the platform DL manager
// ---------------------------------------------------------------------------
//	        
void CCatalogsHttpDownload::UpdateRequestHeadersL()
    {
    DLTRACEIN( ("") );
    RPointerArray<CCatalogsKeyValuePair>& headers( 
        iConfig->RequestHeaders().Headers() );


    // Iterate through headers and add them either to the request headers
    // in DL manager or iAddedRequestHeaders if they don't have an enumeration
    // in DL manager
    for ( TInt i = 0; i < headers.Count(); ++i )
        {
        TUint predefined = MatchWithPredefinedRequestHeader( 
            headers[i]->Key() );
            
        if ( !predefined ) 
            {
            predefined = MatchWithPredefinedGeneralHeader(
                headers[i]->Key() );
            }
            
        if ( !predefined ) 
            {
            AddRequestHeaderL( iAddedRequestHeaders, *headers[i] );
            }
        else 
            {
/*            User::LeaveIfError( iDownload->SetStringAttribute(
                predefined, headers[i]->Value() ) );*/
            }
        }

    // Add the headers that don't have a predefined enumeration in
    // the platform DL manager 
    if ( iAddedRequestHeaders ) 
        {
        DLTRACE(( "added request headers: %S", iAddedRequestHeaders ));
     /*   User::LeaveIfError( iDownload->SetStringAttribute(
            EDlAttrRequestHeaderAddon, *iAddedRequestHeaders ) );*/
        delete iAddedRequestHeaders;
        iAddedRequestHeaders = NULL;
        }    
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Update the response headers from DL manager to the download
// ---------------------------------------------------------------------------
//	    
void CCatalogsHttpDownload::UpdateResponseHeadersL()
    {
    DLTRACEIN( ("") );
#ifndef CATALOGS_DISABLE_DOWNLOAD_RESPONSE_HEADERS
    RBuf8 buf;
    buf.CreateL( KMaxGeneralHeaderFieldLength );
    buf.CleanupClosePushL();
    
        
    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrResponseCharSet, buf ) ) 
        {        
        DLTRACE( ( "CharSet: %S", &static_cast<TDesC8&>(buf) ) );    
        iResponseHeaders->AddHeaderL( KHttpResponseCharSet, buf );
        }    
    
    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrResponseAge, buf ) ) 
        {
        DLTRACE( ( "Age: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpResponseAge, buf );
        }

    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrResponseETag, buf ) )
        {
        DLTRACE( ( "ETag: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpResponseETag, buf );
        }

    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrResponseLocation, buf ) )
        {        
        DLTRACE( ( "Location: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpResponseLocation, buf );
        }
        
    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrResponseRetryAfter, buf ) )
        {
        DLTRACE( ( "RetryAfter: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpResponseRetryAfter, buf );
        }
        
    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrResponseServer, buf ) )
        {
        DLTRACE( ( "ResponseServer: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpResponseServer, buf );
        }

    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrResponseVary, buf ) )
        {
        DLTRACE( ( "ResponseVary: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpResponseVary, buf );
        }

    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrGeneralCacheControl, buf ) )
        {
        DLTRACE( ( "GeneralCacheControl: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpGeneralCacheControlHeader, buf );
        }

    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrGeneralDate, buf ) )
        {
        DLTRACE( ( "Date: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpGeneralDateHeader, buf );
        }

    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrGeneralPragma, buf ) )
        {
        DLTRACE( ( "Pragma: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpGeneralPragmaHeader, buf );
        }

    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrGeneralVia, buf ) )
        {
        DLTRACE( ( "Via: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpGeneralViaHeader, buf );
        }

    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrGeneralWarning, buf ) )
        {
        DLTRACE( ( "Warning: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpGeneralWarningHeader, buf );
        }
        
    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrEntityAllow, buf ) )
        {
        DLTRACE( ( "Allow: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpEntityAllowHeader, buf );
        }

    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrEntityContentEncoding, buf ) )
        {
        DLTRACE( ( "Content-Encoding: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpEntityContentEncodingHeader, buf );
        }

    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrEntityContentLanguage, buf ) )
        {
        DLTRACE( ( "Content-Language: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpEntityContentLanguageHeader, buf );
        }

    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrEntityContentLocation, buf ) )
        {
        DLTRACE( ( "Content-Location: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpEntityContentLocationHeader, buf );
        }

    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrEntityExpires, buf ) )
        {
        DLTRACE( ( "Expires: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpEntityExpiresHeader, buf );
        }

    if ( KErrNone == iDownload->GetStringAttribute( EDlAttrEntityLastModified, buf ) )
        {
        DLTRACE( ( "Last modified: %S", &static_cast<TDesC8&>(buf) ) );
        iResponseHeaders->AddHeaderL( KHttpEntityLastModifiedHeader, buf );
        }
        
    
    CleanupStack::PopAndDestroy( buf );

#endif

    DLTRACEOUT( ("") );
    }


// ---------------------------------------------------------------------------
// Update the response headers from the transaction to the download
// ---------------------------------------------------------------------------
//	    
void CCatalogsHttpDownload::UpdateResponseHeadersL( 
    MCatalogsHttpOperation& aOperation )
    {
    DLTRACEIN( ("") );
    CCatalogsHttpHeaders* headers = 
        static_cast<const CCatalogsHttpHeaders&>(
            aOperation.ResponseHeadersL() ).CloneL();
            
    delete iResponseHeaders;
    iResponseHeaders = headers;
        
    DLTRACEOUT( ("") );
    }


// ---------------------------------------------------------------------------
// Checks if the given header matches any of the request headers that have
// an predefined enumeration in the platform DL manager
// ---------------------------------------------------------------------------
//	    
TUint CCatalogsHttpDownload::MatchWithPredefinedRequestHeader( 
    const TDesC8& aHeader ) const
    {
    /* HLa
    if ( aHeader.CompareF( KHttpRequestAcceptHeader ) == 0 ) 
        {
        return EDlAttrRequestAccept;
        }
    else if ( aHeader.CompareF( KHttpRequestAcceptCharSetHeader ) == 0 ) 
        {
        return EDlAttrRequestAcceptCharSet;
        }
    else if ( aHeader.CompareF( KHttpRequestAcceptLanguageHeader ) == 0 ) 
        {
        return EDlAttrRequestAcceptLanguage;
        }
    else if ( aHeader.CompareF( KHttpRequestExpectHeader ) == 0 ) 
        {
        return EDlAttrRequestExpect;
        }
    else if ( aHeader.CompareF( KHttpRequestFromHeader ) == 0 ) 
        {
        return EDlAttrRequestFrom;
        }
    else if ( aHeader.CompareF( KHttpRequestHostHeader ) == 0 ) 
        {
        return EDlAttrRequestHost;
        }
    else if ( aHeader.CompareF( KHttpRequestMaxForwardsHeader ) == 0 ) 
        {
        return EDlAttrRequestMaxForwards;
        }
    else if ( aHeader.CompareF( KHttpRequestPragmaHeader ) == 0 ) 
        {
        return EDlAttrRequestPragma;
        }
    else if ( aHeader.CompareF( KHttpRequestRefererHeader ) == 0 ) 
        {
        return EDlAttrRequestReferer;
        }
    else if ( aHeader.CompareF( KHttpRequestUserAgentHeader ) == 0 ) 
        {
        return EDlAttrRequestUserAgent;
        }
    else if ( aHeader.CompareF( KHttpRequestVaryHeader ) == 0 ) 
        {
        return EDlAttrRequestVary;
        }
    */
    return 0;
    }
    
    
// ---------------------------------------------------------------------------
// Checks if the given header matches any of the general headers that have
// an predefined enumeration in the platform DL manager
// ---------------------------------------------------------------------------
//	           
TUint CCatalogsHttpDownload::MatchWithPredefinedGeneralHeader( 
    const TDesC8& aHeader ) const
    {
    /* HLa
    if ( aHeader.CompareF( KHttpGeneralCacheControlHeader ) == 0 ) 
        {
        return EDlAttrGeneralCacheControl;
        }    
    else if ( aHeader.CompareF( KHttpGeneralDateHeader ) == 0 ) 
        {
        return EDlAttrGeneralDate;
        }    
    else if ( aHeader.CompareF( KHttpGeneralPragmaHeader ) == 0 ) 
        {
        return EDlAttrGeneralPragma;
        }    
    else if ( aHeader.CompareF( KHttpGeneralViaHeader ) == 0 ) 
        {
        return EDlAttrGeneralVia;
        }    
    else if ( aHeader.CompareF( KHttpGeneralWarningHeader ) == 0 ) 
        {
        return EDlAttrGeneralWarning;
        }
        */    
    return 0;
    }
    
    
// ---------------------------------------------------------------------------
// Checks if the given header matches any of the entity headers that have
// an predefined enumeration in the platform DL manager
// ---------------------------------------------------------------------------
//	    
TUint CCatalogsHttpDownload::MatchWithPredefinedEntityHeader( 
    const TDesC8& aHeader ) const
    {
    /*
    if ( aHeader.CompareF( KHttpEntityAllowHeader ) == 0 ) 
        {
        return EDlAttrEntityAllow;
        }    
    else if ( aHeader.CompareF( KHttpEntityContentEncodingHeader ) == 0 ) 
        {
        return EDlAttrEntityContentEncoding;
        }
    else if ( aHeader.CompareF( KHttpEntityContentLanguageHeader ) == 0 ) 
        {
        return EDlAttrEntityContentLanguage;
        }
    else if ( aHeader.CompareF( KHttpEntityContentLocationHeader ) == 0 ) 
        {
        return EDlAttrEntityContentLocation;
        }
    else if ( aHeader.CompareF( KHttpEntityExpiresHeader ) == 0 ) 
        {
        return EDlAttrEntityExpires;
        }
    else if ( aHeader.CompareF( KHttpEntityLastModifiedHeader ) == 0 ) 
        {
        return EDlAttrEntityLastModified;
        }
        */
    return 0;
    }
        

// ---------------------------------------------------------------------------
// Adds the header from the pair to the target in the format used by
// platform's DL manager
// ---------------------------------------------------------------------------
//	      
void CCatalogsHttpDownload::AddRequestHeaderL( HBufC8*& aTarget, 
    const CCatalogsKeyValuePair& aPair )
    {
    DLTRACEIN((""));
    TInt headerLength = aPair.Key().Length() + 
            aPair.Value().Length() + 1;
    
    TInt newSize = 0;
    
    
    // Allocate or reallocate the buffer if necessary        
    if( !aTarget ) 
        {
        DLTRACE( ("Creating target buffer") );
        aTarget = HBufC8::NewL( headerLength );
        }
    else if( aTarget->Length() ) 
        {
        DLTRACE( ( "Reallocating target buffer with new length" ) );
        newSize = aTarget->Length() + headerLength + 1;
        aTarget = aTarget->ReAllocL( newSize );
        }
    else
        {
        DLTRACE( ("Reallocating target buffer") );
        aTarget = aTarget->ReAllocL( headerLength );
        }

    TPtr8 ptr( aTarget->Des() );
    
    _LIT8( KHttpFieldSeparator, "\n"); // HLa
    const TInt KColon( ':' ); // HLa
    
    if( newSize ) 
        {
        DLTRACE( ("Appending to existing headers") );
        // Append after the existing headers
        ptr.Append( KHttpFieldSeparator );
        ptr.Append( aPair.Key() );
        ptr.Append( KColon );
        ptr.Append( aPair.Value() );
        }
    else 
        {        
        DLTRACE( ("Creating the first header") );
        // Add the first header
        ptr.Append( aPair.Key() );
        ptr.Append( KColon );
        ptr.Append( aPair.Value() );
            
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//	      
void CCatalogsHttpDownload::SetTransferring( TBool aTransferring )
    {
    DLTRACEIN(("aTransferring: %d, iTransferring: %d", 
        aTransferring,
        iTransferring ));
        
    if ( iTransferring != aTransferring ) 
        {
        iTransferring = aTransferring;
        iOwner.ReportConnectionStatus( iTransferring );
        }
    }


// ---------------------------------------------------------------------------
// Updates the connection to download manager and starts the download
// ---------------------------------------------------------------------------
//	      
TInt CCatalogsHttpDownload::StartDownload()
    {
    DLTRACEIN((""));
    DASSERT( iConnection );
    
    TRAPD( err, 
        {
        if ( Config().ConnectionMethod().iType == ECatalogsConnectionMethodTypeDestination )
            {
            RCmManager cmManager;
            CleanupClosePushL( cmManager );
            cmManager.OpenL();
            RCmDestination dest = cmManager.DestinationL( Config().ConnectionMethod().iId );
            CleanupClosePushL( dest );
            HBufC* temp = dest.NameLC();    
            QString destname;
            destname = XQConversions::s60DescToQString( *temp );
            CleanupStack::PopAndDestroy( temp );
            CleanupStack::PopAndDestroy( &dest ); 
            CleanupStack::PopAndDestroy( &cmManager ); 
            if ( iOwner.GetDownloadManager() )
                {
                //iOwner.GetDownloadManager()->attribute(AccessPoints).toStringList();
                //iOwner.GetDownloadManager()->setAttribute(AccessPoint, destname);
                }
            }
            
        // Update the configuration 
        if ( iState.iProgressState == ECatalogsHttpNone ) 
            {
            DLTRACE(("Updating dl config"));
            InitializeDownloadL();
            }
        });
   
    if ( err == KErrNone ) 
        {
						 try
							{
							iDownload->start();
							}
			        catch(const std::exception& exception)
			        	{
							 qt_symbian_exception2LeaveL(exception);
			        	}
        }
    return err;
    }


// ---------------------------------------------------------------------------
// Starts a OMA DD download if necessary
// ---------------------------------------------------------------------------
//	      
TBool CCatalogsHttpDownload::StartDescriptorDownloadL() 
    {
    DLTRACEIN((""));
    if ( iDdDownload )         
        {
        // This ensures that headers are re-get if the op is paused
        iMode = ECatalogsHttpHeaderModeForceHead;
        DLTRACE(("Content type matches DD download"));        
        HBufC* fullName = iConfig->FullPathLC();
        
        DLTRACE(("Opening target file"));
        User::LeaveIfError( iDdFile.Replace( iFs, *fullName, EFileWrite ) );
        CleanupStack::PopAndDestroy( fullName );
        
        DLTRACE(("Creating the transaction"));    
        // Download the descriptor without DL manager
        // Use normal URI because transaction encodes it 
        iTransaction = iOwner.CreateDlTransactionL( 
            Uri(), 
            *this, 
            *iConfig );
            
        iTransaction->Config().SetHttpMethod( ECatalogsHttpGet );
        iTransaction->Start();                

        return ETrue;
        }
        
    DLTRACEOUT(("Not a DD download"));
    return EFalse;
    }


// ---------------------------------------------------------------------------
// Delete downloaded files
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpDownload::DeleteFiles()
    {
    DLTRACEIN((""));
    TInt err = KErrNotFound;
    if ( ContainsData( iTempFilename ) )
        {
        DLTRACE(( _L("Deleting the temp file: %S"), iTempFilename ));
        err = BaflUtils::DeleteFile( iFs, *iTempFilename );
        }
    
    // only delete target file if the temp file was not found in case
    // we were re-downloading something
    if ( err == KErrNotFound && 
         // ensures that we don't accidentally delete whole directories
         iConfig->Directory().Length() &&
         iConfig->Filename().Length() ) 
        {
        TPath path;
        path.Append( iConfig->Directory() );               
        path.Append( iConfig->Filename() );
    
        DLTRACE(( _L("Deleting the final file: %S"), &path ));
        BaflUtils::DeleteFile( iFs, path );
        }    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpDownload::MoveFileL()
    {
    DLTRACEIN((""));
    if ( !iTempFilename ) 
        {
        DLERROR(("Tempfilename has not been set, leaving" ));
        User::Leave( KErrGeneral );
        }
    
    if ( !iConfig->Filename().Length() ) 
        {    
        // Update filename extension 
        if ( iConfig->Options() & ECatalogsHttpDisableHeadRequest )
            {
            UpdateExtensionL();    
            }
        else
            {
            DLTRACE(("No target filename, no need to move" ));    
            UpdateFilenameL();
            return;
            }        
        }
 
    HBufC* target = iConfig->FullPathLC();
    if ( *target != *iTempFilename ) 
        {
        DLTRACE(( _L("Moving %S to %S"), iTempFilename, target ));
        CFileMan* fileMan = CFileMan::NewL( iFs );
        CleanupStack::PushL( fileMan );
         
        User::LeaveIfError( 
            fileMan->Move( *iTempFilename, *target, CFileMan::EOverWrite ) );
        CleanupStack::PopAndDestroy( fileMan );
        }
    CleanupStack::PopAndDestroy( target );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpDownload::ParseFilenameFromUriL()
    {
    DLTRACEIN((""));
    if ( iConfig->Filename().Length() ) 
        {
        DLTRACEOUT(("Filename already set"));
        return;
        }
    
    DASSERT( iUri );
    // Note: Download manager actually does this same stuff, more or less, 
    //(done still in 3.1.50) but it's a bit clearer if we do it by ourselves. 
    TUriParser8 uriParser;
    User::LeaveIfError( uriParser.Parse( Uri() ) );
    HBufC* filename = uriParser.GetFileNameL( EUriFileNameTail );
    DLTRACE(( _L("Parsed filename %S"), filename ));
    CleanupStack::PushL( filename );
    iConfig->SetFilenameL( *filename );
    CleanupStack::PopAndDestroy( filename );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TBool CCatalogsHttpDownload::ContainsData( const HBufC* aDes ) const
    {
    return aDes && aDes->Length(); 
    }


// ---------------------------------------------------------------------------
// Updates file extension 
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpDownload::UpdateExtensionL()
    {
    DLTRACEIN((""));
    // UpdateFilenameL parses the filename from iTempFilename which contains
    // the whole path and sets it to iConfig->Filename()
    UpdateFilenameL();
    
    TFileName filename = iConfig->Filename();
    TDataType type( ContentType() );
    
    // Doc handler does not yet support Widget extension change
    TFileName mimeType;
    mimeType.Copy(type.Des8());
    if ( mimeType.Compare(KMimeTypeMatchWidget) == 0 )
            {
            ReplaceExtension( filename, KWidgetExtension );
            }
    else
        {
        ReplaceExtension( filename, KNativeExtension );
        }
    
    iConfig->SetFilenameL( filename );        
    }


// ---------------------------------------------------------------------------
// Forces HEAD if content-type requires so
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpDownload::UpdateHeadRequirement()
    {
    DLTRACEIN((""));
    if ( ContentType().MatchF( KMimeTypeMatchOdd8 ) != KErrNotFound ||
         ContentType().MatchF( KMimeTypeMatchDrm8 ) != KErrNotFound ||
         ContentType().MatchF( KMimeTypeMatchJad8 ) != KErrNotFound )
        {
        DLTRACE(("Content is DD/JAD/DRM, forcing HEAD"));
        // Clear HEAD flag
        iConfig->SetOptions( 
            iConfig->Options() & ~ECatalogsHttpDisableHeadRequest );
        
        SetHeaderMode( ECatalogsHttpHeaderModeForceHead );
        }
    }
    
    
// ---------------------------------------------------------------------------
// Sets delete status and id to the platform download as user data
// ---------------------------------------------------------------------------
//  
TInt CCatalogsHttpDownload::SetDeleteState( 
    const TDownloadDeleteState aStatus )
    {
    DLTRACEIN(( "aStatus: %x, id: %x", aStatus, OperationId().Id() ));
    
    TInt err = KErrNone;
    if ( iDownload ) 
        {    

        iStatus = aStatus;
        }
    
    return err;
    }

CCatalogsHttpDownload::TDownloadDeleteState CCatalogsHttpDownload::GetStatusState()
	{
		return iStatus;
	}
// ---------------------------------------------------------------------------
// Gets delete status and download id from the platform download
// ---------------------------------------------------------------------------
//  
TInt CCatalogsHttpDownload::GetDeleteState( 
    TDownloadDeleteState& aStatus )
    {
    DLTRACEIN((""));
    TInt32 data = 0;

    TInt err = KErrNotReady;
    if ( iDownload ) 
        {

        err = iStatus;
        DLTRACE(("Data: %d", data));
        aStatus = static_cast<TDownloadDeleteState>( data );
        }
    DLTRACEOUT(("status: %x", aStatus ));
    return err;
    }
    

// ---------------------------------------------------------------------------
// Initializes download
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpDownload::InitializeDownloadL()
    {
    DLTRACEIN((""));
    if ( !iFileIsSet  ) 
        {
        RFile file;
        CleanupClosePushL( file );
        BaflUtils::EnsurePathExistsL( iFs, iConfig->Directory() );

        if ( !iTempFilename->Length() ) 
            {
            // Create a temp name & file 
            DLTRACE(("Creating temp file"));
            
            TFileName filename;
            // Create a temp name
            User::LeaveIfError( file.Temp( 
                iFs, iConfig->Directory(), filename, EFileWrite ) );        
                            
            DLTRACE(( _L("Temp filename: %S"), &filename ));
            AssignDesL( iTempFilename, filename );
            }
        else // tempfilename has been created, use it 
            {
            DLTRACE(( _L("Using path: %S"), iTempFilename ));

            TRAPD( err, 
                OpenOrCreateFileL( iFs, file, *iTempFilename, EFileWrite ) );
            
            DLTRACE(("File open err: %d", err));
            if ( err == KErrNone )
                {
                // Seek to end so that download manager doesn't overwrite
                // already downloaded parts
                TInt pos = 0;
                file.Seek( ESeekEnd, pos );
                }
            else if ( err == KErrInUse )
                {
                // File is (hopefully) used by DL manager because it hasn't
                // exited after we paused the download and thus we can't
                // set the file handle to the download because it already exists
                iFileIsSet = ETrue;
                }
            else
                {
                User::Leave( err );
                }
            }

        if ( !iDownload )
            {
            DLTRACE(("Creating RHttpDownload"));
            iDownload = &iOwner.CreatePlatformDownloadL( EncodedUri() );
            
            // Platform download's id is used as a secondary id
            UpdateSecondaryIdL();
            
            // Sets delete status and also updates id to the download
            User::LeaveIfError( SetDeleteState( EDownloadCanBeDeleted ) );

            // Update the request headers to the download manager but do it
            // only once
            UpdateRequestHeadersL();                    
            }
                
        // Check if the 
        //download has been set progressive
        /* MTA
         * TBool progressive = EFalse;
		try
		{
		         progressive =iDownload->attribute(Progressive).toBool(); 
		}
        catch(const std::exception& exception)
        	{
				 qt_symbian_exception2LeaveL(exception);
        	}  

        
        // We want to have progressive downloads because then DL manager uses
        // a smaller download buffer which makes progress events much more
        // frequent. However, something goes wrong if the progressive flag is
        // set twice so we have to prevent that. Also we don't need progress
        // events for icons so we use the download priority to determine whether
        // we use progressive dls or not
        if ( !progressive && 
             iConfig->Priority() >= ECatalogsPriorityMedium && 
             // for some reason EDlAttrProgressive returns EFalse after 
             // we restart engine but DL manager stays up. We must not
             // set the flag if it has already been set because it 
             // messes the download
             !iFileIsSet ) 
            {
            DLTRACE(("Setting download as progressive"));
						try
						{
						   iDownload->setAttribute(Progressive,ETrue);
						}
						catch(const std::exception& exception)
						{
							qt_symbian_exception2LeaveL(exception);
						}  
						}
        
        // Doesn't pause the download after content type has been received
					try
					{
						iDownload->setAttribute(Progressive,ETrue);
					}
					catch(const std::exception& exception)
					{
						qt_symbian_exception2LeaveL(exception);
					}  
**/
        if ( !iFileIsSet )
            {
            DLTRACE(("Setting filehandle to download"));
           // User::LeaveIfError( iDownload->SetFileHandleAttribute( file ) );
            file.Close();
            TInt ret = BaflUtils::DeleteFile( iFs, *iTempFilename );
            iDownload->setAttribute(DestinationPath,XQConversions::s60DescToQString(iConfig->Directory()));
            iDownload->setAttribute(FileName, XQConversions::s60DescToQString(*iTempFilename));
            iFileIsSet = ETrue;                        
            }

        CleanupStack::PopAndDestroy( &file );        
        }        
    }

Download* CCatalogsHttpDownload::GetDownload()
{
	return iDownload;
}
// ---------------------------------------------------------------------------
// Delete platform download and reset secondary id
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpDownload::DeletePlatformDownload()
    {
    if ( iDownload )
        {
        iOwner.GetDownloadManager()->removeOne(iDownload);
        iDownload = NULL;
        iId.SetSecondaryId( KErrNotFound );
        }
    }


// ---------------------------------------------------------------------------
// Updates secondary id from the platform download
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpDownload::UpdateSecondaryIdL()
    {
    TInt32 secondaryId = KErrNotFound;
    secondaryId = iDownload->id();
    iId.SetSecondaryId( secondaryId );    
    }


// ---------------------------------------------------------------------------
// Encodes the URI
// ---------------------------------------------------------------------------
//              
void CCatalogsHttpDownload::EncodeUriL()
    {
    DeletePtr( iEncodedUri );
    iEncodedUri = CatalogsHttpUtils::EncodeUriL( Uri() );
    }


// ---------------------------------------------------------------------------
// Returns the encoded URI
// ---------------------------------------------------------------------------
//              
const TDesC8& CCatalogsHttpDownload::EncodedUri() const
    {
    DASSERT( iEncodedUri );
    return *iEncodedUri;
    }

// ---------------------------------------------------------
// CCatalogsHttpDownload::ReplaceExtension()
// Replace current extension at aName with extension given (eExt).
// ---------------------------------------------------------
//      
void CCatalogsHttpDownload::ReplaceExtension( TDes& aName, const TDesC& aExt )
    {
    
    TInt dotPos = aName.LocateReverse( '.' );
    if ( dotPos != KErrNotFound )
       {
       aName.Delete( dotPos, aName.Length()- dotPos );
       aName.Append( aExt );
       }
    }
    
CCatalogsHttpQTDownload::CCatalogsHttpQTDownload(CCatalogsHttpDownload* aHttpDownload,Download* aDownload)
	{
		iCatalogsHttpDownload = aHttpDownload;
		iDownload = aDownload;
		connect(iDownload, SIGNAL(downloadEvent(DownloadEvent*)), this,SLOT(downloadEventHandler(DownloadEvent*)));
        connect(iDownload, SIGNAL(downloadError(Error)), this,SLOT(downloadErrorHandler(Error)));
	}
	
void CCatalogsHttpQTDownload::downloadEventHandler(DownloadEvent* aEvent)
	{
		iCatalogsHttpDownload->HandledownloadEventL(*aEvent);
	}
void CCatalogsHttpQTDownload::downloadErrorHandler(Error)
	{
		//iCatalogsHttpDownload->HandledownloadEventL(*aEvent);
	}


