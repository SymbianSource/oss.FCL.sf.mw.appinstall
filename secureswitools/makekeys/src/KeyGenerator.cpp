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
* Implementation of the CKeyGenerator class
* INCLUDES
*
*/


#include "KeyGenerator.h"


// ===========================================================================
// Construction/Destruction
// ===========================================================================

CKeyGenerator::CKeyGenerator()
	{
	m_nKeyLength = DEFAULT_KEY_LENGTH;
	m_Cipher = EDSACipher;
	m_bPassword = FALSE;
	m_bAsk = FALSE;
	m_bVerbose = FALSE;
	memset(m_PrivateKeyFile,0, MAXLEN);
	memset(m_sPassword, 0, MAXLEN);
	}

CKeyGenerator::~CKeyGenerator()
	{
	
	}

const int CKeyGenerator::GetKeyLength() const
	{
	return m_nKeyLength;
	}

void CKeyGenerator::SetKeyLength(int aKeyLen)
	{
	m_nKeyLength = aKeyLen;
	}

const TCHAR* CKeyGenerator::GetPrivateKeyFile() const
	{
	return m_PrivateKeyFile;
	}

void CKeyGenerator::SetPrivateKeyFile(_TCHAR* aPk)
	{
	_tcscpy(m_PrivateKeyFile, aPk);
	}

void CKeyGenerator::SetPassword(_TCHAR* aPw)
	{
	_tcscpy(m_sPassword, aPw);
	}

void CKeyGenerator::SetPasswordRequirement(bool aPwReq)
	{
	m_bPassword = 	aPwReq;
	}

int CKeyGenerator::GenerateSeed(int aLength, unsigned char** aBuf)
//Generate a seed data for DSA key generation
	{
	HCRYPTPROV hProv = 0;
	
	*aBuf = (unsigned char *)malloc(aLength * sizeof(unsigned char));
	
	// Get handle to default key context
	if(!CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0))
		{
		if(!CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET))
			{
			if(!CryptAcquireContext(&hProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, 0))
				{
				if(!CryptAcquireContext(&hProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET))
					{	
					PrintErrorInfo("Error acquiring context!", EMSCrypto, constparams);
					return FAIL;
					}
				}
			}
		}

	//Generate a random number
	if(!CryptGenRandom(hProv, aLength, *aBuf))
		{
		PrintErrorInfo("Error generating random number!", EMSCrypto, constparams);
		return FAIL;
		}

	return SUCCESS;
	}

const _TCHAR* CKeyGenerator::GetPassword()
	{
	return m_sPassword;
	}

void CKeyGenerator::SetAskPassword(bool aAsk)
	{
	m_bAsk = aAsk;
	}

void CKeyGenerator::SetVerbose(bool aVerbose)
	{
	m_bVerbose = aVerbose;
	}

void CKeyGenerator::PrintErrorInfo(char* aMsg, TErrType aEntType, const char* aFileName, int aLineNum)
	{
	PrintError(aMsg, aEntType, m_bVerbose, aFileName, aLineNum);
	}
