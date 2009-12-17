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
* For parsing command line arguments.
* @internalComponent
*
*/


#include "cmdparser.h"
#include "exceptionhandler.h"
#include "details.h"
#include "utils.h"
#include "metagenerator.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

typedef vector<char>::iterator CharIterator;

const char UID			='u';
const char DRIVE		='d';
const char SISNAME		='s';
const char FILENAMES	='f';
const char OUTPUT		='o';
const char HELP1		='h';
const char HELP2		='?';
const char VERSION		='v';
const char DASH			='-';
const char OPTIONLENGTH	=2;
const char BACKSLASH	='\\';
const string HexSymbol = "x";
const int DefaultDrive = 0xff;

CCmdParser::CCmdParser (int aArgc, const char** aArgv): 
	iTokenCount(aArgc),
	iTokens(aArgv),
	iOpDir(Utils::GetCurrentDirectory()),
	iDriveName(DefaultDrive),
	iOpFileName(CMetaGenerator::FilePrefix) 
	{}

void CCmdParser::ValidateOptions() const throw (TExceptionHandler)
	{
	if( iTokenCount == 1 )
		{
		Display (CCmdParser::EVersion);
		Display (CCmdParser::EDescription);
		Display (CCmdParser::ECopyright);
		throw TExceptionHandler (TExceptionHandler::EMandatoryOptions);
		}
	
	if( HELP1 != iTokens[1][1] && HELP2 != iTokens[1][1] && VERSION != iTokens[1][1] )
		{		
		CheckOptionsFormat();
		CheckMandatoryOptions();
		CheckDuplicate();
		CheckOrder();
		}
	
	}

void CCmdParser::Display(CCmdParser::TOptions aOptions)
	{
	if ((aOptions&CCmdParser::EVersion) == CCmdParser::EVersion) 
		{
		cout << gCurrentVersion << endl;
		}

	if ((aOptions&CCmdParser::EDescription) == CCmdParser::EDescription)	
		{
		cout << gDescription << endl;	
		}
	
	if((aOptions&CCmdParser::ECopyright) == CCmdParser::ECopyright)	
		{
		cout << gCopyright << endl;	
		}
	
	if((aOptions&CCmdParser::EUsage) == CCmdParser::EUsage)	
		{
		cout << gUsage << endl;	
		}

	if((aOptions&CCmdParser::EHelp) == CCmdParser::EHelp)
		{
		cout << gHelp << endl;
		}
	}

void CCmdParser::CheckOptionsFormat() const throw (TExceptionHandler)
	{
	int currentIndex = 1;
	char currentChoice = 0;

	while(currentIndex!=iTokenCount)
		{
		if(iTokens[currentIndex][0] != DASH || strlen(iTokens[currentIndex]) != OPTIONLENGTH )
			{
			throw TExceptionHandler (TExceptionHandler::EInvalidParameters);
			}
		currentChoice = iTokens[currentIndex][1];
		++currentIndex;
		if ( currentIndex == iTokenCount || iTokens[currentIndex] == 0 || iTokens[currentIndex][0] == DASH )
			{
			throw TExceptionHandler (TExceptionHandler::EInvalidFormat);
			}
		++currentIndex;
		}
	}

void CCmdParser::CheckMandatoryOptions() const throw (TExceptionHandler)
	{
	int currentIndex = 1;
	bool uid=false;
	bool sisname=false;

	while(currentIndex != iTokenCount)
		{
		switch(iTokens[currentIndex][1])
			{
			case UID:
				uid = true;
				break;
			case SISNAME:
				sisname = true;
				break;				
			}
		++currentIndex;
		}
		if(!uid || !sisname )
			{
			throw TExceptionHandler(TExceptionHandler::EMandatoryOptions);
			}
	}

void CCmdParser::CheckDuplicate() const throw (TExceptionHandler)
	{
	int currentIndex = 1;
	bool uid=false;
	bool drive=false;
	bool sisfile=false;
	bool filename=false;
	bool outputfilename=false;

	while(currentIndex != iTokenCount)
		{
		switch(iTokens[currentIndex][1])
			{
			case UID:
				{
				if(uid)
					{
					throw TExceptionHandler (TExceptionHandler::EParameterRepeat);
					}
				uid = true;
				break;
				}
			
			case DRIVE:
				{
				if(drive)
					{
					throw TExceptionHandler (TExceptionHandler::EParameterRepeat);
					}
				drive = true;
				break;
				}

			case SISNAME:
				{
				if(sisfile)
					{
					throw TExceptionHandler (TExceptionHandler::EParameterRepeat);
					}
				sisfile = true;
				break;
				}

			case FILENAMES:
				{
				if(filename)
					{
					throw TExceptionHandler (TExceptionHandler::EParameterRepeat);
					}
				filename = true;
				break;
				}

			case OUTPUT:
				{
				if(outputfilename)
					{
					throw TExceptionHandler (TExceptionHandler::EParameterRepeat);
					}
				outputfilename = true;
				break;
				}
		} // end of switch
		++currentIndex;
	} // end of while
	}

