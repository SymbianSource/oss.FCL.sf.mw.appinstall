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
* Description:   Implementation of debug logging utilities
*
*/


#ifndef __WINS__
#   ifdef CATALOGS_BUILD_CONFIG_HEAP_CHECKER
#       undef CATALOGS_BUILD_CONFIG_HEAP_CHECKER
#       warning "Automatically disabling heap checker in ARMV5 build!"
#   endif
#   ifdef CATALOGS_BUILD_CONFIG_DEBUG
#       warning "Debug logging on in ARM5 build"!
#   endif
#endif

#ifdef __WINS__
#   ifdef CATALOGS_BUILD_CONFIG_HEAP_CHECKER
#      warning "Heap checker available!"
#   endif
#endif

// Enable this to get alternative XML output
//#define CATALOGS_DEBUG_XML

// Quick method to disable debug functions:
// #undef CATALOGS_BUILD_CONFIG_DEBUG
// #undef CATALOGS_BUILD_CONFIG_HEAP_CHECKER
// #undef CATALOGS_EXT_LOGGER

#include <flogger.h>
#include <e32msgqueue.h>
#include <e32property.h>
#include <hal.h>

#include "catalogsdebug.h"
#include "e32debug.h"

#ifndef __WINS__
extern TUint32 GetStackPointer();
#endif

//#define _DDPRINT( x ) RDebug::Printf x
#define _DDPRINT( x )

// Logger includes from ../../debuglogger/inc/
#include "catalogsdebugdefs.h"
#include "catalogslogger.hrh"

#undef CATALOGS_FILEID
#define CATALOGS_FILEID "$Id: //depot/nf-catalogs/impl/catalogs/engine/src/catalogsdebug.cpp#43 $"

#ifdef CATALOGS_DEBUG_XML
_LIT8( KCatalogsError,   "error" );
_LIT8( KCatalogsWarning, "warning" );
_LIT8( KCatalogsInfo, "info" );
_LIT8( KCatalogsTraceIn, "func" );
_LIT8( KCatalogsTraceOut, "out" );
_LIT8( KCatalogsTraceLeave, "leave" );
_LIT8( KCatalogsTrace, "trace" );
_LIT8( KLogDir, "Catalogs" );
_LIT8( KLogName, "debug.log" );
#else
_LIT8( KCatalogsError,   "ERROR" );
_LIT8( KCatalogsWarning, "WARNING" );
_LIT8( KCatalogsInfo, "INFO" );
_LIT8( KCatalogsTraceIn, "-->" );
_LIT8( KCatalogsTraceOut, "<--" );
_LIT8( KCatalogsTraceLeave, "<--LEAVE" );
_LIT8( KCatalogsTrace, "@" );
_LIT8( KCatalogsSeparator, "\t" );
_LIT( KLogDir, "Catalogs" );
_LIT( KLogName, "debug.log" );
#endif

const TInt KLineLength = 120;

#ifdef CATALOGS_BUILD_CONFIG_DEBUG
static void Append( const char* aText, TDes16& aDestination )
    {
    TPtrC8 ptr( reinterpret_cast<const TUint8*>( aText ) );
    for ( TInt i = 0; i < ptr.Length(); i++ )
        {
        aDestination.Append( ptr[i] );
        }
    }
#endif

#ifndef CATALOGS_DEBUG_XML
static void Append( TPtrC8 aText, TDes8& aDestination, TInt aLength = 60 )
    {
    for ( TInt i = 0; i < aLength; i++ )
        {
        if ( i < aText.Length() )
            {
            aDestination.Append( aText[i] );
            }
        else
            {
            aDestination.Append( '.' );
            }
        }
    }
#else
static void Append( TPtrC8 aText, TDes16& aDestination )
    {
    TInt len = aText.Length();
    for ( TInt i = 0; i < len; i++ )
        {
        aDestination.Append( aText[i] );
        }
    }

const TDesC8& TypeTag( TCatalogsDebug::TType aType )
    {
    switch ( aType )
        {
        case TCatalogsDebug::EInfo:
            return KCatalogsInfo;
            break;

        case TCatalogsDebug::EError:
            return KCatalogsError;
            break;

        case TCatalogsDebug::EWarning:
            return KCatalogsWarning;
            break;

        case TCatalogsDebug::ETrace:
            return KCatalogsTrace;
            break;

        case TCatalogsDebug::ETraceIn:
            return KCatalogsTraceIn;
            break;

        case TCatalogsDebug::ETraceOut:
            return KCatalogsTraceOut;
            break;
            
        case TCatalogsDebug::ETraceLeave:
            return KDebugLoggerTraceLeave;
            break;            

        default:
            DASSERT( EFalse );
            break;
        }

    return KNullDesC(); // for compiler
    }

