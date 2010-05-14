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

#include "sissupportedlanguages.h"

void CSISSupportedLanguages::MakeNeat ()
	{
	CStructure <CSISFieldRoot::ESISSupportedLanguages>::MakeNeat ();
	if (iSupportedLanguage.size () == 0) 
		{
		iSupportedLanguage.Push (CSISLanguage (CSISLanguage::ELangEnglish));
		}
	}


std::string CSISSupportedLanguages::Name () const
	{
	return "Supported Languages";
	}

void CSISSupportedLanguages::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	TUint32 optionCount = iSupportedLanguage.size();
	if (optionCount == 0)
		{
		return;
		}
	if (aVerbose)
		{
		aStream << L"; Languages" << std::endl;
		}
	aStream << L"&";
	for (TUint32 i = 0; i < optionCount; ++i)
		{
		iSupportedLanguage[i].AddPackageEntry(aStream, aVerbose);
		if (i < (optionCount-1))
			{
			aStream << L", ";
			}
		}
	aStream << std::endl << std::endl;
	}

