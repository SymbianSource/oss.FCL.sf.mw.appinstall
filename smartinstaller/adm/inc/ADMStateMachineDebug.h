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
*     State machine debug macros
*
*
*/


#ifndef __ADMSTATEMACHINEDEBUG_H__
#define __ADMSTATEMACHINEDEBUG_H__

#ifdef DEBUG_ADM

// We need to redefine the logging macros for accessing the logger
//TPtrC8((TUint8*)__PRETTY_FUNCTION__)
#define LOG_FUNC { RDebug::Printf("+ %s", __PRETTY_FUNCTION__); }

#undef LOG
#undef LOG2
#undef LOG3
#undef LOG4
#undef LOG5
#undef LOG6
#undef LOG7
#undef LOG8
#undef LOG8_2

#ifdef USE_LOGFILE
#define LOG( aMsg ) { _LIT(KMsg, aMsg); iAppUi.iLog.Write( KMsg ); RDebug::Print( KMsg ); }
#define LOG2( aMsg, aParam1 ) { _LIT(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1 ); RDebug::Print( KMsg, aParam1 ); }
#define LOG3( aMsg, aParam1, aParam2 ) { _LIT(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1, aParam2 ); RDebug::Print( KMsg, aParam1, aParam2 ); }
#define LOG4( aMsg, aParam1, aParam2, aParam3 ) { _LIT(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3 ); }
#define LOG5( aMsg, aParam1, aParam2, aParam3, aParam4 ) { _LIT(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4 ); }
#define LOG6( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ) { _LIT(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ); }
#define LOG7( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ) { _LIT(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ); }
#define LOG8( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ) { _LIT(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ); }
#define LOG8_2( aMsg, aParam1 ) { _LIT8(KMsg, aMsg); iAppUi.iLog.WriteFormat( KMsg, aParam1 ); RDebug::Print(_L("RDebug _L8() at line %d"), __LINE__); }

#define SLOG( aMsg ) { _LIT(KMsg, aMsg); iStateMachine.Log().Write( KMsg ); RDebug::Print( KMsg ); }
#define SLOG2( aMsg, aParam1 ) { _LIT(KMsg, aMsg); iStateMachine.Log().WriteFormat( KMsg, aParam1 ); RDebug::Print( KMsg, aParam1 ); }
#define SLOG3( aMsg, aParam1, aParam2 ) { _LIT(KMsg, aMsg); iStateMachine.Log().WriteFormat( KMsg, aParam1, aParam2 ); RDebug::Print( KMsg, aParam1, aParam2 ); }
#define SLOG4( aMsg, aParam1, aParam2, aParam3 ) { _LIT(KMsg, aMsg); iStateMachine.Log().WriteFormat( KMsg, aParam1, aParam2, aParam3 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3 ); }
#define SLOG5( aMsg, aParam1, aParam2, aParam3, aParam4 ) { _LIT(KMsg, aMsg); iStateMachine.Log().WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4 ); }
#define SLOG6( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ) { _LIT(KMsg, aMsg); iStateMachine.Log().WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ); }
#define SLOG7( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ) { _LIT(KMsg, aMsg); iStateMachine.Log().WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ); }
#define SLOG8( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ) { _LIT(KMsg, aMsg); iStateMachine.Log().WriteFormat( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ); }
#define SLOG8_2( aMsg, aParam1 ) { _LIT8(KMsg, aMsg); iStateMachine.Log().WriteFormat( KMsg, aParam1 ); RDebug::Print(_L("RDebug _L8() at line %d"), __LINE__); }
#else
#define LOG( aMsg ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg ); }
#define LOG2( aMsg, aParam1 ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg, aParam1 ); }
#define LOG3( aMsg, aParam1, aParam2 ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg, aParam1, aParam2 ); }
#define LOG4( aMsg, aParam1, aParam2, aParam3 ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg, aParam1, aParam2, aParam3 ); }
#define LOG5( aMsg, aParam1, aParam2, aParam3, aParam4 ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4 ); }
#define LOG6( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ); }
#define LOG7( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ); }
#define LOG8( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ) { _LIT(KMsg, aMsg); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ); }
#define LOG8_2( aMsg, aParam1 ) { _LIT8(KMsg, aMsg); TBuf8<256> t; t.AppendFormat(KMsg, aParam1); RDebug::RawPrint(t); }
#endif

#else

#define LOG_FUNC
#define LOG( a )
#define LOG2( a, b )
#define LOG3( a, b, c )
#define LOG4( a, b, c, d )
#define LOG5( aMsg, aParam1, aParam2, aParam3, aParam4 )
#define LOG6( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5 )
#define LOG7( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 )
#define LOG8( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 )
#define LOG8_2( a, b )
#define SLOG( a )
#define SLOG2( a, b )
#define SLOG3( a, b, c )
#define SLOG4( a, b, c, d )
#define SLOG5( aMsg, aParam1, aParam2, aParam3, aParam4 )
#define SLOG6( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5 )
#define SLOG7( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 )
#define SLOG8( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 )
#define SLOG8_2( a, b )

#endif // DEBUG_ADM

#endif // __ADMSTATEMACHINEDEBUG_H__
