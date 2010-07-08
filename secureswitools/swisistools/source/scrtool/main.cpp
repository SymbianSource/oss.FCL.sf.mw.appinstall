/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "dblayer.h"
#include "xmlparser.h"
#include "logs.h"
#include "exception.h"
#include "options.h"

#include <iostream>
#include <string>
#include <memory>

typedef std::vector<std::string>::iterator StringIterator;
typedef std::vector<XmlDetails::TScrEnvironmentDetails> EnvDetails;
typedef std::vector<std::string> SchemaDetails;


int main (int argc, char** argv) 
	{
	int returnCode = 0;

	try
		{
		const char* epocRoot = getenv("EPOCROOT");		
		if(NULL == epocRoot)
			{
			throw CException("EPOCROOT environment variable not specified.", ExceptionCodes::EEnvNotSpecified);
			}
		std::string epocRootStr(epocRoot); 
		
		COptions options(argc, argv);

		std::string logFileName(options.GetLogFileName());
		std::auto_ptr<CLogger> logger(new CLogger(logFileName, options.GetLogLevel()));
		
   #ifdef __LINUX__
  	  	 std::string dllPath = "sqlite-3.6.1.so";
   #else
 	       std::string dllPath = "sqlite3.dll";
   #endif
  	  	 	 
		std::auto_ptr<CDbLayer> db( new CDbLayer(dllPath, options.GetDbFileName()));
		std::auto_ptr<CScrXmlParser> xmlParser( new CScrXmlParser());

		if(options.IsDbAbsent())
			{
		  #ifdef __LINUX__ 		  
			std::string dbFileName = epocRootStr + "epoc32/tools/create_db.xml";
			#else
			std::string dbFileName = epocRootStr + "epoc32\\tools\\create_db.xml";
			#endif
			
			std::auto_ptr<SchemaDetails> schema(xmlParser->ParseDbSchema(dbFileName));
			db->CreateScrDatabase(*schema);
			}

		std::vector<std::string> xmlFileNames = options.GetEnvFileNames();
		for(StringIterator xmlIter=xmlFileNames.begin(); xmlIter != xmlFileNames.end(); ++xmlIter)
			{
			std::auto_ptr<EnvDetails> envDetails(xmlParser->GetEnvironmentDetails(*xmlIter));
			db->PopulateScrDatabase(*envDetails);
			}
		
		if(options.IsPreProvisionInfoAvailable())
			{
			std::string preProvFileName = options.GetPrePovisionFileName();
			XmlDetails::TScrPreProvisionDetail preProvisionDetailList = xmlParser->GetPreProvisionDetails(preProvFileName);
			db->PopulatePreProvisionDetails(preProvisionDetailList);
			}
		}
	catch(CException& aException)
		{
		const std::string& exc = aException.GetMessageA();
		returnCode = aException.GetCode();
		std::cout << "Exception caught::" << exc << std::endl;
		std::cout << "Error Code::" << returnCode << std::endl;
		}
		
	return returnCode;
	}


