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
* handles parsing of makekeys command line args
* INCLUDES
*
*/


#include <windows.h>
#include <tchar.h>

#include "ParseCMD.h"
#include "openssllicense.h"

//#include <winCertificateGenerator.h>

// ===========================================================================
// CParseCmd
// Responsable for processing and maintaining the command line options
// ===========================================================================

CParseCmd::CParseCmd()
:m_dwOptions(0L),
m_fShowSyntax(FALSE)
	{
	m_dwKeyLen = DEFAULTKEYLEN;
	m_dwCertExpiryInDays = DEFAULTCERTEXPIRYDAYS;
	m_bPassword = FALSE;
	m_CMDKey = EDSACipher;
	m_bVerbose = FALSE;
	}

BOOL CParseCmd::ParseCommandLine(const int argc, const _TCHAR *argv[], Mode* aApp)
// Purpose  : Proceses the command line, and options
// Inputs   : argc, argv - command line as passed to the process
	{

	// Test the number of arguments
	if(argc < 2) throw ErrInsufficientArgs;
	
	int wCount = 1;
	int dname = 0;
	int founddname = 0;
	int command_count = 0;
	
	int MBCSstrlen;	//used to store the length of MBCS strings 
	
	// walk through the argv[] list, picking out -parameters and parameter strings.
	// Also sets mode of operation of the app
	while(wCount < argc)
		{

		//check we don't have a NULL string here
		if(argv[wCount][1] == '\0')
			throw ErrBadCommandFlag;


		/* Uncomment below for standalone key generation support
		//parse command list for operational parameters
		if (_tcscmp("-keys", argv[wCount]) == 0)	
			{
			*aApp = EKeys;
			//wcscpy(m_privateKeyFile, argv[argc - 2]);
			wcscpy(m_privateKeyFile, argv[argc - 1]);
			}
			
		else
		*/
		
		//display OpenSSL copyright notice.
		if (_tcsicmp(_T("-i"), argv[wCount]) == 0)
			{
			for (int index = 0; index < (sizeof(openSSLLicenseString)/sizeof(openSSLLicenseString[0])); ++index)
				{
				std::cout << openSSLLicenseString [index] << std::endl ;
				}
			break;
			}

		if (_tcsicmp(_T("-cert"), argv[wCount]) == 0)
			{
			if ((*aApp == ECert) || (*aApp == EReq))
				{
				throw ErrMalformedCommand;
				}
			else
				{
				*aApp = ECert;
				}
				
			if (argc < MIN_CERT_ARGS) throw ErrInsufficientArgs;
			
			}

			else if (_tcsicmp(_T("-req"), argv[wCount]) == 0)
				{
				if ((*aApp == ECert) || (*aApp == EReq))
					{
					throw ErrMalformedCommand;
					}
				else
					{
					*aApp = EReq;
					}
						
				if (argc < MIN_REQ_ARGS) throw ErrBadReqCommand;
			
				founddname = 1;							
			}

#ifdef OPTIONAL
			else if (_tcsicmp(_T("-chain"), argv[wCount]) == 0)
				{
				*aApp = EChain;
				command_count++;
				}
#endif
			else if (_tcsicmp(_T("-view"), argv[wCount]) == 0)
				{
				
				if (argc == MIN_VIEW_ARGS)
					{
					CCertificateGenerator* pView = new CCertificateGenerator;
					if (!pView) throw ErrCantViewCertificate;

					pView->View(argv[wCount + 1]);

					delete pView;
					}
					else
						throw ErrMalformedCommand;

				}

			else if (_tcsicmp(_T("-password"), argv[wCount]) == 0)
				{
				m_bPassword = true;
				command_count+=2;
				
				if (wCount < argc - 1)
					{
					_tcscpy(m_password, argv[wCount + 1]);
					if(!(_tcslen(m_password) >=4) )
						{
						_tprintf(_T("Error: Invalid Password\n"));
						throw ErrInvalidPassword;
						}
					}
				else
					throw ErrInsufficientArgs;
				
				}

#ifndef NORSA
			else if (_tcsicmp(_T("-rsa"), argv[wCount]) == 0)
				{
				m_CMDKey = ERSACipher;
				command_count++;
				}
#endif
		
			else if (_tcsicmp(_T("-dsa"), argv[wCount]) == 0)
				{
				m_CMDKey = EDSACipher;
				command_count++;
				}
		
			else if(_tcsicmp(_T("-dname"), argv[wCount]) == 0)
				{
				if(wCount < argc - 1)
					{
					//handle MBCS correctly (we are assuming that the parameter after -dname IS the dname string
					MBCSstrlen = _tcslen(argv[wCount + 1]) * sizeof(TCHAR);
					_tcscpy(m_dname, argv[wCount + 1]);
					dname = 1;
					}
					else
						throw ErrInsufficientArgs;
				}
			else if(_tcsicmp(_T("-len"), argv[wCount]) == 0)
				{
				//handle MBCS correctly
				if(wCount < argc - 1)
					{
					_tcscpy(m_KeyLenStr, argv[wCount + 1]);
					_stscanf(m_KeyLenStr, _T("%d"), &m_dwKeyLen);
				
					command_count+=2;	//len takes an additional parameter, so bump by 2
			
					if (m_dwKeyLen < 512 || m_dwKeyLen > 4096)
						{
						_tprintf(_T("Error: key length should be between 512 and 4096\n"));
						throw ErrInsufficientArgs;
						}
					}
				else
					throw ErrInsufficientArgs;
				}

			else if (_tcsicmp(_T("-expdays") , argv[wCount]) == 0 )
			{
			if(wCount < argc - 1)
				{
				_tcscpy(m_CertExpiryInDaysStr, argv[wCount + 1]);
				_stscanf(m_CertExpiryInDaysStr, _T("%d"), &m_dwCertExpiryInDays);

				command_count+=2;	//expiry days takes an additional parameter, so command_count+2
				}

			else
				{
				throw ErrInsufficientArgs;
				}
			}

			else if(_tcsicmp(_T("-v"), argv[wCount]) == 0)
				{
				SetVerbose(TRUE);
				command_count++;	//-v does not take args, so bump by 1
				}
			else if(argv[wCount][0] == '-')
			//if we get here we can't parse the option chosen
				{
				_tprintf(_T("Error: unknown command option %s\n"), argv[wCount]);
				throw ErrUnknownCommandFlag;		
				}

			wCount++;
		}

	//distinguished names are not optional, so throw up a message
	if (!dname && founddname)
		{
		_tprintf(_T("Error: Missing \"-dname\" command\n"));
		throw ErrInsufficientArgs;
		}


	//check to see if all the optional commands have the correct number of arguments
	if (((command_count + MIN_REQ_ARGS > argc) && (*aApp == EReq)) ||
		((command_count + MIN_CERT_ARGS > argc) && (*aApp == ECert)))

		{
		printf("Error: Command Line malformed - too few arguements\n");
		throw ErrInsufficientArgs;
		}

	if (*aApp == EReq)
		{
 		_tcscpy(m_privateKeyFile, argv[command_count + 4]);
 		_tcscpy(m_publicKeyFile, argv[command_count + 5]);
 		_tcscpy(m_requestFile, argv[command_count + 6]);
		

		if (wCount >= (command_count+2) + MIN_REQ_ARGS - 1)
			{
			printf("Error: Too many arguments for -req command\n");
			throw ErrTooManyArgs;
			}
		}

	if (*aApp == ECert)
		{
		_tcscpy(m_privateKeyFile, argv[command_count + 4]);
		_tcscpy(m_publicKeyFile, argv[command_count + 5]);
		
		if (wCount >= (command_count+2) + MIN_CERT_ARGS - 1)
			{
			_tprintf(_T("Error: too many arguments for -cert command\n"));
			throw ErrTooManyArgs;
			}
		}
	}

int CParseCmd::GetCMDKeyLen()
	{
	return m_dwKeyLen;
	}

int CParseCmd::GetCMDCertExpiryInDays()
	{
	return m_dwCertExpiryInDays;
	}

_TCHAR* CParseCmd::GetPrivateName()
	{
	return m_privateKeyFile;
	}

_TCHAR* CParseCmd::GetPublicName()
	{
	return m_publicKeyFile;
	}

_TCHAR* CParseCmd::GetRequestName()
	{
	return m_requestFile;
	}

_TCHAR* CParseCmd::GetDNameString()
	{
	return m_dname;
	}




_TCHAR* CParseCmd::GetPassword()
	{
	return m_password;
	}

TKeyType CParseCmd::GetKeyType()
	{
	return m_CMDKey;
	}

bool CParseCmd::GetPasswordEnabled()
	{
	return m_bPassword;	
	}

void CParseCmd::SetVerbose(const bool bVerbose)
	{
	m_bVerbose = bVerbose; 

	}

bool CParseCmd::GetVerbose()
	{
	return m_bVerbose; 

	}
