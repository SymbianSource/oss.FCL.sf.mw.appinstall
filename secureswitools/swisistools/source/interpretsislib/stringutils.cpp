/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#pragma warning (disable: 4786)

#include "stringutils.h"

// System includes
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>

// User includes
#include "is_utils.h"
#include "logger.h"
#include "utility_interface.h"
#include "util.h"

// Constants
const int KInterpretSISExpectedSIDLength = 8;
const int KInterpretSISCtlIndexLength = 4;
const std::string KStringUtilsWhiteSpace = "\t  \r\n";

/**
 * Returns the size of the first occurance of an invalid directory separator.
 * @param aPath Path to be validated.
 * @param aIndex index from which the search begin. On function return this 
 * 				index will point to the illegal directory separator. 
 * @return 0 if path is valid. Else the number of character to be replaced.
 * e.g. /sys/bin/ should be replaced with \sys\bin\
 * and \\sys\\bin\\ should be replaced with \sys\bin\
 */ 
int StringUtils::FirstInvalidDirSeparatorSize(std::wstring& aPath, std::wstring::size_type& aIndex)
	{
	// If path semantics is correct (as needed by sisx library)
	// then the function will return 0
	int ret = 0; 
	int pos = 0;
	#ifndef __TOOLS2_LINUX__
	if((pos = aPath.find(L"//", aIndex)) != std::wstring::npos)
		{
		ret = 2;
		}
	#else
	if((pos = aPath.find(L"\\\\", aIndex)) != std::wstring::npos)
		{
		ret = 2;
		}
	#endif

	#ifndef __TOOLS2_LINUX__
	else if((pos = aPath.find(L"/", aIndex)) != std::wstring::npos)
		{
		ret = 1;
		}
	#else
		else if((pos = aPath.find(L"\\", aIndex)) != std::wstring::npos)
		{
		ret = 1;
		}
	#endif
	aIndex = pos;
	return ret;
	}

std::wstring StringUtils::FixPathDelimiters( const std::string& aString )
    {
	std::wstring ret = L"";
    char src[2048];
    strcpy(src, aString.c_str());
    //
	wchar_t* buf = NULL ;
	int len = ConvertMultiByteToWideChar(src, -1, NULL, 0);
	buf = new wchar_t[len+1];
	len = ConvertMultiByteToWideChar(src, -1, buf, len);

	ret = std::wstring( buf ,len);

	delete[] buf;
	//
    std::wstring::size_type idx = 0;
	while(len = FirstInvalidDirSeparatorSize(ret, idx))
        {
		ret.replace(idx, len, KDirectorySeparator);
        }
    //
    return ret.c_str();
    }


std::string StringUtils::TrimWhiteSpace( const std::string& aString )
	{
	const int start = aString.find_first_not_of( KStringUtilsWhiteSpace );
	if (start == std::string::npos)
        {
		return "";
        }
    //
	return aString.substr(start, 1 + aString.find_last_not_of( KStringUtilsWhiteSpace ) - start);
	}


bool StringUtils::CheckForMatch( const std::string& aSearchFor, std::string& aLine )
    {
    bool found = false;
    //
    if ( aLine.size() >= aSearchFor.size() )
        {
        std::string prefix = aLine.substr( 0, aSearchFor.size() );
        if ( prefix == aSearchFor )
            {
            found = true;
            aLine = aLine.substr( aSearchFor.size() );
            }
        }
    //
    return found;
    }


std::string StringUtils::ToUpper( const std::string& aString )
    {
	std::string ret( aString );
    //
	std::transform( ret.begin(), ret.end(), ret.begin(), toupper );
    //
    return ret;
    }

std::wstring StringUtils::ToUpper( const std::wstring& aString )
    {
	std::wstring ret( aString );
    //
	std::transform( ret.begin(), ret.end(), ret.begin(), toupper );
    //
    return ret;
    }


std::string StringUtils::ToLower( const std::string& aString )
    {
	std::string ret( aString );
    //
	std::transform( ret.begin(), ret.end(), ret.begin(), tolower );
    //
    return ret;
    }

std::wstring StringUtils::ToLower( const std::wstring& aString )
    {
	std::wstring ret( aString );
	std::transform( ret.begin(), ret.end(), ret.begin(), tolower );

    return ret;
    }

bool StringUtils::IsLastCharacter( const std::wstring& aString, wchar_t aChar )
    {
    bool isLast = false;
    //
    if ( aString.length() )
        {
        const wchar_t lastChar = aString[ aString.length() - 1 ];
        isLast = ( lastChar == aChar );
        }
    //
    return isLast;
    }


