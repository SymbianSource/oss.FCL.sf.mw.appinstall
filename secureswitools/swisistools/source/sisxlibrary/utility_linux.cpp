/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <stdio.h>
#include "utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <stdint.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>
#include <locale.h>
#include <algorithm>
#include <memory>


// globals
unsigned char g_ucCalledByTestRoutine = 0;
jmp_buf g_pBuf;

void MemTestHandler ( int nSig)
	{
     if ( g_ucCalledByTestRoutine ) longjmp ( g_pBuf, 1);
	}

void* GetStdHandle(unsigned long)
	{
	return;
	}

wchar_t** CommandLineArgs(int &argc , char *argv[])
	{
/* 
This is an entry point of program.It is observed that program doesn't take
LC_ALL and other TYPES from environment so setlocate is the safest way. This 
setting makes it work for japanese characters also.	
*/
	setlocale(LC_ALL,"en_US.utf8");
	wchar_t **argv1 =  new wchar_t*[argc];
	int size=0 ;

// Convert the char* parameter to wchar_t* parameter.
	for(int i=0 ; i<argc ; i++)
		{
		size = ConvertMultiByteToWideChar(argv[i] , strlen(argv[i])+1 , NULL , 0);
		argv1[i] = new wchar_t [size+1];
		ConvertMultiByteToWideChar(argv[i], strlen(argv[i])+1, argv1[i],size+1);
		}
	return argv1;
	}

void cleanup(int argc, wchar_t **argv)
	{
	for(int i = argc-1 ; i>=0 ; i--)
		{
		 delete argv[i];
		}

	delete argv;
	}

/* 
Windows uses 0xFEFF for unicode marker when it tries to create a file with WriteFile()
Eventhough file is in utf-8 format.As Linux is having utf-8 encoding scheme by default
it tries to write 0xEF,0xBB, 0xBF in the begining of file, if the file is utf-8. so to 
make it compatible for the tool we insert 0xFF,0xFE,0x00 in the begining of file.
*/

bool UnicodeMarker(void *hfile)
	{
	unsigned long dwNumBytes;
	::SetFilePointer(hfile, 0L, NULL, FILE_BEGIN);
	const BYTE pBuf[] = { 0xFF,0xFE,0x00 };
	return WriteFile(hfile, (LPVOID)pBuf, sizeof(pBuf), &dwNumBytes, NULL);
	}

int CompareTwoString(wchar_t* string ,wchar_t* option)
	{
	return wcscasecmp(string,option);
	}

int CompareNString(wchar_t* string ,wchar_t* option , int len)
	{
	return wcsncasecmp(string,option,len);
	}

TUint64 GetSizeOfFile(void* hFile)
	{
	DWORD dwHigh = 0;
	DWORD  dwLow = GetFileSize(hFile, &dwHigh);
	TInt64 size = (dwHigh << 32) | dwLow;
	CloseHandle(hFile);
	return size;
	}

/* 

 The below function works in windows because windows largely uses 
 segments to manage memory These functions are not really useful in 
 linux because linux relies on paging to manage memory a running linux
 system only defines four segment, which are shared by all running
 processes (a code and data segment for kernel-space, and a code and 
 data segment for userland) and just swaps pages of memory in and out 
 of those segments as needed. So testing the segment limits under Linux 
 tells you nothing. 

 ** IsBadReadPtr and IsBadWritePtr is deprecated in windows too **

*/

bool IsBadReadPtr(const void* pv, unsigned long ulSize)
	{
 /*   char* pc;
    void(*pPrev) ( int sig);
    g_ucCalledByTestRoutine = 1;
    if(setjmp ( g_pBuf))
		{
         return true;
		}
    pPrev = signal (SIGSEGV, MemTestHandler);
    pc = (char*) malloc ( ulSize);
    memcpy ( pc,const_cast<void*>(pv), ulSize);
    free(pc);
    g_ucCalledByTestRoutine = 0;
    signal ( SIGSEGV, pPrev);*/
    return false;
	}

bool IsBadWritePtr(const void* pv, unsigned long ulSize)
	{
 /*   char* pc;
    void(*pPrev) ( int sig);
    g_ucCalledByTestRoutine = 1;
    if(setjmp ( g_pBuf))
		{
         return true;
		}
    pPrev = signal (SIGSEGV, MemTestHandler);
    pc = (char*) malloc ( ulSize);
    strcpy(pc,"Security Developement");	
    memcpy (const_cast<void*>(pv),pc, ulSize);
    free(pc);
    g_ucCalledByTestRoutine = 0;
    signal ( SIGSEGV, pPrev); */
    return false;
	}

