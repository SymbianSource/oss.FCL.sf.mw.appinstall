/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent 
*/

#ifndef __CERTIFICATECHAIN_H__
#define __CERTIFICATECHAIN_H__
#pragma warning(disable: 4786)

#include "capabilities.h"
#include "siscertificatechain.h"
#include "dumpchainvaliditytool.h"
#include <math.h>
#include <openssl/x509_vfy.h>
#include <openssl/x509.h>

// Define OIDs for Symbian certificate constraints.
const std::string KDeviceIdListConstraint = "1.2.826.0.1.1796587.1.1.1.1" ;
const std::string KSidListConstraint = "1.2.826.0.1.1796587.1.1.1.4" ;
const std::string KVidListConstraint = "1.2.826.0.1.1796587.1.1.1.5" ;
const std::string KCapabilitiesConstraint = "1.2.826.0.1.1796587.1.1.1.6" ;

//Define ASN.1 Primitive Types and their Tags.
const int KDerUtf8StringTag = 0x0C ;
const int KDerSequenceTag = 0x30 ;
const int KDerIntegerTag = 0x02 ;
const int KDerBitStringTag = 0x03 ;

const int KByteLength = 8 ;
const int KDeviceIdLength = 22 ;
typedef std::vector <std::string>  StringVector ; 

class Options ;
class SWICertStore ;

/**
 * The CertificateChain class does the Certificate Chain validation against the certstore and 
 * displays which chains are validated by the certstore.
 * @internalComponent 
 */

class CertificateChain
{
public:

	enum ValidationStatus
	{
	EValidationSuccessful = 0 , 
	EValidityCheckFailure = 1 << 0 , 
	EBrokenChain = 1 << 1
	};

public:

	CertificateChain();

	~CertificateChain();

	/**
	  For each file(containing a certificate chain),the following validations are done
	  1.Signatures Validation
	  2.Validity Period Checking and 
	  3.Check whether the end entity certificate in the chain is self signed or a devcert
	  @param	aFilename	File containing certificate chain.
	  @param	aStore		Reference of SWICertstore object.
	  @return	true		validation successful. 
	 */

	bool ValidateChain(const Options& aOptions, const std::string& aFilename, const SWICertStore& aStore);
	
	/** Returns developer certificate status.
		@return		true	If it is Developer Certificate.
	  */
	bool IsDevCertificate() const { return iDevCertificate;}
	
	/** Returns the developer certificate capabilities .
	  */
	int GetDevCaps() const { return iDevCapabilities;}

	/** Returns the certificate validation status as defined in the Enum.
	  */
	int GetValidationStatus() const { return iValidationStatus; }

	/** Returns the signature of the root cert which has validated the certificate chain.
	  */	
	const std::string& GetValidatedRootSignatures() const { return iValidatedRootSignatures ; }

private:

	/**
	  Check whether the end entity certificate in the chain is  a developer cert.
	  @param	aCert	Reference of end entity certificate to be checked.
	  @return	true	for developer certificate.
	 */
	bool CheckIfDevCert(X509& aCert) ;

	/**
	  Check whether the end entity certificate in the chain is  self signed.
	  @param	aCert	Reference of end entity certificate to be checked.
	  @return	true	for self signed.
	 */
	bool CheckIfSelfSigned(X509& aCert);

	/**
	  Check whether the last certificate in the chain is  validated against the certstore.
	  @param	aCert		Reference of last certificate .
	  @param	aIssuer		Label of the root cert which validates the chain.
							This is populated when validating against certstore.
	  @return	true		if validated.
	 */
	bool ValidateWithCertstore(X509& aCert, const SWICertStore& aStore , std::string& aIssuer);

	/**
	  Displays the Certificate Chain.
	  @param	aSigningChain	Reference of vector containing the certificate chain.
	 */
	void DisplaySigningChain(const StringVector& aSigningChain);

	/**
	  Extracts the validity period information of a certificate .
	  @param	aFromDate	Certificate's validity period (Valid from value).
	  @param	aToDate		Certificate's validity period (Valid to value).
	 */
	void ExtractCertValidityPeriod(const ASN1_UTCTIME& aFromDate, const ASN1_UTCTIME& aToDate);

	/**
	  Extracts the capabilities(DER encoded) into text format.
	  @param	aString		ASN1_BIT_STRING containing the capabilities.
	  @return	string containing capabilities in text format.	
	 */	
	std::string ExtractCapabilities(const ASN1_BIT_STRING* aString);

	/**
	  Extracts the ASN.1_Integer(DER encoded) into text format
	  @param	aString		ASN1_BIT_STRING containing the ASN.1_Integer(DER encoded).
	  @return	string containing integer values in text format.	
	 */
	std::string ExtractASN1IntegerData(const ASN1_BIT_STRING* aString);

	/**
	  Extracts the DER_UTF8 string into text format.
	  @param	aString		ASN1_BIT_STRING containing the DER_UTF8 string.
	  @return	string containing device id's in text format.	
	 */
	std::string ExtractDeviceConstaints(const ASN1_BIT_STRING* aString);

	/**
	  Depending on the value of the aValidationStatus,this function displays whether
	  the certificate chain is valid or is a broken chain or any certificates in the chain is expired or both.
	  @param	aValidationStatus	value containing the validation status of the certificate chain.
	 */	
	void CheckForChainValidity(const int& aValidationStatus);

	/** Extracts the length of the ASN.1 Bit String.
		@param	aASN1BitString	ASN.1 Bit String whose length is to be determined.			
		@return	length of the ASN.1 bit string.
	 */
	unsigned long GetBitStringLength(unsigned char** aASN1BitString) ;
	
	
private:

	bool iDevCertificate ;
	int iDevCapabilities ;
	int iValidationStatus ;
	std::string iDevConstraints ;
	StringVector iCertificateChain ;
	std::string iValidatedRootSignatures ;
};

#endif