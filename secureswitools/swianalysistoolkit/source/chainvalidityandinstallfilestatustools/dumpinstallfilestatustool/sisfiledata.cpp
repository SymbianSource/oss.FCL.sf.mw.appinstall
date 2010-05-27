/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "sisfiledata.h"
#include "osinterface.h"
using namespace std;

SISFileData::SISFileData()
	:iExeLabelAndCapabilities (NULL)
	{
	}

int SISFileData::EciFileRead(const char* aFile , const Options& aOptions)
	{
	ifstream file;
	string line ;
	string exeCapabilitiesFile = ".\\";
	string exeCapabilities ;
	string exeKey ;
	string exeValue;
	unsigned int exeKeyloc = 0 ;
	unsigned int exeValueloc = 0;
	exeCapabilitiesFile.append(aFile);
	file.open(aFile);
	if(!file)
		{
		throw EUnableToOpenFile;
		}
	else
		{
		ExeLabelAndCapabilitiesList* iNext = NULL;
		ExeLabelAndCapabilitiesList* node = NULL ;
		ExeLabelAndCapabilitiesList* nextNode = NULL; 
		while(!file.eof())
			{ 
			getline(file,line);
			if((exeKeyloc = line.find("@",0)) != string::npos)
				{
				int certCapabilities = 0;
				exeKey = line.substr(0,line.find("=",0));
				exeValueloc = line.find("=",0);
				exeValue = line.substr(line.find("=",exeValueloc)+1);
				iExeName = exeValue.substr(0,exeValue.find("@",0));
				if((iExeName.find(".exe") != string::npos) || (iExeName.find(".dll") !=string::npos))
					{
					node = new ExeLabelAndCapabilitiesList;
					node->iExeCapability = 0;
					node->iExeLabel = iExeName;
					exeCapabilities = exeValue.substr(exeValue.find("@",0)+1);
					//convert string to integer in order to form an integer vector.
					std::istringstream stream(exeCapabilities);
					stream >> certCapabilities;
					node -> iExeCapability = certCapabilities ;
					node -> iNext = NULL;
					//when it is the first node in the list
					if(iExeLabelAndCapabilities == NULL)
						{
						iExeLabelAndCapabilities = node;
						}
					else
						{
						nextNode = iExeLabelAndCapabilities;
						while(nextNode -> iNext != NULL)
							{
							nextNode = nextNode->iNext;
							}

						nextNode -> iNext = node;
						}
					}
				}
			}
		}

	file.close();
	if(!iExeLabelAndCapabilities)
		{
		cout << "SIS File doesn't contain any executables." << endl;
		return 0;
		}

	return 1;
	}

SISFileData::~SISFileData()
	{
	for(ExeLabelAndCapabilitiesList* list = iExeLabelAndCapabilities ; list ; )
		{
		ExeLabelAndCapabilitiesList* next = list -> iNext;
		delete list;
		list = next;
		}
	}