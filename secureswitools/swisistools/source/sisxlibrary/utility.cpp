/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "utility.h"
#include "exception.h"
#include "utility_interface.h"
#include "utils.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <fstream>
#include <algorithm>

std::wostream* SISLogger::iStream = NULL;

void SISLogger::SetStream(std::wostream& aStream)
	{
	iStream = &aStream;
	}

void SISLogger::Log(const std::wstring& aString)
	{
	if(NULL != iStream)
		{
		(*iStream) << aString;
		}
	}


DllExport std::string wstring2string (const std::wstring& aWide)
	{
	int max = ConvertWideCharToMultiByte(aWide.c_str(),aWide.length(),0,0);
	std::string reply;
	if (max > 0 )
		{
		char* buffer = new char [max];
		try
			{
			ConvertWideCharToMultiByte(aWide.c_str(),aWide.length(),buffer,max);
			reply = std::string (buffer, max);
			}
		catch (...)
			{
			}
		delete [] buffer;
		}
	return reply;
	}

std::wstring string2wstring (const std::string& aNarrow)
	{
	int max = ConvertMultiByteToWideChar(aNarrow.c_str(),aNarrow.length(),0,0, CP_OEMCP);
	std::wstring reply;
	if (max > 0 )
		{
		wchar_t* buffer = new wchar_t [max];
		try
			{
			ConvertMultiByteToWideChar(aNarrow.c_str(),aNarrow.length(),buffer,max,CP_OEMCP);
			reply = std::wstring (buffer, max);
			}
		catch (...)
			{
			}
		delete [] buffer;
		}	
	return reply.c_str();
	}

void EnsureExtended (std::wstring& aName, const std::wstring& aDefaultExtension)
	{
	assert (! aName.empty ());
	assert (! aDefaultExtension.empty ());
	std::wstring::size_type posSlash (aName.rfind ('\\'));
	if (posSlash == std::wstring::npos)
		{
		posSlash = 0;
		}
	std::wstring::size_type posDot (aName.find (posSlash, '.'));
	if (posDot == std::wstring::npos)
		{
		aName += aDefaultExtension;
		}
	}

bool ValidateFileName (std::wstring& aName, const bool aMustExist)
	{
	assert (! aName.empty ());
	if (aName.size () < 3)
		{
		return false;
		}
	std::wstring filename (aName);
	bool isUNC ((filename.at (0) == L'/') && (filename.at (1) == L'/'));
	bool isRelative (! isUNC && ((filename.at (1) != L':')));

	if (! isRelative)
		{
		if (! isUNC && (filename.at (2) != L'/')) 
			{
			return false;
			}
		}
	else
		{
		const int maxlen = PATHMAX * 8;	/*	The value of PATHMAX assumes short name FAT environment,
												not long names and / or NTFS, UNC, URL, etc. etc.. Using a
												larger value significantly reduces the chances of failure
												when the application is deep in a subdirectory tree. Windows
												calls require fixed length buffers. 
											*/
		wchar_t	directorybuffer [maxlen];
		std::wstring cwd (_wgetcwd (directorybuffer, maxlen));
		if (cwd.empty ()) 
			{
			return false;
			}
		if (cwd.size () > 3) 
			{
			cwd += KSisDirectorySeparator;	// trailing backslash is returned inconsistently according to docs for getcwd
			}
		filename = cwd + filename;
		}

	if (aMustExist)
		{
		DWORD attributes (GetFileAttributesW (filename.c_str ()));
		if (attributes == RESULT) 
			{
			return false;
			}
		}
	aName = filename;
	return true;
	}


bool ValidateDirectory (std::wstring& aName, const bool aMustExist)
	{
	if (! ValidateFileName (aName, aMustExist)) 
		{
		return false;
		}
	if (aName.at (aName.size () - 1) != L'/') 
		{
		aName += L'/';
		}
	return true;
	}

bool ValidateFileName (std::wstring& aName, const bool aMustExist, const std::wstring& aDefaultExtension)
	{
	assert (! aName.empty ());
	assert (! aDefaultExtension.empty ());
	std::wstring filename (aName);
	EnsureExtended (filename, aDefaultExtension);
	if (! ValidateFileName (filename, aMustExist)) 
		{
		return false;
		}
	aName = filename;
	return true;
	}

TUint32 IdentifyUCKeyword (const SKeyword aKeyword [], std::wstring aIdentifier, const std::wstring& aContext)
	{
	for (std::wstring::size_type letter = 0; letter < aIdentifier.size (); letter++)
		{
		aIdentifier [letter] = toupper (aIdentifier [letter]);
		}
	for (const SKeyword* index = &aKeyword [0]; index -> iName != NULL; index++)
		{
		if (aIdentifier.compare (index -> iName) == 0) 
			{
			return index -> iId;
			}
		}
	throw CSISException (CSISException::ESyntax, aContext + aIdentifier);
	}