int _wunlink(const wchar_t* wc)
	{
	  int ret = 0;
	  const int len = wcstombs(0,wc,-1);
	  
	  char* tmp = new char[len+1];
	  ret = wcstombs(tmp, wc, len);

	  if(ret == -1) {
		printf("wunlink: wcstombs error\n");
		delete [] tmp;
		return ret;
		}

	  tmp[ret] = '\0';
	  ret = unlink(tmp);

	  if(ret != 0)
		printf("wunlink: %s: %s\n", tmp, strerror(ret));

	  delete [] tmp;
	  return ret;
	}

int GetTempPathW(unsigned int maxlen, const wchar_t *ptr)
	{
	wcsncpy(const_cast<wchar_t*>(ptr), L"/tmp/", maxlen);
	return 0;
	}


unsigned long SetFilePointer(void *hFile,
		     long distance,
		     long *highword,
		     unsigned long method)
	{
	 (void)highword;

	  int fd = (int)hFile;
	  long off = 0;

	  if(method == 0)
		off = lseek(fd, (long)distance, 0);
	  else if(method == 1)
		off = lseek(fd, (long)distance, 1);
	  else if(method == 2)
		off = lseek(fd, (long)distance, 2);
	  else {
		printf(" NOT IMPLEMENTED  \n");
	  }

	  return off;
	}

int CloseHandle(void *fd)
	{
	return close((int)fd) == 0;
	}


long int GetFileSize(void *h, unsigned long*)
	{
	int fd = (int)h;
	int ret = 0;

	struct stat s;

	ret = fstat(fd, &s);

	if(ret != 0) {
		printf("GetFileSize: %s\n", strerror(ret));
		return 0;
	}
	return s.st_size;
	}

int RemoveDirectoryA(const char *a)
	{
	const int ret = rmdir(a);
	return(ret == 0);
	}


int CreateDirectoryA(const char *pathname, void*)
	{
	const int ret = mkdir(pathname, 0664);
	if(ret == -1) {
	  return 0;
		}
	return 1;
	}


int WriteFile(void *hFile,const void *buffer,unsigned long bytesToWrite,unsigned long *bytesWritten,void*)
	{
	size_t ret = write((int)hFile, buffer, bytesToWrite);
	if(ret == -1) {
		printf("WriteFile: %s", strerror(ret));
		return 0;
	}
	if(bytesWritten)
		*bytesWritten = ret;

	return 1;
	}



void* CreateFileA(const char *filename, unsigned long access, unsigned long sharing,void*, unsigned long creation,\
unsigned long attributes, void*)
	{
	//(void)attributes;
	int fd = -1;
	if(creation == OPEN_EXISTING) 
		{
		if(access == (GENERIC_WRITE|GENERIC_READ)) 
			fd = open(filename, O_RDWR);    // APB: was flags = 0666 
		else if(access == GENERIC_WRITE)
			fd = open(filename, O_WRONLY);  // APB: was flags = 0222
        else if(access == GENERIC_READ)
			fd = open(filename,O_RDONLY);   // APB: was flags = 0444
		}
	else if(creation == CREATE_NEW) 
		{
		fd = creat(filename, 0664);
		}
	else if(creation == CREATE_ALWAYS) 
		{
		fd = creat(filename, 0664);
		}
	else 
		{
		printf("CreateFile: unknown creation flag %lu\n", creation);
		}

// check the handle
	if(fd < 0) 
		{
		return INVALID_HANDLE_VALUE;
		}
	else
		{
		return (void *)fd;
		}
	}


int ReadFile(void  *hFile, void *buffer, unsigned long bytesToRead,unsigned long *bytesRead, void *)
        {
         const int size = read((int)hFile, buffer, bytesToRead);
         if(size < 0)
              return 0;

         if(bytesRead)
             {
              *bytesRead = size;
             }
        return 1;
        }

int GetTempFileNameW(const wchar_t* tmpPath, const wchar_t* prefix, int unique, const wchar_t *filename )
	{	
	wcsncpy(const_cast<wchar_t*>(filename), L"tmpXXXXXX", wcslen(L"tmpXXXXXX"));
	int ret = 0;
	char *tmp1 = new char[PATH_MAX];
	wcscat(const_cast<wchar_t*>(tmpPath),filename);
	ret = wcstombs(tmp1, tmpPath, PATH_MAX);
	if(ret == -1)
		{
		printf("GetTempFileName: wcstombs error\n");
		delete[] tmp1;
		return ret;
		}
	mkstemp(tmp1);

	wchar_t *pwc=(wchar_t *)malloc((PATH_MAX)*sizeof(wchar_t));

	ret = mbstowcs(const_cast<wchar_t*>(tmpPath), tmp1,PATH_MAX);

	if(ret == -1)
		{
		printf("GetTempFileName: mbstowcs error\n");
		delete[] tmp1;
		return ret;
		}

	wcscpy(const_cast<wchar_t*>(filename), tmpPath);
		
	delete[] tmp1;
	free (pwc);
	}


