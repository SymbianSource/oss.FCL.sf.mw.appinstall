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
* Various utility functions for makekeys
*
*/


/**
 @file 
 @internalComponent 
*/

#ifndef __UTILS_H_
#define __UTILS_H_

// ===========================================================================
// INCLUDES
// ===========================================================================

#include <windows.h>
#include <tchar.h>
#include <string.h>
#include "openssl/err.h"
#include "openssl/objects.h"

#define SUCCESS					1
#define FAIL					0

#define ENTRY_COUNT 8
#define MAXBUF 512

struct entry
	{
	char key[MAXBUF];
	_TCHAR value[MAXBUF];
	};

struct entry_pack
	{
	struct entry entries[ENTRY_COUNT];
	int num;
	};


_TCHAR* RemoveSpaces (_TCHAR* aStr);
char*   IsValid (_TCHAR* aTk);
int     SplitString (_TCHAR* aToken, _TCHAR** aStrFirst, _TCHAR** aStrSecond);
void    InitEntryPack(struct entry_pack** aEntPack);

enum TUtilsException
	{ErrNotEnoughMemory	=-1};

enum TErrType
	{EGeneric,
	 EOPENSSL,
	 EMSCrypto};

// ===========================================================================
// GLOBAL FUNCTIONS
// ===========================================================================

// convert a UNICODE string to MBCS
LPSTR	MakeMBCSString(LPCWSTR uniStr, UINT codePage, DWORD& length);
int		MakeMBCSString(LPCWSTR uniStr, UINT codePage, LPSTR* mbStr, int& length);
// convert a MBCS string to UNICODE
LPWSTR MakeUnicodeString(LPSTR mbcsStr, UINT codePage);

// print error information
void	PrintError(char* aMsg, TErrType aErrType, bool aVerbose, const char* aFileName, int aLineNum);

//Format a DN String
void DoFormatted(_TCHAR* aDN, struct entry_pack** aEntPack);


#endif