#endif

EXPORT_C TCatalogsDebug::TCatalogsDebug(
    TType aType,
    const char* aFunction,
    TInt aLine,
    const char* aFileId,
    TUint aDeltaTime,
    TInt aOutput )
        : iType( aType ), iFunction( aFunction ), iLine( aLine ), iFileId( aFileId ), 
          iDeltaTime( aDeltaTime ), iOutput( aOutput )
    {}

inline RCatalogsDebugHeap* DebugHeap()
    {
    return static_cast< RCatalogsDebugHeap* >( &User::Heap() );
    }

void TCatalogsDebug::PrintGeneral( TPtr8 aPrintBuf )
    {
#ifdef CATALOGS_DEBUG_XML

    // Insert start tag beginning.
    aPrintBuf.AppendFormat( "<%S name=\"", &TypeTag( iType ) );
        
    // Process the function string. Assume __PRETTY_FUNCTION__ -style string
    TPtrC8 func( reinterpret_cast<const TUint8*>( iFunction ) );
    // drop params
    func.Set( func.Left( func.Locate( '(' ) ) );
    // drop return value (if specified)
    TInt nameStart = func.LocateReverse( ' ' );
    if ( nameStart != KErrNotFound )
        {
        func.Set( func.Mid( nameStart+1 ) );
        }

    // Insert method name
    aPrintBuf.Append( func );
    aPrintBuf.Append( "\" file=\"" );

    // Insert file name
    aPrintBuf.Append( TDesC8( iFileId ) );

    // Insert the rest of the attributes
    RThread thread;
    TInt totalAllocSize = 0;
    User::AllocSize( totalAllocSize );
    aPrintBuf.AppendFormat( "\" line=\"%d\" thread=\"%x%x\" alloc=\"%d\">",
        iLine, (TUint32)(thread.Id().Id()>>32), (TUint32)thread.Id().Id(), totalAllocSize>>10 );
    
#else   // CATALOGS_DEBUG_XML

    // Insert current thread id.
    RThread thread;
    aPrintBuf.AppendNum( thread.Id().Id(), EHex );
    aPrintBuf.Append( ' ' );

    // Process the function string. Assume __PRETTY_FUNCTION__ -style string
    TPtrC8 ptr( reinterpret_cast<const TUint8*>( iFunction ) );
    // drop params
    ptr.Set( ptr.Left( ptr.Locate( '(' ) ) );
    // drop return value (if specified)
    TInt nameStart = ptr.LocateReverse( ' ' );
    if ( nameStart != KErrNotFound )
        {
//        aPrintBuf.Append( ptr.Mid( nameStart+1 ) );
        Append( ptr.Mid( nameStart+1 ), aPrintBuf );
        }
    else
        {
//        aPrintBuf.Append( ptr );
        Append( ptr, aPrintBuf );
        }
    aPrintBuf.Append( KCatalogsSeparator );
    aPrintBuf.AppendNum( iLine );
    aPrintBuf.Append( KCatalogsSeparator );

    TInt totalAllocSize = 0;
    User::AllocSize( totalAllocSize );
    aPrintBuf.Append( 'M' );
    aPrintBuf.AppendNum( totalAllocSize>>10 );

    TThreadStackInfo stackInfo;
    TInt err = thread.StackInfo( stackInfo );
    if( err == KErrNone )
        {
#ifdef __WINS__        
        volatile TInt32 currentSp=0;
        __asm { mov currentSp, esp }
#else
        TInt32 currentSp = GetStackPointer();
#endif        
//         TInt remainingStack = currentSp - (TInt)stackInfo.iLimit;
        TInt remainingStack = (TInt)stackInfo.iBase - currentSp;
        aPrintBuf.Append( '/' );
        if( remainingStack > 1023 )
            {
            aPrintBuf.AppendNum( remainingStack >> 10 );
            aPrintBuf.Append( 'k' );
            }
        else
            {
            aPrintBuf.AppendNum( remainingStack );
            }
        }

    aPrintBuf.Append( KCatalogsSeparator );

    switch ( iType )
        {
        case EError:
            aPrintBuf.Append( KCatalogsError );
            break;

        case EWarning:
            aPrintBuf.Append( KCatalogsWarning );
            break;

        case ETrace:
            aPrintBuf.Append( KCatalogsTrace );
            break;

        case ETraceIn:
            aPrintBuf.Append( KCatalogsTraceIn );
            break;

        case ETraceOut:
            aPrintBuf.Append( KCatalogsTraceOut );
            break;            

        case ETraceLeave:
        	aPrintBuf.Append( KCatalogsTraceLeave );
            break;            

        case EInfo:
            aPrintBuf.Append( KCatalogsInfo );
            break;

        default:
            break;
        }
        
#endif // CATALOGS_DEBUG_XML
    }


