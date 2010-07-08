/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#include "adornedutilities.h"
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>

#include "stringutils.h"
#include "symbiantypes.h"
#include "logger.h"
#include "is_utils.h"

const std::wstring KAdornedWildCharString = L"{????????}";
const int KFileNameUnadornedPartLength = 10;


void GetUnadornedFileName(const std::wstring& aAdornedFilename, std::wstring& aUnadornedFilename)
{
	aUnadornedFilename = aAdornedFilename;
	int length = aAdornedFilename.length();

	// check to see if the name is adorned. If so, remove the version number
	int startVersion = aAdornedFilename.find('{');
	if (startVersion != 0)
	{
		int endVersion = aAdornedFilename.find('}');
		if ((endVersion != 0) && (endVersion - startVersion == (KFileNameUnadornedPartLength - 1)))
		{
			// get unadorned version of the file
			aUnadornedFilename = aAdornedFilename.substr(0,startVersion);

			// for names ending with } i.e: DummyFile{12345678} nothing is to be appended to 
			// aUnadornedFilename in such a case aAdornedFilename.Mid(endVersion+1) would cause a crash
			if (endVersion < (length - 1))
			{
				aUnadornedFilename.append(aAdornedFilename.substr(endVersion + 1,length));
			}
		}
	}
}

#ifndef __TOOLS2_LINUX__
bool IsAdornedVariationOf(const std::wstring& aFileName1, const std::wstring& aFileName2)
#else
bool IsAdornedVariationOf(const std::wstring& aFileName1, const std::wstring& aFileName2, const std::wstring& aDrivePath)
#endif
{
	std::wstring unadornedFileName1;
	GetUnadornedFileName(aFileName1,unadornedFileName1);

	std::wstring unadornedFileName2;
	GetUnadornedFileName(aFileName2,unadornedFileName2);

	#ifdef __TOOLS2_LINUX__
	ConvertToLocalPath( unadornedFileName1, aDrivePath );
	ConvertToLocalPath( unadornedFileName2, aDrivePath );
	#endif

  	// Check whether filename2 is a variant of filename1
  	// e.g: d:\sys\bin\DummyDll{000A0001}.dll is considered a variant of c:\sys\bin\DummyDll.dll 
  	// because they both break down to \sys\bin\DummyDll.dll
	std::wstring fileName1Path(StringUtils::Path(unadornedFileName1));
	std::wstring fileName1NameAndExt(StringUtils::NameAndExt(unadornedFileName1));

	std::wstring fileName2Path(StringUtils::Path(unadornedFileName2));
	std::wstring fileName2NameAndExt(StringUtils::NameAndExt(unadornedFileName2));

	return ( !wcscmp(fileName1Path.c_str(), fileName2Path.c_str()) && !wcscmp(fileName1NameAndExt.c_str(), fileName2NameAndExt.c_str()) );
}
	
void FindAllAdornedVariants(const std::wstring& aSearchNameWild, const std::wstring& aSearchPath, std::list<std::wstring>& aAdornedFileNamesFound, const DrivesMap& aDriveMap)
{
	DrivesMap::const_iterator it = aDriveMap.begin();
	DrivesMap::const_iterator end = aDriveMap.end();

	for ( ; it != end ; ++it)
	{
		// drive to search on
		int disk = tolower(it->first);
		std::wstring drive = L"$:";
		drive[0] = disk;

		std::wstring searchPath(aSearchPath);

		// actual readable directory
		std::wstring localDir = it->second->iDir;

		// using ROM/ROFS logs for the z drive, searching for adorned variants is handled later.
		if (disk == 'z' && localDir.empty())
			continue;

		// convert to the local path and see if the file exists on the drive
		ConvertToLocalPath( searchPath, localDir );

		// search this directory 
		std::list<std::wstring> dirContents;
		GetDirContents(searchPath, dirContents);

		std::list<std::wstring>::iterator curr = dirContents.begin();
		std::list<std::wstring>::iterator end = dirContents.end();
		while (curr != end)
		{
			std::wstring dirFile(*curr);

			if (StringUtils::WildcardCompare(aSearchNameWild,dirFile))
			{
				// found an adorned file, add to the list of adorned names found
				std::wstringstream foundFile;
				foundFile << drive << aSearchPath << dirFile;
				aAdornedFileNamesFound.push_back(foundFile.str());
			}

			++curr;
		}
	}
}

void GenerateSearchNameWild(const std::wstring& aFileName, std::wstring& aSearchNameWild)
{
	std::wstring unadornedFileName;
	GetUnadornedFileName(aFileName, unadornedFileName);

	aSearchNameWild = StringUtils::Name(unadornedFileName);
	aSearchNameWild.append(KAdornedWildCharString);
	aSearchNameWild.append(StringUtils::Ext(unadornedFileName));
}