void WriteConsole(void* handle, const void * buffer, unsigned long len, unsigned long *nosOfByteWritten, void* reserved)
	{
	wchar_t *buf= (wchar_t *)buffer;
	for (int i = 0;i <len;i++)
	std::wcout<<buf[i];
	}


void WriteConsoleW(void* handle, const void * buffer, unsigned long len, unsigned long *nosOfByteWritten, void* reserved)
	{
	wchar_t *buf= (wchar_t *)buffer;
	for (int i = 0;i <len;i++)
	std::wcout<<buf[i];
	}


int _wchmod(const wchar_t *filename, unsigned long mode )
	{
	int ret = 0;
	int len = wcstombs(0,filename,-1);
	 char *tmp = new char[len];
	 ret = wcstombs(tmp, filename, len);
	if(ret == -1) 
	 {
	 printf("wchmod: wcstombs error\n");
	 delete[] tmp;
	 return ret;
	 }
	 if(mode == _S_IWRITE || mode == _S_IWRITE | _S_IREAD)
		chmod(tmp,S_IWUSR);
	 else 
		if(mode == _S_IREAD)
		  chmod(tmp,S_IRUSR);   
	 
	ret = mbstowcs(const_cast<wchar_t*>(filename),tmp,sizeof(tmp));
	
	if(ret == -1) 
		{
		printf("wchmod: mbstowcs error\n");
		delete[] tmp;
		return ret;
		}

	delete []tmp;
  }


int _wgetcwd(const wchar_t *directory , unsigned long length)
	{
	int ret = 0;
	int len = wcstombs(0,directory,-1);
	char *tmp = new char[len];
	ret = wcstombs(tmp, directory, len);
	if(ret == -1) {
		printf("wgetwd: wcstombs error\n");
		delete[] tmp;
		return ret;
	  }
	getcwd(tmp,length);
	len = mbstowcs(0,tmp,-1);
	ret = mbstowcs(directory, tmp, len);
	if(ret == -1) {
		printf("wgetwd: wcstombs error\n");
		delete[] tmp;
		return ret;
	  }
	delete[] tmp;
	}

char* itoa( int value, char* result, int base ) 
	{
	if (base < 2 || base > 16) { *result = 0; return result; }
	char* out = result;
	int quotient = value;
	do {
	
		*out = "0123456789abcdef"[ std::abs( quotient % base ) ];
		++out;
		quotient /= base;
	} while ( quotient );
	if ( value < 0 && base == 10) *out++ = '-';
	std::reverse( result, out );
	*out = 0;
	return result;
	}

unsigned long GetFileAttributesW(const wchar_t *filename)
	{
	int ret = 0;
	int len = wcstombs(0,filename,-1);
	char *tmp = new char[len];
	ret = wcstombs(tmp, filename, len);
	if(ret == -1) {
		printf("GetFileAttributesW: wcstombs error\n");
		delete[] tmp;
		return ret;
		}

	struct stat s; 
	ret = fstat(tmp, &s);
	return ret;
	}

char* GetTempFile()
	{
	char *templateFile = new char[PATHMAX];
	strcpy(templateFile,"tmpmakesisXXXXXX");
	int fd =  mkstemp(templateFile);
	return templateFile;
	}

void TransferFileData(const wchar_t *Fromfile , char* Tofile)
	{
	BYTE buffer[512];
	FILE *fp = NULL, *tp = NULL; 
	int numread=0;
	int ret = 0;
	int len = wcstombs(0,Fromfile,-1);
	char *tmp = new char[len];
	ret = wcstombs(tmp, Fromfile, len);
	if(ret == -1) {
		printf("wchmod: wcstombs error\n");
		delete[] tmp;
		return ret;
		}
	if((fp=fopen(tmp, "rb")) != NULL)
		{
			if((tp = fopen(Tofile, "wb")) != NULL)
				{
				while(!feof(fp))
					{
					if((numread = fread(buffer, sizeof(unsigned char), 255, fp))>0)
						{
						fwrite(buffer, sizeof(unsigned char), numread, tp);
						}
					}
				fclose(fp);
				fclose(tp);
				delete[] tmp;
				}
		}
	else
		{
		delete[] tmp;
		}
	}

