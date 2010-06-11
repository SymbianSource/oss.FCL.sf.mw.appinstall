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

#ifndef __SISCERTIFICATECHAIN_H__
#define __SISCERTIFICATECHAIN_H__


#include "sisblob.h"

#include <vector>

using std::vector;

// Forward declarations
class CCertChainData;
class CCertificateInfo;

class CSisCertificateChain
	{
public:
	/**
	 * Wrapper class which represents SISCertificateChain.
	 * Actual certificate data is contained in SISX library.
	 * CCertChainData represent the cert chain data (SISX).
	 * @param aSisCertChain reference to CCertChainData
	 */
	explicit CSisCertificateChain (CCertChainData& aSisCertChain);
	/**
	 * Free up owned resources
	 */
	~CSisCertificateChain();
	/**
	 * Class Name
	 */
	virtual std::string Name () const;

public:
	/**
	 * Function will load the certificate into memory.
	 * @param aName certificate file name.
	 */
	void Load (const std::wstring& aName);
	
	/**
	 * This function will extract the signing chain certificates (1 pem file per chain).
	 * The files will me named as cert1.pem, cert2.pem etc. All the certificates 
	 * represented by this chain will be appended in the same pem file. File name of 
	 * the certificate chain is based on the certificate chain index passed to it.
	 * 
	 * @param aChainIndex Certificate Chain index.
	 * @return void
	 */
	void ExtractCertificateChain (std::string& aCertFileName);
	/**
	 * Function will retrieve a certificate from the chain.
	 * 
	 * @param aOffset offset of certificate within the chain. It will be updated
	 * 			after the function call to point to the next certificate in the
	 * 			chain.
	 * @return return X509 certificate instance. Caller is responsible for 
	 * 			calling X509_free on the returned data.
	 */
	X509* GetX509 (CSISFieldRoot::TFieldSize& aOffset) const; 
	/**
	 * Function will retrieve the bottom most certificate from the chain.
	 * 
	 * @return return X509 certificate instance. Caller is responsible for 
	 * 			calling X509_free on the returned data.
	 */
	X509* GetBottomX509 () const;  // calling code responsible for calling X509_free

	/**
	 * Extract the complete certificate chain. Where each certificate is 
	 * represented by CCertificateInfo class.
	 */
	inline const std::vector<CCertificateInfo*>& CertChain() const;

private:
	CSisCertificateChain (const CSisCertificateChain& aInitialiser):iSisCertChain(aInitialiser.iSisCertChain){}
	void ConstructL();
	
	void LoadText (const std::wstring& aName);
	void LoadBinary (const std::wstring& aName);
	
private:
	CCertChainData&	iSisCertChain;
	std::vector<CCertificateInfo*>	iCertificateList;
	};


inline std::string CSisCertificateChain::Name () const
	{
	return "Certificate Chain";
	}

inline const std::vector<CCertificateInfo*>& CSisCertificateChain::CertChain() const
	{
	return iCertificateList;
	}

#endif // __SISCERTIFICATECHAIN_H__

