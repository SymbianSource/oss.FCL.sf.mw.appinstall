/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif // _MSC_VER

#include <iostream>
#include <fstream>
#include <string>

#include "sissignature.h"
#include "siscontroller.h"

#include <openssl/dsa.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

#define RSA_IDENTIFIER				L"1.2.840.113549.1.1.5"
#define DSA_IDENTIFIER				L"1.2.840.10040.4.3"
#define DSA_PRIME_SIZE				1024
#define OLD_PEM_FORMAT_TAG			"Proc-Type"
#define ENCRYPTION_ALGORITHM_TAG	"DEK-Info"
#define PEM_HEADER					"----"

void CalculateHash (TSHADigest& aDigest, const TUint8* aBuffer, TUint32 aBufferSize)
	{
	assert (aBuffer);
	assert (! IsBadReadPtr (aBuffer, aBufferSize));
	memset (&aDigest, 0, SHA_DIGEST_LENGTH);
	SHA1 (aBuffer, aBufferSize, aDigest);
	}


CSignature::CSignature (CSignatureData& aSISSignature): 
		iSISSignature(aSISSignature),
		iSignatureAlgorithm(const_cast<CSISSignatureAlgorithm&>(iSISSignature.GetAlgorithm()))
	{
	}

CSignature::~CSignature()
	{
	}

void CSignature::Sign (const std::wstring& aPrivateKey, const std::wstring& aPassPhrase, const TUint8* aBuffer, const TUint32 aBufferSize)
	{
	assert (aBuffer);
	TSHADigest digest;
	CalculateHash (digest, aBuffer, aBufferSize);
	void* key = LoadKey (aPrivateKey, aPassPhrase);
	assert (key);
	switch (iSISSignature.GetAlgorithm().Algorithm())
		{
	case CSISSignatureAlgorithm::EAlgRSA :
		RSASign (digest, key);
		break;
	case CSISSignatureAlgorithm::EAlgDSA :
		DSASign (digest, key);
		break;
	default:
		throw CSISException (CSISException::ECrypto, "unknown algorithm");
		}
	}


void CSignature::DSASign (const TSHADigest& aDigest, void* aKey)
	{
	DSA* dsa = reinterpret_cast <DSA*> (aKey);
	assert (dsa);
	unsigned int written = 0;
	int status = 0;
	try 
		{
		unsigned int max = DSA_size (dsa);
		iSISSignature.SetDataByteCount(max);
		status = DSA_sign (NID_sha1, aDigest, SHA_DIGEST_LENGTH, const_cast<unsigned char*>(iSISSignature.Data()), &written, dsa);
		} 
	catch (...)
		{
		status = -1;
		}

	if (status <= 0)
		{
		DSA_free (dsa);
		throw CSISException (CSISException::ECrypto, L"cannot create DSA signature");
		}

	CSISException::ThrowIf (written > iSISSignature.DataSize (), CSISException::ECrypto, L"inconsistent DSA sizes");
	}

void CSignature::RSASign (const TSHADigest& aDigest, void* aKey)
	{
	RSA* rsa = reinterpret_cast <RSA*> (aKey);
	assert (rsa);
	unsigned int written = 0;
	int status = 0;
	try 
		{
		iSISSignature.SetDataByteCount(RSA_size (rsa));
		status = RSA_sign (NID_sha1, aDigest, SHA_DIGEST_LENGTH, const_cast<unsigned char*>(iSISSignature.Data()), &written, rsa);
		} 
	catch (...)
		{
		status = -1;
		}

	if (status <= 0)
		{
		RSA_free (rsa);
		throw CSISException (CSISException::ECrypto, L"cannot create RSA signature");
		}
	CSISException::ThrowIf (written > iSISSignature.DataSize (), CSISException::ECrypto, L"inconsistent RSA sizes");
	}


void CSignature::VerifySignature (X509* aX509, const TUint8* aBuffer, const TUint32 aBufferSize) const
	{
	assert (aBuffer);
	TSHADigest digest;

	CalculateHash (digest, aBuffer, aBufferSize);
	switch (iSISSignature.GetAlgorithm().Algorithm ())
		{
	case CSISSignatureAlgorithm::EAlgRSA :
		RSAVerify (digest, aX509);
		break;
	case CSISSignatureAlgorithm::EAlgDSA :
		DSAVerify (digest, aX509);
		break;
	default:
		throw CSISException (CSISException::ECrypto, "unknown algorithm");
		}
	}

