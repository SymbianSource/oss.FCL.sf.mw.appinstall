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
* Description:   Debug logging utility
*
*/


/**
* INSTRUCTIONS
*
* To enable logging, add MACROs in the mmp for individual options:
* This enables debug logging and the undertaker watchdog.
* MACRO CATALOGS_BUILD_CONFIG_DEBUG
*
* Define this to direct debug output to external logger app (must be started before starting Catalogs).
* Only has effect if CATALOGS_BUILD_CONFIG_DEBUG is defined.
* MACRO CATALOGS_BUILD_CONFIG_DEBUG_EXT_LOGGER
*
* Define this to write debug output to a file using file logger (slow)
* Only has effect if CATALOGS_BUILD_CONFIG_DEBUG is defined.
* MACRO CATALOGS_BUILD_CONFIG_DEBUG_FILE_LOGGER
*
* Define this to send Catalogs debug print output to system debug log (RDebug)
* Only has effect if CATALOGS_BUILD_CONFIG_DEBUG is defined.
* MACRO CATALOGS_BUILD_CONFIG_DEBUG_RDEBUG
*
* This enables the heap checker utility, needs CATALOGS_BUILD_CONFIG_DEBUG.
* Note that on ARMV5 build the heap checker is never enabled.
* MACRO CATALOGS_BUILD_CONFIG_HEAP_CHECKER
*
* Enable method in/out logging
* MACRO CATALOGS_LOG_TRACE
*
* Enable general logging (dltrace etc matcros)
* MACRO CATALOGS_LOG_DEBUG
*
* Enable DASSERT macro
* MACRO CATALOGS_DEBUG_ASSERTS
*
* This first version will create normal a normal file log.
* Create dir "c:\Logs\Catalogs". Log file "debug.log" will be created there, if the 
* directory exists.
*
* For each cpp file, include "catalogsdebug.h". 
* For each MMP, include "ncddebug_20019119.lib".
* For each executable, place DLINIT macro in entry code before using any debug prints, and
* DLUNINIT macro in exit code after last debug prints have been made.
* 
* NOTE: DO NOT USE CATALOGS_FILEID. Causes trouble in branch management.
* To log Perforce file and version information, do this for each cpp file:
* #undef CATALOGS_FILEID
* #define CATALOGS_FILEID "$Id$"
* P4 will replace this with "//depot/path/file.txt#3" during the next submit.
* If this define is not done, only file name is logged.
* NOTE! $Id$ is only replaced if the corresponding file has keyword expansion enabled,
* this can be changed by right clicking a file in the changelist and selecting 
* "Change file type...".
*
* Logging is done using macros. Each macro automatically log the following information:
* - source file name and revision (from P4), or filename if not available
* - function name
* - line number
*
* Note the double parentheses in the macros.
*
* Use DLTRACEIN macro to log function entry, and DLTRACEOUT to log function exit. 
* DLTRACE can be used to log entries to some important parts of the code.
* Example:
* DLTRACEIN((_L("Input parameter aParam=%d"),aParam));
* DLTRACE((_L("Branch taken")));
* DLTRACEOUT((_L("Output=%d"),result));
*
* DLINFO can be used to log generic info:
* DLINFO((_L("Parsing created an element with name %S"),&elementName));
*
* DLERROR can be used to log errors:
* DLERROR((_L("Error while processing data error=%d, message=%S"),errorCode,&errorMessage));
*
* DLWARNING is used to log warnings.
*
* The macros support both 16-bit and 8-bit descriptors.
* 8-bit format:
* DLERROR(("this is a 8-bit string: %S",&K8BitString));
*
* NOTE! It's important to choose the right variant depending
* on the %S parameter type you want to print.
*
* DLERRORDUMP and DLINFODUMP can be used to output hex data to the log file.
*
* DASSERT should be used for asserts.
*/


#ifndef CATALOGSDEBUG_H
#define CATALOGSDEBUG_H

#include <e32std.h>
#include <flogger.h>
#include <e32msgqueue.h>

#ifdef CATALOGS_BUILD_CONFIG_DEBUG
// This could be overridden by each source file:
//#undef CATALOGS_FILEID
#define CATALOGS_FILEID __FILE__
#define CATALOGS_FUNCNAME __PRETTY_FUNCTION__

