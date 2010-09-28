/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

// System includes
#include <iosfwd>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>

#ifndef __TOOLS2_LINUX__
#include <windows.h>
#endif

// User includes
#include "commandparser.h"
#include "openssllicense.h"
#include "sisregistry.h"
#include "parameterlist.h"
#include "is_utils.h"
#include "stringutils.h"

// Global/static variables
CCommandParser::WarnMap CCommandParser::KString2Warning;


CCommandParser::CCommandParser():
    iWarnLevel( WARN_WARN )
    {
    }

CCommandParser::~CCommandParser()
    {
    }

void CCommandParser::DisplayError( int aError )
    {
	const wchar_t* msg;
    //
	switch( aError )
	{
	case ECmdLineUnknownOption:
		msg = L"Unknown option specified";
		break;
	case ECmdLineMissingParamFile:
		msg = L"Can't open the parameter file specified";
		break;
	case ECmdLineNoDirArgument:
		msg = L"No directory(s) specified";
		break;
	case ECmdLineNoSisArgument:
		msg = L"No SIS file(s) specified";
		break;
	case ECmdLineNoRomRofsLogsArgument:
		msg = L"No ROM/ROFS[BUILD] log file(s) specified";
		break;
	case ECmdLineNoConfigFileArgument:
		msg = L"No configuration file specified";
		break;
	case ECmdLineNoSisStubPathArgument:
		msg = L"No SIS stub directory specified";
		break;
	case ECmdLineMissingParams:
		msg = L"Missing parameters";
		break;
    case ECmdLineInvalidLanguage:
		msg = L"Invalid language code";
        break;
	case ECmdLineMissingPackageUID:
		msg = L"Package UID not found";
        break;
	default:
		msg = L"Unknown error";
		break;
	}
    //
	LERROR(msg << std::endl);
	DisplayUsage();
    }

void CCommandParser::DisplayOpenSSLCopyright()
	{
	//displays OpenSSL copyright notice.
	for (int index = 0; index < (sizeof(openSSLLicenseString)/sizeof(openSSLLicenseString[0])); ++index)
		{
		std::cout << openSSLLicenseString [index] << std::endl;
		}
	}

void CCommandParser::DisplayUsage()
    {
	DisplayVersion();

	std::cout
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		<< "Usage: " << CommandName () << " [-z dir] [-c dir] [-e] [-f] [-k [4.0 | 5.0 | 5.1 | 5.2 | 5.3 | 5.4]]\n"
		#else
		<< "Usage: " << CommandName () << " [-z dir] [-c dir] [-e] [-k [4.0 | 5.0 | 5.1 | 5.2 | 5.3 | 5.4]]\n"
		#endif
		<< "\t\t[-s [sisfile | dir] [+drive [+mcard | +mcardnr | +nonremovablepkg | +mcardalone | +mcardalonenr] [+sucert]]] [-s ...]\n"
		<< "\t\t[-p param_file] [-d drive] [-r rofsbuild_log_file1,rofsbuild_log_file2,...]\n" 
		<< "\t\t[-t romstubdir] [-n language_code] [-i config_file] \n"
		<< "\t\t[-x pkgUID[,pkgUID2,...]] [-w [off | error | warn | info]] [-l logfile]\n\n"
		<< "Where:\t-h\t\tDisplays help\n"
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		<< "\t-a\t\tThe path representing the location of Resource files\n"
		<< "\t  \t\tSpecifying the ROM Drive (-z) is mandatory.\n"
		<< "\t  \t\tLocalize resource file is read from (Rom_drive\\path_specified_in_resource_file)\n"
		#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		<< "\t-c\t\tThe directory representing the system drive on the device\n"
		<< "\t-d\t\tThe system drive letter [default to 'C']\n"
		<< "\t-e\t\tDisable eclipsing and SID checks using Z drive \n"
		<< "\t  \t\t-z or -r not required when this option used \n"
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		<< "\t-f\t\tUsed to specify that origin of a component has not been verified during installation.\n"
		<< "\t\t\tBy default the origin is verified. This option should not be provided along with registry version information.\n"		
		#endif
		
		<< "\t-i\t\tConfig file with HAL attribute values for installation \n";
	std::cout
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		<< "\t-k\t\tSIS Registry version to generate (default is to generate the db related registry entry) \n"
		#else
		<< "\t-k\t\tSIS Registry version to generate (default v" << SisRegistry::KSisRegistryMajorVersion 
		<< "." << SisRegistry::KSisRegistryMinorVersion << ") \n"
		#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		<< "\t-l\t\tThe file to write diagnostics to (stderr by default)\n"
		<< "\t-n\t\tLanguage code - decimal number as defined in TLanguage enum\n"
		<< "\t-o\t\tOutput licence information\n"
		<< "\t-p\t\tA file to take additional parameters from.\n"
		<< "\t  \t\tCommand line args after the file will override the file contents\n"
		<< "\t-r\t\tList of rom/rofs build log files; should not use with -z option or\n"
		<< "\t	\t\tdefine the Z drive in the config file\n"
		<< "\t-s\t\tThe SIS file(s) to install. Any directory passed in will be searched\n"
		<< "\t\t\tfor SIS files; should not use with -x option\n"

		<< "\t-t\t\tROM Stub sis files directory, must be used with -r option\n"
		<< "\t-v\t\tDisplays the version of the tool\n"
		<< "\t-w\t\tThe level of diagnostics to display- WARN, ERROR, INFO, OFF\n"
		<< "\t  \t\t(warn by default)\n"

		<< "\t-x\t\tRemove the files installed of a package; should not use with -s option\n"
		<< "\t-z\t\tThe directory representing the Z: drive on the device\n"
		<< "\tpkgUID\t\tThe installed package UID either in Hex or Decimal format (e.g. 0x12345678, 305419896)\n"
		<< "\tsisfile\t\tThe SIS file to be installed\n"
		<< "\tdir\t\tThe directory that contains the installing SIS files\n"
		<< "\tdrive\t\tThe drive letter on the device\n"
		<< "\tmcard\t\tOption to generate the stub SIS file for the installing package\n"
		<< "\tmcardnr\t\tOption to generate the non-removable stub SIS file for the installing package\n"
		<< "\tnonremovablepkg\tOption to generate the non-removable SIS file for the installing package\n\n"
		<< "\tmcardalone\tOption to just create a pre-installed package to the media card\n"
		<< "\tmcardalonenr\tOption to generate the non-removable stub SIS file for the installing" << std::endl
		<< "\t\t\tpackage without generating the SISregistry entry\n"
		<< "\tsucert\t\tTo indicate that the SIS file has been signed with a SU certificate\n\n";
    }