void CSignature::DSAVerify (const TSHADigest& aDigest, X509* aX509) const
	{
	assert (aX509);
	EVP_PKEY* pubkey = X509_get_pubkey (aX509);
	if (! pubkey)
		{
		throw CSISException (CSISException::ECrypto, "no public key in certificate");
		}
	DSA* dsa = EVP_PKEY_get1_DSA (pubkey);
	if (! dsa)
		{
		throw CSISException (CSISException::ECrypto, "not a DSA certificate");
		}
	TUint8* sig = const_cast <TUint8*> (iSISSignature.Data());
	int status = DSA_verify (NID_sha1, aDigest, SHA_DIGEST_LENGTH, sig, iSISSignature.DataSize(), dsa);
	if (status <= 0)
		{
		throw CSISException (CSISException::EVerification, "invalid signature");
		}
	}

void CSignature::RSAVerify (const TSHADigest& aDigest, X509* aX509) const
	{
	assert (aX509);
	EVP_PKEY* pubkey = X509_get_pubkey (aX509);
	if (! pubkey)
		{
		throw CSISException (CSISException::ECrypto, "no public key in certificate");
		}
	RSA* rsa = EVP_PKEY_get1_RSA (pubkey);
	if (! rsa)
		{
		throw CSISException (CSISException::ECrypto, "not an RSA certificate");
		}
	TUint8* sig = const_cast <TUint8*> (iSISSignature.Data());
	int status = RSA_verify (NID_sha1, aDigest, SHA_DIGEST_LENGTH, sig, iSISSignature.DataSize(), rsa);
	if (! status)
		{
		throw CSISException (CSISException::EVerification, "invalid signature");
		}
	}


void* CSignature::LoadKey (const std::wstring& aName, const std::wstring& aPassPhrase)
	{
	try
		{
		return LoadTextKey (aName, aPassPhrase);
		}
	catch (...)
		{
		try
			{
			return LoadBinaryKey (aName, aPassPhrase);
			}
		catch (...)
			{
			}
		throw;
		}
	}

		// returns pointer to RSA or DSA, depending on algorithm.
void* CSignature::LoadBinaryKey (const std::wstring& aName, const std::wstring& aPassPhrase)
	{
	TUint64 size;
	HANDLE file = OpenFileAndGetSize (aName, &size);
	TUint8* buffer = NULL;
	void* reply = NULL;
	DSA* dsa = NULL;
	RSA* rsa = NULL;
	try
		{
		buffer = new TUint8 [size];
		ReadAndCloseFile (file, size, buffer);
		file = NULL;
		const TUint8* buf2 = buffer;
		bool unknown = ! iSignatureAlgorithm.IsAlgorithmKnown ();
		if (unknown || (iSignatureAlgorithm.Algorithm () == CSISSignatureAlgorithm::EAlgRSA))
			{
			ERR_clear_error();
			rsa = RSA_new ();
			if (! rsa)
				{
				std::cout << ERR_error_string (ERR_get_error (), NULL);
				throw 2;
				}
			if (! d2i_RSAPrivateKey (&rsa, &buf2, size))
				{
				if (! unknown)
					{
					std::cout << ERR_error_string (ERR_get_error (), NULL);
					}
				RSA_free (rsa);
				rsa = NULL;
				}
			else
				{
				if (unknown)
					{
					iSignatureAlgorithm.SetAlgorithm (CSISSignatureAlgorithm::EAlgRSA);
					unknown = false;
					}
				reply = rsa;
				}
			}
		if (unknown || (iSignatureAlgorithm.Algorithm () == CSISSignatureAlgorithm::EAlgDSA))
			{
			ERR_clear_error();
			dsa = DSA_generate_parameters (DSA_PRIME_SIZE, NULL, 0, NULL, NULL, NULL, NULL);
			if (! dsa)
				{
				std::cout << ERR_error_string (ERR_get_error (), NULL);
				throw 1;
				}
			if (! d2i_DSAPrivateKey (&dsa, &buf2, size))
				{
				std::cout << ERR_error_string (ERR_get_error (), NULL);
				DSA_free (dsa);
				dsa = NULL;
				}
			else
				{
				if (unknown)
					{
					iSignatureAlgorithm.SetAlgorithm (CSISSignatureAlgorithm::EAlgDSA);
					unknown = false;
					}
				reply = dsa;
				}
			}
		if (! reply || unknown)
			{
			throw 3;
			}
		delete [] buffer;
		}
	catch (...)
		{
		if (rsa)
			{
			RSA_free (rsa);
			}
		if (dsa)
			{
			DSA_free (dsa);
			}
		delete [] buffer;
		if (file)
			{
			::CloseHandle(file);
			}
		throw CSISException (CSISException::ECrypto, std::wstring (L"Cannot read ") + aName);
		}
	return reply;
	}

