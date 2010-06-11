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
using namespace std;

void Options::DisplayError (const Exceptions& aErr)
	{
	const char* msg;
	switch (aErr)
		{
		case EUnknownOption:
			msg = "unknown option specified" ;
			DisplayUsage ();
			break;
		case EMissingDATFile:
			msg = "Dat File Input not specified ";
			DisplayUsage ();
			break;
		case EMissingSISFile:
			msg = "SIS File Input not specified ";
			DisplayUsage ();
			break;
		case EUnableToOpenFile:
			msg = "Unable to open file " ;
			break;
		case ESupportingExeError:
			msg = "Unable to execute Supporting Executable.Set Environment PATH" ;
			DisplayUsage ();
			break;
		case ECorruptCCIFile:
			msg = "File containing signing chain information is corrupted " ;
			break;
		case EErrorExecutingSupportingExeSignsis:
			msg = "Error while Executing Supporting \nExecutable SignSIS.exe.Either SIS File is Unsigned or does not exist in the specified path" ;
			break;
		case EErrorExecutingSupportingDumpSwiCertstoreExe:
			msg = "Error while Executing Supporting \nExecutable DumpSWICertstoreTool.exe" ;
			break;
		case ENotADerBitString:
			msg = "Error reading a DER encoded ASN.1 Bit String Type .";
			break;
		case ENotADerSequence:
			msg = "Error reading a DER Encoded ASN.1 Sequence Type." ;
			break;
		case ENotADerInteger:
			msg = "Error reading a DER Encoded ASN.1 Integer Type." ;
			break;
		case ENotADerUtf8String:
			msg = "Error reading a DER Encoded ASN.1 UTF8 String." ;
			break;
		default:
			msg = "unknown error";
			break;
		}
		cerr << CommandName () << ": Error in command: " << msg << std::endl <<std::endl;
	}


void Options::DisplayVersion() const
	{
	 cout << "\nDumpChainValidity Utility   Version " << KMajorVersion << '.' \
		  << setw(2) << KMinorVersion << setfill('0') <<  "\nCopyright (c) 2005-2006 Symbian Software Limited . All rights reserved.\n\n" << flush;
	}


void Options::DisplayUsage()
	{
	cout << "Usage: DumpChainValidityTool [-v][-h][-i][-l] Signed_SIS_Input DAT_Input Directory  \n\n" \
					" -v		specifies the version of the tool \n" \
					" -h		Prints the help message\n" \
					" -i        Output licence information \n" \
					" -l		Displays unified capabilities list of the validated chains \n" \
					"DAT_Input	ROM Based SWICertstore.dat.Optional if Directory is specified \n" \
					"Directory	Writable SWICertstore.Optional if swicertstore.dat is specified\n" \
					"SIS_Input	Signed SIS File\n" \
					"All inputs must be specified in the same order as mentioned above.\n\n" << flush;
	}


Options::Options (int argc, char** argv)
	:iList(false),
	iVersion (false),
	iHelpFlag (false),
	iDATFileSpecified (false),
	iSISFileSpecified (false),
	iDirectorySpecified (false)
	{
	iDATFile.empty();
	iCertstore.empty();
	iSISFile.empty();
	unsigned char stage = 0;
	bool ssl = false;
	int argCount = argc;
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
					case 'l':
					case 'L':
						iList = true;
						break;

					default:
						throw EUnknownOption;
					}
				}
			}
			
		else
			{
			switch(stage)
				{
				case 0:
						{
						iSISFile = *argv;
						iSISFileSpecified = true;
						break;
						}
				case 1:
						{
						iDATFile = *argv;
						iDATFileSpecified = true;
						break;
						}
				case 2:
						{
						iCertstore = *argv;
						iDirectorySpecified = true;
						break;
						}
				default:
						throw EUnknownOption;
				}
			stage++;
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

	CheckCmdLineExceptions();
	}

void Options::CheckCmdLineExceptions()
	{
	if(iDirectorySpecified && iDATFileSpecified)
		{
		iCertstore.append(" ");
		iCertstore.append(iDATFile);
		}


	if(iDATFileSpecified && !iDirectorySpecified)
		{
		iCertstore=iDATFile;
		}

	//if only -h or -v or both are specified
	if(!iDATFileSpecified && !iDirectorySpecified && !iSISFileSpecified )
		{
		exit(0);
		}

	//Neither dat file nor directory is specified			
	if (!iDATFileSpecified && !iDirectorySpecified )
		{
		throw EMissingDATFile;	
		}

	//no sis file specified
	if(!iSISFileSpecified)
		{
		throw EMissingSISFile;
		}
	}

Options::~Options()
	{
	}





