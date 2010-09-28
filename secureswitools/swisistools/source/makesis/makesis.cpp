// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// makesis main function
// INCLUDES
// 
//

/**
 @file
 @internalComponent
 @released
*/
#include <iostream>
#include <new>
#include <fstream>


#include "utils.h"
#include "utility_interface.h"
#include "openssllicense.h"
#include "makesis.h"

// ===========================================================================
// CONSTANTS
// ===========================================================================

#ifndef _UNICODE
#error _UNICODE expected
#endif // _UNICODE

// ===========================================================================
// CMakeSIS
// The main application object. Controls the SIS file generation process
// ===========================================================================

void CMakeSIS::ShowBanner()
// Displays the copyright...
	{
	std::wcout << std::endl << L"MAKESIS  " << L" Version  5.3.0.0" << std::endl;
	std::wcout << L"A utility for creating Software Installation (SIS) files" << std::endl;
	std::wcout << L"Copyright (c) 2000-2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. " << std::endl;
#ifdef _DEBUG
	std::wcout << std::endl << L"Development Version" << std::endl;
#endif
	std::wcout << std::endl;
	}


void CMakeSIS::DoMsg(const wchar_t* aText1, const wchar_t* aText2) const
// Purpose  : Displays 'verbose' output text - if appropriate
// Inputs   : pszText - The text to display
	{
	if(iLineNo > 0)
	{
		char fileName[PATHMAX] = "\0";
		ConvertWideCharToMultiByte(iCmdOptions.SourceFile() , -1 , fileName , PATHMAX);
		unsigned long bytesWritten;
		void *hndl = GetStdHandle(STD_OUTPUT_HANDLE);
		WriteConsole(hndl,iCmdOptions.SourceFile(),wcslen(iCmdOptions.SourceFile()),&bytesWritten,0);
		std::wcout << L"("  << iLineNo << L")" << L" : ";
		}

	std::wcout << aText1 << aText2 << std::endl;
	}

void CMakeSIS::DoMsg(const wchar_t* aText) const
	{
	std::wcout << aText << std::endl;
	}

void CMakeSIS::DoErrMsg(const wchar_t* aText) const
	{
	DoMsg(L"error: ", aText);
	}

void CMakeSIS::DoVerbage(const wchar_t* aText) const
// Purpose  : Displays 'verbose' output text - if appropriate
// Inputs   : pszText - The text to display
	{
	if(Verbose())
		DoMsg(L"", aText);
	}

void CMakeSIS::ShowUtilsError(TUtilsException aError)
// Purpose  : Write message for any error which occured whilst parsing the input pkg file
// Inputs   : err - the error ID
	{
	switch(aError)
		{
		case ErrNotEnoughMemory:
			DoErrMsg(L"not enough memory");
			break;
		case ErrCannotOpenFile:
			DoErrMsg(L"cannot open file, check filename and access rights");
			break;
		case ErrCannotReadFile:
			DoErrMsg(L"cannot read file, check access rights");
			break;
		case ErrCannotWriteFile:
			DoErrMsg(L"cannot write file, check disk space");
			break;
		case ErrCannotConvertFile:
			DoErrMsg(L"cannot convert file to unicode");
			DoErrMsg(L"make sure .PKG and .TXT files are either UTF8 or UNICODE");
			break;
		case ErrCertFileKeyFileMismatch:
			DoErrMsg(L"key file does not match certificate(s) given");
			DoErrMsg(L"make sure key and certificate(s) specified correspond");
			break;
		}
	}

void CMakeSIS::AddErrMsg(const wchar_t* aText)
	{
	iInterpretSisReport.push_back(InterpretSisError(aText, iLineNo));
	}

void CMakeSIS::AddInterpretSisError(TInterpretSisException aError)
// Purpose  : Write message for any error which occured whilst parsing the input pkg file
// Inputs   : err - the error ID
	{
	switch(aError)
		{
		case ErrInvalidAppType:
			AddErrMsg(L"Invalid Application Type. Package type PP not supported");
			break;
		case ErrEmbeddedSisNotSupported:
			AddErrMsg(L"Embedded SIS file will not be installed by InterpretSis");
			break;
		case ErrUserOptionsNotSupported:
			AddErrMsg(L"User options are not supported");
			break;
		default:
			AddErrMsg(L"General error");
			break;
		}
	}


