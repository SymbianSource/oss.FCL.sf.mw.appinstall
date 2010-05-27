/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <string>
#include <algorithm>

#include "sisfiledata.h"
#include "blob.h"
#include "utility.h"


void CSISFileData::Load (const std::wstring& aFile, TUint64* aSize)

	{
	TUint64 size = 0;
	HANDLE file = OpenFileAndGetSize (aFile, &size);
	try
		{
		iData.Content ().SetByteCount (size);
		}
	catch (...)
		{
		::CloseHandle(file);
		throw;
		}
	ReadAndCloseFile (file, size, iData.Content ().Data ());
	if (aSize)
		{
		*aSize = size;
		}
	}

TUint32 CSISFileData::GetSid() const 
	{
	const int fileLength = UncompressedSize();
	if(fileLength <= sizeof(TUint32)*0x05)
		{
		return 0;
		}

	const unsigned char* data8 = Data();
	const TUint32* data32 = reinterpret_cast<const TUint32*>(data8);
	
	TUint32 ret = 0;
	// Check the file signature. If its a ARM based symbian executable
	// then the signature should be "EPOC". Check E32ImageHeader for more
	// details
	if ( data32[0x04] == KFileHeaderSignature )
		{
		// If its a symbian executable then check the SID
		// of the executable.
		if(fileLength > (0x80 + sizeof(TUint32)))
			{
			ret = *((TUint32*) (data8 + 0x80 ));
			}
		}
	else if (data8[0] == 'M' && data8[1] == 'Z')
		{
		// Emulator executable has a different format.
		// All symbian emulator based executable will have ".SYMBIAN" 
		// in it. Symbian header will be after this signature.
		// So search for this marker to read symbian file header.
		const char symbian[] = ".SYMBIAN";
		const int  len2      = sizeof(symbian)-1;

		const unsigned char* index = std::search(data8, data8+fileLength, symbian, symbian+len2);
		if (index != data8+fileLength)
			{
			index += KHeaderUidLength;
			TInt32 offset = *(TInt32*)index;
			
			// There should be enough data to read the UIDs from the file.
			if(fileLength <= (offset + 4*sizeof(TUint32)))
				{
				return 0;
				}
			
			const TUint32* x = (TUint32*)(data8+offset);
			// The three UIDs (12 bytes) of the file
			// x[0] = the structure of the file
			// x[1] = identifies the interface the polymorphic DLL implements
			//		or for shared library DLLs that others link to, this value is always the same
			//		or for executables the UID value has to be set to KUidApp or NULL
			// x[3] = distinguishes between objects with the same UID2 
			//		and can be thought of as a project identifier
			if (x[0] == KExecutableImageUid)
				{
				ret = x[4];
				}
			}
		}
	return ret;
	} 


bool CSISFileData::IsExecutable() const 
	{
	return IsExecutableData(Data(), UncompressedSize());
	}

bool CSISFileData::IsEmulatorExecutable() const
	{
	return IsEmulatorExecutableData(Data(), UncompressedSize());
	}

bool CSISFileData::IsExe() const
	{
	return IsExeData(Data(), UncompressedSize());
	}

bool CSISFileData::IsDll() const
	{
	return IsDllData(Data(), UncompressedSize());
	}


