/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sissupportedoptions.h"

void CSISSupportedOptions::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	TUint32 optionCount = iSupportedOption.size();
	if (optionCount == 0)
		{
		return;
		}
	aStream << L"!(";
	for (TUint32 i = 0; i < optionCount; ++i)
		{
		aStream << L"{";
		iSupportedOption[i].AddPackageEntry(aStream, aVerbose);
		aStream << L"}";
		if (i < (optionCount-1))
			{
			aStream << L", ";
			}
		}
	aStream << L")" << std::endl << std::endl;
	}

