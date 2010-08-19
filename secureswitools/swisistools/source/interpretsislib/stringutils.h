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


/**
 @file STRINGUTILS.H
 @internalComponent
 @released
*/
#ifndef	STRINGUTILS_H
#define	STRINGUTILS_H

// System includes
#include <istream>
#include <list>
#include <vector>
#include <map>
#include <string>

// User includes
#include "symbiantypes.h"

// Constants
#ifndef __TOOLS2_LINUX__
const std::wstring KDirectorySeparator( L"\\" );
const std::wstring KDoubleSlashPathPrefix( L"\\\\" );
const std::string KDirectorySeparatorString( "\\" );
const std::string KDoubleSlashPathPrefixString( "\\\\" );
#else
const std::wstring KDirectorySeparator( L"/" );
const std::wstring KDoubleSlashPathPrefix( L"//" );
const std::string KDirectorySeparatorString( "/" );
const std::string KDoubleSlashPathPrefixString( "//" );
#endif

class StringUtils
	{
public:
	static int FirstInvalidDirSeparatorSize(std::wstring& aPath, std::wstring::size_type& aIndex);
    static std::wstring FixPathDelimiters( const std::string& aString );
    static std::string TrimWhiteSpace( const std::string& aString );
    static bool CheckForMatch( const std::string& aSearchFor, std::string& aLine );
    static std::string ToUpper( const std::string& aString );
	static std::wstring ToUpper( const std::wstring& aString );
	static std::string ToLower( const std::string& aString );
	static std::wstring ToLower( const std::wstring& aString );
    static bool IsLastCharacter( const std::wstring& aString, wchar_t aChar );
    static bool StartsWithDrive( const std::wstring& aText );
    static std::wstring MakePathFromSID( const std::wstring& aBasePath, TUint32 aSID );
	static std::wstring BuildSisRegistryFileName( const int aIndex );
	static std::wstring BuildControllerFileName( const int aIndex, const int aCtlIndex);
    static std::wstring EnsureDirectoryTerminated( const std::wstring& aDir );
	static bool WildcardCompare( const std::wstring& aWildCardFileName, const std::wstring& aFileName );
	// File parsing methods
	static std::wstring NameAndExt( const std::wstring& aFile );
	static std::wstring DriveAndPath( const std::wstring& aFile );
	static std::wstring Path( const std::wstring& aFile );
	static std::wstring Name( const std::wstring& aFile );
	static std::wstring Ext( const std::wstring& aFile );
    };


#endif
