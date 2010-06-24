/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* makekeys main function
*
*/


/**
 @file 
 @internalComponent 
*/

#ifndef __MAKEKEYS_H__
#define __MAKEKEYS_H__

#define KMakeKeysVersion 100

enum TAppError
	{ErrOutOfMemory =-1
	}; 

// ===========================================================================
// INCLUDES
// ===========================================================================

#include <stdio.h>
#include "ParseCmd.h"
#include "utils.h"
#include "CertificateGenerator.h"
#include "CertificateRequestGenerator.h"
#include "DSAKeyGenerator.h"
#include "RSAKeyGenerator.h"

// ===========================================================================
// CLASS DEFINITION
// ===========================================================================

class CMakeKeys
// The main application object. Controls the Key / Certificate file generation process
	{
	public:
		CMakeKeys();
		
		int  Run(const int argc, const _TCHAR *argv[]);
			
	public: 
		CKeyGenerator* m_KeyGen;
		CCertificateGenerator* pMakeCert;
		CCertificateRequestGenerator* pReq;
		
		void DoVerbage(const _TCHAR* pszText) const;
		void DoErrMsg(const _TCHAR* pszText) const;
		BOOL Verbose() const				  { return m_fVerbose; };
		BOOL SetVerbose(BOOL fVerbose)		  { BOOL v = m_fVerbose; m_fVerbose = fVerbose; return v; };
		
	private:
		void DoMsg(const _TCHAR* pszText1, const _TCHAR* pszText2) const;
		void ShowBanner();
		void ShowUsage ();
				
		void ShowUtilsError(TUtilsException err);

		BOOL m_fVerbose;
				
		CParseCmd m_CmdOptions;
	protected:
		Mode m_dwMode;
	};

#endif // __MAKEKEYS_H__
