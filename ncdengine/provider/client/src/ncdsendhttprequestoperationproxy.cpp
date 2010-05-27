/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements CNcdSendHttpRequestOperationProxy
*
*/


#include "ncdsendhttprequestoperationproxy.h"

#include <s32strm.h>

#include "ncdsendhttprequestoperationobserver.h"
#include "ncdoperationproxyremovehandler.h"
#include "catalogsclientserver.h"
#include "catalogsutils.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdSendHttpRequestOperationProxy* CNcdSendHttpRequestOperationProxy::NewLC( 
    MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler& aRemoveHandler,
    CNcdNodeManagerProxy& aNodeManager,
    MNcdSendHttpRequestOperationObserver& aObserver )
    {
    CNcdSendHttpRequestOperationProxy* self =
        new( ELeave ) CNcdSendHttpRequestOperationProxy( aObserver );
    
    self->AddRef();
    CleanupReleasePushL( *self );    
    self->ConstructL( 
        aSession, 
        aHandle, 
        aRemoveHandler,         
        aNodeManager );
    return self;
    }



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC8* CNcdSendHttpRequestOperationProxy::ResponseL() const
    {
    DLTRACEIN((""));    
    
    if ( OperationStateL() != MNcdOperation::EStateComplete ) 
        {
        DLERROR(("Operation has not completed yet, leaving with KErrNotReady"));
        User::Leave( KErrNotReady );
        }
    
    HBufC8* data = NULL;
    User::LeaveIfError( ClientServerSession().SendSyncAlloc( 
        ENCDOperationFunctionGetData,
        KNullDesC8(),
        data,
        Handle(),
        0 ) );
    
    if ( !data ) 
        {
        DLERROR(("No data, leaving with KErrCorrupt"));
        User::Leave( KErrCorrupt );
        }
    
    return data;
    }


 
// ---------------------------------------------------------------------------
// From MNcdOperation
// Operation type getter
// ---------------------------------------------------------------------------
//
TNcdInterfaceId CNcdSendHttpRequestOperationProxy::OperationType() const
    {
    return static_cast<TNcdInterfaceId>(
        MNcdSendHttpRequestOperation::KInterfaceUid );
    }



// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdSendHttpRequestOperationProxy::CNcdSendHttpRequestOperationProxy(
    MNcdSendHttpRequestOperationObserver& aObserver )    
    : CNcdOperation< MNcdSendHttpRequestOperation >( NULL ), 
      iObserver( aObserver )      
    {
    }



// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdSendHttpRequestOperationProxy::~CNcdSendHttpRequestOperationProxy()
    {
    DLTRACEIN(( "this: %X", this )); 
            
    if ( iRemoveHandler )
        {
        iRemoveHandler->RemoveOperationProxy( *this );
        }
    
    DLTRACEOUT(( "" ));    
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdSendHttpRequestOperationProxy::ConstructL( 
    MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler& aRemoveHandler,
    CNcdNodeManagerProxy& aNodeManager )
    {
    DLTRACEIN( ( "this: %X", this ) );
    CNcdBaseOperationProxy::ConstructL( 
        aSession, 
        aHandle, 
        &aRemoveHandler,
        NULL, 
        &aNodeManager );    
    }
    

    
// ---------------------------------------------------------------------------
// Handle progress callback
// ---------------------------------------------------------------------------
//
void CNcdSendHttpRequestOperationProxy::ProgressCallback()
    {
    DLTRACEIN( ( "this: %X", this ) );
    TNcdSendableProgress& sendableProgress( SendableProgress() );
    TNcdProgress progress( sendableProgress.iProgress, 
                           sendableProgress.iMaxProgress );

    DLTRACE(("Progress: %d/%d", sendableProgress.iProgress,
        sendableProgress.iMaxProgress ));

    AddRef();
    DLTRACE(("Calling observer"));            
    iObserver.Progress( *this, progress );
        
    Release();
    
    }
    
    
// ---------------------------------------------------------------------------
// Handle query received callback
// ---------------------------------------------------------------------------
//
void CNcdSendHttpRequestOperationProxy::QueryReceivedCallback( CNcdQuery* /*aQuery*/ )
    {
    DLTRACEIN( ( "" ) );
    }
    
    
// ---------------------------------------------------------------------------
// Handle operation complete
// ---------------------------------------------------------------------------
//
void CNcdSendHttpRequestOperationProxy::CompleteCallback( TInt aError )
    {
    DLTRACEIN( ( "Error: %d, this: %X", aError, this ) );
    AddRef();
        
    // Iterate backwards in case observers want to remove themselves
    iObserver.OperationComplete( *this, aError );

    DLTRACE(("Observers handled, this: %X", this));
    Release();    
    }
    