CSISFieldRoot::TFieldSize Alignment (const CSISFieldRoot::TFieldSize aOriginal)
	{
	const CSISFieldRoot::TFieldSize coarseness (4);
	CSISFieldRoot::TFieldSize modulus (aOriginal % coarseness);
	return (modulus == 0) ? 0 : (coarseness - modulus);
	}


CSISFieldRoot::TFieldSize AlignedSize (const CSISFieldRoot::TFieldSize aOriginal)
	{
	return aOriginal + Alignment (aOriginal);
	}



void ReadFiller (TSISStream& aFile)
	{
	CSISFieldRoot::TFieldSize diff (Alignment (aFile.tell ()));
	assert (diff >= 0);
	if (diff)
		{
		TUint8 junk;
		for (CSISFieldRoot::TFieldSize index = 0; index < diff; index++) 
			{
			aFile >> junk;
			}
		}
	}


void WriteFiller (TSISStream& aFile)  
	{
	TSISStream::pos_type pos (aFile.tell ());
	if (pos >= 0)
		{
		CSISFieldRoot::TFieldSize diff (Alignment (pos));
		if (diff)
			{
			TUint8 padding = 0;
			for (CSISFieldRoot::TFieldSize index = 0; index < diff; index++) 
				{
				aFile << padding;
				}
			}
		}
	}

HANDLE OpenFileAndGetSize (const std::wstring& aName, TUint64* aSize)
	{
	HANDLE file = MakeSISOpenFile(aName.c_str(),GENERIC_READ,OPEN_EXISTING);
	CSISException::ThrowIf ((INVALID_HANDLE_VALUE == file), 
							CSISException::EFileProblem, std::wstring (L"cannot open ") + aName);
	if (aSize)
		{
		DWORD dwHigh = 0;
		DWORD dwLow = ::GetFileSize(file,&dwHigh);
		TUint64 size = (dwHigh << 32) | dwLow;
		*aSize = static_cast <TUint64> (size);
		}
	return file;
	}


void ReadAndCloseFile (HANDLE aFile, const TUint64 aSize, TUint8* aBuffer)
	{
	size_t got = 0;
	::ReadFile(aFile,aBuffer,aSize,(DWORD*)&got,0);
	::CloseHandle(aFile);
	CSISException::ThrowIf (got != static_cast <size_t> (aSize), 
							CSISException::EFileProblem, 
							"cannot read file");
	}


int SearchSortedUCTable (const SIdentifierTable aTable [], const std::wstring& aIdentifier)
	{
	for (int index = 0; aTable [index].iName; index++)
		{
		int comparision = aIdentifier.compare (aTable [index].iName);
		if (comparision == 0)
			{
			return index;
			}
		if (comparision < 0)
			{
			break;
			}
		}
	return -1;
	}


LPSTR MakeMBCSString(LPCWSTR uniStr, UINT codePage, DWORD& length)
// Convert a UNICODE string to a multi-byte string
	{
	LPSTR mbStr;
	// get num unicode chars required
	DWORD len = ConvertWideCharToMultiByte(uniStr, length, NULL, 0);
	mbStr = new CHAR[len+1];
	if (!mbStr) throw ErrNotEnoughMemory;
	// convert
	ConvertWideCharToMultiByte(uniStr, length, mbStr, len);
	mbStr[len]='\0';
	length=len;

	return mbStr;
	}


char* Copy2TmpFile(const wchar_t *fName, TFileType fType)
{
	char *tmpName, *tmpFileName=NULL;
	int numread = 0;

	tmpName = GetTempFile();
		
	if(tmpName == NULL)
		return NULL;

	tmpFileName = new char[strlen(tmpName)+5];
	strcpy(tmpFileName, tmpName);

	if(fType == CERTFILE)
		{
		tmpFileName = strcat(tmpFileName, ".cer");
		}	
	else if(fType == KEYFILE)
		{
		tmpFileName = strcat(tmpFileName, ".key");
		}
	else
		{
		tmpFileName = strcat(tmpFileName, ".dat");
		}

		TransferFileData(fName,tmpFileName);
		return tmpFileName;

		if(tmpFileName != NULL)
			delete tmpFileName;

	return NULL;
}

