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
* Description:   Implementation of RCatalogsClientServerClientSession
*
*/


#include "catalogsclientserverclientsession.h"
#include "catalogsclientserverserver.h"
#include "catalogsserverdefines.h"
#include "catalogsclientserverasynctask.h"
#include "catalogsclientserverallocutils.h"
#include "catalogsdebug.h"

// Amount of attempts to create and connect to server before giving up
const TInt KCatalogsServerStartAttempts( 5 );
// Amount of sleep between retries
const TInt KRetryInterval( 100000 ); // tenth of a second

// Amount of time before we quit on waiting on the semaphore and go on
const TInt KSemaphoreTimeout( 1000000 * 3 ); // 3 seconds

// ======== LOCAL FUNCTIONS ========

static TInt CreateServerProcess()
    {
    DLTRACEIN((""));    
    RProcess server;

    TInt result = server.Create( KCatalogsServerFilename, KNullDesC );
    if ( result != KErrNone )
        {
        DLERROR(( "Failed to create server process: err %d", result ));
        DLTRACEOUT(( "%d", result ));    
        return result;
        }
    
    // Resume server thread and close handle
    server.Resume();
    server.Close();  

    return KErrNone;
    }

static TInt StartServer()
    {
    DLTRACEIN((""));

    // Check if the server is already running
    TFindServer findServer( KCatalogsServerName );
    TFullName name;

    TInt result = findServer.Next( name );
    if ( result == KErrNone )
        {
        // Server is running
        return KErrNone;
        }

    // Create a semaphore so we can wait while the server starts
    RSemaphore semaphore;
    result = semaphore.CreateGlobal( KCatalogsServerSemaphoreName, 0 );
    if ( result != KErrNone )
        {
        DLERROR(( "Failed to create semaphore: err %d", result ));
        DLTRACEOUT(( "%d", result ));    
        return result;
        }

    // Create new Engine service process 
    result = CreateServerProcess();            
    if ( result != KErrNone )
        {
        DLTRACEOUT(( "%d", result ));
        semaphore.Close();    
        return result;
        }

    // Wait while the server starts, if the timeout occurs, we can
    // assume the server didn't start correctly
    TInt err = semaphore.Wait( KSemaphoreTimeout );

    // Semaphore has been signaled, close and return
    semaphore.Close();

    DLTRACEOUT(("error: %d", err ));
    return err;
    }
    

// ======== MEMBER FUNCTIONS ========
    

RCatalogsClientServerClientSession::RCatalogsClientServerClientSession() :
    iSessionStatus( EClosed )
    {
    }

// ---------------------------------------------------------------------------
// This is function to create server process if it does not exist and to
// to connect to the server. Notice that if the current server is going
// down the StartServer() still does not create a new server. If this happens
// then the CreateSession returns -15. If this happens we try the
// server creation for a few times. We also have a sleep between tries
// that halts this thread.
// ---------------------------------------------------------------------------
//
TInt RCatalogsClientServerClientSession::Connect( TUid aClientUid )
    {
    DLTRACEIN((""));
    if ( iSessionStatus == EOpen )
        {
        return KErrNone;
        }
    
    TInt err( KErrNone );
    TInt startTryCounter( 0 );
    while( startTryCounter < KCatalogsServerStartAttempts )
        {
        DLINFO(( "Trying to start server, try nro: %d", startTryCounter + 1 ));
        err = KErrNone; // reset before a new try
        
        // StartServer() does not return error even if the current server is
        // shutting down.
        err = StartServer();
        DLINFO(( "startserver %d", err ));
        if ( KErrNone == err )
            {
            // If current server is dying, this returns error
            err = CreateSession(
                KCatalogsServerName,
                Version(),
                KCatalogsDefaultMessageSlots );
            DLINFO(( "createsession %d", err ));
            if ( err == KErrNone )
                {
                iSessionStatus = EOpen;
                
                TIpcArgs args( aClientUid.iUid );
                err = SendSync( ECatalogsCreateContext, args );
                }
            }        
        
        if ( err == KErrNone )
            {
            break;
            }
            
        ++startTryCounter;        
        DLINFO(( "Did not succeed in connecting, entering sleep... " ));
        User::After( KRetryInterval ); // This halts the whole thread
        DLINFO(( "Retrying connect." ));
        }

    DLTRACEOUT(("err: %d", err));
    return err;
    }
    
