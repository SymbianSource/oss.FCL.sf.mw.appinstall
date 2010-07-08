/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif // _MSC_VER


#include "siscapabilities.h"
#include "utility_interface.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

typedef int TInt;
typedef bool TBool;

enum TCapability
	{
	ECapability_HardLimit			= 255
	};

const TInt KCapabilitySetMaxSize	= (((TInt)ECapability_HardLimit + 7)>>3);

LPWSTR TempFileName();

#include <seclib.h>

CSISCapabilities::CSISCapabilities(const CSISCapabilities& aInitialiser)
	: CStructure<CSISFieldRoot::ESISCapabilities> (aInitialiser),
	iCapabilities(aInitialiser.iCapabilities),
	iCapabilitySetSize(0)
	{
	
	InsertMembers();
	
	}

std::string CSISCapabilities::Name() const
	{
	
	return "Capabilties";

	}

void CSISCapabilities::ExtractCapabilities(const std::wstring& aFileName)
	{
	SBinarySecurityInfo info;

	// This call has some rather innapropriate side effects when called
	// on a file that isn't an executable.
	// Here we redirect stderr, so that we don't see them.

	int oldStderr = 0;
	FILE* nulStderr;

	_dup2(oldStderr, 2);

	CSISException::ThrowIf(oldStderr == -1, CSISException::EFileProblem, "Failed in redirection operation");

	#ifdef __TOOLS2_LINUX__
	nulStderr = fopen("/dev/null", "w");
	#else
	nulStderr = fopen("NUL:", "w");
	#endif


	CSISException::ThrowIf(nulStderr == NULL, CSISException::EFileProblem, "Failed in redirection operation");

	CSISException::ThrowIf(_dup2(_fileno(nulStderr), 2) == -1, CSISException::EFileProblem, "Failed in redirection operation");
 	TInt err = 1;
	char buffer[PATHMAX] = {0};
	ConvertWideCharToMultiByte(aFileName.c_str(),-1,buffer,sizeof(buffer));
	err = GetSecurityInfo(buffer, info);
	fflush(stdout);
	_dup2(oldStderr, 2);

	fclose(nulStderr);

	if (!err)
		{

		iCapabilitySetSize = 0;

		for (TInt i = KCapabilitySetMaxSize - 1; i > 0; i--)
			{
		
			if (!iCapabilitySetSize && info.iCapabilities[i])
				{
				iCapabilitySetSize = i + 1;
				} 
		
			iCapabilities |= info.iCapabilities[i];
			iCapabilities = iCapabilities << 8;
			
			}
		
		iCapabilities |= info.iCapabilities[0];
		
		if (iCapabilities && !iCapabilitySetSize)
			{
			iCapabilitySetSize = 1;
			}
		
		} 
	}

bool CSISCapabilities::WasteOfSpace() const
	{
	
	return (0 == iCapabilities);
	
	}

void CSISCapabilities::Verify(const TUint32 aLanguages) const
	{
	
	CStructure <CSISFieldRoot::ESISCapabilities>::Verify (aLanguages);

	CSISException::ThrowIf(iCapabilitySetSize > sizeof(iCapabilities),
		CSISException::EVerification, 
		"Capability set size if greater than this version of makesis is capable of storing");
	
	}

void CSISCapabilities::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	if(WasteOfSpace() || !aVerbose)
		{
		return;
		}

	// Following neads to be printed only if aVerbose is true
	aStream << L"; Capabilities: ";
	int capability = 0;

	TUint32 capabilityField = iCapabilities.Value();
	for (int i = 0; i < KCapabilitySetMaxSize; ++i, ++capability)
		{
		if (capabilityField & 0x01)
			{
			 if (capability < NUMOFCAPABILITIES)
				{
				aStream << KCapabilityNames[capability] << L", ";
				}
			 else
				{
				aStream << L"Unknown Capability (" << (capability + 1) << L"), ";	
				}
			}

		capabilityField >>= 1;
		}
	aStream << std::endl;
	}

void CSISCapabilities::GetCapabilityList(std::vector<std::wstring>& aCapList) const
	{
	int capability = 0;

	TUint32 capabilityField = iCapabilities.Value();
	for (int i = 0; (i < KCapabilitySetMaxSize) && (0 != capabilityField); ++i, ++capability)
		{
		if (capabilityField & 0x01)
			{
			 if (capability < NUMOFCAPABILITIES)
				{
				 aCapList.push_back(KCapabilityNames[capability]);
				}
			 else
				{
				wchar_t capName[35];
#ifdef _MSC_VER
				swprintf(capName, L"Unknown Capability (%d)", (capability + 1));
#else
				swprintf(capName, 35, L"Unknown Capability (%d)", (capability + 1));
#endif //_MSC_VER
				aCapList.push_back(capName);
				}
			}
		capabilityField >>= 1;
		}
	}


