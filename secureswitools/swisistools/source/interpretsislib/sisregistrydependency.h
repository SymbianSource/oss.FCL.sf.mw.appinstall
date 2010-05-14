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


#ifndef	SISREGISTRYDEPENDENCY_H
#define	SISREGISTRYDEPENDENCY_H

// System include files
#include <istream>
#include <vector> 

// User include files
#include "serialisable.h"
#include "symbiantypes.h"
#include "version.h"

// Forward Declarations
class Serialiser;
class Deserialiser;
class CSISDependency;

/** 
* @file SISREGISTRYDEPENDENCY.H
*
* @internalComponent
* @released
*/
class SisRegistryDependency : public Serialisable//<SisRegistryDependency>
	{
public:
	/**
	Constructor
	*/
	SisRegistryDependency () {};
	SisRegistryDependency (const CSISDependency& aDep);
	virtual ~SisRegistryDependency ();

	virtual void Internalize(Deserialiser& des);
	virtual void Externalize(Serialiser& ser);

	const Version& GetToVersion() const
	{ return iToVersion; }

	void SetToVersion(const Version& aToVersion)
	{ iToVersion = aToVersion; }

	const Version& GetFromVersion() const
	{ return iFromVersion; }

	void SetFromVersion(const Version& aFromVersion)
	{ iFromVersion = aFromVersion; }

	TUint32 GetUid() const
	{ return iUid; }

	void SetUid(TUint32& aUid)
	{ iUid = aUid; }

private:
	TUint32 iUid;
	Version iFromVersion;
	Version iToVersion;
    };

std::basic_ostream<wchar_t>& operator<<(std::basic_ostream<wchar_t>& os,
										const Version& ver);

std::wostream& operator<<(std::wostream& os,
						 const SisRegistryDependency& aDep);

#ifdef _MSC_VER

Deserialiser& operator>>(Deserialiser& aInput, std::vector<SisRegistryDependency*>& val);

#endif 

#endif	/* SISREGISTRYDEPENDENCY_H */
