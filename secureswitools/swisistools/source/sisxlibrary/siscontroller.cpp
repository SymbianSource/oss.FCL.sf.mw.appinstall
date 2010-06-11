/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif // _MSC_VER

#include "siscontroller.h"
#include "utils.h"

const unsigned short KRawSkipCount = 8;
	// count of fields signed by sign sis, all up to but excluding iDataIndex

bool CSISController::iSkip = false;
int CSISController::iLastCtlID = -2;

CSISController::CSISController (const CSISController& aInitialiser) :
		CStructure <CSISFieldRoot::ESISController> (aInitialiser),
		iInfo (aInitialiser.iInfo),
		iSupportedOptions (aInitialiser.iSupportedOptions),
		iSupportedLanguages (aInitialiser.iSupportedLanguages),
		iPrerequisites (aInitialiser.iPrerequisites),
		iProperties (aInitialiser.iProperties),
		iLogo (aInitialiser.iLogo),
		iInstallBlock (aInitialiser.iInstallBlock),
		iSignatures (aInitialiser.iSignatures),
		iDataIndex (aInitialiser.iDataIndex),
		iRawBuffer (NULL),
		iRawBufferSize(0),
		iBufferSize (0)
	{
	InsertMembers ();
	if (aInitialiser.iRawBuffer)
		{
		try
			{
			iRawBuffer = new TUint8 [aInitialiser.iBufferSize];
			memcpy (iRawBuffer, aInitialiser.iRawBuffer, aInitialiser.iBufferSize);
			iRawBufferSize = aInitialiser.iRawBufferSize;
			iBufferSize = aInitialiser.iBufferSize;
			}
		catch (...)
			{
			}
		}
	}


void CSISController::InsertMembers ()
	{
	// if you change these, change Read and Write (for SignSIS) and KRawSkipCount
	InsertMember (iInfo);
	InsertMember (iSupportedOptions);
	InsertMember (iSupportedLanguages);
	InsertMember (iPrerequisites);
	InsertMember (iProperties);
	InsertMember (iLogo);
	InsertMember (iInstallBlock);
	InsertMember (iSignatures);
	InsertMember (iDataIndex);
	}

int CSISController::DeleteDirectory(std::string& aDirectory)
	{
	return RemoveDirectoryA(aDirectory.c_str());
	}

void CSISController::Read (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType)
	{
	delete [] iRawBuffer;
	iRawBuffer = NULL;
	iRawBufferSize = iBufferSize = 0;
	iRawBuffer = new TUint8 [aContainerSize];
	iControllerID = ++iLastCtlID;
	try
		{
		iBufferSize = aContainerSize;
		TSISStream::pos_type pos = aFile.tell ();
		CSISHeader header;
		header.Read (aFile, aContainerSize, aArrayType);
		assert (header.DataType () == CSISFieldRoot::ESISController);
		iRawBufferSize = header.DataSize();
		aFile.read (iRawBuffer, iRawBufferSize);
		aFile.seek (pos);
		if (iSkip)
			{
			assert (aArrayType == CSISFieldRoot::ESISUndefined);	// arrays not tested
			iBufferSize = CStructure <CSISFieldRoot::ESISController>::SkipRead (aFile, header.DataSize(), aArrayType, KRawSkipCount, true);
			}
		else
			{
			iBufferSize = CStructure <CSISFieldRoot::ESISController>::SkipRead (aFile, header.DataSize(), aArrayType, KRawSkipCount, false);
			aFile.seek (pos);
			CStructure <CSISFieldRoot::ESISController>::Read (aFile, header.DataSize(), aArrayType);
			}
		}
	catch (...)
		{
		delete [] iRawBuffer;
		iRawBuffer = NULL;
		iBufferSize = 0;
		iRawBufferSize = 0;
		throw CSISException (CSISException::EFileFormat, L"error while reading sis");
		}
	}


void CSISController::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
	if (! iRawBuffer)
		{
		CStructure <CSISFieldRoot::ESISController>::Write (aFile, aIsArrayElement);
		}
	else
		{
		RawSkipWrite (aFile, aIsArrayElement, iRawBuffer, iBufferSize, KRawSkipCount);
		}
	}

void CSISController::AddSignatureChain(CSignatureCertChainData& aSISSigCertChain)
	{
	iSignatures.Push (aSISSigCertChain);	
	}

void CSISController::RemoveSignature ()
	{
	CSISException::ThrowIf (iSignatures.size () == 0, CSISException::ENotSigned, L"input file");
	CSISFieldRoot::TFieldSize size = iSignatures.Last ().ByteCountWithHeader (false);
	assert (size < iBufferSize);
	iSignatures.Pop ();
	iBufferSize -= size;
	iRawBufferSize -= size;
	}

void CSISController::Load (const std::wstring& aFileName)
	{
	TSISStream input;
	HANDLE file = ::MakeSISOpenFile(aFileName.c_str(),GENERIC_READ,OPEN_EXISTING);
	CSISException::ThrowIf ((INVALID_HANDLE_VALUE == file), 
							CSISException::EFileProblem, std::wstring (L"cannot open ") + aFileName);
	bool success = input.import (file, NULL);
	
	TUint32 len = ::GetFileSize(file,0);

	::CloseHandle(file);
	
	CSISException::ThrowIf (! success, CSISException::EFileProblem, std::wstring (L"cannot read ") + aFileName);

	TUint32 controllerSize;
	input.seek (0);
	input>>controllerSize;

	CSISException::ThrowIf ((controllerSize+4) != len, CSISException::EFileProblem, aFileName + std::wstring (L" Is not a valid SIS or SIS Controller"));

	input.seek (0); 

	//default parameter CSISFieldRoot::ESISController is passed to ensure that the 
	//Stub SIS Controller file reading starts from the controller length 
	Read (input, len,CSISFieldRoot::ESISController); 
}


void CSISController::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	iSupportedLanguages.AddPackageEntry(aStream, aVerbose);
	iInfo.AddPackageEntry(aStream, aVerbose);
	iPrerequisites.AddPackageEntry(aStream, aVerbose);
	iSupportedOptions.AddPackageEntry(aStream, aVerbose);
	iProperties.AddPackageEntry(aStream, aVerbose);
	iLogo.AddPackageEntry(aStream, aVerbose);
	iInstallBlock.AddPackageEntry(aStream, aVerbose);
	for(int i = 0; i < iSignatures.size(); ++i)
		{
		iSignatures[i].AddPackageEntry(aStream, aVerbose);
		}
	}

TUint64 CSISController::ControllerSizeForHash() const
	{
	TUint64 ctlSize = ByteCount(false);
	TUint64 size = ctlSize - iSignatures.ByteCountWithHeader(false) - iDataIndex.ByteCountWithHeader(false);
	if(CSISHeader::IsBigHeader(ctlSize))
		{
		size += 8;
		}
	else
		{
		size += 4;
		}
	
	return size;
	}

