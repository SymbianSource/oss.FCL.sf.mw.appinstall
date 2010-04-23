/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* more utility functions.
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#ifdef _DEBUG
#define GENERATE_ERRORS
#endif

#ifdef __TOOLS2_WINDOWS__
#include <windows.h>
#include <tchar.h> 
#include <io.h>
#define PATHMAX		_MAX_PATH
#define RESULT		0xFFFFFFFF
#endif //__TOOLS2_WINDOWS__


// Defined macros for compatibility reason
#ifdef __TOOLS2_LINUX__
#define PATHMAX							PATH_MAX
#define	HANDLE							void*
#define LPCWSTR							const wchar_t*
#define LPSTR							char*
#define	UINT							unsigned int
#define DWORD							unsigned long
#define WORD							unsigned short
#define WCHAR							wchar_t
#define _TCHAR							wchar_t
#define LPWSTR							wchar_t*
#define BOOL							int
#define	LONG							long
#define	LPVOID							void*
#define _fileno							fileno
#define BYTE							unsigned char
#define CHAR							char
#define GENERIC_READ					(0x80000000L)
#define GENERIC_WRITE                   (0x40000000L)
#define STD_OUTPUT_HANDLE				(unsigned long)-11
#define INVALID_HANDLE_VALUE			(HANDLE)-1
#define TRUE							1
#define FALSE							0
#define FILE_BEGIN						0
#define FILE_CURRENT					1
#define FILE_END						2
#define CREATE_NEW						1
#define CREATE_ALWAYS					2
#define OPEN_EXISTING					3
#define FILE_ATTRIBUTE_NORMAL           0x00000080
#define	_S_IWRITE						222
#define	_S_IREAD						444
#define	CP_OEMCP						0
#define	CP_UTF8							0
#define _dup							dup
#define _dup2							dup2
#define	RESULT							-1

// for iswdigit
#include <wctype.h>
#include <unistd.h>

#endif // __TOOLS2_LINUX__

#include "symbiantypes.h"

#endif // __COMMON_TYPES_H__
