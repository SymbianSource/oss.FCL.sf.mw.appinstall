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
#include "ncdrightsobjectoperationproxy.h"
#include "ncdrightsobjectoperationobserver.h"
#include "ncdoperationproxyremovehandler.h"
#include "catalogsclientserver.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdRightsObjectOperationProxy* CNcdRightsObjectOperationProxy::NewL( 
    MCatalogsClientServer& aSession,
    CNcdNodeManagerProxy* aNodeManager,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler& aRemoveHandler,
    MNcdRightsObjectOperationObserver& aObserver )
    {
    CNcdRightsObjectOperationProxy* self = CNcdRightsObjectOperationProxy::NewLC( 
        aSession, aNodeManager, aHandle, aRemoveHandler, aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdRightsObjectOperationProxy* CNcdRightsObjectOperationProxy::NewLC( 
    MCatalogsClientServer& aSession,
    CNcdNodeManagerProxy* aNodeManager,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler& aRemoveHandler,
    MNcdRightsObjectOperationObserver& aObserver )
    {
    CNcdRightsObjectOperationProxy* self =
        new (ELeave) CNcdRightsObjectOperationProxy( aObserver );
    
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
TNcdInterfaceId CNcdRightsObjectOperationProxy::OperationType() const
    {
    return static_cast< TNcdInterfaceId >( MNcdRightsObjectOperation::KInterfaceUid );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdRightsObjectOperationProxy::CNcdRightsObjectOperationProxy( MNcdRightsObjectOperationObserver& aObserver )
    : CNcdOperation< MNcdRightsObjectOperation >( NULL ), iObserver( aObserver )
    {
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdRightsObjectOperationProxy::~CNcdRightsObjectOperationProxy()
    {
    DLTRACEIN( ( "" ) );     
    DLTRACE(("Removing proxy from remove handler"));
    if ( iRemoveHandler )
        iRemoveHandler->RemoveOperationProxy( *this );
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdRightsObjectOperationProxy::ConstructL(
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
void CNcdRightsObjectOperationProxy::ProgressCallback()
    {
    DLTRACEIN( ( "" ) );
    }
    
    
// ---------------------------------------------------------------------------
// Handle query received callback
// ---------------------------------------------------------------------------
//
void CNcdRightsObjectOperationProxy::QueryReceivedCallback( CNcdQuery* /*aQuery*/ )
    {
    DLTRACEIN( ( "" ) );
    }
    
// ---------------------------------------------------------------------------
// Handle operation complete
// ---------------------------------------------------------------------------
//
void CNcdRightsObjectOperationProxy::CompleteCallback( TInt aError )
    {
    DLTRACEIN( ( "Error: %d", aError ) );
    AddRef();   // in case last reference would be released in OperationComplete callback.
    iObserver.OperationComplete( *this, aError );
    Release();
    }
