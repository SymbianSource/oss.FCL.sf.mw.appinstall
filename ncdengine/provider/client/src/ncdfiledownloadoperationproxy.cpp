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
* Description:   Implements CNcdFileDownloadOperationProxy
*
*/


#include <s32strm.h>
#include "ncdfiledownloadoperationproxy.h"
#include "ncdfiledownloadoperationobserver.h"
#include "catalogsdebug.h"
#include "ncddownloadoperationstates.h"
#include "ncdoperationproxyremovehandler.h"
#include "catalogsclientserver.h"
#include "catalogsutils.h"
#include "ncdnodeproxy.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdFileDownloadOperationProxy* CNcdFileDownloadOperationProxy::NewL( 
    MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    CNcdNodeManagerProxy* aNodeManager,
    const TDesC& aTargetFilename,
    MNcdFileDownloadOperationObserver* aObserver,
    TNcdDownloadDataType aDataType )
    {
    CNcdFileDownloadOperationProxy* self = 
        CNcdFileDownloadOperationProxy::NewLC( 
        aSession,
        aHandle,
        aRemoveHandler,
        aNode,
        aNodeManager,
        aTargetFilename,
        aObserver,
        aDataType );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CNcdFileDownloadOperationProxy* CNcdFileDownloadOperationProxy::NewLC( 
    MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    CNcdNodeManagerProxy* aNodeManager,
    const TDesC& aTargetFilename,
    MNcdFileDownloadOperationObserver* aObserver,
    TNcdDownloadDataType aDataType )
    {
    CNcdFileDownloadOperationProxy* self =
        new( ELeave ) CNcdFileDownloadOperationProxy( aDataType );    
    
    self->AddRef();
    CleanupReleasePushL( *self );        
    self->ConstructL( aSession, aHandle, aRemoveHandler,
        aNode, aNodeManager, aObserver, aTargetFilename );
    return self;
    }


// ---------------------------------------------------------------------------
// From MNcdFileDownloadOperation
// 
// ---------------------------------------------------------------------------
//
const TDesC& CNcdFileDownloadOperationProxy::TargetFileName() const
    {
    DASSERT( iTargetFilename );
    return *iTargetFilename;
    }


// ---------------------------------------------------------------------------
// From MNcdFileDownloadOperation
// 
// ---------------------------------------------------------------------------
//
void CNcdFileDownloadOperationProxy::PauseL()
    {
    DLTRACEIN( ( "" ) );
    HBufC8* data = NULL;
    
    TState state = OperationStateL();
    // Check that the operation is in a resumable state    
    if ( state == EStateCancelled ) 
        {
        DLERROR(("Cannot resume! Operation has been cancelled. Leaving"));
        User::Leave( KErrCancel );
        }
    
    // Operation must be running for pause to work    
    if ( state != EStateRunning || iIsPaused ) 
        {     
        DLTRACEOUT(("Operation complete or other unresumable state"));   
        return;
        }    
    
    ClientServerSession().SendSyncAlloc( ENCDOperationFunctionPause,
        KNullDesC8(),
        data,
        Handle(),
        0 );
    
    if ( data ) 
        {
        DLTRACE(("Received progress information"));
        
        RDesReadStream stream( *data );
        CleanupStack::PushL( data );
        CleanupClosePushL( stream );

        // Just read completionId from the stream
        stream.ReadInt32L();
        
        TNcdSendableProgress progress;
        // Read progress info
        progress.InternalizeL( stream );
        
        // Update paused state
        iIsPaused = ( progress.iState == ENcdDownloadPaused );        
        
        CleanupStack::PopAndDestroy( 2, data ); // stream, data        
        }
                    
    SetState( EStateRunning );     
    DLTRACEOUT( ( "IsPaused: %d", iIsPaused ) );    
    }


// ---------------------------------------------------------------------------
// From MNcdFileDownloadOperation
// 
// ---------------------------------------------------------------------------
//
void CNcdFileDownloadOperationProxy::ResumeL()
    {
    DLTRACEIN(( "" ));
    HBufC8* data = NULL;
    TState state = OperationStateL();

    // Check that the operation is in a resumable state    
    if ( state == EStateCancelled ) 
        {
        DLERROR(("Cannot resume! Operation has been cancelled. Leaving"));
        User::Leave( KErrCancel );
        }
        
    if ( ( state != EStateStopped && state != EStateRunning ) || !iIsPaused )
        {     
        DLTRACEOUT(("Operation complete or other unresumable state"));   
        return;
        }
    
    ClientServerSession().SendSyncAlloc( ENCDOperationFunctionResume,
        KNullDesC8(),
        data,
        Handle(),
        0 );
        
    if ( data ) 
        {
        DLTRACE(("Received progress information"));
        RDesReadStream stream( *data );
        CleanupStack::PushL( data );
        CleanupClosePushL( stream );

        // Just read the completionId from the stream
        stream.ReadInt32L();
        
        TNcdSendableProgress progress;
        // Read progress info
        
        progress.InternalizeL( stream );
        // Update paused-state
        iIsPaused = ( progress.iState == ENcdDownloadPaused );
            
        DLTRACE(("Sending continue message"));
        if ( progress.iState == KNcdDownloadContinueMessageRequired  )
            {
            SendContinueMessageL();
            }        
        
        CleanupStack::PopAndDestroy( 2, data ); // stream, data        
        }
                
    SetState( EStateRunning );     
    DLTRACEOUT(( "" ));
    
    }


// ---------------------------------------------------------------------------
// From MNcdFileDownloadOperation
// 
// ---------------------------------------------------------------------------
//
TBool CNcdFileDownloadOperationProxy::IsPaused()
    {
    return iIsPaused;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CNcdFileDownloadOperationProxy::IsPausableL()
    {
    DLTRACEIN((""));
    TInt data = KErrNone;
    User::LeaveIfError( ClientServerSession().SendSync( 
        ENCDOperationFunctionGetData,
        KNullDesC8(),
        data,
        Handle() ) );
    
    DLTRACEOUT(("pausable: %d", data == KNcdDownloadIsPausable ));
    return data == KNcdDownloadIsPausable;
    }


// ---------------------------------------------------------------------------
// Download type getter
// ---------------------------------------------------------------------------
//
TNcdDownloadDataType CNcdFileDownloadOperationProxy::DownloadDataType() const
    {
    return iDataType;
    }


// ---------------------------------------------------------------------------
// From MNcdOperation
// Operation type getter
// ---------------------------------------------------------------------------
//
TNcdInterfaceId CNcdFileDownloadOperationProxy::OperationType() const
    {
    return static_cast<TNcdInterfaceId>(
        MNcdFileDownloadOperation::KInterfaceUid );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdFileDownloadOperationProxy::CNcdFileDownloadOperationProxy( 
    TNcdDownloadDataType aDataType )
    : CNcdOperation< MNcdFileDownloadOperation >( NULL ), iIsPaused( EFalse ),
       iDataType( aDataType )
    {
    }



// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdFileDownloadOperationProxy::~CNcdFileDownloadOperationProxy()
    {
    DLTRACEIN( ( "" ) );    
    delete iTargetFilename; 
    DASSERT( iRemoveHandler ); 
    if ( iRemoveHandler )
        {        
        DLTRACE(("Removing proxy from remove handler"));
        iRemoveHandler->RemoveOperationProxy( *this );
        }
      
    DLTRACEOUT( ( "" ) );    
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdFileDownloadOperationProxy::ConstructL( MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    CNcdNodeManagerProxy* aNodeManager,
    MNcdFileDownloadOperationObserver* aObserver,
    const TDesC& aTargetFilename )
    {
    DLTRACEIN( ( "" ) );
    CNcdBaseOperationProxy::ConstructL( aSession, aHandle, aRemoveHandler,
        aNode, aNodeManager );
    iObserver = aObserver;
    iTargetFilename = aTargetFilename.AllocL();
    
    // Initialize the operation if necessary. Initialization mechanism is used
    // here to transfer the targetfilename to serverside unless it is
    // transferred in node id    
    if ( aNode ) 
        {        
        InitializeOperationL();
        }
    DLTRACEOUT( ( "" ) );    
    }
    
    
// ---------------------------------------------------------------------------
// Handle progress callback
// ---------------------------------------------------------------------------
//
void CNcdFileDownloadOperationProxy::ProgressCallback()
    {
    DLTRACEIN( ( "" ) );
    if ( iObserver ) 
        {
        
        TNcdSendableProgress& sendableProgress( SendableProgress() );
        TNcdProgress progress( sendableProgress.iProgress, 
                               sendableProgress.iMaxProgress );
                       
        iObserver->FileDownloadProgress( *this, progress );
        }
    DLTRACEOUT( ( "" ) );
    }
    
    
// ---------------------------------------------------------------------------
// Handle query received callback
// ---------------------------------------------------------------------------
//
void CNcdFileDownloadOperationProxy::QueryReceivedCallback( CNcdQuery* /*aQuery*/ )
    {
    DLTRACEIN( ( "" ) );
    DLTRACEOUT( ( "" ) );
    }
    
    
// ---------------------------------------------------------------------------
// Handle operation complete
// ---------------------------------------------------------------------------
//
void CNcdFileDownloadOperationProxy::CompleteCallback( TInt aError )
    {
    AddRef();
    DLTRACEIN( ( "Error: %d", aError ) );
    iIsPaused = EFalse;
    
    if ( iObserver ) 
        {        
        iObserver->OperationComplete( *this, aError );
        }

    Release();
    DLTRACEOUT( ( "" ) );
    }
    

// ---------------------------------------------------------------------------
// Create initialization data sent to the server-side
// ---------------------------------------------------------------------------
//
HBufC8* CNcdFileDownloadOperationProxy::CreateInitializationBufferL()
    {
    DLTRACEIN( ( "" ) );
    DASSERT( iTargetFilename );
    
    HBufC8* buf = HBufC8::NewLC( 2 * KMaxPath );
    
    TPtr8 ptr( buf->Des() );
    RDesWriteStream stream( ptr );
    CleanupClosePushL( stream );
    ExternalizeDesL( *iTargetFilename, stream );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::Pop( buf );    
    return buf;
    }


// ---------------------------------------------------------------------------
// Handle initialization callback
// ---------------------------------------------------------------------------
//
void CNcdFileDownloadOperationProxy::InitializationCallback( 
    RReadStream& /* aReadStream */, TInt /* aDataLength */ )
    {
    DLTRACEIN( ( "" ) );
    }

    
    
