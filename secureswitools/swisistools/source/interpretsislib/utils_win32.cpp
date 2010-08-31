/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#define WIN32_LEAN_AND_MEAN 1

#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif // _MSC_VER

#include <windows.h>
#include <fstream>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

#include "logger.h"
#include "stringutils.h"
#include "is_utils.h"


int GetStat(const std::wstring& aFile, struct stat* s)
{
	return _wstat(aFile.c_str(), reinterpret_cast<struct _stat*>(s));
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


bool FileExists(const std::wstring& aFile)
{
	struct stat x;
	int err = GetStat(aFile,&x );
	return err == 0; 
}


bool RemoveFile(const std::wstring& aFile)
{
	return _wunlink(aFile.c_str()) == 0;
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

bool OpenFile(const std::wstring& aFile, std::fstream& aStream,
			  std::ios_base::open_mode aMode)
{
	std::string s;
	aStream.open(Ucs2ToUtf8(aFile, s).c_str(), aMode);
	return aStream.good();
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
		if (!CreateDirectory(dir.c_str(),0) && GetLastError() != ERROR_ALREADY_EXISTS)
            {
			int lastErr = GetLastError();
  	  	    (void) lastErr;
			return false;
            }

		} while (index != std::wstring::npos);

    return true;
	}

#ifndef _MSC_VER

void GetDirContents(const std::wstring& path, 
					std::list<std::wstring>& contents)
	{
	_WDIR* currDir =  _wopendir(path.c_str());
	if(currDir == NULL)
		{
		return;
		}
	
	_wdirent* currElem = _wreaddir(currDir);
	
	while (currElem)
		{
		contents.push_back(currElem->d_name);
		currElem = _wreaddir(currDir);
		}
	_wclosedir(currDir);
	}

#else

// missing structures for the posix directory calls 
struct dirent 
{
	wchar_t* d_name;
};

struct DIR 
{
	WIN32_FIND_DATA iWinData;
	HANDLE          iHandle;
	dirent          iEntry;
	std::wstring     iName;	
};

DIR* opendir(const wchar_t* dirname) 
	{
	static DIR dir;
	
	std::wstring s(dirname);
	std::wstring wildcard = strchr("/\\", s[s.length()-1]) ? L"*" : L"/*";
	dir.iName = s.append(wildcard);
	dir.iHandle = FindFirstFile(dir.iName.c_str(), &dir.iWinData);
	if (dir.iHandle != INVALID_HANDLE_VALUE )
		return &dir;
	else
		return 0;
	};


dirent* readdir(DIR* aDir)
{
	int result = FindNextFile(aDir->iHandle, &aDir->iWinData);
	if (result != 0)
	{
		aDir->iEntry.d_name = aDir->iWinData.cFileName;
		return &aDir->iEntry;
	}
	return 0;
}

void closedir(DIR* aDir)
{
	FindClose(aDir->iHandle);
	aDir = 0;
}


void GetDirContents(const std::wstring& path, 
					std::list<std::wstring>& contents)
{

	DIR* currDir =  opendir(path.c_str());
	
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
			contents.push_back(currElem->d_name);
		}
	}
}

#endif



bool CheckSisRegistryDirPresent(const std::wstring& aDrivePath, TUint32 aUid)
{
    std::wstring ret = StringUtils::MakePathFromSID( aDrivePath + L"/sys/install/sisregistry/", aUid );

	return IsDirectory(ret);
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
/*
//std::string Utils::wstring2string (const std::wstring& aWide)
std::string wstring2string (const std::wstring& aWide)
	{
	int max = WideCharToMultiByte(CP_OEMCP,0,aWide.c_str(),aWide.length(),0,0,0,0);
	std::string reply;
	if (max > 0 )
		{
		char* buffer = new char [max];
		try
			{
			WideCharToMultiByte(CP_OEMCP,0,aWide.c_str(),aWide.length(),buffer,max,0,0);
			reply = std::string (buffer, max);
			}
		catch (...)
			{
			}
		delete [] buffer;
		}
	return reply;
	}

//std::wstring Utils::string2wstring (const std::string& aNarrow)
std::wstring string2wstring (const std::string& aNarrow)
	{
	int max = MultiByteToWideChar(CP_OEMCP,0,aNarrow.c_str(),aNarrow.length(),0,0);
	std::wstring reply;
	if (max > 0 )
		{
		wchar_t* buffer = new wchar_t [max];
		try
			{
			MultiByteToWideChar(CP_OEMCP,0,aNarrow.c_str(),aNarrow.length(),buffer,max);
			reply = std::wstring (buffer, max);
			}
		catch (...)
			{
			}
		delete [] buffer;
		}
	return reply;
	}
*/