// panic with <filename>, <lineno>
// Filename is formatted so that extension (.cpp) is dropped, and 16
// rightmost characters are used. This maximises information about where problem occured.
// (the code is inside if statements is in one line because otherwise doesn't compile on codewarrior)
#define DEBUG_PANIC { \
    const TInt KPanicSize = 16; \
    TPtrC8 fname((const TUint8 *)CATALOGS_FILEID); \
    TBuf<KPanicSize> panic; \
    TInt dotPos = fname.LocateReverse('.'); \
    if ( dotPos == KErrNotFound ) { panic.Copy(fname.Right(KPanicSize)); } \
    else \
        { \
        TInt start = dotPos - KPanicSize; \
        /* we have to clip from start, panicsize=4 -> "c:\something\test.cpp" -> "test" */ \
        if ( start >= 0 ) { panic.Copy( fname.Mid( start, KPanicSize ) ); } \
        else \
            { /* no clipping from the start needed */ \
            panic.Copy( fname.Left( KPanicSize ) ); \
            } \
        }  \
    User::Panic( panic, __LINE__ ); }

#define DLINIT { \
    TRAPD( err, RCatalogsDebugHeap::InstallL() ); \
    if( err != KErrNone ) DEBUG_PANIC; }
#define DLUNINIT RCatalogsDebugHeap::Uninstall();
#else
#define DLINIT
#define DLUNINIT
#endif  // CATALOGS_BUILD_CONFIG_DEBUG

// For now, enable all debug utilities disabled by default
//#define CATALOGS_LOG_TRACE
//#define CATALOGS_LOG_DEBUG
//#define CATALOGS_DEBUG_ASSERTS

#ifdef CATALOGS_BUILD_CONFIG_HEAP_CHECKER
#define CATALOGS_DEBUG_CHECKS
#endif  // CATALOGS_BUILD_CONFIG_HEAP_CHECKER

// Define to enable debug logger GUI
//#define CATALOGS_EXT_LOGGER



#if defined (CATALOGS_BUILD_CONFIG_DEBUG) && defined (CATALOGS_LOG_TRACE)
#   define DLTRACE( x )     { TCatalogsDebug debug( TCatalogsDebug::ETrace,    CATALOGS_FUNCNAME, __LINE__, CATALOGS_FILEID ); debug.Print x; }
#   define DLTRACEIN( x )   TCatalogsLocalExitTrace __traceIn( CATALOGS_FUNCNAME, __LINE__, CATALOGS_FILEID ); { TCatalogsDebug debug( TCatalogsDebug::ETraceIn, CATALOGS_FUNCNAME, __LINE__, CATALOGS_FILEID ); debug.Print x; }
#   define DLTRACEOUT( x )  { TCatalogsDebug debug( TCatalogsDebug::ETraceOut, CATALOGS_FUNCNAME, __LINE__, CATALOGS_FILEID, User::FastCounter()-__traceIn.iEntryTime ); debug.Print x; __traceIn.Disable(); }
#   undef TRAP_INSTRUMENTATION_LEAVE
#   define TRAP_INSTRUMENTATION_LEAVE( x ) { TCatalogsDebug debug( TCatalogsDebug::ETrace, CATALOGS_FUNCNAME, __LINE__, CATALOGS_FILEID ); debug.Print(_L("Leave code=%d"), x); }
#else
#   define DLTRACE( x )
#   define DLTRACEIN( x )
#   define DLTRACEOUT( x )
#endif  // CATALOGS_BUILD_CONFIG_DEBUG && CATALOGS_LOG_TRACE

#if defined (CATALOGS_BUILD_CONFIG_DEBUG) && defined (CATALOGS_LOG_DEBUG)
#   define DLERROR( x )     { TCatalogsDebug debug( TCatalogsDebug::EError, CATALOGS_FUNCNAME, __LINE__, CATALOGS_FILEID ); debug.Print x; }
#   define DLWARNING( x )   { (void)__traceIn; TCatalogsDebug debug( TCatalogsDebug::EWarning, CATALOGS_FUNCNAME, __LINE__, CATALOGS_FILEID ); debug.Print x; }
#   define DLINFO( x )      { (void)__traceIn; TCatalogsDebug debug( TCatalogsDebug::EInfo,  CATALOGS_FUNCNAME, __LINE__, CATALOGS_FILEID ); debug.Print x; }
#   define DLERRORDUMP( ptr, size, clip ) { (void)__traceIn; TCatalogsDebug debug( TCatalogsDebug::EError, CATALOGS_FUNCNAME, __LINE__, CATALOGS_FILEID ); debug.DumpData( ptr, size, clip ); }
#   define DLINFODUMP( ptr, size, clip )  { (void)__traceIn; TCatalogsDebug debug( TCatalogsDebug::EInfo,  CATALOGS_FUNCNAME, __LINE__, CATALOGS_FILEID ); debug.DumpData( ptr, size, clip ); }
#else
#   define DLERROR( x )
#   define DLWARNING( x )
#   define DLINFO( x )
#   define DLERRORDUMP( ptr, size, clip )
#   define DLINFODUMP( ptr, size, clip )
#endif  // CATALOGS_BUILD_CONFIG_DEBUG && CATALOGS_LOG_DEBUG

