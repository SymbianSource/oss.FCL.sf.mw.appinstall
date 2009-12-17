/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of the CCertificateRequestGenerator class
* INCLUDES
*
*/


#include "CertificateRequestGenerator.h"


// ===========================================================================
// Construction/Destruction
// ===========================================================================

CCertificateRequestGenerator::CCertificateRequestGenerator()
	{
	memset(m_RequestFile, 0, MAXLEN);
	}

CCertificateRequestGenerator::CCertificateRequestGenerator(_TCHAR* aDnPtr)
	{
	}

CCertificateRequestGenerator::~CCertificateRequestGenerator()
	{
	}

void CCertificateRequestGenerator::SetRequestName(_TCHAR* aStr)
	{
	if(aStr != NULL)
		{
		_tcscpy(m_RequestFile, aStr);
		}
	}

int CCertificateRequestGenerator::Generate()
//Generate certificate request and write into a file
	{
	FILE*		  fp			= NULL;
	char*		  pbPassword	= NULL;
	EVP_PKEY* 	  pKey			= NULL;
	X509_REQ*	  pReq			= NULL;
	X509_NAME*	  pSubj			= NULL;
	const EVP_MD* pDigest		= NULL;
	DWORD		  bytesWritten;
	struct entry_pack* pEntPack = NULL;

	int retFunc	= FAIL;

	//Get command prompt handle
	HANDLE hndl = GetStdHandle(STD_OUTPUT_HANDLE);
	
	OPENSSL_add_all_algorithms_conf();
	ERR_load_crypto_strings();

	//First read private key from key file
	if(!(fp = _tfopen(m_privateKeyFile, _T("r"))))
		{
		PrintErrorInfo("Error reading key file!", EGeneric, constparams);
		WriteConsole(hndl, m_privateKeyFile, wcslen(m_privateKeyFile), &bytesWritten, 0);
		return retFunc;
		}

	if(m_password[0] != 0)
		{
		DWORD len = 0;
		len = _tcslen(m_password);
		pbPassword = MakeMBCSString(m_password, CP_UTF8, len);
		pKey = PEM_read_PrivateKey(fp, NULL, NULL, pbPassword);
		delete pbPassword;
		}
	else
		{
		pKey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
		}
			
	fclose(fp); fp = NULL;

	if(!pKey)
		{
		PrintErrorInfo("Error reading private key in key file!", EOPENSSL, constparams);
		return retFunc;
		}
	try
		{
		//Create a new cert request and add the public key into it
		if(!(pReq = X509_REQ_new()))
			{
			PrintErrorInfo("Error creating X509 request object!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		X509_REQ_set_pubkey(pReq, pKey);

		//Now create DN name entries and assign them to request
		if(!(pSubj = X509_NAME_new()))
			{
			PrintErrorInfo("Error creating X509 name object!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		//Format DN string
		DoFormatted(m_dname, &pEntPack);

		if(pEntPack->num == 0)
			{
			PrintErrorInfo("Error formatting Distinguished Name!", EGeneric, constparams);
			throw EGeneric;
			}

		for (int i = 0; i < pEntPack->num; i++)
			{
			int nid = 0;
			DWORD lent = 0;
			X509_NAME_ENTRY *pEnt = NULL;
			LPSTR pbMBSTRUTF8 = NULL;

			if((pEntPack->entries[i].value == NULL) || (pEntPack->entries[i].key == NULL))
				{
				PrintErrorInfo("Error in Distinguished Name construction!", EGeneric, constparams);
				throw EGeneric;
				}

			if((nid = OBJ_txt2nid(pEntPack->entries[i].key)) == NID_undef)
				{
				PrintErrorInfo("Error finding NID for a DN entry!", EOPENSSL, constparams);
				throw EOPENSSL;
				}
			lent = _tcslen(pEntPack->entries[i].value);
			pbMBSTRUTF8 = MakeMBCSString(pEntPack->entries[i].value, CP_UTF8, lent);

			if(lent > 64) //OpenSSL does not accept a string longer than 64 
				{
				if(!(pEnt = X509_NAME_ENTRY_create_by_NID(NULL, nid, MBSTRING_UTF8, (unsigned char *)"dummy", 5)))
					{
					PrintErrorInfo("Error creating name entry from NID!", EOPENSSL, constparams);
					throw EOPENSSL;
					}
				
				pEnt->value->data = (unsigned char *)malloc(lent+1);
				
				for(DWORD j=0; j<lent; j++ )
					{
					pEnt->value->data[j] = pbMBSTRUTF8[j];
					}
				
				pEnt->value->length = lent;

				} 
			else if(!(pEnt = X509_NAME_ENTRY_create_by_NID(NULL, nid, MBSTRING_UTF8, (unsigned char *)pbMBSTRUTF8, lent)))
				{
				PrintErrorInfo("Error creating name entry from NID!", EOPENSSL, constparams);
				throw EOPENSSL;
				}

			if(X509_NAME_add_entry(pSubj, pEnt, -1, 0) != 1)
				{
				PrintErrorInfo("Error adding entry to X509 Name!", EOPENSSL, constparams);
				throw EOPENSSL;
				}
			delete pbMBSTRUTF8;
			}//for
		
		SYMBIAN_FREE_MEM(pEntPack);

		if(X509_REQ_set_subject_name(pReq, pSubj) != 1)
			{
			PrintErrorInfo("Error adding subject to request!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

			//Find the correct digest and sign the request

		if(EVP_PKEY_type(pKey->type) == EVP_PKEY_DSA)
			{
			pDigest = EVP_dss1();
			}
		else if(EVP_PKEY_type(pKey->type) == EVP_PKEY_RSA)
			{
			pDigest = EVP_sha1();
			}
		else
			{
			PrintErrorInfo("Error checking private key type!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		if(!(X509_REQ_sign(pReq, pKey, pDigest)))
			{
			PrintErrorInfo("Error signing request!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		if(!(fp = _tfopen(m_RequestFile, _T("w"))))
			{
			PrintErrorInfo("Error writing to request file!",EGeneric,constparams);
			throw EGeneric;
			}
	
		if(PEM_write_X509_REQ(fp, pReq) != 1)
			{
			PrintErrorInfo("Error while writing to request file!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		//Free variables
		EVP_PKEY_free(pKey);
		X509_NAME_free(pSubj);
		X509_REQ_free(pReq);
		fclose(fp);

		_tprintf(_T("\nCreated request: "));
		WriteConsole(hndl, m_RequestFile, wcslen(m_RequestFile), &bytesWritten, 0);

		retFunc = SUCCESS;

		}
	catch (...)
		{
		if(pKey)
			{
			EVP_PKEY_free(pKey);
			}
		
		if(pSubj)
			{
			X509_NAME_free(pSubj);
			}

		if(pReq)
			{
			X509_REQ_free(pReq);
			}

		SYMBIAN_FREE_MEM(pEntPack);
		}

	return retFunc;
	}
