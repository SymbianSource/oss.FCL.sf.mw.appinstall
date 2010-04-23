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
* CTestXmlParser - Used to testing retrieval of data pertianing to creation and updation 
* of database.
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#include <vector>
#include <string>
#include <memory>
#include <iostream>

#include <xercesc/sax2/DefaultHandler.hpp>

#include "txmlparser.h"
#include "xmlparser.h"
#include "exception.h"
#include "logger.h"
#include "../../source/common/utility.h"

using namespace std;

typedef vector<string>::iterator StringIterator;

typedef vector<CScrXmlParser::TScrEnvironmentDetails>::iterator envDetailsIter;

std::string CTestXmlParser::Passed = "Passed";
std::string CTestXmlParser::Failed = "Failed";

CTestXmlParser::CTestXmlParser()
	{
	std::string logFile = "xmlparse.log";
	CLogger::TLogLevel logLevel = static_cast<CLogger::TLogLevel>( CLogger::ELogEnter | CLogger::ELogExit);
	iLogger = new CLogger(logFile, logLevel);
	iXmlParser = new CScrXmlParser();
	}

CTestXmlParser::~CTestXmlParser()
	{
	delete iXmlParser;
	delete iLogger;
	}

CTestXmlParser::TestParser()
	{
	
	// tests for createdb xml schema
	PrintResult("TestValidDbFile()",TestValidDbFile());
	PrintResult("TestDbFileNotPresent",TestDbFileNotPresent());
	PrintResult("TestEmptyDbFile",TestEmptyDbFile());
	PrintResult("TestInvalidDbFiles",TestInvalidDbFiles());
	
	PrintResult("TestValidUpdateDb",TestValidUpdateDb());
	PrintResult("TestUpdateFileNotPresent",TestUpdateFileNotPresent());
	PrintResult("TestEmptyUpdateFile",TestEmptyUpdateFile());
	PrintResult("TestInvalidUpdateDb",TestInvalidUpdateDb());

	}


void CTestXmlParser::PrintResult(const char* aMethodName, bool aResult)
	{
	cout << aMethodName << ": ";
	if(aResult)
		{
		cout << Passed;
		}
	else
		{
		cout << Failed;
		}
	cout << endl;
	}


bool CTestXmlParser::TestValidDbFile()
	{
	bool retVal = false;
	
	try
		{
		string createDbFile(".\\scripts\\data\\create_db.xml");
		vector<string>* schema = iXmlParser->ParseDbSchema(createDbFile);
		retVal = true;
		}
	catch(CException& xmle)
		{
		LERROR(xmle.GetMessageA());
		}
	return retVal;
	}


bool CTestXmlParser::TestDbFileNotPresent()
	{
	bool retVal = false;
	string createDbFile(".\\scripts\\data\\not_present.xml");
	try
		{
		vector<string>* schema = iXmlParser->ParseDbSchema(createDbFile);
		}
	catch(CException& xmle)
		{
		
		LERROR(xmle.GetMessageA());

		if(xmle.GetCode() == (int)ExceptionCodes::EFatalError)
			{
			retVal = true;
			}
		}
	return retVal;
	}


bool CTestXmlParser::TestEmptyDbFile()
	{
	bool retVal = false;
	string createDbFile(".\\scripts\\data\\create_db_empty.xml");
	try
		{
		vector<string>* schema = iXmlParser->ParseDbSchema(createDbFile);
		
		}
	catch(CException& xmle)
		{
		
		LERROR(xmle.GetMessageA());
		
		if(xmle.GetCode() == (int)ExceptionCodes::EFatalError)
			{
			retVal = true;
			}
		}
	return retVal;
	}


bool CTestXmlParser::TestInvalidDbFiles()
	{
	bool retVal = false;
	if( TestEndTagMissing() && TestInvalidRoot() && AgainstDtdSpec())
		{
		retVal = true;
		}
	return retVal;
	}

bool CTestXmlParser::TestInvalidRoot()
	{
	bool retVal = false;
	
	try
		{
		string createDbFile(".\\scripts\\data\\create_db_invalid_root.xml");
		vector<string>* schema = iXmlParser->ParseDbSchema(createDbFile);
		}
	catch(CException& xmle)
		{
		
		LERROR(xmle.GetMessageA());
		
		if(xmle.GetCode() == (int)ExceptionCodes::EParseError)
			{
			retVal = true;
			}
		}
	return retVal;
	}

bool CTestXmlParser::TestEndTagMissing()
	{
	bool retVal = false;
	try
		{
		string createDbFile(".\\scripts\\data\\create_db_end_tag_missing.xml");
		vector<string>* schema = iXmlParser->ParseDbSchema(createDbFile);
		
		}
	catch(CException& xmle)
		{
		
		LERROR(xmle.GetMessageA());
		
		if(xmle.GetCode() == (int)ExceptionCodes::EFatalError)
			{
			retVal = true;
			}
		}
	return retVal;
	}