void CMakeSIS::ShowParseError(TParseException aError)
// Purpose  : Write message for any error which occured whilst parsing the input pkg file
// Inputs   : err - the error ID
	{
	switch(aError)
		{
		case ErrUnknownLine:
			DoErrMsg(L"unknown line");
			break;
		case ErrHeaderAlreadyDefined:
			DoErrMsg(L"installation header already found");
			break;
		case ErrUnknownLanguagesId:
			DoErrMsg(L"unknown language specified");
			std::wcout << L"  Usage : &aa[(dddd)],bb,...zz" << std::endl;
			std::wcout << L"          AF - Afrikaans, SQ - Albanian, AH - Amharic" << std::endl;
			std::wcout << L"          AR - Arabic, HY - Armenian, AU - Australian" << std::endl;
			std::wcout << L"          AS - Austrian, BA - Basque, BE - Belarussian, BN - Bengali"<< std::endl;
			std::wcout << L"          BG - Bulgarian, MY - Burmese, CA - Catalan" << std::endl;
			std::wcout << L"          TC - Taiwan Chinese, HK - Hong Kong Chinese"<< std::endl;
			std::wcout << L"          ZH - PRC Chinese"<< std::endl;
			std::wcout << L"          HR - Croatian, CS - Czech, DA - Danish, DU - Dutch" << std::endl;
			std::wcout << L"          EN - English, AM - American English, CE - Canadian English"<< std::endl;
			std::wcout << L"          IE - International English, SA - SOUTh African English" << std::endl;
			std::wcout << L"          ET - Estonian, FA - Farsi, FI - Finnish, BL - Belgian Flemish" << std::endl;
			std::wcout << L"          FR - French, BF - Belgian French,  CF - Canadian French"<< std::endl;
			std::wcout << L"          IF - International French, SF - Swiss French"<< std::endl;
			std::wcout << L"          GD - Scots Gaelic, GL - Galician, KA - Georgian, GE - German" << std::endl;
			std::wcout << L"          SG - Swiss German, EL - Greek, GU - Gujarati"<< std::endl;
			std::wcout << L"          HE - Hebrew, HI - Hindi, HU - Hungarian" << std::endl;
			std::wcout << L"          IC - Icelandic, IN - Indonesian, GA - Irish"<< std::endl;
			std::wcout << L"          IT - Italian, SZ - Swiss Italian, JA - Japanese, " << std::endl;
			std::wcout << L"          KN - Kannada, KK - Kazakh, KM - Khmer"<< std::endl;
			std::wcout << L"          KO - Korean, LO - Laothian, LV - Latvian" << std::endl;
			std::wcout << L"          LT - Lithuanian, MK - Macedonian, MS - Malay" << std::endl;
			std::wcout << L"          ML - Malayalam, MR - Marathi, MO - Moldovian" << std::endl;
			std::wcout << L"          MN - Mongolian, NZ - New Zealand, NO - Norwegian" << std::endl;
			std::wcout << L"          NN - Norwegian Nynorsk, PL - Polish, PO - Portuguese"<< std::endl;
			std::wcout << L"          BP - Brazilian Portuguese, PA - Punjabi, RO - Romanian" << std::endl;
			std::wcout << L"          RU - Russian, SR - Serbian, SI - Sinhalese" << std::endl;
			std::wcout << L"          SK - Slovak, SL - Slovenian, SO - Somali"<< std::endl;
			std::wcout << L"          SP - Spanish, OS - International Spanish" << std::endl;
			std::wcout << L"          LS - Latin American Spanish"<< std::endl;
			std::wcout << L"          SH - Swahili, SW - Swedish, FS - Finland Swedish" << std::endl;
			std::wcout << L"          TL - Tagalog, TA - Tamil, TE - Telugu, TH - Thai" << std::endl;
			std::wcout << L"          BO - Tibetan, TI - Tigrinya, TU - Turkish"<< std::endl;
			std::wcout << L"          CT - Cyprus Turkish, TK - Turkmen, UK - Ukrainian" << std::endl;
			std::wcout << L"          UR - Urdu" << std::endl;
			//Put reserved here
			std::wcout << L"          VI - Vietnamese, CY - Welsh, ZU - Zulu" << std::endl;
			std::wcout << std::endl
				<< L"          (dddd) - optional dialect ID" << std::endl;
			break;
		case ErrLanguagesAlreadyDefined:
			DoErrMsg(L"the languages have already been defined");
			break;
		case ErrLanguagesNotDefined:
			DoErrMsg(L"languages have not been defined yet");
			break;
		case ErrHeaderNotDefined:
			DoErrMsg(L"the installation header has not been defined");
			break;
		case ErrNoMainDependency:
			DoErrMsg(L"missing main component dependency");
			std::wcout << L"the header UID should equal the UID of the main component and dependency" << std::endl;
			break;
		case ErrFileNotFound:
			DoErrMsg(L"file not found");
			break;
		case ErrPackageNotASISFile:
			DoErrMsg(L"component is not a SIS file");
			break;
		case ErrBadCondFormat:
			DoErrMsg(L"incorrect condition format");
			std::wcout << L"  Usage : IF condition" << std::endl;
			std::wcout << L"          ELSEIF condition" << std::endl;
			std::wcout << L"          ELSE" << std::endl;
			std::wcout << L"          ENDIF" << std::endl << std::endl;
			std::wcout << L"  condition : var op \"string\"" << std::endl;
			std::wcout << L"              var op number" << std::endl;
			std::wcout << L"              EXISTS \"filename\"" << std::endl;
			std::wcout << L"              (condition)AND(condition)" << std::endl;
			std::wcout << L"              (condition)OR(condition)" << std::endl;
			std::wcout << L"              NOT(condition)" << std::endl << std::endl;
			std::wcout << L"  op :  =  <>  >  <  >=  <=" << std::endl;
			break;
		case ErrReadFailed:
			DoErrMsg(L"failed to read file");
			break;
		case ErrBadOption:
			DoErrMsg(L"invalid option");
			break;
		case ErrBadString:
			DoErrMsg(L"invalid string");
			break;
		case ErrUnexpectedToken:
			DoErrMsg(L"unexpected text");
			break;
		case ErrInvalidEscape:
			DoErrMsg(L"invalid UNICODE character value");
			break;
		case ErrNumberOutOfRange:
			DoErrMsg(L"number std::wcout of range");
			break;
		case ErrUnknownVariable:
			DoErrMsg(L"unknown variable name");
			std::wcout << L"  Known variables :" << std::endl;
			std::wcout << L"    Manufacturer, ManufacturerHardwareRev, ManufacturerSoftwareRev," << std::endl;
			std::wcout << L"      ManufacturerSoftwareBuild" << std::endl;
    		std::wcout << L"    Model" << std::endl;
			std::wcout << L"    MachineUid" << std::endl;
			std::wcout << L"    DeviceFamily, DeviceFamilyRev" << std::endl;
			std::wcout << L"    CPU, CPUArch, CPUABI, CPUSpeed" << std::endl;
			std::wcout << L"    SystemTickPeriod" << std::endl;
			std::wcout << L"    MemoryRAM, MemoryRAMFree, MemoryROM, MemoryPageSize" << std::endl;
			std::wcout << L"    PowerBackup" << std::endl;
			std::wcout << L"    Keyboard, KeyboardDeviceKeys, KeyboardAppKeys, KeyboardClick," << std::endl;
			std::wcout << L"      KeyboardClickVolumeMax" << std::endl;
			std::wcout << L"    DisplayXPixels, DisplayYPixels, DisplayXTwips, DisplayYTwips," << std::endl;
			std::wcout << L"      DisplayColors, DisplayContrastMax" << std::endl;
			std::wcout << L"    Backlight" << std::endl;
			std::wcout << L"    Pen, PenX, PenY, PenDisplayOn, PenClick, PenClickVolumeMax" << std::endl;
			std::wcout << L"    Mouse, MouseX, MouseY, MouseButtons" << std::endl;
			std::wcout << L"    CaseSwitch" << std::endl;
			std::wcout << L"    LEDs" << std::endl;
			std::wcout << L"    IntegratedPhone" << std::endl;
			std::wcout << L"    NumHalAttributes" << std::endl;
			std::wcout << L"    Language" << std::endl;
			std::wcout << L"    RemoteInstall" << std::endl;
			break;
		case ErrUninstallNeedsWaitEnd:
			DoErrMsg(L"RW/RUNWAITEND option required with RR/RUNREMOVE or RB/RUNBOTH options");
			break;
		case ErrUIDMismatch:
			DoErrMsg(L"UID in the embedded SIS file is not the same as the UID given in the package file.");
			break;
		case ErrCannotGetFullPath:
			DoErrMsg(L"Cannot get full path of embedded SIS file.");
			break;
		case ErrBadDestinationPath:
			DoErrMsg(L"Bad destination path for file.");
			break;
		case ErrDuplicateUID:
			DoErrMsg(L"UID in the embedded SIS file is the same as the UID of the main component.");
			break;
		case ErrExceedVersionNumber:
			DoErrMsg(L"The version numbers cannot be more than 32767.");
			break;
		case ErrUnBalancedParenthesis:
			DoErrMsg(L"parenthesis are not balanced");
			break;
		case ErrHeaderRejectPreInstalledNonRemovable:
			DoErrMsg(L"Pre-installed packages can not be marked as non-removable.");
			break;
		case ErrInvalidInstallFlagOption:
			DoErrMsg(L"Install flag option is not supported with the given install type.");
			break;
		case ErrVersionInvalidRelationalOperator:
			DoErrMsg(L"Invalid Relational Operator within VERSION() condition");
			ShowVersionFunctionSyntax();
			break;
		case ErrVersionWildcardsNotSupported:
			DoErrMsg(L"Wildcards are not supported within VERSION() conditions");
			ShowVersionFunctionSyntax();
			break;
		case ErrVersionNegativesNotSupported:
			DoErrMsg(L"Negative version components are not supported within VERSION() conditions");
			ShowVersionFunctionSyntax();
			break;
		case ErrBadIntegerToStringConversion:
			DoErrMsg(L"Failed to successfully convert integer value to string");
			break;
		default:
			DoErrMsg(L"syntax error");
			break;
		}
	}
