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


#include "sisprerequisites.h"


bool CSISPrerequisites::HasUID (const CSISUid& aUid) const
	{
	for (	CSISArray<CSISDependency, CSISFieldRoot::ESISDependency>::TMemConIter iter = iDependencies.begin ();
			iter != iDependencies.end ();
			iter++) 
		{
		if ((**iter) -> HasUID (aUid))
			{
			return true;
			}
		}
	return false;
	}

void CSISPrerequisites::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	if (iTargetDevices.size())
		{
		// Since iTargetDevices and iDependencies are of same type and they
		// have different structure in the package (pkg) file we need to set
		// 
		for(int i = 0; i < iTargetDevices.size(); ++i)
			{
			iTargetDevices[i].SetDependencyType(CSISDependency::ETargetDependency);
			}
		
		if (aVerbose)
			{
			aStream << L"; Target dependencies" << std::endl;
			}
		iTargetDevices.AddPackageEntry(aStream, aVerbose);
		}

	if (iDependencies.size())
		{
		if (aVerbose)
			{
			aStream << L"; Dependencies" << std::endl;
			}
		iDependencies.AddPackageEntry(aStream, aVerbose);
		}
	}

