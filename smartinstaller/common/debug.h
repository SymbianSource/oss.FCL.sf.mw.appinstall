/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*     Debug definitions.
*
*
*/

#ifndef __SI_DEBUG_H__
#define __SI_DEBUG_H__

#include <e32debug.h>
#include <flogger.h>
#include <coeutils.h>

// Add:
// ConeUtils::EnsurePathExistsL( *buf ); \
// ..if log dir is required to be always created

#ifdef USE_LOGFILE
#define INIT_DEBUG_LOG( aLogFile, aLogDir, aLogName ) \
{ \
HBufC* buf = aLogFile.AllocLC(); \
TChar systemDrive; \
RFs::DriveToChar(RFs::GetSystemDrive(),systemDrive); \
buf->Des()[0] = systemDrive; \
CleanupStack::PopAndDestroy(buf); \
iLog.Connect(); \
iLog.CreateLog( aLogDir, aLogName, EFileLoggingModeOverwrite); \
}

#define CLOSE_DEBUG_LOG \
	iLog.CloseLog();    \
	iLog.Close();

#define INIT_OBJECT_DEBUG_LOG \
	RFileLogger iLog;
#else

#define INIT_DEBUG_LOG( a, b, c )
#define CLOSE_DEBUG_LOG
#define INIT_OBJECT_DEBUG_LOG

#endif

#if defined(DEBUG_ADM) || defined(_DEBUG)

#ifdef USE_LOGFILE

#define LOG( aMsg ) { _LIT(KMsg, aMsg); iLog.Write( KMsg ); RDebug::Print( KMsg ); }
#define LOG2( aMsg, aParam1 ) { _LIT(KMsg, aMsg); iLog.WriteFormat( KMsg, aParam1 ); RDebug::Print( KMsg, aParam1 ); }
#define LOG3( aMsg, aParam1, aParam2 ) { _LIT(KMsg, aMsg); iLog.WriteFormat( KMsg, aParam1, aParam2 ); RDebug::Print( KMsg, aParam1, aParam2 ); }
#define LOG4( aMsg, aParam1, aParam2, aParam3 ) { _LIT(KMsg, aMsg); iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3 ); }
#define LOG5( aMsg, aParam1, aParam2, aParam3, aParam4 ) { _LIT(KMsg, aMsg); iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4 ); }
#define LOG6( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ) { _LIT(KMsg, aMsg); iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ); }
#define LOG7( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ) { _LIT(KMsg, aMsg); iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ); }
#define LOG8( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ) { _LIT(KMsg, aMsg); iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ); }
#define LOG8_2( aMsg, aParam1 ) { _LIT8(KMsg, aMsg); iLog.WriteFormat( KMsg, aParam1 ); RDebug::Print(_L("RDebug _L8() at line %d"), __LINE__); }
#else

#define LOG( aMsg ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg ); }
#define LOG2( aMsg, aParam1 ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg, aParam1 ); }
#define LOG3( aMsg, aParam1, aParam2 ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg, aParam1, aParam2 ); }
#define LOG4( aMsg, aParam1, aParam2, aParam3 ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg, aParam1, aParam2, aParam3 ); }
#define LOG5( aMsg, aParam1, aParam2, aParam3, aParam4 ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4 ); }
#define LOG6( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ); }
#define LOG7( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ); }
#define LOG8( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ); }
#define LOG8_2( aMsg, aParam1 ) { _LIT8(KMsg, aMsg); TBuf8<128> t; t.AppendFormat(KMsg, aParam1); RDebug::RawPrint(t); }
#endif

#define RLOG( aMsg ) { RDebug::Print ( _L(##aMsg) ); }
#define RLOG2( aMsg, aParam1 ) { RDebug::Print ( _L(##aMsg), aParam1 ); }
#define RLOG3( aMsg, aParam1, aParam2 ) { RDebug::Print ( _L(##aMsg), aParam1, aParam2 ); }
#define RLOG4( aMsg, aParam1, aParam2, aParam3 ) { RDebug::Print ( _L(##aMsg), aParam1, aParam2, aParam3 ); }
#define RLOG5( aMsg, aParam1, aParam2, aParam3, aParam4 ) { RDebug::Print ( _L(##aMsg), aParam1, aParam2, aParam3, aParam4 ); }

#else

#define LOG( a )
#define LOG2( a, b )
#define LOG3( a, b, c )
#define LOG4( a, b, c, d )
#define LOG5( aMsg, aParam1, aParam2, aParam3, aParam4 )
#define LOG6( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5 )
#define LOG7( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 )
#define LOG8( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 )
#define LOG8_2( a, b )
#define RLOG( a )
#define RLOG2( a, b )
#define RLOG3( a, b, c )
#define RLOG4( a, b, c, d )
#define RLOG5( a, b, c, d, e )
#endif

// Forced Logs - Always printed.
#define FLOG( aMsg ) { _LIT(KMsg, aMsg); iLog.Write( KMsg ); RDebug::Print( KMsg ); }
#define FLOG2( aMsg, aParam1 ) { _LIT(KMsg, aMsg); iLog.WriteFormat( KMsg, aParam1 ); RDebug::Print( KMsg, aParam1 ); }
#define FLOG3( aMsg, aParam1, aParam2 ) { _LIT(KMsg, aMsg); iLog.WriteFormat( KMsg, aParam1, aParam2 ); RDebug::Print( KMsg, aParam1, aParam2 ); }

#endif