void CMakeSIS::ShowCommandLineError(TCommandLineException aError)
// Purpose  : Write message for any error which occured whilst processing the command line (to
//            STDOUT )
// Inputs   : err - the error ID
	{
	switch(aError)
		{
		case ErrInsufficientArgs:
			DoErrMsg(L"wrong number of arguments");
			break;
		case ErrBadCommandFlag:
			DoErrMsg(L"unknown flag");
			break;
		case ErrBadSourceFile:
			DoErrMsg(L"invalid source file");
			break;
		case ErrNoSourceFile:
			DoErrMsg(L"no source file specified");
			break;
		case ErrCannotOpenSourceFile:
			DoErrMsg(L"cannot open source file");
			break;
		case  ErrBadTargetFile:
			DoErrMsg(L"invalid destination file");
			break;
		default:
			DoErrMsg(L"bad command line argumants");
			break;
		}
	}

void CMakeSIS::ShowUsage()
	{
	std::wcout << L"Usage : MakeSIS [-h] [-i] [-s] [-v] [-c] [-d directory] pkgfile [sisfile]" << std::endl << std::endl;
	std::wcout << L"  Options : [-h] Show help page and PKG file format" << std::endl;
	std::wcout << L"  Options : [-i] Show Open SSL licence" << std::endl;
	std::wcout << L"  Options : [-s] Generate a ROM Stub" << std::endl;
	std::wcout << L"  Options : [-v] Verbose output to screen" << std::endl;
	std::wcout << L"  Options : [-c] Reports errors where the SIS file being generated will not be installable by InterpretSis" << std::endl;
	std::wcout << L"  Options : [-d directory] Directory path to search" << std::endl;
	std::wcout << L"  Options : [pkgfile] Name of the PKG file" << std::endl;
	std::wcout << L"  Options : [sisfile] Output SIS filename. If not specified, the filename will be derived from the name of the PKG file" << std::endl;
	}

