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
#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif // _MSC_VER
//
// Note: This file may contain code to generate corrupt files for test purposes.
// Such code is excluded from production builds by use of compiler defines;
// it is recommended that such code should be removed if this code is ever published publicly.
//

#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/stat.h>

#include "siscontents.h"
#include "utils.h"
#include "utility_interface.h"

#include <fstream>

CSISContents::TStub CSISContents::iStub = CSISContents::EStubNone;



void CSISContents::InsertMembers ()
	{
	InsertMember (iControllerChecksum);
	InsertMember (iDataChecksum);
	InsertMember (iController);
	InsertMember (iData);
	}


CSISContents::CSISContents (const CSISContents& aInitialiser) :
		CStructure <CSISFieldRoot::ESISContents> (aInitialiser),
		iControllerChecksum (aInitialiser.iControllerChecksum), 
		iDataChecksum (aInitialiser.iDataChecksum),
		iController (aInitialiser.iController),
		iData (aInitialiser.iData)
	{ 
	InsertMembers (); 
	}

bool CSISContents::IsSisFile(const std::wstring& aFileName)
	{
	TSISStream input;
	#ifdef __TOOLS2_LINUX__
	std::wstring fileName = FixPathDelimiters(aFileName);
	#else
	std::wstring fileName = aFileName;
	#endif
	HANDLE file = ::MakeSISOpenFile(fileName.c_str(),GENERIC_READ,OPEN_EXISTING);
	CSISException::ThrowIf ((INVALID_HANDLE_VALUE == file), 
							CSISException::EFileProblem, std::wstring (L"cannot open ") + fileName);
	bool success = input.import (file, NULL);
	::CloseHandle(file);
	CSISException::ThrowIf (! success, CSISException::EFileProblem, std::wstring (L"cannot read ") + fileName);
	return IsSisFile(input);
	}

bool CSISContents::IsSisFile(TSISStream& aStream)
	{
	CSISUid::TUid uid [4];
	aStream.seek (0);
	aStream >> uid [0] >> uid [1] >> uid [2] >> uid [3];
	bool isSisFile = false;
	if( uid [0] == KUidSISXApp )
		{
		isSisFile = true;
		}
	else if(uid[2] == KUidLegacySisFile)
		{
		throw CSISException (CSISException::ELegacyFormat, L"file is a unspported legacy SIS file.");
		}

	return isSisFile;
	}


void CSISContents::Load (const std::wstring& aFileName, TUint64* size)
	{
	TSISStream input;
	#ifdef __TOOLS2_LINUX__
	std::wstring fileName = FixPathDelimiters(aFileName);
	#else
	std::wstring fileName = aFileName;
	#endif
	HANDLE file = ::MakeSISOpenFile(fileName.c_str(),GENERIC_READ,OPEN_EXISTING);
	if (INVALID_HANDLE_VALUE == file)
		{
		SISLogger::Log(L" Error : Cannot open file : ");
		SISLogger::Log(fileName);
		SISLogger::Log(L"\n");
		throw CSISException (CSISException::EFileProblem, std::wstring (L"cannot open ") + fileName);
		}
	bool success = input.import (file, size);
	::CloseHandle(file);
	if(!success)
		{
		SISLogger::Log(L" Error : Cannot read file : ");
		SISLogger::Log(fileName);
		SISLogger::Log(L"\n");
		throw CSISException (CSISException::EFileProblem, std::wstring (L"cannot read ") + fileName);
		}
	CSISUid::TUid uid [4];
	input.seek (0);
	input >> uid [0] >> uid [1] >> uid [2] >> uid [3];
	if( uid [0] != KUidSISXApp )
		{
		SISLogger::Log(fileName);
		SISLogger::Log(L" is not a SIS file .\n");
		if(uid[2] == KUidLegacySisFile)
			{
			throw CSISException (CSISException::ELegacyFormat, fileName + L" is a unspported legacy SIS file.");
			}
		throw CSISException (CSISException::EFileFormat, fileName + L" is not a SIS file.");
		}
		
	CSISException::ThrowIf (uid [3] != UidChecksum (uid [0], uid [1], uid [2]),
							CSISException::EFileFormat,
							std::wstring (fileName) + L" has invalid UID checksum");
	Read (input, input.length ());
	Verify (LanguageCount ());
	}

void CSISContents::Save (const std::wstring& aFileName)	const
	{
	TSISStream output;
	if (iStub == EStubROM)
		{
		iController.Content ().Write (output, false);
		}
	else
		{
		OutputHeaderUids (output);
		Write (output, false);
		}
	// remove R/O attributes from file in case it exists already
	_wchmod(aFileName.c_str(),_S_IREAD | _S_IWRITE);
	HANDLE file = ::MakeSISOpenFile(aFileName.c_str(),GENERIC_WRITE,CREATE_ALWAYS);
	CSISException::ThrowIf ((INVALID_HANDLE_VALUE == file), 
							CSISException::EFileProblem, std::wstring (L"cannot create ") + aFileName);
	bool written = output.exportfile (file);
	::CloseHandle(file);
	if (! written)
		{
		::MakeSISDeleteFile(aFileName.c_str());
		throw CSISException (CSISException::EFileProblem, std::wstring (L"cannot write to ") + aFileName);
		}
	}



