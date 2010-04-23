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
*
*/


#include <iostream>

#include "filedescription.h"
#include "sisfiledescription.h"
#include "deserialiser.h"
#include "serialiser.h"
#include "sisstring.h"
#include "sishash.h"

FileDescription::~FileDescription ()
	{
	}
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
FileDescription::FileDescription(const CSISFileDescription& aFile, TInt aSid, const TInt aTargetDrive, const std::wstring& aFileName, const std::wstring& aLocalName)
	: iTarget(aFileName),
	iLocalFile(aLocalName),
	iMimeType(aFile.MimeType().GetString()),
	iOperation(aFile.Operation()),
	iOperationOptions(aFile.OperationOptions()),
	iUncompressedLength(aFile.UncompressedLength()),
	iIndex(aFile.FileIndex()),
	iSid(aSid)
{
	const unsigned char* data = aFile.Hash().Blob().Data();
	TUint32 len = aFile.Hash().Blob().Size();

	std::string hashData((char*)data, len);

	iHash.SetData(hashData);
	iHash.SetHashId(HashContainer::EHashSHA);
}
#else
FileDescription::FileDescription(const CSISFileDescription& aFile, TInt aSid, const TInt aTargetDrive, const std::wstring& aFileName)
	: iTarget(aFileName),
	iMimeType(aFile.MimeType().GetString()),
	iOperation(aFile.Operation()),
	iOperationOptions(aFile.OperationOptions()),
	iUncompressedLength(aFile.UncompressedLength()),
	iIndex(aFile.FileIndex()),
	iSid(aSid)
{
	const unsigned char* data = aFile.Hash().Blob().Data();
	TUint32 len = aFile.Hash().Blob().Size();

	std::string hashData((char*)data, len);

	iHash.SetData(hashData);
	iHash.SetHashId(HashContainer::EHashSHA);
}
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

void FileDescription::Internalize(Deserialiser& des)
	{

	des >> iTarget
		>> iMimeType
		>> iOperation
		>> iOperationOptions
		>> iHash
		>> iUncompressedLength
		>> iIndex
		>> iSid;
	}
void FileDescription::Externalize(Serialiser& ser)
	{
	ser << iTarget
		<< iMimeType
		<< iOperation
		<< iOperationOptions
		<< iHash
		<< iUncompressedLength
		<< iIndex
		<< iSid;
	}


#ifdef _MSC_VER
Deserialiser& operator>>(Deserialiser& aInput, std::vector<FileDescription*>& val)
	{
	TUint32 size = 0;
	aInput>> size;
	val.resize(size);
	for (TUint32 i = 0; i < size ; ++i)
		{
		val[i] = new FileDescription;
		aInput >> *val[i];
		}
	return aInput;
	}
#endif
