/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "capabilities.h"
using namespace std;

/**
 * Command Line Exception Messages
 */

void Options::DisplayError (const Exceptions& aErr)
	{
	const char* msg;
	switch (aErr)
		{
		case EUnknownOption:
			msg = "Unknown option specified";
			DisplayUsage ();
			break;
		case EMissingDATFile:
			msg = "Dat file input not specified ";
			DisplayUsage ();
			break;
		case ESupportingExeError:
			msg = "Unable to execute supporting executable";
			break;
		case EMissingSISFile:
			msg = "SIS file input not specified ";
			DisplayUsage ();
			break;
		case EUnableToOpenFile:
			msg = "Unable to open file ";
			break;
		case ECorruptCCIFile:
			msg = "File containing signing chain information is corrupted " ;
			break;
		case EErrorExecutingSupportingExeDumpsis:
			msg = "Error while executing supporting \nexecutable dumpsis.exe";
			break;
		case EErrorExecutingSupportingExeSignsis:
			msg = "Error while executing supporting \nexecutable signsis.exe";
			break;
		case EErrorExecutingSupportingDumpSwiCertstoreExe:
			msg = "Error while executing supporting \nexecutable dumpswicertstore.exe";
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
			msg = "Unknown error";
			break;
		}
	cerr << CommandName () << ": Error in command: " << msg << endl;
	}


void Options::DisplayVersion() const
	{
	cout << "\nDumpInstallFileStatus Utility   Version " << KMajorVersion << '.' \
		<< setw(2) << KMinorVersion << setfill('0') <<  "\nCopyright (c) 2005-2006 Symbian Software Limited . All rights reserved.\n\n" << flush;
	}

void Options::DisplayUsage()
	{
	cout <<"Usage:DumpInstallFileStatusTool [-v][-h][-i] SISFile DATFile Directory Capabilities \n\n" \
				" -v		Specifies the version of the tool. \n" \
				" -h		Prints the help message. \n"  \
				" -i        Output licence information \n" \
				" SISFile	Signed SIS File.\n" \
				" DATFile	ROM Based SWICertstore.dat.OPTIONAL, if Directory is specified.\n"  \
				" Directory	Writable SWICertstore.OPTIONAL,if swicertstore.dat is specified.\n" \
				" Capabilities	List of capabilities to be specified by user(OPTIONAL).\n" \
				" All Inputs must be specified in the same order as mentioned above.\n\n" <<flush; 
	}


Options::Options (int argc, char** argv)
	:iVersion (false),
	iHelpFlag (false),
	iDATFileSpecified (false),
	iSISFileSpecified (false),
	iDirectorySpecified (false),
	iUserCapabilities(0)
	{
	StringVector userCapabilities;
	unsigned char stage = 0;
	bool ssl = false;
	string fileInput;
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
						iSisFile = *argv;
						iSISFileSpecified = true;
						break;
						}
					case 1:
						{
						iDATFile = *argv;
						iDATFileSpecified = true;
						break;
						}

					default:
						{
						string fileInput = *argv;
						if(PathIsDirectory(fileInput.c_str()))
							{
							iCertstore = fileInput;
							iDirectorySpecified = true;
							}
						else if(fileInput.find(".")== string::npos)
							{
							string capabilities = fileInput;
							userCapabilities.push_back(capabilities);
							}
						else
							{
							throw EUnknownOption;
							}
						break;
						}
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

		//if user capabilities specified.
		if(userCapabilities.size())
			{
			iUserCapabilities = ConvertUserCapsToInt(userCapabilities);
			}

		if(iDirectorySpecified && iDATFileSpecified)
			{
			iCertstore.append(" ");
			iCertstore.append(iDATFile);
			}

		if(iDATFileSpecified && !iDirectorySpecified)
				{
				iCertstore = iDATFile;
				}
		
		//if only -h or -v is specified,display and exit.
		if(!iDATFileSpecified && !iDirectorySpecified && !iSISFileSpecified)	
			{
			exit(0);
			}
				
		if (!iDATFileSpecified && !iDirectorySpecified)
			{
			throw EMissingDATFile;	
			}

		if(!iSISFileSpecified)
			{
			throw EMissingSISFile;
			}
	}

const int Options::ConvertUserCapsToInt(vector <string>& aUserCapabilities)
	{
	string listOfCapabilities[KNumberOfCaps];
	int userCaps = 0;
	if(aUserCapabilities.size())
		{
		for(StringVector::iterator userCapsIter = aUserCapabilities.begin(); userCapsIter != aUserCapabilities.end(); ++userCapsIter)
			{
			string capabilities = *userCapsIter;
			//convert each user capability to uppercase.
			for(int j = 0 ; j < capabilities.length() ; j++)
				{
				capabilities[j] = ::toupper(capabilities[j]);
				}

			//convert the predefined CapabilityList into uppercase.
			for(int k = 0 ; k < KNumberOfCaps ; k++)
				{
				listOfCapabilities[k] = CapabilityList[k];
				for(int j = 0 ; j < listOfCapabilities[k].length() ; j++)
					{
					listOfCapabilities[k][j] = ::toupper(listOfCapabilities[k][j]);
					}
				}
		
			for (int i = 0; i < KNumberOfCaps; i++)
				{
					if(capabilities.compare(listOfCapabilities[i])==0)
					{
					userCaps += pow((double)2,i);
					}
				}
			}
		}
	return  userCaps;
	}

Options::~Options()
	{
	}



