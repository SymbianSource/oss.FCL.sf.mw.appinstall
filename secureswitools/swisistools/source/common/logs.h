/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* xmllogger.h
* CXmlLogger - Used to log details to the specified log files.
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef	LOGS_H
#define	LOGS_H

#include <ostream>
#include <iostream>

#include "exception.h"
#include <fstream>
#include "toolsconf.h"

namespace LogDetails
	{
	extern std::ostream* LogStream;
	extern int LogLevel;
	};



class CLogger
	{
	public:	
		/**
			Different types of logging levels which can be enabled.
		 */
		enum TLogLevel
			{
			ELog = 1,
			ELogWarning = ELog << 1,
			ELogError = ELogWarning << 1,
			ELogEnter = ELogError << 1,
			ELogExit = ELogEnter << 1,
			ELogAll = ELog | ELogWarning | ELogError | ELogEnter | ELogExit,
			};

	public:
		
		DllExport CLogger(std::string& aLogFileName, TLogLevel aLogLevel);

		DllExport ~CLogger();
		
		/**
			Logs the details based on the logging parameters.
		 */
		DllExport void Log(const char* aMessage);
		
		/**
			Logs the details based on the logging parameters.
		 */
		DllExport void Log(std::string& aMessage);
		
		/**
			returns the stream initialized to the log file.
		 */
		DllExport static std::ostream& GetStream();

		
	};

/**
	Logs information of file name, logging time and line number	based on logging parameters.
 */
#define LOGDETAILS()	CLogger::GetStream() << "\n<File>"; \
						CLogger::GetStream() << __FILE__; \
						CLogger::GetStream() << "\t<Line>"; \
						CLogger::GetStream() << __LINE__ << std::ends;

/**
	Logs message in the form of a warning.
 */
#define LOGWARN(msg)	if( CLogger::ELogWarning == CLogger::ELogWarning ) \
						{ \
						LOGDETAILS() \
						CLogger::GetStream() << "\t<Warning> "; \
						CLogger::GetStream() << msg; \
						CLogger::GetStream().flush(); \
						} 

/**
	Logs message in the form of an error.
 */
#define LOGERROR(msg)	if( CLogger::ELogError == CLogger::ELogError ) \
						{ \
						CLogger::GetStream() << "\n<Error> "; \
						CLogger::GetStream() << msg; \
						CLogger::GetStream().flush(); \
						}

/**
	Used for logging message while entering a method.
 */
#define LOGENTER(msg)	if( CLogger::ELogEnter == CLogger::ELogEnter ) \
						{ \
						LOGDETAILS() \
						CLogger::GetStream() << "\t<Enter> "; \
						CLogger::GetStream() << msg; \
						CLogger::GetStream().flush(); \
						}

/**
	Used for logging message while exiting a method.
 */
#define LOGEXIT(msg)	if( CLogger::ELogExit == CLogger::ELogExit ) \
						{ \
						LOGDETAILS() \
						CLogger::GetStream() << "\t<Exit> "; \
						CLogger::GetStream() << msg; \
						CLogger::GetStream().flush(); \
						}

/**
	Used for logging generic messages.
 */
#define LOGINFO(msg)	if( CLogger::ELog == CLogger::ELog ) \
						{ \
						LOGDETAILS() \
						CLogger::GetStream() << msg; \
						CLogger::GetStream().flush(); \
						}


#endif	/* LOGGER_H */
