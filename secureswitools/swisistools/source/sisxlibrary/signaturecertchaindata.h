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
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __SIGNATURECERTCHAINDATA_H__
#define __SIGNATURECERTCHAINDATA_H__

#include "signaturedata.h"
#include "certchaindata.h"
#include "sisarray.h"


class CSISController;


class CSignatureCertChainData : public CStructure <CSISFieldRoot::ESISSignatureCertificateChain>
	{
private:
	void InsertMembers ();

public:
	CSignatureCertChainData ();
	CSignatureCertChainData (const CSignatureCertChainData& aInitialiser);

	virtual std::string Name () const;

public:
	inline CSISArray <CSignatureData, CSISFieldRoot::ESISSignature>	& Signatures();
	inline const CCertChainData& CertificateChain() const;
	inline void AddSignature(CSignatureData& aSignature);
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;

protected:
	CSISArray <CSignatureData, CSISFieldRoot::ESISSignature>	iSignatures;
	CCertChainData									iCertificateChain;
	};



inline CSISArray <CSignatureData, CSISFieldRoot::ESISSignature>& CSignatureCertChainData::Signatures()
	{
	return iSignatures;
	}

inline void CSignatureCertChainData::InsertMembers ()
	{
	InsertMember (iSignatures);
	InsertMember (iCertificateChain);
	}


inline CSignatureCertChainData::CSignatureCertChainData ()
	{
	InsertMembers ();
	}


inline CSignatureCertChainData::CSignatureCertChainData (const CSignatureCertChainData& aInitialiser) :
		CStructure <CSISFieldRoot::ESISSignatureCertificateChain> (aInitialiser),
		iSignatures (aInitialiser.iSignatures),
		iCertificateChain (aInitialiser.iCertificateChain)
	{
	InsertMembers ();
	}


inline std::string CSignatureCertChainData::Name () const
	{
	return "Signature Certificate Chain";
	}

inline const CCertChainData& CSignatureCertChainData::CertificateChain() const
	{
	return iCertificateChain;
	}

inline void CSignatureCertChainData::AddSignature(CSignatureData& aSignature)
	{
	iSignatures.Push(aSignature);
	}

inline void CSignatureCertChainData::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	if (aVerbose)
		{
		aStream << L"; Signing chain:" << std::endl;
		iCertificateChain.AddPackageEntry(aStream, aVerbose);
		iSignatures.AddPackageEntry(aStream, aVerbose);
		}
	}

#endif // __SIGNATURECERTCHAINDATA_H__