void* MakeSISOpenFile(const wchar_t *pszFilename, unsigned long dwAccessMode, unsigned long dwCreateFlags)
// Open file with Unicode filename correctly under Win95 and WinNT
	{
		char pszMultiByte[PATHMAX] = "\0";
		wchar_t *p=pszFilename;
		
		if (!wcsncmp(pszFilename,L"./",2)) p+=2;
		ConvertWideCharToMultiByte(
			p,			 		// address of wide-character string
			-1,					// number of characters in string
			pszMultiByte, 		// address of buffer for new string
			PATHMAX);			// size of buffer
		
		return CreateFileA(pszMultiByte, dwAccessMode, 0, NULL, dwCreateFlags, FILE_ATTRIBUTE_NORMAL, NULL);		
	}

int MakeSISDeleteFile(const wchar_t *pszFilename)
	{
	char pszMultiByte[PATHMAX] = "\0";
	wchar_t* p=(wchar_t*)pszFilename;
		
		if (!wcsncmp(pszFilename,L"./",2)) p+=2;
		ConvertWideCharToMultiByte(
			p,			 		// address of wide-character string
			-1,					// number of characters in string
			pszMultiByte, 		// address of buffer for new string
			PATHMAX);			// size of buffer
	return unlink(pszMultiByte);
	}

int DeleteFileA(const char* filename)
	{
	return unlink(filename);
	}

int FullPath(wchar_t *pszAbsolutePath, const wchar_t *pszRelativePath, unsigned int maxLength)
	{
	// Not implemented because this function is not used in the tool
	return 0;
	}

bool CreateDir(const wchar_t* aPathName)
	{
	int size = wcslen(aPathName) * 2;
	char* fileName = new char[size];
	int ret = wcstombs(fileName, aPathName, size);
	
	int dirMode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

	int retVal = mkdir(fileName, dirMode);


	return (retVal == 0)? true: false;
	}

int FileAttributes(const wchar_t* aPathName)
	{
	int size = wcslen(aPathName) * 2;
	char* fileName = new char[size];
	int ret = wcstombs(fileName, aPathName, size);
	
	struct stat attrStruct;
	
	stat(fileName, &attrStruct);
	int attr = 0; 
	
	if(S_ISDIR(attrStruct.st_mode))
		{
		attr |= FILE_ATTRIBUTE_DIRECTORY;
		}
	if(attrStruct.st_mode & S_IROTH == 0)
		{
		attr |= FILE_ATTRIBUTE_READONLY;	
		}
	
	return attr;
	}

void WriteToFile(const std::wstring& aFileName, const TUint8* aFileData, int aFileLength)
	{
	int size = wcslen(aFileName.c_str()) * 2;
	std::auto_ptr<char> fileName(new char[size]);
	int ret = wcstombs(fileName.get(), aFileName.c_str(), size);
	std::ofstream stream(fileName.get(), std::ios::out);
	stream.write(aFileData, aFileLength);
	stream.close();
	}

int GetErrorValue()
	{
	return errno;
	}


int ConvertWideCharToMultiByte(const wchar_t* aSource, int /*aSourceLen*/, char* aTarget, int aTargetLen, TUint32 /*aCodePage*/)
	{
	int retValue = wcstombs(aTarget, aSource, aTargetLen);
	if (-1 == retValue)
		{
		return 0;
		}
	return retValue;
	}

int ConvertMultiByteToWideChar(const char* aSource, int /*aSourceLen*/, wchar_t* aTarget, int aTargetLen, TUint32 /*aCodePage*/)
	{
	int retValue = mbstowcs(aTarget, aSource, aTargetLen);
	if (-1 == retValue)
		{
		return 0;
		}
	return retValue;
	}


int FileCopyA(const char* aSrc, const char* aDest, bool aFailIfExistsFlag)
	{
		int err= 0;

		char cmd[ commandLength ] = "";
		if( aFailIfExistsFlag )
		{
			strcpy(cmd, "cp ");
		}
		else
		{
			strcpy(cmd, "cp -f ");
		}
		strcat(cmd, aSrc);
		strcat(cmd, " ");
		strcat(cmd, aDest);
		strcat(cmd, " 2> /dev/null");

        err = system(cmd);

		return err;
	}
int FileMoveA(const char* aSrc, const char* aDest)
	{
		int err= 0;

		// Overwrites the orphaned file(if any).
		char cmd[ commandLength ] = "mv -f ";
		strcat(cmd, aSrc);
		strcat(cmd, " ");
		strcat(cmd, aDest);
		strcat(cmd, " 2> /dev/null");

		err = system(cmd);

		return err;
	}