void RCatalogsClientServerClientSession::Disconnect()
    {
    DLTRACEIN((""));
    // Before cleaning up the iTasks, we ensure that if there
    // possibly still are pending tasks, server-side objects don't
    // complete them anymore. Also we assume that everything
    // possible is already done to ensure that there are no
    // tasks pending.
    if ( iSessionStatus == EOpen )
        {
        // Have to be sure so that we don't try to
        // send message using invalid handle
        TIpcArgs args = TIpcArgs();
        SendSync( ECatalogsClientSideDown, args );
        }
    
    // We trust that disconnect is always said, so contents of
    // iTasks is destroyed here
    iTasks.ResetAndDestroy();

	RHandleBase::Close(); //close handle (this) to the server-side session
	
	iSessionStatus = EClosed;
	DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// Creation of a provider uses a internal message type
// to tell the server-side to create the desired provider.
// ---------------------------------------------------------------------------
//
void RCatalogsClientServerClientSession::CreateProvider(
                                                    TInt aUid,
                                                    TRequestStatus& aStatus,
                                                    TInt& aHandle,
                                                    TUint32 aOptions )
    {
    DLTRACEIN((""));

    aStatus = KRequestPending;

    CCatalogsClientServerAsyncTask* task = NULL;
    TRAPD( error, GetAsyncTaskL( task ) );
    if ( error != KErrNone )
        {
        // This is called before setActive in client?
        // Is that a problem?
        DLTRACEOUT(("error=%X",error));
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, error );
        return;
        }

    DLTRACE(("CreateProvider using task"));
    task->CreateProvider( aUid, aStatus, aHandle, aOptions );

    DLTRACEOUT(("RCatalogsClientServerClientSession end"));
    }

// ---------------------------------------------------------------------------
// Replace all asynchronous SendReceives in this class with this 
// SendAsync so we get all asynchronous sending into one function
// ---------------------------------------------------------------------------
//
void RCatalogsClientServerClientSession::SendAsync( 
    TCatalogsServerFunction aMessageType,
    const TIpcArgs& aArgs,
    TRequestStatus& aStatus )
    {
    aStatus = KRequestPending;
    SendReceive( aMessageType, aArgs, aStatus );
    }

// ---------------------------------------------------------------------------
// Replace all synchronous SendReceives in this class with this
// SendSync so we get all synchronous sending into one function
// ---------------------------------------------------------------------------
//
TInt RCatalogsClientServerClientSession::SendSync( 
    TCatalogsServerFunction aMessageType,
    const TIpcArgs& aArgs )
    {
    return SendReceive( aMessageType, aArgs );
    }

void RCatalogsClientServerClientSession::TaskCompleted(
    CCatalogsClientServerAsyncTask* aCompletedTask )
    {
    DLTRACEIN((""));
    TInt index( iTasks.Find( aCompletedTask ) );
    iTasks.Remove( index );
    delete aCompletedTask;
#ifdef CATALOGS_BUILD_CONFIG_DEBUG    
/*
    RThread currentThread;
    
    TInt semCount = currentThread.RequestCount();
    DLTRACE(("semCount: %d", semCount ));
    currentThread.Close();
*/    
#endif    
    
    DLTRACEOUT((""));
    }

TInt RCatalogsClientServerClientSession::DeleteIncompleteMessage( 
    TInt aHandle )
    {
    DLTRACEIN((""));
    TPckgBuf<TInt> handle( aHandle );
    TIpcArgs args = TIpcArgs();
    args.Set( 1, &handle );
    return SendReceive( ECatalogsRemoveIncompleteMessage,
                        args );    
    }
    

TVersion RCatalogsClientServerClientSession::Version() const
    {
    return TVersion( KCatalogsServerMajorVersionNumber, 
                     KCatalogsServerMinorVersionNumber, 
                     KCatalogsServerBuildVersionNumber );
    }
    
    
