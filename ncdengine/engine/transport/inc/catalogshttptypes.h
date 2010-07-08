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


#ifndef T_CATALOGSHTTPTYPES_H
#define T_CATALOGSHTTPTYPES_H

// #include <HttpDownloadMgrCommon.h>  // HLa: DLMgr remove

class MCatalogsHttpOperation;

/**
* HTTP events
*/

/**
* HTTP operation states
*/
enum TCatalogsHttpOperationState
    {
    /**
    * The operation has been created. This is the initial state
    * of the operation and no event will be sent for it.
    */
    ECatalogsHttpOpCreated,
    
    /**
    * The operation is in progress
    */
    ECatalogsHttpOpInProgress,
    
    /**
    * The operation is on pause. 
    * The operation is not automatically resumed from this state
    */
    ECatalogsHttpOpPaused,
    
    /**
    * The operation is in the operation queue waiting for its
    * execution. 
    *
    * The difference between ECatalogsHttpOpPaused
    * and ECatalogsHttpOpQueued is that the operation goes to the
    * former state if the client pauses the operation. The latter state
    * is entered automatically if the limit for concurrently executing
    * operations would be otherwise exceeded.
    *
    * The operation will be automatically resumed.
    */
    ECatalogsHttpOpQueued,
    
    /**
    * The operation is complete
    */
    ECatalogsHttpOpCompleted,
    
    /**
    * The operation failed
    * 
    * This event is not sent to observers. Handle failures in HandleHttpError()
    * Transport uses this internally.
    */
    ECatalogsHttpOpFailed,
    
    
    /**
    * Not supported yet
    * @ Fix this
    */
    ECatalogsHttpOpMediaRemoved,
    ECatalogsHttpOpMediaInserted,
    
    /**
    * The operation can be paused. Event for this will be sent
    * only after ECatalogsHttpOpNonPausable
    */
    ECatalogsHttpOpPausable,
    
    /**
    * The operation cannot currently be paused
    */
    ECatalogsHttpOpNonPausable,
    
    /**
    * The operation is already running
    */
    ECatalogsHttpOpAlreadyRunning,
    
    /**
    * The operation is being deleted
    */
    ECatalogsHttpOpDeleting    
    };
   
    
/**
* HTTP operation state progress
*/  
enum TCatalogsHttpProgressState
    {
    /**
    * Initial state of the operation
    */
    ECatalogsHttpNone,
    
    /**
    * The operation has started.
    * Event about this is sent only for downloads
    */
    ECatalogsHttpStarted,

    /**
    * The operation is creating a network connection.
    * Event about this is sent only for downloads
    */
    ECatalogsHttpCreatingConnection,

    /**
    * The operation has created a network connection
    * with the server.
    */
    ECatalogsHttpConnected,
    
    /**
    * Network connection has been suspended.
    * Event about this is sent only for downloads
    */
    ECatalogsHttpConnectionSuspended,

    /**
    * Network connection was disconnected.
    * Event about this is sent only for downloads
    */
    ECatalogsHttpDisconnected,

    /**
    * The download has started
    * Event about this is sent only for downloads
    */
    ECatalogsHttpDownloadStarted,
    
    
    /**
    * The operation has received the content type header
    * @note There won't be a separate ECatalogsHttpResponseHeaderReceived
    * event for content type receival.
    */
    ECatalogsHttpContentTypeReceived,
    
    /**
    * Event about this is sent only for downloads
    */    
    ECatalogsHttpSubmitIssued,
    
    /**
    * The operation has received a response header
    */
    ECatalogsHttpResponseHeaderReceived,
    
    /**
    * The operation has received a part of the response body.
    *
    * Transaction observers should request the body when handling
    * this event since the body will be lost after the handler exits.
    *
    * For downloads the body is empty since it is written straight to
    * a file.
    */
    ECatalogsHttpResponseBodyReceived,

    /**
    * The download has been redirected permanently.
    * Transactions don't support redirection.
    * Event about this is sent only for downloads
    */
    ECatalogsHttpRedirectedPermanently,

    /**
    * The download has been redirected temporarily. It shouldn't be
    * paused because there's no guarantee that it can be resumed correctly
    * Transactions don't support redirection.
    * Event about this is sent only for downloads
    */
    ECatalogsHttpRedirectedTemporarily,

    /**
    * 
    * Event about this is sent only for downloads
    */
    ECatalogsHttpNameChanged,

    /**
    * 
    * Event about this is sent only for downloads
    */
    ECatalogsHttpContentTypeChanged,

    /**
    * 
    * Event about this is sent only for downloads
    */
    ECatalogsHttpMovingContentFile,

    /**
    * 
    * Event about this is sent only for downloads
    */
    ECatalogsHttpContentFileMoved,
    
    ECatalogsHttpDone = ECatalogsHttpContentFileMoved
    
    };



