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


#include "sisregistrypackage.h"
#include "deserialiser.h"
#include "serialiser.h"

SisRegistryPackage::SisRegistryPackage ()
	{
	iPackageName = new std::wstring();
	iVendorName = new std::wstring();
	}

SisRegistryPackage::SisRegistryPackage(
	TUint32 aIndex, 
	const std::wstring& aVendorName,
	const std::wstring& aPackageName,
	TUint32 aPackageUid) 
	: iIndex(aIndex), iUid(aPackageUid)
{
	iPackageName = new std::wstring(aPackageName);
	iVendorName = new std::wstring(aVendorName);
}


SisRegistryPackage::~SisRegistryPackage ()
	{
	delete iPackageName;
	delete iVendorName;
	}

void SisRegistryPackage::Internalize(Deserialiser& des)
	{
	des >> iUid
	    >> *iPackageName
	    >> *iVendorName
	    >> iIndex;
	}
void SisRegistryPackage::Externalize(Serialiser& ser)
	{
	ser << iUid << *iPackageName << *iVendorName << iIndex;
	}

#ifdef _MSC_VER

Deserialiser& operator>>(Deserialiser& aInput, std::vector<SisRegistryPackage*>& val)
	{
	TUint32 size = 0;
	aInput>> size;
	val.resize(size);
	for (TUint32 i = 0; i < size ; ++i)
		{
		val[i] = new SisRegistryPackage;
		aInput >> *val[i];
		}
	return aInput;
	}
#endif 