/**
 * Returns the size of the first occurance of an invalid directory separator.
 * @param aPath Path to be validated.
 * @param aIndex index from which the search begin. On function return this 
 * 				index will point to the illegal directory separator. 
 * @return 0 if path is valid. Else the number of character to be replaced.
 * e.g. \sys\bin\ should be replaced with /sys/bin/
 * and \\sys\\bin\\ should be replaced with /sys/bin/
 */ 
int FirstInvalidDirSeparatorSize(std::wstring& aPath, std::wstring::size_type& aIndex)
	{
	// If path semantics is correct (as needed by sisx library)
	// then the function will return 0
	int ret = 0; 
	int pos = 0;
	if((pos = aPath.find(L"\\\\", aIndex)) != std::wstring::npos)
		{
		ret = 2;
		}
	else if((pos = aPath.find(L"\\", aIndex)) != std::wstring::npos)
		{
		ret = 1;
		}
	aIndex = pos;
	return ret;
	}


std::wstring FixPathDelimiters( const std::wstring& aString )
    {
    std::wstring ret = aString;
	
    std::wstring::size_type idx = 0;
    int len = 0;
	while(len = FirstInvalidDirSeparatorSize(ret, idx))
        {
		if(idx == 0 && len == 2)
			{
			ret.replace( idx, len, KSisInitialNetworkPathSlashes );
			}
		else
			{
			ret.replace( idx, len, KSisDirectorySeparator );
			}
        }
    return ret.c_str();
    }

int GetFileType(std::wstring& aFileName)
	{
    int len = ConvertWideCharToMultiByte(aFileName.c_str(), -1, NULL, 0);
    char* fileName = new char[len + 1];
    ConvertWideCharToMultiByte(aFileName.c_str(), -1, fileName, len);

    std::ifstream fs(fileName, std::ios::binary | std::ios::in);
    fs.seekg(0, std::ios::end);
    len = fs.tellg();
    fs.seekg(0, std::ios::beg);
    unsigned char* buffer = new unsigned char[len];
    fs.read((char*)buffer, len);
    fs.close();
    
    int type = EFileUnknown;
    
    const TUint32* data32 = reinterpret_cast<const TUint32*>(buffer);
    
	if (len <= sizeof(TUint32)*0x04)
		{
		delete[] buffer;
		return type;
		}
	
	if(IsExeData(buffer, len))
		{
		type = EFileExe;
		}
	else if(IsDllData(buffer, len))
		{
		type = EFileDll;
		}
 
	if(IsEmulatorExecutableData(buffer, len))
		{
		type |= EFileEmulatorExe;
		}

    return type;
	}

bool IsExecutableData(const unsigned char* aContent, TUint32 aLength) 
	{
	if (aLength <= sizeof(TUint32)*0x05)
		{
		return false;
		}
	const TUint32* data32 = reinterpret_cast<const TUint32*>(aContent);
	bool isExecutable = false;

	if (	IsExeData(aContent, aLength) || 
			IsDllData(aContent, aLength) || 
			IsEmulatorExecutableData(aContent, aLength))
		{
		isExecutable = true;
		} 
	
	return isExecutable;
	}

bool IsEmulatorExecutableData(const unsigned char* aContent, TUint32 aLength)
	{
	bool isEmulatorExe = false;

	// winscw binary
	if (aLength > 2 && aContent[0] == 'M' && aContent[1] == 'Z')
		{
		isEmulatorExe = true;
		}

	return isEmulatorExe;
	}


bool IsExeData(const unsigned char* aContent, TUint32 aLength)
	{
	return (GetExecutableType(aContent, aLength) == KExecutableImageUid)? true: false;
	}

bool IsDllData(const unsigned char* aContent, TUint32 aLength)
	{
	return (GetExecutableType(aContent, aLength) == KDynamicLibraryUid)? true: false;
	}


TInt32 GetExecutableType(const unsigned char* aContent, TUint32 aLength)
	{
	const TUint32* data32 = reinterpret_cast<const TUint32*>(aContent);
	if(aLength < sizeof(TUint32)*0x05)
		{
		return 0;
		}
	
	TInt32 type = 0;

	if(data32[0x04] == KFileHeaderSignature)
		{
		type = data32[0];
		}
	else if (IsEmulatorExecutableData(aContent, aLength))
		{
		const char symbian[] = ".SYMBIAN";
		const int  len2      = sizeof(symbian)-1;
		const unsigned char* index = std::search(aContent, aContent+aLength, symbian, symbian+len2);

		if ( (index + KHeaderUidLength < aContent+aLength) && (index != aContent+aLength) )
			{
			index += KHeaderUidLength;
			TInt32 offset = *(TInt32*)index;

			if (offset < aLength)
				{
				const TUint32* x = (TUint32*)(aContent+offset);
				type = x[0];
				}
			}
		}

	return type;
	}
