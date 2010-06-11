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


#ifndef	SISREGISTRYPACKAGE_H
#define	SISREGISTRYPACKAGE_H

#include <istream>
#include "serialisable.h"
#include "symbiantypes.h"


class Serialiser;
class Deserialiser;
/**
* @file SISREGISTRYPACKAGE.H
*
* @internalComponent
* @released
*/
class SisRegistryPackage : public Serialisable
	{
public:
	SisRegistryPackage ();

	SisRegistryPackage(
		TUint32 aIndex,
		const std::wstring& aVendorName,
		const std::wstring& aPackageName,
		TUint32 aPackageUid);

	virtual ~SisRegistryPackage ();

	virtual void Internalize(Deserialiser& des);
	virtual void Externalize(Serialiser& ser);
	//virtual Serialisable* Make() { return new SisRegistryPackage; }

	TInt GetIndex() const
	{ return iIndex; }

	void SetIndex(TInt& aIndex)
	{ iIndex = aIndex; }

	const std::wstring& GetVendorName() const
	{ return *iVendorName; }

	void SetVendorName(const std::wstring& aVendorName)
	{ *iVendorName = aVendorName; }

	const std::wstring& GetPackageName() const
	{ return *iPackageName; }

	void SetPackageName(const std::wstring& aPackageName)
	{ *iPackageName = aPackageName; }

	TUint32 GetUid() const
	{ return iUid; }

	void SetUid(TUint32& aUid)
	{ iUid = aUid; }

protected:
	TUint32       iUid;
	std::wstring* iPackageName;
	std::wstring* iVendorName;
	TInt        iIndex;
	};



#ifdef _MSC_VER
#include <vector>
Deserialiser& operator>>(Deserialiser& aInput, std::vector<SisRegistryPackage*>& val);

#endif

#endif	/* SISREGISTRYPACKAGE_H */
