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
* Description:   This file defines logging macros for DM
*
*/




#ifndef IAUPDATEFWDEBUG_H
#define IAUPDATEFWDEBUG_H

//#ifdef _DEBUG
// ===========================================================================
//#ifdef __WINS__     // File logging for WINS
// ===========================================================================
#include <e32std.h>
#include <f32file.h>
#include <flogger.h>


#ifdef _DEBUG

_LIT( KLogFile, "fw.log" );
_LIT( KLogDirFullName, "c:\\logs\\" );
_LIT( KLogDir, "iaupdate" );

#define FLOG( a )   { FPrint( _L(a) ); }
#define FLOG_NUM( a, num ) { FPrint(_L(a), num ); }
#define FTRACE( a ) { a; }
#define FLOGSTR(a)  { FPrint(a); }

// ===========================================================================
// KNST #ifdef __WINS__     // File logging for WINS
// ===========================================================================
// Declare the FPrint function
//
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

#else

#define FLOG( a )   
#define FTRACE( a ) 
#define FLOG_NUM( a, num ) 
#define FLOGSTR(a)  

#endif //_DEBUG

/*
// ===========================================================================
#else               // RDebug logging for target HW
// ===========================================================================
#include <e32svr.h>

#define FLOG( a )   { RDebug::Print( _L(a) ); }
#define FTRACE( a ) { a; }

// Declare the FPrint function
//
inline void FPrint( const TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TInt tmpInt = VA_ARG( list, TInt );
    TInt tmpInt2 = VA_ARG( list, TInt );
    TInt tmpInt3 = VA_ARG( list, TInt );
    VA_END( list );
    RDebug::Print( aFmt, tmpInt, tmpInt2, tmpInt3 );
    }

//#endif //__WINS__


// ===========================================================================
#else // // No loggings --> Reduced binary size
// ===========================================================================
#define FLOG( a )
#define FTRACE( a )
*/

//#endif // _DEBUG

#endif      // IAUPDATEFWDEBUG_H
            
// End of File
