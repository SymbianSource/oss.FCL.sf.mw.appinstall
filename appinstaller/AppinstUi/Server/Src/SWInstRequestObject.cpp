/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CSWInstRequestObject 
*                class member functions.
*
*/


// INCLUDE FILES

#include <StringLoader.h>
#include <SWInstDefs.h>
#include <SWInstSvrUI.rsg>
#include <SWInstTaskManager.h>

#include "SWInstRequestObject.h"
#include "SWInstUIPluginAPI.h"
#include "CUIDialogs.h"
#include "SWInstServerPanic.h"

using namespace SwiUI;

_LIT( KSWInstSingleInstanceMutex, "SWINST_INSTANCE" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSWInstRequestObject::CSWInstRequestObject
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSWInstRequestObject::CSWInstRequestObject( const RMessage2& aMessage )
    : CActive( CActive::EPriorityStandard ),
      iMessage( aMessage ),
      iMessageSet( ETrue ),
      iObjectId( aMessage.Function() )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSWInstRequestObject::CSWInstRequestObject
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSWInstRequestObject::CSWInstRequestObject( TInt aObjectId )
    : CActive( CActive::EPriorityStandard ),
      iObjectId( aObjectId )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSWInstRequestObject::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSWInstRequestObject::BaseConstructL()
    {
    iTaskManager = CTaskManager::NewL();
    }
    
// Destructor
CSWInstRequestObject::~CSWInstRequestObject()
    {    
    delete iUIPlugin;
    CActive::Cancel();
    delete iTaskManager;
    iSingleInstanceMutex.Close();
    }


// -----------------------------------------------------------------------------
// CSWInstRequestObject::Message
// Returns the message object having this request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RMessage2* CSWInstRequestObject::Message()
    {
    return &iMessage;    
    }

// -----------------------------------------------------------------------------
// CSWInstRequestObject::ObjectHandle
// Returns the request id of this object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSWInstRequestObject::ObjectId() const
    {
    return iObjectId;
    }

// -----------------------------------------------------------------------------
// CSWInstRequestObject::Complete
// Completes the request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstRequestObject::Complete( TInt aResult )
    {
    iSingleInstanceMutex.Close();

    if ( iMessageSet )
        {
        iMessage.Complete( aResult );    
        }

    if ( iCancelMessageSet )
        {
        iCancelMessage.Complete( KErrNone );        
        }    
    
    if ( iCallback )
        {
        iCallback->RequestCompleted( aResult );
        } 
    }

// -----------------------------------------------------------------------------
// CSWInstRequestObject::SetCallback
// Sets the request callback handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstRequestObject::SetCallback( MRequestCallback* aCallback )
    {
    iCallback = aCallback;    
    }

// -----------------------------------------------------------------------------
// CSWInstRequestObject::Cancel
// Cancel current operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstRequestObject::Cancel()
    {
    if ( IsActive() )
        {    
        iCancelling = ETrue;    
        CancelRequest();    
        }    
    }

// -----------------------------------------------------------------------------
// CSWInstRequestObject::Cancel
// Cancel current operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstRequestObject::Cancel( const RMessage2& aMessage )
    {
    if ( IsActive() )
        {        
        iCancelMessage = aMessage;
        iCancelMessageSet = ETrue;    
        iCancelling = ETrue;    
        CancelRequest();    
        }        
    }

// -----------------------------------------------------------------------------
// CSWInstRequestObject::CreatePluginL
// Creates a UI plugin based on the given mime time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstRequestObject::CreatePluginL( const TDesC8& aMime )
    {
    iUIPlugin = CSWInstUIPluginAPI::NewL( aMime );
    }

// -----------------------------------------------------------------------------
// CSWInstRequestObject::LeaveIfInUseL
// Checks if there is another request object in use globally. Leaves with
// KSWInstErrBusy if there is.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstRequestObject::LeaveIfInUseL()
    {
    TInt err = iSingleInstanceMutex.CreateGlobal( KSWInstSingleInstanceMutex() );
    if ( err == KErrAlreadyExists )
        {
        if ( !iIsSilent )
            {            
            HBufC* noteText = StringLoader::LoadLC( R_SWINSTSVRUI_BUSY_INSTALL );
            CommonUI::CCUIDialogs* commonDlg = CommonUI::CCUIDialogs::NewL();
            CleanupStack::PushL( commonDlg );
            commonDlg->ShowErrorNoteL( *noteText );
            CleanupStack::PopAndDestroy( 2, noteText );        
            }        
        User::Leave( KSWInstErrBusy );
        }    
    }
  
// -----------------------------------------------------------------------------
// CSWInstRequestObject::CompleteSelf
// Completes this active object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstRequestObject::CompleteSelf( TInt aResult )
    {
    __ASSERT_ALWAYS( !IsActive(), PanicServer( ESWInstPanicBadHandle ) );
    
    TRequestStatus* stat = &iStatus;    
    User::RequestComplete( stat, aResult );
    SetActive();  
    }

// -----------------------------------------------------------------------------
// CSWInstRequestObject::RunL
// Called by framework when request is finished.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CSWInstRequestObject::RunL()
    {
    RequestCompleteL( iStatus.Int() );
    }

// -----------------------------------------------------------------------------
// CSWInstRequestObject::DoCancelL
// Called by framework when request is cancelled.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSWInstRequestObject::DoCancel()
    {
    // Empty 
    }

// -----------------------------------------------------------------------------
// CSWInstRequestObject::RunError
// Handles a leave occurring in the request completion event handler RunL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CSWInstRequestObject::RunError( TInt aError )
    {
    // Complete the client ipc message with the error
    Complete( aError );

    return KErrNone;
    }

//  End of File  
