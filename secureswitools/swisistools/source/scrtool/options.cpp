/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "options.h"
#include "../common/exception.h"

#include <fstream.h>
#include <vector>
#include <string>
#include <algorithm>

std::string COptions::iCurrentVersion = "Registry Database Generator Version 1, 0, 2";

std::string COptions::iDescription =	"A utility for generating and updating the Software Component Registry database which stores "
							"information for all installed software components.";

std::string COptions::iCopyright = "Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.";

std::string COptions::iUsage	=	
						"Usage : scrtool [-h|-?] [-v] [-l] [-w] -c or -d [DB name] -f [Environment XML file] -p [Pre-provision XML file]"
						
						"\n-h|-? Show help usage."
						
						"\n-v Show tool version."
						
						"\n-c [mandatory if -d not specified]" 
						"\n\tCreate or regenerate database. This option would always create a new database." 
						"\n\tIf a database with the same name already exists it would be overwritten.\n"
						
						"-d [mandatory if -c not specified]"
						"\n\tName of an already existing SCR database."
						"\n\tThis option assumes existence of a database and updates it with" 
						"\n\tadditional software environment or pre-provisioning information."
						
						"\n-f\tXML file which contains software environment information."
						
						"\n-p\tXML file which contains pre-provision information."

						"\n-l\tName of the log file, Default: scrtool.log\n"
						
						"\n-w\tLevel of logging information [ERROR,WARN,INFO,ALL], Default: ALL.\n";
						
std::string COptions::iGeneralInfo =  
							"Tool can be used in the following ways: \n"
							"1. [-c and -f]: Create database with environment information.\n"
							"2. [-c and -f]: Replace an existing database with new environment information.\n"
							"3. [-d and -f]: Add new environment information to an existing database as specified in input XML files.\n"
							"4. [-d and -p]: Add pre-provision details to an already existing database.\n"
							"5. [-c,-f and -p]: Create database, populate environments and update the pre-provision information.\n"
							"Name of the database ( either -c or -d ) and at least one XML file (-f) with environment update"
							"\nshould be specified.";


COptions::COptions(int aParamsCount, char** aParams)
	:iDbExist(false),
	 iLoggingEnabled(false),
	 iLogLevel(CLogger::ELogAll),
	 iLogFileName("scrtool.log"),
	 iPreProvisionUpdate(false)
	{
	ValidateOptions(aParamsCount, aParams);
	}

COptions::~COptions()
	{
	}

