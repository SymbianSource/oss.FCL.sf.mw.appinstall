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
* Various utility functions
* INCLUDES
*
*/


#include "utils.h"
#include <stdio.h>
// ===========================================================================
// GLOBAL UTILS FUNCTIONS
// ===========================================================================

LPSTR MakeMBCSString(LPCWSTR uniStr, UINT codePage, DWORD& length)
// Convert a UNICODE string to a multi-byte string
	{
	LPSTR mbStr;
	// get num unicode chars required
	DWORD len = WideCharToMultiByte(codePage, 0, uniStr, length, NULL, 0, NULL, NULL);
	mbStr = new CHAR[len+1];
	if (!mbStr) throw ErrNotEnoughMemory;
	// convert
	WideCharToMultiByte(codePage, 0, uniStr, length, mbStr, len, NULL, NULL);
	mbStr[len]='\0';
	length=len;

	return mbStr;
	}

LPWSTR  MakeUnicodeString(LPSTR mbcsStr, UINT codePage)
// Convert a multi-byte string to a UNICODE string
{
	int retVal=0, len;
	LPWSTR uniStr;

	if(mbcsStr == NULL) return NULL;

	len = MultiByteToWideChar(codePage, 0, mbcsStr, strlen(mbcsStr)+1, NULL, 0);
	uniStr = new WCHAR[len+1];
	retVal = MultiByteToWideChar(codePage, 0, mbcsStr, strlen(mbcsStr)+1, uniStr, len);

	return uniStr;
}

void PrintError(char* aMsg, TErrType aErrType, bool aVerbose, const char* aFileName, int aLineNum)
//Print error messages in the generation of a key or certificate
{
   	if(!aMsg)
		return;

	fprintf(stderr, "\n** %s ", aMsg);

	if(aVerbose)
	{	
		if(aErrType == EOPENSSL)
		{
			fprintf(stderr, " %s:%i\n", aFileName, aLineNum);
			ERR_print_errors_fp(stderr);
		
		}else if(aErrType == EMSCrypto)
		{
			LPVOID lpMsgBuf = NULL;
   
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
						   | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), 0, 
						   (LPTSTR)&lpMsgBuf, 0, NULL);
     
			fprintf(stderr, "%s:%i %s\n", aFileName, aLineNum, (LPCTSTR)lpMsgBuf);

			// Free the buffer.
			LocalFree( lpMsgBuf ); 

		} else
		{
			fprintf(stderr, "%s:%i\n", aFileName, aLineNum);
		}
	}
}

// ===========================================================================
// These functions format a DN string
// ===========================================================================

// ===========================================================================
// CONSTANTS
// ===========================================================================

_TCHAR TSN_commonName[]				= _T("CN");
_TCHAR TSN_countryName[]			= _T("C");
_TCHAR TSN_countryNameOld[]			= _T("CO");
_TCHAR TSN_localityName[]			= _T("L");
_TCHAR TSN_stateOrProvinceName[]	= _T("ST");
_TCHAR TSN_organizationName[]		= _T("O");
_TCHAR TSN_organizationNameOld[]	= _T("OR");
_TCHAR TSN_organizationalUnitName[]	= _T("OU");
_TCHAR TSN_email[]					= _T("EM");

void DoFormatted(_TCHAR* aDN, entry_pack** aEntPack)
//Given Distinguished Name string is parsed and filled into entry pack structure
	{
	
	_TCHAR* pToken  = NULL;
	_TCHAR* pLstr   = NULL;
	_TCHAR* pRstr   = NULL;
    char*   pKeyWord = NULL;

	*aEntPack = (struct entry_pack*)malloc(sizeof(struct entry_pack ));
	InitEntryPack(aEntPack);

	_TCHAR sep[] = _T("=");
	pToken = _tcstok(aDN, sep);
	
	if(!(pKeyWord = IsValid(pToken)))
		{
		return;
		}
	
	int i = 0;
	strcpy((*aEntPack)->entries[i].key, pKeyWord);
	pToken = _tcstok(NULL, sep);

	bool bMayBadParam = false;
	int  retVal		  = 0;

	while (pToken != NULL)
		{	
		if(bMayBadParam)
			{
			i = 0;
			break;
			}

		retVal = SplitString(pToken, &pLstr, &pRstr);
		
		if(retVal)
			{	
			_tcscpy((*aEntPack)->entries[i++].value, RemoveSpaces(pLstr));
			if(pRstr)
				{
				strcpy((*aEntPack)->entries[i].key, IsValid(pRstr));
				}
			}
		else
			{	
			_tcscpy((*aEntPack)->entries[i++].value, RemoveSpaces(pLstr));
			
			if(!pRstr)
				{	
				bMayBadParam = true;
				}
			}
		
		pToken = _tcstok(NULL, sep);
		}
	
		(*aEntPack)->num = i;

	}


