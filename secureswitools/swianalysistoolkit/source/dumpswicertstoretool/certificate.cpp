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



#include "certificate.h"
#include <algorithm>

Certificate::Certificate(const Stream& aStream, StoreFile& aStore)
	:iStream(aStream),
	iStore(aStore)
	{
	}

bool Certificate::Read(const char* aCertName , const Options& aOptions)
	{
	bool writeToFile = false;
	Frames::Iterator f = iStream.Frame();
	do
		{
		FramePos pos = f->iPos;
		int len = f++ -> iDes.Length();
		if (len == 0)
			len = f -> iPos.Pos()- pos.Pos();
		
		char* buffer = new char[len];
		
		iStore.File().seekg(FileOffset(pos).Offset(),ios::beg);
		iStore.File().read(buffer,len);

		int aOffset = 0;
		X509* x509 = NULL;
		unsigned char* was = reinterpret_cast<unsigned char*>(buffer);
		unsigned char* ptr = was;
		assert (ptr);
		if (! d2i_X509 (&x509, &ptr, static_cast <int> (len)))
			{
			delete[]  buffer;
			throw EInvalidCertificateChain;
			}
		aOffset = (ptr - was);
		assert (x509);

		char buf [KLength];
		X509_NAME_oneline (X509_get_issuer_name (x509), buf, KLength);
		if (buf)
			{		
			iIssuerName.append(buf);
			}

		unsigned char* signature = x509->signature->data;
		int signatureLen = x509->signature->length;
		std::string certSignature((const char*)signature, signatureLen);
		//this vector contains the certificate signatures of writable certstore,when both ROM and writable certstore are specified
		if(aOptions.CBasedWritableCertstore() && aOptions.ROMCertstore() && SwiCertStoreMetaData::iCount == 1)
			{
			iCertSignature.push_back(certSignature);
			writeToFile = true;
			}
		else
			{
			StringVector::iterator certVector;
	    	StringVector::iterator start = iCertSignature.begin();
			StringVector::iterator last = iCertSignature.end();
			int size = iCertSignature.size();
			//in case the same certificate is present in both ROM Based and writable certstore
			//first their signatures are compared,and only those certificates which are only available in 
			//the ROM certstore are considered for display.
			certVector = std::find(start, last, certSignature);
			if(certVector == last)
				{
				writeToFile = true;
				}
			}
		
		if(writeToFile)
			{
			BIO* b64 = BIO_new(BIO_f_base64());
			if(aOptions.GenerateCertificate())
				{
				ofstream file_write (aCertName,ofstream::binary);
				file_write.write(buffer,len);
				file_write.close();
				}

			if(aOptions.CreateCCIFile())
				{
				BIO* bio = BIO_new_fp(stdout, BIO_NOCLOSE);
				if (!PEM_write_bio_X509(bio, x509 ))
					{
					cout << "error in pem " << endl;
					}
				BIO_free_all(bio);
				}
			if(aOptions.CreateDetailCCIFile())
				{
				BIO* bio = BIO_new_fp(stdout, BIO_NOCLOSE);
				if (!PEM_write_bio_X509(bio, x509 ))
					{
					cout << "error in pem " << endl;
					}
				BIO_free_all(bio);
				}
			BIO_free_all(b64);
			}
		
		delete [] buffer;
		} while (f->iDes.Type()==FrameDes::Continuation);

		if(writeToFile)
			{
			return true;
			}
		else
			{
			return false;
			}
	}

Certificate::~Certificate()
	{
	}

StringVector Certificate::iCertSignature;