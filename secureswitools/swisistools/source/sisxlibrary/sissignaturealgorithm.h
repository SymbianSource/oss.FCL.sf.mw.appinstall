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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
* As specified in SGL.GT0188.251
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __SISSIGNATUREALGORITHM_H__
#define __SISSIGNATUREALGORITHM_H__

#include "sisstring.h"
#include "structure.h"

class CSISSignatureAlgorithm : public CStructure <CSISFieldRoot::ESISSignatureAlgorithm>

	{
public:
	typedef enum
		{
		EAlgNone,
		EAlgRSA,
		EAlgDSA,
		EAlgIllegal
		} TAlgorithm;

public:
	CSISSignatureAlgorithm ();
	CSISSignatureAlgorithm (const CSISSignatureAlgorithm& aInitialiser);

	virtual std::string Name () const;

public:
	void SetAlgorithm (const TAlgorithm aAlgorithm);
	TAlgorithm Algorithm () const;
	bool IsAlgorithmKnown () const;
	void Verify (const TUint32 aLanguages) const;
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;

protected:
	CSISString	iAlgorithmIdentifier;

	mutable TAlgorithm	iAlgorithm;
	};




inline CSISSignatureAlgorithm::CSISSignatureAlgorithm ()
		: iAlgorithm (EAlgNone)
	{
	InsertMember (iAlgorithmIdentifier); 
	}


inline CSISSignatureAlgorithm::CSISSignatureAlgorithm (const CSISSignatureAlgorithm& aInitialiser) :
		CStructure <CSISFieldRoot::ESISSignatureAlgorithm> (aInitialiser),
		iAlgorithm (aInitialiser.iAlgorithm),
		iAlgorithmIdentifier (aInitialiser.iAlgorithmIdentifier)
	{
	InsertMember (iAlgorithmIdentifier); 
	}


inline std::string CSISSignatureAlgorithm::Name () const
	{
	return "Signature Algorithm";
	}


inline bool CSISSignatureAlgorithm::IsAlgorithmKnown () const
	{
	return (iAlgorithm > EAlgNone) && (iAlgorithm < EAlgIllegal);
	}

inline void CSISSignatureAlgorithm::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	if (aVerbose)
		{
		aStream << L"Algorithm: ";
		iAlgorithmIdentifier.AddPackageEntry(aStream, aVerbose);
		aStream << std::endl;
		}
	}

#endif // __SISSIGNATUREALGORITHM_H__