void TCatalogsDebug::FileWrite( const TDesC8& aPrintBuf )
    {
    TLex8 lex( aPrintBuf );
    TInt length = KLineLength;
    lex.Mark();
    while ( ! lex.Eos() )
        {
        while ( length-- > 0 && ! lex.Eos() )
            {
            lex.Inc();
            }
        length = KLineLength;
        TPtrC8 line( lex.MarkedToken() );
        lex.Mark();
        RFileLogger::Write( KLogDir, KLogName, EFileLoggingModeAppend, line );
        }
    }

EXPORT_C void TCatalogsDebug::Print( TRefByValue<const TDesC16> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );

    HBufC8* buffer = NULL;
    HBufC* buffer16 = NULL;
    HBufC8* buffer16to8  = NULL;

    if( iOutput & EOutputExtLogger )
        if( !DebugHeap()->IsEnabled( iType ) ) return;

    // Disable debug heap (if installed) to prevent infinite recursion.
    TBool debugHeapActive = RCatalogsDebugHeap::Activate( EFalse );

    buffer = HBufC8::New( KCatalogsDebugBufferSize );
    if ( buffer == NULL ) goto exit1;

    PrintGeneral( buffer->Des() );
    
    buffer16 = HBufC::New( KCatalogsDebugBufferSize );
    if ( buffer16 == NULL ) goto exit2;
    buffer16->Des().AppendFormatList( aFmt, list );

    buffer16to8 = HBufC8::New( buffer16->Length() );
    if ( buffer16to8 == NULL ) goto exit3;
    buffer16to8->Des().Copy( *buffer16 );

#ifndef CATALOGS_DEBUG_XML
    buffer->Des().Append( KCatalogsSeparator );

    buffer->Des().Append( *buffer16to8 );
#else
    TInt formatPos = buffer->Length();
    buffer->Des().Append( *buffer16to8 );
    // replace xml-evil chars with ¤
    TPtr8 formattedText = buffer->Des().MidTPtr( formatPos );
    TInt pos;

    while( (pos = formattedText.Locate( '<' )) != KErrNotFound )
        {
        formattedText[pos] = '¤';
        }
    while( (pos = formattedText.Locate( '>' )) != KErrNotFound )
        {
        formattedText[pos] = '¤';
        }
    while( (pos = formattedText.Locate( '&' )) != KErrNotFound )
        {
        formattedText[pos] = '¤';
        }
    
#endif

#ifdef CATALOGS_DEBUG_XML
    // In tag is not closed until after out
    if( iType != ETraceIn )
        {
        // close the tag
        buffer->Des().AppendFormat( "</%S>", &TypeTag( iType ) );
        if( iType == ETraceOut || iType == ETraceLeave )
            {
            buffer->Des().AppendFormat( "</%S>", &KCatalogsTraceIn() );
            }
        }
#endif

    if( iOutput & EOutputRDebug )
        RDebug::Printf( (char*)buffer->Des().PtrZ() );

    if( iOutput & EOutputExtLogger )
        DebugHeap()->ChunkOutput( *buffer, iDeltaTime );

    if( iOutput & EOutputFileLogger )
        FileWrite( *buffer );

    delete buffer16to8;
exit3:
    delete buffer16;
exit2:
    delete buffer;
exit1:

    RCatalogsDebugHeap::Activate( debugHeapActive );
    }


