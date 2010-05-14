/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains debug macros
*
*/



#ifndef IAUPDATEDEBUG_H
#define IAUPDATEDEBUG_H


#ifdef _DEBUG

#define IAUPDATE_FILELOG   //To file log 

// output is c:\logs\iaupdate\iaupdate.txt
// c:\logs\iaupdate folder must be created (manually) first
#ifdef IAUPDATE_FILELOG  //file logs
#include <flogger.h>

_LIT(KLogFilename,"iaupdate.txt");
_LIT(KLogDirFullName,"c:\\logs\\");
_LIT(KLogDir,"iaupdate");

// Declare the FPrint function
inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KLogDir, KLogFilename, EFileLoggingModeAppend, aFmt, list);
    }

#define IAUPDATE_TRACE(aText)              { FPrint(_L(aText)); }
#define IAUPDATE_TRACE_1(aText,p1)         { FPrint(_L(aText),p1); }
#define IAUPDATE_TRACE_2(aText,p1,p2)      { FPrint(_L(aText),p1,p2); }
#define IAUPDATE_TRACE_3(aText,p1,p2,p3)   { FPrint(_L(aText),p1,p2,p3); }

#else //RDebug logs
#include <e32debug.h>
// output is RDebug port (epocwind.out in emulator)
#define IAUPDATE_TRACE(aText)              { RDebug::Print(_L(aText)); }
#define IAUPDATE_TRACE_1(aText,p1)         { RDebug::Print(_L(aText),p1); }
#define IAUPDATE_TRACE_2(aText,p1,p2)      { RDebug::Print(_L(aText),p1,p2); }
#define IAUPDATE_TRACE_3(aText,p1,p2,p3)   { RDebug::Print(_L(aText),p1,p2,p3); }
#endif //IAUPDATE_FILELOG

#else // No loggings

#define IAUPDATE_TRACE(aText)
#define IAUPDATE_TRACE_1(aText,p1)   
#define IAUPDATE_TRACE_2(aText,p1,p2) 
#define IAUPDATE_TRACE_3(aText,p1,p2,p3)

#endif // _DEBUG

#endif // IAUPDATEDEBUG_H

// End of File
