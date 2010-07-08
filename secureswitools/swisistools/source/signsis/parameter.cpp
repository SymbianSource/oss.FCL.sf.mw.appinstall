/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#include "parameter.h"
#include "openssllicense.h"
#include "utility.h"
#include <string.h>


#include <iostream>

CParameter::CParameter () :
		iSign (false),
		iUnsign (false),
		iReport (false),
		iVerbose (false),
		iDump (false),
		iAlgorithm (CSISSignatureAlgorithm::EAlgNone)
	{
	}



bool CParameter::CommandLine (int argc, _TCHAR* argv[])

{
	bool help = argc == 1;
	bool ssl = false;
	unsigned char stage = 0;
	bool bien = argc > 1;
	bool multi = false;

	iSign = true;

	for (int index = 1; index < argc; index++)
		{
 			// WINDOWS ENVIRONMENT : If the underlying platform is WINDOWS then, 
 			// cope up with multiple arguments following the '-' or '/'. 
 			//
 			// LINUX ENVIRONMENT : If the underlying platform is LINUX then, cope 
 			// up with multiple arguments following only the '-'. This restriction 
 			// of not dealing with arguments following '/' is due to the fact that, 
 			// the absolute paths in case of LINUX start with a '/'. So, this could 
 			// be mistaken as an option if we treat anything prefixed by a '/' as 
 			// an option. Hence, this facility is being removed once for all and 
 			// only '-' can(should) be used for specifying an option.
		    if (	   (argv [index][0] == '-')   
#ifndef __LINUX__			
				    || (argv [index][0] == '/')
#endif
			   )
			{
 			int wCharacter = 1;
 			while (argv[index][wCharacter] !='\0')
				{
				switch (argv [index][wCharacter])
					{
				case 'a' :
				case 'A' :
#ifdef _DEBUG
					iDump = true;
#endif
					break;
#ifdef GENERATE_ERRORS
				case 'b' :
				case 'B' :
					{
					for (int offset = wCharacter+1; argv [index] [offset]; offset++)
						{
						switch(argv [index] [offset])
							{
						case 'a' :
						case 'A' :
							CSISFieldRoot::SetBug (CSISFieldRoot::EBugArrayCount);
							break;
						case 'b' :
						case 'B' :
							CSISFieldRoot::SetBug (CSISFieldRoot::EBugInsaneBlob);
							break;
						case 'c' :
						case 'C' :
							CSISFieldRoot::SetBug (CSISFieldRoot::EBugCRCError);
							break;
						case 'e' :
						case 'E' :
							CSISFieldRoot::SetBug (CSISFieldRoot::EBugBigEndian);
							break;
		
						case 'f' :
						case 'F' :
							CSISFieldRoot::SetBug (CSISFieldRoot::EBugDuffFieldType);
							break;
						case 'h' :
						case 'H' :
							CSISFieldRoot::SetBug (CSISFieldRoot::EBugHashError);
							break;
						case 'l' :
						case 'L' :
							CSISFieldRoot::SetBug (CSISFieldRoot::EBugInvalidLength);
							break;
						case 'm' :
						case 'M' :
							CSISFieldRoot::SetBug (CSISFieldRoot::EBugMissingField);
							break;
						case 'n' :
						case 'N' :
							CSISFieldRoot::SetBug (CSISFieldRoot::EBugNegativeLength);
							break;
						case 's' :
						case 'S' :
							CSISFieldRoot::SetBug (CSISFieldRoot::EBugInsaneString);
							break;
						case 't' :
						case 'T' :
							CSISFieldRoot::SetBug (CSISFieldRoot::EBug32As64);
							break;
						case 'u' :
						case 'U' :
							CSISFieldRoot::SetBug (CSISFieldRoot::EBugUnexpectedField);
							break;
						case 'v' :
						case 'V' :
							CSISFieldRoot::SetBug (CSISFieldRoot::EBugInvalidValues);
							break;
						case 'x' :
						case 'X' :
							CSISFieldRoot::SetBug (CSISFieldRoot::EBugUnknownField);
							break;
							}
						wCharacter++;
						}
				}
				break;
#endif // GENERATE_ERRORS
				case 'c' :
				case 'C' :
					{
					switch (argv [index][wCharacter+1])
						{
					case 'd' :
					case 'D' :
						if (iAlgorithm != CSISSignatureAlgorithm::EAlgNone)
							{
							multi = true;
							break;
							}
						iAlgorithm = CSISSignatureAlgorithm::EAlgDSA;
						break;
					case 'r' :
					case 'R' :
						if (iAlgorithm != CSISSignatureAlgorithm::EAlgNone)
							{
							multi = true;
							break;
							}
						iAlgorithm = CSISSignatureAlgorithm::EAlgRSA;
						break;
						}
					wCharacter++;
					}
					break;
				case 'h' :
				case 'H' :
				case '?' :
					help = true;
					break;
				case 'i' :
				case 'I' :
					ssl = true;
					break;
				case 'o' :
				case 'O' :
					iReport = true;
					iSign = false;
					break;
				//EC023 additional prarameter added to extract the certificate 
				case 'p' :
				case 'P' :
					iExtractCert = true;
					iSign = false;
					break;
#ifdef GENERATE_ERRORS
				case 'q' :
				case 'Q' :
					CSISFieldRoot::SetBugStart (atoi(wstring2string (&argv [index] [wCharacter+1]).c_str ()));
					wCharacter++;
					break;
				case 'r' :
				case 'R' :
					CSISFieldRoot::SetBugRepeat (atoi (wstring2string (&argv [index] [wCharacter+1]).c_str ()));
					wCharacter++;
					break;
#endif // GENERATE_ERRORS
				case 's' :
				case 'S' :
					iSign = true;
					break;
				case 'u' :
				case 'U' :
					iUnsign = true;
					iSign = false;
					break;
				case 'v' :
				case 'V' :
					iVerbose = true;
					break;
				case 'x' :
				case 'X' :
					{
					for (int offset = wCharacter+1; argv [index][offset]; offset++)
						{
						switch(argv[index][offset])
							{
						case 'c' :
						case 'C' :
							CSISFieldRoot::SetDebugOption (CSISFieldRoot::EDbgControllerChecksum);
							break;
						case 'd' :
						case 'D' :
							CSISFieldRoot::SetDebugOption (CSISFieldRoot::EDbgDataChecksum);
							break;
						case 'w' :
						case 'W' :
							CSISFieldRoot::SetDebugOption (CSISFieldRoot::EDbgCompress);
							break;
						case 'x' :
						case 'X' :
							CSISFieldRoot::SetDebugOption (CSISFieldRoot::EDbgNoCompress);
							break;
							}
						wCharacter++;
						}
					}
					break;
		
				default :
					std::cout << "Unknown switch " << argv [index];
					bien = false;
					break;
					}
				wCharacter++;
				}
			}
		else 
			{
			switch (stage)
				{
			case 0 :
				iSISFileName = argv [index];
				break;
			case 1 :
				iOutput = argv [index];
				break;
			case 2 :
				iCertificate = argv [index];
				break;
			case 3 :
				iKey = argv [index];
				break;
			case 4 :
				iPassPhrase = argv [index];
				break;
			default :
				if (bien)
					{
					std::cout << "Unexpected arguments from  "; //<< std::string (argv [index]) << "." << std::endl;
					HANDLE	hndl;
					DWORD bytesWritten;
					//Get command prompt handle
					hndl = GetStdHandle(STD_OUTPUT_HANDLE);
					WriteConsole(hndl, argv [index], wcslen(argv [index]), &bytesWritten, 0);
					std::cout << "." << std::endl;
					bien = false;
					}
				break;
				}
			stage++;
			}
		}

	if (bien && ! iSign && ! iUnsign && ! iReport && !iExtractCert)
		{
		std::cout << "Nothing to do." << std::endl;
		bien = false;
		}

	if (bien && multi)
		{
		std::cout << "More than one signing algorithm specified." << std::endl;
		bien = false;
		}

	if (bien)
		{
		if (iSign && iUnsign)
			{
			std::cout << "Can't sign and unsign in single operation." << std::endl; 
			bien = false;
			}
		else
			if (iSign || iUnsign || iReport || iExtractCert)
			{
			if (iSISFileName.empty ()) 
				{
				std::cout << "Please name the input file." << std::endl; 
				bien = false;
				}
			else
					if ((! iReport && iOutput.empty ())&& (! iExtractCert && iOutput.empty())) 
				{
				std::cout << "Please name the output file." << std::endl; 
				bien = false;
				}
			}
		}

	if (iVerbose)
		{
		ShowBanner ();
		}

	if (bien && ! iSign && ! iCertificate.empty ())
		{
		std::cout << "A certificate is only used for signing a sis file." << std::endl;
		bien = false;
		}

	if (! bien && ! help)
		{
		std::cout << "Type \"" << "signsis" << " -?\" for command line options." << std::endl;
		}

	if (ssl)
		{
		for (int index = 0; index < (sizeof(openSSLLicenseString)/sizeof(openSSLLicenseString[0])); index++)
			std::cout << openSSLLicenseString [index] << std::endl;
		std::cout << std::endl;
		bien = false;
		}

	if (help)
		{
		std::cout << "Usage: SignSIS [-?] [-c...] [-i] [-o[-p]] [-s] [-u] [-v] input [output [certificate key [passphrase] ] ]" << std::endl << std::endl;
		std::cout << "-? or -h      Output this information" << std::endl;
		std::cout << "-c...         Sign using specific algorithm: -cd for DSA, -cr for RSA" << std::endl;
		std::cout << "-i            Output licence information" << std::endl;
		std::cout << "-o            Report on content of sis file (after any other operation)" << std::endl;
		std::cout << "-p            Extracts the certificates present (This option has to be given along with -o option)"<< std::endl;
		std::cout << "-s            Sign SIS file (requires sis file, certificate, key and passphrase)" << std::endl;
		std::cout << "-u            Remove most recent signature from SIS file" << std::endl;
		std::cout << "-v            Verbose output" << std::endl;
		std::cout << "input         The SIS file to be sign, unsigned or investigated" << std::endl;
		std::cout << "output        The SIS file generated by signing or unsigned" << std::endl;
		std::cout << "certificate   The certificate file used for signing" << std::endl;
		std::cout << "key           The certificate's private key file" << std::endl << std::endl;
		std::cout << "passphrase    The certificate's private key file's passphrase" << std::endl << std::endl;
		std::cout << "You can sign or unsign a file in a single operation." << std::endl;
		std::cout << "You can generate SIS files using MakeSIS v5." << std::endl << std::endl;
		bien = false;
		}

	return bien;
}

void CParameter::ShowBanner()
// Displays the copyright...
	{
	std::cout << std::endl << "SIGNSIS  " << " Version  5.1" << std::endl;
	std::cout << "A utility for signing Software Installation (SIS) files." << std::endl;
	std::cout << "Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). All rights reserved." << std::endl;
#ifdef _DEBUG
	std::cout << std::endl << "Development Version" << std::endl;
#endif
	std::cout << std::endl;
	}



bool CParameter::iExtractCert=false;
