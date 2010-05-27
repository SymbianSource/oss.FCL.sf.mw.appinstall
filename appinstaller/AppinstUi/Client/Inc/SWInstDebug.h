/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file defines logging macros for SW Installer UIs.
*
*/


#ifndef SWINSTDEBUG_H
#define SWINSTDEBUG_H


#ifdef _DEBUG

#include <e32std.h>

#define FLOG(a)   { FPrint(a); }
#define FLOG_1(a,b)   { FPrint(a,b); }
#define FLOG_2(a,b,c)   { FPrint(a,b,c); }
#define FLOG_3(a,b,c,d)   { FPrint(a,b,c,d); }

#include <flogger.h>

_LIT(KLogFile,"SWInst.txt");
_LIT(KLogDir,"SWInst");

inline void FPrint( const TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    RFileLogger::WriteFormat( KLogDir, KLogFile, EFileLoggingModeAppend, aFmt, list );
    }

// ===========================================================================
#else // // No loggings
// ===========================================================================
#define FLOG(a)
#define FLOG_1(a,b)   
#define FLOG_2(a,b,c) 
#define FLOG_3(a,b,c,d)

#endif // _DEBUG

#endif // SWINSTDEBUG_H

// End of File