void CCommandParser::DisplayVersion()
	{
	std::cout << "\nINTERPRETSIS  " << " Version  3.1.0 ." << std::endl;
	std::cout << "Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.\n " << std::endl;
	}
 

CParameterList* CCommandParser::ParseOptions(int argc, const char**argv)
    {
	CParameterList* paramList = new CParameterList();
	bool bAbort = ParseParam(argc, argv, paramList);
	if(bAbort)
		{
		delete paramList;
		paramList = NULL;
		}
	return paramList;
    }
bool CCommandParser::ParseParam(int argc, const char**argv, CParameterList* aParamList)
	{
	if (argc < 2)
	    {
		throw CCommandParser::ECmdLineUnknownOption;
	    }
	
	bool bContinue = true;
	
	while ((--argc > 0) && bContinue)
	    {
		++argv;
		if (**argv != '-')
			{
			continue;
            }

		bool err = false;
		const char* optPtr = *argv;

		switch (toupper(*++optPtr))
		    {
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			case 'A':
				{
				if (argc <= 1)
					throw CCommandParser::ECmdLineNoDirArgument;
				
				--argc;
				aParamList->SetResourceFilePath(*(++argv));
				break;	
				}
			#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			case 'C':
				{
				if (argc <= 1)
					throw CCommandParser::ECmdLineNoDirArgument;
				
				--argc;
				aParamList->SetSystemDrive(*(++argv));
				break;
				
				}
			case 'D':
				{	
				--argc;
				wchar_t buf[2048];
				ConvertMultiByteToWideChar(*++argv,-1, buf, 2048);
				aParamList->SetSystemDriveLetter(tolower(buf[0]));
				break;
				}					
			case 'E':
				{
				aParamList->SetFlag(CParameterList::EFlagsDisableZDriveChecksSet);
				break;
				}
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			case 'F':
				{				
				aParamList->SetOriginVerificationStatus(false);
				break;
				}
			#endif
			case 'H':
			case '?':
				{
				DisplayUsage();
				bContinue = false;
				break;
				}
			case 'I':
				{
				if (argc <= 1)
					throw CCommandParser::ECmdLineNoConfigFileArgument;
	
				--argc;
				aParamList->SetConfigFile(*(++argv));
				break;
				}
			case 'K':
				{
				// Last command
				if (argc == 1)
					{
					aParamList->SetSisRegistryVersion(SisRegistry::KRegistryV40string);
					}
				else
					{
					// Check that the next arg is valid
					std::string versionStr = *(++argv);

					if (**argv == '-')
						{
						aParamList->SetSisRegistryVersion(SisRegistry::KRegistryV40string);
						--argv;
						}
					else
						{
						--argc;
						aParamList->SetSisRegistryVersion(versionStr);
						}
					}
				break;
				}
			case 'L':
				{
				if (argc > 1)
					{
					--argc;
					wchar_t buf[2048];
					ConvertMultiByteToWideChar(*++argv,-1, buf, 2048);
					iLogFile = buf;
					}
				break;
				}
			case 'N':
				{
				if (argc <= 1)
					throw CCommandParser::ECmdLineInvalidLanguage;

				--argc;
				aParamList->SetLanguage(static_cast< CSISLanguage::TLanguage >( String2Language( *(++argv) ) ));
				break;
				}
			case 'O':
				{
				DisplayOpenSSLCopyright();
				bContinue = false;
				break;
				}
			case 'P':
				{
				if (!(err = argc <= 1))
					{
					--argc;
					std::string paramFile = *(++argv);
					ParseParam(argc, paramFile, aParamList);
					}
				break;
				}
			case 'R':
				{
				if (argc <= 1)
					throw CCommandParser::ECmdLineNoRomRofsLogsArgument;

				--argc;
				aParamList->EmptyRomLogFiles();
#ifdef _MSC_VER
				FilePtr funtionPtr = aParamList->AddRomLogFile;
#else
				FilePtr funtionPtr = &(aParamList->AddRomLogFile);
#endif // _MSC_VER	
				String2List(*aParamList, funtionPtr, *(++argv) );
				break;
				}
			case 'S':
				{
				if (argc <= 1)
					throw CCommandParser::ECmdLineNoSisArgument;

				--argc;

				String2SISFileList(*aParamList, argc, ++argv);
				break;
				}
			case 'T':
				{
				if (argc <= 1)
					throw CCommandParser::ECmdLineNoSisStubPathArgument;

				--argc;
				aParamList->SetStubDir(*(++argv));
				break;
				}
			case 'V':
				{
				DisplayVersion();
				bContinue = false;
				break;
				}
			case 'W':
				{
				--argc;
				iWarnLevel = String2Warn(*(++argv));
				break;
				}
			case 'X':
				{
				if (argc <= 1)
					throw CCommandParser::ECmdLineMissingPackageUID;

				--argc;
				aParamList->EmptyPkgUids();
#ifdef _MSC_VER
				FilePtr funtionPtr = (aParamList->AddPkgUid);
#else
				FilePtr funtionPtr = &(aParamList->AddPkgUid);
#endif // _MSC_VER	
				String2List(*aParamList, funtionPtr, *(++argv) );
				break;
				}
			case 'Z':
				{
				if (argc <= 1)
					throw CCommandParser::ECmdLineNoDirArgument;
			
				--argc;
				aParamList->SetZDrive(*(++argv));
				break;
				}
			default:
				{
				LERROR(string2wstring(std::string(optPtr)));
				throw CCommandParser::ECmdLineUnknownOption;
				}
			}

		if (err)
			{
			throw CCommandParser::ECmdLineUnknownOption;
			}
		} // Finished parsing all the command line options
	
	return !bContinue; 
    }


