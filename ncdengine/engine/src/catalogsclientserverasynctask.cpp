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


#include "catalogsclientserverasynctask.h"
#include "catalogsclientserverclientsession.h"
#include "catalogsclientserverallocutils.h"
#include "catalogsdebug.h"


// ======== MEMBER FUNCTIONS ========



CCatalogsClientServerAsyncTask::CCatalogsClientServerAsyncTask( 
    RCatalogsClientServerClientSession& aSession ) : 
    CActive( EPriorityStandard ),
    iState( ENoRequestsPending ),
    iSession( aSession ),
    iTempWritableReturnBuf( NULL, 0 )
    {
    }


void CCatalogsClientServerAsyncTask::ConstructL()
    {
    DLTRACEIN(("this: %x", this));
    CActiveScheduler::Add( this ); // Removal of this from active scheduler
                                   // is done when this is destroyed
                                   
    }

CCatalogsClientServerAsyncTask* CCatalogsClientServerAsyncTask::NewL( 
        RCatalogsClientServerClientSession& aSession )
    {
    CCatalogsClientServerAsyncTask* self = 
        CCatalogsClientServerAsyncTask::NewLC( aSession );
    CleanupStack::Pop( self );
    return self;
    }

CCatalogsClientServerAsyncTask* CCatalogsClientServerAsyncTask::NewLC(
    RCatalogsClientServerClientSession& aSession )
    {
    CCatalogsClientServerAsyncTask* self = 
        new( ELeave ) CCatalogsClientServerAsyncTask( aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CCatalogsClientServerAsyncTask::~CCatalogsClientServerAsyncTask()
    {
    DLTRACEIN(("this: %x", this));
    // If object of this class is destroyed for some odd reason
    // when things are not complete, we destroy own allocated variables
    delete iTempReturnBuf;
    iTempReturnBuf = NULL;
    
    // There must not be outstanding requests when ~CActive()
    // is called. This is ensured by calling Cancel() here.
    Cancel();
    }
    
// ---------------------------------------------------------------------------
// Because the handle has to be returned, a task (this) is
// used here which reads the handle from the package after
// server-side completes.
// ---------------------------------------------------------------------------
//      
void CCatalogsClientServerAsyncTask::CreateProvider( TInt aUid,
                                                     TRequestStatus& aStatus,
                                                     TInt& aHandle,
                                                     TUint32 aOptions )
    {
    DLTRACEIN(("Sending request %08x, this: %x", &aStatus, this));
    // Object cannot be active when setActive() is used.
    // With Cancel() we ensure that it is not.
    // Notice that this should actually never be needed as
    // a new task should be given to asynctask only when it is idle
    Cancel();
              
    iState = ECreateProviderRequest;
    
    iClientStatus = &aStatus;
    iReturnHandle = &aHandle;

    iArgs = TIpcArgs( aUid, &iIntegerHandleBuf, aOptions );
    iSession.SendAsync( ECatalogsCreateProvider, iArgs, iStatus );
    SetActive();

    DLTRACEOUT((""));
    return;
    }

// ---------------------------------------------------------------------------
// As the TIpcArgs is passed as reference, also SendAsync
// has to be done using a task (this) which holds the parameter
// in a member variable during the communication.
// ---------------------------------------------------------------------------
//   
void CCatalogsClientServerAsyncTask::SendAsync( TInt aFunction,
                                                const TDesC8& aInput,
                                                TDes8& aOutput,
                                                TInt aHandle,
                                                TRequestStatus& aStatus )
    {
    DLTRACEIN(("Sending request %08x, this: %x", &aStatus, this));
    // Notice that this should actually never be needed as
    // a new task should be given to asynctask only when it is idle
    Cancel();
    iState = EAsyncMessageSendRequest;
    iClientStatus = &aStatus;
    
    // Packaging could be removed and integer put straight to args
    iIntegerFunctionBuf = aFunction;
    iIntegerHandleBuf = aHandle;

    iReturnPointer = &aOutput;

    // Next we create a temporary descriptor which is used in the message
    // transfer instead of the given aOutput descriptor. This is done
    // to prevent a crash in a situation where the sender has died before
    // the message is returned from the server side.
    // (Don't actually know if a crash would occur in the situation
    //  described here, but this temporary descriptor is used to be
    //  sure that no crash occurs.)

    TRAPD( error, iTempReturnBuf = HBufC8::NewL( aOutput.MaxLength() ) );
    if ( error != KErrNone )
        {
        iState = ENoRequestsPending;
        TRequestStatus* status = &aStatus;
        DLINFO(( "Completing request %08x with %d", status, error ));
        User::RequestComplete( status, error );
        return;         
        }
    iTempWritableReturnBuf.Set( iTempReturnBuf->Des() );

    iArgs = TIpcArgs( &iIntegerFunctionBuf,
                      &iIntegerHandleBuf,
                      &aInput,
                      &iTempWritableReturnBuf );

    iSession.SendAsync( ECatalogsExternalMessage, iArgs, iStatus );
    SetActive();
    DLTRACEOUT((""));
    return;
    }

// ---------------------------------------------------------------------------
// At the moment, this always creates a new descriptor for returning
// purposes.
// ---------------------------------------------------------------------------
// 
void CCatalogsClientServerAsyncTask::SendAsyncAlloc( TInt aFunction,
                                                     const TDesC8& aInput,
                                                     HBufC8*& aOutput,
                                                     TInt aHandle,
                                                     TRequestStatus& aStatus,
                                                     TInt aLength )
    {
    DLTRACEIN(("Sending request %08x, this: %x", &aStatus, this));
    // Notice that this should actually never be needed as
    // a new task should be given to asynctask only when it is idle
    Cancel();
    iState = EAsyncAllocMessageSendRequest;

    TInt returnValue = AllocReturnBuf( aLength, iTempReturnBuf );
    if ( returnValue != KErrNone )
        {
        iState = ENoRequestsPending;
        TRequestStatus* status = &aStatus;
        DLINFO(( "Completing request %08x with %d", status, returnValue ));
        User::RequestComplete( status, returnValue );
        return;         
        }

    iClientStatus = &aStatus;

    // We use pointer here so it can be easily checked whether
    // the member variable is set or null.
    iAllocReturnPointer = &aOutput;
    
    // Packaging could be removed and integer put straight to args
    iIntegerFunctionBuf = aFunction;
    iIntegerHandleBuf = aHandle;

    iTempWritableReturnBuf.Set( iTempReturnBuf->Des() );
    iArgs = TIpcArgs( &iIntegerFunctionBuf,
                      &iIntegerHandleBuf,
                      &aInput,
                      &iTempWritableReturnBuf );


    iSession.SendAsync( ECatalogsExternalAllocMessage, iArgs, iStatus );
    SetActive();
    DLTRACEOUT((""));
    return;
    }

TRequestStatus* CCatalogsClientServerAsyncTask::SendersRequestStatus() const
    {
    return iClientStatus;
    }

void CCatalogsClientServerAsyncTask::SenderDown()
    {
    DLTRACEIN((""));
    iSenderDown = ETrue;
    
    if ( iClientStatus != NULL )
        {
        // Pointer to TRequestStatus is set to NULL in the end of
        // RequestComplete
        TRequestStatus* status = iClientStatus;
        DLINFO(( "Completing, in sender down, request %08x with %d",
                 status,
                 KErrCancel ));
        User::RequestComplete( status, KErrCancel );
        
        // To disable unintended usage later
        iClientStatus = NULL;        
        }
    }

// ---------------------------------------------------------------------------
// From class CActive.
// ---------------------------------------------------------------------------
//
void CCatalogsClientServerAsyncTask::RunL()
    {
    DLTRACEIN(("iState=%d, this: %x", iState, this));
    switch ( iState ) 
        {
        case ECreateProviderRequest:
            {
            // If original sender is already down we should not write
            // anything into it.
            if ( !iSenderDown )
                {            
                // Next line is among the most important functionalities
                // that instance of this class performs because it seems
                // that two directional communication cannot work properly
                // without implicit reading of the input from server side
                // even if using TPckg in the transfer.
                *iReturnHandle = iIntegerHandleBuf();
                }
            iReturnHandle = NULL;
            GeneralCleanAndComplete();
            break;
            }
        case EAsyncMessageSendRequest:
            {
            DLTRACE(("EAsyncMessageSendRequest"));

            // If original sender is already down we should not write
            // anything into memory reserved by it.
            if ( !iSenderDown )
                {
                // Now as we can be sure that receiver exists, let's
                // write message received from the server into the
                // descriptor given by the sender.
                (*iReturnPointer).Copy( *iTempReturnBuf );
                }
            delete iTempReturnBuf;
            iReturnPointer = NULL;
            iTempReturnBuf = NULL;
            iTempWritableReturnBuf.Set( NULL, 0 ,0 );

            GeneralCleanAndComplete();
            break;
            }
        case EAsyncAllocMessageSendRequest:
            {
            DLTRACE(("EAsyncAllocMessageSendRequest"));
            HandleAllocMessageComplete();
            break;
            }
        case EAsyncAllocCompleteMessageRequest:        
            {
            DLTRACE(("EAsyncAllocCompleteMessageRequest"));
            HandleReallocMessageComplete();
            break;
            }

        default:
            {
            break;
            }
        }    
    DLTRACEOUT((""));
    return;        
    }
    
// ---------------------------------------------------------------------------
// From class CActive.
// ---------------------------------------------------------------------------
//
void CCatalogsClientServerAsyncTask::DoCancel()
    {
    DLTRACEIN(("this: %x", this));
    // We should end up here only if session is closing down
    // and this task is still pending.
    // We assume here that everything that is possible
    // is done so that no pending task would be present (cancel
    // message is sent to the server-side and so on).

    // Server-side does not complete messages anymore, but it is possible it has
    // completed some asynchronous message already, but the message is still
    // waiting its turn in active scheduler. That is, its RunL is not yet
    // executed. Such messages must not be completed here anymore, otherwise E32USER-Cbase 46
    // panic will be raised. Complete other messages so that Cancel() won't hang.

    TRequestStatus* status = &iStatus;
    
    if ( iStatus != KRequestPending ) 
        {
        // The message is completed from server side already, do not complete it twice!
        DLINFO(("The message is completed already, do not complete it"));
        }
    else
        {        
        DLINFO(( "Completing request %08x with KErrGeneral", status ));
        User::RequestComplete( status, KErrGeneral );
        iState = ENoRequestsPending;
        }
    DLTRACEOUT((""));
    }
    
void CCatalogsClientServerAsyncTask::GeneralCleanAndComplete()
    {
    DLTRACEIN(("this: %x", this));
    
    // If original sender is already down we should not notify
    // it of completed message
    if ( !iSenderDown )
        {
        // Pointer to TRequestStatus is set to NULL in the end of
        // RequestComplete
        TRequestStatus* status = iClientStatus;
        DLINFO(( "Completing request %08x with %d",
                 status,
                 iStatus.Int() ));
        User::RequestComplete( status, iStatus.Int() );        
        }

    // To inable unintended usage later
    iClientStatus = NULL;
    
    iState = ENoRequestsPending;
 
    // Tells session that this task has been completed.
    // NOTICE: This is the same as delete this. So no member variable
    //         altering after this.
    iSession.TaskCompleted( this );
    
    }

void CCatalogsClientServerAsyncTask::CompleteAllocWithError( 
    TInt aError )
    {
    DLTRACEIN(("this: %x", this));
    delete iTempReturnBuf;
    iTempReturnBuf = NULL;            
    iTempWritableReturnBuf.Set( NULL, 0 ,0 );
            
    SetAllocReturnPointer( NULL );
    iStatus = aError;
    GeneralCleanAndComplete();
    DLTRACEOUT((""));
    }


void CCatalogsClientServerAsyncTask::HandleAllocMessageComplete()
    {
    
    TInt outcome( iStatus.Int() );
    DLTRACEIN(("outcome: %d, this: %x", outcome, this ));
    if ( outcome == KErrNone )
        {
        SetAllocReturnPointer( iTempReturnBuf );
        }
    else if ( outcome == KCatalogsErrorTooSmallDescriptor )
        {
        DLTRACE(("Too small descriptor"));
        iState = EAsyncAllocCompleteMessageRequest;
        TInt incompleteMessageHandle = -1;
        TInt error = RetrieveNewDescLengthAndReAlloc(
                         iTempReturnBuf,
                         incompleteMessageHandle );
        if ( error != KErrNone )
            {
            if ( incompleteMessageHandle != -1 )
                {
                iSession.DeleteIncompleteMessage( incompleteMessageHandle );
                }            
            CompleteAllocWithError( KErrGeneral );
            DLTRACEOUT(("error: %d", error));
            return;
            }
                
        // to be sure
        iTempWritableReturnBuf.Set( iTempReturnBuf->Des() );

        iIntegerHandleBuf = incompleteMessageHandle;
        iArgs = TIpcArgs();
        iArgs.Set( 1, &iIntegerHandleBuf );
        iArgs.Set( 3, &iTempWritableReturnBuf );
        iSession.SendAsync( ECatalogsCompleteMessage,
                            iArgs,
                            iStatus );
        SetActive();
        DLTRACEOUT((""));
        return;                
        }
    else
        {
        DLTRACE(("Some error"));
        // Although this is an error situation we might
        // want to return a message. User has to be careful
        // if using the message.
        SetAllocReturnPointer( iTempReturnBuf );
        }
                
    iTempReturnBuf = NULL;
    iTempWritableReturnBuf.Set( NULL, 0 ,0 );
            
    GeneralCleanAndComplete();    
    DLTRACEOUT((""));
    }
    
void CCatalogsClientServerAsyncTask::HandleReallocMessageComplete()
    {
    DLTRACEIN((""));
    if ( iStatus.Int() == KCatalogsErrorTooSmallDescriptor )
        {
        delete iTempReturnBuf;
        SetAllocReturnPointer( NULL );
        }
    else
        {
        SetAllocReturnPointer( iTempReturnBuf );
        }
     
    iTempReturnBuf = NULL;
    iTempWritableReturnBuf.Set( NULL, 0 ,0 );
            
    GeneralCleanAndComplete();
    DLTRACEOUT((""));
    }

void CCatalogsClientServerAsyncTask::SetAllocReturnPointer(
    HBufC8* aBufferForReturn )
    {
    // If original sender is already down we should not write
    // anything into it.
    if ( !iSenderDown )
        {
        *iAllocReturnPointer = aBufferForReturn;
        }
    else
        {
        delete aBufferForReturn;
        }
    iAllocReturnPointer = NULL;
    }