_TCHAR* RemoveSpaces(_TCHAR* aStr)
//Removes unnecessary spaces from a given string
	{

	LPCTSTR lpszTargetList = _T(" ");
	LPTSTR  lpsz = aStr;
	LPTSTR  lpszLast = NULL;

	//Trim Left
	while (*lpsz != '\0')
		{
		if (_tcschr(lpszTargetList, *lpsz) == NULL)
			break;
		lpsz = _tcsinc(lpsz);
		}

	if (lpsz != aStr)
		{
		int nDataLength = _tcslen(aStr) - (lpsz - aStr);
		wmemmove(aStr, lpsz, (nDataLength+1)*sizeof(TCHAR));
		}

	lpsz = aStr;
	lpszLast = NULL;

	//Trim Right
	while (*lpsz != '\0')
		{
		if (_tcschr(lpszTargetList, *lpsz) != NULL)
			{
			if (lpszLast == NULL)
				lpszLast = lpsz;
			}
		else
			lpszLast = NULL;
		lpsz = _tcsinc(lpsz);
		}

	if (lpszLast != NULL)
		{
		// truncate at left-most matching character
		*lpszLast = '\0';
		}
	return aStr;
	}

char* IsValid(_TCHAR* aTk)
//Checks whether a found string is a valid distinguished name key
	{

	if(!aTk)
		return NULL;

	if(!_tcscmp(aTk, TSN_commonName))
		{
		return LN_commonName;
		}
	else if(!_tcscmp(aTk, TSN_countryName) || !_tcscmp(aTk, TSN_countryNameOld))
		{
		return LN_countryName;
		}
	else if(!_tcscmp(aTk, TSN_localityName))
		{
		return LN_localityName;
		}
	else if(!_tcscmp(aTk, TSN_stateOrProvinceName))
		{
		return LN_stateOrProvinceName;
		}
	else if(!_tcscmp(aTk, TSN_organizationName) || !_tcscmp(aTk, TSN_organizationNameOld))
		{
		return LN_organizationName;
		}
	else if(!_tcscmp(aTk, TSN_organizationalUnitName))
		{
		return LN_organizationalUnitName;
		}
	else if(!_tcscmp(aTk, TSN_email))
		{
		return LN_pkcs9_emailAddress;
		}
	else
		{
		return NULL;
		} 
	
	}


int SplitString(_TCHAR* aToken, _TCHAR** aStrFirst, _TCHAR** aStrSecond)
//A given string is divided in two parts if applicable. First part is DN value
//second part is key value.
	{
	int i = 0;
	_TCHAR *pTmp = NULL;

	pTmp = aToken;
	pTmp = pTmp + (_tcslen(pTmp)-2);

	if(IsValid(pTmp) && (aToken[(pTmp - aToken)-1] == 32))
		{
		*aStrFirst = aToken;
		aToken[_tcslen(aToken)-3] = '\0';
		
		*aStrSecond = pTmp;
		} 
	else 
		{
		pTmp = aToken;
		pTmp = pTmp + (_tcslen(pTmp)-1);

		if(IsValid(pTmp) && (aToken[(pTmp - aToken)-1] == 32))
			{
			*aStrFirst = aToken;
			aToken[_tcslen(aToken)-2] = '\0';
		
			*aStrSecond = pTmp;
			} 
		else
			{
			*aStrFirst = aToken;
			*aStrSecond = NULL;
			return 0;
			}
		}

	return 1;
	}

void InitEntryPack(struct entry_pack** aEntPack)
//Initialize an entry pack structure pointer
	{
	memset(*aEntPack,0, sizeof(struct entry_pack));

	(*aEntPack)->num = 0;
	}