void CCommandParser::ParseParam(int argc, const std::string& paramFile, CParameterList* aParamList)
    {
	if (paramFile.size() > 0)
	    {
		std::ifstream params;
		params.open(paramFile.c_str(), std::ios::in);

		if (!params.good())
            {
			throw CCommandParser::ECmdLineMissingParamFile;
            }

		std::vector<std::string> tokens;
		argc = 0;
		while (params.good())
		    {
			std::string token;
			params >> token;
			if(token != "")
				{
				argc++;
				tokens.push_back(token);
				}
		    }

		const char** newArgv = new const char*[tokens.size()+1];
		for (int i = 0 ; i < tokens.size() ; ++i)
            {
			newArgv[i+1] = tokens[i].c_str();
            }

        ParseParam(argc+1, newArgv, aParamList);
		delete [] newArgv;
        params.close();
	    }
    }

void CCommandParser::String2List(CParameterList& aParamList, FilePtr& aFilePtr, const std::string& aOptionsString)
    {
	std::string::const_iterator it = aOptionsString.begin();
	std::string::const_iterator end = aOptionsString.end();
	std::string::const_iterator currentPos = it;
    //
	while (currentPos != end)
	    {
		currentPos = std::find(it, end, ',');
		std::wstring x = string2wstring(std::string(it,(currentPos-it)));
		//aOptionsList.push_back(x);
		(aParamList.*aFilePtr)(x);

		if (currentPos == end)
			{
			return;
			}

		it = currentPos;
		++it;
	    }
    }

