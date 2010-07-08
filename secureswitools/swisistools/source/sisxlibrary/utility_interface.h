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

#ifndef __UTILITY_INTERFACE_H__
#define __UTILITY_INTERFACE_H__

#include <iostream>
#include "commontypes.h"

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

// common interface for Windows and Linux

const int commandLength = 512;

wchar_t** CommandLineArgs(int &argc , char *argv[]);

void cleanup(int argc, wchar_t **argv);

int  CompareTwoString(wchar_t* string ,wchar_t* option);

int CompareNString(wchar_t* string ,wchar_t* option , int len);

TUint64 GetSizeOfFile(HANDLE hFile);


char* GetTempFile();

void TransferFileData(const wchar_t *Fromfile , char* Tofile);

void* MakeSISOpenFile(const wchar_t *pszFilename, unsigned long dwAccessMode, unsigned long dwCreateFlags);

int MakeSISDeleteFile(const wchar_t *pszFilename);

// Converts a relative path to an absolute path under Win95 and WinNT
int FullPath(wchar_t *pszAbsolutePath, const wchar_t *pszRelativePath, unsigned int maxLength);

// skip unicode marker if present
bool UnicodeMarker(void* hfile);

bool CreateDir(const wchar_t* aPathName);
int FileAttributes(const wchar_t* aFileName );

void WriteToFile(const std::wstring& aFileName, const TUint8* aFileData, int aFileLength);

int GetErrorValue( void );

/**
 * Converts wide char (unicode) string to multibyte string
 * This interface is provided so that we can have different implementation 
 * in windows and linux machine.
 * @param aSource 		string to be converted
 * @param aSourceLen	Source len. If this is -1 then it will calculate the length of the source.
 * @param aTarget		target location.
 * @param aTargetLen	Space in the target location.
 * @param aCodePage		Code page number (currently supported in windows only)
 * @return Number of bytes that make up the converted part of multibyte sequence. 
 * 			If aTarget is NULL then the function will return the size needed to store
 * 			the complete conversion of the source string.
 */
int ConvertWideCharToMultiByte(const wchar_t* aSource, int aSourceLen, char* aTarget, int aTargetLen, TUint32 aCodePage = 0);
/**
 * Converts multibyte string to wide char (unicode)
 * This interface is provided so that we can have different implementation 
 * in windows and linux machine.
 * @param aSource 		string to be converted
 * @param aSourceLen	Source len. If this is -1 then it will calculate the length of the source.
 * @param aTarget		target location.
 * @param aTargetLen	Space in the target location.
 * @param aCodePage		Code page number (currently supported in windows only)
 * @return Number of bytes that make up the converted part of widechar sequence. 
 * 			If aTarget is NULL then the function will return the size needed to store
 * 			the complete conversion of the source string.
 */
int ConvertMultiByteToWideChar(const char* aSource, int aSourceLen, wchar_t* aTarget, int aTargetLen, TUint32 aCodePage = 0);

/*
 * Copies an existing file to a new file. 
 *
 * @Parameters aSrc					The name of an existing file.  
 * @Parameters aDest				The name of the new file. 
 * @Parameters aFailIfExistsFlag    If this parameter is TRUE and the new file specified by aDest already exists, 
 *									the function fails. If this parameter is FALSE and the new file already exists, 
 *									the function overwrites the existing file and succeeds.
 * @return 
 *									If the function succeeds, the return value is zero.
 *									If the function fails, the return value is non-zero.
 */

int FileCopyA(const char* aSrc, const char* aDest, bool aFailIfExistsFlag);


/*
 * Moves an existing file or a directory, including its children.
 *
 * @Parameters aSrc					The current name of the file or directory on the local computer. 
 * @Parameters aDest				The new name for the file or directory. The new name must not already exist. 
 *									A new file may be on a different file system or drive. A new directory must be 
 *									on the same drive. 
 *
 * @return
 *									If the function succeeds, the return value is zero.
 *									If the function fails, the return value is non-zero.
 */

int FileMoveA(const char* aSrc, const char* aDest);


// interface for only Linux

#ifdef __TOOLS2_LINUX__
#include <errno.h>

// File attributes
const int FILE_ATTRIBUTE_READONLY				= 0x00000001;
const int FILE_ATTRIBUTE_HIDDEN					= 0x00000002;
const int FILE_ATTRIBUTE_SYSTEM					= 0x00000004;
const int FILE_ATTRIBUTE_DIRECTORY				= 0x00000010;
const int FILE_ATTRIBUTE_ARCHIVE				= 0x00000020;
const int FILE_ATTRIBUTE_ENCRYPTED				= 0x00000040;
const int FILE_ATTRIBUTE_COMPRESSED				= 0x00000800;
const int FILE_ATTRIBUTE_OFFLINE				= 0x00001000;


// File creation errors
const int ERROR_PATH_NOT_FOUND		= ENOENT;
const int ERROR_ACCESS_DENIED		= EACCES;
const int ERROR_ALREADY_EXISTS		= EEXIST;
const int ERROR_INVALID_NAME		= EFAULT;

unsigned long GetFileAttributesW(const wchar_t *filename);

int _wunlink(const wchar_t* wc);

void* GetStdHandle(unsigned long handle);

int GetTempPathW(unsigned int maxlen, const wchar_t *ptr);
// Implement WriteFile // Works in Linux
int WriteFile(void *hFile, const void *buffer, 
			  unsigned long bytesToWrite,
			  unsigned long *bytesWritten,
			  void *overlap);

// Implement SetFilePointer // Works in Linux
unsigned long SetFilePointer(void *hFile,
		     long distance,
		     long *highword,
		     unsigned long method);

// Implement CloseHandle // Works in Linux
int CloseHandle(void *fd);

// Implement GetFileSize in  Linux
long int GetFileSize(void *hFile, unsigned long* HighWord);

// Implement ReadFile in  Linux
int ReadFile(void  *hFile, void *buffer, unsigned long bytesToRead,
	      unsigned long *bytesRead, void *);

// Implement RemoveDirectoryA in Linux
int RemoveDirectoryA(const char *a);


// Implement CreateFileA in Linux
void* CreateFileA(const char *filename, unsigned long access, unsigned long sharing,
		   void*, unsigned long creation,
		   unsigned long attributes, void*);

// Implement CreateDirectoryA in Linux
int CreateDirectoryA(const char *pathname, void*);

// Implement WriteConsole in  Linux
void WriteConsole(void* handle,const void *pwNameEnt, unsigned long len, 
				  unsigned long *nosOfByteWritten, void* reserved);

// Wide character support of WriteConsole
void WriteConsoleW(void* handle,const void *pwNameEnt, unsigned long len, 
				  unsigned long *nosOfByteWritten, void* reserved);

// Permission changes of file
int _wchmod(const wchar_t *filename, unsigned long mod);

// Get a current working directory
int _wgetcwd(const wchar_t *directory , unsigned long length);

// Get a temporary file Name
int GetTempFileNameW(const wchar_t* tmpPath, const wchar_t* prefix, int unique, const wchar_t *filename );

//
int DeleteFileA(const char* filename);

bool IsBadReadPtr(const void* pv, unsigned long ulSize);

bool IsBadWritePtr(const void* pv, unsigned long ulSize);

char* itoa( int value, char* result, int base );

#endif // __TOOLS2_LINUX__

#if 0
inline std::ostream& operator<<(std::ostream& aStream, TUint64 aValue)
	{
    char buffer[20];
    sprintf(buffer,"%I64d", aValue);
    aStream << buffer;
    return aStream;
	}
#endif

#endif // __UTILITY_INTERFACE_H__
