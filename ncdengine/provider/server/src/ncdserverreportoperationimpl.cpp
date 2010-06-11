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


#include "ncdserverreportoperationimpl.h"
#include "ncdreportmanager.h"
#include "catalogsbasemessage.h"
#include "catalogscontext.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdServerReportOperation* CNcdServerReportOperation::NewL(
    CNcdGeneralManager& aGeneralManager,
    MNcdOperationRemoveHandler& aRemoveHandler, 
    CNcdReportManager& aReportManager,
    MCatalogsSession& aSession )
    {
    CNcdServerReportOperation* self = new( ELeave ) CNcdServerReportOperation( 
        aGeneralManager, aRemoveHandler, aReportManager, aSession );
    CleanupClosePushL( *self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdServerReportOperation::~CNcdServerReportOperation()
    {
    DLTRACEIN( ( "" ) );    
    }


// ---------------------------------------------------------------------------
// Report manager
// ---------------------------------------------------------------------------
//
CNcdReportManager& CNcdServerReportOperation::ReportManager()
    {
    return iReportManager;
    }


// ---------------------------------------------------------------------------
// Cancel
// ---------------------------------------------------------------------------
//
void CNcdServerReportOperation::Cancel() 
    {    
    DLTRACEIN(( "" ));
    ReportManager().CancelReportSending();
    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// RunOperation
// ---------------------------------------------------------------------------
//
TInt CNcdServerReportOperation::RunOperation()
    {
    DLTRACEIN(( "Pending message: %X", iPendingMessage ));
    
    if ( !iPendingMessage ) 
        {      
        DLTRACE(("No pending message"));  
        return KErrNotReady;
        }
    
    // Start download.
    DLINFO(("Starting reporting"));
    TRAPD( trapError, ReportManager().StartSendReportsL( *this ) );

    DLTRACEOUT(("trapError: %d", trapError));
    return trapError;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdServerReportOperation::CNcdServerReportOperation(
    CNcdGeneralManager& aGeneralManager,
    MNcdOperationRemoveHandler& aRemoveHandler,
    CNcdReportManager& aReportManager,
    MCatalogsSession& aSession )
    :
    CNcdBaseOperation( aGeneralManager, &aRemoveHandler, EServerReportOperation, aSession ), 
    iReportManager( aReportManager )        
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdServerReportOperation::ConstructL()
    {
    DLTRACEIN( ( "" ) );

    // Call ConstructL for the base class
    CNcdBaseOperation::ConstructL();

    DLTRACEOUT(( "" ));
    }
    
void CNcdServerReportOperation::Progress( CNcdBaseOperation& /*aOperation*/ )
    {
    DLTRACEIN((""));
    }

void CNcdServerReportOperation::QueryReceived(
    CNcdBaseOperation& /*aOperation*/, CNcdQuery* /*aQuery*/ )
    {
    DLTRACEIN((""));
    }

void CNcdServerReportOperation::OperationComplete( 
    CNcdBaseOperation* aOperation, TInt aError )
    {
    DLTRACEIN(("aOperation=%08x, aError=%d", aOperation, aError));
    (void) aOperation; // suppresses compiler warning

    // Check if pending message still exists.
    // In case of the operation cancel, the pending message may have
    // already been completed and set to NULL. Then, do not try to
    // complete the same message again.
    if ( iPendingMessage )
        {
        DLINFO(("Completing server report operation with %d", aError));
        CompleteMessage( iPendingMessage, 
            aError != KErrNone ? 
                ENCDOperationMessageCompletionError : 
                ENCDOperationMessageCompletionComplete, 
            aError );        
        }
    }