TInt RCatalogsClientServerClientSession::SendSync( TInt aFunction,
                                                   const TDesC8& aInput,
                                                   TDes8& aOutput,
                                                   TInt aHandle )
    {
    DLTRACEIN(("Handle: %d", aHandle));
    // Actually these would not need to be in TPckgBuf
    // when they are not used for returning purposes.
    TPckgBuf<TInt> function( aFunction );
    TPckgBuf<TInt> handle( aHandle );
 
    return SendReceive( ECatalogsExternalMessage,
                        TIpcArgs( &function,
                                  &handle,
                                  &aInput,
                                  &aOutput ));
    }


TInt RCatalogsClientServerClientSession::SendSync( TInt aFunction,
                                                   const TDesC16& aInput,
                                                   TDes16& aOutput,
                                                   TInt aHandle )
    {
    DLTRACEIN(("Handle: %d", aHandle));
    // Actually these would not need to be in TPckgBuf
    // when they are not used for returning purposes.
    TPckgBuf<TInt> function( aFunction );
    TPckgBuf<TInt> handle( aHandle );
 
    return SendReceive( ECatalogsExternalMessage,
                         TIpcArgs( &function,
                                   &handle,
                                   &aInput,
                                   &aOutput ));
    }


TInt RCatalogsClientServerClientSession::SendSync( TInt aFunction,
                                                   const TDesC16& aInput,
                                                   TInt& aOutputInt,
                                                   TInt aHandle )
    {
    DLTRACEIN(("Handle: %d", aHandle));

    // Actually these would not need to be in TPckgBuf
    // when they are not used for returning purposes.
    TPckgBuf<TInt> function( aFunction );
    TPckgBuf<TInt> handle( aHandle );

    TPckgBuf<TInt> integerReturnBuf;

    DLTRACE(("Sending message to server"));
    
    TInt returnValue = SendReceive( ECatalogsExternalMessage,
                                    TIpcArgs( &function,
                                              &handle,
                                              &aInput,
                                              &integerReturnBuf ));
    
    DLTRACE(("Done, err: %i, handle: %d", returnValue, aHandle ));
    
    aOutputInt = integerReturnBuf();
    return returnValue;
    }

TInt RCatalogsClientServerClientSession::SendSync( TInt aFunction,
                                                   const TDesC8& aInput,
                                                   TInt& aOutputInt,
                                                   TInt aHandle )
    {
    DLTRACEIN(("Handle: %d", aHandle));

    // Actually these would not need to be in TPckgBuf
    // when they are not used for returning purposes.
    TPckgBuf<TInt> function( aFunction );
    TPckgBuf<TInt> handle( aHandle );

    TPckgBuf<TInt> integerReturnBuf;

    DLTRACE(("Sending message to server"));
    
    TInt returnValue = SendReceive( ECatalogsExternalMessage,
                                    TIpcArgs( &function,
                                              &handle,
                                              &aInput,
                                              &integerReturnBuf ));
    
    DLTRACE(("Done, err: %i, handle: %d", returnValue, aHandle ));
    
    aOutputInt = integerReturnBuf();
    return returnValue;
    }


