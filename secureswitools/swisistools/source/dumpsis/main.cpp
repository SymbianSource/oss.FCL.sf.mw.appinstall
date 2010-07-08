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

#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif // _MSC_VER


#include <iostream>
#include <string>
#include <fstream>
#include <wchar.h>
#include <openssl/err.h>
#include <stdio.h>

#include "utility_interface.h"
#include "options.h"
#include "siscontents.h"
#include "siscontroller.h"
#include "dumpsis.h"

enum DumpsisException
	{
	EDumpsisExecutionSuccessful,
	EDumpsisExecutionError,
	EDumpsisUnknownError
	};

void DisplayFileCapList(CDumpSis& dumpsis, Options& aOptions)
	{
	TFileCapTestList fileList;
	dumpsis.GetCapVerifiedFileList(fileList);
	int fileCount = fileList.size();

	if(fileCount != 0 && aOptions.CreateECI() && !aOptions.Verbose())
		{
		std::wcout << L"[FILELIST]" << std::endl;
		}

	for(int i = 0; i < fileCount; ++i)
		{
		std::wcout << std::endl; 
		const CSISFileDescription* fdesc = fileList[i].iFileDesc; 
		std::wstring str = fdesc->Target().GetString();
		std::wcout << L"File" << i+1;
		if(aOptions.CreateECI() && !aOptions.Verbose())
			{
			std::wcout << L"=" << str;
			std::wcout << L"@" << fdesc->Capabilities() << std::endl;
			}
		else
			{
			std::wcout << L":" << str.c_str();
			std::vector<std::wstring> capList;
			fdesc->GetCapabilityList(capList);
			std::wcout << std::endl;
			for(int j = 0; j < capList.size(); ++j)
				{
				std::wcout << L"\t" << capList[j] << std::endl;
				}
			}
		}
	if(!aOptions.Verbose())
		{
		for(int i = 0; i < fileCount; ++i)
			{
			if(fileList[i].iActualCap < 0)
				{
				continue;
				}
			if(aOptions.CreateECI() && fileList[i].iFileDesc->Capabilities() == 0)
				{
				std::wcout << L"0" << std::endl;
				}
			else if(!aOptions.CreateECI())
				{
				std::wcout << std::endl;
				std::wcout << L"Executable" << i+1; 
				if(fileList[i].iActualCap)
					{
					std::wcout << L": capabilities matched with the SIS file header capabilities";
					}
				else
					{
					std::wcout << L": capabilities mismatched with the SIS file header capabilities";
					}
				std::wcout << std::endl << std::endl;
				}
			}
		}
	}

int main(int argc, char *argv[])
	{
	wchar_t **argv1 = CommandLineArgs(argc,argv);

	Options* options = NULL;
	int retVal = EDumpsisExecutionSuccessful;
	try
		{
		options = new Options (argc, argv1);
		
		SISLogger::SetStream(std::wcout);
		
		CDumpSis dumpsis(options->SISFileName(), options->Verbose());
		
		CDumpSis::TExtractionLevel extractionLevel = CDumpSis::ENone;
		
		if(options->ExtractFiles())
			{
			extractionLevel = (options->Verbose())? CDumpSis::EEverything : CDumpSis::EAllButCerts;
			}
		else
			{
			extractionLevel = CDumpSis::EAllDataFiles;
			}
		
		dumpsis.ExtractFiles(options->ExtractDirectory(), extractionLevel);
		
		if(options->Listexe())
			{
			DisplayFileCapList(dumpsis, *options);
			}
		}
	catch (CmdLineException err)
		{
		Options::DisplayError (err);
		retVal = EDumpsisExecutionError;
		}
	catch(CSISException oops)
		{
		switch(oops.ErrorCategory())
			{
			case CSISException::EFileProblem:
				std::cerr << "cannot open specified SIS file for reading" << std::endl;
				break;
			case CSISException::EFileFormat:
			case CSISException::EMemory:
			case CSISException::EIllegal:
				std::cerr << "error whilst parsing file contents" << std::endl;
				break;
			case CSISException::ELegacyFormat:
				std::cerr << "This is an old format (pre 9.x) SIS file, which is no longer supported" << std::endl;
				break;
			case CSISException::ECompress:
				std::cerr << "error while decompressing" << std::endl;
				break;
			default:
				std::cerr << "unknown error reading SIS file" << std::endl;
				break;
			}
		retVal = EDumpsisExecutionError;
		}
	catch (std::exception &err)
		{
		std::cerr << "Error: " << err.what() << std::endl;
		retVal = EDumpsisExecutionError;
		}
	catch (...)
		{
		retVal = EDumpsisUnknownError;
		std::cerr << "Unknown error";
		}

	if ((NULL != options) && (options->PauseOnExit()))
		{
		std::cout << argv[0] << " Finished" << std::endl;
		getchar ();
		}
	delete options;

	return retVal;
	}
