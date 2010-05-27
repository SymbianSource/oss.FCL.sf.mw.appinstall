/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "sisregistrydependency.h"
#include "deserialiser.h"
#include "serialiser.h"
#include "sisdependency.h"
#include "sisuid.h"
#include "sisversionrange.h"
#include "sisversion.h"

SisRegistryDependency::~SisRegistryDependency ()
	{
	}

SisRegistryDependency::SisRegistryDependency(const CSISDependency& aDep)
 : iUid(aDep.SisUID().UID1())
{
	const CSISVersion& x = aDep.VersionRange().FromVersion();
	iFromVersion = Version(x.Major(), x.Minor(), x.Build());
	
	const CSISVersion& y = aDep.VersionRange().ToVersion();
	
	if (y.WasteOfSpace())
		{ 
		iToVersion = Version(-1, -1, -1);
		}
	else
		{
		iToVersion = Version(y.Major(), y.Minor(), y.Build());
		} 
}

void SisRegistryDependency::Internalize(Deserialiser& des)
	{
	des >> iUid >> iFromVersion >> iToVersion;
	}
void SisRegistryDependency::Externalize(Serialiser& ser)
	{
	ser << iUid << iFromVersion << iToVersion;
	}

std::basic_ostream<wchar_t>& operator<<(std::basic_ostream<wchar_t>& os,
										const Version& ver)
{
	return os << ver.GetMajor() << L"." << ver.GetMinor() << L"." << ver.GetBuild();
};

std::wostream& operator<<(std::wostream& os,
						 const SisRegistryDependency& aDep)
{
	return os << L"(" << std::hex << aDep.GetUid() << std::dec << L") -"
			  << aDep.GetFromVersion() << L" to "
			  << aDep.GetToVersion();
}

#ifdef _MSC_VER

Deserialiser& operator>>(Deserialiser& aInput, std::vector<SisRegistryDependency*>& val)
	{
	TUint32 size = 0;
	aInput>> size;
	val.resize(size);
	for (TUint32 i = 0; i < size ; ++i)
		{
		val[i] = new SisRegistryDependency;
		aInput >> *val[i];
		}
	return aInput;
	}
#endif 