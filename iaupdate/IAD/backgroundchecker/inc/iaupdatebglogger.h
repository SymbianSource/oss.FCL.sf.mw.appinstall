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
* Description:    
*
*/



#ifndef IAUPDATEBGLOGGER_H
#define IAUPDATEBGLOGGER_H


#include <e32std.h>
#include <f32file.h>
#include <flogger.h>

#ifdef _DEBUG

_LIT( KLogFile, "bgchecker.log" );
_LIT( KLogDirFullName, "c:\\logs\\" );
_LIT( KLogDir, "iaupdate" );

#define FLOG( a )   { FPrint( _L(a) ); }
#define FLOG_NUM( a, num ) { FPrint(_L(a), num ); }
#define FTRACE( a ) { a; }
#define FLOGSTR(a)  { FPrint(a); }
#define FTIME(a)    { FTime(a); }


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

inline void FTime( const TTime& aTime )
    {
    HBufC* date = HBufC::NewL(15);
    TPtr ptrdate = date->Des();
    _LIT(KDate,"%D%M%Y%/0%1%/1%2%/2%3%/3");
    aTime.FormatL( ptrdate, KDate );
    
    FPrint( *date );
    
    HBufC* time = HBufC::NewL(15);
    TPtr ptrtime = time->Des();
    _LIT(KTime,"%J%:1%T%:2%S%:3");
    aTime.FormatL( ptrtime, KTime );
    
    FPrint ( *time );
    
    delete date;
    delete time;
    }


#else

#define FLOG( a )   
#define FLOG_NUM( a, num ) 
#define FTRACE( a ) 
#define FLOGSTR(a)
#define FTIME(a) 

#endif //_DEBUG

#endif //IAUPDATEBGLOGGER_H
// End of File
