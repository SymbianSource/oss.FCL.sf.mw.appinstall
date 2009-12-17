/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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




#include <ncdprovider.h>

#include "iaupdatecachecleaner.h"
#include "iaupdatedebug.h"


CIAUpdateCacheCleaner* CIAUpdateCacheCleaner::NewL( MNcdProvider& aProvider )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::NewL begin");
    CIAUpdateCacheCleaner* self(
        CIAUpdateCacheCleaner::NewLC( aProvider ) );
    CleanupStack::Pop( self );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::NewL end");
    return self;
    }
    
    
CIAUpdateCacheCleaner* CIAUpdateCacheCleaner::NewLC( MNcdProvider& aProvider )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::NewLC begin");
    CIAUpdateCacheCleaner* self(
        new( ELeave ) CIAUpdateCacheCleaner( aProvider ) );
    CleanupStack::PushL( self );
    self->ConstructL();
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::NewLC end");
    return self;
    }


CIAUpdateCacheCleaner::CIAUpdateCacheCleaner( MNcdProvider& aProvider )
: CActive( CActive::EPriorityStandard ),
  iProvider( aProvider )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::CIAUpdateCacheCleaner begin");    

    CActiveScheduler::Add( this );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::CIAUpdateCacheCleaner end");
    }


void CIAUpdateCacheCleaner::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::ConstructL");
    }
    
    
CIAUpdateCacheCleaner::~CIAUpdateCacheCleaner()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::~CIAUpdateCacheCleaner begin");    
    
    // Wait until the ongoing operation is finished.
    // Cancel handles this.
    Cancel();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::~CIAUpdateCacheCleaner end");
    }


void CIAUpdateCacheCleaner::ClearL( TRequestStatus& aStatus )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::ClearL begin");    
    
    if ( IsActive()
         || iRequestStatus )
        {
        IAUPDATE_TRACE("[IAUPDATE] ERROR: ClearL already started");
        User::Leave( KErrInUse );
        }

    // Use provider to clear the cache.
    // This will asynchronously finish the operation
    // and update the status when operation finishes.
    // Start to wait the completion in this active object.
    iProvider.ClearCacheL( iStatus );
    SetActive();

    // Set the pending value for the request.
    // Notice, iRequestStatus is just a reference to the
    // actual status object of the user of this object.
    // The user will be informed when the operation finishes
    // by completing the request.
    aStatus = KRequestPending;
    iRequestStatus = &aStatus;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::ClearL begin");    
    }


void CIAUpdateCacheCleaner::DoCancel()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::DoCancel begin");    
    
    // Because we do not have any means to cancel the actual
    // operation. We need to use CActiveSchedulerWait::Start
    // here. This way this function will not proceed directly
    // to the User::WaitForRequest in Cancel after DoCancel. 
    // That would block the thread and the iStatus
    // would not be updated by the cache clear operation. 
    // Then, Cancel would be stuck forever. iWaiter->AsyncStop
    // is called in RunL that is in turn called when the actual 
    // operation finishes. Then, this will continue and the 
    // RunL has set the iStatus and Cancel can continue also.
    TRAP_IGNORE( iWaiter = new( ELeave ) CActiveSchedulerWait() );
	if ( iWaiter  )
	    {
        iWaiter->Start();
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::DoCancel begin");    
    }
    
    
void CIAUpdateCacheCleaner::RunL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::RunL begin");    
    
    TInt errorCode( iStatus.Int() );
    IAUPDATE_TRACE_1("[IAUPDATE] Error code: %d", errorCode);
    
    if ( iWaiter )
        {
        IAUPDATE_TRACE("[IAUPDATE] Waiter exists. Cancel should be going on.");
        
        // Notice, that we come here only when the Cancel is
        // called for this operation and there was an cache
        // clear operation going on.

        // Stop the waiter. 
        // So, DoCancel may continue after this.
        iWaiter->AsyncStop();    
        delete iWaiter;
        iWaiter = NULL;

        // Update the request status.
        // So, Cancel can continue after DoCancel.
        TRequestStatus* status( &iStatus );
        User::RequestComplete( status, errorCode );
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] Cache clear operation done.");
        
        // Complete the original operation because it
        // has completed normally.
        User::RequestComplete( iRequestStatus, errorCode );
        }

    // Because the operation is finished now,
    // set the request status pointer to NULL.
    iRequestStatus = NULL;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateCacheCleaner::RunL end");
    }


TInt CIAUpdateCacheCleaner::RunError( TInt aError )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateCacheCleaner::RunError: %d", aError);
    
    // Because the operation is finished now,
    // set the request status pointer to NULL.
    iRequestStatus = NULL;    
    
    // Notice, that if the return value is not KErrNone,
    // the code will panic.
    return aError;
    }