TCRC CSISContents::UIDCheck (const void *pPtr)
	{
	const unsigned char *pB = reinterpret_cast <const unsigned char *> (pPtr);
	const unsigned char *pE = pB + (KUidInstallCount * sizeof (CSISUid::TUid));
	unsigned char buf [(KUidInstallCount * sizeof(DWORD))>>1];
	unsigned char *pT = (&buf[0]);
	while (pB < pE)
		{
		*pT++ = (*pB);
		pB += 2;
		}
	TCRC reply (0); 
	DoTheCrc (reply, reinterpret_cast <const TUint8*> (buf), (KUidInstallCount * sizeof(DWORD))>>1);
	return reply;
	}

void CSISContents::OutputHeaderUids (TSISStream& output) const
	{
	CSISUid::TUid uid [KUidInstallCount + 1];
	uid [0] = KUidSISXApp;
	uid [1] = 0;				// Reserved for future use
	uid [2] = UID1 ();
	uid [3] = UidChecksum (uid [0], uid [1], uid [2]);
	for (unsigned short index = 0; index <= KUidInstallCount; index++) 
		{
		output << uid [index];
		}
	}

CSISUid::TUid CSISContents::UidChecksum (const CSISUid::TUid aUid1, const CSISUid::TUid aUid2, const CSISUid::TUid aUid3)
	{
	CSISUid::TUid uid [3];

	uid [0] = aUid1;
	uid [1] = aUid2;
	uid [2] = aUid3;

	TCRC crcHigh (UIDCheck ((reinterpret_cast <char*> (uid)) + 1));
	TCRC crcLow (UIDCheck (reinterpret_cast <char*> (uid)));
	return static_cast <CSISUid::TUid> ((static_cast <unsigned int> (crcHigh) << 16) + static_cast <unsigned int> (crcLow));
	}

void CSISContents::EmbedFile (const std::wstring& aFile, const CSISUid::TUid& aUid, CSISInstallBlock& aInstall)
	{
	EmbedPackage (aInstall, aFile, aUid);
	}

void CSISContents::EmbedPackage (CSISInstallBlock& aInstallBlock, const std::wstring& aFile, const CSISUid::TUid& aUid)
	{
	std::auto_ptr<CSISContents> embed (new CSISContents());
	embed->Load (aFile);

	//makesis doesn't support embedded packages of type PA and PP.
	if(embed->iData.Stub ())
		{
		SISLogger::Log(aFile);
		SISLogger::Log(L" is a stub.\t \n");
		SISLogger::Log(L"WARNING : Embedded Preinstalled Stub (PA/PP type) SIS file is not supported.\n");
		throw CSISException (CSISException::EFileFormat, "Stub File");
		} 

	if (aUid != 0)
		{
		CSISException::ThrowIf (embed->UID1 () != aUid, CSISException::EUID, aFile + L" contains incorrect UID");
		}
	TUint32 index = iData.AppendData (embed->iData);
	if (index > 0)
		{
		embed->SetDataIndex (index);
		}
	aInstallBlock.Embed (embed->iController.Content (), index);
	}

void CSISContents::Verify (const TUint32 aLanguages) const
	{
	assert (aLanguages == LanguageCount ());
	CSISException::ThrowIf (aLanguages == 0, CSISException::ELanguage, "no languages defined");		
	CStructure <CSISFieldRoot::ESISContents>::Verify (aLanguages);
	}


void CSISContents::PrepareCrcs ()
	{
	if ((iDbgFlag & EDbgDataChecksum) == 0)
		{
		iDataChecksum.Set (iData.Crc ());
		}
	if ((iDbgFlag & EDbgControllerChecksum) == 0)
		{
		iControllerChecksum.Set (iController.Crc ());
		}
	}

void CSISContents::WriteSIS (std::wstring aTargetFileName)
	{
	MakeNeat ();	// MakeNeat () should ensure at least one language defined
	assert (LanguageCount () > 0);

	switch (iStub)
		{
	case EStubPreInstalled :
		iController.Content ().SetNow ();
		iData.SetStub ();
		iDataChecksum.Set(0);
		SetDebugOption(EDbgDataChecksum);
		break;
	case EStubROM :
			{
			// The below piece of code tries to put .sis extension not .SIS as target sisfile name.
			std::wstring::size_type count (aTargetFileName.size ());
			if (	(count >= 4) &&
					(aTargetFileName [count - 4] == L'.'))
				{
				count -= 3;
				std::wstring extension (aTargetFileName.substr (count));
				if (	(toupper(extension [0]) == L'S') &&
						(toupper(extension [1]) == L'I') &&
						(toupper(extension [2]) == L'S'))
					{
					aTargetFileName = aTargetFileName.substr (0, count);
					aTargetFileName += std::wstring (L"sis");
					}
				}
			}
			// Leave the date and time initialised to 1st Jan 2004, so that ROM stub
			// files don't change every time they are built.
		break;
	default:
		iController.Content ().SetNow ();
		break;
		}

	PrepareCrcs ();
	Verify (LanguageCount ());
#ifdef GENERATE_ERRORS
	if (CSISFieldRoot::IsAnyBugSet ())
		{
		CreateDefects ();
		}
#endif // GENERATE_ERRORS

	Save (aTargetFileName);
	}

void CSISContents::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	iControllerChecksum.AddPackageEntry(aStream, aVerbose);
	iDataChecksum.AddPackageEntry(aStream, aVerbose);
	iController.AddPackageEntry(aStream, aVerbose); // CSISCompressed
	Controller().AddPackageEntry(aStream, aVerbose);
	iData.AddPackageEntry(aStream, aVerbose);
	}
