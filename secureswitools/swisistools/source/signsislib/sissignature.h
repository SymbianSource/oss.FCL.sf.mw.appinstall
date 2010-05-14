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
 @publishedPartner
 @released
*/

#ifndef __SISSIGNATURE_H__
#define __SISSIGNATURE_H__

#include "sissignaturealgorithm.h"
#include "signaturedata.h"
#include "sisblob.h"
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/x509.h>


typedef TUint8 TSHADigest [SHA_DIGEST_LENGTH];

/**
 * Calculate the hash of a given buffer. SHA1 is used to calculate the hash.
 * @param aDigest Buffer where calculated hash will be stored.
 * @param aBuffer Content for which the hash need to be calculated.
 * @param aBufferSize Content length
 * 
 * @internalComponent
 * @released
 */
void CalculateHash (TSHADigest& aDigest, const TUint8* aBuffer, TUint32 aBufferSize);


/**
 * This class represents SISSignature field of sis file format. 
 */
class CSignature
	{
public:

public:
	/**
	 * This class represents a signature.
	 * Actual signature data is contained in SISX library.
	 * CSignatureData represent the signature data (SISX).
	 * @param aSisCertChain reference to CCertChainData
	 */
	explicit CSignature (CSignatureData& aSISSignature);
	/**
	 * Cleanup owned resources.
	 */
	~CSignature();

public:
	/**
	 * Function to sign content using a private key. And the
	 * signature is stored in the class.
	 * 
	 * @param aPrivateKey private key to sign the content.
	 * @param aPassPhrase pass phrase by which the private key is encrypted.
	 * @param aBuffer content to be signed.
	 * @param aBufferSize content length.
	 */
	void Sign (const std::wstring& aPrivateKey, const std::wstring& aPassPhrase, const TUint8* aBuffer, const TUint32 aBufferSize);
	/**
	 * Function to veriify the signature. It will take the buffer and the public
	 * key to verify the stored signature(in the class).
	 * @param aX509 public key by which the signature is verified.
	 * @param aBuffer content for which the signature need to be verified.
	 * @param aBufferSize content length
	 */
	void VerifySignature (X509* aX509, const TUint8* aBuffer, const TUint32 aBufferSize) const;
	/**
	 * Gets the reference to the signature algorithm.
	 */
	inline const CSISSignatureAlgorithm& SignatureAlgorithm() const;
	
private:
	void DSASign (const TSHADigest& aDigest, void* aKey);
	void RSASign (const TSHADigest& aDigest, void* aKey);
	void DSAVerify (const TSHADigest& aDigest, X509* aX509) const;
	void RSAVerify (const TSHADigest& aDigest, X509* aX509) const;
	// returns pointer to RSA or DSA, depending on algorithm.
	void* LoadKey (const std::wstring& aName, const std::wstring& aPassPhrase);

private:
	CSignature (const CSignature& aInitialiser)
				:iSISSignature(aInitialiser.iSISSignature),
				iSignatureAlgorithm(aInitialiser.iSignatureAlgorithm){}
	void* LoadBinaryKey (const std::wstring& aName, const std::wstring& aPassPhrase);
	void* LoadTextKey (const std::wstring& aName, const std::wstring& aPassPhrase);
	
protected:
	CSignatureData&			iSISSignature;
	CSISSignatureAlgorithm&	iSignatureAlgorithm;
	};

inline const CSISSignatureAlgorithm& CSignature::SignatureAlgorithm() const
	{
	return iSignatureAlgorithm;
	}

#endif // __SISSIGNATURE_H__

