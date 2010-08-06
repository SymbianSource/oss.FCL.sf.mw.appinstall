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

#include "sisinfo.h"
#include "utility.h"
#include "siscontents.h"

static const SIdentifierTable KInstallType [] = 
	{
		{	L"PA",				CSISInfo::EInstPreInstalledApp,			0,	false	},
		{	L"PARTIALUPGRADE",	CSISInfo::EInstPartialUpgrade,			0,	false	},
		{	L"PIAPP",			CSISInfo::EInstPreInstalledApp,			0,	false	},
		{	L"PIPATCH",			CSISInfo::EInstPreInstalledPatch,		0,	false	},
		{	L"PP",				CSISInfo::EInstPreInstalledPatch,		0,	false	},
		{	L"PU",				CSISInfo::EInstPartialUpgrade,			0,	false	},
		{	L"SA",				CSISInfo::EInstInstallation,			0,	false	},
		{	L"SC",				CSISInfo::EInstInstallation,			0,	true	},
		{	L"SISAPP",			CSISInfo::EInstInstallation,			0,	false	},
		{	L"SISCONFIG",		CSISInfo::EInstInstallation,			0,	true	},
		{	L"SISOPTION",		CSISInfo::EInstInstallation,			0,	true	},
		{	L"SISPATCH",		CSISInfo::EInstAugmentation,			0,	false	},
		{	L"SISSYSTEM",		CSISInfo::EInstInstallation,			0,	true	},
		{	L"SISUPGRADE",		CSISInfo::EInstInstallation,			0,	true	},
		{	L"SO",				CSISInfo::EInstInstallation,			0,	true	},
		{	L"SP",				CSISInfo::EInstAugmentation,			0,	false	},
		{	L"SU",				CSISInfo::EInstInstallation,			0,	true	},
		{	L"SY",				CSISInfo::EInstInstallation,			0,	true	},
		{	NULL,				0	}
	};


void CSISInfo::InsertMembers ()
	{
	InsertMember (iUid);
	InsertMember (iVendorUniqueName);
	InsertMember (iNames);
	InsertMember (iVendorNames);
	InsertMember (iVersion);
	InsertMember (iCreationTime);
	InsertMember (iInstallType);
	InsertMember (iInstallFlags);
	}


CSISInfo::CSISInfo (const CSISInfo& aInitialiser) :
		CStructure <CSISFieldRoot::ESISInfo> (aInitialiser),
		iUid (aInitialiser.iUid),
		iVendorUniqueName (aInitialiser.iVendorUniqueName),
		iNames (aInitialiser.iNames),
		iVendorNames (aInitialiser.iVendorNames),
		iVersion (aInitialiser.iVersion),
		iCreationTime (aInitialiser.iCreationTime),
		iInstallType (aInitialiser.iInstallType),
		iInstallFlags (aInitialiser.iInstallFlags)
	{
	InsertMembers (); 
	}


void CSISInfo::Verify (const TUint32 aLanguages) const
	{
	CStructure <CSISFieldRoot::ESISInfo>::Verify (aLanguages);
	int n = iNames.size ();
	CSISException::ThrowIf (iNames.size () != aLanguages,
							CSISException::ELanguage,
							"language and name counts differ");

	CSISException::ThrowIf (iVendorNames.size () != aLanguages,
								CSISException::ELanguage,
								"language and vendor name counts differ");
	
	}



std::string CSISInfo::Name () const
	{
	return "Info";
	}



CSISInfo::TSISInstallationType CSISInfo::InterpretType (const std::wstring& aOption)
	{
	int index = SearchSortedUCTable (KInstallType, aOption);
	CSISException::ThrowIf (index < 0, CSISException::ESyntax, L"unknown install type: " + aOption);
	if (KInstallType [index].iDeprecated)
		{
		SISLogger::Log(L"Installation type ");
		SISLogger::Log(aOption);
		SISLogger::Log(L" ignored, application assumed.\n");
		}
	iInstallType = KInstallType [index].iValue;
	CSISInfo::TSISInstallationType reply = static_cast <CSISInfo::TSISInstallationType> (KInstallType [index].iValue);
	if ((reply == EInstPreInstalledApp) || (reply == EInstPreInstalledPatch))
		{
		CSISContents::SetStub (CSISContents::EStubPreInstalled);
		}
	return reply;
	}

void CSISInfo::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	if (aVerbose)
		{
		aStream << L"; Package header" << std::endl;
		}
	aStream << L"#{";
	for (int i=0; i < iNames.size(); ++i)
		{
		aStream << L"\"";
		iNames[i].AddPackageEntry(aStream, aVerbose);
		aStream << L"\"";
		if ((i+1)<iNames.size())
			{
			aStream << L",";
			}
		}
	aStream << L"}, (";
	iUid.AddPackageEntry(aStream, aVerbose);
	aStream << L"), ";
	iVersion.AddPackageEntry(aStream, aVerbose);
	aStream << L", TYPE=";
	switch (iInstallType)
		{
		case EInstInstallation:
			aStream << L"SA";
			break;
		case EInstAugmentation:
			aStream << L"SP";
			break;
		case EInstPartialUpgrade:
			aStream << L"PU";
			break;
		case EInstPreInstalledApp:
			aStream << L"PA";
			break;
		case EInstPreInstalledPatch:
			aStream << L"PP";
			break;
		default:
			aStream << L"*UNKNOWN*";
			break;
		}
		
	//Flag indicates package is ROM upgradeable.
	if (iInstallFlags & EInstFlagROMUpgrade)
		{
		aStream << L", RU";
		}
	
	if (iInstallFlags & EInstFlagShutdownApps)
		{
		aStream << L", SH";
		}
    if (iInstallFlags & EInstFlagNonRemovable)
        {
        aStream << L", NR";
        }
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	if (iInstallFlags & EInstFlagHide)
		{
		aStream << L", H";
		}
	#endif

	aStream << std::endl;

	TUint32 vendorCount = iVendorNames.size();
	if (vendorCount)
		{
		if (aVerbose)
			{
			aStream << L"; Vendor names" << std::endl;
			}
		aStream << L"%{";
		for (int j = 0; j < vendorCount; ++j)
			{
			aStream << L"\"";
			iVendorNames[j].AddPackageEntry(aStream, aVerbose);
			aStream << L"\"";
			if ((j+1)<vendorCount)
				{
				aStream << L",";
				}
			}
		aStream << L"}" << std::endl;
		}
	else if (aVerbose)
		{
		aStream << L"; No vendor names" << std::endl;
		}

	if (aVerbose)
		{
		aStream << L"; SIS file build time ";
		iCreationTime.AddPackageEntry(aStream, aVerbose);
		aStream << std::endl;
		}
	aStream << L":\"";
	iVendorUniqueName.AddPackageEntry(aStream, aVerbose);
	aStream << L"\"" << std::endl;
	aStream << std::endl;
	}


