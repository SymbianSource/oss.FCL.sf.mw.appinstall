/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CERTIFICATEINFO_H__
#define __CERTIFICATEINFO_H__

#include <openssl/x509.h>
#include "sisdatetime.h"

// Forward declarations
struct TConfValue
	{
	std::string	iName;
	std::string	iValue;
	};

struct TExtension
	{
	std::string				iExtensionName;
	bool					iIsCritical;
	bool					iIsMultiLine;
	std::string				iValue;
	std::vector<TConfValue>	iValueList;
	};

enum TPublicKeyType
	{
	EUnknownKey,
	EPubKeyDSA,
	EPubKeyRSA
	};

/**
 * This class extracts certificate details from a X509 certificate.
 */
class CCertificateInfo
	{
	friend class CCertificateChain;

public:
	/**
	 * Constructor will take X509 class pointer. Then it will
	 * extract certificate details from the class.
	 */
	explicit CCertificateInfo (X509* aCertificate);
	/**
	 * Free up owned resources
	 */
	~CCertificateInfo();
	
public:
	/**
	 * Get the instance of X509 certificate class.
	 */
	inline const X509* Certificate() const;
	/**
	 * Issuer Name - Issuer of the certificate.
	 * @param aGetFullName If true it will return detail name else only
	 * 			common name (CN) will be returned.
	 * @return Issuer name
	 */
	std::wstring IssuerName(bool aGetFullName = false) const;
	/**
	 * Subject Name - Name of the entity to whom the certificate was issued.
	 * @param aGetFullName If true it will return detail name else only
	 * 			common name (CN) will be returned.
	 * @return Subject name
	 */
	std::wstring SubjectName(bool aGetFullName = false) const;
	/**
	 * Get the time from which the certificate is valid.
	 */
	const CSISDateTime& ValidFrom() const;
	/**
	 * Get the time till which the certificate is valid.
	 */
	const CSISDateTime& ValidTo() const;
	/**
	 * Certificate version. Version number starts with 0.
	 * Therefore certifcate having version 1 will return (0),
	 * certificate with version 3 will return 2.
	 * @return Certificate version.
	 */
	int Version() const;
	/**
	 * Serial number of the certificate
	 */
	std::string SerialNumber() const;
	/**
	 * Algorithm used to sign the certificate.
	 */
	std::string SignatureAlgo() const;
	/**
	 * Algorithm used for key generation
	 */
	std::string PublicKeyAlgo() const;
	/**
	 * Function to retrieve extensions present in the certificate.
	 * @return vector of TExtension class. TExtension class provide detail information 
	 * 			of extension. 
	 */
	const std::vector<TExtension>& Extensions() const;
	/**
	 * Return the public key type.
	 */
	inline TPublicKeyType PublicKeyType() const;
	/**
	 * This function will write the public key into the stream provided.
	 * @param aStream where the public key information need to be written.
	 * @param aIndent Indentation size.
	 */
	void PrintPublicKey(std::ostream& aStream, int aIndent = 0) const;
	/**
	 * This function will write the signature into the stream provided.
	 * @param aStream where the signature need to be written.
	 * @param aIndent Indentation size.
	 */
	void PrintSignature(std::ostream& aStream, int aIndent = 0) const;
	/**
	 * Write the certificate into a file.
	 * @param aCertFileName Certificate file name
	 */ 
	void ExtractCertificate(std::string& aCertFileName) const;
	/**
	 * Check if a particular NID is present in the certificate.
	 * @return true if the NID is present else false.
	 */
	bool IsNIDPresent(int aNID) const;
	
private: // Private Member Functions
	void ConstructL();
	int GetNameEntry(X509_NAME* aName, char *aKey, wchar_t** aNameEntry) const;
	std::wstring GetDistinguishedName(X509_NAME *x509Name, bool aGetFullName) const;
	void SetDateTime(CSISDateTime& dateTime, ASN1_TIME* aASNTime);
	void ExtractExtensions();
	void ExtractConfValues(STACK_OF(CONF_VALUE) *confList, TExtension& aExtInfo);
	std::string Get_ASN1_STRING_Data(ASN1_STRING *aASN1String);
	
private:
	X509*					iCertificate;
	CSISDateTime			iValidFrom;
	CSISDateTime			iValidTo;
	TPublicKeyType			iPublicKeyType;	
	std::vector<TExtension>	iExtensions;
	};

inline const X509* CCertificateInfo::Certificate() const
	{
	return iCertificate;
	}

inline TPublicKeyType CCertificateInfo::PublicKeyType() const
	{
	return iPublicKeyType;
	}



#endif // __CERTIFICATEINFO_H__

