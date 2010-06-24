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
* INCLUDES
*
*/


#include <windows.h>
#include <iostream> 
#include <tchar.h>
#include "utils.h"
#include "makekeys.h"

#define NORSA	//to keep DTI sweet!

#ifdef KEYS	//define KEYS for standalone keygen support (ie -KEYS works)
#undef KEYS
#endif

// ===========================================================================
// CONSTANTS
// ===========================================================================

// Set up the cout stream so that we can use it with either narrow or wide
// chars at build time
using namespace std;
#undef OUT

#ifdef _UNICODE
#define OUT wcout
#else
#define OUT cout
#endif


// ===========================================================================
// CMakeKeys
// The main application object. Interfaces to Wincrypto.lib
// ===========================================================================


CMakeKeys::CMakeKeys()
:m_fVerbose(FALSE)
	{ ; }

int CMakeKeys::Run(const int argc, const _TCHAR *argv[])
// Inputs   : argc, argv, envp - The command line passed to the process
	{	
	BOOL bValid = TRUE;
	m_KeyGen = NULL;
	pMakeCert = NULL;
	pReq = NULL;

	try
		{
		m_CmdOptions.ParseCommandLine(argc, argv, &m_dwMode);

		// We now have valid parameters
		SetVerbose(m_CmdOptions.GetVerbose());
		}
	catch(TCommandLineException)
		{
		// Show the title and help
		ShowBanner();
		ShowUsage();
		
		return 1;
		}
	catch(TUtilsException err)
		{
		ShowUtilsError(err);
		}
	catch(TAppError apperr)
		{
		OUT << endl << _T("Internal application error" << apperr) << endl;
		}
	
#ifdef KEYS
	//Key maker, -keys is not in the spec but its handy to have for testing!

	DoVerbage(_T("Generating private key..."));
	if (m_dwMode == EKeys)
		{ 
		if (m_CmdOptions.GetKeyType() == EDSACipher)
			{
			m_KeyGen = new CDSAKeyGenerator;
			}
		else
			{
			m_KeyGen = new CRSAKeyGenerator;
			}
		
		if (!m_KeyGen) throw ErrOutOfMemory;
		
		m_KeyGen->SetKeyLength(m_CmdOptions.GetCMDKeyLen());
		m_KeyGen->SetPasswordRequirement(m_CmdOptions.GetPasswordEnabled());
		m_KeyGen->SetPrivateKeyFile(m_CmdOptions.GetPrivateName());
		m_KeyGen->SetVerbose(m_CmdOptions.GetVerbose());
		
		//If user did not supply password to encrypt private key before writing, get it
		if (!m_CmdOptions.GetPasswordEnabled())
			{
			char Warn[128];

			OUT << _T("Warning: the private key should be encrypted with the -password option") << endl;
			OUT << _T("Do you want to use a password (y/n)? ");
			OUT.flush();

			cin >> Warn;

			if (stricmp(Warn, "n") != 0)
				{
				m_KeyGen->SetAskPassword(TRUE);
				}
			} 
		else
			{
			m_KeyGen->SetPassword(m_CmdOptions.GetPassword());
			}

		m_KeyGen->Generate();

		if (m_KeyGen) delete m_KeyGen;
		}
#endif
	
	//create a Self Signed Cert
	if (m_dwMode == ECert)
		{
		DoVerbage(_T("Generating private key and self-signed certificate..."));
		
		pMakeCert = new CCertificateGenerator;
		if (!pMakeCert) throw ErrOutOfMemory;
			
		pMakeCert->SetKeyLength(m_CmdOptions.GetCMDKeyLen());
		pMakeCert->SetPasswordRequirement(m_CmdOptions.GetPasswordEnabled());
		if(m_CmdOptions.GetPasswordEnabled())
		pMakeCert->SetPassword(m_CmdOptions.GetPassword());
		if(m_CmdOptions.GetDNameString())
			pMakeCert->SetDName(m_CmdOptions.GetDNameString());
		pMakeCert->SetPrivKeyName(m_CmdOptions.GetPrivateName());
		pMakeCert->SetCertificateName(m_CmdOptions.GetPublicName());
		pMakeCert->SetVerbose(m_CmdOptions.GetVerbose());
		
#ifndef NORSA
		pMakeCert->SetKeyType(m_CmdOptions.GetKeyType());
#else
			pMakeCert->SetKeyType(EDSACipher);
#endif
		
			if ((!m_CmdOptions.GetPasswordEnabled()) && (!pMakeCert->Exists(m_CmdOptions.GetPrivateName()) ))
				{
				char Warn[128];

				OUT << _T("Warning: the private key should be encrypted with the -password option") << endl;
				OUT << _T("Do you want to use a password (y/n)? ");
				OUT.flush();

				cin >> Warn;

				if (stricmp(Warn, "n") != 0)
					{
					pMakeCert->SetAskPassword(TRUE);
					}
				}
		
			if (!pMakeCert->Exists(m_CmdOptions.GetPrivateName()))
				{
				//OUT << _T("Generating private key...") << endl;
				}

			pMakeCert->MakeSelfSigned(_T(""),m_CmdOptions.GetDNameString(),
			m_CmdOptions.GetPrivateName(), m_CmdOptions.GetPublicName(),
			m_CmdOptions.GetCMDKeyLen(),0,m_CmdOptions.GetCMDCertExpiryInDays());

			if (pMakeCert) delete pMakeCert;
			}
	
		//Create a CSR from command line parameters
		
		if (m_dwMode == EReq)
			{
			DoVerbage(_T("Generating certificate request...\n"));
			
			pReq = new CCertificateRequestGenerator();
			if (!pReq) throw ErrOutOfMemory;
			
			pReq->SetVerbose(m_CmdOptions.GetVerbose());	//enable additional reporting within Wincrypto
			pReq->SetPasswordRequirement(m_CmdOptions.GetPasswordEnabled());
			pReq->SetPassword(m_CmdOptions.GetPassword());
			pReq->SetDName(m_CmdOptions.GetDNameString());
			pReq->SetPrivKeyName(m_CmdOptions.GetPrivateName());
			pReq->SetCertificateName(m_CmdOptions.GetPublicName());
			pReq->SetRequestName(m_CmdOptions.GetRequestName());
			
			pReq->Generate();

			if (pReq) delete pReq;
			}
		
			return 0;
		}
	
	void CMakeKeys::ShowBanner()
		// Displays the copyright gumph...
		{
		short	major = KMakeKeysVersion / 100,
				minor = KMakeKeysVersion % 100;
	
		// Show the title
		OUT << endl << _T("MakeKeys, version ") << major << _T(".") << minor << endl;
		OUT << _T("Creates public/private key pairs and certificate requests for use with makesis.") << endl;
#ifdef _DEBUG
		OUT << _T("Development Version") << endl;
#endif
	OUT << _T("Copyright (c) 2007 Symbian Ltd. All rights reserved.") << endl;
	OUT << endl;
	}

	void CMakeKeys::ShowUtilsError(TUtilsException err)
		// Purpose  : Not currently used
		// Inputs   : err - the error ID
		{
		switch(err)
			{
			case ErrNotEnoughMemory:
				DoErrMsg(_T("not enough memory"));
				break;
			}
		}

	void CMakeKeys::ShowUsage()
		// Write command line help to STDOUT (cout/wcout)
		{
		OUT << _T("Usage:") << endl;
		
	OUT << _T(" makekeys -cert [-expdays <cert-expiry-in-days>] [-password <password> <At least 4 characters>] [-len <key-length>] -dname <distinguised-name-string> <private-key-file> <public-key-cert>") << endl;	
	OUT << _T(" makekeys -req [-password <password> <At least 4 characters>] -dname <distinguised-name-string> <private-key-file> <public-key-cert> <cert-request-file>") << endl;	
	OUT << _T(" makekeys -view <certificate-file>") << endl << endl;
	OUT << _T(" (use -v on the command line for verbose mode)") << endl;
	OUT << _T(" (use -i on the command line for license information)") << endl;
	OUT << endl;
		

		OUT << _T("Notes:") << endl;
		OUT << _T(" For -cert if the private-key-file does not exist it will be created") << endl;	
		OUT << _T(" Distinguished name string format:") << endl;	
		OUT << _T("    CN - Common Name, e.g. CN=Joe Bloggs") << endl;
		OUT << _T("    C - Country, e.g. C=GB") << endl;
		OUT << _T("    O - Organisation, e.g. O=Acme Ltd") << endl;
		OUT << _T("    OU - Organisational Unit, e.g. OU=Development") << endl;
		OUT << _T("    EM - E-Mail address, e.g. EM=noone@nowhere.com") << endl;
		OUT << _T(" A distinguished name strings needs at least two attributes") << endl;
		OUT << _T(" Certificate expiry date(in days) can be specified by the user (optional) , else defaults to a year.") << endl;
		OUT << endl;


		OUT << _T("Example Usage:") << endl;
		
		OUT << _T(" makekeys -cert -expdays 3650 -password yourpassword -len 2048 -dname \"CN=Joe Bloggs OU=Development O=Acme Ltd C=GB EM=noone@nowhere.com\" mykey.key mycert.cer") << endl;
		OUT << _T(" makekeys -req -password yourpassword -dname \"CN=Joe Bloggs OU=Development O=Acme Ltd C=GB EM=noone@nowhere.com\" mykey.key mycert.cer myreq.p10") << endl; 
		}


	void CMakeKeys::DoVerbage(const _TCHAR* pszText) const
		// Purpose  : Displays 'verbose' output text - if appropriate
		// Inputs   : pszText - The text to display
		{
		if(Verbose())
			DoMsg(_T(""),pszText);
		}

	void CMakeKeys::DoErrMsg(const _TCHAR* pszText) const
		{
		DoMsg(_T("error: "),pszText);
		}	

	void CMakeKeys::DoMsg(const _TCHAR* pszText1, const _TCHAR* pszText2) const
	// Purpose  : Displays 'verbose' output text - if appropriate
	// Inputs   : pszText - The text to display
		{
		OUT << pszText1 << pszText2 << endl;
		}

	// main function - constructs a CMakeKeys object and calls it's run method
	extern "C"
		{
		int main(int argc, const char *argv[], const char *envp[])
			{
			// MingW doesn't support _tmain(),
			// so using GetCommandLineW() to get wide args.
			wchar_t** argvW = CommandLineToArgvW( GetCommandLineW(), &argc);
			
			CMakeKeys app;
			return app.Run(argc, argvW);
			}
		}