void CCommandParser::String2SISFileList(CParameterList& aParamList, int aArgc, const char**aArgv)
    {
	std::string sisFileOption(*aArgv);

	std::string::const_iterator it = sisFileOption.begin();
	std::string::const_iterator end = sisFileOption.end();
	std::string::const_iterator currentPos = it;

	currentPos = std::find(it, end, ',');

	while (currentPos != end)
	    {
		currentPos = std::find(it, end, ',');
		std::wstring x = string2wstring(std::string(it,(currentPos-it)));

		InstallSISFile sisFileName(x, '$', false);
		aParamList.AddSISFile(sisFileName);

		if (currentPos == end)
			{
			return;
			}

		it = currentPos;
		++it;
	    }
	
	// At this stage, aArgv could be something like: file.sis +e +mcard +sucert
	// As the sisfile attributes are predetermined input, therefore we can just
	// parse according to the predetermined inputs.

	std::wstring fileName = string2wstring(sisFileOption);

	InstallSISFile sisFileName(fileName, '$', false);
		
	while (--aArgc > 0)
		{ // Process associated SIS file attributes
		++aArgv;

		if (**aArgv == '-')
			{ // No more attribute to process
			aParamList.AddSISFile(sisFileName);

			--aArgv;
			++aArgc;
			return;
			}

		if (**aArgv == '+')
			{
			std::string versionStr;

			// Check that the next arg is valid
			versionStr = *(aArgv);
			
			if (versionStr.size() == 2)
				{
				// Found the associated SIS file drive; e.g. +e
				const char* optPtr = *aArgv;

				char drive = tolower(*++optPtr);

				if (drive < 'a' || drive > 'y')
					throw CCommandParser::ECmdLineInvalidSISFileAttribute;

				// Set target drive
				sisFileName.iTargetDrive = drive;

				continue;
				}

			versionStr = StringUtils::ToUpper( versionStr );

			if (versionStr == "+MCARD")
				{
				sisFileName.iGenerateStub = true;
				}
			else if (versionStr == "+MCARDNR")
				{
				sisFileName.iGenerateStub = true;
				sisFileName.iReadOnly = true;
				}
			else if (versionStr == "+MCARDALONE")
				{
				sisFileName.iGenerateStub = true;
				sisFileName.iNotRegister = true;
				}
			else if (versionStr == "+MCARDALONENR")
				{
				sisFileName.iGenerateStub = true;
				sisFileName.iNotRegister = true;
				sisFileName.iReadOnly = true;
				}
			else if (versionStr == "+SUCERT")
				{
				sisFileName.iSUFlag = true;
				}
			else if (versionStr == "+NONREMOVABLEPKG")
				{
				sisFileName.iNonRemovable = true;
				}
			else
				throw CCommandParser::ECmdLineInvalidSISFileAttribute;
			}
			else
			{
			throw CCommandParser::ECmdLineInvalidSISFileAttribute;
			}
		}
	aParamList.AddSISFile(sisFileName);
	}


WarnLevel CCommandParser::String2Warn(const std::string& level)
    {
	std::string l(level);
	std::transform(l.begin(),l.end(),l.begin(), toupper);
    //
	if (KString2Warning.size() == 0)
	    {
		KString2Warning["OFF"] = WARN_OFF;
		KString2Warning["WARN"] = WARN_WARN;
		KString2Warning["ERROR"] = WARN_ERROR;
		KString2Warning["INFO"] = WARN_INFO;
	    }
    //
	WarnMap::const_iterator result = KString2Warning.find(l);
	if (result == KString2Warning.end())
	    {
		throw CCommandParser::ECmdLineUnknownOption;
	    }
    //
	return result->second;
    }


int CCommandParser::String2Language( const std::string& aLanguage )
    {
	std::istringstream stringStream( aLanguage );
    int language = 1;
    //
    if ( stringStream >> language )
        {
        }
    else
        {
        throw CCommandParser::ECmdLineInvalidLanguage;
        }
    //
    return language;
    }