bool StringUtils::StartsWithDrive( const std::wstring& aText )
    {
    bool startsWithDrive = false;
    //
    if ( aText.length() >= 3 )
        {
        const std::string prefix = ToUpper( wstring2string( aText.substr( 0, 3 ) ) );
        //
        const char drive = prefix[ 0 ];
        const char colon = prefix[ 1 ];
        const char backslash = prefix[ 2 ];
        //
        #ifndef __TOOLS2_LINUX__
        if  ( colon == ':' && backslash == '\\' )
		#else
		if  ( colon == ':' && (backslash == '/' || backslash == '\\'))
		#endif
            {
            startsWithDrive = ( drive >= 'A' && drive <= 'Z' ) || (drive == '!') ;
            }
        }
    //
    return startsWithDrive;
    }


std::wstring StringUtils::MakePathFromSID( const std::wstring& aBasePath, TUint32 aSID )
    {
    // Must make sure that the directory is eight characters long
    // since other parts of the code assume this (or check for it).
	std::wstringstream stream;
	stream << std::hex << aSID;
    //
    std::wstring ret = stream.str();
    //
    while( ret.length() < KInterpretSISExpectedSIDLength )
        {
        ret.insert( 0, L"0" );
        }
    // Add trailing slash
  	ret.append( L"\\" );
    //
  	// Convert it to a local path
	ConvertToLocalPath( ret, aBasePath );
    return ret;
    }


std::wstring StringUtils::BuildSisRegistryFileName( const int aIndex )
{
	std::wstringstream stream;
	stream << std::hex << aIndex;

	std::wstring ret = stream.str();
	
	while( ret.length() < KInterpretSISExpectedSIDLength )
    {
        ret.insert( 0, L"0" );
    }

	ret.append( L".reg" );

	return ret;
}


std::wstring StringUtils::BuildControllerFileName( const int aIndex, const int aCtlIndex)
{
	std::wstringstream stream, stream2;
	stream << std::hex << aIndex;
	stream2 << std::hex << aCtlIndex;

	std::wstring ret = stream.str();
	std::wstring ret2 = stream2.str();

	while( ret.length() < KInterpretSISExpectedSIDLength )
    {
        ret.insert( 0, L"0" );
    }

	while( ret2.length() < KInterpretSISCtlIndexLength )
    {
        ret2.insert( 0, L"0" );
    }

	ret.append(L"_");
	ret += ret2 + (L".ctl");

	return ret;
}


std::wstring StringUtils::EnsureDirectoryTerminated( const std::wstring& aDir )
    {
    std::wstring ret = aDir;
    //
    if ( ret.length() == 0 )
        {
        ret.insert( 0, KDirectorySeparator );
        }
    else if ( ret[ ret.length() - 1 ] != KDirectorySeparator[ 0 ] )
        {
        ret.append( KDirectorySeparator );
        }
    //
    return ret;
    }

/**
 Compares a candidate string to a wildcard string - one containing any number of the wildcard characters '*' and '?'
 e.g. z:\foo\bar\lang*.txt with z:\foo\bar\lang35.txt.

 The function returns TRUE if a match is found, or FALSE.
 */
