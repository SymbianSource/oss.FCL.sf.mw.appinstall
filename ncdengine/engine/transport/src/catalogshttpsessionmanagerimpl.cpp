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


#include "catalogshttpsessionmanagerimpl.h"

#include <es_sock.h>

#include "catalogshttpoperation.h"
#include "catalogshttpconfig.h"
#include "catalogstransportoperationid.h"
#include "catalogshttpconnectioncreator.h"
#include "catalogsnetworkmanager.h"
#include "catalogsutils.h"
#include "catalogshttputils.h"

#include "catalogsdebug.h"

// 20 minutes
const TInt KDisconnectionDelayInMicroSeconds = 1000000*60*20;

// 20 seconds after we switch to another APN even if the old one is still
// up
const TInt KApSwitchDelay = 1000000*20;

// ======== MEMBER FUNCTIONS ========
 
CCatalogsNetworkManager* CCatalogsHttpSessionManager::iNetworkManager = NULL;
 
// ---------------------------------------------------------------------------
// ResumeOperationCallback, called by CAsyncCallBack
// ---------------------------------------------------------------------------
//	    
static TInt ResumeOperationCallback( TAny* aData )
    {
    DLTRACEIN((""));
    static_cast<CCatalogsHttpSessionManager*>( aData )->ResumeOperationAction();
    return KErrNone;
    }
    
 
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//	
CCatalogsHttpSessionManager* CCatalogsHttpSessionManager::NewL()
    {
    CCatalogsHttpSessionManager* self = new( ELeave ) 
        CCatalogsHttpSessionManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//	
CCatalogsHttpSessionManager::~CCatalogsHttpSessionManager()
    {
    DLTRACEIN((""));
    Cancel();
    iConnectionTimer.Close();
    DeletePtr( iMonitor );
    iTransactionQueue.Close();
    iDownloadQueue.Close();
    iGeneralQueue.Close();
    iRunningQueue.Close();
    ReleasePtr( iConnection );
    
    delete iConnectionCreator;
    delete iCallback;    
    
    // set as NULL after delete because iNetworkManager is static    
    DeletePtr( iNetworkManager );
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Add a reference
// ---------------------------------------------------------------------------
//

void CCatalogsHttpSessionManager::AddRef()
    {
    iRefCount++;    
    }


// ---------------------------------------------------------------------------
// Release a reference
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpSessionManager::Release()
    {
    DLTRACEIN( ( "" ) );
    iRefCount--;
    if ( !iRefCount ) 
        {
        delete this;
        return 0;
        }
    return iRefCount;
    }

// ---------------------------------------------------------------------------
// StartOperation
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpSessionManager::StartOperation( 
    MCatalogsHttpOperation* aOperation, TBool aConnect )
    {
    DLTRACEIN((""));
    DLINFO( ("Operation ID: %i, Type: %i", 
        aOperation->OperationId().Id(), aOperation->OperationType() ) );
    DASSERT( aOperation );
    
    UpdateRunningOperations();
    
    DLINFO(("Running operations: %d, transactions: %d, general: %d",
        iRunningOperations, iRunningTransactions, iRunningFromGeneral ));
        
    // Make sure the download is removed from queue
    RemoveFromQueue( aOperation );
       

    // Check if the operation can connect either by using the current AP
    // or by opening a new one
    if ( aConnect )
        {
        TBool isRunning = EFalse;
        TRAPD( err, isRunning = ConnectL( *aOperation ) );
        if ( err != KErrNone )  
            {
            DLTRACEOUT(("Error: %d, err"));
            return err;
            }
        if ( !isRunning ) 
            {
            DLTRACEOUT(("Not running, adding to queue"));
            return AddToQueue( aOperation );
            }
        }
     // Check whether operation belongs to general queue or not
    if ( aOperation->Config().Priority() > ECatalogsPriorityQueued )
        {        
        // non-general queues
        if ( iRunningOperations < KMaxConcurrentOperations ) 
            {
            DLTRACE(("Not general queue"));
            // Check that transactions leave room for downloads and vice versa
            if ( ( ( aOperation->OperationType() == ECatalogsHttpTransaction ) && 
                 ( iRunningTransactions < KMaxConcurrentOperations - KMinDownloads ) ) ||
                 ( ( aOperation->OperationType() == ECatalogsHttpDownload ) &&
                 ( iRunningOperations - iRunningTransactions < 
                   KMaxConcurrentOperations - KMinTransactions ) ) )
                {
                
                iRunningOperations++;
                if ( aOperation->OperationType() == ECatalogsHttpTransaction ) 
                    {
                    iRunningTransactions++;
                    }               
                     
                DLINFO( ( "Running: %i, TRs: %i", iRunningOperations,
                    iRunningTransactions ) );
                return iRunningQueue.InsertInAddressOrder( aOperation );
                }            
            else 
                {
                DLINFO( ( "Running %i, TRs: %i, DLs: %i", iRunningOperations,
                    iRunningTransactions, 
                    iRunningOperations - iRunningTransactions ) );
                }
            }
        else 
            {
            DLINFO( ( "Running: %i >= Max: %i", iRunningOperations, 
                KMaxConcurrentOperations ) );
            }   
        }
    else if ( !iRunningFromGeneral )    
        {
        // General queue
        DLTRACE(("Operation from general"));        
        return iRunningQueue.InsertInAddressOrder( aOperation );                
        }
        
    return AddToQueue( aOperation );
    }
    
    
// ---------------------------------------------------------------------------
// PauseOperation
// ---------------------------------------------------------------------------
//	    
TInt CCatalogsHttpSessionManager::PauseOperation(   
    MCatalogsHttpOperation* aOperation )
    {
    DLTRACEIN( ("") );
    DASSERT( aOperation );
    
    TInt err = RemoveFromQueue( aOperation );
    if ( err == KErrNone )
        {
        err = iRunningQueue.InsertInAddressOrder( aOperation );
        ResumeOperation();
        }
    else
        {
        DLERROR(( "Error %d", err ));
        }
        
    return err;
    }

    
// ---------------------------------------------------------------------------
// CompleteOperation
// ---------------------------------------------------------------------------
//	    
TInt CCatalogsHttpSessionManager::CompleteOperation( 
    MCatalogsHttpOperation* aOperation )
    {
    DLTRACEIN( ("Running ops: %d, queues: Run: %d, DL: %d, TR: %d, Gen: %d",
        iRunningOperations, iRunningQueue.Count(),
        iDownloadQueue.Count(), iTransactionQueue.Count(),
        iGeneralQueue.Count() ) );
    
    // This shouldn't happen unless the operation is canceled before the
    // connection has been created
    if ( iConnectedOperation == aOperation ) 
        {
        iConnectedOperation = NULL;
        if ( iConnection ) 
            {
            // This prevents AP selection queries after the operation
            // has been canceled. 
            DLTRACE(("Cancel connection creation"));            
            iConnection->Cancel();    
            
            // DLMAIN-545, iConnectionState was left as Disconnecting
            // which prevented the creation of new connections
            iConnectionState = ECatalogsConnectionDisconnected;  
            
            // iConnection will be released in ConnectL/destructor
            }        
        }
    
    TInt index = iRunningQueue.FindInAddressOrder( aOperation );
    if ( index != KErrNotFound ) 
        {
        DLTRACE(("Removing from running ops, count after: %d", 
            iRunningQueue.Count() - 1 ));
            
        iRunningQueue.Remove( index );
        ResumeOperation();        
        }
    else 
        {
        RemoveFromQueue( aOperation );
        // This ensures that connection timer is started if necessary
        UpdateRunningOperations();
        }
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// Priority change handler
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpSessionManager::OperationPriorityChanged(
    MCatalogsHttpOperation* aOperation )
    {
    if ( RemoveFromQueue( aOperation ) == KErrNone ) 
        {
        return AddToQueue( aOperation );
        }
    return KErrNotFound;
    }



// ---------------------------------------------------------------------------
// Connection creator
// ---------------------------------------------------------------------------
//	
CCatalogsHttpConnectionCreator& CCatalogsHttpSessionManager::ConnectionCreatorL()
    {
    DLTRACEIN((""));
    if( !iConnectionCreator ) 
        {
        iConnectionCreator = CCatalogsHttpConnectionCreator::NewL();
        }
    return *iConnectionCreator;        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
CCatalogsNetworkManager& CCatalogsHttpSessionManager::NetworkManagerL()
    {
    DLTRACEIN((""));
    if ( !iNetworkManager )
        {
        iNetworkManager = CCatalogsNetworkManager::NewL();         
        }
    return *iNetworkManager;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpSessionManager::SetResumeMode( TBool aResumeQueued )
    {
    iResumeQueued = aResumeQueued;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpSessionManager::RunL()
    {
    DLTRACEIN(("iStatus: %d", iStatus.Int() ));
    if ( iStatus.Int() == KErrNone )
        {
        if ( iConnection )
            {
            DLTRACE(("Idle timeout, releasing connection"));
            DASSERT( iConnection && iConnection->RefCount() == 1 );
            // Monitor is deleted elsewhere        
            if ( iMonitor ) 
                {
                iMonitor->Cancel();
                }
    
            NetworkManagerL().CloseAccessPointL( iConnection->ConnectionMethod() );
            // We don't need to monitor the shutdown because we're not 
            // switching to an another APN
            ReleasePtr( iConnection );
            iConnectionState = ECatalogsConnectionDisconnected;
            }
        else if ( iSwitchApTimer )
            {
            iSwitchApTimer = EFalse;
            // emulate real connection shutdown
            ConnectionStateChangedL( KConnectionUninitialised );
            }        
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CCatalogsHttpSessionManager::DoCancel()
    {
    DLTRACEIN((""));
    iConnectionTimer.Cancel();
    iSwitchApTimer = EFalse;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TInt CCatalogsHttpSessionManager::RunError( TInt aError )
    {
    DLTRACEIN(("aError: %d", aError ));
    (void) aError;
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CCatalogsHttpSessionManager
// ---------------------------------------------------------------------------
//	
CCatalogsHttpSessionManager::CCatalogsHttpSessionManager() : 
    CActive( CActive::EPriorityStandard ),
    iRefCount( 1 ),
    iResumeQueued( ETrue )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// ResumeOperation
// ---------------------------------------------------------------------------
//	    
void CCatalogsHttpSessionManager::ResumeOperation()
    {
    DLTRACEIN((""));
    DLINFO( ( "Tr queue: %i, Dl queue: %i, general: %i", 
        iTransactionQueue.Count(),
        iDownloadQueue.Count(), 
        iGeneralQueue.Count() ) );
    
    if ( !iResumeQueued ) 
        {
        DLTRACEOUT(("Operation resuming disabled"));
        return;
        }    
    
    DLTRACE(("Callback"))
    if ( !iCallback->IsActive() ) 
        {        
        iResumeCount++;
        iCallback->CallBack();
        }
    else
        {
        iResumeCount++;
        }
    }
    

// ---------------------------------------------------------------------------
// ResumeOperationAction
// ---------------------------------------------------------------------------
//	        
TInt CCatalogsHttpSessionManager::ResumeOperationAction()
    {
    DLTRACEIN((""));
    
    RCatalogsHttpOperationArray* array = NULL;    
    UpdateRunningOperations();
    
    if ( !iRunningFromGeneral && iGeneralQueue.Count() )    
        {
        array = &iGeneralQueue;
        }
    else if ( iTransactionQueue.Count() && ( iDownloadQueue.Count() == 0 ||
         iTransactionQueue[0]->Config().Priority() >= 
         iDownloadQueue[0]->Config().Priority() ) )
        {
        DLTRACE( ("Resuming transaction") );
        // Resume transaction
        array = &iTransactionQueue;
        }
    else if ( iDownloadQueue.Count() ) 
        {
        DLTRACE( ("Resuming download") );
        // Resume download
        array = &iDownloadQueue;
        }
    

    if ( array ) 
        {            
        iResumeCount--;
        if ( iResumeCount ) 
            {
            DLTRACE(("Call resume callback again"));
            iCallback->CallBack();
            }
        
        MCatalogsHttpOperation* operation = (*array)[0];
        DLTRACE( ( "Resuming: %x, type: %i, id: %i", 
            operation, operation->OperationType(),
            operation->OperationId().Id() ) );
                
        // Start will remove the operation
        return operation->Start( MCatalogsHttpOperation::EAutomaticResume );            
        }
    
    iResumeCount = 0;
    DLTRACE( ( "Nothing to resume" ) );    
    return KErrNotFound; 
    }


// ---------------------------------------------------------------------------
// ConnectionCreatedL
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpSessionManager::ConnectionCreatedL( 
    const TCatalogsConnectionMethod& aMethod )
    {
    DLTRACEIN(("method: %d, %u, %u",
        aMethod.iType, aMethod.iId, aMethod.iApnId ));
    
    StartMonitoringL( EFalse );
    iLatestConnectionMethod = aMethod;
    iConnectionState = ECatalogsConnectionConnected;
    
    // Update connection to queued operations that require it
    SetConnectionToQueue( 
        aMethod,
        iDownloadQueue );

    SetConnectionToQueue( 
        aMethod,
        iTransactionQueue );

    SetConnectionToQueue( 
        aMethod,
        iGeneralQueue );
    
    DLTRACE(("Starting the operation"));
    
    if ( iConnectedOperation ) 
        {        
        MCatalogsHttpOperation* connect = iConnectedOperation;
        iConnectedOperation = NULL;
        User::LeaveIfError( connect->Start() );    
        }
        
    // Try to fill the running queue by resuming queued operations
    iResumeCount = KMaxConcurrentOperations - 1;
    ResumeOperation();
    
    NetworkManagerL().OpenAccessPointL( aMethod );
    }


// ---------------------------------------------------------------------------
// Sets current connection to those operations that have a matching 
// connection method
// ---------------------------------------------------------------------------
//
void CCatalogsHttpSessionManager::SetConnectionToQueue( 
    const TCatalogsConnectionMethod& aMethod,  
    RCatalogsHttpOperationArray& aArray )
    {
    DASSERT( iConnection );
    TInt count = aArray.Count();
    while ( count-- ) 
        {
        if ( aArray[ count ]->Config().ConnectionMethod().Match( aMethod ) ) 
            {            
            aArray[ count ]->SetConnection( *iConnection );
            }
        }
    }


// ---------------------------------------------------------------------------
// ConnectionError
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpSessionManager::ConnectionError( TInt aError )
    {
    DLTRACEIN(("error: %d", aError ));
    iConnectionState = ECatalogsConnectionDisconnected;
    Cancel();
    // notify operations about failure
        
    ReportConnectionError(
        iConnection->ConnectionMethod(),
        iDownloadQueue,
        aError );

    ReportConnectionError(
        iConnection->ConnectionMethod(),
        iTransactionQueue,
        aError );

    ReportConnectionError(
        iConnection->ConnectionMethod(),
        iGeneralQueue,
        aError );

    iConnectedOperation = NULL;
    
    
    // Try to resume remaining operations in case they get lucky
    ReleasePtr( iConnection );
    iResumeCount = 0;
    ResumeOperation();    
    }
    
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpSessionManager::ConnectionStateChangedL( TInt aStage )
    {
    DLTRACEIN(("aStage: %d", aStage ));
    
    if ( aStage == KConnectionUninitialised ) 
        {
        DLTRACE(("Resuming operations"));
        Cancel();
        
        if ( iMonitor ) 
            {
            // Monitor is deleted elsewhere        
            iMonitor->Cancel();
            }
        
        iConnectionState = ECatalogsConnectionDisconnected;        
        NetworkManagerL().NotifyAccessPointReallyClosedL( 
            iLatestConnectionMethod );
        
        // no point resuming more than one op since it needs to create
        // the connection before other operations can be resumed
        iResumeCount = 0;
        ResumeOperation();            
        }
    else if ( aStage == KErrConnectionTerminated ) 
        {
        DLTRACE(("Connection error: %d", aStage ));

        if ( iMonitor ) 
            {
            // Monitor is deleted elsewhere        
            iMonitor->Cancel();
            }
        
        iConnectionState = ECatalogsConnectionDisconnecting;
        NetworkManagerL().NotifyAccessPointReallyClosedL( 
            iLatestConnectionMethod );

        // notify operations about failure
        SetResumeMode( EFalse );
        
        ReportConnectionError(
            iRunningQueue,
            aStage );
        
        ReportConnectionError(
            iDownloadQueue,
            aStage );

        ReportConnectionError(
            iTransactionQueue,
            aStage );
        
        ReportConnectionError(
            iGeneralQueue,
            aStage );

        TCatalogsConnectionMethod method;
        if ( iConnection ) 
            {
            method = iConnection->ConnectionMethod();
            }
        NetworkManagerL().CloseAccessPointL( method );
        
        SetResumeMode( ETrue );
        iConnectedOperation = NULL;
        
        iConnectionState = ECatalogsConnectionDisconnected;       
        Cancel();
        }
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	    
void CCatalogsHttpSessionManager::ConnectionStateError( TInt  aError )
    {
    DLTRACEIN(("aError: %d", aError ));
    (void) aError;
    iConnectionState = ECatalogsConnectionDisconnected;  
    Cancel();
    // Just try to resume an operation, nothing else we can do
    iResumeCount = 0;
    ResumeOperation();    
        
    }


// ---------------------------------------------------------------------------
// ReportConnectionError
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpSessionManager::ReportConnectionError( 
    const TCatalogsConnectionMethod& aMethod,  
    RCatalogsHttpOperationArray& aArray,
    TInt aError )
    {    
    TInt count = aArray.Count();
    while ( count-- ) 
        {
        // iConnectedOperation-pointer is used to ensure that at least it
        // is notified even if the connection method would have changed
        // for some reason
        if ( aArray[ count ]->Config().ConnectionMethod().Match( aMethod ) ||
             aArray[ count ] == iConnectedOperation ) 
            {            
            aArray[ count ]->ReportConnectionError( aError );
            }
        }
    }


// ---------------------------------------------------------------------------
// ReportConnectionError
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpSessionManager::ReportConnectionError(  
    RCatalogsHttpOperationArray& aArray,
    TInt aError )
    {    
    TInt count = aArray.Count();
    while ( count-- ) 
        {
        aArray[ count ]->ReportConnectionError( aError );
        }
    }


// ---------------------------------------------------------------------------
// AddToQueue
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpSessionManager::AddToQueue( 
    MCatalogsHttpOperation* aOperation )
    {
    DLTRACEIN((""));
    DLINFO( ( "Operation ID: %i, Type: %i", 
        aOperation->OperationId().Id(), aOperation->OperationType() ) );
        
    // Insert the operation in priority order
    TLinearOrder<MCatalogsHttpOperation> order( 
        &CCatalogsHttpSessionManager::PrioritizeOperations );

    TInt err = ChooseArray( *aOperation ).InsertInOrderAllowRepeats( 
        aOperation, order );
    if ( err != KErrNone ) 
        {
        DLTRACE( ( "Failed with err: %i", err ) );
        return err;
        }
        
    DLINFO( ( "After Add, Queued TRs: %i, DLs: %i" , 
        iTransactionQueue.Count(), iDownloadQueue.Count() ) );
        
    return KCatalogsHttpOperationQueued;        
    }


// ---------------------------------------------------------------------------
// RemoveFromQueue
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpSessionManager::RemoveFromQueue( 
    MCatalogsHttpOperation* aOperation )
    {
    DLTRACEIN((""));
    
    if ( aOperation == iConnectedOperation ) 
        {
        iConnectedOperation = NULL;
        }
    
    TInt index = iRunningQueue.Find( aOperation );
    if ( index != KErrNotFound ) 
        {        
        iRunningQueue.Remove( index );
        DLTRACEOUT(("Removed from running"));
        return KErrNone;
        }
        
    RCatalogsHttpOperationArray& array = ChooseArray( *aOperation );
        
    index = array.Find( aOperation );
    if ( index != KErrNotFound ) 
        {
        array.Remove( index );
        DLINFO( ( "After Remove, Queued TRs: %i, DLs: %i" , 
            iTransactionQueue.Count(), iDownloadQueue.Count() ) );

        return KErrNone;
        }
    return KErrNotFound;        
    }
    

// ---------------------------------------------------------------------------
// Compares the priorities of two operations
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpSessionManager::PrioritizeOperations( 
    const MCatalogsHttpOperation& aFirst, 
    const MCatalogsHttpOperation& aSecond )
    {        
    return aFirst.Config().Priority() - aSecond.Config().Priority();
    }


// ---------------------------------------------------------------------------
// Choose array
// ---------------------------------------------------------------------------
//
RCatalogsHttpOperationArray& CCatalogsHttpSessionManager::ChooseArray(
    const MCatalogsHttpOperation& aOperation )
    {
    DLTRACEIN((""));
    if ( aOperation.Config().Priority() <= ECatalogsPriorityQueued )
        {
        DLTRACEOUT(("Queued general"));
        return iGeneralQueue;
        }
        
    if ( aOperation.OperationType() == ECatalogsHttpTransaction ) 
        {
        DLTRACEOUT(("Queued transaction"));
        return iTransactionQueue;
        }
    
    DLTRACEOUT(("Queued download"));
    return iDownloadQueue;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpSessionManager::UpdateRunningOperations()
    {
    DLTRACEIN((""));
    const TTimeIntervalMicroSeconds32 KDisconnectionDelay( 
        KDisconnectionDelayInMicroSeconds );
    
    iRunningDownloads = 0;
    iRunningTransactions = 0;
    iRunningFromGeneral = 0;
    for ( TInt i = 0; i < iRunningQueue.Count(); ++i )
        {
        if ( iRunningQueue[i]->Config().Priority() <= ECatalogsPriorityQueued )
            {
            iRunningFromGeneral++;
            }
        else if ( iRunningQueue[i]->OperationType() == ECatalogsHttpTransaction ) 
            {
            iRunningTransactions++;
            }
        else if ( iRunningQueue[i]->State() != 
            ECatalogsHttpOpPaused )
            {
            iRunningDownloads++;
            }
        }
        
    iRunningOperations = iRunningTransactions + iRunningDownloads;
    
    // Handle connection timer: either cancel it if we have running operations
    // or start it if it's not running already
    if ( iRunningOperations + iRunningFromGeneral ) 
        {
        Cancel();
        }
    else if ( !IsActive() && iConnection )
        {
        DLTRACE(("Starting timer, time: %d", 
            KDisconnectionDelayInMicroSeconds ));
        iSwitchApTimer = EFalse;
        // Using RTimer instead of RConnectionMonitor because we can't
        // observe the RConnection used by Download manager 
        iConnectionTimer.After( iStatus, KDisconnectionDelay );
        SetActive();
        }
    
    DLTRACEOUT(("Running dls: %d, trs: %d, paused dls: %d, general: %d", 
        iRunningDownloads, 
        iRunningTransactions, 
        iRunningQueue.Count() - iRunningDownloads - iRunningTransactions,
        iRunningFromGeneral ));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpSessionManager::ConstructL()
    {
    DLTRACEIN((""));
    iCallback = new (ELeave) CAsyncCallBack( 
        TCallBack( ResumeOperationCallback, this ),
        CActive::EPriorityStandard );   
    User::LeaveIfError( iConnectionTimer.CreateLocal() );
    }    


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCatalogsHttpSessionManager::ConnectL( MCatalogsHttpOperation& aOperation )
    {
    DLTRACEIN((""));
    DLMETHOD( aOperation.Config().ConnectionMethod() );    
        
#ifdef CATALOGS_BUILD_CONFIG_DEBUG    
    if ( iConnection ) 
        {
        DLINFO(("Connection refcount: %d", iConnection->RefCount() ));
        DLMETHOD( iConnection->ConnectionMethod() );    
        }
#endif

    // We must not cancel the timer if we are waiting for the old connection
    // to go down because that same timer is used for switching to new
    // AP after a timeout.
    if ( !iSwitchApTimer ) 
        {
        Cancel();
        }
    
    // Don't even try to connect when connection is going up or down
    if ( iConnectionState == ECatalogsConnectionConnecting ||
         iConnectionState == ECatalogsConnectionDisconnecting ) 
        {
        DLTRACEOUT(("(Dis)connecting..., cannot resume now")); 
        return EFalse;
        }
    
    // Check that the connection is still alive
    TBool connected = EFalse;
    if ( iConnection ) 
        {
        connected = iConnection->IsConnectedL();
        }
        
    // If there's a connection and it matches the wanted connection we can just
    // use that
    if ( connected )
        {
        DLTRACE(( "Connected" ));
        const TCatalogsConnectionMethod& newMethod( 
            aOperation.Config().ConnectionMethod() );
        const TCatalogsConnectionMethod& currentMethod( 
            iConnection->ConnectionMethod() );
        
        // We accept an connection if either type and id match
        // or the connected APN matches with the wanted one
        if ( newMethod.Match( currentMethod ) ||
             ( currentMethod.iApnId != 0 && 
                 ( newMethod.iApnId == currentMethod.iApnId  ||
                     ( newMethod.iType == ECatalogsConnectionMethodTypeAccessPoint &&
                         newMethod.iId == currentMethod.iApnId ) ) ) )
            {
            // Update connected operation in case the old one has been released
            iConnectedOperation = &aOperation;
            DLTRACE(("Not connected, setting connection"));
            aOperation.SetConnection( *iConnection );
    
            return ETrue;
            }
        }
    
    
    if ( DisconnectL() )
        {
        // connection was on so we don't want to recreate a new one until
        // the old one has gone down
        return EFalse;
        }
    

    if ( !iConnection ) 
        {
        DLTRACE(("Creating a new connection"));
        DeletePtr( iMonitor );
        // create connection        
        iConnection = CCatalogsConnection::NewL(
            ConnectionCreatorL(),
            aOperation.ConnectionManager(),
            *this );
        
        iConnectedOperation = &aOperation;        
        iConnection->ConnectL( aOperation.Config().ConnectionMethod() );
        iConnectionState = ECatalogsConnectionConnecting;    
        }
        
    return EFalse;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCatalogsHttpSessionManager::StartMonitoringL( TBool aStartSwitchTimer )
    {
    DLTRACEIN((""));
    DeletePtr( iMonitor );        
    
    TUint count = 0;
    
    // RConnection may have been closed already if an operation was canceled
    // before the connection was created
    if ( !iConnection->Connection().SubSessionHandle() ) 
        {
        DLTRACEOUT(("Connection has been closed already"));
        return EFalse;
        }
    
    User::LeaveIfError( iConnection->Connection().EnumerateConnections( count ) );
    TConnectionInfoBuf connectionBuf;
    TBool open = CatalogsHttpUtils::IsConnectionMethodOpen( 
        iConnection->Connection(),
        count,
        iConnection->ConnectionMethod(),
        connectionBuf );
    
    // We don't start monitoring unless the connection is up because we
    // wouldn't get any progress notifications if it is already down
    if ( open ) 
        {
        iMonitor = CCatalogsConnectionMonitor::NewL( *this );
        
        DLTRACE(("Start monitoring for connection shutdown"));
        iMonitor->StartL( connectionBuf );

        if ( aStartSwitchTimer ) 
            {
            Cancel();
            iSwitchApTimer = ETrue;
            // Initiate delay after which we switch to new AP even if we the old
            // connection hasn't closed yet
            iConnectionTimer.After( iStatus, KApSwitchDelay );
            SetActive();
            }

        // Prevent operation from starting by returning EFalse
        return ETrue;
        }
    return EFalse;
    }


TBool CCatalogsHttpSessionManager::DisconnectL()
    {
    DLTRACEIN((""));
    // If the connection is not used anymore we can release it so that a new
    // one can be created
    if ( iConnection &&  
         iConnection->RefCount() == 1 ) 
        {
        DLTRACE(("Releasing unused connection"));
        NetworkManagerL().CloseAccessPointL( iConnection->ConnectionMethod() );
        
        // Starts monitoring the connection shutdown if the connection is up        
        if ( StartMonitoringL( ETrue ) ) 
            {
            DLTRACE(("Stopping the connection"));
            iConnectionState = ECatalogsConnectionDisconnecting;
            ReleasePtr( iConnection );
            
            return ETrue;
            }
        
        DLTRACE(("Connection was not open"));
        iConnectionState = ECatalogsConnectionDisconnected;    
        // Connection was not open, we can try to connect straight away    
        ReleasePtr( iConnection );        
        }
    return EFalse;
    }
