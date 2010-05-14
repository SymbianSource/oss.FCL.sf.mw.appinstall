/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "options.h"
#include "openssllicense.h"
#include "osinterface.h"
 
void Options::DisplayError (Exceptions aErr)
	{
	const char* msg;
	switch (aErr)
		{
		case EUnknownOption:
			msg = "unknown option specified";
			DisplayUsage ();
			break;
		case EInvalidFile:
			msg = "Invalid file specified ";
			break;
		case ECannotOpenFile:
			msg = "Cannot open file or file does'nt exist";
			break;
		case ENotAPermanentFileStore:
			msg = "Not a Permanent File Store";
			break;
		case EInvalidCertificateChain:
			msg = "Invalid Certificate Chain";
			break;
		default:
			msg = "unknown error";
			break;
		}
	cerr << CommandName () << ": Error in command: " << msg << endl;
	}

void Options::DisplayUsage ()
	{
	cout << "Usage: DumpSWICertstoreTool [-v][-h][-i][-c] [-y] [-d] datfile directory \n" \
				" -v		specifies the version of the tool. \n" \
				" -h		Prints the help message.\n" \
				" -i        Output licence information \n" \
				" -c		Extracts the certificate. \n" \
				" -y(Deprecated) hidden option which dumps the certificates in pem format \n\t\talong with their capabilities and mandatory status \n" \
				" -d		hidden option which dumps the certificates in pem format \n\t\talong with details of capabilities and mandatory status \n" \
				" datfile	swicertstore.dat(ROM Based).  \n" \
				" directory	directory where writable certstore files exists. \n \n" <<flush;
	}

void Options::DisplayVersion()
	{
	cout << "\nDumpSWICertstore Utility   Version " << KMajorVersion << '.' \
		<< setw(2) << KMinorVersion << setfill('0') <<  "\nCopyright (c) 2005-2006 Symbian Software Limited . All rights reserved.\n\n" << flush;
	}


Options::Options (int argc, char** argv)
	:iVersion (false),
	iHelpFlag (false),
	iROMCertstore (false),
	iCBasedCertstore (false),
	iNoOfDirectories (0),
	iCertificate (false),
	iCreateCCI (false),
	iCreateDetailCCI (false)
	{
	int isDat = 0;
	bool ssl = false;
	while (--argc > 0)
		{
		argv++;
		if ((**argv == '-') || (**argv == '/'))
			{
			const char* optPtr = *argv;
			while (*++optPtr)
				{
				switch (*optPtr)
					{
					case 'h':
					case 'H':
					case '?':
						{
						iHelpFlag = true;
						DisplayUsage();
						break;
						}
					case 'v':
					case 'V':
						{
						iVersion = true;
						DisplayVersion();
						break;
						}
					case 'i' :
					case 'I' :
						{
						ssl = true;
						break;
						}
					case 'c':
					case 'C':
						iCertificate = true;
						break;
  					case 'y':
 					case 'Y':
  						iCreateCCI = true;
  						break;
					case 'd':
 					case 'D':
  						iCreateDetailCCI = true;
  						break;
					default:
						throw EUnknownOption;
					}
				}
			}
		else
			{
			string fileName = *argv;
			//checks whether it is a file or directory		
			if(OSInterface::CheckIfDirectory(fileName.c_str()))
				{
				iNoOfDirectories++;
				iCBasedCertstore = true;
				int pos = fileName.find_last_of("\\" ,fileName.length());
				if(pos == (fileName.length()-1))
					{
					fileName.replace(pos,1,"");
					}

				iDirFiles = OSInterface::ExtractFilesFromDirectory(fileName);
				}
			else
				{
				iROMCertstore = true;
				iROMCertstoreFileName = fileName.c_str();
				}
			}
		}

		//displays OpenSSL copyright notice.
		if (ssl)
			{
			for (int index = 0; index < (sizeof(openSSLLicenseString)/sizeof(openSSLLicenseString[0])); ++index)
				{
				cout << openSSLLicenseString [index] << endl;
				}
			}
		
		if(iNoOfDirectories > 1)
			{
			throw EUnknownOption;
			}
	}


Options::~Options()
	{
	}