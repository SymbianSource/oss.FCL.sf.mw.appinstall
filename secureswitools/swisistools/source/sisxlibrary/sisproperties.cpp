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


#include "sisproperties.h"

void CSISProperties::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	TUint32 propertiesCount = iProperty.size();
	if (propertiesCount == 0)
		{
		return;
		}
	if (aVerbose)
		{
		aStream << L"; Properties" << std::endl;
		}
	aStream << L"+(";
	TUint32 i = 0;
	do
		{
		iProperty[i].AddPackageEntry(aStream, aVerbose);
		if (++i < propertiesCount)
			{
			aStream << L", ";
			}
		} while (i < propertiesCount);
	aStream << L")" << std::endl << std::endl;
	}