bool StringUtils::WildcardCompare(const std::wstring& aWildCardFileName, const std::wstring& aFileName)
{
	#ifdef __TOOLS2_LINUX__
	// In case the incoming data is a DB entry then it will be having windows
	// specific paths due to the consistency of DB contents across WINDOWS
	// and LINUX. So, we need to convert them to LINUX paths and then compare.

	std::wstring::size_type idx = 0;

	while( (idx = aWildCardFileName.find(L"\\", idx)) != std::wstring::npos)
        {
		aWildCardFileName.replace( idx, 1, L"/" );
        }

	idx = 0;

	while( (idx = aFileName.find(L"\\", idx)) != std::wstring::npos)
        {
		aFileName.replace( idx, 1, L"/" );
        }
	#endif

	aWildCardFileName = StringUtils::ToLower(aWildCardFileName);
	aFileName = StringUtils::ToLower(aFileName);

	std::wstring::const_iterator wildCurr = aWildCardFileName.begin();
	std::wstring::const_iterator wildEnd = aWildCardFileName.end();

	std::wstring::const_iterator fileCurr = aFileName.begin();
	std::wstring::const_iterator fileEnd = aFileName.end();

	std::wstring::const_iterator currentPos = NULL;
	std::wstring::const_iterator afterPos = NULL;

	// handle the '?' wildcard
	while ((fileCurr != fileEnd) && (*wildCurr != L'*'))
		{
		if ((*wildCurr != *fileCurr) && (*wildCurr != L'?'))
			{
			// the current character is different (and not represented by ? or *)
			return false;
			}

		if (wildCurr != wildEnd)
			wildCurr++;

		if (fileCurr != fileEnd)
			fileCurr++;

		}

	// encountered the '*' wildcard
	while (fileCurr != fileEnd)
		{
		if (*wildCurr == L'*')
			{

			// get the next char after the '*'
			if (wildCurr != wildEnd)
				wildCurr++;

			if (wildCurr == wildEnd)
				{
				// abcd* - abcdefg. Matches the remaining characters
				return true;
				}

			afterPos = wildCurr;

			if (fileCurr != fileEnd)
				currentPos = fileCurr++;

			}
		// keep iterating until we come the character which starts after the '*'
		else if ((*wildCurr == *fileCurr) || (*wildCurr == L'?')) 
			{
			if (wildCurr != wildEnd)
				wildCurr++;

			if (fileCurr != fileEnd)
				fileCurr++;
			} 
		else
			{
			wildCurr = afterPos;

			if (currentPos != fileEnd)
				fileCurr = currentPos++;
			}
		}

	while (*wildCurr == L'*') 
		{
		if (wildCurr != wildEnd)
			wildCurr++;
		}

	// if we end up with an empty wild card string, we have a match
	return (*wildCurr == *wildEnd);
}

/**
Gets the filename and extension.
 
This is in the form:

filename.ext

@return The filename and extension.
*/
std::wstring StringUtils::NameAndExt( const std::wstring& aFile )
{
#ifndef __TOOLS2_LINUX__
	int pos = aFile.find_last_of(L"\\");
#else
	// We should also check for backward slash since the caller could be
	// passing a string containing a windows-specific paths within LINUX.
	//
	//    One instance being - an SCR database entry under LINUX, which has
	//    the windows specific paths of the installed files to maintain
	//    consistency across WINDOWS and LINUX platforms.

	int pos = aFile.find_last_of(L"/");
	pos = pos == std::wstring::npos ? aFile.find_last_of(L"\\") : pos;
#endif
	if (pos == std::wstring::npos)
	{
		return L"";
	}
	else 
    {
		std::wstring extension(aFile.substr(pos+1));
		return extension;
    }
}

/**
Gets the drive letter and path.
 
This is in the form
 
drive-letter:\\path\\
 
Note that the drive letter is folded

@return The drive and path.
*/
std::wstring StringUtils::DriveAndPath( const std::wstring& aFile )
{
#ifndef __TOOLS2_LINUX__
		int pos = aFile.find_last_of(L"\\");
#else
		int pos = aFile.find_last_of(L"/");
#endif
	if (pos == std::wstring::npos)
	{
		return L"";
	}
	else 
    {
		std::wstring extension(aFile.substr(0,pos+1));
		return extension;
    }
}

/**
Gets the path.
 
The path is in the form:
 
\\path\\
 
@return The path. It always begins and ends in a backslash.
*/
std::wstring StringUtils::Path( const std::wstring& aFile )
{
#ifndef __TOOLS2_LINUX__
		int firstPos = aFile.find_first_of(L"\\");
		int lastPos = aFile.find_last_of(L"\\");
#else
		int firstPos = aFile.find_first_of(L"/");
		int lastPos = aFile.find_last_of(L"/");
#endif
	
	if (lastPos >= firstPos)
	{
		std::wstring path(aFile.substr(firstPos,lastPos-1));
		return path;
	}

	return L"";
}

/**
Gets the filename.
 
This is in the form
 
filename
 
@return The filename.
*/
std::wstring StringUtils::Name( const std::wstring& aFile )
{
#ifndef __TOOLS2_LINUX__
	int startPos = aFile.find_last_of(L"\\");
#else
	int startPos = aFile.find_last_of(L"/");
#endif
	
	int endPos = aFile.find_last_of(L".");

	if (endPos > startPos)
    {
		std::wstring extension(aFile.substr(startPos+1,endPos));
		return extension;
    }

	return L"";
}

/**
Gets the extension.
 
This is in the form:
 
.extension
 
@return The extension and preceding dot.
*/
std::wstring StringUtils::Ext( const std::wstring& aFile )
{
	int pos = aFile.find_last_of(L".");
	if (pos == std::wstring::npos)
	{
		return L"";
	}
	else 
    {
		std::wstring extension(aFile.substr(pos));
		return extension;
    }
}