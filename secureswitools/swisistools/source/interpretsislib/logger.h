/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file LOGGER.H
 @publishedPartner
 @released
*/
#ifndef	LOGGER_H
#define	LOGGER_H

#include <iosfwd>

enum WarnLevel
	{
	WARN_OFF,
	WARN_ERROR,
	WARN_WARN,
	WARN_INFO
	};


struct LoggerImpl;

/**
 * Logger class to log messages from the system.
 */
class Logger
{
public:
	/**
	 * Function will set the minimum warning level.
	 * Messages having warning level greated than this 
	 * won't be printed. Check WarnLevel for details.
	 */
	static bool SetLevel(WarnLevel aLev);
	/**
	 * Set the stream to which the mssage should be written.
	 */
	static bool SetStream(std::wostream& aOs);
	/**
	 * Get a reference to the stream.
	 */
	static std::wostream& Stream(); 
	/**
	 * Checks whether a warning level is allowed to print or not.
	 * @param aLev level at which the message should be printed.
	 */
	static bool ShouldPrint(WarnLevel aLev); 
	
private:
	static LoggerImpl& Logger::Impl();

	static LoggerImpl* iLogger;
};

/**
 * Helper macro to print infom messages
 */
#define LINFO(x)  if (Logger::ShouldPrint(WARN_INFO)) { Logger::Stream() << L"INFO: " << x << std::endl; }
/**
 * Helper macro to print warning messages
 */
#define LWARN(x)  if (Logger::ShouldPrint(WARN_WARN)) { Logger::Stream() << L"WARN: " << x << std::endl; }
/**
 * Helper macro to print error messages
 */
#define LERROR(x) if (Logger::ShouldPrint(WARN_ERROR)){ Logger::Stream() << L"ERR : " << x << std::endl; }

#endif	/* LOGGER_H */
