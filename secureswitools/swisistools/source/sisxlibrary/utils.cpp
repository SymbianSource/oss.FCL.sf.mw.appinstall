/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* INCLUDES
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#include "utils.h"
#include <wchar.h>
#include "utf8_wrapper.h"
#include "utility_interface.h"

#define TMP_FILE_STUB	L"~si"

// ===========================================================================
// GLOBAL UTILS FUNCTIONS
// ===========================================================================

bool FileIsUnicode(LPCWSTR fileName, TEncodingScheme& encScheme)
// check whether a text file is in UNICODE format & whether little/big-endian or utf8
	{
	BYTE pBuf[3] = { 0,0,0 };
	DWORD dwNumBytes;
	DWORD ok;
	const BYTE cUtf8[] = { 0xEF,0xBB, 0xBF };
	const BYTE cUcs2LE[] = { 0xFF,0xFE };
	const BYTE cUcs2BE[] = { 0xFE,0xFF };

		HANDLE hFile = ::MakeSISOpenFile(fileName, GENERIC_READ, OPEN_EXISTING);
	if(hFile == INVALID_HANDLE_VALUE)
		throw ErrCannotOpenFile;
				
	// Make sure we're at the beginning of the file	
	::SetFilePointer(hFile, 0L, NULL, FILE_BEGIN);	

	ok=::ReadFile(hFile, (LPVOID)pBuf, sizeof(pBuf), &dwNumBytes, NULL);

	::CloseHandle(hFile);
	
	if (!ok) throw ErrCannotReadFile;

	if (dwNumBytes>=2 && memcmp(pBuf,cUcs2LE,sizeof(cUcs2LE)) == 0)
		{
		encScheme=EUcs2LE;
		return true;			
		}
	else if (dwNumBytes>=2 && memcmp(pBuf,cUcs2BE,sizeof(cUcs2BE)) == 0)
		{
		encScheme=EUcs2BE;
		return true;
		}
	else
		{
		if (dwNumBytes==sizeof(pBuf) && memcmp(pBuf,cUtf8,sizeof(pBuf)) == 0)
			encScheme=EUtf8;
		else
			encScheme=EAscii;
		return false;
		}
	}

LPWSTR	ConvertUCS2FileToUCS4(LPCWSTR fileName)
// convert a UCS-2 file to UCS-4 format
	{
	LPWSTR pszTempSource;
	DWORD dwNumBytes;
	HANDLE hFile;
	DWORD fileSize;
	UTF16 *pBuf;
	BOOL ok;

	// open file & get file size
	hFile = ::MakeSISOpenFile(fileName, GENERIC_READ, OPEN_EXISTING);
	if(hFile == INVALID_HANDLE_VALUE) throw ErrCannotOpenFile;
	fileSize = ::GetFileSize(hFile, NULL);

	// read in whole file
	pBuf = new UTF16 [fileSize/2+1];
	ok=::ReadFile(hFile, (LPVOID)pBuf, fileSize, &dwNumBytes, NULL);
	::CloseHandle(hFile);
	if (!ok) throw ErrCannotReadFile;

	int targetLength = (fileSize/2)*sizeof(WCHAR);
	UCS4 *ptrUCS4 = new UCS4[targetLength + 1];
	UTF16* sourceStart = reinterpret_cast<UTF16*>(pBuf);
	UTF16* sourceEnd = sourceStart + (fileSize/2)*(sizeof(WCHAR)/2);
	UCS4* targetStart = reinterpret_cast<UCS4*>(ptrUCS4);
	UCS4* targetEnd = targetStart + targetLength;
	ConvertUTF16toUCS4(&sourceStart, sourceEnd, &targetStart, targetEnd);
	int endOffset = (UCS4*)targetStart - ptrUCS4;
	targetStart = reinterpret_cast<UCS4*>(ptrUCS4) + endOffset;
	*targetStart = 0;
	

	// write to new temporary file
	pszTempSource=TempFileName();
	hFile = ::MakeSISOpenFile(pszTempSource, GENERIC_WRITE|GENERIC_READ, CREATE_ALWAYS);
	if (hFile==INVALID_HANDLE_VALUE) throw ErrCannotOpenFile;

	ok = ::WriteFile(hFile, (LPVOID)ptrUCS4, targetLength, &dwNumBytes, NULL);
	::CloseHandle(hFile);

	delete [] pBuf;

	if (!ok)
		{
		_wunlink(pszTempSource);
		throw ErrCannotConvertFile;
		}
	return pszTempSource;
	}