EXPORT_C void TCatalogsDebug::Print( const char* aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );

    if( iOutput & EOutputExtLogger )
        {
        if( !DebugHeap()->IsEnabled( iType ) ) return;
        }

    // Disable debug heap (if installed) to prevent infinite recursion.
    TBool debugHeapActive = RCatalogsDebugHeap::Activate( EFalse );

    HBufC8* buffer = HBufC8::New( KCatalogsDebugBufferSize );
    if ( buffer == NULL ) return;

    PrintGeneral( buffer->Des() );
#ifndef CATALOGS_DEBUG_XML
    buffer->Des().Append( KCatalogsSeparator );
    buffer->Des().AppendFormatList( TPtrC8( ( const TUint8* )aFmt ), list );
#else
    TInt formatPos = buffer->Length();
    buffer->Des().AppendFormatList( TPtrC8( ( const TUint8* )aFmt ), list );

    // replace xml-evil chars with ¤
    TPtr8 formattedText = buffer->Des().MidTPtr( formatPos );
    TInt pos;
    while( (pos = formattedText.Locate( '<' )) != KErrNotFound )
        {
        formattedText[pos] = '¤';
        }
    while( (pos = formattedText.Locate( '>' )) != KErrNotFound )
        {
        formattedText[pos] = '¤';
        }
    while( (pos = formattedText.Locate( '&' )) != KErrNotFound )
        {
        formattedText[pos] = '¤';
        }

    // In tag is not closed until after out
    if( iType != ETraceIn )
        {
        // close the tag
        buffer->Des().AppendFormat( "</%S>", &TypeTag( iType ) );
        if( iType == ETraceOut || iType == ETraceLeave )
            {
            buffer->Des().AppendFormat( "</%S>", &KCatalogsTraceIn() );
            }
        }
#endif

    if( iOutput & EOutputRDebug )
        RDebug::Printf( (char*)buffer->Des().PtrZ() );

    if( iOutput & EOutputExtLogger )
        DebugHeap()->ChunkOutput( *buffer, iDeltaTime );

    if( iOutput & EOutputFileLogger )
        FileWrite( *buffer );

    delete buffer;

    RCatalogsDebugHeap::Activate( debugHeapActive );
    }


EXPORT_C void TCatalogsDebug::DumpData( const TAny* aData, TInt aSize, TInt aClipToSize )
    {
    TInt size = aSize;

    Print( "dump: %d bytes total", aSize );

    TUint8* data = ( TUint8* )aData;

    while ( size > 0 )
        {
        // Cut dumps of > aClipToSize from the middle.
        if ( ( aSize-size > aClipToSize/2 ) && ( size > aClipToSize/2 ) )
            {
            TInt clipAmount = ( ( aSize - aClipToSize ) / 8 + 1 ) * 8;

            Print( "... %d bytes of data clipped ...", clipAmount );

            size -= clipAmount;
            data += clipAmount;
            continue;
            }

        switch ( size )
            {
            case 1:
                Print( "%04x: %02x", aSize-size,
                       ( TInt )data[0] );
                break;

            case 2:
                Print( "%04x: %02x %02x", aSize-size,
                       ( TInt )data[0], ( TInt )data[1] );
                break;

            case 3:
                Print( "%04x: %02x %02x %02x", aSize-size,
                       ( TInt )data[0], ( TInt )data[1], ( TInt )data[2] );
                break;

            case 4:
                Print( "%04x: %02x %02x %02x %02x", aSize-size,
                       ( TInt )data[0], ( TInt )data[1], ( TInt )data[2],
                       ( TInt )data[3] );
                break;

            case 5:
                Print( "%04x: %02x %02x %02x %02x %02x", aSize-size,
                       ( TInt )data[0], ( TInt )data[1], ( TInt )data[2],
                       ( TInt )data[3], ( TInt )data[4] );
                break;

            case 6:
                Print( "%04x: %02x %02x %02x %02x %02x %02x", aSize-size,
                       ( TInt )data[0], ( TInt )data[1], ( TInt )data[2],
                       ( TInt )data[3], ( TInt )data[4], ( TInt )data[5] );
                break;

            case 7:
                Print( "%04x: %02x %02x %02x %02x %02x %02x %02x", aSize-size,
                       ( TInt )data[0], ( TInt )data[1], ( TInt )data[2],
                       ( TInt )data[3], ( TInt )data[4], ( TInt )data[5],
                       ( TInt )data[6] );
                break;

            default:    // 8 or more
                Print( "%04x: %02x %02x %02x %02x %02x %02x %02x %02x", aSize-size,
                       ( TInt )data[0], ( TInt )data[1], ( TInt )data[2],
                       ( TInt )data[3], ( TInt )data[4], ( TInt )data[5],
                       ( TInt )data[6], ( TInt )data[7] );
                break;
            }

        size -= 8;
        data += 8;
        }
    }


