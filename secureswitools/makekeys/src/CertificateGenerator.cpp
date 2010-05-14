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
* Implementation of the CCertificateGenerator class
* INCLUDES
*
*/


#include "CertificateGenerator.h"


// ===========================================================================
// Construction/Destruction
// ===========================================================================

CCertificateGenerator::CCertificateGenerator()
	{
	m_KeyType	= EDSACipher;
	m_KeyLength = DEFAULT_KEY_LENGTH;
	m_bPassword = FALSE;
	m_bAsk		= FALSE;
	m_bVerbose	= FALSE;

	memset(m_certificateFile,0,MAXLEN);
	memset(m_privateKeyFile, 0, MAXLEN);
	memset(m_password, 0, MAXLEN);
	memset(m_dname, 0, MAXLEN);
	}

CCertificateGenerator::~CCertificateGenerator()
	{

	}


_TCHAR* CCertificateGenerator::GetDNamePtr()
	{
	return m_dname;
	}

void CCertificateGenerator::SetDName(_TCHAR* aDntr)
	{
	if(aDntr)
		{
		_tcscpy(m_dname, aDntr);
		}
	}

void CCertificateGenerator::SetPrivKeyName(_TCHAR* aStr)
	{
	if(aStr)
		_tcscpy(m_privateKeyFile, aStr);
	}

void CCertificateGenerator::SetCertificateName(_TCHAR* aStr)
	{
	if(aStr)
		{
		_tcscpy(m_certificateFile, aStr);
		}
	}

void CCertificateGenerator::SetPassword(_TCHAR* aStr)
	{
	if(aStr)
		{
		_tcscpy(m_password, aStr);
		}
	}

void CCertificateGenerator::SetKeyType(TKeyType aKey)
	{
	m_KeyType = aKey;
	}

void CCertificateGenerator::SetKeyLength(int aLen)
	{
	if(aLen >= 0)
		{
		m_KeyLength = aLen;
		}
	}

void CCertificateGenerator::SetPasswordRequirement(bool aPwReq)
	{
	m_bPassword = aPwReq;
	}

void CCertificateGenerator::SetAskPassword(bool aAsk)
	{
	m_bAsk = aAsk;
	}

void CCertificateGenerator::SetVerbose(bool aVerbose)
	{
	m_bVerbose = aVerbose;
	}

void CCertificateGenerator::PrintErrorInfo(char* aMsg, TErrType aErrType, const char* aFileName, int aLineNum)
	{
	PrintError(aMsg, aErrType, m_bVerbose, aFileName, aLineNum);
	}

BOOL CCertificateGenerator::Exists(const _TCHAR* aFile)
	{
	FILE *fp = NULL;

	fp = _tfopen(aFile , _T("r"));
	
	if(fp)
		{	
		fclose(fp);
		return TRUE;
		}
	else
		{
		return FALSE;
		}
	}

void CCertificateGenerator::View(const _TCHAR* aCertName)
	{
	HINSTANCE retVal = 0;
	
	if(!aCertName)
		{	
		PrintErrorInfo("Bad parameter error!",EGeneric, constparams);
		return;
		}

	retVal = ShellExecute(NULL, _T("open"), aCertName, NULL, NULL, SW_SHOWNORMAL);

	if((int)retVal <= 32)
		{
		PrintErrorInfo("Error showing certificate!", EMSCrypto, constparams);
		}

}

