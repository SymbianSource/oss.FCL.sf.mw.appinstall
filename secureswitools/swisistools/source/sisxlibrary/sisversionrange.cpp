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

#include "sisversionrange.h"
#include "exception.h"

void CSISVersionRange::Verify (const TUint32 aLanguages) const
	{
	CStructure <CSISFieldRoot::ESISVersionRange>::Verify (aLanguages);
	CSISException::ThrowIf (iFromVersion.Specified () && iToVersion.Specified () && (iToVersion < iFromVersion),
							CSISException::EVerification,
							"bad version range");
	}


bool CSISVersionRange::WasteOfSpace () const
	{
	return ! (Required () || iFromVersion.Specified () || iToVersion.Specified ());
	}

std::string CSISVersionRange::Name () const
	{
	return "Version Range";
	}

void CSISVersionRange::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	iFromVersion.AddPackageEntry(aStream, aVerbose);
	if (iToVersion.Specified())
		{
		aStream << L" ~ ";
		iToVersion.AddPackageEntry(aStream, aVerbose);
		}
	}


