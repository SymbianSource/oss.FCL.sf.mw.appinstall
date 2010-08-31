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
 @file 
 @internalComponent
 @released
*/

#include "utility_interface.h"
#include "exception.h"

#include <direct.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <io.h>
#include <sys/stat.h>

wchar_t** CommandLineArgs(int &argc ,char *argv[])
	{
	// Entry point of tool.
	return CommandLineToArgvW( GetCommandLineW(), &argc);
	}

void cleanup(int argc, wchar_t **argv)
	{
	return;
	}

int CompareTwoString(wchar_t* string ,wchar_t* option)
	{
	return wcsicmp(string,option);
	}

int CompareNString(wchar_t* string ,wchar_t* option, int len)
	{
	return wcsnicmp(string,option,len);
	}

bool UnicodeMarker(void *hfile)
	{
	return 1;
	}

TUint64 GetSizeOfFile(HANDLE hFile)
	{
	ULARGE_INTEGER li;
	li.LowPart = GetFileSize(hFile, &li.HighPart);
	::CloseHandle(hFile);
	return li.QuadPart;
	}

char* GetTempFile()
	{
	char *tmpName = _tempnam(NULL, "tmpmakesis");
	return tmpName;
	}

void TransferFileData(const wchar_t *Fromfile , char* Tofile)
	{
	int numread = 0;
	byte buffer[512];

	FILE *fp = NULL, *tp = NULL;

	if((fp=_wfopen(Fromfile, L"rb")) != NULL)
		{
		if((tp = fopen(Tofile, "wb")) != NULL)
			{
			while(!feof(fp))
				{
				if((numread = fread(buffer, sizeof(byte), 255, fp))>0)
					{
					fwrite(buffer, sizeof(byte), numread, tp);
					}
				}
			fclose(fp);
			fclose(tp);
			}
		}
	}

int FullPath(wchar_t * pszAbsolutePath, const wchar_t * pszRelativePath, size_t maxLength)
// Converts a relative path to an absolute path under Win95 and WinNT
	{
	char pszMultiByteRelative[PATHMAX] = "\0";
	char pszMultiByteAbsolute[PATHMAX] = "\0";
	LPWSTR p=(LPWSTR)pszRelativePath;
	
	if (!wcsncmp(pszRelativePath,L".\\",2)) p+=2;
	::WideCharToMultiByte(CP_OEMCP,				// code page
		0,					// performance and mapping flags
		p,			 		// address of wide-character string
		-1,					// number of characters in string
		pszMultiByteRelative,	// address of buffer for new string
		PATHMAX,		// size of buffer
		NULL,				// address of default for unmappable characters
		NULL);				// address of flag set when default char. used
	
	int returnValue = _fullpath(pszMultiByteAbsolute, pszMultiByteRelative, maxLength)
		? 1 : 0;
	
	::MultiByteToWideChar(CP_OEMCP,
		0,
		pszMultiByteAbsolute,
		-1,
		pszAbsolutePath,
		PATHMAX);
	
	return returnValue;
	}

HANDLE MakeSISOpenFile(LPCWSTR pszFilename, DWORD dwAccessMode, DWORD dwCreateFlags)
// Open file with Unicode filename correctly under Win95 and WinNT
	{
	// Opening the file with FILE_SHARE_READ access, so that other processes can access
	// the package file in parallel( Issue observed in Raptor with parallel builds udeb and urel)
	return CreateFileW(pszFilename, dwAccessMode, 1, NULL, dwCreateFlags, FILE_ATTRIBUTE_NORMAL, NULL);
	}

BOOL MakeSISDeleteFile(LPCWSTR pszFilename)
	{
	// Delete file with Unicode filename correctly under Win95 and WinNT
	return DeleteFileW(pszFilename);
	}

bool CreateDir(const wchar_t* aPathName)
	{
	return ::CreateDirectoryW(aPathName, NULL)? true: false;
	}

int FileAttributes( LPCTSTR lpFileName )
	{
	return ::GetFileAttributesW( lpFileName );
	}

void WriteToFile(const std::wstring& aFileName, const TUint8* aFileData, int aFileLength)
	{
	_wchmod(aFileName.c_str(), _S_IREAD | _S_IWRITE);
	HANDLE file = ::MakeSISOpenFile(aFileName.c_str(), GENERIC_WRITE, CREATE_ALWAYS);
	CSISException::ThrowIf ((INVALID_HANDLE_VALUE == file), CSISException::EFileProblem, std::wstring (L"cannot create ") + aFileName);
	
	size_t written = 0;
	::WriteFile(file,aFileData, aFileLength, (DWORD*)&written,0);
	::CloseHandle(file);
	CSISException::ThrowIf ((aFileLength != written), CSISException::EFileProblem, std::wstring (L"error writting to file ") + aFileName);
	}

int ConvertWideCharToMultiByte(const wchar_t* aSource, int aSourceLen, char* aTarget, int aTargetLen, TUint32 aCodePage)
	{
	if(0 == aCodePage)
		{
		aCodePage = CP_OEMCP;
		}
	return WideCharToMultiByte( aCodePage, 0, aSource, aSourceLen, aTarget, aTargetLen, NULL, NULL);
	}

int ConvertMultiByteToWideChar(const char* aSource, int aSourceLen, wchar_t* aTarget, int aTargetLen, TUint32 aCodePage)
	{
	if(0 == aCodePage)
		{
		aCodePage = CP_UTF8;
		}
	return MultiByteToWideChar( aCodePage, 0, aSource, aSourceLen, aTarget, aTargetLen);
	}

int GetErrorValue()
	{
	return ::GetLastError();
	}

int FileCopyA(const char* aSrc, const char* aDest, bool aFailIfExistsFlag)
{
	// CopyFileA() returns zero on failure and non-zero otherwise.
	int err=CopyFileA(aSrc,aDest,aFailIfExistsFlag);
	// To maintain consistency with the LINUX wrapper API FileCopyA() which uses cp 
	// command with system(), we return 0 on success and 1 on failure.
	return !err;
}

int FileMoveA(const char* aSrc, const char* aDest)
{
	// MoveFileA() returns zero on failure and non-zero otherwise.
	int err=MoveFileA(aSrc,aDest);
	// To maintain consistency with the LINUX wrapper API FileMoveA() which uses mv
	// command with system(), we return 0 on success and 1 on failure.
	return !err;
}

