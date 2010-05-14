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

#ifndef __SIGNATUREDATA_H__
#define __SIGNATUREDATA_H__

#include "sissignaturealgorithm.h"
#include "sisblob.h"
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

class CSignatureData : public CStructure <CSISFieldRoot::ESISSignature>

	{
private:
	typedef TUint8 TSHADigest [SHA_DIGEST_LENGTH];

private:
	void InsertMembers ();

public:
	inline void SetAlgorithm (const CSISSignatureAlgorithm::TAlgorithm aAlgorithm);
	inline const CSISSignatureAlgorithm& GetAlgorithm();

public:
	CSignatureData ();
	CSignatureData (const CSignatureData& aInitialiser);

	inline virtual std::string		Name () const;
	inline const TUint8*			Data();
	inline int						DataSize() const;
	inline void					SetDataByteCount(unsigned int aSize);
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;

protected:
	CSISSignatureAlgorithm	iSignatureAlgorithm;
	CSISBlob				iData;
	};


inline void CSignatureData::InsertMembers ()
	{
	InsertMember (iSignatureAlgorithm);
	InsertMember (iData);
	}


inline CSignatureData::CSignatureData ()
	{
	InsertMembers (); 
	}

inline CSignatureData::CSignatureData (const CSignatureData& aInitialiser) :
		CStructure <CSISFieldRoot::ESISSignature> (aInitialiser),
		iSignatureAlgorithm (aInitialiser.iSignatureAlgorithm),
		iData (aInitialiser.iData)
	{
	InsertMembers (); 
	}

inline std::string CSignatureData::Name () const
	{
	return "Signature";
	}

inline void CSignatureData::SetAlgorithm (const CSISSignatureAlgorithm::TAlgorithm aAlgorithm)
	{
	iSignatureAlgorithm.SetAlgorithm (aAlgorithm);
	}

inline const TUint8* CSignatureData::Data()
	{
	return iData.Data();
	}

inline int CSignatureData::DataSize() const
	{
	return static_cast<int>(iData.Size());
	}

inline void CSignatureData::SetDataByteCount(unsigned int aSize)
	{
	iData.SetByteCount(aSize);
	}

inline const CSISSignatureAlgorithm& CSignatureData::GetAlgorithm()
	{
	return iSignatureAlgorithm;
	}

inline void CSignatureData::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	if (aVerbose)
		{
		aStream << L"; Signature:"; 
		iSignatureAlgorithm.AddPackageEntry(aStream, aVerbose);
		aStream << L";";
		iData.AddPackageEntry(aStream, aVerbose);
		}
	}

#endif // __SIGNATUREDATA_H__

