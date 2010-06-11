/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Handles parsing of makekeys command line args
*
*/


/**
 @file 
 @internalComponent 
*/

#ifndef __PARSE_CMD_H_
#define __PARSE_CMD_H_

#include <stdio.h>
#include <iostream>
#include "utils.h"
#include "KeyGenerator.h"
#include "CertificateGenerator.h"

//undefined by default, the ability to generate keys on its own is useful for testing
//and maybe in the real-world too
#define KEYS
//some work that's already been done, thats planned as optional so may or may not be needed
#define OPTIONAL


// ===========================================================================
// CONSTANTS
// ===========================================================================


#define MAXLEN 256
#define DEFAULTKEYLEN 1024
#define DEFAULTCERTEXPIRYDAYS 365


//These constants are temporarily file names. They will be changed
//with original ones at the end of the program 
#define KEYFILENAME "mytempkey.key"
#define CERTFILENAME "mytempcert.cer"
#define REQUESTFILENAME "mytemprequest.p10"

enum TCommandLineException
	{ErrInsufficientArgs,
	ErrTooManyArgs,
	ErrBadReqCommand,
	ErrBadCommandFlag,
	ErrUnknownCommandFlag,
	ErrCantViewCertificate,
	ErrMalformedCommand,
	ErrUnsupportedCP,
	ErrInvalidPassword
	}; 

enum Mode
	{EKeys = 1,
	ECert,
	EReq,
	EChain,
	EView,
	EPassword,
	EDname,
	ERSA,
	EDSA,
	EUnknown
	};

enum TOptions
	{EOptVerbose  = 0x01,
	};
		


#define MIN_VIEW_ARGS 3
#define MIN_CERT_ARGS 6
#define MIN_REQ_ARGS 7


// ===========================================================================
// CLASS DEFINITION
// ===========================================================================

class CParseCmd
// Responsable for processing and maintaining the command line options
	{
	public:
		TKeyType GetKeyType();
		bool GetPasswordEnabled();
		
		_TCHAR* GetPassword();
		_TCHAR* GetPrivateName();
		_TCHAR* GetPublicName();
		_TCHAR* GetRequestName();
		_TCHAR* GetDNameString();
		bool GetVerbose();

		int GetCMDKeyLen();
		int GetCMDCertExpiryInDays();
		CParseCmd();
		
		ParseCommandLine(const int argc, const _TCHAR *argv[], Mode* aApp);
		
		BOOL  ShowSyntax()			const { return m_fShowSyntax; }
		DWORD Flags()				const { return m_dwOptions;   }

	private:
		bool m_bVerbose;
		void SetVerbose(const bool bVerbose);
		
		DWORD m_dwKeyLen;
		DWORD m_dwCertExpiryInDays;
		
		DWORD m_dwOptions;
		BOOL  m_fShowSyntax;
		TKeyType m_CMDKey;
		bool m_bPassword;

		_TCHAR m_password[MAXLEN];
		_TCHAR m_dname[512];
		_TCHAR m_privateKeyFile[MAX_PATH];
		_TCHAR m_publicKeyFile[MAX_PATH];
		_TCHAR m_requestFile[MAX_PATH];
		_TCHAR m_KeyLenStr[MAXLEN];
		_TCHAR m_CertExpiryInDaysStr[MAXLEN];

	};

#endif // __PARSE_CMD_H_
