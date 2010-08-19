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
*
*/


#include <iostream>
#include "options.h"
#include "utility.h"
#include "symbiantypes.h"
#include "utility_interface.h"

void Options::DisplayError (CmdLineException err)
	{
	const char* msg;
	switch (err)
		{
		case ECmdLineUnknownOption:
			msg = "unknown option specified";
			break;
		case ECmdLineMissingSIS:
			msg = "no SIS file specified";
			break;
		case ECmdLineMultipleSIS:
			msg = "too many SIS files specified";
			break;
		case ECmdLineNoDirArgument:
			msg = "No argument supplied with the -d flag";
			break;
		case ECmdLineDirIsFile:
			msg = "Directory specified with the -d flag is a file";
			break;
		case ECmdLineNoPermission:
			msg = "Permission was denied when using directory specified with -d";
			break;
		case ECmdLineOtherDirFailure:
			msg = "Operation failed when accessing directory specified with -d";
			break;
		case ECmdLineInvalidDir:
			msg = "Directory name specified with -d could not be created, invalid name";
			break;
		default:
			msg = "unknown error";
			break;
		}
	std::cerr << CommandName () << ": Error in command: " << msg << std::endl;
	DisplayUsage ();
	}

void Options::DisplayUsage ()
	{
	std::cout << "Usage: " << CommandName () << " [-v] [-h] [-x] [-l[-y]][-d directory] filename.sis" << std::endl;
	std::cout << "Where:\t-i\tdisplays verbose output" << std::endl;
	std::cout << "\t-v\tdisplays version" << std::endl;
	std::cout << "\t-h\tdisplays this message" << std::endl;
	std::cout << "\t-x\textracts the files" << std::endl;
	std::cout << "\t-d\tspecifies where you wish to extract the files to" << std::endl; 
	std::cout << "\t-p\tpauses when finishing" << std::endl;
	std::cout << "\t-l\tlist the executable against their capablities. " << std::endl;
	std::cout << "\t\tAlso verifies capablities with executable header" << std::endl;
	std::cout << "\t-y\tlist the executable against their capablities, " << std::endl;
	std::cout << "\t\tin the format supported by DumpInstallFileStatus" << std::endl;
	std::cout << "\t\tThis option has to be given along with -l option" << std::endl;
	
	}

/**
*This is a new function added to provide version information
*/

void Options::DisplayVersion()
{
	std::cout << "\nDUMPSIS  Version  " << MajorVersion << '.' << MinorVersion << std::endl;
	std::cout << "A utility for decompiling Software Installation (SIS) files" << std::endl;
	std::cout <<"Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.\n\n" << std::flush;
}

Options::Options (int argc, wchar_t** argv)
	: iVerboseFlag (false),
	iVersion (false),
	iHelpFlag (false),
	iExtractFilesFlag (false),
	iPauseOnExit (false),
	iList(false),
	iCreateECI(false)
	{
	while (--argc)
		{
		argv++;
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
					if (	   (**argv == '-')   
#ifndef __TOOLS2_LINUX__
							|| (**argv == '/')
#endif
					   )
			{
			const wchar_t* optPtr = *argv;

			while (*++optPtr)
				{
				switch (*optPtr)
					{
					case 'd':
					case 'D':
						if (argc > 1)
							{
							argc--;
							iExtractDirectory = *(++argv);
							}
						else
							{
							
							throw ECmdLineNoDirArgument;

							}
						break;

					case 'h':
					case 'H':
					case '?':
						iHelpFlag = true;
						break;
					case 'v':
					case 'V':
						iVersion = true;
						break;

					case 'p':
					case 'P':
						iPauseOnExit = true;
						break;
					case 'i':
					case 'I':
						iVerboseFlag = true;
						break;
					case 'x':
					case 'X':
						iExtractFilesFlag = true;
						break;

					case 'l':
					case 'L':
						iList = true;
						break;
					case 'y':
					case 'Y':
						iCreateECI = true;
						break;

			
					default:
						throw ECmdLineUnknownOption;
					}
				}
			}
		else
			{
			if (iSISFileName != L"")
				{
				throw ECmdLineMultipleSIS;
				}
			else
				{
				iSISFileName = *argv;
				}
			}
		}
		
	if (iHelpFlag)
		{
		DisplayUsage ();
		if (iSISFileName == L"")	// If we were asked for help then we shouldn't
			{				// complain that there's no SIS file specified,
			exit (0);		// just exit quietly.
			}
		}

	if (iVersion)
		{
		DisplayVersion ();
		if (iSISFileName == L"")
			{
			exit (0);		
			}
		}

	if (iSISFileName == L"")
		{
		throw ECmdLineMissingSIS;	
		}
	
	// Sanity check the directory argument
	// If directory name is not provided then
	// generate a directory name.
	GetExtractDir();
	}

void Options::GetExtractDir()
	{
	iExtractDirectory = FixPathDelimiters(iExtractDirectory);

	if(iExtractDirectory != L"")
		{
		return;
		}

	int pos = iSISFileName.rfind(L".sis");
	if(std::wstring::npos == pos)
		{
		pos = iSISFileName.rfind(L".ctl");
		}
	
	if(std::wstring::npos == pos)
		{
		iExtractDirectory = L".";
		}
	else
		{
		iExtractDirectory = FixPathDelimiters(iSISFileName);
		pos = iExtractDirectory.find_last_of(L"/");
		if (std::wstring::npos == pos)
			{
			pos = 0;
			}
		else
			{
			pos += 1; // Discard "/"
			}
		
		// Discard the extension
		int dirLength = iExtractDirectory.length() - 4 - pos;
		iExtractDirectory = iExtractDirectory.substr(pos, dirLength);
		}
	}

