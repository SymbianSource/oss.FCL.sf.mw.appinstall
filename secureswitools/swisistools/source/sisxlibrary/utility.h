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

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <string>
#include "basetype.h"
#include "fieldroot.h"
#include "../common/toolsconf.h"

#define FreeMemory(x) {if((x) != NULL) {free(x); (x)=NULL;}}

// Constants
const std::wstring KSisDirectorySeparator( L"/" );
const std::wstring KSisInitialNetworkPathSlashes( L"//" );

typedef struct {
	wchar_t*		iName;
	TUint32			iValue;
	TUint32			iSubValue;
	bool			iDeprecated;
} SIdentifierTable;

enum 
	{
	EFileUnknown		= 0x00,
	EFileDll			= 0x01,
	EFileExe			= 0x02,
	EFileEmulatorExe	= 0x04,
	};

const TInt32 KDynamicLibraryUid=0x10000079;
const TInt32 KExecutableImageUid=0x1000007a;
const int KFileHeaderSignature = 0x434f5045;
const int KHeaderUidLength = 3 * sizeof(TUint32);

typedef enum {CERTFILE, KEYFILE} TFileType;

int SearchSortedUCTable (const SIdentifierTable aTable [], const std::wstring& aIdentifier);
TUint32 IdentifyUCKeyword (const SKeyword aKeyword [], std::wstring aIdentifier, const std::wstring& aContext);
DllExport std::string wstring2string (const std::wstring& aWide);
std::wstring string2wstring (const std::string& aNarrow);
bool ValidateFileName (std::wstring& aName, const bool aMustExist);
bool ValidateFileName (std::wstring& aName, const bool aMustExist, const std::wstring& aDefaultExtension);
bool ValidateDirectory (std::wstring& aName, const bool aMustExist);
void EnsureExtended (std::wstring& aName, const std::wstring& aDefaultExtension);
CSISFieldRoot::TFieldSize AlignedSize (const CSISFieldRoot::TFieldSize aOriginal);
void ReadFiller (TSISStream& aFile);
void WriteFiller (TSISStream& aFile);  
HANDLE OpenFileAndGetSize (const std::wstring& aName, TUint64* aSize);
void ReadAndCloseFile (HANDLE aFile, const TUint64 aSize, TUint8* aBuffer);
LPSTR MakeMBCSString(LPCWSTR uniStr, UINT codePage, DWORD& length);
char* Copy2TmpFile(const wchar_t *fName, TFileType fType);
std::wstring FixPathDelimiters( const std::wstring& aString );
int FirstInvalidDirSeparatorSize(std::wstring& aPath, std::wstring::size_type& aIndex);
int GetFileType(std::wstring& aFileName);
bool IsExecutableData(const unsigned char* aContent, TUint32 aLength);
bool IsEmulatorExecutableData(const unsigned char* aContent, TUint32 aLength);
bool IsExeData(const unsigned char* aContent, TUint32 aLength);
bool IsDllData(const unsigned char* aContent, TUint32 aLength);
TInt32 GetExecutableType(const unsigned char* aContent, TUint32 aLength);

class SISLogger
	{
public:
	static void SetStream(std::wostream& aStream);
	static void Log(const std::wstring& aString);
	
private:
	static std::wostream*	iStream;
	};


#endif // __UTILITY_H__