TBool RCatalogsDebugHeap::IsEnabled( TCatalogsDebug::TType aPrintType )
    {
    if( iChunkIndex == -1 ) return EFalse;
    TCatalogsDebugChunkHeader* header = (TCatalogsDebugChunkHeader*)iChunk[iChunkIndex].Base();

    switch( aPrintType )
        {
        case TCatalogsDebug::EError:
            return header->iFlags & ECatalogsDebugFlagEnableError;

        case TCatalogsDebug::EWarning:
            return header->iFlags & ECatalogsDebugFlagEnableWarning;

        case TCatalogsDebug::EInfo:
            return header->iFlags & ECatalogsDebugFlagEnableInfo;

        case TCatalogsDebug::ETrace:
        case TCatalogsDebug::ETraceIn:
        case TCatalogsDebug::ETraceOut:
        case TCatalogsDebug::ETraceLeave:
            return header->iFlags & ECatalogsDebugFlagEnableTrace;
        }
    return EFalse;
    }

void RCatalogsDebugHeap::ChunkOutput( const TDesC8& aBuffer, TUint aDeltaTime )
{
    if( iChunkIndex == -1 )
        return; // not initialized ok; debuglogger app propably not running

    // Get access to output.
    iMutex.Wait();

    // Read the output chunk header.
    TCatalogsDebugChunkHeader* header = (TCatalogsDebugChunkHeader*)iChunk[iChunkIndex].Base();
    if( header->iFlags & ECatalogsDebugFlagFlushChunk )
    {
    _DDPRINT(( "DPRN: Debug chunk %d -> %d, flushed by someone else", iChunkIndex, iChunkIndex ^ 1 ));

        // Chunk flushed, need to switch.
        iChunkIndex ^= 1;   // toggle 0/1
    
        header = (TCatalogsDebugChunkHeader*)iChunk[iChunkIndex].Base();
    }
    
    if( header->iFlags & ECatalogsDebugFlagFlushChunk )
    {
        // Horror error
        DASSERT( EFalse );
    }
    
    // Now we have exclusive access to writable chunk.
    
    // Generate time stamp.

    TUint32 fastCounter = User::FastCounter();
/*
    TBuf8< 14 > timeStamp;  // HH:mm:ss.ss
    TUint32 hours = fastCounter / (fastCounterFrequency*60*60);
    fastCounter -= hours * (fastCounterFrequency*60*60);
    TUint32 minutes = fastCounter / (fastCounterFrequency*60);
    fastCounter -= minutes * (fastCounterFrequency*60);
    TUint32 seconds = fastCounter / fastCounterFrequency;
    fastCounter -= seconds * fastCounterFrequency;
    TUint32 secondParts = fastCounter / (fastCounterFrequency / 100 );
    
    timeStamp.Format( "%02d:%02d:%02d.%02d", hours, minutes, seconds, secondParts );
*/

    TBuf8< 32 > timeStamp;
    TUint32 seconds = fastCounter / iFastCounterFrequency;
    TUint32 secondParts = (fastCounter % iFastCounterFrequency) * 1000 / iFastCounterFrequency;
    if( aDeltaTime != 0 )
        {
        TUint32 dseconds = aDeltaTime / iFastCounterFrequency;
        TUint32 dsecondParts = (aDeltaTime % iFastCounterFrequency) * 10000 / iFastCounterFrequency;
        timeStamp.Format( _L8("%u.%03u %u.%04u\t"), seconds, secondParts, dseconds, dsecondParts );
        }
    else
        {
        timeStamp.Format( _L8("%u.%03u       \t"), seconds, secondParts );
        }
    
//    aPrintBuf.AppendNum( User::FastCounter() );
    
    TInt spaceRemaining = KCatalogsDebugChunkSize - sizeof( TCatalogsDebugChunkHeader) - header->iOffset;
    if( spaceRemaining < (timeStamp.Size() + aBuffer.Size() + KCatalogsDebugLineSeparator().Size() ) )
    {
        _DDPRINT(( "DPRN: Debug chunk %d -> %d, full, flushing", iChunkIndex, iChunkIndex ^ 1 ));

        // Not enough space for writing the entry to current chunk, need to flush it.
        header->iFlags |= ECatalogsDebugFlagFlushChunk;
        iMsgQueue.Send( iChunkIndex );
        
        // Switch to the other chunk, not letting other threads get in between
        iChunkIndex ^= 1;   // toggle 0/1
        
        // Get access to the new chunk. Will block if the block is still being processed by ext-logger.
        // The semaphore will be signalled by ext-logger after the chunk has been processed.
        _DDPRINT(( "DPRN: Debug chunk write semaphore wait" ));
        iChunkWriteSemaphore.Wait();
        _DDPRINT(( "DPRN: Debug chunk write semaphore wait done" ));

        // Initialize the new chunk.
        header = (TCatalogsDebugChunkHeader*)iChunk[iChunkIndex].Base();
        header->iFlags &= ~ECatalogsDebugFlagFlushChunk;
        header->iOffset = 0;
    }
    
    // Copy the output buffer to output chunk.
    TUint8* ptr = (TUint8*)(header+1) + header->iOffset;
    Mem::Copy( ptr, timeStamp.Ptr(), timeStamp.Size() );
    ptr += timeStamp.Size();
    Mem::Copy( ptr, aBuffer.Ptr(), aBuffer.Size() );
    ptr += aBuffer.Size();
    Mem::Copy( ptr, KCatalogsDebugLineSeparator().Ptr(), KCatalogsDebugLineSeparator().Size() );
 
    // Update the output chunk header.
    header->iOffset += timeStamp.Size() + aBuffer.Size() + KCatalogsDebugLineSeparator().Size();

    // Release output for others.
    iMutex.Signal(); 
}

