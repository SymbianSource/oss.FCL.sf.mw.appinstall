/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdateManager
*                class member functions.
*
*/



// INCLUDES
#include <eikenv.h>
#include <iaupdateobserver.h>
#include <iaupdateresult.h>
#include <iaupdateparameters.h>

#include "iaupdatemanager.h"
#include "iaupdateclient.h"
#include "iaupdatedebug.h"


// -----------------------------------------------------------------------------
// CIAUpdateManager::NewL
// 
// -----------------------------------------------------------------------------
// 
CIAUpdateManager* CIAUpdateManager::NewL( MIAUpdateObserver& aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::NewL() begin");
    CIAUpdateManager* self = CIAUpdateManager::NewLC( aObserver );
    CleanupStack::Pop( self );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::NewL() end");
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::NewLC
// 
// -----------------------------------------------------------------------------
//    
CIAUpdateManager* CIAUpdateManager::NewLC( MIAUpdateObserver& aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::NewLC() begin");
    CIAUpdateManager* self = new( ELeave ) CIAUpdateManager( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::NewLC() end");
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::CIAUpdateManager
// 
// -----------------------------------------------------------------------------
//    
CIAUpdateManager::CIAUpdateManager( MIAUpdateObserver& aObserver )
: CActive( CActive::EPriorityStandard ),
  iObserver( aObserver ),
  iUpdateType( EIAUpdateIdle )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::CIAUpdateManager()");
    iEikEnv = CEikonEnv::Static();
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::ConstructL()
// 
// -----------------------------------------------------------------------------
//    
void CIAUpdateManager::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::ConstructL() begin");
    CActiveScheduler::Add( this );
    if ( iEikEnv )
        {
    	iEikEnv->AddForegroundObserverL(*this);
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::ConstructL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::~CIAUpdateManager
// 
// -----------------------------------------------------------------------------
//
CIAUpdateManager::~CIAUpdateManager()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::~CIAUpdateManager() begin");
    delete iUpdateParameters;
    iUpdateParameters = NULL;
    // If an operation is still active, 
    // then DoCancel will cancel ongoing request
    Cancel();
    UpdateClient().Close();
    if ( iEikEnv )
        {
    	iEikEnv->RemoveForegroundObserver(*this);
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::~CIAUpdateManager() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::CheckUpdates
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateManager::CheckUpdates( const CIAUpdateParameters& aUpdateParameters )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::CheckUpdates() begin");
    if ( BusyCheck() )
        {
        // An operation is already going on.
        // Do not continue with this new operation.
        // Only, inform the observer with the error code.
        // Notice that the callback is called synchronously here 
        // because there is no better way to do it. Asynchronous way 
        // would be really cumbersome because this object is already active. 
        // This error is users fault. Therefore we, could also panic here.
        Observer().CheckUpdatesComplete( KErrServerBusy, 0 );
        return;
        }

    // Reset result values because we are starting a new operation.
    ResetResults();

    //
    delete iUpdateParameters;
    iUpdateParameters = NULL;
    TInt error = KErrNone;
    TRAP(error,CopyUpdateParamsL( aUpdateParameters ) );
    if ( error == KErrNone )
        {
        // Set the update type. 
        // So, we know later in RunL what operation was requested.
        SetUpdateType( EIAUpdateStartServer );   
        error = UpdateClient().OpenToBackroundAsync( iStatus);
        }
 
    if ( error == KErrNone )
        {
        // Set this object active.
        // Because everything went ok, 
        // the operation will be handled asynchronously
        // and the service provider will inform us when the operation
        // is finished.
        SetActive();        
        }
    else
        {
        // Because we are going to activate this active object,
        // set the status pending.
        iStatus = KRequestPending;
        
        // An error occurred above. 
        // Therefore, the operation did not proceed any further.
        // Set this object active for asynchronous error handling.
        SetActive();
                
        // Now, that everything is ready, just inform the active scheduler
        // that operation is finished. Pass the error code for the observer.
        TRequestStatus* status( &iStatus );
        User::RequestComplete( status, error );                
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::CheckUpdates() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::ShowUpdates
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateManager::ShowUpdates( const CIAUpdateParameters& aUpdateParameters )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::ShowUpdates() begin");
    if ( BusyCheck() )
         {
         // An update operation is already going on.
         // Do not continue with this new operation.
         // Only, inform the observer with the error code.
         // Notice that the callback is called synchronously here 
         // because there is no better way to do it. Asynchronous way 
         // would be really cumbersome because this object is already active. 
         // This error is users fault. Therefore we, could also panic here.
         Observer().UpdateComplete( KErrServerBusy, NULL );
         return;
         }

    // Reset result values because we are starting a new operation.
    ResetResults();

    // Set the update type. 
    // So, we know later in RunL what operation was requested.
    SetUpdateType( EIAUpdateUpdate );
    
    TInt error( UpdateClient().Open() );
    if ( error == KErrNone )
        {    
        error =
            UpdateClient().ShowUpdates( aUpdateParameters, 
                                        iSuccessCount,
                                        iFailCount,
                                        iCancelCount,  
                                        iStatus );
        }        

    if ( error == KErrNone )
        {
        // Set this object active.
        // Because everything went ok, 
        // the operation will be handled asynchronously
        // and the service provider will inform us when the operation
        // is finished.
        SetActive();        
        }
    else
        {
        // Because we are going to activate this active object,
        // set the status pending.
        iStatus = KRequestPending;
        
        // An error occurred above. 
        // Therefore, the operation did not proceed any further.
        // Set this object active for asynchronous error handling.
        SetActive();
                
        // Now, that everything is ready, just inform the active scheduler
        // that operation is finished. Pass the error code for the observer.
        TRequestStatus* status( &iStatus );
        User::RequestComplete( status, error );                
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::ShowUpdates() end");
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateManager::Update
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateManager::Update( const CIAUpdateParameters& /*aUpdateParameters*/ )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::Update() begin");
    Observer().UpdateComplete( KErrNotSupported, NULL );
    
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::Update() begin");
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::UpdateQuery
// 
// -----------------------------------------------------------------------------
//   
void CIAUpdateManager::UpdateQuery()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::UpdateQuery() begin");
    if ( BusyCheck() )
        {
        // An update operation is already going on.
        // Do not continue with this new operation.
        // Only, inform the observer with the error code.
        // Notice that the callback is called synchronously here 
        // because there is no better way to do it. Asynchronous way 
        // would be really cumbersome because this object is already active. 
        // This error is users fault. Therefore we, could also panic here.
        Observer().UpdateQueryComplete( KErrServerBusy, EFalse );
        return;
    }

    // Reset result values because we are starting a new operation.
    ResetResults();

    // Set the update type. 
    // So, we know later in RunL what operation was requested.
    SetUpdateType( EIAUpdateQuery );
    
    TInt error( UpdateClient().Open() );
    if ( error == KErrNone )
        {    
        error =
            UpdateClient().UpdateQuery( iUpdateNow, iStatus );
        }        

    if ( error == KErrNone )
        {
        // Set this object active.
        // Because everything went ok, 
        // the operation will be handled asynchronously
        // and the service provider will inform us when the operation
        // is finished.
        SetActive();        
        }
    else
        {
        // Because we are going to activate this active object,
        // set the status pending.
        iStatus = KRequestPending;
        
        // An error occurred above. 
        // Therefore, the operation did not proceed any further.
        // Set this object active for asynchronous error handling.
        SetActive();
                
        // Now, that everything is ready, just inform the active scheduler
        // that operation is finished. Pass the error code for the observer.
        TRequestStatus* status( &iStatus );
        User::RequestComplete( status, error );                
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::UpdateQuery() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateManager::DoCancel()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::DoCancel() begin");
    UpdateClient().CancelAsyncRequest();

    // Reset result values because we are starting a new operation.
    ResetResults();
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::DoCancel() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::RunL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateManager::RunL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::RunL() begin");
    // Update operation error code
    TInt errorCode( iStatus.Int() );
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", errorCode );
    if ( errorCode == KErrServerTerminated )
        {
    	// Handle is released if server is terminated
        UpdateClient().Close();
        }

    // Variables for operation and result information.
    TIAUpdateType updateType( UpdateType() );
    IAUPDATE_TRACE_1("[IAUPDATE] update type: %d", updateType );
    CIAUpdateResult* updateResult( NULL );
        
    if ( updateType == EIAUpdateUpdate )
        {
        // Update result object is required.
        // Notice that the ownership is transferred later.
        // So, this function does not need to delete updateResult object.
        TRAPD( trapError, updateResult = CIAUpdateResult::NewL() );
        if ( updateResult )
            {
            updateResult->SetSuccessCount( iSuccessCount );
            updateResult->SetFailCount( iFailCount );
            updateResult->SetCancelCount( iCancelCount );
            }
        else
            {
            // Something went wrong when creating update result object.
            // Update the error code accordingly.
            errorCode = trapError;
            }
        // Let's assume that connection is not needed anymore
        UpdateClient().Close();
        }

    
    // Inform that no operation is going on anymore.
    // This is required for busy check.
    SetUpdateType( EIAUpdateIdle );    
    
    // Use the update type of the ongoing operation to check what callback
    // function to call.
    switch ( updateType )
        {
        case EIAUpdateStartServer:
            if ( errorCode == KErrNone )
                {
                CheckUpdatesContinue();            
                }
            else
                {
                UpdateClient().Close();
                Observer().CheckUpdatesComplete( errorCode, 0);
                }
            break;
            
        case EIAUpdateCheck:
            if ( iSuccessCount == 0 )
                {
            	// Let's assume that connection is not needed anymore
                UpdateClient().Close();
                }
            IAUPDATE_TRACE_1("[IAUPDATE] success count: %d", iSuccessCount );
            Observer().CheckUpdatesComplete( errorCode, iSuccessCount );
            break;

        case EIAUpdateUpdate:
            // Notice that ownership of result object is transferred here.
            IAUPDATE_TRACE_3("[IAUPDATE] success count: %d failed count: %d cancelled count: %d", iSuccessCount, iFailCount, iCancelCount );
            Observer().UpdateComplete( errorCode, updateResult );
            break;
            
        case EIAUpdateQuery:
            if ( !iUpdateNow )
                {
            	// Let's assume that connection is not needed anymore
                UpdateClient().Close();
                } 
            IAUPDATE_TRACE_1("[IAUPDATE] update now: %d", iUpdateNow );
            Observer().UpdateQueryComplete( errorCode, iUpdateNow );
            break;
         
        default:
            // Should not ever come here.
            break;
        }
        
    // Do not anything else than return after callback function is called because 
    // this instance can be deleted by a client in a callback function
    // 
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::RunL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::ResetResults
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateManager::ResetResults()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::ResetResults() begin");
    // Set the member variables to their default values.
    iSuccessCount = 0;
    iFailCount = 0;
    iCancelCount = 0;
    iUpdateNow = EFalse;
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::ResetResults() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::BusyCheck
// 
// -----------------------------------------------------------------------------
//
TBool CIAUpdateManager::BusyCheck() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::BusyCheck()");
    if ( UpdateType() == EIAUpdateIdle )
        {
        return EFalse;
        }
    else
        {
        return ETrue; 
        }
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateManager::UpdateClient 
// 
// -----------------------------------------------------------------------------
//
RIAUpdateClient& CIAUpdateManager::UpdateClient()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::UpdateClient()");
    return iUpdateClient;
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::Observer
// 
// -----------------------------------------------------------------------------
//
MIAUpdateObserver& CIAUpdateManager::Observer() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::Observer()");
    return iObserver;
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::UpdateType
// 
// -----------------------------------------------------------------------------
//
CIAUpdateManager::TIAUpdateType CIAUpdateManager::UpdateType() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::UpdateType()");
    return iUpdateType;
    }


// -----------------------------------------------------------------------------
// CIAUpdateManager::SetUpdateType
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateManager::SetUpdateType( CIAUpdateManager::TIAUpdateType aUpdateType )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::SetUpdateType() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] update type: %d", aUpdateType );
    iUpdateType = aUpdateType;
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::SetUpdateType() end");
    }
 
// -----------------------------------------------------------------------------
// CIAUpdateManager::HandleGainingForeground
//  iaupdate.exe is brought to foreground if visible request is ongoing
// -----------------------------------------------------------------------------
//     
void CIAUpdateManager::HandleGainingForeground()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::HandleGainingForeground() begin");
	iUpdateClient.BroughtToForeground();   
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::HandleGainingForeground() end");  
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::HandleLosingForeground
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateManager::HandleLosingForeground()
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::CheckUpdatesContinue
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateManager::CheckUpdatesContinue()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::CheckUpdatesContinue() begin");
    TInt error = KErrNone;
    SetUpdateType( EIAUpdateCheck );
    error = UpdateClient().ConnectToApp();
    if ( error == KErrNone )
        {
        error = UpdateClient().CheckUpdates( *iUpdateParameters, 
                                             iSuccessCount, 
                                             iStatus ); 
        }
    
    if ( error == KErrNone )
        {
        // Set this object active.
        // Because everything went ok, 
        // the operation will be handled asynchronously
        // and the service provider will inform us when the operation
        // is finished.
        SetActive();        
        }
    else
        {
        // Because we are going to activate this active object,
        // set the status pending.
        iStatus = KRequestPending;
        
        // An error occurred above. 
        // Therefore, the operation did not proceed any further.
        // Set this object active for asynchronous error handling.
        SetActive();
                
        // Now, that everything is ready, just inform the active scheduler
        // that operation is finished. Pass the error code for the observer.
        TRequestStatus* status( &iStatus );
        User::RequestComplete( status, error );                
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::CheckUpdatesContinue() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateManager::CopyUpdateParamsL
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateManager::CopyUpdateParamsL( const CIAUpdateParameters& aUpdateParameters )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::CopyUpdateParamsL() begin");
    iUpdateParameters = CIAUpdateParameters::NewL();
    iUpdateParameters->SetCommandLineArgumentsL( aUpdateParameters.CommandLineArguments() );
    iUpdateParameters->SetCommandLineExecutableL( aUpdateParameters.CommandLineExecutable() );
    iUpdateParameters->SetImportance( aUpdateParameters.Importance() );
    iUpdateParameters->SetRefresh( aUpdateParameters.Refresh() );
    iUpdateParameters->SetSearchCriteriaL( aUpdateParameters.SearchCriteria() );
    iUpdateParameters->SetShowProgress( aUpdateParameters.ShowProgress() );
    iUpdateParameters->SetType( aUpdateParameters.Type() );
    iUpdateParameters->SetUid( aUpdateParameters.Uid() );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateManager::CopyUpdateParamsL() end");
    }