void CCmdParser::CheckOrder() const throw (TExceptionHandler)
	{
	int currentIndex = 1;
	vector<char> options;

	while(currentIndex != iTokenCount)
		{	
		if( iTokens[currentIndex][0] == DASH)
			{
			options.push_back(iTokens[currentIndex][1]);					
			}
		++currentIndex;
		}
	
	CharIterator optIter=options.begin();
	
	// first should be uid
	if(*optIter != UID)
		{
		throw TExceptionHandler (TExceptionHandler::EInvalidOrder);
		}
	++optIter;
	
	bool isSisPresent = false;

	while(optIter != options.end())
		{
		switch(*optIter)
			{
			// drive name should be followed by at least one sis file name.
			case DRIVE:
				{
				isSisPresent = false;
				if(*(optIter+1) != SISNAME)
					{
					throw TExceptionHandler (TExceptionHandler::EInvalidOrder);
					}
				
				break;
				}

			case SISNAME:
				{
				isSisPresent = true;
				break;
				}

			// if filename, then previously at least one sis file should be specified.
			case FILENAMES:
				{
				if( !isSisPresent )
					{
					throw TExceptionHandler (TExceptionHandler::EInvalidOrder);	
					}
				break;
				}	
			}

		++optIter;
		}
	}


void CCmdParser::LoadParams() throw (TExceptionHandler)
	{
	int currIndex = 1;
	char choice = 0;
	int drivename = 0;

	while (currIndex != iTokenCount)
		{
		choice = tolower(iTokens[currIndex][1]);
		++currIndex;
		
		switch(choice)
			{	
			case UID:
				{
				string uid = iTokens[currIndex];
				uid = Utils::StringToLower(uid);
				int start = uid.find_first_of(HexSymbol); 
				
				if( start != -1 )
					uid = uid.substr(start+1,uid.length());
				iUid = uid;
				
				++currIndex;
				
				iOpFileName.append(uid);
				break;
				}
			
			case DRIVE:
				{
				int drive = toupper(*(iTokens[currIndex]));
				iDriveName = drive;
				++currIndex;
				break;
				}

			case SISNAME:
				{
				string sisstore = iTokens[currIndex];
				if( Utils::FileExists(sisstore) == false )
					{
					throw TExceptionHandler(TExceptionHandler::EFileNotPresent);
					}
				ifstream stream(sisstore.c_str(), std::ios::in);
				string sisFileName;
				while(stream.eof() == false)
					{
					getline(stream,sisFileName);
					if(Utils::FileExists(sisFileName ))
						iSISFileNames.push_back(sisFileName);
					}
				++currIndex;
				break;
				}

			case FILENAMES:
				{
				string fileStore = iTokens[currIndex];
				ifstream stream(fileStore.c_str(), std::ios::in);
				string fileName;
				while(stream.eof() == false)
					{
					getline(stream,fileName);
					Utils::TrimSpaces(fileName);
					if(!fileName.empty())
						iFileNames.push_back(fileName);
					}
				++currIndex;
				break;
				}

			case OUTPUT:
				{
				string dirName;
				string fileName;
				Parse (iTokens[currIndex], dirName, fileName);
				if (dirName.length() != 0)
					{
					iOpDir = dirName;
					}
				if (fileName.length() != 0)
					{
					iOpFileName = fileName;
					}
				if( Utils::FileExists(iOpDir) == false)
					{
					throw TExceptionHandler(TExceptionHandler::EDirNotPresent);
					}

				++currIndex;
				break;
				}

			case HELP1: case HELP2:
				{
				Display (CCmdParser::EAll);
				break;
				}

			case VERSION:
				{
				Display (CCmdParser::EVersion);
				break;
				}

			default:
				{
				throw TExceptionHandler(TExceptionHandler::EInvalidOptions);
				}
			
			} // end of switch
		

		} // end of while
	}

void CCmdParser::Parse (const char* aPath, string& aDirName, string& aFileName) const
	{
	string sPath(aPath);
	size_t position = sPath.find_last_of(BACKSLASH);
	if(position == string::npos)
		{
		aFileName = sPath;
		}
	else
		{
		aDirName = sPath.substr(0,position+1);
		aFileName = sPath.substr(position+1,sPath.length());
		}
	}


bool CCmdParser::IsOptionsSet () const 
	{ 
	bool bOptions = false;
	if(iUid.length() != 0 && iSISFileNames.size() != 0 )
		{	
		bOptions = true;
		}
	return bOptions; 
	}

const string& CCmdParser::GetUid () const 
	{ 
	return iUid; 
	}

const int CCmdParser::GetDrive() const 
	{ 
	return iDriveName; 
	}

const vector<string>& CCmdParser::GetSISFileNames () const
	{ 
	return iSISFileNames; 
	}

const vector<string>& CCmdParser::GetFileNames () const
	{ 
	return iFileNames; 
	}

const string& CCmdParser::GetOpDir () const	
	{ 
	return iOpDir; 
	}

const string& CCmdParser::GetOpFileName() const	
	{ 
	return iOpFileName; 
	}
