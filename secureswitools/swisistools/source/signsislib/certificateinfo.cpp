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
 @internalComponent
 @released
*/

#include <iostream>

#include <openssl/rsa.h>
#include <openssl/dsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509v3.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>


#include "certificateinfo.h"


// Constants
const char* const KUTCTimeFormat = "YYMMDDHHMMSSZ";
const char* const KGeneralizedTimeFormat = "YYYYMMDDHHMMSSZ";
const int KUTCLength = 13;
const int KGeneralizedTimeLength = 15;

CCertificateInfo::CCertificateInfo (X509* aCertificate):
	iCertificate(aCertificate)
	{
	ConstructL();
	}

CCertificateInfo::~CCertificateInfo()
	{
	X509_free(iCertificate);
	}

void CCertificateInfo::ConstructL()
	{
	ASN1_TIME* from = X509_get_notBefore (iCertificate);
	SetDateTime(iValidFrom, from);
	ASN1_TIME* to = X509_get_notAfter (iCertificate);
	SetDateTime(iValidTo, to);
	
	EVP_PKEY *pKey = X509_get_pubkey(iCertificate);
	assert(pKey);

	iPublicKeyType = EUnknownKey;
	if(EVP_PKEY_type(pKey->type) == EVP_PKEY_DSA)
		{
		iPublicKeyType = EPubKeyDSA;
		}
	else if(EVP_PKEY_type(pKey->type) == EVP_PKEY_RSA)
		{
		iPublicKeyType = EPubKeyRSA;
		}

	if(pKey != NULL)
		EVP_PKEY_free(pKey);

	ExtractExtensions();
	}


void ParseConfValue(std::string& aString, std::vector<TConfValue>& aValueList)
	{
	std::string str = aString;
	while(str != "")
		{
		str = str.substr(str.find_first_not_of(' '));
		TConfValue conf;
		int pos = str.find(':');
		if(pos != std::string::npos)
			{
			conf.iName = str.substr(0, pos);
			str = str.substr(pos+1);
			}
		str = str.substr(str.find_first_not_of(' '));
		pos = str.find('\n');
		conf.iValue = str.substr(0, pos);
		if(std::string::npos == pos)
			{
			break;
			}
		str = str.substr(pos+1);
		aValueList.push_back(conf);
		}
	}

void CCertificateInfo::ExtractConfValues(STACK_OF(CONF_VALUE) *aConfList, TExtension& aExtInfo)
	{
	assert(aConfList);
	int confCount = sk_CONF_VALUE_num(aConfList);
	for(int j = 0; j < confCount; ++j)
		{
		CONF_VALUE *nval = sk_CONF_VALUE_value(aConfList, j);
		if(NULL == nval)
			{
			continue;
			}
		TConfValue confValue;
		if(nval->name)
			{
			confValue.iName = nval->name;
			}
		
		if(nval->value)
			{
			confValue.iValue = nval->value;
			}
		
		aExtInfo.iValueList.push_back(confValue);
		}			
	}

void CCertificateInfo::ExtractExtensions()
	{
	STACK_OF(X509_EXTENSION)* extensions = iCertificate->cert_info->extensions;
	int extCount = sk_X509_EXTENSION_num(extensions);
	
	for(int i = 0; i < extCount; ++i)
		{
		TExtension extInfo;
		X509_EXTENSION* ext = sk_X509_EXTENSION_value(extensions, i);
		ASN1_OBJECT* obj = X509_EXTENSION_get_object(ext);
		char extType[100];
		i2t_ASN1_OBJECT(extType, sizeof(extType), obj);
		extInfo.iExtensionName = extType;
		extInfo.iIsCritical = X509_EXTENSION_get_critical(ext) ? true: false;
		X509V3_EXT_METHOD *method = X509V3_EXT_get(ext);
		if(NULL == method)
			{
			extInfo.iValue = Get_ASN1_STRING_Data(ext->value);
			continue;
			}
		
		void *ext_str = X509V3_EXT_d2i(ext);
		if(NULL == ext_str)
			{
			continue;
			}
		
		if(method->i2s)
			{
			extInfo.iValue = method->i2s(method, ext_str);
			}
		else if(method->i2v)
			{
			STACK_OF(CONF_VALUE) *confList = method->i2v(method, ext_str, NULL);
			extInfo.iIsMultiLine = (method->ext_flags & X509V3_EXT_MULTILINE)? true: false;
			ExtractConfValues(confList, extInfo);
			}
		else if(method->i2r)
			{
			BIO *mem = BIO_new(BIO_s_mem());
			method->i2r(method, ext_str, mem, 12);
			BUF_MEM *bptr;
			BIO_get_mem_ptr(mem, &bptr);
			std::string str(bptr->data, bptr->length);
			ParseConfValue(str, extInfo.iValueList);
			BIO_free(mem);
			}
		else
			{
			assert(0);
			}
		
		iExtensions.push_back(extInfo);
		}
	
	}