void RCatalogsDebugHeap::InitExtLogger()
{
    TInt err = iChunk[0].OpenGlobal( KCatalogsDebugChunk1Name, EFalse );
    if( err != KErrNone ) return;

    err = iChunk[1].OpenGlobal( KCatalogsDebugChunk2Name, EFalse );
    if( err != KErrNone ) return;
    
    err = iMutex.OpenGlobal( KCatalogsDebugMutexName );
    if( err != KErrNone ) return;

    err = iChunkWriteSemaphore.OpenGlobal( KCatalogsDebugChunkWriteSemaphoreName );
    if( err != KErrNone ) return;

    err = iMsgQueue.OpenGlobal( KCatalogsDebugMsgQueueName );
    if( err != KErrNone ) return;

    TInt freq = 0;
    err = HAL::Get( HALData::EFastCounterFrequency, freq );
    DASSERT( freq >= 0 );
    iFastCounterFrequency = (TUint)freq;
    if( err != KErrNone ) return;

    // Init ok, start with chunk 0.
    iChunkIndex = 0;
    }

EXPORT_C void RCatalogsDebugHeap::InstallL( TBool aEnabled )
    {
//    DLTRACEIN( ( "" ) );
    RHeap* oldHeap = &User::Heap();
//    DLINFO( ( "Previous heap at %08x", oldHeap ) );
    RCatalogsDebugHeap* heap = new RCatalogsDebugHeap( *oldHeap, aEnabled );
    heap->InitExtLogger();
//    DLINFO( ( "Created debug heap at %08x, switching", heap ) );
    User::SwitchHeap( heap );
//    DLTRACEOUT( ( "" ) );
    }

EXPORT_C TBool RCatalogsDebugHeap::Activate( TBool aActive )
    {
    // Need some clever way to check that User::Heap() really is our heap.
    RCatalogsDebugHeap* heap = static_cast< RCatalogsDebugHeap* >( &User::Heap() );
    TBool result = heap->iActive;
    heap->iActive = aActive;
    return result;
    }

RCatalogsDebugHeap::RCatalogsDebugHeap( RHeap& aBaseHeap, TBool aEnabled )
    : iBaseHeap( aBaseHeap ), iAllocCounter( 0 ), iAllocInfo( 1024 /*list granularity*/ ), 
    iEnabled( aEnabled ), iActive( EFalse ), iChunkIndex( -1 )
    {
    }

