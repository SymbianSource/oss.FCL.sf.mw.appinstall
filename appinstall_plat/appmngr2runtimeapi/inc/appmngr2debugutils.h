/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines trace macros
*
*/


#ifndef APPMNGR2DEBUGUTILS_H
#define APPMNGR2DEBUGUTILS_H

#include "appmngr2debugconfig.h"        // Configuration macros
#ifdef PRINT_FLOG_TO_RDEBUG
#include <e32debug.h>                   // RDebug
#endif
#ifdef PRINT_FLOG_TO_UTRACE
#include <e32utrace.h>                  // TUTrace
#endif
#ifdef PRINT_FLOG_TO_FLOGGER
#include <flogger.h>                    // RFileLogger
#endif
#ifdef FLOG_PERFORMANCE_TRACES
#include <e32std.h>                     // TTime, TTimeIntervalMicroSeconds
#endif

inline void DebugPrint( TRefByValue<const TDesC> aFmt, ... );

#ifdef FLOG_TRACES_ENABLED

#ifdef FLOG_PERFORMANCE_TRACES

// Static macros
#define FLOG_PERF_STATIC_BEGIN(a)   TTime perf_begin_ ##a ; perf_begin_ ##a.UniversalTime();
#define FLOG_PERF_STATIC_END(a)     TTime perf_end_ ##a; perf_end_ ##a.UniversalTime(); \
    TTimeIntervalMicroSeconds perf_diff_ ##a; \
    perf_diff_ ##a = perf_end_ ##a.MicroSecondsFrom( perf_begin_ ##a ); \
    _LIT( perf_name_ ##a, #a ); \
    DebugPrint( _L("PERF %S: %Ld ms" ), &perf_name_ ##a, perf_diff_ ##a.Int64() / TInt64( 1000 ) );

// Dynamic macros
#define FLOG_PERF_DEFINE(a)         TTime perf_begin_ ##a; TTime perf_end_ ##a; \
    TTimeIntervalMicroSeconds perf_diff_ ##a;
#define FLOG_PERF_START(a)          perf_begin_ ##a.UniversalTime();
#define FLOG_PERF_STOP(a)           perf_end_ ##a.UniversalTime();
#define FLOG_PERF_PRINT(a)          _LIT( perf_name_ ##a, #a ); \
    perf_diff_ ##a = perf_end_ ##a.MicroSecondsFrom( perf_begin_ ##a ); \
    DebugPrint( _L("PERF %S: %Ld ms" ), &perf_name_ ##a, perf_diff_ ##a.Int64() / TInt64( 1000 ) );

#else

#define FLOG_PERF_STATIC_BEGIN(a)
#define FLOG_PERF_STATIC_END(a)
#define FLOG_PERF_DEFINE(a)
#define FLOG_PERF_START(a)
#define FLOG_PERF_STOP(a)
#define FLOG_PERF_PRINT(a)

#endif  // FLOG_PERFORMANCE_TRACES


#if( defined( FLOG_DEBUG_TRACES ) )
#define FLOG( a, ARGS... )           DebugPrint( _L( a ), ##ARGS );
#else
#define FLOG( a, ARGS... )
#endif  // FLOG_DEBUG_TRACES


#else
#define FLOG_PERF_STATIC_BEGIN(a)
#define FLOG_PERF_STATIC_END(a)
#define FLOG_PERF_DEFINE(a)
#define FLOG_PERF_START(a)
#define FLOG_PERF_STOP(a)
#define FLOG_PERF_PRINT(a)
#define FLOG( a, ARGS... )
#endif  // FLOG_TRACES_ENABLED


const TInt KMaxPrintSize = 256;
_LIT( KAppMngr2Tag, "AppMngr2: " );

#if( defined( PRINT_FLOG_TO_UTRACE ) )

inline void DebugPrint( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf<KMaxPrintSize> buf;
    buf.Copy( KAppMngr2Tag );
    buf.AppendFormatList( aFmt, list );
    TUTrace::PrintfPrimary( KDebug, EFalse, EFalse, buf );
    }

#elif( defined( PRINT_FLOG_TO_RDEBUG ) ) 

inline void DebugPrint( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf<KMaxPrintSize> buf;
    buf.Copy( KAppMngr2Tag );
    buf.AppendFormatList( aFmt, list );
    RDebug::Print( buf );
    }

#elif( defined( PRINT_FLOG_TO_FLOGGER ) )

_LIT( KLogDir, "AppMngr2" );
_LIT( KLogFile, "AppMngr2.txt" );

inline void DebugPrint( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    RFileLogger::WriteFormat( KLogDir, KLogFile, EFileLoggingModeAppend, aFmt, list );
    }

#else

// No implementation needed, DebugPrint() not used

#endif

#endif  // APPMNGR2DEBUGUTILS_H

