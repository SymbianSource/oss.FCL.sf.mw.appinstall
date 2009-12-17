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
* Common utility functions.
* @internalComponent
*
*/


#include "utils.h"
#include "exceptionhandler.h"
#include <direct.h>

#include <io.h>

using std::string;

string Utils::GetCurrentDirectory()
	{
	char* currentPath = new char [_MAX_PATH];
	getcwd (currentPath, _MAX_PATH);
	string currPath(currentPath);
	currPath += "\\";
	delete[] currentPath; 
	return currPath;
	}


int Utils::HexToInt(const string& aHexString )
	{
	
	int uid;
	sscanf(aHexString.c_str(),"%x",&uid);
	return uid;
	}

string& Utils::StringToLower(string& aString)
	{
	for(int count=0;count<aString.length();++count)
		{
		aString[count] = tolower(aString[count]);
		}
	return aString;
	}

bool Utils::FileExists(const string& aFile)
	{
	bool retVal = false;
	if(access(aFile.c_str(),0) == 0)
		{
		retVal = true;
		}
	return retVal;
	}

void Utils::TrimSpaces(std::string& aString)
	{
	char const* delims = " \t\r\n";

	// trim leading whitespace
	string::size_type  notwide = aString.find_first_not_of(delims);
	aString.erase(0,notwide);

	// trim trailing whitespace
	notwide = aString.find_last_not_of(delims);
	aString.erase(notwide+1);

	}