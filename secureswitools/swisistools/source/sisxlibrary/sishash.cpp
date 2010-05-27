/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
*
*/


/**
 @file 
 @internalComponent
 @released
*/
#include <iostream>
#include <openssl/sha.h>

#include "symbiantypes.h"
#include "sishash.h"

CSISHash::CSISHash (const CSISHash& aInitialiser) :
		CStructure <CSISFieldRoot::ESISHash> (aInitialiser),
		iAlgorithm (aInitialiser.iAlgorithm),
		iBlob (aInitialiser.iBlob)
	{
	InsertMembers (); 
	}


void CSISHash::Verify (const TUint32 aLanguages)  const

	{
	CStructure <CSISFieldRoot::ESISHash>::Verify (aLanguages);
	CSISException::ThrowIf (iAlgorithm != EHashSHA, CSISException::EVerification, "unsupported hash algorithm");
	}


std::string CSISHash::Name () const
	{
	return "Hash";
	}

bool CSISHash::VerifyAlgorithm () const
	{
	// Only EHashSHA supported
	if (iAlgorithm != EHashSHA)
		{
		return false;
		}
	return true;
	}
		
void CSISHash::operator=(const CSISHash& aHash)
	{
	const TUint8* data = aHash.Blob().Data();
	int size = aHash.Blob().Size();
	iBlob.Assign (data, size);
	}

void CSISHash::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	if(aVerbose)
		{
		aStream << L"; " << ((iAlgorithm.Value()==EHashSHA)?L"SHA-1: ":L"Unknown hash: ");
		iBlob.AddPackageEntry(aStream, aVerbose);
		aStream << std::endl;
		}
	}

void CSISHash::SetHash(const TUint8* aHash, TUint32 aHashSize)
	{
	iBlob.Assign(aHash, aHashSize);
	}