void CCertificateInfo::SetDateTime(CSISDateTime& dateTime, ASN1_TIME* aASNTime)
	{
	TUint16 year = 0;
	if(KUTCLength == strlen((char*)aASNTime->data))
		{
		sscanf((char*)aASNTime->data, "%2d", &year);
		if(year > 50)
			{
			year += 1900;
			}
		else
			{
			year += 2000;
			}
		}
	if(KGeneralizedTimeLength == strlen((char*)aASNTime->data))
		{
		sscanf((char*)aASNTime->data, "%4d", &year);
		}

	int value = 0;
	sscanf((char*)&(aASNTime->data[2]), "%2d", &value);
	TUint8 month = value;
	sscanf((char*)&(aASNTime->data[4]), "%2d", &value);
	TUint8 day = value;
	dateTime.SetDate(year, month-1, day);
	sscanf((char*)&(aASNTime->data[6]), "%2d", &value);
	TUint8 hours = value;
	sscanf((char*)&(aASNTime->data[8]), "%2d", &value);
	TUint8 minutes = value;
	sscanf((char*)&(aASNTime->data[10]), "%2d", &value);
	TUint8 seconds = value;
	dateTime.SetTime(hours, minutes, seconds);	
	}

std::wstring CCertificateInfo::GetDistinguishedName (X509_NAME *x509Name, bool aGetFullName) const
	{
	std::wstring distinsuiName;
	bool firstPrinted = false;
	wchar_t* nameEntry = NULL;

	if(GetNameEntry(x509Name, LN_commonName, &nameEntry))
		{
		if(aGetFullName)
			{
			distinsuiName.append(L"CN="); 
			firstPrinted = true;
			}
		distinsuiName.append(nameEntry);
		delete[] nameEntry;
		}
	
	if(aGetFullName)
		{
		if(GetNameEntry(x509Name, LN_organizationalUnitName, &nameEntry))
			{
			if(firstPrinted) 
				{
				distinsuiName.append(L", ");
				}
			distinsuiName.append(L"OU=");
			distinsuiName.append(nameEntry);
			delete[] nameEntry;
			firstPrinted = true;
			}
		if(GetNameEntry(x509Name, LN_organizationName, &nameEntry))
			{	
			if(firstPrinted) distinsuiName.append(L", ");
			distinsuiName.append(L"O=");
			distinsuiName.append(nameEntry);
			delete[] nameEntry;
			firstPrinted = true;
			}
		if(GetNameEntry(x509Name, LN_countryName, &nameEntry))
			{	
			if(firstPrinted) distinsuiName.append(L", ");
			distinsuiName.append(L"C=");
			distinsuiName.append(nameEntry);
			delete[] nameEntry;
			firstPrinted = true;
			}
		if(GetNameEntry(x509Name, LN_pkcs9_emailAddress, &nameEntry))
			{
			if(firstPrinted) distinsuiName.append(L", ");
			distinsuiName.append(L"EM=");
			distinsuiName.append(nameEntry);
			delete[] nameEntry;
			}
		}
	return distinsuiName;
	}

std::wstring CCertificateInfo::IssuerName(bool aGetFullName) const
	{
	X509_NAME *x509Name = X509_get_issuer_name (iCertificate);
	
	std::wstring issuerName = GetDistinguishedName(x509Name, aGetFullName);
	
	return issuerName;
	}

std::wstring CCertificateInfo::SubjectName(bool aGetFullName) const
	{
	X509_NAME *x509Name = X509_get_subject_name (iCertificate);
	
	std::wstring subjectName = GetDistinguishedName(x509Name, aGetFullName);
	
	return subjectName;
	}

const CSISDateTime& CCertificateInfo::ValidFrom() const
	{
	return iValidFrom;
	}

const CSISDateTime& CCertificateInfo::ValidTo() const
	{
	return iValidTo;
	}

int CCertificateInfo::Version() const
	{
	return X509_get_version(iCertificate);
	}

std::string CCertificateInfo::SerialNumber() const
	{
	std::string serialNo;
	
	//Get serial number
	ASN1_INTEGER* asnNum = X509_get_serialNumber(iCertificate);

	if(asnNum->length <= 4)
		{
		char sno[30];
		int sNum = ASN1_INTEGER_get(asnNum);
		
		if(sNum < 0)
			{
			sNum = -1;
			serialNo.append("-");
			}
	
		sprintf(sno, " %d (0x%x)", sNum, sNum);
		serialNo.append(sno);
		}
	else
		{
		if(asnNum->type == V_ASN1_NEG_INTEGER)
			{
			serialNo = " (Negative)";
			}

		for(int i = 0; i < asnNum->length; i++)
			{
			char sno[10];
			sprintf(sno, "%02x", (int)asnNum->data[i]);
			serialNo.append(sno);
			if(i < asnNum->length - 1)
				{
				serialNo.append(":");
				}
			}
		}
	
	return serialNo;
	}

