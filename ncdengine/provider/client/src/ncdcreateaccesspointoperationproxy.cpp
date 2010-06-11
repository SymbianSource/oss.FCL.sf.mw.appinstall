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
* Description:   Implements CNcdCreateAccessPointOperationProxy
*
*/


#include "ncdcreateaccesspointoperationproxy.h"

#include <s32strm.h>

#include "ncdcreateaccesspointoperationobserver.h"
#include "ncdoperationproxyremovehandler.h"
#include "catalogsclientserver.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdCreateAccessPointOperationProxy* CNcdCreateAccessPointOperationProxy::NewL( 
    MCatalogsClientServer& aSession,
    CNcdNodeManagerProxy* aNodeManager,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler& aRemoveHandler,
    MNcdCreateAccessPointOperationObserver& aObserver )
    {
    CNcdCreateAccessPointOperationProxy* self = 
        CNcdCreateAccessPointOperationProxy::NewLC( 
            aSession, aNodeManager, aHandle, aRemoveHandler, aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdCreateAccessPointOperationProxy* CNcdCreateAccessPointOperationProxy::NewLC( 
    MCatalogsClientServer& aSession,
    CNcdNodeManagerProxy* aNodeManager,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler& aRemoveHandler,
    MNcdCreateAccessPointOperationObserver& aObserver )
    {
    CNcdCreateAccessPointOperationProxy* self =
        new (ELeave) CNcdCreateAccessPointOperationProxy( aObserver );
    
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
TNcdInterfaceId CNcdCreateAccessPointOperationProxy::OperationType() const
    {
    return static_cast< TNcdInterfaceId >( 
        MNcdCreateAccessPointOperation::KInterfaceUid );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdCreateAccessPointOperationProxy::CNcdCreateAccessPointOperationProxy( 
    MNcdCreateAccessPointOperationObserver& aObserver )
    : CNcdOperation< MNcdCreateAccessPointOperation >( NULL ), 
      iObserver( aObserver )
    {
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdCreateAccessPointOperationProxy::~CNcdCreateAccessPointOperationProxy()
    {
    DLTRACEIN( ( "" ) );         
    if ( iRemoveHandler )
        {
        iRemoveHandler->RemoveOperationProxy( *this );
        }
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdCreateAccessPointOperationProxy::ConstructL(
    MCatalogsClientServer& aSession,
    CNcdNodeManagerProxy* aNodeManager,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler& aRemoveHandler )
    {
    DLTRACEIN( ( "" ) );
    CNcdBaseOperationProxy::ConstructL( 
        aSession, 
        aHandle, 
        &aRemoveHandler,
        NULL, 
        aNodeManager );
    }


// ---------------------------------------------------------------------------
// Return the ap id
// ---------------------------------------------------------------------------
//
TUint32 CNcdCreateAccessPointOperationProxy::AccessPoint() const
    {
    return iAccessPoint;
    }


// ---------------------------------------------------------------------------
// Handle progress callback
// ---------------------------------------------------------------------------
//
void CNcdCreateAccessPointOperationProxy::ProgressCallback()
    {
    DLTRACEIN( ( "" ) );
    }
    
    
// ---------------------------------------------------------------------------
// Handle query received callback
// ---------------------------------------------------------------------------
//
void CNcdCreateAccessPointOperationProxy::QueryReceivedCallback( 
    CNcdQuery* /*aQuery*/ )
    {
    DLTRACEIN( ( "" ) );
    // Needed because this is abstract in baseoperation
    }
    
// ---------------------------------------------------------------------------
// Handle operation complete
// ---------------------------------------------------------------------------
//
void CNcdCreateAccessPointOperationProxy::CompleteCallback( TInt aError )
    {
    DLTRACEIN( ( "Error: %d", aError ) );
    
    AddRef();   // in case last reference would be released in OperationComplete callback.
    iObserver.OperationComplete( *this, aError );
    Release();
    }

// ---------------------------------------------------------------------------
// HandleCompletedMessage
// ---------------------------------------------------------------------------
//
void CNcdCreateAccessPointOperationProxy::HandleCompletedMessage(
    TNcdOperationMessageCompletionId aCompletionId,
    RReadStream& aReadStream,
    TInt aDataLength )
    {
    DLTRACEIN(( "aCompletionId =%d, aDataLength =%d", aCompletionId,
        aDataLength));
    switch ( aCompletionId )
        {
        // Handle operation completion
        case ENCDOperationMessageCompletionComplete:
            {
            DLTRACE( ( "ENCDOperationMessageCompletionComplete" ) );
                 // Read progress info
            TRAPD( err,
                {                
                TNcdSendableProgress progress; 
                progress.InternalizeL( aReadStream );
                iAccessPoint = progress.iProgress;
                }); //TRAPD
            aReadStream.Close();  
            
            SetState( MNcdOperation::EStateComplete );
            CompleteCallback( err );          
            break;
            }
                                
        default:
            {
            DLTRACE(("Calling base class"));
            // Call base class
            CNcdBaseOperationProxy::HandleCompletedMessage( aCompletionId,
                aReadStream,
                aDataLength );
            
            break;
            }
        }
    DLTRACEOUT((""));
    }
