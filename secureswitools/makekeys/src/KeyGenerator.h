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
* Interface for the CKeyGenerator class
*
*/


/**
 @file 
 @internalComponent 
*/

#if !defined(AFX_KEYGENERATOR_H__E70F430E_9D8C_4EA6_B2A8_1973F813312E__INCLUDED_)
#define AFX_KEYGENERATOR_H__E70F430E_9D8C_4EA6_B2A8_1973F813312E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_WINNT 0x0500

#define DEFAULT_KEY_LENGTH 1024
#define PW_LEN 32
#define MAXLEN 256
#define constparams __FILE__,__LINE__
#define SYMBIAN_FREE_MEM(mem){if(!mem) free(mem); mem=NULL;}

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include <wincrypt.h>
#include "utils.h"

#include "openssl/pem.h"
#include "openssl/evp.h"


enum TKeyType {ERSACipher, EDSACipher};
enum TKeyGeneratorException {ErrFileOpen, ErrNULLKeyStructure};

class CKeyGenerator  
	{
	public:
	
		void PrintErrorInfo(char* aMsg, TErrType aEntType, const char* aFileName, int aLineNum);
		void SetVerbose(bool aVerbose);
		void SetAskPassword(bool aAsk);
		const _TCHAR* GetPassword();
		void SetPasswordRequirement (bool aPwReq);
		void SetPassword(_TCHAR* aPw);
		void SetPrivateKeyFile(_TCHAR* aPk);
		const TCHAR* GetPrivateKeyFile(void) const;
		void SetKeyLength(int aKeyLen);
		const int GetKeyLength() const;
		virtual int Generate() =0;
		CKeyGenerator();
		virtual ~CKeyGenerator();

	protected:

		bool m_bVerbose;
		TKeyType m_Cipher;
		bool m_bPassword;
		bool m_bAsk;
		int GenerateSeed(int aLength, unsigned char** aBuf);

	private:

		_TCHAR m_sPassword[PW_LEN];
		int m_nKeyLength;
		_TCHAR m_PrivateKeyFile[MAXLEN];
		
	};

#endif // !defined(AFX_KEYGENERATOR_H__E70F430E_9D8C_4EA6_B2A8_1973F813312E__INCLUDED_)