bool CTestXmlParser::AgainstDtdSpec()
	{
	bool retVal = false;
	
	try
		{
		string createDbFile(".\\scripts\\data\\create_db_against_dtd_spec.xml");
		vector<string>* schema = iXmlParser->ParseDbSchema(createDbFile);
		}
	catch(CException& xmle)
		{
		
		LERROR(xmle.GetMessageA());
		
		if(xmle.GetCode() == (int)ExceptionCodes::EParseError)
			{
			retVal = true;
			}
		}
	return retVal;
	}

bool CTestXmlParser::TestValidUpdateDb()
	{
	bool retVal = false;
	try
		{
		string envFile(".\\scripts\\data\\update_db.xml");
		vector<CScrXmlParser::TScrEnvironmentDetails>* envDetails = iXmlParser->GetEnvironmentDetails(envFile);
		retVal = true;
		}
	catch(CException& xmle)
		{
		
		LERROR(xmle.GetMessageA());
		
		}
	return retVal;
	}

bool CTestXmlParser::TestUpdateFileNotPresent()
	{
	bool retVal = false;
	string createDbFile(".\\scripts\\data\\not_present.xml");
	try
		{
		vector<CScrXmlParser::TScrEnvironmentDetails>* envDetails = iXmlParser->GetEnvironmentDetails(createDbFile);
		
		}
	catch(CException& xmle)
		{
		
		LERROR(xmle.GetMessageA());
		
		if(xmle.GetCode() == (int)ExceptionCodes::EFatalError)
			{
			retVal = true;
			}
		}
	return retVal;
	}

bool CTestXmlParser::TestEmptyUpdateFile()
	{
	bool retVal = false;
	string createDbFile(".\\scripts\\data\\update_db_empty.xml");
	
	try
		{
		vector<CScrXmlParser::TScrEnvironmentDetails>* envDetails = iXmlParser->GetEnvironmentDetails(createDbFile);
		
		}
	catch(CException& xmle)
		{
		
		LERROR(xmle.GetMessageA());
		
		if(xmle.GetCode() == (int)ExceptionCodes::EFatalError)
			{
			retVal = true;
			}
		}
	return retVal;
	}

bool CTestXmlParser::TestInvalidUpdateDb()
	{
	if(TestUpdateDbEndTagMissing() && TestUpdateDbInvalidRoot() &&	UpdateDbAgainstDtdSpec())
		return true;
	return false;
	}


bool CTestXmlParser::TestUpdateDbEndTagMissing()
	{
	bool retVal = false;
	
	try
		{
		string createDbFile(".\\scripts\\data\\update_db_end_tag_missing.xml");
		vector<CScrXmlParser::TScrEnvironmentDetails>* envDetails = iXmlParser->GetEnvironmentDetails(createDbFile);
		
		}
	catch(CException& xmle)
		{
		
		LERROR(xmle.GetMessageA());
		
		if(xmle.GetCode() == (int)ExceptionCodes::EFatalError)
			{
			retVal = true;
			}
		}
	return retVal;
	}

bool CTestXmlParser::TestUpdateDbInvalidRoot()
	{
	bool retVal = false;
	
	try
		{
		string createDbFile(".\\scripts\\data\\update_db_invalid_root.xml");
		vector<CScrXmlParser::TScrEnvironmentDetails>* envDetails = iXmlParser->GetEnvironmentDetails(createDbFile);
		
		}
	catch(CException& xmle)
		{
		
		LERROR(xmle.GetMessageA());
		
		if(xmle.GetCode() == (int)ExceptionCodes::EParseError)
			{
			retVal = true;
			}
		}
	return retVal;
	}

bool CTestXmlParser::UpdateDbAgainstDtdSpec()
	{
	bool retVal = false;
	
	try
		{
		string createDbFile(".\\scripts\\data\\update_db_against_dtd_spec.xml");
		vector<CScrXmlParser::TScrEnvironmentDetails>* envDetails = iXmlParser->GetEnvironmentDetails(createDbFile);
		
		}
	catch(CException& xmle)
		{
		
		LERROR(xmle.GetMessageA());
		
		if(xmle.GetCode() == (int)ExceptionCodes::EParseError)
			{
			retVal = true;
			}
		}
	return retVal;
	}


void CTestXmlParser::DisplayDetails(std::vector<std::string>& aDetails)
	{
	for(StringIterator iter=aDetails.begin(); iter != aDetails.end(); ++iter)
		{
		cout << *iter << endl;
		}
	}

void CTestXmlParser::DisplayDetails(vector<CScrXmlParser::TScrEnvironmentDetails>& aEnvDetails)
	{
	for(envDetailsIter iter=aEnvDetails.begin(); iter != aEnvDetails.end(); ++iter)
		{
		cout << "Software type name:" << iter->iSoftwareTypeName << endl;
		cout << "SIF Plugin Id:" << iter->iSifPluginUid << endl;
		cout << "Installer Sid:" << iter->iInstallerSid << endl;
		cout << "Execution Layer Sid:" << iter->iExecutionLayerSid << endl;
		cout << "MIME Details" << endl;
		for(vector<string>::iterator mimeiter = iter->iMIMEDetails.begin() ; mimeiter != iter->iMIMEDetails.end() ; ++mimeiter )
			{
			cout << "MIME Type:" << *mimeiter << endl;
			}
		}
	}