#if defined (CATALOGS_BUILD_CONFIG_DEBUG) && defined (CATALOGS_DEBUG_ASSERTS)
#   define DASSERT( x ) if(!(x)) { DLERROR(("ASSERTION FAILED")); DEBUG_PANIC; }
#else
#   define DASSERT( x )
#endif

#if defined (CATALOGS_BUILD_CONFIG_DEBUG) && defined (CATALOGS_DEBUG_CHECKS)
  #define DCHECK_CSTACK TCatalogsLocalCleanupStackWatcher __cleanupStackWatcher;
  #define DCHECK_HEAP TCatalogsLocalDebugHeapActivator __debugHeapActivator;
  #define DCHECK \
    TCatalogsLocalCleanupStackWatcher __cleanupStackWatcher; \
    TCatalogsLocalDebugHeapActivator __debugHeapActivator;
#else
  #define DCHECK_CSTACK
  #define DCHECK_HEAP
  #define DCHECK
#endif // CATALOGS_BUILD_CONFIG_DEBUG && CATALOGS_DEBUG_CHECKS


#if defined (CATALOGS_BUILD_CONFIG_DEBUG) && defined (CATALOGS_PROFILING)
#   if defined(COMPONENT_CATALOGSSERVEREXE)
#       define DPROFILING_BEGIN( x ) TTime x; x.HomeTime(); static TInt64 total = 0
#       define DPROFILING_END( x ) { TTime now2; \
                    now2.HomeTime(); \
                    TInt64 time1 = now2.MicroSecondsFrom( x ).Int64();\
                    total = total + time1; \
                    TCatalogsDebug debug( TCatalogsDebug::ETrace, CATALOGS_FUNCNAME, __LINE__, CATALOGS_FILEID ); debug.Print( "%Ld ms, total: %Ld ms", (time1 / 1000 ), (total / 1000) ); }
#   else // Non-server code
#       define DPROFILING_BEGIN( x ) TTime x; x.HomeTime()
#       define DPROFILING_END( x ) { TTime now2; \
                    now2.HomeTime(); \
                    TInt64 time1 = now2.MicroSecondsFrom( x ).Int64();\
                    TCatalogsDebug debug( TCatalogsDebug::ETrace, CATALOGS_FUNCNAME, __LINE__, CATALOGS_FILEID ); debug.Print( "%Ld ms", (time1 / 1000 ) ); }
#   endif
#else
#   define DPROFILING_BEGIN( x )
#   define DPROFILING_END( x )
#endif


// Helpers for reference count checking
#if defined (CATALOGS_BUILD_CONFIG_DEBUG)

    #define DREFCOUNT_IS( objectPtr, value ) { if ( objectPtr ) { (objectPtr)->AddRef(); TInt count = (objectPtr)->Release(); DASSERT( count == value ); } }
    
    #define DREFCOUNT_EVAL( objectPtr, expression ) { if ( objectPtr ) { (objectPtr)->AddRef(); TInt count = (objectPtr)->Release(); DASSERT( count expression ); } }            

#else

    #define DREFCOUNT_IS( objectPtr, value )
    #define DREFCOUNT_EVAL( objectPtr, expression )

#endif


/**
 * Function-local exit tracer. When instantiated as a local variable at start of a scope (e.g. function)
 * makes a debug print when the scope is left (either normally or through a leave)
 *
 * @note Cannot be used in scopes that leave item(s) on cleanup stack, such as an LC function.
 * @note Can be used in nested scopes.
 */
class TCatalogsLocalExitTrace
    {
public:
    IMPORT_C TCatalogsLocalExitTrace( const char* aFunctionName, TInt aLine, const char* aFileId );
    IMPORT_C ~TCatalogsLocalExitTrace();
    IMPORT_C void Disable();

    const char* iFunctionName;
    TInt iLine;
    const char* iFileId;   
    TBool iDisabled;
    TUint32 iEntryTime;
    };

/**
 * Function-local cleanup stack watcher. When instantiated as a local variable at start of a scope (e.g. function),
 * tracks cleanupstack balance; will cause E32USER-CBase 90 panic if cleanupstack in the beginning and end of the scope
 * doesn't match (one or more items have been left on cleanup stack or popped out).
 *
 * @note Cannot be used in scopes that are supposed to leave item(s) on cleanup stack, such as an LC function.
 * @note Can be used in nested scopes.
 */
class TCatalogsLocalCleanupStackWatcher
    {
public:
    IMPORT_C TCatalogsLocalCleanupStackWatcher();
    IMPORT_C ~TCatalogsLocalCleanupStackWatcher();
    TBool iPopped;
    };

/**
 * Function-local debug heap activator. When instantiated as a local variable at start of a scope (e.g. function)
 * makes sure debug heap is active only within the scope.
 *
 * @note Cannot be used in scopes that leave item(s) on cleanup stack, such as an LC function.
 * @note Can be used in nested scopes.
 */
