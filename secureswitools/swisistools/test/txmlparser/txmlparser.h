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

#ifndef TXMLPARSER_H
#define TXMLPARSER_H

#include <vector>
#include <string>
#include "logger.h"
#include "xmlparser.h"

class CTestXmlParser
	{
	public:
		CTestXmlParser();
		~CTestXmlParser();
		TestParser();
	
	private:
		bool TestValidDbFile();
		bool TestDbFileNotPresent();
		bool TestEmptyDbFile();
		bool TestInvalidDbFiles();
		
		bool TestInvalidRoot();
		bool TestEndTagMissing();
		bool AgainstDtdSpec();

		
		bool TestValidUpdateDb();
		bool TestUpdateFileNotPresent();
		bool TestXmlFileNotPresent();
		bool TestEmptyUpdateFile();
		bool TestInvalidUpdateDb();
		
		bool TestUpdateDbEndTagMissing();
		bool TestUpdateDbInvalidRoot();
		bool UpdateDbAgainstDtdSpec();

		void DisplayDetails(std::vector<std::string>& aDetails);
		void DisplayDetails(std::vector<CScrXmlParser::TScrEnvironmentDetails>& aEnvDetails);
		void PrintResult(const char* aMethodName, bool aResult);
	private:
		CScrXmlParser* iXmlParser;
		CLogger* iLogger;
		static std::string Passed;
		static std::string Failed;
	};



#endif // TXMLPARSER_H