void COptions::ValidateOptions(int aParamsCount, char** aParams)
	{
	bool envFileNameOptionExists = false;
	bool createDbOptionExists=  false;
	bool preProvFileExists = false;

	int count = 0;
	while(++count < aParamsCount)
		{
		// each option should have a corresponding data present 
		if(*aParams[count] != '-' )
			{
			DisplayUsage();
			throw CException("Invalid argument passed.",ExceptionCodes::EInvalidArgument);
			}

		const char* ch = aParams[count]+1;

		switch(*ch)
			{
			case 'h':
			case '?':
				DisplayDetailedUsage();
				exit(0);
				break;
			case 'v':
				DisplayVersion();
				exit(0);
				break;
			case 'c':
				CheckParamExists(aParamsCount,++count);
				iScrDbName = aParams[count];
				createDbOptionExists = true;
				break;
			case 'd':
				CheckParamExists(aParamsCount,++count);
				iScrDbName = aParams[count];
				iDbExist = true;
				break;
			case 'f':
				{
				CheckParamExists(aParamsCount,count+1);
				// get the XML file names
				char * xmlFileName;
				do
					{
					xmlFileName = aParams[++count];
					if(IsFileAbsent(xmlFileName))
						{
						DisplayUsage();
						std::string errMsg(xmlFileName);
						errMsg += " not present";
						throw CException(errMsg,ExceptionCodes::EFileNotPresent);
						}
					iXmlFileNames.push_back(xmlFileName);
					} while( count+1 < aParamsCount && *aParams[count+1] != '-'  );
				envFileNameOptionExists = true;
				break;
				}

			case 'p':
				{
				CheckParamExists(aParamsCount,++count);
				// get the XML file names
				char * xmlFileName = aParams[count];
				
				if(IsFileAbsent(xmlFileName))
					{
					DisplayUsage();
					std::string errMsg(xmlFileName);
					errMsg += " not present";
					throw CException(errMsg,ExceptionCodes::EFileNotPresent);
					}
				iPreProvisionFileName = xmlFileName;
				iPreProvisionUpdate = true;
				break;
				}
			case 'l':
				CheckParamExists(aParamsCount,++count);
				iLogFileName = aParams[count];
				break;
			case 'w':
				CheckParamExists(aParamsCount,++count);
				SetLogLevel(aParams[count]);
				break;
			default:
				{
				DisplayUsage();
				throw CException("Invalid argument passed.",ExceptionCodes::EInvalidArgument);
				}
			} // end of switch
		} // end of while
	
	// if any of these 3 options are not satisfied then 
	// mandatory option has not been specified.	
	if( // existing db name and pre-provision file
		!((iDbExist && iPreProvisionUpdate)
		// existing db and env file
		|| (envFileNameOptionExists && iDbExist) 
		// create new db and env file
		|| (createDbOptionExists && envFileNameOptionExists)))
		{
		DisplayUsage();
		throw CException("Mandatory option not specified",ExceptionCodes::EMandatoryOption);
		}
	if(createDbOptionExists && iDbExist)
		{
		DisplayUsage();
		throw CException("-c and -d cannot be specified at the same time.",ExceptionCodes::EMutuallyExclusive);
		}
	if(createDbOptionExists)
		{
		remove(iScrDbName.c_str());
		}
	}


void COptions::CheckParamExists(int aParamsCount, int aCurrentCount)
	{
	if(aCurrentCount >= aParamsCount)
		{
		DisplayUsage();
		throw CException("Invalid argument passed.",ExceptionCodes::EInvalidArgument);
		}
	}

bool COptions::IsFileAbsent(const char* aFileName)
	{
	std::ifstream fileStream(aFileName,ios::in);
	if(!fileStream)
		return true;
	fileStream.close();
	return false;
	}

void COptions::DisplayDetailedUsage()
	{
	std::cout << iDescription << std::endl;
	std::cout << iCopyright << std::endl;
	DisplayUsage();
	std::cout << iGeneralInfo << std::endl;
	}

void COptions::DisplayUsage()
	{
	DisplayVersion();
	std::cout << iUsage << std::endl;
	}

void COptions::DisplayVersion()
	{
	std::cout << iCurrentVersion << std::endl;
	}

void COptions::SetLogLevel(const char* aLogLevel)
	{
	std::string logLevel(aLogLevel);
	std::transform(logLevel.begin(), logLevel.end(), logLevel.begin(), ::toupper);

	if(logLevel == "ERROR")
		{
		iLogLevel = CLogger::ELogError;
		}
	else if(logLevel == "WARN")
		{
		iLogLevel = CLogger::ELogWarning;
		}
	else if(logLevel == "INFO")
		{
		iLogLevel = static_cast<CLogger::TLogLevel>(CLogger::ELogEnter | CLogger::ELogExit | CLogger::ELog);
		}
	else if(logLevel == "ALL")
		{
		iLogLevel = CLogger::ELogAll;
		}	
	else
		{
		throw CException("Invalid log level specified",ExceptionCodes::EInvalidArgument);
		}
	}

std::string COptions::GetLogFileName()
	{
	return iLogFileName;
	}

CLogger::TLogLevel COptions::GetLogLevel()
	{
	return iLogLevel;
	}

std::string COptions::GetDbFileName()
	{
	return iScrDbName;
	}

std::vector<std::string> COptions::GetEnvFileNames()
	{
	return iXmlFileNames;
	}

std::string COptions::GetPrePovisionFileName()
	{
	return iPreProvisionFileName;
	}

bool COptions::IsDbAbsent()
	{
	return !iDbExist;
	}

bool COptions::IsPreProvisionInfoAvailable()
	{
	return iPreProvisionUpdate;
	}
