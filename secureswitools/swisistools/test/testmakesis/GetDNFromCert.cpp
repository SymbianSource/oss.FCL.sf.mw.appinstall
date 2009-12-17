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
* This file is implementation of GETDNFROMCERT.EXE which read a given certificate
* and writes its DN name entries into an INI file with given name.
*
*/


#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include "../openssl/import/inc/openssl/x509.h"
#include "../openssl/import/inc/openssl/err.h"
#include "../openssl/import/inc/openssl/pem.h"

int GetNameEntry(X509_NAME *pName, char *pKey, LPWSTR *pNameEntry);

int _tmain(int argc, _TCHAR* argv[])
{
	
	FILE *fp = NULL;
	X509 *x509 = NULL;
	LPWSTR pwNameEnt = NULL;
	HANDLE hFile;
	WORD BOM=0xfeff;
	DWORD cBytes; 

	if( argc < 3 )
		return 0;
	
	HANDLE hndl = GetStdHandle(STD_OUTPUT_HANDLE);

	TCHAR lpFilename[MAX_PATH] = {0};
	GetModuleFileName(NULL,lpFilename,MAX_PATH);
	TCHAR* pLast = _tcsrchr(lpFilename,_T('\\'));
	if ( pLast )
	{
		pLast++;
		*pLast = 0;
		_tcscat(lpFilename,argv[2]);
	}

	hFile = CreateFile(lpFilename,  
                           GENERIC_WRITE,  
                           0,  
                           NULL,  
                           CREATE_ALWAYS,  
                           FILE_ATTRIBUTE_NORMAL |
                           FILE_FLAG_SEQUENTIAL_SCAN,
                           NULL); 
	if(hFile == INVALID_HANDLE_VALUE)
	{
		WriteConsole(hndl, argv[2], wcslen(argv[2]), 0, 0);
		return 0;
	}
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	if(!WriteFile(hFile, &BOM, sizeof(BOM), &cBytes, NULL))
		return 0;
	CloseHandle(hFile);

	if((fp = _tfopen(argv[1], _T("rb"))) == NULL)
	{
		_tprintf(_T("\nFile couldn't be opened:"));
		WriteConsole(hndl, argv[1], wcslen(argv[1]), 0, 0);
		_tprintf(_T("\n"));
		return 0;
	}

	OPENSSL_add_all_algorithms_conf();
	ERR_load_crypto_strings();

	if(PEM_read_X509 (fp , &x509 ,0 ,NULL) == NULL)
	{
		ERR_print_errors_fp(stderr);
		return 0;
	}
	
	if(GetNameEntry(X509_get_subject_name (x509), LN_commonName, &pwNameEnt))
	{	
		WritePrivateProfileString (_T("Section1"), _T(SN_commonName),  pwNameEnt, lpFilename); 
		FreeMemory(pwNameEnt);
	}
	if(GetNameEntry(X509_get_subject_name (x509), LN_organizationalUnitName, &pwNameEnt))
	{	
		WritePrivateProfileString (_T("Section1"), _T(SN_organizationalUnitName),  pwNameEnt, lpFilename); 
		FreeMemory(pwNameEnt);
	}
	if(GetNameEntry(X509_get_subject_name (x509), LN_organizationName, &pwNameEnt))
	{	
		WritePrivateProfileString (_T("Section1"), _T(SN_organizationName),  pwNameEnt, lpFilename); 
		FreeMemory(pwNameEnt);
	}
	if(GetNameEntry(X509_get_subject_name (x509), LN_countryName, &pwNameEnt))
	{	
		WritePrivateProfileString (_T("Section1"), _T(SN_countryName),  pwNameEnt, lpFilename); 
		FreeMemory(pwNameEnt);
	}
	
	fclose(fp);
	X509_free(x509);
	return 0;
}


int GetNameEntry(X509_NAME *pName, char *pKey, LPWSTR *pNameEntry)
{
	int len = 0, nid = 0, index = 0;

	X509_NAME_ENTRY *pNameEnt	= NULL;
	ASN1_STRING		*pASN1Str   = NULL;
	BYTE			*pUTF8Str	= NULL;

	if(!pName || !pKey )
		return 0;

	//pUTF8Str = (BYTE *)malloc(100);

	if((nid = OBJ_txt2nid(pKey)) == NID_undef)
		return 0;
	index = X509_NAME_get_index_by_NID(pName, nid, -1);

	if(index == -1)
		return 0;

	pNameEnt = X509_NAME_get_entry(pName, index);
	pASN1Str = X509_NAME_ENTRY_get_data(pNameEnt);
	ASN1_STRING_to_UTF8(&pUTF8Str, pASN1Str);
	
	len = MultiByteToWideChar(CP_UTF8, 0, (LPSTR)pUTF8Str, strlen((LPSTR)pUTF8Str)+1, NULL, 0);
	*pNameEntry = (LPWSTR)malloc(len*sizeof(WCHAR));
	MultiByteToWideChar(CP_UTF8, 0, (LPSTR)pUTF8Str, strlen((LPSTR)pUTF8Str)+1, *pNameEntry, len);
	
	return 1;
}