// ---------------------------------------------------------------------------
// When noticed the need for 16-bit versions of functions, thought that
// because asynchronous functions would not work by converting the call
// from 16-bit to 8-bit in an inline function, we put all the functions into
// the interface and add implementations here. Also thought that
// as we have functions to put the 16-bit descriptor through the
// client-server we use them instead of pushing the 16-bit descriptors in
// 8-bit descriptors to the other side.
// Now we can see that this last decision makes up some repitition when
// it comes to synchronous alloc-functions.
//
// Note: If good ideas come up, replace variants of alloc functions that
//       do same things only to different kind of descriptors
//       (8- and 16-bit variants) with possibly only one implementation.
// ---------------------------------------------------------------------------
//
TInt RCatalogsClientServerClientSession::SendSyncAlloc( TInt aFunction,
                                                        const TDesC8& aInput,
                                                        HBufC8*& aOutput,
                                                        TInt aHandle,
                                                        TInt aLength )
    {
    DLTRACEIN(("Handle: %d", aHandle));
    
    // Although aOutput should be null, it is deleted to be sure that
    // no memory is leaked
    delete aOutput;
    aOutput = NULL;
    
    HBufC8* tempReturnBuf( NULL );
    TInt returnValue = AllocReturnBuf( aLength, tempReturnBuf );
    if ( returnValue != KErrNone )
        {
        return returnValue;        
        }
        
    TPtr8 tempWritableReturnBuf( NULL, 0 );
    tempWritableReturnBuf.Set( tempReturnBuf->Des() );

    // Actually these would not need to be in TPckgBuf
    // when they are not used for returning purposes.
    TPckgBuf<TInt> function( aFunction );
    TPckgBuf<TInt> handle( aHandle );
 
    TInt outcome = SendReceive( ECatalogsExternalAllocMessage,
                                TIpcArgs( &function,
                                          &handle,
                                          &aInput,
                                          &tempWritableReturnBuf ));

    if ( outcome == KCatalogsErrorTooSmallDescriptor )
        {
        TInt incompleteMessageHandle = -1;
        TInt error = RetrieveNewDescLengthAndReAlloc(
                         tempReturnBuf,
                         incompleteMessageHandle );
        if ( error != KErrNone )
            {
            if ( incompleteMessageHandle != -1 )
                {
                DeleteIncompleteMessage( incompleteMessageHandle );
                }            
            aOutput = NULL;
            delete tempReturnBuf;
            return error;
            }
                
        // to be sure
        tempWritableReturnBuf.Set( tempReturnBuf->Des() );

        TPckgBuf<TInt> handle( incompleteMessageHandle );
        TIpcArgs args = TIpcArgs();
        args.Set( 1, &handle );
        args.Set( 3, &tempWritableReturnBuf );
        outcome = SendReceive( ECatalogsCompleteMessage,
                               args );
             
        }
        
        
    if ( outcome == KCatalogsErrorTooSmallDescriptor || outcome < 0 )
        {
        aOutput = NULL;
        delete tempReturnBuf;
        }
    else
        {
        aOutput = tempReturnBuf;
        }
    DLTRACEOUT(("outcome: %d, handle: %d", outcome, aHandle));
    return outcome;
    }

TInt RCatalogsClientServerClientSession::SendSyncAlloc( TInt aFunction,
                                                        const TDesC16& aInput,
                                                        HBufC16*& aOutput,
                                                        TInt aHandle,
                                                        TInt aLength )
    {
    DLTRACEIN(("Handle: %d", aHandle));

    // Although aOutput should be null, it is deleted to be sure that
    // no memory is leaked
    delete aOutput;
    aOutput = NULL;

    HBufC16* tempReturnBuf( NULL );
    TInt returnValue = AllocReturnBuf( aLength, tempReturnBuf );
    if ( returnValue != KErrNone )
        {
        return returnValue;        
        }
        
    TPtr16 tempWritableReturnBuf( NULL, 0 );
    tempWritableReturnBuf.Set( tempReturnBuf->Des() );

    // Actually these would not need to be in TPckgBuf
    // when they are not used for returning purposes.
    TPckgBuf<TInt> function( aFunction );
    TPckgBuf<TInt> handle( aHandle );
 
    TInt outcome = SendReceive( ECatalogsExternalAllocMessage,
                                TIpcArgs( &function,
                                          &handle,
                                          &aInput,
                                          &tempWritableReturnBuf ));

    if ( outcome == KCatalogsErrorTooSmallDescriptor )
        {
        TInt incompleteMessageHandle = -1;
        TInt error = RetrieveNewDescLengthAndReAlloc(
                         tempReturnBuf,
                         incompleteMessageHandle );
        if ( error != KErrNone )
            {
            if ( incompleteMessageHandle != -1 )
                {
                DeleteIncompleteMessage( incompleteMessageHandle );
                }            
            aOutput = NULL;
            delete tempReturnBuf;
            return error;
            }
                
        // to be sure
        tempWritableReturnBuf.Set( tempReturnBuf->Des() );

        TPckgBuf<TInt> handle( incompleteMessageHandle );
        TIpcArgs args = TIpcArgs();
        args.Set( 1, &handle );
        args.Set( 3, &tempWritableReturnBuf );
        outcome = SendReceive( ECatalogsCompleteMessageWide,
                               args );
             
        }
        
        
    if ( outcome == KCatalogsErrorTooSmallDescriptor || outcome < 0 )
        {
        aOutput = NULL;
        delete tempReturnBuf;
        }
    else
        {
        aOutput = tempReturnBuf;
        }

    DLTRACEOUT(("outcome: %d, handle: %d", outcome, aHandle));
    return outcome;
    }
    
    
    
