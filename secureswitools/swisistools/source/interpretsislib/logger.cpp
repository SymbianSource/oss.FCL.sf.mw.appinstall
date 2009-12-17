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


#include "logger.h"
#include <iostream>


struct LoggerImpl
{
	bool SetLevel(WarnLevel aLev) {  iLev = aLev; return true; }
	bool SetStream(std::wostream& aOs) { iWos = &aOs; return true; }

	std::wostream& Stream() { return *iWos; } 
	bool ShouldPrint(WarnLevel aLev) { return iLev >= aLev; } 

	WarnLevel iLev;
	std::wostream *iWos;
};


LoggerImpl* Logger::iLogger = 0;

LoggerImpl& Logger::Impl()	
{
	if (!iLogger)
	{
		iLogger = new LoggerImpl();
		iLogger->SetStream(std::wcout);
		iLogger->SetLevel(WARN_WARN);
	} 
	return *iLogger;
}


bool Logger::SetLevel(WarnLevel aLev) 
{
	return Impl().SetLevel(aLev);
}
bool Logger::SetStream(std::wostream& aOs)
{
	return Impl().SetStream(aOs);
}

std::wostream& Logger::Stream()
{
	return Impl().Stream();
}

bool Logger::ShouldPrint(WarnLevel aLev)
{
	return Impl().ShouldPrint(aLev);
}
