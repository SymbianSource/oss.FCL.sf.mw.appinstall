/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file defines trace and log macros for SWI Daemon.
*
*/


#ifndef SWINSTDEBUG_H
#define SWINSTDEBUG_H

//-----------------------------------------------------------------------------
#ifdef _DEBUG // Trace if debug
//-----------------------------------------------------------------------------

// This will enable trace printing to Musti.
// Comment off if log file is needed.
#define _PRINT_TO_TRACE

// This will enable printing to log file c:\logs\SWIDaemon\SWIDaemon.txt
// Comment off is trace is needed.
//#define _PRINT_TO_LOG

//-----------------------------
#ifdef _PRINT_TO_TRACE
//-----------------------------
#include <e32debug.h>

const TInt KMaxDebugSize = 256;
const TInt KMaxPrefix = 20;
_LIT( KTracePrefixDaemon, "[SWIDaemon]: " );

inline void PrintDebugData( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf<KMaxDebugSize> buf;
    buf.Copy( KTracePrefixDaemon );
    buf.AppendFormatList( aFmt, list );
    RDebug::Print( buf );
    }

#define FLOG(a)          { PrintDebugData(a); }
#define FLOG_1(a,b)      { PrintDebugData(a,b); }
#define FLOG_2(a,b,c)    { PrintDebugData(a,b,c); }
#define FLOG_3(a,b,c,d)  { PrintDebugData(a,b,c,d); }

//#define FLOG(a)           { RDebug::Print(a); }
//#define FLOG_1(a,b)       { RDebug::Print(a,b); }
//#define FLOG_2(a,b,c)     { RDebug::Print(a,b,c); }
//#define FLOG_3(a,b,c,d)   { RDebug::Print(a,b,c,d); }

//-----------------------------
# endif // _PRINT_TO_TRACE
//-----------------------------

//-----------------------------
#ifdef _PRINT_TO_LOG
//-----------------------------
#include <e32std.h>

#define FLOG(a)            { FPrint(a); }
#define FLOG_1(a,b)        { FPrint(a,b); }
#define FLOG_2(a,b,c)      { FPrint(a,b,c); }
#define FLOG_3(a,b,c,d)    { FPrint(a,b,c,d); }

#include <flogger.h>
// Log file in c:\logs\SWIDaemon\
_LIT(KLogFile,"SWIDeamon.txt");
// Log dir
_LIT(KLogDir,"SWIDaemon");  

inline void FPrint( const TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    RFileLogger::WriteFormat( KLogDir, 
                              KLogFile, 
                              EFileLoggingModeAppend, 
                              aFmt, 
                              list );
    }
//-----------------------------
# endif // _PRINT_TO_LOG
//-----------------------------

//-----------------------------------------------------------------------------
#else // No trace or logs
//-----------------------------------------------------------------------------

#define FLOG(a)
#define FLOG_1(a,b)   
#define FLOG_2(a,b,c) 
#define FLOG_3(a,b,c,d)

#endif // _DEBUG

#endif // SWINSTDEBUG_H

// EOF
