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
* Description:   Implements CNcdDownloadOperationProxy
*
*/


#include <s32strm.h>
#include "ncddownloadoperationproxy.h"
#include "ncddownloadoperationobserver.h"
#include "catalogsdebug.h"
#include "ncddownloadoperationstates.h"
#include "ncdoperationproxyremovehandler.h"
#include "catalogsclientserver.h"
#include "ncdnodeproxy.h"
#include "ncdnodemanagerproxy.h"
#include "catalogsutils.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdDownloadOperationProxy* CNcdDownloadOperationProxy::NewL( 
    MCatalogsClientServer& aSession,
    TNcdDownloadDataType aDownloadType,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    MNcdDownloadOperationObserver* aObserver,
    CNcdNodeManagerProxy* aNodeManager,
    TInt aDownloadIndex,
    TBool aIsRunning )
    {
    CNcdDownloadOperationProxy* self = CNcdDownloadOperationProxy::NewLC( 
        aSession,
        aDownloadType,
        aHandle,
        aRemoveHandler,
        aNode,
        aObserver,
        aNodeManager,
        aDownloadIndex, 
        aIsRunning );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdDownloadOperationProxy* CNcdDownloadOperationProxy::NewLC( 
    MCatalogsClientServer& aSession,
    TNcdDownloadDataType aDownloadType,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    MNcdDownloadOperationObserver* aObserver,
    CNcdNodeManagerProxy* aNodeManager,
    TInt aDownloadIndex,
    TBool aIsRunning )
    {
    CNcdDownloadOperationProxy* self =
        new( ELeave ) CNcdDownloadOperationProxy( aDownloadType, 
            aDownloadIndex );
    
    self->AddRef();
    CleanupReleasePushL( *self );    
    self->ConstructL( aSession, aHandle, aRemoveHandler, aNode,
        aObserver, aNodeManager, aIsRunning );
    return self;
    }


// ---------------------------------------------------------------------------
// From MNcdDownloadOperation
// 
// ---------------------------------------------------------------------------
//
TInt CNcdDownloadOperationProxy::FileCount()
    {
    return iFileCount;
    }


// ---------------------------------------------------------------------------
// From MNcdDownloadOperation
// 
// ---------------------------------------------------------------------------
//
TInt CNcdDownloadOperationProxy::CurrentFile()
    {
    return iCurrentFile;
    }


// ---------------------------------------------------------------------------
// From MNcdDownloadOperation
// 
// ---------------------------------------------------------------------------
//
void CNcdDownloadOperationProxy::PauseL()
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
// From MNcdDownloadOperation
// 
// ---------------------------------------------------------------------------
//
void CNcdDownloadOperationProxy::ResumeL()
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
        else if ( progress.iState == KNcdDownloadStartMessageRequired ) 
            {
            DLTRACE(("Start the operation"));
            SetState( EStateStopped );
            DoStartOperationL();
            }
        
        CleanupStack::PopAndDestroy( 2, data ); // stream, data        
        }
                
    SetState( EStateRunning );     
    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// From MNcdDownloadOperation
