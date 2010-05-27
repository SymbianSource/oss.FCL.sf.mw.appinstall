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

#include "sissupportedoption.h"
#include "exception.h"

void CSISSupportedOption::Verify (const TUint32 aLanguages) const
	{
	CStructure <CSISFieldRoot::ESISSupportedOption>::Verify (aLanguages);
	CSISException::ThrowIf (iNames.size () != aLanguages,
							CSISException::ELanguage,
							"supported option name and language counts differ");
	}


std::string CSISSupportedOption::Name () const
	{
	return "Supported Option";
	}

void CSISSupportedOption::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	TUint32 optionCount = iNames.size();
	for (TUint32 i = 0; i < optionCount; ++i)
		{
		aStream << L"\"";
		iNames[i].AddPackageEntry(aStream, aVerbose);
		aStream << L"\"";
		if (i < (optionCount-1))
			{
			aStream << L", ";
			}
		}
	}