std::string CCertificateInfo::SignatureAlgo() const
	{
	std::string algo = OBJ_nid2ln(OBJ_obj2nid(iCertificate->sig_alg->algorithm));
	return algo;
	}

std::string CCertificateInfo::PublicKeyAlgo() const
	{
	std::string algo = OBJ_nid2ln(OBJ_obj2nid(iCertificate->cert_info->key->algor->algorithm));
	return algo;
	}

int CCertificateInfo::GetNameEntry(X509_NAME* aName, char *aKey, wchar_t** aNameEntry) const
	{
	if(!aName || !aKey)
		return 0;


	int nid = 0;
	
	if((nid = OBJ_txt2nid(aKey)) == NID_undef)
		return 0;

	int index = X509_NAME_get_index_by_NID(aName, nid, -1);

	if(index == -1)
		return 0;


	X509_NAME_ENTRY* nameEnt = X509_NAME_get_entry(aName, index);
	ASN1_STRING* ASN1Str = X509_NAME_ENTRY_get_data(nameEnt);
	
	BYTE* UTF8Str = NULL;
	
	if(ASN1_STRING_to_UTF8(&UTF8Str, ASN1Str) < 0)
	{
		if (M_ASN1_STRING_type(ASN1Str) == V_ASN1_UTF8STRING)
		{
			UTF8Str = ASN1_STRING_data(ASN1Str);
		}
		else 
		{
			return 0;
		}
	}
	
	int len = ConvertMultiByteToWideChar((LPSTR)UTF8Str, strlen((LPSTR)UTF8Str)+1, NULL, 0);
	*aNameEntry = new wchar_t[len*sizeof(wchar_t)];
	ConvertMultiByteToWideChar((LPSTR)UTF8Str, strlen((LPSTR)UTF8Str)+1, *aNameEntry, len);
	return 1;
	}

std::string CCertificateInfo::Get_ASN1_STRING_Data(ASN1_STRING *aASN1String)
	{
	if(NULL == aASN1String)
		{
		return NULL;
		}

	int length = aASN1String->length;
	std::string ASN_String;
	unsigned char* ptr = aASN1String->data;
	
	for (int i = 0; i < aASN1String->length; ++i)
		{
		if ((ptr[i] > '~') || ((ptr[i] < ' ') && (ptr[i] != '\n') && (ptr[i] != '\r')))
			ASN_String.append(".");
		else
			ASN_String.append(1, ptr[i]);
		}
	return ASN_String;
	}

const std::vector<TExtension>& CCertificateInfo::Extensions() const
	{
	return iExtensions;
	}

void CCertificateInfo::PrintPublicKey(std::ostream& aStream, int aIndent) const
	{
	EVP_PKEY *pKey = X509_get_pubkey(iCertificate);
	assert(pKey);
	
	BIO *mem = BIO_new(BIO_s_mem());

	switch(iPublicKeyType)
		{
		case EPubKeyDSA:
			{
			aStream << "" << "DSA Public Key:" << std::endl;
			struct dsa_st *pDSA = EVP_PKEY_get1_DSA(pKey);
			DSA_print(mem, pDSA, 28);
			break;
			}
	
		case EPubKeyRSA:
			{
			aStream.width(aIndent);
			aStream << "" ;
			aStream << "RSA Public Key: (" << std::dec << BN_num_bits(pKey->pkey.rsa->n) << " bit)"<< std::endl;
			struct rsa_st *pRSA = EVP_PKEY_get1_RSA(pKey);
			RSA_print(mem, pRSA, 16);
			break;
			}
		}

	BUF_MEM *bptr = NULL;
	BIO_get_mem_ptr(mem, &bptr);
	std::string str(bptr->data, bptr->length);
	aStream << str;

	BIO_free(mem);

	if(pKey != NULL)
		EVP_PKEY_free(pKey);
	}

void CCertificateInfo::PrintSignature(std::ostream& aStream, int aIndent) const
	{
	BIO *mem = BIO_new(BIO_s_mem());
	X509_signature_print(mem, iCertificate->sig_alg, iCertificate->signature);
	BUF_MEM *bptr = NULL;
	BIO_get_mem_ptr(mem, &bptr);
	std::string str(bptr->data, bptr->length);
	aStream << str << std::endl;
	BIO_free(mem);
	}

void CCertificateInfo::ExtractCertificate(std::string& aCertFileName) const
	{
	BIO* bio = BIO_new_file(aCertFileName.c_str(), "a");
	if(bio)
		{
		if (!PEM_write_bio_X509(bio, iCertificate ))
			{
			throw "Error While Creating pem file";
			}
		BIO_free(bio);
		}
	else
		{
		throw "Unable to create Certificate File";
		}
	}

bool CCertificateInfo::IsNIDPresent(int aNID) const
	{
	int val = X509_get_ext_by_NID(iCertificate, aNID, -1);
	return (val == -1)? false: true;
	}


// END of FILE
