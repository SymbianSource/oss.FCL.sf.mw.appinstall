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
* SIGNATURECERTIFICATECHAIN.H
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#ifndef __SIGNATURECERTIFICATECHAIN_H__
#define __SIGNATURECERTIFICATECHAIN_H__

#include <vector>

#include "signaturedata.h"
#include "certchaindata.h"
#include "sisarray.h"
#include "certificateinfo.h"
#include "siscertificatechain.h"


class CSISController;
class CSignature;
class CSignatureCertChainData;

/**
 * This class represents SISSignatureCertificateChain field in 
 * SISX file format. Refer software install file format document 
 * for more details.
 * 
 * SISSignatureCertificateChain contains the signatures used to 
 * sign the SIS file and the certificate chain needed to
 * validate the signatures
 */
class CSisSignatureCertificateChain
	{
public:
	/**
	 * Constructor.
	 */
	explicit CSisSignatureCertificateChain (CSignatureCertChainData& aSisSignatureCertChain);
	/**
	 * Cleanup owned resources.
	 */
	~CSisSignatureCertificateChain();

public:
	/**
	 * Function creates an instance of CSignature class (which represents a signature)
	 * and then call its Sign method to actually sign the content. Then the instance 
	 * of CSignature is added in the already existing list of signatures.
	 *  
	 * @param aAlgorithm Algorithm by which the content needs to be signed.
	 * @param aCertificate public Key
	 * @param aPrivateKey private key to sign the content.
	 * @param aPassPhrase pass phrase by which the private key is encrypted.
	 * @param aBuffer content to be signed.
	 * @param aBufferSize content length.
	 */
	void Sign (	const CSISSignatureAlgorithm::TAlgorithm aAlgorithm, const std::wstring& aCertificate,
				const std::wstring& aPrivateKey, const std::wstring& aPassPhrase,  const TUint8* aBuffer, const TUint32 aBufferSize);
	/**
	 * Function to veriify the signature of the controller. 
	 * @param aController Controller whose signature needs to be verified.
	 * @param aParentHeaderSize controller's header position.
	 */
	void VerifySignature (const CSISController* aController, const TSISStream::pos_type aParentHeaderSize) const;

	/**
	 * Function to extract certificate chain. All the certificates present
	 * in the chain will be extracted into a single certificate file (.pem)
	 * @param aCertFileName Certificate chain file name
	 */

	void ExtractCertificateChain (std::string& aCertFileName);
	
	/**
	 * Retrieve the certificate chain. Where each certificate of the
	 * chain is represented by CCertificateInfo class.
	 */
	inline const std::vector<CCertificateInfo*>& CertChain() const;

private:
	CSisSignatureCertificateChain (const CSisSignatureCertificateChain& aInitialiser):iSisSignatureCertChain(aInitialiser.iSisSignatureCertChain)
	{}

protected:
	std::vector<CSignature*>	iSignatures;
	CSignatureCertChainData&	iSisSignatureCertChain;
	CSisCertificateChain*		iCertificateChain;
	};


inline void CSisSignatureCertificateChain::ExtractCertificateChain (std::string& aCertFileName)
	{
	iCertificateChain->ExtractCertificateChain (aCertFileName);
	}

inline const std::vector<CCertificateInfo*>& CSisSignatureCertificateChain::CertChain() const
	{
	return iCertificateChain->CertChain();
	}

#endif // __SIGNATURECERTIFICATECHAIN_H__

