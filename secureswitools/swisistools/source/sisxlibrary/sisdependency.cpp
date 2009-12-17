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

#include "sisdependency.h"
#include "exception.h"


void CSISDependency::InsertMembers ()
	{
	InsertMember (iUid);
	InsertMember (iVersionRange);
	InsertMember (iDependencyNames);
	}


CSISDependency::CSISDependency (const CSISDependency& aInitialiser) :
		CStructure <CSISFieldRoot::ESISDependency> (aInitialiser),
		iDependencyNames (aInitialiser.iDependencyNames),
		iVersionRange (aInitialiser.iVersionRange),
		iUid (aInitialiser.iUid)						
	{
	InsertMembers (); 
	}


CSISDependency::CSISDependency (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo)
	{
	InsertMembers ();
	iUid.SetUID (aUID);
	iVersionRange.SetVersionRange (aFrom, aTo);
	}



void CSISDependency::Verify (const TUint32 aLanguages) const
	{
	CStructure <CSISFieldRoot::ESISDependency>::Verify (aLanguages);
	CSISException::ThrowIf (	iDependencyNames.size () != aLanguages, 
								CSISException::ELanguage, 
								"total of names of dependency or target devices differs from language count");
	}



std::string CSISDependency::Name () const
	{
	return "Dependency";
	}

void CSISDependency::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	aStream << ((iDependencyType == ETargetDependency)?L"[" : L"(");
	iUid.AddPackageEntry(aStream, aVerbose);
	aStream << ((iDependencyType == ETargetDependency)?L"]," : L"),");
	iVersionRange.AddPackageEntry(aStream, aVerbose);
	aStream << L" ,{";
	for (int i = 0; i < iDependencyNames.size(); ++i)
		{
		aStream << L"\"";
		iDependencyNames[i].AddPackageEntry(aStream, aVerbose);
		aStream << L"\"";
		if ((i+1) < iDependencyNames.size())
			{
			aStream << L",";
			}
		}
	aStream << L"}" << std::endl << std::endl;
	}

