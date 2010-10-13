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


#include<errno.h>
#include<fstream>
#include<iostream>
#include<sstream>

#include"utility_interface.h"

bool FileExists(const std::wstring& aFile)
{
	return true;
}
bool RemoveFile(const std::wstring& aFile)
{
	return true;
}
bool CreateFile(const std::wstring& aFile)
{
	return true;
}

int GetStat(const std::wstring& aFile, struct stat* s)
{
	std::string str;
 	return stat(Ucs2ToUtf8(aFile, str).c_str(), s);
}

void GetDirContents(const std::wstring& path, 
					std::list<std::wstring>& contents)
{

	std::string utfString;
	DIR* currDir =  opendir(Ucs2ToUtf8(path, utfString).c_str());
	
	while (currDir)
	{
		dirent* currElem = readdir(currDir);
		if (currElem == 0)
		{
			closedir(currDir);
			currDir = 0;
		}
		else
		{
			std::wstring ucsString;
			contents.push_back(Utf8ToUcs2(currElem->d_name, ucsString));
		}
	}
}

bool IsDirectory(std::wstring& aStr)
{
    bool ret = false;
    //
    if ( aStr.length() > 0 )
        {
	    struct stat x;
        //
	    int err = GetStat(aStr,&x );
        //
        if ( err != 0 )
            {
            wchar_t lastChar = aStr[ aStr.length() - 1 ];

            if ( lastChar == L'\\' || lastChar == '/' )
                {
                // Try again, but without the trailing backslash
                std::wstring path = aStr.substr( 0, aStr.length() - 1 );
                err = GetStat( path, &x );
                if ( err == 0 && (x.st_mode & S_IFDIR) > 0 )
                    {
                    aStr = path;
                    }
                }
            }
        //
        ret = ( err == 0 && (x.st_mode & S_IFDIR) > 0 );
        }
    //
	return ret;
}


bool MakeDir(const std::wstring& aDir)
{
	// This function makes all the directories in a subdirectory hierarchy.
  	// If the first character in aDir is a backslash, then we assume it refers
  	// to the root of a drive, hence we start the index below at 1 to skip over
  	// this initial root directory.
    size_t index = aDir.find(L':', 0);
    if(std::wstring::npos == index)
    	{
    	index = 0;
    	}
    else
    	{// Skip creation of root directory
    	index = aDir.find(L'\\', index);
    	}
	do
		{
    	index += 1;
		// Try to make each directory in the path. If ERR_ALREADY_EXISTS is returned
  	  	// then this is okay. Other errors are fatal.
		index = aDir.find(L'\\', index);
		std::wstring dir = aDir.substr( 0, index );
		if(dir == L".")
			{
			continue;
			}
		if (!CreateDir(dir.c_str()) && GetErrorValue()  != ERROR_ALREADY_EXISTS)
            {
			int lastErr = GetErrorValue();
  	  	    (void) lastErr;
			return false;
            }

		} while (index != std::wstring::npos);

    return true;
	}


bool OpenFile(const std::wstring& aFile, std::fstream& aStream,
			  std::ios_base::open_mode aMode)
{
	std::string s;
	aStream.open(Ucs2ToUtf8(aFile, s).c_str(), aMode);
	return aStream.good();
}


int GetAugmentationsNumber(const std::wstring& aFile)
{
	for (int index = 1; index < 0xFFFFFFFF; index++)
	{
		std::wstringstream s2;
		s2 << std::hex << index;

		// e.g. 12345678_x.sis
		std::wstring fileName = aFile + s2.str() + L".sis";

		if ( !FileExists(fileName) )
		{
			// return the next available index
			return index;
		}
	}

	return 0;
}


bool CheckSisRegistryDirPresent(const std::wstring& aDrivePath, TUint32 aUid)
{
    std::wstring ret = StringUtils::MakePathFromSID( aDrivePath + L"/sys/install/sisregistry/", aUid );

	return IsDirectory(ret);
}

void RemoveHashForFile(const std::wstring& aFile, const int aDriveLetter, const std::wstring& aPath)
{
	std::wstring hashdir = L"$:\\sys\\hash\\";
	std::wstring basename = aFile.substr( aFile.rfind( KDirectorySeparator ) + 1) ;
	if (basename.size() == 0)
	{
		basename = aFile.substr(aFile.rfind(L"\\"));
	}

	hashdir[0] = aDriveLetter;
	std::wstring hashFile = aPath + L"\\sys\\hash\\" + basename;
	if (FileExists(hashFile))
	{
		RemoveFile(hashFile);
	}
}
