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
* Description:   Implements CNcdServerReportOperationProxy
*
*/


#include <s32strm.h>
#include "ncdserverreportoperationproxy.h"
#include "ncdserverreportoperationobserver.h"
#include "ncdoperationproxyremovehandler.h"
#include "catalogsclientserver.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdServerReportOperationProxy* CNcdServerReportOperationProxy::NewL( 
    MCatalogsClientServer& aSession,
    CNcdNodeManagerProxy* aNodeManager,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler& aRemoveHandler,
    MNcdServerReportOperationObserver& aObserver )
    {
    CNcdServerReportOperationProxy* self = CNcdServerReportOperationProxy::NewLC( 
        aSession, aNodeManager, aHandle, aRemoveHandler, aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdServerReportOperationProxy* CNcdServerReportOperationProxy::NewLC( 
    MCatalogsClientServer& aSession,
    CNcdNodeManagerProxy* aNodeManager,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler& aRemoveHandler,
    MNcdServerReportOperationObserver& aObserver )
    {
    CNcdServerReportOperationProxy* self =
        new (ELeave) CNcdServerReportOperationProxy( aObserver );
    
    self->AddRef();
    CleanupReleasePushL( *self );    
    self->ConstructL( aSession, aNodeManager, aHandle, aRemoveHandler );
    return self;
    }

// ---------------------------------------------------------------------------
// From MNcdOperation
// Operation type getter
// ---------------------------------------------------------------------------
//
TNcdInterfaceId CNcdServerReportOperationProxy::OperationType() const
    {
    return static_cast< TNcdInterfaceId >( MNcdServerReportOperation::KInterfaceUid );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdServerReportOperationProxy::CNcdServerReportOperationProxy( MNcdServerReportOperationObserver& aObserver )
    : CNcdOperation< MNcdServerReportOperation >( NULL ), iObserver( aObserver )
    {
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdServerReportOperationProxy::~CNcdServerReportOperationProxy()
    {
    DLTRACEIN( ( "" ) );     
    DLTRACE(("Removing proxy from remove handler"));
    if ( iRemoveHandler )
        {
        iRemoveHandler->RemoveOperationProxy( *this );        
        }
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdServerReportOperationProxy::ConstructL(
    MCatalogsClientServer& aSession,
    CNcdNodeManagerProxy* aNodeManager,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler& aRemoveHandler )
    {
    DLTRACEIN( ( "" ) );
    CNcdBaseOperationProxy::ConstructL( aSession, aHandle, &aRemoveHandler,
                                        NULL, aNodeManager );
    }


// ---------------------------------------------------------------------------
// Handle progress callback
// ---------------------------------------------------------------------------
//
void CNcdServerReportOperationProxy::ProgressCallback()
    {
    DLTRACEIN( ( "" ) );
    }
    
    
// ---------------------------------------------------------------------------
// Handle query received callback
// ---------------------------------------------------------------------------
//
void CNcdServerReportOperationProxy::QueryReceivedCallback( CNcdQuery* /*aQuery*/ )
    {
    DLTRACEIN( ( "" ) );
    }
    
// ---------------------------------------------------------------------------
// Handle operation complete
// ---------------------------------------------------------------------------
//
void CNcdServerReportOperationProxy::CompleteCallback( TInt aError )
    {
    DLTRACEIN( ( "Error: %d", aError ) );
    AddRef();   // in case last reference would be released in OperationComplete callback.
    iObserver.OperationComplete( *this, aError );
    Release();
    }
