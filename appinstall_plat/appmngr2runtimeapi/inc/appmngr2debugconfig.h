/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef APPMNGR2DEBUGCONFIG_H
#define APPMNGR2DEBUGCONFIG_H

#ifdef _DEBUG


/* ---------------------------------------------------------------------------
 * Enable/disable traces
 * ---------------------------------------------------------------------------
 * Uncomment one of the following macros to enable tracing:
 */
//#define PRINT_FLOG_TO_RDEBUG
//#define PRINT_FLOG_TO_UTRACE
//#define PRINT_FLOG_TO_FLOGGER
/*
 * Select the trace type by uncommenting FLOG_DEBUG_TRACES or
 * FLOG_PERFORMANCE_TRACES (or both) below.
 * 
 * Note that also SYMBIAN_TRACE_ENABLE must be defined in order
 * to use PRINT_FLOG_TO_UTRACE.  
 */ 


/* ---------------------------------------------------------------------------
 * Debug traces
 * ---------------------------------------------------------------------------
 * Uncomment the following line to enable debug traces:
 */
//#define FLOG_DEBUG_TRACES
/*
 * Debug traces are generated using FLOG macro. FLOG takes a format
 * string and variable number of arguments, like TDes::Format(). Except
 * that format string "x" is replaced with string literal _L("x"), so
 * string literal _L() should not be used when calling FLOG. 
 * 
 * Examples:
 *    FLOG( "CAppMngr2AppUi::ConstructL" );
 *    FLOG( "CAppMngr2AppUi::HandleCommandL( %d )", aCommand );
 *    FLOG( "Scanning %d: %S", iIndex, &iDirName ) );
 */


/* ---------------------------------------------------------------------------
 * Performance traces
 * ---------------------------------------------------------------------------
 * Uncomment the following line to enable performance traces:
 */
//#define FLOG_PERFORMANCE_TRACES
/*
 * Performance traces measure and print times that different
 * code fragments take (in milliseconds). Please note that
 * printing traces is usually slow and this may affect to the 
 * results if debug or other performance traces are printed
 * out from the timed functions.
 * 
 * There are two sets of performance tracing macros: static and
 * dynamic. Parameter is used to match corresponding macros and
 * it is also printed in trace output. Macros use temporary TTime
 * variables, hence macros (using the same parameter label) must
 * be in the same scope.
 * 
 * The macros are:
 * FLOG_PERF_STATIC_BEGIN - records start time
 * FLOG_PERF_STATIC_END - records end time and prints results
 * FLOG_PERF_DEFINE - defines temporary variables
 * FLOG_PERF_START - records start time (fast)
 * FLOG_PERF_STOP - records end time (fast)
 * FLOG_PERF_PRINT - prints out the results (slow)
 * 
 * Examples:
 *    FLOG_PERF_STATIC_BEGIN( LoadPluginsL )
 *    LoadPluginsL();
 *    FLOG_PERF_STATIC_END( LoadPluginsL )
 *    FLOG_PERF_STATIC_BEGIN( CreateScannerL )
 *    CreateScannerL();
 *    FLOG_PERF_STATIC_END( CreateScannerL )
 *    FLOG_PERF_STATIC_BEGIN( FetchDataTypesL )
 *    FetchDataTypesL();
 *    FLOG_PERF_STATIC_END( FetchDataTypesL )
 * 
 * This generates traces like:
 *    AppMngr2: PERF LoadPluginsL: 157 ms
 *    AppMngr2: PERF CreateScannerL: 8 ms
 *    AppMngr2: PERF FetchDataTypesL: 2 ms
 * 
 * Dynamic macros do the same thing as static macros, but possibly
 * slow parts (that may affect to results) are put into separate 
 * FLOG_PERF_DEFINE and FLOG_PERF_PRINT macros. Printing the trace
 * is usually slow, which may affect to results (e.g. if static
 * macros are nested). Dynamic macros can be used to measure run
 * times of active objects too.
 * 
 * Semicolons are not needed after FLOG macros. Semicolons can be
 * used, but they may issue "illegal empty declaration" warning
 * after FLOG_PERF_DEFINE() macros when traces are disabled.  
 */


// Sanity check: each FLOG macro produces compilation error if
// PRINT_FLOG_TO_UTRACE is defined but SYMBIAN_TRACE_ENABLE is not.
#if( defined( PRINT_FLOG_TO_UTRACE ) && !defined( SYMBIAN_TRACE_ENABLE ) )
#error "SYMBIAN_TRACE_ENABLE not defined, PRINT_FLOG_TO_UTRACE needs it." 
#endif

// Sanity check: traces can be printed to only one target (RDebug, UTrace, or FLogger)
#if( defined( PRINT_FLOG_TO_RDEBUG ) && defined( PRINT_FLOG_TO_UTRACE ) )
#error "Cannot define both PRINT_FLOG_TO_RDEBUG and PRINT_FLOG_TO_UTRACE macros."
#endif
#if( defined( PRINT_FLOG_TO_RDEBUG ) && defined( PRINT_FLOG_TO_FLOGGER ) )
#error "Cannot define both PRINT_FLOG_TO_RDEBUG and PRINT_FLOG_TO_FLOGGER macros."
#endif
#if( defined( PRINT_FLOG_TO_UTRACE ) && defined( PRINT_FLOG_TO_FLOGGER ) )
#error "Cannot define both PRINT_FLOG_TO_UTRACE and PRINT_FLOG_TO_FLOGGER macros."
#endif

// Sanity check: when traces are enabled, also trace type must be selected.
#if( defined( PRINT_FLOG_TO_RDEBUG ) || defined( PRINT_FLOG_TO_UTRACE ) || defined( PRINT_FLOG_TO_FLOGGER ) )
#define FLOG_TRACES_ENABLED
#endif
#if( defined( FLOG_TRACES_ENABLED ) )
#if( !defined( FLOG_DEBUG_TRACES ) && !defined( FLOG_PERFORMANCE_TRACES ) )
#error "Traces enabled but FLOG_DEBUG_TRACES or FLOG_PERFORMANCE_TRACES is not defined."
#endif
#endif

#endif  // _DEBUG

#endif  // APPMNGR2DEBUGCONFIG_H

