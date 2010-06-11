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
* Interface for the CCertificateGenerator class
*
*/


/**
 @file 
 @internalComponent 
*/

#if !defined(AFX_CERTIFICATEGENERATOR_H__022A7975_9385_4936_B181_35BD5D1884BE__INCLUDED_)
#define AFX_CERTIFICATEGENERATOR_H__022A7975_9385_4936_B181_35BD5D1884BE__INCLUDED_

#include "KeyGenerator.h"	// Added by ClassView
#include "DSAKeyGenerator.h"
#include "RSAKeyGenerator.h"
#include "utils.h"
#include "openssl/x509.h"
#include "openssl/x509v3.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define EXPIRE_DATE 365
#define EXPIRE_SECS (60*60*24*EXPIRE_DATE)


class CCertificateGenerator  
	{
	public:
	
		void PrintErrorInfo(char* aMsg, TErrType aErrType, const char* aFileName, int aLineNum);
		void SetAskPassword(bool aAsk);
		int MakeSelfSigned(_TCHAR* aPassword, _TCHAR* aDname, _TCHAR* aPrivOut, _TCHAR* aPubOut, int aBits, int aSerial, int aDays);
		void SetVerbose(bool aVerbose);
		void View(const _TCHAR* aCertName);
		BOOL Exists(const _TCHAR* aFile);
		void SetPasswordRequirement(bool aPwReq);
		void SetKeyLength(int aLen);
		void SetKeyType(TKeyType aKey);
		void SetPassword(_TCHAR* aStr);
		void SetCertificateName(_TCHAR* aStr);
		void SetPrivKeyName(_TCHAR* aStr);
		void SetDName(_TCHAR* aDntr);
		_TCHAR* GetDNamePtr();
		CCertificateGenerator();
		virtual ~CCertificateGenerator();

	protected:

		int m_KeyLength;
		bool m_bVerbose;
		bool m_bPassword;
		bool m_bAsk;

		TKeyType m_KeyType;
		CKeyGenerator* m_KeyGen;
	
		_TCHAR m_certificateFile[MAXLEN];
		_TCHAR m_privateKeyFile[MAXLEN];
		_TCHAR m_password[MAXLEN];
		_TCHAR m_dname[MAXLEN];

	private:
		
		_TCHAR m_KeyLenStr[MAXLEN];
	};

#endif // !defined(AFX_CERTIFICATEGENERATOR_H__022A7975_9385_4936_B181_35BD5D1884BE__INCLUDED_)