void CMakeSIS::ShowSSL()
	{
	for (int index = 0; index < (sizeof(openSSLLicenseString)/sizeof(openSSLLicenseString[0])); index++)
		std::cout << openSSLLicenseString [index] << std::endl;
	}

void CMakeSIS::ShowSyntax()
// Writes a PKG syntax summary to STDOUT 
	{
	// Show the title
	ShowBanner();
	std::wcout << L"MakeSIS PKG File format help" << std::endl;
	std::wcout << L"============================" << std::endl << std::endl;
	std::wcout << L"&aa[(dddd)],bb,...,zz    Define languages used in installation. Choose from:-" << std::endl;
	std::wcout << L"          AF - Afrikaans, SQ - Albanian, AH - Amharic" << std::endl;
	std::wcout << L"          AR - Arabic, HY - Armenian, AU - Australian" << std::endl;
	std::wcout << L"          AS - Austrian, BA - Basque, BE - Belarussian, BN - Bengali"<< std::endl;
	std::wcout << L"          BG - Bulgarian, MY - Burmese, CA - Catalan" << std::endl;
	std::wcout << L"          TC - Taiwan Chinese, HK - Hong Kong Chinese"<< std::endl;
	std::wcout << L"          ZH - PRC Chinese"<< std::endl;
	std::wcout << L"          HR - Croatian, CS - Czech, DA - Danish, DU - Dutch" << std::endl;
	std::wcout << L"          EN - English, AM - American English, CE - Canadian English"<< std::endl;
	std::wcout << L"          IE - International English, SA - South African English" << std::endl;
	std::wcout << L"          ET - Estonian, FA - Farsi, FI - Finnish, BL - Belgian Flemish" << std::endl;
	std::wcout << L"          FR - French, BF - Belgian French,  CF - Canadian French"<< std::endl;
	std::wcout << L"          IF - International French, SF - Swiss French"<< std::endl;
	std::wcout << L"          GD - Scots Gaelic, GL - Galician, KA - Georgian, GE - German" << std::endl;
	std::wcout << L"          SG - Swiss German, EL - Greek, GU - Gujarati"<< std::endl;
	std::wcout << L"          HE - Hebrew, HI - Hindi, HU - Hungarian" << std::endl;
	std::wcout << L"          IC - Icelandic, IN - Indonesian, GA - Irish"<< std::endl;
	std::wcout << L"          IT - Italian, SZ - Swiss Italian, JA - Japanese, " << std::endl;
	std::wcout << L"          KN - Kannada, KK - Kazakh, KM - Khmer"<< std::endl;
	std::wcout << L"          KO - Korean, LO - Laothian, LV - Latvian" << std::endl;
	std::wcout << L"          LT - Lithuanian, MK - Macedonian, MS - Malay" << std::endl;
	std::wcout << L"          ML - Malayalam, MR - Marathi, MO - Moldovian" << std::endl;
	std::wcout << L"          MN - Mongolian, NZ - New Zealand, NO - Norwegian" << std::endl;
	std::wcout << L"          NN - Norwegian Nynorsk, PL - Polish, PO - Portuguese"<< std::endl;
	std::wcout << L"          BP - Brazilian Portuguese, PA - Punjabi, RO - Romanian" << std::endl;
	std::wcout << L"          RU - Russian, SR - Serbian, SI - Sinhalese" << std::endl;
	std::wcout << L"          SK - Slovak, SL - Slovenian, SO - Somali"<< std::endl;
	std::wcout << L"          SP - Spanish, OS - International Spanish" << std::endl;
	std::wcout << L"          LS - Latin American Spanish"<< std::endl;
	std::wcout << L"          SH - Swahili, SW - Swedish, FS - Finland Swedish" << std::endl;
	std::wcout << L"          TL - Tagalog, TA - Tamil, TE - Telugu, TH - Thai" << std::endl;
	std::wcout << L"          BO - Tibetan, TI - Tigrinya, TU - Turkish"<< std::endl;
	std::wcout << L"          CT - Cyprus Turkish, TK - Turkmen, UK - Ukrainian" << std::endl;
	std::wcout << L"          UR - Urdu" << std::endl;
			//Put reserved here
	std::wcout << L"          VI - Vietnamese, CY - Welsh, ZU - Zulu" << std::endl;
	std::wcout << std::endl
		<< L"          (dddd) - optional dialect ID" << std::endl << std::endl;
	std::wcout << L"#{\"NAMEaa\", ... \"NAMEzz\"},(UID), Major, Minor, Build, Options" << std::endl;
	std::wcout << L"                 Define file header, including name, uid and version" << std::endl;
	std::wcout << L"%{\"Vendor-EN\", ... \"Vendor-FR\"}" << std::endl;
	std::wcout << L"                 Localised vendor names" << std::endl;
	std::wcout << L":\"Nokia Software Ltd\"" << std::endl;
	std::wcout << L"                 The non-localised vendor name" << std::endl;
	std::wcout << L"=\"logo.jpg\",\"image/jpeg\",\"target.jpg\"" << std::endl;
	std::wcout << L"                 Define the logo using file, mimetype, and optional target" << std::endl;
	std::wcout << L"\"Source\"-\"Destination\",Options" << std::endl;
	std::wcout << L"                 File to be installed/viewed" << std::endl;
	std::wcout << L"@\"Component\",(UID)" << std::endl;
	std::wcout << L"                 Sub-component with UID to be installed" << std::endl;
	std::wcout << L"(UID),<versionrange>,{\"DEPENDaa\", ... \"DEPENDzz\"}" << std::endl;
	std::wcout << L"                 Define a dependency on another component version" << std::endl;
	std::wcout << L"[UID],<versionrange>,{\"DEPENDaa\", ... \"DEPENDzz\"}" << std::endl;
	std::wcout << L"                 Define a dependency on target hardware" << std::endl;
	std::wcout << L"Where <versionrange> is defined as:" << std::endl;
	std::wcout << L"                 Minimally, a single version of the form number major,minor,build" << std::endl;
	std::wcout << L"                 optionally, you may specify a range of versions in the form" << std::endl;
	std::wcout << L"                 major,minor,build~major,minor,build" << std::endl;
	std::wcout << L"                 Versions may include the wildcard '*' or -1" << std::endl;
	std::wcout << L"{                Start of a language block" << std::endl;
	std::wcout << L"\"Source\"         A file to install within a language block" << std::endl;
	std::wcout << L"@\"Component\"     A component to install within a language block" << std::endl;
	std::wcout << L"}-\"Destination\", Options" << std::endl;
	std::wcout << L"                 End a language block containing files" << std::endl;
	std::wcout << L"},(UID)          End a language block containing components" << std::endl;
	std::wcout << L"IF condition     Start of block which is installed if condition evaluates" << std::endl;
	std::wcout << L"                 to TRUE at install time. Condition format is of the form:-" << std::endl;
	std::wcout << L"                 condition : variable op number" << std::endl;
	std::wcout << L"                             EXISTS \"filename\"" << std::endl;
	std::wcout << L"                             (condition)AND(condition)" << std::endl;
	std::wcout << L"                             (condition)OR(condition)" << std::endl;
	std::wcout << L"                             NOT(condition)" << std::endl;
	std::wcout << L"                 op :  =  <>  >  <  >=  <=" << std::endl;
	std::wcout << L"ELSEIF condition Block which is installed if a previous condition block" << std::endl;
	std::wcout << L"                 was not evaluated and condition is TRUE" << std::endl;
	std::wcout << L"ELSE             Block which is installed if no previous condition blocks" << std::endl;
	std::wcout << L"                 were evaluated" << std::endl;
	std::wcout << L"ENDIF            Marks end of condition block(s)" << std::endl;
	}



