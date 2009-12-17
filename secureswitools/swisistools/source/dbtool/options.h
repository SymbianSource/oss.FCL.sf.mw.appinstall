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
* COptions - Used to parse command liine options.
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef DBOPTIONS_H
#define DBOPTIONS_H

#pragma warning(disable: 4786)

#include "logs.h"
#include <string>
#include <vector>

class COptions
{
	public:
		COptions(int aParamsCount, char** aParams);
		~COptions();
	
	public:
		std::string GetLogFileName();
		CLogger::TLogLevel GetLogLevel();
		std::string GetDbFileName();
		std::vector<std::string> GetEnvFileNames();
		std::string GetPrePovisionFileName();
		bool IsDbAbsent();
		bool IsPreProvisionInfoAvailable();

	private:
		void ValidateOptions(int aParamsCount, char** aParams);
		void DisplayDetailedUsage();
		void DisplayUsage();
		void DisplayVersion();
		void SetLogLevel(const char* aLogLevel);
		void CheckParamExists(int aParamsCount, int aCurrentCount);
		bool IsFileAbsent(const char* aFileName);

	private:
		bool iDbExist;
		bool iLoggingEnabled;
		bool iPreProvisionUpdate;
		std::vector<std::string> iXmlFileNames;
		std::string iPreProvisionFileName;
		std::string iScrDbName;
		CLogger::TLogLevel iLogLevel;
		std::string iLogFileName;

	private:
		static std::string iCurrentVersion;
		static std::string iDescription;
		static std::string iCopyright;
		static std::string iUsage;
		static std::string iGeneralInfo;
};

#endif //DBOPTIONS_H