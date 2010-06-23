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
#include <iostream>
#include <windows.h>

void Options::DisplayError (const Exceptions& aErr)
	{
	const char* msg;
	switch (aErr)
		{
		case EUnknownOption:
			msg = "Unknown option specified";
			DisplayUsage ();
			break;
		case EMissingRegistryEntry:
			msg = "One of the registry entry (PkgUID ,PkgName and VendorName ) not specified";
			DisplayUsage ();
			break;
		case EMissingPkgUID:
			msg = "Package UID not specified ";
			DisplayUsage ();
			break;
		case EInvalidOption:
			msg = "Invalid option specified ";
			DisplayUsage ();
			break;
		case EInvalidPackageUID:
			msg = "Invalid Package UID";
			break;
		case ESetEnvironmentVariable:
			msg = "Set the environment variable or \nspecify registry file path";
			break;
		case EInvalidFile:
			msg = "Registry format not supported .";
			break;
		default:
			msg = "Unknown error";
			break;
		}

	cerr << CommandName () << ": Error : " << msg << endl;
	}


void Options::DisplayVersion()
	{
	cout << "\nDumpSWIRegistry Utility   Version " << KMajorVersion << '.' \
			<< setw(2) << KMinorVersion << setfill('0') <<  "\nCopyright (c) 2005-2006 Symbian Software Limited . All rights reserved.\n\n" << flush;
	}


void Options::DisplayUsage()
	{
	cout << "Usage: DumpSWIRegistryTool [-v] [-h] [RegistryPath] [-p PackageUid] "  
			"\n\t\t\t[-r PackageUid PackageName VendorName] \n\n" \
			" -v		specifies the version of the tool \n" \
			" -h		Prints the help message\n" \
			" -p		interpret package information by specifying packageuid \n\n" \
			" -r		Displays controller information about a particular package ,"
			"\n\t\twhen it's registry entry(package uid,package name & vendor name"
			"\n\t\t)are specified."
			"\n\t\tThe registry entry can be chosen from the information displayed"
			"\n\t\tby -p option.\n\n" \
			"RegistryPath	swiregistry path - OPTIONAL if Environment PATH is set \n" \
			"PackageUid	UID of the package.  \n" \
			"PackageName	Name of the package obtained when -p option is used \n" \
			"VendorName	Name of the vendor obtained when -p option is used \n\n" \
			"Set the environment variable DUMPSWIREGISTRY to the sisregistry path - DEFAULT: \n" \
			"DUMPSWIREGISTRY=\\epoc32\\<platform>\\c\\sys\\install\\sisregistry\\ \n" \
			"All inputs must be specified in the same order as mentioned above. \n\n" << flush;
	}
 

Options::Options (int argc, char** argv)
	:iVersion (false),
	iHelpFlag (false),
	iPkgUIDSpecified (false),
	iRegistryEntrySpecified (false)
	{
	int argCount = argc;
	const char* buffer = getenv("DUMPSWIREGISTRY");
	if(buffer)
		{
		string regPath(buffer);
		iRegistryFilePath = regPath;
		}
	
	++argv;
	//check whether the first argument is a registry path
	if((**argv != '-') && (**argv != '/'))
		{
		iRegistryFilePath = *argv;
		++argv;
		--argc;
		} 

	if(*argv != NULL)
		{
		if ((**argv != '-') && (**argv != '/'))
			{
			throw EInvalidOption;
			}

		switch ((*argv)[1])
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

			case 'p':
			case 'P':
				{
				iPkgUIDSpecified = true;
				if(iRegistryFilePath.length() == 0)
					{
					throw ESetEnvironmentVariable;
					}
			
				if(argc !=3)
					{
					throw EInvalidOption;
					}
			
				iPkgUID = *++argv;
				break;
				}

			case 'r':
			case 'R':
				{
				iRegistryEntrySpecified = true;
				if(iRegistryFilePath.length() == 0)
					{
					throw ESetEnvironmentVariable;
					}

				if(argc != 5)
					{
					throw EInvalidOption;
					}
			
			iPkgUID = *++argv;
			iPkgName = *++argv;
			iVendorName = *++argv;
			break;
			}
					
			default:
				{
				throw EInvalidOption;
				}
			}
		}

	else
		{
		throw EInvalidOption;
		}

	CheckCmdLineExceptions();
	}


void Options::CheckCmdLineExceptions() 
	{
	//if -p is specified 
	if (iPkgUIDSpecified)
		{
		if(iPkgUID.length()==0)
			{
			throw EMissingPkgUID;	
			}
		//check whether a valid pkg uid is specified
		else if((iPkgUID.length()<8) || (iPkgUID.length()>8))
			{
			throw EInvalidPackageUID;
			}
		else
			{
			//handles both ways of specifying registrypath(for instance:\c\sys\install\sisregistry or \c\sys\install\sisregistry\)
			int pos = iRegistryFilePath.find_last_of("\\" ,iRegistryFilePath.length());
			if(pos != (iRegistryFilePath.length()-1))
				{
				iRegistryFilePath.append("\\");
				}
			iRegistryFilePath.append(iPkgUID);
			iPkgUID = iRegistryFilePath;			
			}
		}

	//if -r is specified and any one of the entries are missing
	if(iRegistryEntrySpecified)
		{
		if(((iRegistryFilePath.length())== 0) || ((iPkgName.length())== 0) || ((iVendorName.length())== 0))
			{
			throw EMissingRegistryEntry;
			}
		else
			{
			int pos = iRegistryFilePath.find_last_of("\\" , iRegistryFilePath.length());
			if(pos != (iRegistryFilePath.length()-1))
				{
				iRegistryFilePath.append("\\");
				}
			iRegistryFilePath.append(iPkgUID);
			}
		}
	}

Options::~Options()
	{
	}

