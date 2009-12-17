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

#include "sislogo.h"
#include "exception.h"

void CSISLogo::Set (const std::wstring& aMime, const std::wstring& aTarget, const TUint32 aFileIndex, const CSISDataUnit& aDataUnit)

{
	assert (! aMime.empty ());	// if empty mime type is valid, rewrite WasteOfSpace below.
	iLogoFile.SetFileIndex(aFileIndex);
	iLogoFile.SetMimeType (aMime);
	if (aTarget.empty ())
		{
		iLogoFile.SetOperation (CSISFileDescription::EOpRun);
		iLogoFile.SetOptions (CSISFileDescription::EInstFileRunOptionByMimeType);
		}
	else
		{
		iLogoFile.SetOperation (CSISFileDescription::EOpRun + CSISFileDescription::EOpInstall);
		iLogoFile.SetOptions (	CSISFileDescription::EInstFileRunOptionByMimeType +
								CSISFileDescription::EInstFileRunOptionInstall);
		iLogoFile.SetTarget (aTarget);
		}
}



std::string CSISLogo::Name () const
	{
	return "Logo";
	}


bool CSISLogo::WasteOfSpace () const
	{
	return ! (Required () || iLogoFile.HasMimeType ());
	}

void CSISLogo::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	if(WasteOfSpace())
		{
		return;
		}
	if (aVerbose)
		{
		aStream << L"; Logo" << std::endl;
		}
	aStream << L"=";
	const wchar_t* dataFileNameW = iLogoFile.GetFileName();
	aStream <<L"\"" << dataFileNameW << L"\"";
	delete[] const_cast<wchar_t*>(dataFileNameW); 
	aStream << L", " ;
	aStream << L"\"";
	iLogoFile.MimeType().AddPackageEntry(aStream, aVerbose);
	aStream << L"\"";
	aStream << L", " ;
	aStream << L"\"";
	iLogoFile.Target().AddPackageEntry(aStream, aVerbose);
	aStream << L"\"";
	aStream << std::endl << std::endl;
	}