void CMakeSIS::DisplayInterpretSisReport() const
	{
	char fileName[PATHMAX] = "\0";
	ConvertWideCharToMultiByte(iCmdOptions.SourceFile() , -1 , fileName , PATHMAX);
	unsigned long bytesWritten;
	void *hndl = GetStdHandle(STD_OUTPUT_HANDLE);
	for (InterpretSisErrors::const_iterator curr = iInterpretSisReport.begin() ;
         curr != iInterpretSisReport.end();
		 ++curr)
		{
		WriteConsole(hndl,iCmdOptions.SourceFile(),wcslen(iCmdOptions.SourceFile()),&bytesWritten,0);
		std::wcout << L"(" << curr->second << L")" << L" : " << curr->first << std::endl;
		}
	}

void CMakeSIS::ShowVersionFunctionSyntax() const
// Writes the correct usage of the VERSION() condition syntax to the command line 	
	{
	std::wcout << std::endl;
	std::wcout << L"Usage :		VERSION( <Package Uid> , <Operator> , <vMajor> , <vMinor> , <vBuild> )" << std::endl << std::endl;
	std::wcout << L"<Package Uid> :	UID of the installed package which is being queried in Hexadecimal" << std::endl;
	std::wcout << L"<Operator> :	Relational comparison operator ( =  <>  >  >=  <  <= )" << std::endl;
	std::wcout << L"<vMajor> :	Major component of the version (Range 0 - 127)" << std::endl;
	std::wcout << L"<vMinor> :	Minor component of the version (Range 0 - 99)" << std::endl;
	std::wcout << L"<vBuild> :	Build component of the version (Range 0 - 32767)" << std::endl;
	}


