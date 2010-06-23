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

#ifndef __SISHASH_H__
#define __SISHASH_H__


#include "structure.h"
#include "numeric.h"
#include "sisblob.h"
#include "sisfiledata.h"

class CSISHash : public CStructure <CSISFieldRoot::ESISHash>

	{
public:
	typedef enum
		{
		EHashNone,
		EHashSHA,
		EHashIllegal
		} 
	TSISHash;

private:
	void InsertMembers ();

public:
	CSISHash ();
	CSISHash (const CSISHash& aInitialiser);

	virtual void operator=(const CSISHash& aHash);
	virtual void Verify (const TUint32 aLanguages) const;
	virtual std::string Name () const;

	void SetHash(const TUint8* aHash, TUint32 aHashSize);
	bool VerifyAlgorithm () const;
	const CSISBlob& Blob() const;
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;

private:
	CSISUInt32	iAlgorithm;
	CSISBlob	iBlob;

	};


inline void CSISHash::InsertMembers ()
	{
	InsertMember (iAlgorithm);
	InsertMember (iBlob);
	}


inline CSISHash::CSISHash () :
		iAlgorithm (EHashSHA)
	{
	InsertMembers (); 
	}

inline const CSISBlob& CSISHash::Blob() const
	{
	return (CSISBlob&)iBlob;
	}


#endif // __SISHASH_H__