RCatalogsDebugHeap::~RCatalogsDebugHeap()
    {
    iAllocInfo.Reset();
    iChunk[0].Close();
    iChunk[1].Close();
    iChunkWriteSemaphore.Close();
    iMutex.Close();
    iMsgQueue.Close();
    }

TAny* RCatalogsDebugHeap::operator new( TUint aSize )
    {
    return User::Heap().Alloc( aSize );
    }

void RCatalogsDebugHeap::operator delete( TAny* aPtr )
    {
    User::Heap().Free( aPtr );
    }

EXPORT_C void RCatalogsDebugHeap::Uninstall()
    {
    RCatalogsDebugHeap* heap = static_cast< RCatalogsDebugHeap* >( &User::Heap() );

    if( heap->iEnabled )
        {
        if( heap->iAllocInfo.Count() > 0 )
            {
        
            DLTRACEIN( ( "" ) );
            DLINFO( ( "Debug heap at %08x", heap ) );
        
            DLINFO( ( "%d entries in alloc info list", heap->iAllocInfo.Count() ) );
        
            for ( TInt i=0; i<heap->iAllocInfo.Count(); i++ )
                {
                TAllocInfo& info = heap->iAllocInfo[i];
                DLERROR( ( "ALLOC #%d: memory leak %d bytes at %08x:", info.iAllocNum, info.iAllocSize, info.iAllocPtr ) );
                const TInt KMaxText = 64;
                TPtrC8 text( reinterpret_cast<unsigned char*>( info.iAllocPtr ),
                             info.iAllocSize <= KMaxText ? info.iAllocSize : KMaxText );
                DLERROR( ( "text: %S", &text ) );
                DLERRORDUMP( info.iAllocPtr, info.iAllocSize, 1024 );
                }
            }
        }

    RHeap* oldHeap = &heap->iBaseHeap;
//    DLINFO( ( "Switching back to previous heap %08x", oldHeap ) );
    User::SwitchHeap( oldHeap );

    delete heap;

//    DLTRACEOUT( ( "" ) );
    }

TAny* RCatalogsDebugHeap::Alloc( TInt aSize )
    {
    TAny* result = iBaseHeap.Alloc( aSize );

    if ( iEnabled && iActive )
        {
        DLTRACEIN(("aSize=%d", aSize));
        iAllocCounter++;
        DLINFO( ( "ALLOC #%d: allocated %d bytes at %08x", iAllocCounter, aSize, result ) );

        TAllocInfo info;
        info.iAllocPtr = result;
        info.iAllocSize = aSize;
        info.iAllocNum = iAllocCounter;
 
        iActive = EFalse;   // deactivate temporarily to prevent internal alloc recording
        TInt err = iAllocInfo.Append( info );
        iActive = ETrue;

        if ( err != KErrNone )
            {
            DLERROR( ( "ALLOC ERROR appending alloc info" ) );
            }
        DLTRACEOUT(("%08x", result));
        }

    return result;
    }

void RCatalogsDebugHeap::Free( TAny* aPtr )
    {
    iBaseHeap.Free( aPtr );

    if( iEnabled && iActive )
        {
        DLTRACEIN(("aPtr=%08x", aPtr));
        for ( TInt i=0; i<iAllocInfo.Count(); i++ )
            {
            TAllocInfo& info = iAllocInfo[i];
            if ( info.iAllocPtr == aPtr )
                {
                DLINFO( ( "ALLOC #%d: freed %d bytes at %08x", info.iAllocNum, info.iAllocSize, aPtr ) );
                iAllocInfo.Remove( i );
                return;
                }
            }
        }
    }

TAny* RCatalogsDebugHeap::ReAlloc( TAny* aPtr, TInt aSize, TInt aMode )
    {
    TAny* result = iBaseHeap.ReAlloc( aPtr, aSize, aMode );

    if( iEnabled )
        {
        for ( TInt i=0; i<iAllocInfo.Count(); i++ )
            {
            TAllocInfo& info = iAllocInfo[i];
            if ( info.iAllocPtr == aPtr )
                {
                DLTRACEIN(("aPtr=%08x, aSize=%d, aMode=%d", aPtr, aSize, aMode));
                DLINFO( ( "ALLOC #%d: realloc %d bytes at %08x to %d bytes at %08x",
                          info.iAllocNum, info.iAllocSize, aPtr, aSize, result ) );
                info.iAllocSize = aSize;
                info.iAllocPtr = result;
                return result;
                }
            }
        }
    return result;
    }

