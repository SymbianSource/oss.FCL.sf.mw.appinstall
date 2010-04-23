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


#include "catalogssmsoperationimpl.h"
#include "catalogssmsobserver.h"
#include "catalogssmssessionimpl.h"
#include "catalogssmssender.h"
#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//
CCatalogsSmsOperation* CCatalogsSmsOperation::NewL( 
    CCatalogsSmsSession& aOwner, 
    const TCatalogsTransportOperationId& aId,
	MCatalogsSmsObserver* aObserver )    
    {
    
    CCatalogsSmsOperation* self = new( ELeave ) CCatalogsSmsOperation(
        aOwner, 
        aId, 
        aObserver );
    
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCatalogsSmsOperation::~CCatalogsSmsOperation()
    {   
    DLTRACEIN( ( "" ) );
    // Execute the quit operation
    iObserver = NULL;
    Cancel();
    
    delete iSender;
    // Remove the operation from the iSession
    iOwner.RemoveOperation( this );
    DLTRACEOUT( ( "" ) );
    }


// ---------------------------------------------------------------------------
// Add a reference
// ---------------------------------------------------------------------------
//
TInt CCatalogsSmsOperation::AddRef()
    {
    iRefCount++;
    return iRefCount;
    }


// ---------------------------------------------------------------------------
// Release a reference
// ---------------------------------------------------------------------------
//
TInt CCatalogsSmsOperation::Release()
    {
    iRefCount--;
    if ( !iRefCount ) 
        {
        delete this;
        return 0;
        }
    return iRefCount;
    }
        

// ---------------------------------------------------------------------------
// Reference count
// ---------------------------------------------------------------------------
//        
TInt CCatalogsSmsOperation::RefCount() const
    {
    return iRefCount;
    }


// ---------------------------------------------------------------------------
// Cancel operation
// ---------------------------------------------------------------------------
//
TInt CCatalogsSmsOperation::Cancel()
    {
    DLTRACEIN( ( "" ) );
    // Cancellation is done synchronously 
    if( iSender ) 
        {
        iSender->CancelOperation();
        }
        
    // Notify observer that the operation has been cancelled    
    if( iObserver ) 
        {        
        return iObserver->HandleSmsEvent( *this, ECatalogsSmsCancelled );        
        }
    DLTRACEOUT( ( "" ) );
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// Progress information getter
// ---------------------------------------------------------------------------
//
TCatalogsTransportProgress CCatalogsSmsOperation::Progress() const
    {
    return TCatalogsTransportProgress( iState, -1, -1 );
    }


// ---------------------------------------------------------------------------
// Operation ID getter
// ---------------------------------------------------------------------------
//
const TCatalogsTransportOperationId& CCatalogsSmsOperation::OperationId() const
    {
    return iId;
    }


// ---------------------------------------------------------------------------
// Starts sending the message
// ---------------------------------------------------------------------------
//        
void CCatalogsSmsOperation::StartL( const TDesC& aRecipient, 
    const TDesC& aBody )
    {
    DLTRACEIN( ( "" ) );
    
    if ( iState != ECatalogsSmsIdle &&
         iState != ECatalogsSmsSendingFailed )
        {
        User::Leave( KErrInUse );
        }
    
    delete iSender;
    iSender = NULL;
   
    iSender = CCatalogsSmsSender::NewL( *this );
    iState = ECatalogsSmsSending;
    iSender->StartL( aRecipient, aBody );    
    
    }
    

// ---------------------------------------------------------------------------
// SMS sender event handler
// ---------------------------------------------------------------------------
//	
void CCatalogsSmsOperation::HandleSmsSenderEvent( TInt aError )
    {
    DLTRACEIN( ( "ID: %i, aError: %d", iId.Id(), aError ) );
    
    // Operation has been cancelled
    if ( aError == KErrCancel ) 
        {
        iState = ECatalogsSmsCancelled;
        // Return to Cancel()
        return;
        }
    
    if ( aError == KErrNone ) 
        {
        iState = ECatalogsSmsSent;
        }
    else if ( aError == ECatalogsSmsSending ) 
        {
        iState = ECatalogsSmsSending;
        }
    else
        {
        iState = ECatalogsSmsSendingFailed;
        }
    
    // Notify the observer about the success of the send operation
    if ( iObserver ) 
        {
        iObserver->HandleSmsEvent( *this, iState );
        }
    
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//	
CCatalogsSmsOperation::CCatalogsSmsOperation( CCatalogsSmsSession& aOwner, 
    const TCatalogsTransportOperationId& aId,
    MCatalogsSmsObserver* aObserver ) : 
    iOwner( aOwner ), iId( aId ), iObserver( aObserver ), 
    iRefCount( 1 ),
    iState( ECatalogsSmsIdle )
    {
    
    }