LPWSTR ConvertUCS2FileToLittleEndianUnicode(LPCWSTR fileName)
// convert a UCS-2 big-endian UNICODE file to a little-endian UNICODE file
	{
	LPWSTR pszTempSource;
	DWORD dwNumBytes;
	HANDLE hFile;
	DWORD fileSize;
	LPWSTR pBuf;
	BOOL ok;

	// open file & get file size
	hFile = ::MakeSISOpenFile(fileName, GENERIC_READ, OPEN_EXISTING);
	if(hFile == INVALID_HANDLE_VALUE) throw ErrCannotOpenFile;
	fileSize = ::GetFileSize(hFile, NULL);

	// read in whole file
	pBuf = new WCHAR [fileSize/2+1];
	ok=::ReadFile(hFile, (LPVOID)pBuf, fileSize, &dwNumBytes, NULL);
	::CloseHandle(hFile);
	if (!ok) throw ErrCannotReadFile;

	// convert text to little endian unicode
	for (DWORD i=0; i<(fileSize/2); i++)
		pBuf[i]=(WCHAR)(((pBuf[i]&0xFF00)>>8) | ((pBuf[i]&0xFF)<<8));

	// write to new temporary file
	pszTempSource=TempFileName();
	hFile = ::MakeSISOpenFile(pszTempSource, GENERIC_WRITE|GENERIC_READ, CREATE_ALWAYS);
	if (hFile==INVALID_HANDLE_VALUE) throw ErrCannotOpenFile;
	ok=::WriteFile(hFile, (LPVOID)pBuf, fileSize, &dwNumBytes, NULL);
	::CloseHandle(hFile);
	delete [] pBuf;
	if (!ok)
		{
		_wunlink(pszTempSource);
		throw ErrCannotWriteFile;
		}
	return pszTempSource;
	}

LPWSTR ConvertFileToUnicode(LPCWSTR fileName, TEncodingScheme encScheme)
// convert text file to UNICODE
	{
	LPWSTR pszTempSource;
	DWORD dwNumBytes;
	HANDLE hFile;
	DWORD fileSize;
	LPSTR pNarrowBuf;
	LPWSTR pBufU;
	BOOL ok;

	// open file & get file size
	hFile = ::MakeSISOpenFile(fileName, GENERIC_READ, OPEN_EXISTING);
	if(hFile == INVALID_HANDLE_VALUE) throw ErrCannotOpenFile;
	fileSize = ::GetFileSize(hFile, NULL);

	// read in whole file
	// Linux needs mbstowcs(NULL,src,0)+1 for buffer allocation so added +1.
	pNarrowBuf = new CHAR [fileSize+1];
	ok=::ReadFile(hFile, (LPVOID)pNarrowBuf, fileSize, &dwNumBytes, NULL);
	::CloseHandle(hFile);
	if (!ok) throw ErrCannotReadFile;

	if ( EAscii == encScheme )
		{
		if ( !CorrectUTF8(pNarrowBuf,dwNumBytes) )
			throw ErrCannotConvertFile;
		}
	DWORD dwConvCount = ConvertMultiByteToWideChar(pNarrowBuf,dwNumBytes,0,0);
	pBufU=new WCHAR [dwConvCount];

	// make sure the buffer is zeroed
	memset(pBufU,0,sizeof(WCHAR)*dwConvCount);

	dwConvCount = ConvertMultiByteToWideChar(pNarrowBuf,dwNumBytes,pBufU,dwConvCount);
	if ( !dwConvCount ) throw ErrCannotConvertFile;

	// write to new temporary file
	pszTempSource=TempFileName();
	hFile = ::MakeSISOpenFile(pszTempSource, GENERIC_WRITE|GENERIC_READ, CREATE_ALWAYS);
	if (hFile==INVALID_HANDLE_VALUE) throw ErrCannotOpenFile;

	ok = ::WriteFile(hFile, (LPVOID)pBufU, dwConvCount*sizeof(WCHAR), &dwNumBytes, NULL);
	::CloseHandle(hFile);

	delete [] pNarrowBuf;
	delete [] pBufU;
	if (!ok)
		{
		_wunlink(pszTempSource);
		throw ErrCannotConvertFile;
		}

	return pszTempSource;
	}

LPWSTR TempFileName()
// generate a unique temporary filename
// creates a sub-directory in TEMP and uses fileName as a guide for the
// filename
// returns name of temporary directory if fileName is NULL
	{
	static WCHAR tmpFileName[PATHMAX]={'\0'};
	static WCHAR tmpPath[PATHMAX]={'\0'};
	if (*tmpPath=='\0')
		{
		GetTempPathW(PATHMAX,tmpPath);
		tmpFileName [0] = 0;
		GetTempFileNameW (tmpPath, TMP_FILE_STUB, 0, tmpFileName);
		}
	return tmpFileName;
	}

bool CorrectUTF8(LPSTR pNarrowBuf, DWORD dwNumBytes)
	{
	bool bUtf8Text = true;
	int nBadUtf = 0;
	wchar_t szDbg[255] = {L"0"};
	for (DWORD i = 1; i < dwNumBytes; i++)
		{
			if ( (pNarrowBuf[i] & 0xC0) == 0x80 )
				// if the uppermost bit in current byte is set...
				{
					if ( (pNarrowBuf[i-1] & 0x80) == 0x00 )
						// but previous byte has it reset...
						{
							// if current byte is not 'No-Break Space' char...
							if ( pNarrowBuf[i] != (CHAR)0xA0 )
								{
								nBadUtf ++;
							 	wprintf(L"Detecting illegal UTF8 character at position 0x%02X : 0x%02X \n",i,pNarrowBuf[i]);
								}
						}
				} 
			else 
				// Overlong encoding: lead-byte of a 2 byte sequence, but code point <= 127
				{
					if ( (pNarrowBuf[i-1] & 0xC0) == 0xC0 )
						{
							nBadUtf ++;
							wprintf(L"Detecting illegal UTF8 character at position 0x%02X: 0x%02X\n",i,pNarrowBuf[i]);
						}
				}
		}
	if ( nBadUtf )
		bUtf8Text = false;

	return bUtf8Text;
	}

