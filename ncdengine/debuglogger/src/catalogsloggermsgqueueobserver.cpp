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


#include "catalogsloggermsgqueueobserver.h"
#include "catalogsloggerappui.h"

//#define _DDPRINT( x ) RDebug::Printf x
#define _DDPRINT( x )

CCatalogsLoggerMsgQueueObserver* CCatalogsLoggerMsgQueueObserver::NewL()
    {
    CCatalogsLoggerMsgQueueObserver* self =
        CCatalogsLoggerMsgQueueObserver::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CCatalogsLoggerMsgQueueObserver* CCatalogsLoggerMsgQueueObserver::NewLC()
    {
    CCatalogsLoggerMsgQueueObserver* self =
        new( ELeave ) CCatalogsLoggerMsgQueueObserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CCatalogsLoggerMsgQueueObserver::~CCatalogsLoggerMsgQueueObserver()
    {
    Flush();
    Cancel();

    delete iObexSender;
    iLogFile.Close();
    iFs.Close();
    iChunk1.Close();
    iChunk2.Close();
    iChunkMutex.Close();
    iChunkWriteSemaphore.Close();
    iMsgQueue.Close();
    }

void CCatalogsLoggerMsgQueueObserver::StartLogging()
    {
    iIsLogging = ETrue;
    SetEnableFlags( iEnableFlags );
    }

void CCatalogsLoggerMsgQueueObserver::StopLogging()
    {
    TInt flags = iEnableFlags;
    SetEnableFlags( 0 );
    iIsLogging = EFalse;
    iEnableFlags = flags;
    Flush();
    }

TUint CCatalogsLoggerMsgQueueObserver::CurrentChunkOffset()
    {
    // take the the other chunk than the one last full/written
    RChunk& chunk = iLastChunk == 1 ? iChunk1 : iChunk2;

    // read its header
    TCatalogsDebugChunkHeader* header = (TCatalogsDebugChunkHeader*)chunk.Base();
    
    return header->iOffset;
    }

TPtrC8 CCatalogsLoggerMsgQueueObserver::LastData( TUint aMaxSize )
    {
    // take the the other chunk than the one last full/written
    RChunk& chunk = iLastChunk == 1 ? iChunk1 : iChunk2;

    // read its header
    TCatalogsDebugChunkHeader* header = (TCatalogsDebugChunkHeader*)chunk.Base();

    // calculate start offset to data
    TInt start = Max( (TInt)0, (TInt)header->iOffset - (TInt)aMaxSize );

    // return descriptor pointing to the data.
    return TPtrC8( (TUint8*)(header+1) + start, header->iOffset - start );
    }

void CCatalogsLoggerMsgQueueObserver::Flush()
    {
    // Prevent log writes.
    iChunkMutex.Wait();
    
    // take the the other chunk than the one last full/written
    RChunk& chunk1 = iLastChunk == 1 ? iChunk1 : iChunk2;

    TCatalogsDebugChunkHeader* header = (TCatalogsDebugChunkHeader*)chunk1.Base();
    if( !(header->iFlags & ECatalogsDebugFlagFlushChunk) && (header->iOffset > 0) )
        {
        _DDPRINT(( "DLOG: Debug chunk %d has data, flushing", iLastChunk ^ 1 ));

        // Make a descriptor to hold the data in chunk.
        TPtrC8 data( (TUint8*)(header+1), header->iOffset );
        
        // Write it to the log file in one big chunk.
        _DDPRINT(( "DLOG: Debug chunk %d data writing to disk", iLastChunk ^ 1 ));
        iLogFile.Write( data );
        _DDPRINT(( "DLOG: Debug chunk %d data written to disk", iLastChunk ^ 1 ));

        header->iOffset = 0;
        header->iFlags &= ~ECatalogsDebugFlagFlushChunk;
        }
    
    // check if the other one has already been written to as well
    RChunk& chunk2 = iLastChunk == 0 ? iChunk1 : iChunk2;
    
    header = (TCatalogsDebugChunkHeader*)chunk2.Base();
    if( !(header->iFlags & ECatalogsDebugFlagFlushChunk) )
        {
        _DDPRINT(( "DLOG: Debug chunk %d has data, flushing", iLastChunk ));

        // has been written to
        // Make a descriptor to hold the data in chunk.
        TPtrC8 data( (TUint8*)(header+1), header->iOffset );
        
        // Write it to the log file in one big chunk.
        _DDPRINT(( "DLOG: Debug chunk %d data writing to disk", iLastChunk ));
        iLogFile.Write( data );
        _DDPRINT(( "DLOG: Debug chunk %d data written to disk", iLastChunk ));

        header->iOffset = 0;
        header->iFlags |= ECatalogsDebugFlagFlushChunk;
        }
    
    // Enable log writes again.
    iChunkMutex.Signal();
    }

void CCatalogsLoggerMsgQueueObserver::RunL()
    {
    // Read the chunk to flush
    TInt msg;
    iMsgQueue.Receive( msg );

    _DDPRINT(( "DLOG: Debug chunk %d flushed, got message", msg ));

    // Reissue message request.
    iMsgQueue.NotifyDataAvailable( iStatus );
    SetActive();

    if( iIsLogging )
        {
        RChunk& chunk = msg == 0 ? iChunk1 : iChunk2;
        iLastChunk = msg;
        
        TCatalogsDebugChunkHeader* header = (TCatalogsDebugChunkHeader*)chunk.Base();
    
        // Make a descriptor to hold the data in chunk.
        TPtrC8 data( (TUint8*)(header+1), header->iOffset );
        
        // Write it to the log file in one big chunk.
        _DDPRINT(( "DLOG: Debug chunk %d writing to disk", msg ));
        iLogFile.Write( data );
        _DDPRINT(( "DLOG: Debug chunk %d written to disk", msg ));
    
        iFunCounter++;
        }

    // Signal the write semaphore.
    _DDPRINT(( "DLOG: Debug chunk write semaphore signal" ));
    iChunkWriteSemaphore.Signal();
    }

void CCatalogsLoggerMsgQueueObserver::SetEnableFlags( TInt aFlags )
    {
    iEnableFlags = aFlags;
    if( iIsLogging )
        {
        // Prevent log writes.
        iChunkMutex.Wait();
        TCatalogsDebugChunkHeader* header = (TCatalogsDebugChunkHeader*)iChunk1.Base();
        header->iFlags = (header->iFlags & ECatalogsDebugFlagFlushChunk) | aFlags;
        header = (TCatalogsDebugChunkHeader*)iChunk2.Base();
        header->iFlags = (header->iFlags & ECatalogsDebugFlagFlushChunk) | aFlags;
        
        // Enable log writes.
        iChunkMutex.Signal();
        }
    }

void CCatalogsLoggerMsgQueueObserver::DoCancel()
    {
    iMsgQueue.CancelDataAvailable();
    }

void ClearChunk( RChunk& aChunk, TInt aFlags )
{
    TCatalogsDebugChunkHeader* header = (TCatalogsDebugChunkHeader*)aChunk.Base();
    header->iFlags = aFlags;
    header->iOffset = 0;
}

void CCatalogsLoggerMsgQueueObserver::ConstructL()
    {
    // Open the log file for append
    User::LeaveIfError( iFs.Connect() );

    // Stuff log into MMC if available.
#ifndef __WINS__
    TVolumeInfo mmcVolume;
    if ( iFs.Volume( mmcVolume, EDriveE ) == KErrNone )
        {
        iLogFileName.Copy( _L("e:") );
        }
    else
#endif
        {
        iLogFileName.Copy( _L("c:") );
        }
    iLogFileName.Append( KCatalogsLoggerFileName );

    TInt err = iFs.MkDirAll( iLogFileName );
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
    
    User::LeaveIfError( iLogFile.Replace( iFs,
        iLogFileName,
        EFileShareAny | EFileWrite ) );

    // Create global chunks for debug log data.
    User::LeaveIfError( iChunk1.CreateGlobal( 
        KCatalogsDebugChunk1Name, 
        KCatalogsDebugChunkSize,
        KCatalogsDebugChunkSize ) );
    
    ClearChunk( iChunk1, iEnableFlags );
    
    User::LeaveIfError( iChunk2.CreateGlobal( 
        KCatalogsDebugChunk2Name, 
        KCatalogsDebugChunkSize,
        KCatalogsDebugChunkSize ) );
    
    ClearChunk( iChunk2, iEnableFlags | ECatalogsDebugFlagFlushChunk );
    
    // Create mutex for debug data chunks.
    User::LeaveIfError( iChunkMutex.CreateGlobal(
        KCatalogsDebugMutexName ) );

    // Create semaphore for block write control.
    User::LeaveIfError( iChunkWriteSemaphore.CreateGlobal(
        KCatalogsDebugChunkWriteSemaphoreName, 1 ) );

    // Create message queue.
    User::LeaveIfError( iMsgQueue.CreateGlobal( KCatalogsDebugMsgQueueName,
                                       KCatalogsLoggerNrOfSlots,
                                       EOwnerProcess ) );
    iMsgQueue.NotifyDataAvailable( iStatus );
    SetActive();
    }

CCatalogsLoggerMsgQueueObserver::CCatalogsLoggerMsgQueueObserver()
        : CActive( EPriorityStandard ), iLastChunk( 1 )
    {
    CActiveScheduler::Add( this );
    }

void CCatalogsLoggerMsgQueueObserver::SendFileL()
    {
    TBool wasLogging = IsLogging();
    if( IsLogging() )
        {
        StopLogging();
        }
    iLogFile.Close();

    iObexSender = COsmObexSender::NewL( *this );
    iObexSender->SendFileL( COsmObexSender::EOsmConnectionBT, iLogFileName );

    CActiveScheduler::Start();

    User::LeaveIfError( iObexSendError );

    User::LeaveIfError( iLogFile.Open( iFs,
                                       iLogFileName,
                                       EFileShareAny | EFileWrite ) );
    TInt pos = 0;
    User::LeaveIfError( iLogFile.Seek( ESeekEnd, pos ) );

    if( wasLogging )
        {
        StartLogging();
        }

    }

void CCatalogsLoggerMsgQueueObserver::ObexFileSent()
    {
    iObexSender->StopL();
    delete iObexSender;
    iObexSender = NULL;

    iObexSendError = KErrNone;
    CActiveScheduler::Stop();
    }

void CCatalogsLoggerMsgQueueObserver::ObexDisconnected( TInt aError, TInt /*aState*/ )
    {
    iObexSender->StopL();
    delete iObexSender;
    iObexSender = NULL;

    iObexSendError = aError;
    CActiveScheduler::Stop();
    }

TInt CCatalogsLoggerMsgQueueObserver::ClearLog()
    {
    TInt err = iLogFile.SetSize( 0 );
    if( err == KErrNone )
    {
        // Prevent log writes.
        iChunkMutex.Wait();
        ClearChunk( iChunk1, iEnableFlags );
        ClearChunk( iChunk2, iEnableFlags | ECatalogsDebugFlagFlushChunk );
        iLastChunk = 1;

        // Enable log writes.
        iChunkMutex.Signal();
    }
        
    return err;
    }
