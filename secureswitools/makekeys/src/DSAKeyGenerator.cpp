/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of the CDSAKeyGenerator class
* INCLUDES
*
*/


#include "DSAKeyGenerator.h"
#include <iostream.h>

// ===========================================================================
// Construction/Destruction
// ===========================================================================

CDSAKeyGenerator::CDSAKeyGenerator()
	{

	}

CDSAKeyGenerator::~CDSAKeyGenerator()
	{

	}

int CDSAKeyGenerator::Generate()
//Generate a DSA key with pre-determined length
	{
	unsigned char* pbSeed       = NULL; 
	DSA*		   pDSAParams   = NULL;
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

	int dwKeyLength = 0;
	dwKeyLength = GetKeyLength();
	
	try
		{
		retVal = GenerateSeed(dwKeyLength, &pbSeed);
		if(retVal != SUCCESS)
			{
			throw EMSCrypto;
			}

		//Generate DSA params (p,q and g)
		_tprintf(_T("\nGenerating DSA key ."));
		pDSAParams = DSA_generate_parameters(dwKeyLength, pbSeed, dwKeyLength, NULL, NULL, DSAKeyStatus, NULL);
		if(!pDSAParams)
			{
			PrintErrorInfo("Error generating DSA key params!", EOPENSSL, constparams);
			throw EOPENSSL;
			}
		
		//Generate DSA key
		retVal = DSA_generate_key(pDSAParams);
		if(!retVal)
			{
			PrintErrorInfo("DSA key generation failed!", EOPENSSL, constparams);
			throw EOPENSSL;
			}

		_tprintf(_T("Generated!\n"));
		//Create a key file
		fp = _tfopen(pPrivKeyFile, _T("w"));

		if(!fp)
			{
			PrintErrorInfo("Error creating key file!", EGeneric, constparams);
			throw EOPENSSL;
			}
		
		//Write generated DSA key to the key file
		if(m_bPassword)
			{
			DWORD len = 0;
			len = _tcslen(GetPassword());
			pbPassword = MakeMBCSString(GetPassword(), CP_UTF8, len);
			retVal = PEM_write_DSAPrivateKey(fp, pDSAParams, EVP_des_ede3_cbc(), (unsigned char *) pbPassword, len, NULL, NULL);
			delete pbPassword;
			}
		else if(m_bAsk)
			{
			retVal = PEM_write_DSAPrivateKey(fp, pDSAParams, EVP_des_ede3_cbc(), NULL, 0, NULL, NULL);
			}
		else 
			{
			_tprintf(_T("\n"));
			retVal = PEM_write_DSAPrivateKey(fp, pDSAParams, NULL , NULL, 0, NULL, NULL);
			}

		if(!retVal)
			{
			PrintErrorInfo("Error writing to key file", EOPENSSL, constparams);
			throw EOPENSSL;
			}
		//Free variables
		DSA_free(pDSAParams);
		fclose(fp);
		SYMBIAN_FREE_MEM(pbSeed);

		//Get command prompt handle
		HANDLE hndl = 0;
		hndl = GetStdHandle(STD_OUTPUT_HANDLE);
		_tprintf(_T("\nCreated key: "));
		DWORD bytesWritten;
		WriteConsole(hndl, pPrivKeyFile, wcslen(pPrivKeyFile), &bytesWritten, NULL);
		retFunc = SUCCESS;	

		}
	catch (...)
		{
		//Delete dsa params
		if(pDSAParams)
			{
			DSA_free(pDSAParams);
			}
		if (fp)
			{
			fclose(fp);
			}
		SYMBIAN_FREE_MEM(pbSeed);
		}

	return retFunc;
}

//DSA key generation callback function
static void DSAKeyStatus(int aCode, int aArg, void* aCbArg)
	{
	
	if (aCode == 1 && aArg && !(aArg % 3))
		{
		printf(".");
		}
	return;
	}