// 
// ---------------------------------------------------------------------------
//
TBool CNcdDownloadOperationProxy::IsPaused()
    {
    return iIsPaused;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CNcdDownloadOperationProxy::IsPausableL()
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
// From MNcdDownloadOperation
// Observer adder
// ---------------------------------------------------------------------------
//
void CNcdDownloadOperationProxy::AddObserverL( 
    MNcdDownloadOperationObserver& aObserver )
    {
    DLTRACEIN( ( "this: %X, aObserver: %X", this, &aObserver ) );
    TInt err = iObservers.InsertInAddressOrder( &aObserver );
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
    }


// ---------------------------------------------------------------------------
// From MNcdDownloadOperation
// Observer remover
// ---------------------------------------------------------------------------
//
TBool CNcdDownloadOperationProxy::RemoveObserver( 
    MNcdDownloadOperationObserver& aObserver )
    {
    DLTRACEIN( ( "this: %X, aObserver: %X", this, &aObserver ) );
    // observer must exist
    
    TInt index = iObservers.FindInAddressOrder( &aObserver );
    if ( index != KErrNotFound )
        {
        DLTRACE(("Removing observer"));
        iObservers.Remove( index );
        return ETrue;
        }
    DLTRACE(("Observer didn't exist"))
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From MNcdOperation
// Operation type getter
// ---------------------------------------------------------------------------
//
TNcdInterfaceId CNcdDownloadOperationProxy::OperationType() const
    {
    return static_cast<TNcdInterfaceId>(MNcdDownloadOperation::KInterfaceUid);
    }


// ---------------------------------------------------------------------------
// Download type getter
// ---------------------------------------------------------------------------
//
TNcdDownloadDataType CNcdDownloadOperationProxy::DownloadDataType() const
    {
    return iDownloadType;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdDownloadOperationProxy::CNcdDownloadOperationProxy(
    TNcdDownloadDataType aDownloadType,
    TInt aDownloadIndex )
    : CNcdOperation< MNcdDownloadOperation >( NULL ), iDownloadType( aDownloadType ),
      iFileCount( 1 ), iCurrentFile( aDownloadIndex + 1 ), iIsPaused( EFalse )
    {
    }



// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdDownloadOperationProxy::~CNcdDownloadOperationProxy()
    {
    DLTRACEIN(( "this: %X", this )); 
    iObservers.Close(); 
    DASSERT( iRemoveHandler );
    DLTRACE(("Removing proxy from remove handler"));
    if ( iRemoveHandler )
        iRemoveHandler->RemoveOperationProxy( *this );
    
    DLTRACEOUT(( "" ));    
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdDownloadOperationProxy::ConstructL( MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    MNcdDownloadOperationObserver* aObserver,
    CNcdNodeManagerProxy* aNodeManager, 
    TBool aIsRunning )
    {
    DLTRACEIN( ( "this: %X", this ) );
    CNcdBaseOperationProxy::ConstructL( aSession, aHandle, aRemoveHandler,
        aNode, aNodeManager );
    
    if ( aObserver ) 
        {                
        AddObserverL( *aObserver );
        }        
        
    TNcdSendableProgress& sendableProgress( SendableProgress() );
    sendableProgress.iMaxProgress = 1;
    
    // Initialize the operation
    InitializeOperationL();
    if ( aIsRunning ) 
        {
        DLTRACE(("Setting state as EStateRunning"));
        SetState( EStateRunning ); 
        iIsPaused = ETrue;    
        }

    DLTRACEOUT( ( "" ) );    
    }
    

// ---------------------------------------------------------------------------
// Handle progress callback
// ---------------------------------------------------------------------------
//
void CNcdDownloadOperationProxy::ProgressCallback()
    {
    DLTRACEIN( ( "this: %X", this ) );
    TNcdSendableProgress& sendableProgress( SendableProgress() );
    TNcdProgress progress( sendableProgress.iProgress, 
                           sendableProgress.iMaxProgress );

    iCurrentFile = sendableProgress.iState;
    iFileCount = sendableProgress.iOperationId;
    
    DLTRACE(("Progress: %d/%d", sendableProgress.iProgress,
        sendableProgress.iMaxProgress ));

    AddRef();
    // Iterate backwards in case observers want to remove themselves
    // from observers during callback
    for ( TInt i = iObservers.Count() - 1; i >= 0; --i )
        {
        DLTRACE(("Calling observer"));            
        iObservers[i]->DownloadProgress( *this, progress );
        }
    Release();
    DLTRACEOUT( ( "" ) );
    }
    
    
// ---------------------------------------------------------------------------
// Handle query received callback
// ---------------------------------------------------------------------------
//
void CNcdDownloadOperationProxy::QueryReceivedCallback( CNcdQuery* /*aQuery*/ )
    {
    DLTRACEIN( ( "" ) );
    }
    
    
// ---------------------------------------------------------------------------
// Handle operation complete
// ---------------------------------------------------------------------------
//
void CNcdDownloadOperationProxy::CompleteCallback( TInt aError )
    {
    DLTRACEIN( ( "Error: %d, this: %X", aError, this ) );
    AddRef();
    iIsPaused = EFalse;
    
    if ( aError == KErrNone ) 
        {        
        iCurrentFile = iFileCount;
        }

    if ( DownloadDataType() == ENcdContentDownload ) 
        {
        // The download operation for the purchased content has completed.
        // Update the error code and operation time into the purchase history.
        DLINFO(("Update purchase history"));
        TRAP_IGNORE( UpdateOperationInfoToPurchaseHistoryL( aError ) );
        
        // If no errors occurred during operation, then internalize the node.
        if ( aError == KErrNone )
            {
            DLTRACE(("Updating related nodes"));
            TRAP_IGNORE( NodeManager()->InternalizeRelatedNodesL( *NodeProxy() ) );
            DLTRACE(("Node updated"));            
            }
        }
    
    DLTRACE(("Calling observers, count: %d", iObservers.Count() ));
    // Iterate backwards in case observers want to remove themselves
    // from observers during callback
    for ( TInt i = iObservers.Count() - 1; i >= 0; --i )
        {
        iObservers[i]->OperationComplete( *this, aError );
        }
    DLTRACE(("Observers handled, this: %X", this));
    Release();
    DLTRACEOUT( ( "" ) );
    }
    

// ---------------------------------------------------------------------------
// Create initialization data sent to the server-side
// ---------------------------------------------------------------------------
//
HBufC8* CNcdDownloadOperationProxy::CreateInitializationBufferL()
    {
    DLTRACEIN((""));
    return KNullDesC8().AllocL();        
    }


// ---------------------------------------------------------------------------
// Handle initialization callback
// ---------------------------------------------------------------------------
//
void CNcdDownloadOperationProxy::InitializationCallback( 
    RReadStream& aReadStream, TInt /*aDataLength*/ )
    {
    DLTRACEIN( ( "" ) );
    TRAP_IGNORE( 
        {
        iCurrentFile = 1;
        // Read file count from the stream
        iFileCount = aReadStream.ReadInt32L();        
        iIsPaused = aReadStream.ReadInt32L();
        });
    DLTRACEOUT(( "%d files, paused: %d", iFileCount, iIsPaused ));
    }


void CNcdDownloadOperationProxy::DoCancel()
    {
    DLTRACEIN( ( "" ) );
    // Let's internalize the node in cases of content download.
    // This is needed for example in the following case:
    // Cancel occurs when download notification is being sent to
    // server. The content info has already been saved to purchase
    // history. To enable retry of get after this cancellation
    // the node information has to be transported from the
    // server side to proxy side. This is done with internalize.
    // Redownload from the actual server is not possible as it
    // may have already received the installation confirmation.
    if ( DownloadDataType() == ENcdContentDownload )
        {
        DLINFO(("Internalizing node in content download cancellation."));
        // NOTICE: It is chosen that if the internalization fails then
        // no error message is shown to user. This is because the
        // cancellation can still be successfull and then it would be a
        // bit misleading to show that the cancellation has failed. One
        // possibility could be that in case of error the cancellation
        // could be aborted, but it is probably more important to
        // try the cancel than abort and show correct message.
        // If the error message should still be shown, then this
        // internalization should probably be done elsewhere.
        // If this fails, then the outcome can be that the user
        // cannot use option Get before the node is reinternalized.
        // (This seems to happen with current implementation for example
        // by pressing Back and re-entering the same folder)

        // The download operation for the purchased content has completed.
        // Update the error code and operation time into the purchase history.
        DLINFO(("Update purchase history with cancel"));
        TRAP_IGNORE( UpdateOperationInfoToPurchaseHistoryL( KErrCancel ) );
        
        DLINFO(("Updating related nodes"));
        TRAP_IGNORE( NodeManager()->InternalizeRelatedNodesL( *NodeProxy() ) );

        DLINFO(("Node updated"));            
        }
        
    CNcdBaseOperationProxy::DoCancel();
    }
