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
* main.h
* Entry point.
* @internalComponent
*
*/


#include "cmdparser.h"
#include "metagenerator.h"
#include "exceptionhandler.h"
#include "details.h"

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

typedef vector<string>::iterator StringIterator;

void DisplayDetails(const CCmdParser&);

int main(int argc, char** argv )
{
	
	int errorCode = 0;
	CCmdParser cmd(argc,const_cast<const char**>(argv));
	CMetaGenerator* metagenerator = NULL;

	try
		{
		cmd.ValidateOptions();
		cmd.LoadParams();

		if( cmd.IsOptionsSet() )
			{
			string metaFileName = cmd.GetOpDir() + cmd.GetOpFileName();
			metagenerator = new CMetaGenerator(	cmd.GetUid(), 
												cmd.GetDrive(), 
												cmd.GetSISFileNames(), 
												cmd.GetFileNames(), 
												metaFileName 
											  );
			metagenerator->GenerateMetaData();
			DisplayDetails(cmd);
			}
		} 
	catch(TExceptionHandler m)
		{
		if( metagenerator != NULL )
			{
			metagenerator->CleanUp();
			}
			
		CCmdParser::Display(CCmdParser::EUsage);
		errorCode = m.GetErrorCode();
		cout << "Error Message:: " << m.GetErrorString() << endl;
		cout << "Error Code:: " << errorCode << endl;
		} 
	catch(std::exception e)
		{
		cout << e.what() << endl;
		errorCode = TExceptionHandler::ESystemError;
		}	
	
	delete metagenerator;

	return errorCode;
	
}

void DisplayDetails(const CCmdParser& aCmdParser)
	{
	cout << gMetaSuccess << endl;
	cout << "Drive name:" << aCmdParser.GetDrive() << endl;
	vector<string> SISFileNames = aCmdParser.GetSISFileNames();
	StringIterator SISIter;	
	StringIterator SISEnd = SISFileNames.end();	
	
	cout << gSISName << endl;
	int count = 0;
	for(SISIter=SISFileNames.begin();SISIter!=SISEnd;++SISIter)
		{
		cout << ++count << ":" << *SISIter << endl;
		}
	
	cout << gMetaName << aCmdParser.GetOpFileName() << endl;
	cout << gMetaLocation << aCmdParser.GetOpDir() << endl;
	}

