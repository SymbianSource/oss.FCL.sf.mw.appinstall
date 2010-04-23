/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
* various utility functions
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __UTILS_H_
#define __UTILS_H_

// ===========================================================================
// INCLUDES
// ===========================================================================

#include "utility_interface.h"
#include <wchar.h>
#include "basetype.h"
#include <iostream>
 

enum TUtilsException
	{ErrNotEnoughMemory	=-1,
	ErrCannotOpenFile	=-2,
	ErrCannotReadFile	=-3,
	ErrCannotWriteFile	=-4,
	ErrCannotConvertFile=-5,
	ErrCertFileKeyFileMismatch = -6};

enum TEncodingScheme
	{
	EAscii	= 0,
	EUtf8	= 1,
	EUcs2LE	= 2,
	EUcs2BE	= 3,
	};

// ===========================================================================
// GLOBAL FUNCTIONS
// ===========================================================================

// is a file unicode
bool FileIsUnicode(LPCWSTR fileName, TEncodingScheme& encScheme);
// create a temporary file which is a UCS2 little endian UNICODE version of a
// big endian UNICODE file
LPWSTR ConvertUCS2FileToLittleEndianUnicode(LPCWSTR fileName);
// create a temporary file which is a UCS4 little endian UNICODE version of a
// big endian UNICODE file 
LPWSTR	ConvertUCS4FileToLittleEndianUnicode(LPCWSTR fileName);
// create a temporary file which is a UNICODE version of an MBCS file
LPWSTR ConvertFileToUnicode(LPCWSTR fileName, TEncodingScheme encScheme);
// get a temporary filename (in c:\temp\mks???\ with given file name)
LPWSTR TempFileName();
// Verifies if given buffer has correct UTF8 encoding
bool CorrectUTF8(LPSTR pNarrowBuf, DWORD dwNumBytes);
// Convert UCS2 file to UCS4 format
LPWSTR ConvertUCS2FileToUCS4(LPCWSTR fileName);

#endif