class TCatalogsLocalDebugHeapActivator
    {
public:
    IMPORT_C TCatalogsLocalDebugHeapActivator();
    IMPORT_C ~TCatalogsLocalDebugHeapActivator();
    TBool iPopped;
    TBool iActive;
    };


// Big enough formatting buffer. If the format parameters exceeds this, a panic occurs!
const TInt KCatalogsDebugBufferSize = 1024*80;

/**
* Debug utility class. 
*/
class TCatalogsDebug
    {
public:
    /**
    * Debug log event types
    */
    enum TType
        {
        ETrace,
        ETraceIn,
        ETraceOut,
        EError,
        EWarning,
        EInfo,
        ETraceLeave
        };
    
    enum
        {
        EOutputRDebug = 1,
        EOutputFileLogger = 2,
        EOutputExtLogger = 4
        };
        
    enum
        {
        EBuildOutput = 0
#ifdef CATALOGS_BUILD_CONFIG_DEBUG_RDEBUG
        | EOutputRDebug
#endif
#ifdef CATALOGS_BUILD_CONFIG_DEBUG_FILE_LOGGER
        | EOutputFileLogger
#endif
#ifdef CATALOGS_BUILD_CONFIG_DEBUG_EXT_LOGGER
        | EOutputExtLogger
#endif
        };
        
    /**
    * Constructor, sets generic information for the print.
    * @param aType
    * @param aFunction Function name
    * @param aLine Source code line
    * @param aFileId File identification
    */
    IMPORT_C TCatalogsDebug( 
        TType aType, 
        const char* aFunction, 
        TInt aLine, 
        const char* aFileId,
        TUint aDeltaTime = 0,
        TInt aOutput = EBuildOutput
        );
    
    /**
    * Print formatted data to the log, 16-bit descriptors.
    * @param aFmt Format string
    */
    IMPORT_C void Print( TRefByValue<const TDesC16> aFmt, ... );

    /**
    * Print formatted data to the log, 8-bit descriptors.
    * @param aFmt Format string
    */
    IMPORT_C void Print( const char* aFmt, ... );

    /**
    * Print hex data to the log.
    * @param aData Data pointer
    * @param aSize 
    * @param aClipToSize 
    */
    IMPORT_C void DumpData( const TAny* aData, TInt aSize, TInt aClipToSize );

private:
    void PrintGeneral( TPtr8 aPrintBuf );
    void FileWrite( const TDesC8& aPrintBuf );

private:

    TType       iType;
    const char* iFunction;
    TInt        iLine;
    const char* iFileId;
    TUint       iDeltaTime;
    TInt        iOutput;
    };

/**
 * Debug heap implementation. Holds "global" data for debug printing, and has
 * utilities for finding memory leaks.
 */
class RCatalogsDebugHeap : public RHeap
    {
public:

    IMPORT_C static void InstallL( TBool aEnabled = 
#ifdef CATALOGS_BUILD_CONFIG_HEAP_CHECKER
        ETrue );
#else
        EFalse );
#endif    
    IMPORT_C static void Uninstall();
    IMPORT_C static TBool Activate( TBool aActive );

    void ChunkOutput( const TDesC8& aBuffer, TUint aDeltaTime );
    TBool IsEnabled( TCatalogsDebug::TType aPrintType );

public: // from RHeap

    TAny* Alloc( TInt aSize );
    void Free( TAny* aPtr );
    TAny* ReAlloc(TAny* aPtr, TInt aSize, TInt aMode=0);
    TInt AllocLen(const TAny* aCell) const;
    TInt Compress();
    void Reset();
    TInt AllocSize(TInt& aTotalAllocSize) const;
    TInt Available(TInt& aBiggestBlock) const;
    TInt DebugFunction(TInt aFunc, TAny* a1=NULL, TAny* a2=NULL);

    TAny* operator new( TUint aSize );
    void operator delete( TAny* aPtr );

protected: // from RHeap
    TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);

    RCatalogsDebugHeap( RHeap& aBaseHeap, TBool aEnabled );
    ~RCatalogsDebugHeap();

private:

    RHeap&  iBaseHeap;
    TInt    iAllocCounter; // running counter for allocs
    
    struct TAllocInfo
        {
        TAny*   iAllocPtr;
        TInt    iAllocSize;
        TInt    iAllocNum;
        };

    RArray< TAllocInfo > iAllocInfo;
    TBool               iEnabled;
    TBool               iActive;

    // Debug chunk logging
    void InitExtLogger();

    TInt    iChunkIndex;    // current chunk index
    RChunk  iChunk[2];
    RSemaphore iChunkWriteSemaphore;
    RMutex  iMutex;
    RMsgQueue<TInt> iMsgQueue;
    
    TUint   iFastCounterFrequency;
    };

#endif // CATALOGSDEBUG_H