int CCertificateGenerator::MakeSelfSigned(_TCHAR* aPassword, _TCHAR* aDname, _TCHAR* aPrivOut, _TCHAR* aPubOut, int aBits, int aSerial, int aDays)
//Create self-signed certificate and write into a file
	{

	FILE*		  fp		   = NULL;
	EVP_PKEY*	  pKey		   = NULL;
	X509*		  pCert		   = NULL;
	X509_NAME*	  pSubj		   = NULL;
	const EVP_MD* pDigest	   = NULL;
	LPSTR		  pbPasswdUTF8 = NULL;
	DWORD		  bytesWritten;

	struct entry_pack* pEntPack = NULL;

	int			retFunc = FAIL;
	int			retVal  = FAIL;
	int			expSecs = (60*60*24*aDays);
		
	//Get command prompt handle
	HANDLE hndl = 0;
	hndl = GetStdHandle(STD_OUTPUT_HANDLE);
	
	if(!Exists(aPrivOut))
		{
		//First create a key
		if ( m_KeyType == EDSACipher)
			{
			m_KeyGen = new CDSAKeyGenerator;
			}
		else
			{
			m_KeyGen = new CRSAKeyGenerator;
			}
			
		if (!m_KeyGen) 
			{
			return FAIL;
			}
			
 		m_KeyGen->SetKeyLength(m_KeyLength);
 		m_KeyGen->SetPasswordRequirement(m_bPassword);
		m_KeyGen->SetAskPassword(m_bAsk);

		if(m_bPassword && m_password)
			{
			m_KeyGen->SetPassword(m_password);
			}

		if(m_privateKeyFile)
			{
 			m_KeyGen->SetPrivateKeyFile(m_privateKeyFile);
			}

		m_KeyGen->SetVerbose(m_bVerbose);
		
		retVal = m_KeyGen->Generate();
		
		delete m_KeyGen;
		}
	else
		{	
		_tprintf(_T("\nExisting private key will be used: "));
		WriteConsole(hndl, m_privateKeyFile, _tcslen(m_privateKeyFile), &bytesWritten, 0);
		retVal = SUCCESS;
		
		//Since key generation class did not called, we should call these macros here
		OPENSSL_add_all_algorithms_conf();
		ERR_load_crypto_strings();
		}

	if(retVal != SUCCESS)
		{
		return retFunc;
		}
	
	//Generate certificate
	try
		{
		//First read private key from key file
		if(!(fp = _tfopen(m_privateKeyFile, _T("r"))))
			{
			PrintErrorInfo("Error reading key file!", EGeneric, constparams);
			WriteConsole(hndl, m_privateKeyFile, wcslen(m_privateKeyFile), &bytesWritten, 0);
			throw EGeneric;
			}

		DWORD len = 0;
		if(m_password[0] != 0)
			{
			len = _tcslen(m_password);
			pbPasswdUTF8 = MakeMBCSString(m_password, CP_UTF8, len);
			pKey = PEM_read_PrivateKey(fp, NULL, NULL, pbPasswdUTF8);
			delete pbPasswdUTF8;
			}
		else
			{
			pKey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
			}
			
		fclose(fp); fp = NULL;

		if(!pKey)
			{
			PrintErrorInfo("Error reading private key in key file!", EOPENSSL, constparams);
			throw EGeneric;
			}
		
			//Create a new certificate
		if(!(pCert = X509_new()))
			{
			PrintErrorInfo("Error creating X509 object!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		if(X509_set_version(pCert, 2L) != 1)
			{
			PrintErrorInfo("Error setting certificate version!", EOPENSSL, constparams);
			throw EOPENSSL;
			}
		
		long serial = 1;
		ASN1_INTEGER_set(X509_get_serialNumber(pCert), serial++);

		//Set issuer and subject name
	
		//First create a name object
		if(!(pSubj = X509_NAME_new()))
			{
			PrintErrorInfo("Error creating X509 name object!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		//Format DN string
		DoFormatted(m_dname, &pEntPack);

		if(pEntPack->num <= 0)
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
			}
		
		SYMBIAN_FREE_MEM(pEntPack);
		
		if(X509_set_subject_name(pCert, pSubj) != 1)
			{
			PrintErrorInfo("Error setting subject name of the certificate!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		if(X509_set_issuer_name(pCert, pSubj) != 1)
			{
			PrintErrorInfo("Error setting issuer name of the certificate!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		if(X509_set_pubkey(pCert, pKey) != 1)
			{
			PrintErrorInfo("Error setting public key of the certificate!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		if(!(X509_gmtime_adj(X509_get_notBefore(pCert), 0)))
			{
			PrintErrorInfo("Error setting beginning time of the certificate!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		if(!(X509_gmtime_adj(X509_get_notAfter(pCert), expSecs)))
			{
			PrintErrorInfo("Error setting ending time of the certificate!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

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
	
		if(!(X509_sign(pCert, pKey, pDigest)))
			{
			PrintErrorInfo("Error signing certificate!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		if(!(fp = _tfopen(m_certificateFile, _T("w"))))
			{
			PrintErrorInfo("Error writing to certificate file!",EGeneric,constparams);
			throw EOPENSSL;
			}
	
		if(PEM_write_X509(fp, pCert) != 1)
			{
			PrintErrorInfo("Error while writing to certificate!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		//Free variables
		X509_free(pCert);
		EVP_PKEY_free(pKey);
		X509_NAME_free(pSubj);
		fclose(fp);
	
		_tprintf(_T("\nCreated certificate: "));
		WriteConsole(hndl, m_certificateFile, wcslen(m_certificateFile), &bytesWritten, 0);
		_tprintf(_T("\n"));	

		retFunc = SUCCESS;

		} //try

	catch (...)
		{
		if(pKey)
			{
			EVP_PKEY_free(pKey);
			}
		
		if(pCert)
			{
			X509_free(pCert);	
			}
		
		if(pSubj)
			{
			X509_NAME_free(pSubj);
			}
		
		SYMBIAN_FREE_MEM(pEntPack);
		}
	
	return retFunc;
}