TInt RCatalogsDebugHeap::AllocLen( const TAny* aCell ) const
    {
    return iBaseHeap.AllocLen( aCell );
    }

TInt RCatalogsDebugHeap::Compress()
    {
    return iBaseHeap.Compress();
    }

void RCatalogsDebugHeap::Reset()
    {
    DLTRACEIN((""));
    iBaseHeap.Reset();
    DLINFO( ( "ALLOC: heap reset!" ) );
    iAllocInfo.Reset();
    }

TInt RCatalogsDebugHeap::AllocSize( TInt& aTotalAllocSize ) const
    {
    return iBaseHeap.AllocSize( aTotalAllocSize );
    }

TInt RCatalogsDebugHeap::Available( TInt& aBiggestBlock ) const
    {
    return iBaseHeap.Available( aBiggestBlock );
    }

TInt RCatalogsDebugHeap::DebugFunction( TInt aFunc, TAny* a1, TAny* a2 )
    {
    return iBaseHeap.DebugFunction( aFunc, a1, a2 );
    }

TInt RCatalogsDebugHeap::Extension_( TUint /*aExtensionId*/, TAny*& /*a0*/, TAny* /*a1*/ )
    {
    DLTRACEIN((""));
    DASSERT( EFalse );
    return 0;
    }

static void CleanupWatcherPop( TAny* aArg )
    {
    TCatalogsLocalCleanupStackWatcher* watcher = static_cast< TCatalogsLocalCleanupStackWatcher* >( aArg );
    watcher->iPopped = ETrue;
    }

EXPORT_C TCatalogsLocalCleanupStackWatcher::TCatalogsLocalCleanupStackWatcher()
        : iPopped( EFalse )
    {
    CleanupDeletePushL( ( TAny* )NULL );  // use NULL pointer as a marker
    CleanupStack::PushL( TCleanupItem( CleanupWatcherPop, ( TAny* )this ) );
    }

EXPORT_C TCatalogsLocalCleanupStackWatcher::~TCatalogsLocalCleanupStackWatcher()
    {
    if ( !iPopped )
        {
        CleanupStack::PopAndDestroy();
        CleanupStack::Pop( ( TAny* )NULL ); // check for NULL marker at this position
        }
    }

static void DebugHeapRestore( TAny* aArg )
    {
    TCatalogsLocalDebugHeapActivator* activator = static_cast< TCatalogsLocalDebugHeapActivator* >( aArg );
    RCatalogsDebugHeap::Activate( activator->iActive );
    activator->iPopped = ETrue;
    }

EXPORT_C TCatalogsLocalDebugHeapActivator::TCatalogsLocalDebugHeapActivator()
    : iPopped( EFalse )
    {
    // Activate debug heap.
    iActive = RCatalogsDebugHeap::Activate( ETrue );

    // Push cleanup item for restoring the previous debug heap enable status.
    CleanupStack::PushL( TCleanupItem( DebugHeapRestore, ( TAny* )this ) );
    }

EXPORT_C TCatalogsLocalDebugHeapActivator::~TCatalogsLocalDebugHeapActivator()
    {
    if ( !iPopped )
        {
        // Restore debug heap previous enable status.
        RCatalogsDebugHeap::Activate( iActive );
        CleanupStack::Pop();
        }
    }

EXPORT_C TCatalogsLocalExitTrace::TCatalogsLocalExitTrace( const char* aFunctionName, TInt aLine, const char* aFileId )
    : iFunctionName( aFunctionName ), iLine( aLine ), iFileId( aFileId ), iDisabled( EFalse )
    {
    iEntryTime = User::FastCounter();
    }

EXPORT_C TCatalogsLocalExitTrace::~TCatalogsLocalExitTrace()
    {
    if ( std::uncaught_exception() )
    	{
    	TCatalogsDebug debug( TCatalogsDebug::ETraceLeave, iFunctionName, iLine, iFileId, User::FastCounter()-iEntryTime );
        debug.Print( "" );
    	}
    else if  ( !iDisabled )
        {
        TCatalogsDebug debug( TCatalogsDebug::ETraceOut, iFunctionName, iLine, iFileId, User::FastCounter()-iEntryTime );
        debug.Print( "" );
        }
    }

EXPORT_C void TCatalogsLocalExitTrace::Disable()
    {
    iDisabled = ETrue;
    }