/**
 * Error types
 */
enum TCatalogsHttpErrorType
    {
    ECatalogsHttpErrorGeneral = 0,
    ECatalogsHttpErrorHttp
    };


/**
* HTTP event
*/
struct TCatalogsHttpEvent
    {
    /// State of the operation
    TCatalogsHttpOperationState iOperationState;
    
    /// Progress of the operation state
    TCatalogsHttpProgressState iProgressState;  
    
    TCatalogsHttpEvent( const TCatalogsHttpOperationState& aOpState, 
        const TCatalogsHttpProgressState& aPrState ) :
        iOperationState( aOpState ), iProgressState( aPrState )
        {
        }
    };
    
    
/**
 * HTTP erros
 */    
struct TCatalogsHttpError
    {
    /** 
     * Defines the interpretation for iError    
     */
    TCatalogsHttpErrorType iType;
    
    /**
     * Error code. 
     * 
     * If iType is ECatalogsHttpErrorGeneral then this is a Symbian error
     * code.
     * If iType is ECatalogsHttpErrorHttp then this is a HTTP status code
     */
    TInt iError;
    
    TCatalogsHttpError( const TCatalogsHttpErrorType& aType, TInt aError ) :
        iType( aType ), iError( aError )
        {
        }
    };

/**
* HTTP methods
*/    
enum TCatalogsHttpMethod
    {
    ECatalogsHttpGet = 0,
    ECatalogsHttpPost,
    ECatalogsHttpPut,
    ECatalogsHttpHead
    };


/**
* HTTP operation types
*/    
enum TCatalogsHttpOperationType
    {
    ECatalogsHttpTransaction = 0,
    ECatalogsHttpDownload,
    ECatalogsHttpUpload
    };
    
            

/**
* HTTP configuration events
* INTERNAL
* @see MCatalogsHttpConfigObserver
* @see MCatalogsHttpConfig
*/
enum TCatalogsHttpConfigEvent
    {
    ECatalogsHttpCfgPriorityChanged = 0
    };


/**
* HTTP transaction types
* INTERNAL
*/
enum TCatalogsHttpTransactionType
    {
    ECatalogsHttpTransactionNormal = 0,
    ECatalogsHttpTransactionDownload
    };


/**
* Modes for getting HTTP headers in downloads
* INTERNAL
*/
enum TCatalogsHttpHeaderMode
    {
    ECatalogsHttpHeaderModeDefault = 0,
    ECatalogsHttpHeaderModeForceHead,
    ECatalogsHttpHeaderModeNoHead
    };

/**
* Maximum length for content types
*/
const TUint KMaxContentTypeLength = 256; // HLa: DLMgr remove
const TUint KCatalogsHttpMaxContentTypeLength = KMaxContentTypeLength;


/**
* The operation has been queued
*/
const TInt KCatalogsHttpOperationQueued = 1;

/**
* Type for HTTP operation arrays
*/    
typedef RPointerArray<MCatalogsHttpOperation> 
    RCatalogsHttpOperationArray;
    

const TUint KCatalogsErrorHeaderPresent( 1234 );

_LIT8( KCatalogsHeaderErrorNumber, "catalogs-client-error-number" );
_LIT8( KCatalogsHeaderErrorMessage, "catalogs-client-error-message" );

// HTTP methods
_LIT8( KCatalogsHttpPost, "POST" );
_LIT8( KCatalogsHttpGet,  "GET" );
_LIT8( KCatalogsHttpHead,  "HEAD" );

_LIT8( KCatalogsHttpHeaderHost, "Host" );

_LIT8( KCatalogsHttpHeaderContentDisposition, "Content-Disposition" );

/**
 * Options that can be set with MCatalogsHttpConfig::SetOptions()
 */
enum TCatalogsHttpOptions 
    {
    ECatalogsHttpDisableHeadRequest = 1
    };
        
#endif // T_CATALOGSHTTPTYPES_H