RFile RCatalogsClientServerClientSession::SendSyncFileOpenL( 
    TInt aFunction,
    const TDesC8& aInput, 
    TInt aHandle )
    {
    DLTRACEIN(("Handle: %d", aHandle));

    // Actually these would not need to be in TPckgBuf
    // when they are not used for returning purposes.
    TPckgBuf<TInt> function( aFunction );
    TPckgBuf<TInt> handle( aHandle );

    TPckgBuf<TInt> fileHandleBuf;

    DLTRACE(("Sending message to server"));
    
    TInt fsHandle = SendReceive( ECatalogsExternalMessage,
                                    TIpcArgs( &function,
                                              &handle,
                                              &aInput,
                                              &fileHandleBuf ));
    
    TInt fileHandle = fileHandleBuf();
    DLTRACE(("AdoptFromServer( %d, %d )", fsHandle, fileHandle ));
    RFile file;
    User::LeaveIfError( file.AdoptFromServer( fsHandle, fileHandle ) );
    
    DLTRACE(("Done, handle: %d", aHandle ));
    
    
    return file;    
    }
    
    
    
void RCatalogsClientServerClientSession::SendAsync( TInt aFunction,
                                                    const TDesC8& aInput,
                                                    TDes8& aOutput,
                                                    TInt aHandle,
                                                    TRequestStatus& aStatus )
    {
    DLTRACEIN(("Handle: %d", aHandle));
    aStatus = KRequestPending;

    CCatalogsClientServerAsyncTask* task = NULL;
    TRAPD( error, GetAsyncTaskL( task ) );
    if ( error != KErrNone )
        {
        DLERROR(( "Error getting an async task: %d", error ));
        // This is called before setActive in client?
        // Is that a problem?
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, error );
        return;
        }

    task->SendAsync( aFunction, aInput, aOutput, aHandle, aStatus );
    }
    
    
void RCatalogsClientServerClientSession::SendAsyncAlloc( 
                                                    TInt aFunction,
                                                    const TDesC8& aInput,
                                                    HBufC8*& aOutput,
                                                    TInt aHandle,
                                                    TRequestStatus& aStatus,
                                                    TInt aLength )
    {
    DLTRACEIN(("Handle: %d", aHandle));

    // Although aOutput should be null, it is deleted to be sure that
    // no memory is leaked
    delete aOutput;
    aOutput = NULL;

    aStatus = KRequestPending;
    
    CCatalogsClientServerAsyncTask* task = NULL;
    TRAPD( error, GetAsyncTaskL( task ) );
    if ( error != KErrNone )
        {
        // This is called before setActive in client?
        // Is that a problem?
        DLERROR(("Error %d occurred when retrieving new async task", error ));
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, error );        
        return;
        }

    task->SendAsyncAlloc( aFunction,
                          aInput,
                          aOutput, 
                          aHandle,
                          aStatus,
                          aLength );
    DLTRACEOUT((""));
    }

void RCatalogsClientServerClientSession::AsyncMessageSenderDown(
    TRequestStatus& aStatus )
    {
    // Go through all messages
    TInt indexer( iTasks.Count() - 1 );
    while ( indexer > -1 )
        {
        // If the message sender's TRequestStatus is the same as the
        // given TRequestStatus, the async task should complete
        // the request immediately and continue to wait for the completion
        // of the server side message. Comparation done by comparing
        // pointers to the request statuses.
        if ( &aStatus == iTasks[indexer]->SendersRequestStatus() )
            {
            iTasks[indexer]->SenderDown();
            }
        --indexer;
        }
    // If no tasks were found with given sender and there has
    // actually been task(s) that should complete request it is possible
    // that the request has been recently completed but the completion
    // has not yet been handled by active scheduler (?) and passed to
    // the sender.
    }
    
void RCatalogsClientServerClientSession::GetAsyncTaskL( 
    CCatalogsClientServerAsyncTask*& aTask )
    {
    CCatalogsClientServerAsyncTask* requestedTask = 
        CCatalogsClientServerAsyncTask::NewLC( *this );
    
    iTasks.AppendL( requestedTask );
    CleanupStack::Pop( requestedTask );
    
    aTask = requestedTask;
    }