void* CSignature::LoadTextKey (const std::wstring& aName, const std::wstring& aPassPhrase)
	{
	std::ifstream keyFile;
	std::string line ;
	std::string buffer;
	bool oldPemFormat = false;
	char *fName = NULL;

	keyFile.rdbuf()->open(wstring2string (aName).c_str (), std::ios::in);

	if(!keyFile.is_open())
		{
		if((fName = Copy2TmpFile(aName.c_str(), CERTFILE)) != NULL)
			{
			keyFile.rdbuf()->open(fName, std::ios::in);
			}
		}

	//check if file is successfully opened.
	if(keyFile.is_open())
		{
		//scans the file until it encounters the first header.
		do
			{
			getline(keyFile,line);
			}while(!(line.find(PEM_HEADER , 0) != std::string::npos));

		buffer.append(line);
		buffer.append("\n");

		//scanning the file until the next non blank line is obtained.
		 do
			{
			getline(keyFile,line);
			}while(!(line.length()));

		//check whether the next non blank line contains the Proc Type Tag.
		if((line.find(OLD_PEM_FORMAT_TAG , 0)) != std::string::npos)
			{
			oldPemFormat = true;
			}

		buffer.append(line);
		buffer.append("\n");
	
		while(!keyFile.eof())
			{
			getline(keyFile , line);

			//skips any blank line between the Proc Type tag and the encryption algorithm tag
			//and appends a blank line.After which all the blank lines will be skipped.
			if(oldPemFormat)
				{
				if((line.find(ENCRYPTION_ALGORITHM_TAG , 0) != std::string::npos))
					{
					buffer.append(line);
					buffer.append("\n\n");
					}

				else if(line.length())
					{	
					buffer.append(line);
					buffer.append("\n");
					}			
				}

			else
				{
				//if key file in new format
				if(line.length())
					{
					buffer.append(line);
					buffer.append("\n");
					}
				}

			}
		}
	else
		{
		CSISException::ThrowIf (1, CSISException::EFileProblem, std::wstring (L"cannot open ") + aName);
		}

	BIO* mem = NULL;
	void* reply = NULL;
	char* pass = NULL;
	bool unknown = ! iSignatureAlgorithm.IsAlgorithmKnown ();
	try
		{
		//pass = new char [aPassPhrase.size () + 1];
		//strcpy (pass, wstring2string (aPassPhrase).c_str ());
		DWORD ilen = aPassPhrase.length();
		int targetLen = ConvertWideCharToMultiByte(aPassPhrase.c_str(), -1, NULL, 0, CP_UTF8);
		pass = new char[targetLen+1];
		ConvertWideCharToMultiByte(aPassPhrase.c_str(), -1, pass, targetLen+1, CP_UTF8);
		if (unknown || (iSignatureAlgorithm.Algorithm () == CSISSignatureAlgorithm::EAlgRSA))
			{
			ERR_clear_error();
			//creates a memory BIO and writes the buffer data into it.
			mem = BIO_new(BIO_s_mem());
			BIO_puts(mem , buffer.c_str());
			RSA* rsa = RSA_new ();
			
			if (! PEM_read_bio_RSAPrivateKey (mem, &rsa, NULL, pass))
				{
				if (! unknown)
					{
					std::cout << ERR_error_string (ERR_get_error (), NULL);
					}
				BIO_free(mem);
				mem = NULL;
				RSA_free (rsa);
				}
			else
				{
				if (unknown)
					{
					iSignatureAlgorithm.SetAlgorithm (CSISSignatureAlgorithm::EAlgRSA);
					unknown = false;
					}
				reply = rsa;
				}
			}
		if (unknown || (iSignatureAlgorithm.Algorithm () == CSISSignatureAlgorithm::EAlgDSA))
			{
			ERR_clear_error();
			//creates a memory BIO and writes the buffer data into it.
			mem = NULL;
			mem = BIO_new(BIO_s_mem());
			BIO_puts(mem , buffer.c_str());
			DSA* dsa = DSA_generate_parameters (DSA_PRIME_SIZE, NULL, 0, NULL, NULL, NULL, NULL);
			if (! PEM_read_bio_DSAPrivateKey (mem, &dsa, NULL, pass))
				{
				std::cout << ERR_error_string (ERR_get_error (), NULL);
				BIO_free(mem);
				mem = NULL;
				DSA_free (dsa);
				}
			else
				{
				if (unknown)
					{
					iSignatureAlgorithm.SetAlgorithm (CSISSignatureAlgorithm::EAlgDSA);
					unknown = false;
					}
				reply = dsa;
				}
			}
		if (unknown || ! reply)
			{
			throw 3;
			}
		}
	catch (...)
		{
		if (keyFile.rdbuf()->is_open())
			{
			keyFile.rdbuf()->close();
			}
		if(mem)
			{
			BIO_free(mem);
			}
		
		delete[] pass;
		throw CSISException (CSISException::ECrypto, std::wstring (L"Cannot load ") + aName);
		}
	
	BIO_free(mem);
	delete pass;
	return reply;
	}



