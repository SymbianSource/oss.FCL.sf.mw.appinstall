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
 @internalComponent
 @released
*/


#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

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


#include "utility_interface.h"
#include "siscertificatechain.h"
#include "certchaindata.h"
#include "exception.h"
#include "utility.h"
#include "certificateinfo.h"


CSisCertificateChain::CSisCertificateChain (CCertChainData& aSisCertChain): iSisCertChain(aSisCertChain) 
	{
	ConstructL();
	}

void CSisCertificateChain::ConstructL()
	{
	CSISFieldRoot::TFieldSize offset = 0;
	X509* x509 = GetX509 (offset);

	while(x509 != NULL)
		{
		CCertificateInfo* certInfo = new CCertificateInfo(x509);
		iCertificateList.push_back(certInfo);
		x509 = GetX509 (offset);
		}
	}

CSisCertificateChain::~CSisCertificateChain()
	{
	for(int i = 0; i < iCertificateList.size(); ++i)
		{
		delete iCertificateList[i];
		}
	iCertificateList.clear();
	}

X509* BlobToX509 (const CSISBlob& aBlob, CSISFieldRoot::TFieldSize& aOffset)
	{
	assert (aBlob.Size ());
	assert (aBlob.Size () < 0X7FFFFFFF);	// openssl limits
	X509* x509 = NULL;
	unsigned char* was = const_cast <unsigned char*> (aBlob.Data ());
	unsigned char* ptr = was;
	assert (ptr);
	ptr += aOffset;
	if (! d2i_X509 (&x509, &ptr, static_cast <int> (aBlob.Size ())))
		{
		throw CSISException (CSISException::ECrypto, "invalid certificate chain");
		}
	aOffset = (ptr - was);
	assert (x509);
	return x509;
	}

void X509ToBlob (CSISBlob& aBlob, X509* aX509)
	{
	TUint8* buffer = NULL;
	int size = i2d_X509 (aX509, &buffer);
	if ((size <= 0) || (buffer == NULL))
		{
		throw CSISException (CSISException::ECrypto, "invalid X509");
		}
	try
		{
		aBlob.Assign (buffer, static_cast <CSISFieldRoot::TFieldSize> (size));
		}
	catch (...)
		{
		OPENSSL_free (buffer);
		throw;
		}
	OPENSSL_free (buffer);
	}


void X509ToBlobAppend (CSISBlob& aBlob, X509* aX509)
	{
	TUint8* buffer = NULL;
	int size = i2d_X509 (aX509, &buffer);
	if ((size <= 0) || (buffer == NULL))
		{
		throw CSISException (CSISException::ECrypto, "invalid X509");
		}
	try
		{

#ifndef X509_CHAIN_ORDER_ROOT_FIRST
		aBlob.Append (buffer, static_cast <CSISFieldRoot::TFieldSize> (size));
#else
		aBlob.Prepend (buffer, static_cast <CSISFieldRoot::TFieldSize> (size));
#endif /* X509_CHAIN_ORDER_ROOT_FIRST */

		}
	catch (...)
		{
		OPENSSL_free (buffer);
		throw;
		}
	OPENSSL_free (buffer);
	}

void CSisCertificateChain::LoadText (const std::wstring& aName)
	{	
		char *fName = NULL;	
		std::ifstream certFile;
		std::string line;
		std::string buffer;

		certFile.rdbuf()->open(wstring2string (aName).c_str (), std::ios::in);

		if (!certFile.is_open())
			{
			if((fName = Copy2TmpFile(aName.c_str(), CERTFILE)) != NULL)
				{
				certFile.rdbuf()->open(fName, std::ios::in);
				}
			}
		
		//check if file is successfully opened.
		if(certFile.is_open())
			{
			//reads the file (pem certificate) into the buffer ignoring empty lines.
			while(!certFile.eof())
				{
				getline(certFile,line);
				//ignore blank lines.
				if(line.length())
					{
					buffer.append(line);
					buffer.append("\n");
					}
				}
			
			certFile.rdbuf()->close();
			}
		
		else
			{
			CSISException::ThrowIf (1, CSISException::EFileProblem, std::wstring (L"cannot open ") + aName);
			}

		if(fName != NULL)
		{
			DeleteFileA(fName);
			delete fName;
		}
		
		X509* x509 = NULL;
		BIO* mem = NULL;
		
		try
			{
			ERR_clear_error();
			//creates a memory BIO and writes the buffer data into it.
			mem = BIO_new(BIO_s_mem());
			BIO_puts(mem , buffer.c_str());
			while(PEM_read_bio_X509 (mem , &x509 ,0 ,NULL) != NULL)
				{
				X509ToBlobAppend (const_cast<CSISBlob&>(iSisCertChain.CertificateData()), x509);
				X509_free (x509);
				x509 = NULL;
				}
			BIO_free(mem); mem = NULL;
			if(iSisCertChain.CertificateData().Size() == 0)
				throw 0;
			}
		
		catch (...)
			{

			if (certFile.rdbuf()->is_open())
			{
			certFile.rdbuf()->close();
			}
			
			if (x509)
				{
				X509_free (x509);
				}
			
			if(mem)
				{
				BIO_free(mem);
				}
			
			iSisCertChain.CertificateData().Dispose ();
			throw CSISException (CSISException::ECrypto, std::wstring (L"Cannot read ") + aName);
			}
	
	}



void CSisCertificateChain::LoadBinary (const std::wstring& aName)

	{
	try
		{
		iSisCertChain.CertificateData().Load (aName);
		}
	catch (...)
		{
		throw CSISException (CSISException::ECrypto, aName + std::wstring (L" is invalid"));
		}
	}



void CSisCertificateChain::Load (const std::wstring& aName)
	{
	try
		{
		LoadText (aName);
		return;
		}
	catch (...)
		{
		try
			{
			LoadBinary (aName);
			return;
			}
		catch (...)
			{
			}
		throw;
		}
	}


X509* CSisCertificateChain::GetX509 (CSISFieldRoot::TFieldSize& aOffset) const
	{
	if (aOffset >= iSisCertChain.CertificateData().Size ())
		{
		return NULL;
		}

	return BlobToX509 (iSisCertChain.CertificateData(), aOffset);
	}


X509* CSisCertificateChain::GetBottomX509 () const
	{
	CSISFieldRoot::TFieldSize offset = 0;
	X509* reply = GetX509(offset);
	return reply;
	}


void CSisCertificateChain::ExtractCertificateChain (std::string& aCertFileName)
{
	ERR_clear_error();
	CSISFieldRoot::TFieldSize offset = 0;

	for(int i = 0; i < iCertificateList.size(); ++i)
		{
		iCertificateList[i]->ExtractCertificate(aCertFileName);
		}
	}