CMakeSIS::CMakeSIS() :
			iLineNo(0),
			iDump(false),
			iVerbose(false)
	{}

int CMakeSIS::RunL(int argc, wchar_t* argv[])
// Inputs   : argc, argv, envp - The command line passed to the process
	{
	int err=1;
	
	try
		{
		iCmdOptions.ParseCommandLine(argc, argv);
		bool bAbort = false;
		if(iCmdOptions.ShowSSL ())
			{
			ShowSSL ();
			bAbort = true;
			}
		if(iCmdOptions.ShowSyntax ())
			{
			ShowSyntax();
			bAbort = true;
			}
		if (bAbort)
			{
			return 0;
			}
		// We now have valid parameters
		iVerbose = iCmdOptions.Flags() & CParseCmd::EOptVerbose;
		iDump = (iCmdOptions.Flags() & CParseCmd::EOptDump) != 0;

		if (iCmdOptions.Flags() & CParseCmd::EOptPassword)
			{
			std::wcout << L"Warning: MakeSIS no longer signs. Use SignSIS." << std::endl;
			}

		std::wcout << L"Processing " << iCmdOptions.SourceFile() << L"..." << std::endl;

		SISLogger::SetStream(std::wcout);
		CPackageParser pkgParser(*this);

		if(iCmdOptions.Flags() & CParseCmd::EOptDirectory)
			{
			pkgParser.SetSearchDirectory(iCmdOptions.SearchPath());
			}
		
		
		bool isStubFile= (iCmdOptions.Flags() & CParseCmd::EOptMakeStub) != 0;
		pkgParser.MakeSisL(iCmdOptions.SourceFile(), iCmdOptions.TargetFile(), isStubFile, iCmdOptions.InterpretSisReport());
		err = 0;

		if (iCmdOptions.InterpretSisReport())
			{
			if ( iInterpretSisReport.empty() )
				{
				std::wcout << L"*** SIS installation file VALID for InterpretSis ***" << std::endl;
				}
			else
				{
				std::wcout << L"*** SIS installation file INVALID for InterpretSis ***" << std::endl;
				DisplayInterpretSisReport();
				err = 1;
				}
			}
#if defined (_DEBUG)
		if (Dump ())
			pkgParser.Dump (std::cout);
#endif // _DEBUG	

		std::wcout << L"Created  " << iCmdOptions.TargetFile() <<  L".";
		}
	catch(TCommandLineException err)
		{
		// Show the title
		ShowBanner();
		ShowCommandLineError(err);
		ShowUsage();
		}
	catch(CSISException oops)
		{
		DoErrMsg (oops.widewhat ());
		err = 1;
		}
	catch(TParseException x)
		{
		ShowParseError(x);
		}
	catch(TUtilsException err)
		{
		ShowUtilsError(err);
		}
	return err;

	}

// main function - constructs a CMakeSIS object and calls it's run method
int main(int argc, char *argv[])
	{
/*
MinGW doesn't support wide main() and wide character stuff also
But CommandLineArgs works as a wide main for MinGW and STLPort
is used for wide characters stuff.
*/
	wchar_t **argv1	= CommandLineArgs(argc,argv);
	CMakeSIS app;
	int reply = app.RunL(argc, argv1);
	cleanup(argc,argv1);
	return reply;
	}

