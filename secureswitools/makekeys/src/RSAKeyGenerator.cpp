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
* Implementation of the CRSAKeyGenerator class
* INCLUDES
*
*/


#include "RSAKeyGenerator.h"
#include <iostream.h>

// ===========================================================================
// Construction/Destruction
// ===========================================================================

CRSAKeyGenerator::CRSAKeyGenerator()
{

}

CRSAKeyGenerator::~CRSAKeyGenerator()
{

}

int CRSAKeyGenerator::Generate()
//Generate an RSA key with pre-determined length
{
	RSA*		   pRSAKey	    = NULL;
	FILE*		   fp		    = NULL;
	LPSTR		   pbPassword   = NULL;
	const _TCHAR*  pPrivKeyFile = NULL;

	int retVal  = FAIL;
	int retFunc = FAIL;

	pPrivKeyFile = GetPrivateKeyFile();
	if(!pPrivKeyFile)
		{
		PrintErrorInfo("Bad parameter error!", EGeneric, constparams);
		return 0;
		}
	
	OPENSSL_add_all_algorithms_conf();
	ERR_load_crypto_strings();

	int keyLength = 0;
	keyLength = GetKeyLength();
	try
		{
		//Generate RSA key
		_tprintf(_T("\nGenerating RSA key ."));
		do
			{
			pRSAKey = RSA_generate_key(keyLength, RSA_F4, RSAKeyStatus, NULL);

			}
		while((retVal = RSA_check_key(pRSAKey)) == 0); // if return is 0, the key should be regenerated!

		if(retVal != 1)
			{
			PrintErrorInfo("RSA key generation failed!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		_tprintf(_T("Generated!\n"));
		//Create a key file
		fp = _tfopen(pPrivKeyFile, _T("w"));
		
		if(!fp)
			{
			PrintErrorInfo("Error creating key file!", EGeneric, constparams);
			throw EGeneric;
			}

		//Write generated DSA key to the key file
		if(m_bPassword)
			{
			DWORD len = 0;
			len = _tcslen(GetPassword());
			pbPassword = MakeMBCSString(GetPassword(), CP_UTF8, len);
			retVal = PEM_write_RSAPrivateKey(fp, pRSAKey, EVP_des_ede3_cbc(), (unsigned char *) pbPassword, len, NULL, NULL);
			delete pbPassword;
			}
		else if(m_bAsk)
			{
			retVal = PEM_write_RSAPrivateKey(fp, pRSAKey, EVP_des_ede3_cbc(), NULL, 0, NULL, NULL);
			}

		if(!retVal)
			{
			PrintErrorInfo("Error writing to key file", EOPENSSL, constparams);
			throw EOPENSSL;
			}
		
		//Free variables
		RSA_free(pRSAKey);
		fclose(fp);	
		
		//Get command prompt handle
		HANDLE	hndl = 0;
		DWORD bytesWritten;
		hndl = GetStdHandle(STD_OUTPUT_HANDLE);
		_tprintf(_T("\nCreated key: "));
		WriteConsole(hndl, pPrivKeyFile, wcslen(pPrivKeyFile), &bytesWritten, 0);
		retFunc = SUCCESS;

		}
	catch (...)
		{
		//Delete rsa params
		if(pRSAKey != NULL) 
			{
			RSA_free(pRSAKey);
			}

		}
	return retFunc;
	}


//RSA key generation callback function
static void RSAKeyStatus(int aCode, int aArg, void *aCbArg)
	{
	
	if (aCode == 1 && aArg && !(aArg % 3))
		{
		printf(".");
		}
	return;
	}
