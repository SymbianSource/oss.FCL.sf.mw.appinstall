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
* xmllogger.cpp
* CLogger - Used to log details to the specified log files. 
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#include "logs.h"
#include "exception.h"
#include <fstream>
#include <iostream>

std::ostream* LogDetails::LogStream = NULL;
int LogDetails::LogLevel = 0;

DllExport CLogger::CLogger(std::string& aLogFileName, CLogger::TLogLevel aLogLevel)
	{
	if(LogDetails::LogStream)
		{
		throw CException("Log file already initialized");
		}
	
	
	LogDetails::LogStream = new std::ofstream(aLogFileName.c_str(), std::ios::out | std::ios::app);
		
	LogDetails::LogLevel = aLogLevel;
	}

DllExport CLogger::~CLogger()
	{
	delete LogDetails::LogStream;
	}

/**
	Logs the details based on the logging parameters.

	@param	aMessage: Message to be logged in.
	@param	aXmlParams: Logging parameters based on which message is to be logged.
 */
DllExport void CLogger::Log(const char* aMessage)
	{
	if(LogDetails::LogStream == NULL)
		{
		std::string streamUnintialized = "Stream has not been initialized";
		throw CException(streamUnintialized, ExceptionCodes::EUnintializedCode);
		}
	
	*LogDetails::LogStream << aMessage << std::endl;
	}

/**
	Logs the details based on the logging parameters.

	@param	aMessage: Message to be logged in.
	@param	aXmlParams: Logging parameters based on which message is to be logged.
 */
DllExport void CLogger::Log(std::string& aMessage)
	{
	Log(aMessage.c_str());
	}


DllExport std::ostream& CLogger::GetStream()
	{
	return *LogDetails::LogStream;
	}