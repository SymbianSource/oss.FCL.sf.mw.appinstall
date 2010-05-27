/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* handles parsing of PKG file
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
* INCLUDES
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif 

#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include "packageparser.h"
#include "utility_interface.h"
#include "version.h"
#include "utils.h"


#define EOF_TOKEN			 0
#define NUMERIC_TOKEN		 1
#define ALPHA_TOKEN			 2
#define QUOTED_STRING_TOKEN	 3
#define AND_TOKEN			 4
#define OR_TOKEN			 5
#define NOT_TOKEN			 6
#define EXISTS_TOKEN		 7
#define DEVCAP_TOKEN		 8
#define APPCAP_TOKEN		 9
#define GE_TOKEN			 10
#define LE_TOKEN			 11
#define NE_TOKEN			 12
#define IF_TOKEN			 13
#define ELSEIF_TOKEN		 14
#define ELSE_TOKEN			 15
#define ENDIF_TOKEN			 16
#define TYPE_TOKEN			 17
#define KEY_TOKEN			 18
#define VERSION_TOKEN		 19
#define SUPPORTED_LANG_TOKEN 20
#define LAST_TOKEN			 20



TDuplicates CPackageParser::iSrcFiles;

// ===========================================================================
// CONSTANTS
// ===========================================================================

// Set up the cout stream so that we can use it with either narrow or wide
// chars at build time

#ifndef _UNICODE
#error _UNICODE expected
#endif // _UNICODE


// Parse options lookups
const SParseOpt KHeaderOptions[] =
	{
		{L"IU", EInstIsUnicode},
		{L"SH", EInstShutdownApps},
		{L"NC", EInstNoCompress},
		{L"IUNICODE",		EInstIsUnicode},
		{L"SHUTDOWNAPPS",	EInstShutdownApps},
		{L"NOCOMPRESS",		EInstNoCompress},
		{L"NR",				EInstNonRemovable},
		{L"NONREMOVABLE",	EInstNonRemovable},
		{L"RU",				EInstROMUpgrade},
 		{L"ROMUPGRADE",		EInstROMUpgrade},
 		{L"H",				EInstHide},
 		{L"HIDE",			EInstHide}

	};
#define NUMHEADEROPTIONS (sizeof(KHeaderOptions)/sizeof(SParseOpt))

// Parse options lookups
#define MAXTOKENLEN	30
struct SParseToken
	{
	WCHAR pszOpt[MAXTOKENLEN];
	DWORD dwOpt;
	};

const SParseToken KTokens[] =
	{
		{L"if",			IF_TOKEN},
		{L"elseif",		ELSEIF_TOKEN},
		{L"else",		ELSE_TOKEN},
		{L"endif",		ENDIF_TOKEN},
		{L"exists",		EXISTS_TOKEN},
		{L"devcap",		DEVCAP_TOKEN},
		{L"appcap",		APPCAP_TOKEN},
		{L"package",	DEVCAP_TOKEN},
		{L"appprop",	APPCAP_TOKEN},
		{L"not",		NOT_TOKEN},
		{L"and",		AND_TOKEN},
		{L"or",			OR_TOKEN},
		{L"type",		TYPE_TOKEN},
		{L"key",		KEY_TOKEN},
		{L"version",	VERSION_TOKEN},
		{L"supported_language",SUPPORTED_LANG_TOKEN},

	};
#define NUMPARSETOKENS (sizeof(KTokens)/sizeof(SParseToken))

// ===========================================================================
// CPackageParser
// ===========================================================================

CPackageParser::CPackageParser(MParserObserver& aObserver) :
				iValidSISFile(true),
				iMakeStub(false),
				iReportInterpretSisError(false),
				iEnoughForStub(false),
				iIsHeaderDefined(false),
				iUniqueVendorName (false),
				iLocalisedVendorNames (false),
				iPkgChar(0),
				iLineNo(0),	   		// The line we are currently on
				iCurrentLang(0),	// If we are in a lang/lang file block - which lang are we processing
				iToken(0),
				iFileHandle(0),
				iObserver(aObserver)
	{
	memset((void *)iSearchDir, 0x0000, PATHMAX * sizeof(wchar_t));
	}

void CPackageParser::SetSearchDirectory(LPCWSTR pszPath)
// Purpose  : Sets the search directory
// Inputs   : pszPath  - The directory path (including trailing backslash!)
	{
	wcsncpy(iSearchDir, pszPath, PATHMAX - 1);
	DWORD len=wcslen(iSearchDir);
	wchar_t *pBuffer = iSearchDir;
	wchar_t *pCurrent = pBuffer;

	while (pBuffer && *pBuffer && (pCurrent = wcschr(pBuffer,L'\\')) != NULL)
		{
		*pCurrent = L'/';
		pBuffer = pCurrent + 1;
		} 

	if(len>0 && iSearchDir[len - 1] != '/')
		{
		iSearchDir[len] = '/';
		iSearchDir[len+1] = '\0';
		}

	}

void CPackageParser::MakeSisL(const wchar_t* aPkgFileName, const wchar_t* aSisFileName, bool aMakeStub, bool aReportInterpretSisError)
	{
	iMakeStub = aMakeStub;
	iReportInterpretSisError = aReportInterpretSisError;
	ParseL(aPkgFileName);
	// Turn off line number output
	iObserver.SetLineNumber(0);
	
	if (iMakeStub)
		iObserver.DoVerbage(L"Generating SIS stub file...");
	else
		iObserver.DoVerbage(L"Generating SIS installation file...");
	
	if (iValidSISFile)
		{
		iSISXWriter.WriteSIS (aSisFileName);
		}
	}

HANDLE CPackageParser::OpenPackageFile(const wchar_t* aPkgFileName)
	{
	bool fResult = true;
	wchar_t pszTempSource [PATHMAX];
	bool converted = true;
	HANDLE hFile = 0;

	TEncodingScheme encScheme;
	if (!FileIsUnicode(aPkgFileName,encScheme))
		{
		if ( EUtf8 != encScheme )
			iObserver.DoVerbage(L"Byte order marker not detected. Assuming UTF-8 encoding");
		iObserver.DoVerbage(L"Encoding scheme: UTF8");
		wcscpy(pszTempSource,ConvertFileToUnicode(aPkgFileName,encScheme));
		}
	else if (encScheme == EUcs2BE && sizeof(WCHAR) == 2)
		{
		iObserver.DoVerbage(L"Encoding scheme: UCS2 (BE)");
		wcscpy(pszTempSource,ConvertUCS2FileToLittleEndianUnicode(aPkgFileName));
		}
	/*
		Linux first needs conversion to little endian and then into ucs-4
		Linux has wchar in 4 bytes and processing of .pkg file happens based on wchar
		so need conversion here from ucs2 to ucs4
	*/
	else if (encScheme == EUcs2BE && sizeof(WCHAR) == 4)
		{
		iObserver.DoVerbage(L"Encoding scheme: UCS2 (BE)");
		wcscpy(pszTempSource,ConvertUCS2FileToLittleEndianUnicode(aPkgFileName));
		wcscpy(pszTempSource,ConvertUCS2FileToUCS4(pszTempSource));
		}
	else if (encScheme == EUcs2LE && sizeof(WCHAR) == 2)
		{
		iObserver.DoVerbage(L"Encoding scheme: UCS2 (LE)");
		wcscpy(pszTempSource, aPkgFileName);
		converted = false;
		}
	// Convert UCS2 file to UCS4 for Linux
	else if (encScheme == EUcs2LE && sizeof(WCHAR) == 4)
		{
		iObserver.DoVerbage(L"Encoding scheme: UCS2 (LE)");
		wcscpy(pszTempSource,ConvertUCS2FileToUCS4(aPkgFileName));
		}
	else
		{
		iObserver.DoVerbage(L"Encoding scheme not supported");
		throw ErrCannotOpenFile;
		}
	
	hFile = ::MakeSISOpenFile(pszTempSource,GENERIC_READ,OPEN_EXISTING);

	if (hFile==INVALID_HANDLE_VALUE)
		{
		throw ErrCannotOpenFile;
		}
	
	if(encScheme == EUtf8)
		{
		UnicodeMarker(hFile);
		}

	if(converted)
		{
		_wunlink(pszTempSource);
		}
	
	return hFile;
	}

void CPackageParser::ParseL (const wchar_t* aPkgFileName)
// Purpose  : Check the contents of an input line and defers parsing to the appropriate ParseXXXL()
//			  method
// Inputs   : m_pkgPtr - The line to process
	{
	iValidSISFile = true;
	iFileHandle = OpenPackageFile(aPkgFileName);
	assert(iFileHandle != INVALID_HANDLE_VALUE);

	iEnoughForStub = false;
	iSISXWriter.SetDefaultContent ();

	iObserver.SetLineNumber(++iLineNo);

	// Make sure we're at the beginning of the file
	::SetFilePointer(iFileHandle, 0L, NULL, FILE_BEGIN);

	GetNextChar();

	// skip unicode marker if present
	if(iPkgChar==0xFEFF) GetNextChar();

	GetNextToken ();
	while(iToken!=EOF_TOKEN && !(iMakeStub && iEnoughForStub))
		{
		ParseEmbeddedBlockL (iSISXWriter.InstallBlock ());
		switch (iToken)
			{
			case '&':
				// Pkg file keyword check is disabled here 
 				// to avoid conflicts with the language code 
 				// (e.g. "IF - International French").
 				GetNextToken (1);
				ParseLanguagesL();
				break;
			case '#':
				GetNextToken ();
				ParseHeaderL();
				break;
			case '%':
				GetNextToken ();
				ParseVendorNameL();
				break;
			case '=':
				GetNextToken ();
				ParseLogoL();
				break;
			case '(':
				GetNextToken ();
				ParseDependencyL();
				break;
			case '*':
				GetNextToken ();
				ParseSignatureL();
				break;
			case ':':
				GetNextToken ();
				ParseVendorUniqueNameL();
				break;
			case '[':
				GetNextToken ();
				ParseTargetDeviceL();
				break;
			case EOF_TOKEN:
				break;
			default :
				{
				throw ErrUnknownLine;
				}
			}
		}
	if (! iUniqueVendorName && ! iLocalisedVendorNames)
		{
		iObserver.DoMsg(L"No vendor names found.\n");
		}
	else if (! iLocalisedVendorNames)
		{
		iObserver.DoMsg(L"Localised vendor names not found.\n");
		}
	else if (! iUniqueVendorName)
		{
		iObserver.DoMsg(L"Unique vendor name not found.\n");
		}
	
	if (iFileHandle)
		{
		::CloseHandle(iFileHandle);
		}
	}

void CPackageParser::ParseEmbeddedBlockL (CSISInstallBlock& aInstall)
	{
	while(iToken!=EOF_TOKEN)
		{
		switch (iToken)
			{
			case QUOTED_STRING_TOKEN:
				ParseFileL (aInstall);
				break;
			case '@':
				GetNextToken ();
				ParsePackageL (aInstall);
				break;
			case '!':
				GetNextToken ();
				ParseOptionsBlockL();
				break;
			case '{':
				GetNextToken ();
				ParseLanguageBlockL (aInstall);
				break;
			case '+':
				GetNextToken ();
				ParsePropertyL ();
				break;
			case IF_TOKEN:
				GetNextToken ();
				ParseIfBlockL (aInstall);
				break;
			case ';' :
				ParseCommentL ();
				break;
			default :
				return;
			}
		}
	}

void CPackageParser::ParseLanguagesL()
// Parses the language definition line
// Updated to support "dialects"
// Dialect definition syntax is: <language>["("<dialect-code>")"]

	{
	iObserver.DoVerbage(L"processing languages");

	if (iSISXWriter.AreLanguagesSpecified ())
		throw ErrLanguagesAlreadyDefined;

	CSISLanguage::TDialect dialect (0);

	while (true)
		{
		if (iToken==ALPHA_TOKEN)
			{
			dialect = CSISLanguage::IdentifyLanguage (iTokenValue.pszString);
			if (dialect == CSISLanguage::ELangNone) throw ErrUnknownLanguagesId;
			}
		else if (iToken==NUMERIC_TOKEN && iTokenValue.dwNumber>=0 )
			{
			dialect = static_cast <CSISLanguage::TDialect> (iTokenValue.dwNumber);
			}
		else
			throw ErrUnknownLanguagesId;
		GetNextToken ();
		// Check if a dialect is defined
		if (iToken == '(')
			{
			GetNumericToken();
			// Modify the last added language code, combining it with dialect code
			if (dialect) dialect = dialect + static_cast <CSISLanguage::TDialect> (iTokenValue.dwNumber);
			GetNextToken ();
			if (iToken != ')')
				throw ErrUnexpectedToken;
			GetNextToken ();
			}
		iSISXWriter.AddLanguage (dialect);
		dialect=0;
		if (iToken!=',')
			return;
		GetNextToken (1);
		}
	}


void CPackageParser::ParseHeaderL()
// Parses the pkg header line
	{
	iObserver.DoVerbage(L"processing header");

	// Default type
	std::wstring interpretType = L"SA";
	bool isNonRemovable = false;

	// By default the ROM Upgradeable flag (RU) is FALSE.
	bool isROMUpgrade = false;

	if (iIsHeaderDefined)
		throw ErrHeaderAlreadyDefined;

	if (! iSISXWriter.AreLanguagesSpecified ())
		{
		iObserver.DoVerbage (L"No languages defined, assuming English.");
		}

	// process application names
	ExpectToken('{');
	for (WORD wNumLangs = 0; wNumLangs < iSISXWriter.GetNoLanguages(); wNumLangs++)
		{
		GetNextToken ();
		ExpectToken(QUOTED_STRING_TOKEN);
		iSISXWriter.AddName (iTokenValue.pszString);
		GetNextToken ();
		if (wNumLangs < iSISXWriter.GetNoLanguages() -1 )
			{
			ExpectToken(',');
			}
		}
	ExpectToken('}');
	GetNextToken ();

	DWORD dwFlags = 0;
	DWORD dwType = 0;

	ExpectToken(',');
	GetNextToken ();
	ExpectToken('(');
	GetNextToken ();

	ExpectToken(NUMERIC_TOKEN);
	CSISUid::TUid uid = iTokenValue.dwNumber;
	GetNextToken ();

	ExpectToken(')');
	GetNextToken ();
	ExpectToken(',');
	GetNextToken ();

	ExpectToken(NUMERIC_TOKEN);
	CSISVersion::TMajor major = iTokenValue.dwNumber;
	GetNextToken ();
	ExpectToken(',');
	GetNextToken ();

	ExpectToken(NUMERIC_TOKEN);
	CSISVersion::TMinor minor = iTokenValue.dwNumber;
	GetNextToken ();
	ExpectToken(',');
	GetNextToken ();

	ExpectToken(NUMERIC_TOKEN);
	CSISVersion::TBuild build = iTokenValue.dwNumber;
	GetNextToken ();

	ValidateVersion(major,minor,build);

	// Parse any options
	bool narrow=false;
	while (iToken==',')
		{
		GetNextToken ();
		if (iToken==TYPE_TOKEN)
			{
			GetNextToken ();
			ExpectToken('=');
			GetNextToken ();
			interpretType = iTokenValue.pszString;
			iSISXWriter.InterpretType (iTokenValue.pszString);
			GetNextToken ();
			}
		else
			{
			DWORD option=ParseOption(KHeaderOptions,NUMHEADEROPTIONS,&dwFlags);
			narrow=(option==0);

			switch (option)
				{

			case EInstShutdownApps:
				iSISXWriter.AddInstallFlag(CSISInfo::EInstFlagShutdownApps);
				break;

			case EInstNonRemovable:
				isNonRemovable = true;
				iSISXWriter.AddInstallFlag(CSISInfo::EInstFlagNonRemovable);
				break;			
				
			case EInstROMUpgrade:
				isROMUpgrade = true;
				iSISXWriter.AddInstallFlag(CSISInfo::EInstFlagROMUpgrade);
				break;
				
			case EInstHide:
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK				
				iSISXWriter.AddInstallFlag(CSISInfo::EInstFlagHide);
			#endif				
				break;
				}			
			}
		}
		
	//ROM Upgrade is applicable only for SA, PU & SP, reject package file if RU is coupled 
 	//with anything other than these three install types. 
 	if(isROMUpgrade && ((interpretType != L"SA") && (interpretType != L"PU") && (interpretType != L"SP")))
 		{ 		
		throw ErrInvalidInstallFlagOption;
 		}		

	// Reject package types PA-PP if marked non-removable
	if (isNonRemovable && (interpretType == L"PA" || interpretType == L"PP"))
		{
		throw ErrHeaderRejectPreInstalledNonRemovable;
		}

	if (InterpretSis() && (interpretType == L"PP"))
		{
		ReportInterpretSisError(ErrInvalidAppType);
		}

	// if narrow not explicitly set default to unicode
	if (!narrow) dwFlags|=EInstIsUnicode;

   	iSISXWriter.SetVersionInfo (uid, TVersion (major, minor, build), dwType, dwFlags);
	iIsHeaderDefined = true;
	}

void CPackageParser::ParseVendorNameL ()
	{
	iObserver.DoVerbage(L"processing vendor name");

	ExpectToken('{');
	for (WORD wNumLangs = 0; wNumLangs < iSISXWriter.GetNoLanguages(); wNumLangs++)
		{
		GetNextToken ();
		ExpectToken(QUOTED_STRING_TOKEN);
		iSISXWriter.AddVendorName (iTokenValue.pszString);
		GetNextToken ();
		if (wNumLangs < iSISXWriter.GetNoLanguages() -1 )
			{
			ExpectToken(',');
			}
		}
	ExpectToken('}');
	GetNextToken ();
	iLocalisedVendorNames = true;
	}

void CPackageParser::ParseLogoL ()
	{
	iObserver.DoVerbage(L"processing logo");

	ExpectToken (QUOTED_STRING_TOKEN);
	std::wstring file (iTokenValue.pszString);

	const wchar_t *pBuffer = file.c_str();
	wchar_t *pCurrent = const_cast<wchar_t*>(pBuffer);
	while (pBuffer && *pBuffer && (pCurrent = wcschr(pBuffer,L'\\')) != NULL)
		{
		*pCurrent = L'/';
		pBuffer = pCurrent + 1;
		}

	GetNextToken ();
	ExpectToken(',');
	GetNextToken ();
	ExpectToken (QUOTED_STRING_TOKEN);
	std::wstring mime (iTokenValue.pszString);

	pBuffer = mime.c_str();
	pCurrent = const_cast<wchar_t*>(pBuffer);
	while (pBuffer && *pBuffer && (pCurrent = wcschr(pBuffer,L'\\')) != NULL)
		{
		*pCurrent = L'/';
		pBuffer = pCurrent + 1;
		}

	GetNextToken ();
	std::wstring target;

	if (iToken==',')
		{
		GetNextToken ();
		ExpectToken (QUOTED_STRING_TOKEN);
		target = iTokenValue.pszString;
		}
	iSISXWriter.SetLogo (file, mime, target);
	const CSISLogo& logo = iSISXWriter.SISContent().Controller().Logo();
	CSISFileDescription& fd = const_cast<CSISFileDescription&>(logo.FileDesc());
	ComputeAndSetHash(fd);
	GetNextToken ();
	}

void CPackageParser::ParseFileL (CSISInstallBlock& aInstall)
// Parses a file definition line
	{
	iObserver.DoVerbage(L"processing file");
	bool fileProblem = false;

	if (! iIsHeaderDefined)
		throw ErrHeaderNotDefined;

	std::wstring sourceFile (iTokenValue.pszString);

	// Linux and windows both support forward slashes so if source path is given '\' need to convert
	// in forward slash for compatibility.
	const wchar_t *pBuffer = sourceFile.c_str();
	wchar_t *pCurrent = const_cast<wchar_t*>(pBuffer);
	while (pBuffer && *pBuffer && (pCurrent = wcschr(pBuffer,L'\\')) != NULL)
		{
		*pCurrent = L'/';
		pBuffer = pCurrent + 1;
		}

	TUint64 size;
	TUint64 compressedSize = 0;

	bool isFileNameEmpty = sourceFile.empty ();
	if(CSISContents::IsPreInstalledApp() && isFileNameEmpty)
		{
		fileProblem = true;
		iObserver.DoMsg(L" Error : Source file is missing for PreInstalled APP : ");
		CSISException::ThrowIf (fileProblem ,
							CSISException::EFileProblem,
							std::wstring (L"Source file is missing for PreInstalled APP "));
		}

	if(! isFileNameEmpty && ! DoesFileExist (sourceFile, size))
		{
		wchar_t msg[255];
		wcscpy(msg, L"Cannot find file : ");
		wcscat(msg, sourceFile.c_str());
		wcsncat(msg, L"\n", 1);
		iObserver.DoErrMsg(msg);
		fileProblem = true;
		}

	CSISException::ThrowIf (fileProblem ,
							CSISException::EFileProblem,
							std::wstring (L"cannot find ") + sourceFile);

	GetNextToken ();

	ExpectToken('-');
	GetNextToken ();

	ExpectToken(QUOTED_STRING_TOKEN);

	// BAW-5CQEA5 Check for invalid destination (i.e starts with ':')
	if ((sourceFile.size () > 0) && (sourceFile [0] == ':'))
		throw ErrBadDestinationPath;

	std::wstring destinationFile (iTokenValue.pszString);

	// SWI only supports backward slashesh so need to convert destination path in backward slash if
	// user gives '/' in Linux.
	pBuffer = destinationFile.c_str();
	pCurrent = const_cast<wchar_t*>(pBuffer);
	while (pBuffer && *pBuffer && (pCurrent = wcschr(pBuffer,L'/')) != NULL)
		{
		*pCurrent = L'\\';
		pBuffer = pCurrent + 1;
		}

	// SWI only supports backward slashesh so need to convert destination path in backward slash if
	// user gives '/' in Linux.
	pBuffer = destinationFile.c_str();
	pCurrent = const_cast<wchar_t*>(pBuffer);
	while (pBuffer && *pBuffer && (pCurrent = wcschr(pBuffer,L'/')) != NULL)
		{
		*pCurrent = L'\\';
		pBuffer = pCurrent + 1;
		} 	

	bool bNewFile = true;
	// CSisFileDescription is allocated on the heap, since the compiler does not support well
	// its allocation on the stack combined with exceptions (DEF108815)
	std::auto_ptr<CSISFileDescription> fd(new CSISFileDescription);
	TUint32 fileIndex = Find((wchar_t*)sourceFile.c_str(),*fd);
	if ( fileIndex == -1 )
		{
		aInstall.AddFileDescription ();
		}
	else
		{
		aInstall.AddFileDescription (*fd);
		bNewFile = false;
		}

	if (! isFileNameEmpty)
		{
		if ( bNewFile )
			{
			TUint64 size = 0;
			fileIndex = iSISXWriter.LoadFile (sourceFile, &size);
			aInstall.SetFileIndex (fileIndex);
			aInstall.ExtractCapabilities(sourceFile);
			aInstall.SetLengths (size, iSISXWriter.CompressedSize ());
			ComputeAndSetHash(aInstall.FileDescription());

			Set(sourceFile,aInstall.FileDescription());
			}
		}

	aInstall.SetTarget (destinationFile);

	GetNextToken ();

	if ( !bNewFile )
		{
		CSISFileDescription& fdLast = aInstall.FileDescription();
		fdLast.SetOperation(0);
		fdLast.SetOptions(0);
		}

	// Test for options
	if (iToken!=',')
		{
		aInstall.SetOperation (CSISFileDescription::EOpInstall);
		}
	else
		{
		GetNextToken ();

		CSISFileDescription::TSISInstOption type = aInstall.InterpretOption (iTokenValue.pszString);
		GetNextToken ();
		if (type == CSISFileDescription::EInstFileRunOptionByMimeType)
			{
			ExpectToken (',');
			GetNextToken ();
			ExpectToken (QUOTED_STRING_TOKEN);
			aInstall.SetMimeType (iTokenValue.pszString);
			GetNextToken ();
			}
		if (iToken==',')
			{
			do
				{
   
				GetNextToken ();
				aInstall.InterpretOption (iTokenValue.pszString);

				//if the next token is a comma , then continue.
				GetNextToken ();

				} while (iToken == ',');

			}
		}

	}

void CPackageParser::ParsePackageL (CSISInstallBlock& aInstall)
// Parses a package (component SIS file) line
	{
	iObserver.DoVerbage(L"processing embedded package file");

	if (InterpretSis())
		{
		ReportInterpretSisError(ErrEmbeddedSisNotSupported);
		}

	if (! iIsHeaderDefined)
		throw ErrHeaderNotDefined;

	ExpectToken(QUOTED_STRING_TOKEN);
	WCHAR pszFile[MAX_STRING];
	wcscpy(pszFile,iTokenValue.pszString);
	std::wstring fileName;
	std::wstring name;
	//check whether search directory is specified.
	if(iSearchDir[0] != '\0')
		{
		if ((wcslen (iSearchDir) + wcslen (pszFile)) < PATHMAX - 1)
			{
			WCHAR pszNewPath[PATHMAX];
			wcscpy(pszNewPath, iSearchDir);
			wcscat(pszNewPath, pszFile);
			fileName.append(pszNewPath);
			}
		}
	else
		{
		name = iTokenValue.pszString;
		}


	// Test that the file exists
	GetNextToken ();

	ExpectToken(',');
	GetNextToken ();
	ExpectToken('(');
	GetNextToken ();
	ExpectToken(NUMERIC_TOKEN);
	CSISUid::TUid uid = iTokenValue.dwNumber;
	GetNextToken ();
	ExpectToken(')');
	GetNextToken ();

	if(iSearchDir[0] != '\0')
		{
		iSISXWriter.EmbedFile (fileName, uid, aInstall );
		}
	else
		{
		iSISXWriter.EmbedFile (name, uid, aInstall );
		}
	}

TVersion CPackageParser::ParseVersion()
// Parse version information for dependency lines
	{

	CSISVersion::TMajor major;
	GetNextToken();
	if (iToken == '*')
		{
		major = -1;
		}
	else
		{
		ExpectToken(NUMERIC_TOKEN);
		major = iTokenValue.dwNumber;
		}

	GetNextToken();
	ExpectToken(',');

	CSISVersion::TMinor minor;
	GetNextToken();
	if (iToken == '*')
		{
		minor = -1;
		}
	else
		{
		ExpectToken(NUMERIC_TOKEN);
		minor = iTokenValue.dwNumber;
		}

	GetNextToken();
	ExpectToken(',');

	CSISVersion::TBuild build;
	GetNextToken();
	if (iToken == '*')
		{
		build = -1;
		}
	else
		{
		ExpectToken(NUMERIC_TOKEN);
		build = iTokenValue.dwNumber;
		}

	GetNextToken();

	ValidateVersion(major, minor, build);
	
	return TVersion(major, minor, build);

	}

void CPackageParser::ParseDependencyL()
// Parses a dependency line
	{
	iObserver.DoVerbage(L"processing dependency");

	if (! iIsHeaderDefined)
		throw ErrHeaderNotDefined;

	ExpectToken(NUMERIC_TOKEN);
	CSISUid::TUid uid = iTokenValue.dwNumber;
	GetNextToken ();

	ExpectToken(')');
	GetNextToken ();
	ExpectToken(',');

	TVersion from = ParseVersion();
	TVersion to(-1,-1,-1);

	if (iToken == '~')
		{
		to = ParseVersion();
		ExpectToken(',');
		}
	else if (iToken == ',')
		{
		to = TVersion(KIrrelevant, KIrrelevant, KIrrelevant);
		}
	else
		{
		iObserver.DoErrMsg(L"Expected '-' or ',' in dependency line");
		throw ErrUnexpectedToken;
		}


	GetNextToken ();
	ExpectToken('{');

	iSISXWriter.AddDependency (uid, from, to);
		// must do this before adding language strings

	for (TUint16 numLangs = 0; numLangs < iSISXWriter.GetNoLanguages (); ++numLangs)
		{
		GetNextToken ();
		ExpectToken(QUOTED_STRING_TOKEN);
		iSISXWriter.AddDependencyName (iTokenValue.pszString);
		GetNextToken ();
		if (numLangs < (iSISXWriter.GetNoLanguages() - 1))
			ExpectToken(',');
		}
	ExpectToken('}');
	GetNextToken ();
	}

void CPackageParser::ParseTargetDeviceL()
// Parses a target device line
	{
	iObserver.DoVerbage(L"processing target device");

	if (! iIsHeaderDefined)
		throw ErrHeaderNotDefined;

	ExpectToken(NUMERIC_TOKEN);
	CSISUid::TUid uid = iTokenValue.dwNumber;
	GetNextToken ();

	ExpectToken(']');
	GetNextToken ();
	ExpectToken(',');

	TVersion from = ParseVersion();
	TVersion to(-1,-1,-1);

	if (iToken == '~')
		{
		to = ParseVersion();
		ExpectToken(',');
		}
	else if (iToken == ',')
		{
		to = TVersion(KIrrelevant, KIrrelevant, KIrrelevant);
		}
	else
		{
		iObserver.DoErrMsg(L"Expected '-' or ',' in dependency line");
		throw ErrUnexpectedToken;
		}

	GetNextToken ();
	ExpectToken('{');

	iSISXWriter.AddTarget (uid, from, to);
		// must do this before adding language strings

	for (TUint16 numLangs = 0; numLangs < iSISXWriter.GetNoLanguages (); ++numLangs)
		{
		GetNextToken ();
		ExpectToken(QUOTED_STRING_TOKEN);
		iSISXWriter.AddTargetName (iTokenValue.pszString);
		GetNextToken ();
		if (numLangs < (iSISXWriter.GetNoLanguages() - 1))
			ExpectToken(',');
		}
	ExpectToken('}');
	GetNextToken ();
	}

void CPackageParser::ParseSignatureL()
{
	// Parses the package-signature
	iObserver.DoVerbage(L"processing signature");

	// MAKESIS ignore the Signature,this option is deprecated.
	iObserver.DoMsg(L"Signature ignored,this option is deprecated...\n");

	if (! iIsHeaderDefined)
		throw ErrHeaderNotDefined;

	// If generating a stub file then once have signature line don't need to
	// process rest of PKG file
	iEnoughForStub=true;

	ExpectToken(QUOTED_STRING_TOKEN);

	GetNextToken ();
	ExpectToken(',');
	GetNextToken ();

	ExpectToken(QUOTED_STRING_TOKEN);
	GetNextToken ();

	//check for optional items (password)
	if (iToken==',')
		{
		GetNextToken ();
		if (iToken==KEY_TOKEN)
			{
			GetNextToken ();
			ExpectToken('=');
			GetNextToken ();
			ExpectToken(QUOTED_STRING_TOKEN);
			GetNextToken ();
			}
		}

}


void CPackageParser::ParseVendorUniqueNameL()
{
	// Parses the package-signature
	iObserver.DoVerbage(L"processing unique vendor name");

	ExpectToken(QUOTED_STRING_TOKEN);
	iSISXWriter.SetVendorUniqueName (iTokenValue.pszString);
	iUniqueVendorName = true;
	GetNextToken ();
}

void CPackageParser::ParsePropertyL ()
// Parses a capability line
	{
	iObserver.DoVerbage(L"processing property");

	CSISProperty::TKey key;
	CSISProperty::TValue value;

	ExpectToken('(');
	do
		{
		GetNextToken ();

		ExpectToken(NUMERIC_TOKEN);
		key = iTokenValue.dwNumber;
		GetNextToken ();
		ExpectToken('=');
		GetNextToken ();
		ExpectToken(NUMERIC_TOKEN);
		value = iTokenValue.dwNumber;
		iSISXWriter.AddProperty (key, value);

		GetNextToken ();
		} while (iToken==',');
	ExpectToken(')');
	GetNextToken ();
	}

void CPackageParser::ParseOptionsBlockL ()
// To parse an options block
	{
	WORD wNumLangs;

	iObserver.DoVerbage(L"processing options block");

	ExpectToken('(');
	GetNextToken ();

	if (InterpretSis())
		{
		ReportInterpretSisError(ErrUserOptionsNotSupported);
		}

	for (;;)
		{
		ExpectToken('{');
		GetNextToken ();

		iSISXWriter.AddOption ();

		wNumLangs = 0;
		while (wNumLangs < iSISXWriter.GetNoLanguages())
			{
			ExpectToken(QUOTED_STRING_TOKEN);
			iSISXWriter.AddOptionName (iTokenValue.pszString);
			GetNextToken ();
			if (wNumLangs < iSISXWriter.GetNoLanguages() - 1)
				{
				ExpectToken(',');
				GetNextToken ();
				}
			wNumLangs++;
			}

		ExpectToken('}');
		GetNextToken ();
		if (iToken!=',') break;
		GetNextToken ();
		}

	ExpectToken(')');
	GetNextToken ();

	}

void CPackageParser::ParseLanguageBlockL (CSISInstallBlock& aInstall)
// To parse the start of a language block
	{
	iObserver.DoVerbage(L"processing language block");

	if (! iIsHeaderDefined)
		throw ErrHeaderNotDefined;

	bool packages=(iToken=='@');
	unsigned languageCount = iSISXWriter.GetNoLanguages ();
	std::vector <unsigned> fileIndex (languageCount);
	std::vector <TUint64> fileSize (languageCount);
	std::vector <TUint64> compressedSize (languageCount);
	std::vector <CSISHash> hashes (languageCount);

	for (DWORD iCurrentLang = 0; iCurrentLang < languageCount; iCurrentLang++)
		{
		if (iCurrentLang == 0)
			{
			aInstall.AddIf ();
			aInstall.If ().Expression ().SetLanguageComparision (iSISXWriter.Language (iCurrentLang));
			}
		else
			{
			aInstall.If ().AddElseIf ();
			aInstall.If ().ElseIf ().Expression ().SetLanguageComparision (iSISXWriter.Language (iCurrentLang));
		}
		if (packages)
			{
			ExpectToken ('@');
			GetNextToken ();
			if (iCurrentLang == 0)
				{
				iSISXWriter.EmbedPackage (aInstall.If ().InstallBlock (), iTokenValue.pszString, 0);
				}
			else
				{
				iSISXWriter.EmbedPackage (aInstall.If ().ElseIf ().InstallBlock (), iTokenValue.pszString, 0);
				}
			}
		else
			{
			CSISFileDescription fd;
			wchar_t *pBuffer = iTokenValue.pszString;
            wchar_t *pCurrent = pBuffer;
            while (pBuffer && *pBuffer && (pCurrent = wcschr(pBuffer,L'\\')) != NULL)
		    {
      		*pCurrent = L'/';
        	pBuffer = pCurrent + 1;
		    }

			TUint32 index = Find(iTokenValue.pszString,fd);
			if ( index == -1 )
				{
				HANDLE  hFile = ::MakeSISOpenFile (iTokenValue.pszString, GENERIC_READ, OPEN_EXISTING);
				if (hFile == INVALID_HANDLE_VALUE)
				{
				// If we are using a search directory
			 	if(iSearchDir[0] != '\0')		
					{
					if (wcslen (iSearchDir) + wcslen (iTokenValue.pszString) < PATHMAX - 1)
						{
						WCHAR pszNewPath[PATHMAX];
						wcscpy(pszNewPath, iSearchDir);
						wcscat(pszNewPath, iTokenValue.pszString);
						HANDLE hFile = ::MakeSISOpenFile(pszNewPath, GENERIC_READ, OPEN_EXISTING);
						if (hFile != INVALID_HANDLE_VALUE) wcscpy(iTokenValue.pszString, pszNewPath);
						::CloseHandle(hFile);
						}
					}
				}
				::CloseHandle(hFile);

				fileIndex [iCurrentLang] = iSISXWriter.LoadFile (iTokenValue.pszString, &fileSize [iCurrentLang]);
				compressedSize [iCurrentLang] = iSISXWriter.CompressedSize ();

				fd.SetFileIndex(fileIndex[iCurrentLang]);
				fd.SetLengths (fileSize [iCurrentLang], compressedSize [iCurrentLang]);
				ComputeAndSetHash(fd);

				hashes [iCurrentLang] = fd.Hash();

				// Add new file to the map
				std::wstring newfile(iTokenValue.pszString);
				Set(newfile,fd);
				}
			else
				{
				fileIndex [iCurrentLang] = fd.FileIndex();
				fileSize [iCurrentLang] = fd.UncompressedLength();
				compressedSize [iCurrentLang] = fd.Length();
				hashes [iCurrentLang] = fd.Hash();
				}
			}
		GetNextToken ();
		}

	ExpectToken('}');
	GetNextToken ();

	if(!packages)
		{
		ExpectToken('-');
		GetNextToken ();

		// Get the destination & options
		ExpectToken(QUOTED_STRING_TOKEN);
		CSISFileDescription file;
		file.SetTarget (iTokenValue.pszString);

		GetNextToken ();

		if (iToken==',')
			{
			GetNextToken ();
			CSISFileDescription::TSISInstOption type = file.InterpretOption (iTokenValue.pszString);
			GetNextToken();
			if (type == CSISFileDescription::EInstFileRunOptionByMimeType)
				{
				ExpectToken(',');
				GetNextToken ();
				ExpectToken(QUOTED_STRING_TOKEN);
				file.SetMimeType (iTokenValue.pszString);
				GetNextToken ();
				}
			else
			if (iToken==',')
				{
				do
					{

					GetNextToken ();
					file.InterpretOption (iTokenValue.pszString);
					GetNextToken ();

					} while (iToken == ',');

				}
			}

		for (unsigned index = 0; index < languageCount; index++)
			{
			if (index == 0)
				{
				aInstall.If ().InstallBlock ().AddFileDescription (file);
				aInstall.If ().InstallBlock ().SetFileIndex (fileIndex [index]);
				aInstall.If ().InstallBlock ().SetLengths (fileSize [index], compressedSize [index]);
				aInstall.If ().InstallBlock ().SetHash (hashes[index]);
				}
			else
				{
				aInstall.If ().ElseIf (index - 1).InstallBlock ().AddFileDescription (file);
				aInstall.If ().ElseIf (index - 1).InstallBlock ().SetFileIndex (fileIndex [index]);
				aInstall.If ().ElseIf (index - 1).InstallBlock ().SetLengths (fileSize [index], compressedSize [index]);
				aInstall.If ().ElseIf (index - 1).InstallBlock ().SetHash (hashes[index]);
				}
			}
		}
	else
		{
		ExpectToken(',');
		GetNextToken ();
		ExpectToken('(');
		GetNextToken ();

		ExpectToken(NUMERIC_TOKEN);
		GetNextToken ();

		ExpectToken(')');
		GetNextToken ();

		}
	}

void CPackageParser::ParseIfBlockL (CSISInstallBlock& aInstall)
	{
	iObserver.DoVerbage(L"processing IF block");

	aInstall.AddIf ();

	ParseLogicalOp (aInstall.If ().Expression ());
	ParseEmbeddedBlockL (aInstall.If ().InstallBlock ());

	while (iToken==ELSEIF_TOKEN)
		{
		GetNextToken ();
		aInstall.If ().AddElseIf ();
		ParseLogicalOp (aInstall.If ().ElseIfExpression ());
		ParseEmbeddedBlockL (aInstall.If ().ElseIf ().InstallBlock ());
		}

	if (iToken==ELSE_TOKEN)
		{
		GetNextToken ();
		aInstall.If ().AddElse ();
		// elseif true
		ParseEmbeddedBlockL(aInstall.If ().ElseIf ().InstallBlock ());
		}

	ExpectToken(ENDIF_TOKEN);
	GetNextToken ();
	}

void CPackageParser::ParseCommentL()
// Purpose  : Parses a comment line (Does nothing, just throws the line away)
// Inputs   : m_pkgPtr - The string to parse
// Returns  : Success or failure
	{
	iObserver.DoVerbage(L"processing comment");

	// parse to end of line
	while (iPkgChar && (iPkgChar!='\n')) GetNextChar();
	GetNextToken ();
	}

void CPackageParser::ParseLogicalOp (CSISExpression& aExpression)
	{
	CSISExpression left;

	if (InterpretSis())
		{
		std::wstring aOption = iTokenValue.pszString;
		for(size_t i = 0; i < aOption.length(); ++i)
			aOption.at(i) = towupper(aOption.at(i));

		if (aOption.find(L"OPTION",0) != std::string::npos)
			{
			ReportInterpretSisError(ErrUserOptionsNotSupported);
			}
		}

    ParseRelation (left);
	switch (iToken)
		{
		case AND_TOKEN:
		case OR_TOKEN:
			{
			if (iToken==AND_TOKEN)
				aExpression.SetOperator (CSISExpression::ELogOpAnd, left);
			else
				aExpression.SetOperator (CSISExpression::ELogOpOr, left);
			GetNextToken ();
			ParseLogicalOp (aExpression.RHS ());
			}
			break;
		default:
			aExpression = left;
			break;
		}
	}


void CPackageParser::ParseRelation(CSISExpression& aExpression)
	{
	CSISExpression left;
    ParseUnary (left);
	switch (iToken)
		{
		case '=':
		case '>':
		case '<':
		case GE_TOKEN:
		case LE_TOKEN:
		case NE_TOKEN:
			{
			switch (iToken)
				{
				case '=':
					aExpression.SetOperator (CSISExpression::EBinOpEqual, left);
					break;
				case '>':
					aExpression.SetOperator (CSISExpression::EBinOpGreaterThan, left);
					break;
				case '<':
					aExpression.SetOperator (CSISExpression::EBinOpLessThan, left);
					break;
				case GE_TOKEN:
					aExpression.SetOperator (CSISExpression::EBinOpGreaterThanOrEqual, left);
					break;
				case LE_TOKEN:
					aExpression.SetOperator (CSISExpression::EBinOpLessThanOrEqual, left);
					break;
				case NE_TOKEN:
					aExpression.SetOperator (CSISExpression::EBinOpNotEqual, left);
					break;
				}
			GetNextToken ();
			ParseUnary (aExpression.RHS ());
			break;
			}
		default:
			aExpression = left;
			break;
		}
	}

void CPackageParser::ParseUnary(CSISExpression& aExpression)
	{
    switch (iToken)
		{
		case NOT_TOKEN:
			aExpression.SetOperator (CSISExpression::EUnaryOpNot);
			GetNextToken ();
			ParseUnary (aExpression.RHS ());
			break;
		case APPCAP_TOKEN:
			{	// 2 arg function
			GetNextToken ();
			ExpectToken('(');

			aExpression.SetOperator (CSISExpression::EFuncAppProperties);
			GetNextToken ();
			ParseUnary (aExpression.LHS ());
			ExpectToken(',');
			GetNextToken ();
			ParseUnary (aExpression.RHS ());
//			GetNextToken ();

			ExpectToken(')');
			GetNextToken ();
			break;
			}
		case EXISTS_TOKEN:
		case DEVCAP_TOKEN:
			{	// 1 arg function
			TOKEN token=iToken;
			GetNextToken ();
			ExpectToken('(');
			GetNextToken ();
			if (token==EXISTS_TOKEN)
				{
				aExpression.SetOperator (CSISExpression::EFuncExists);
				ExpectToken(QUOTED_STRING_TOKEN);
				GetNextToken ();
				aExpression.SetValue (std::wstring (iTokenValue.pszString));
				}
			else
				{
				aExpression.SetOperator (CSISExpression::EFuncDevProperties);
				ParseUnary (aExpression.RHS ());
				}
			ExpectToken(')');
			GetNextToken ();
			break;
			}
		case VERSION_TOKEN:
			{
			/**
			CR1125 - VERSION() function to allow for the evaluation of installed package versions

			The functions arguments are parsed and formatted into an argument string within 
			CPackageParser::ParseVersionArgs(). The SISString is then stored within the SISExpression
			using the CSISExpression::EFuncExists TOperator enumeration for compatibility with old
			versions of the SWI Server.
			**/  
			GetNextToken ();
			ExpectToken('(');

			aExpression.SetOperator (CSISExpression::EFuncExists);
			GetNextToken ();
			aExpression.SetValue (ParseVersionArgs());

			ExpectToken(')');
			GetNextToken ();
			break;
			}
			
		// Block to handle Supported_Language token used in package file
		// This block sets the SISExpression with the corresponding fields
		// like Operator = EFuncExists, Value = "/sys/install/supported_language/?10" 
		// (language ID for the example given below), 
		// Eg:supported_language = 10
		case SUPPORTED_LANG_TOKEN:
			{
			GetNextToken();
			ExpectToken('=');
			
			aExpression.SetOperator (CSISExpression::EFuncExists);
			GetNextToken ();
			aExpression.SetValue (ParseSupportedLangArgs());
			break;
			}

			
		default:
			ParseFactor (aExpression);
			break;
		}
	}

void CPackageParser::ParseFactor(CSISExpression& aExpression)
	{
    switch (iToken) {
		case '(':
			{
			GetNextToken ();
			ParseLogicalOp (aExpression);
			ExpectToken(')');
			}
			break;
		case QUOTED_STRING_TOKEN:
		case ALPHA_TOKEN:
		case NUMERIC_TOKEN:
			{
			switch (iToken)
				{
				case QUOTED_STRING_TOKEN:
					aExpression.SetOperator (CSISExpression::EPrimTypeString);
					aExpression.SetValue (std::wstring (iTokenValue.pszString));
					break;
				case ALPHA_TOKEN:
					{
					if(!CompareNString(iTokenValue.pszString,L"option",6))
						{
						WCHAR *temp,*end;
						temp=&iTokenValue.pszString[6];
						DWORD optionNum = wcstol(temp, &end, 10);
						if (end==temp || errno==ERANGE)
							throw ErrUnknownVariable;
						aExpression.SetOperator (CSISExpression::EPrimTypeOption);
						aExpression.SetValue (optionNum);
						}
					else
						{
						aExpression.SetVariable (std::wstring (iTokenValue.pszString));
						}
					}
					break;
				case NUMERIC_TOKEN:
					aExpression.SetOperator (CSISExpression::EPrimTypeNumber);
					aExpression.SetValue (iTokenValue.dwNumber);
					break;
				}
			}
			break;
		default:
			throw ErrBadCondFormat;
		}
	GetNextToken ();
	}

// Additional Functionality Provided By CR1125
std::wstring CPackageParser::ParseVersionArgs()
	{
	std::wstring agrsString;
	TInt32 vMaj = 0;
	TInt32 vMin = 0;
	TInt32 vBld = 0;
	wchar_t* relation;
	TVersion version;

	ExpectToken(NUMERIC_TOKEN);
	CSISUid::TUid pUid = iTokenValue.dwNumber;
	GetNextToken ();

	ExpectToken(',');
	GetNextToken ();

	switch (iToken)
		{
		case '=':
			relation = L"ET";
			break;
		case '>':
			relation = L"GT";
			break;
		case '<':
			relation = L"LT";
			break;
		case GE_TOKEN:
			relation = L"GE";
			break;
		case LE_TOKEN:
			relation = L"LE";
			break;
		case NE_TOKEN:
			relation = L"NE";
			break;
		default:
			throw ErrVersionInvalidRelationalOperator;
		}

	GetNextToken ();
	ExpectToken(',');

	version = ParseVersion();

	vMaj = version.Major();
	vMin = version.Minor();
	vBld = version.Build();

	
	if(vMaj == -1 || vMin == -1 || vBld == -1)
		{
		// If a wildcard has been specified, throw a bad condition
		throw ErrVersionWildcardsNotSupported;
		}
	else if(vMaj < 0 || vMin < 0 || vBld < 0)
		{
		// If a negative version component has been specified, throw a bad condition
		throw ErrVersionNegativesNotSupported; 
		}

	// Integer to String Conversions
	std::wstring pUidStr;
	pUidStr = ConvertToString(pUid,std::hex);	 

	std::wstring vMajStr;
	vMajStr = ConvertToString(vMaj,std::dec);

	std::wstring vMinStr;
	vMinStr = ConvertToString(vMin,std::dec);

	std::wstring vBldStr;
	vBldStr = ConvertToString(vBld,std::dec);

	// Construct the formatted argument string
	agrsString = L"\\sys\\install\\pkgversion\\?";
	agrsString += L"0x";
	agrsString += pUidStr;
	agrsString += L",";
	agrsString += relation;
	agrsString += L",";
	agrsString += vMajStr;
	agrsString += L",";
	agrsString += vMinStr;
	agrsString += L",";
	agrsString += vBldStr;

	return agrsString;
	}

// Function added to parse the supported language token(suppoted_language) and 
// provide the appropriate value string, which is set onto SISExpression.
// It returns a string like "\sys\install\supported_langauge\?01" if the parsed 
// token is like "(((01)))"
std::wstring CPackageParser::ParseSupportedLangArgs()
	{
	std::wstring argsString;
	TInt32 langId = 0;
	TInt32 parenthesisCount = 0;
	while ( iToken == '(' )
		{
		GetNextToken();
		parenthesisCount++;
		}
	
	ExpectToken(NUMERIC_TOKEN);

	langId = iTokenValue.dwNumber;
	GetNextToken ();
	while ( iToken == ')' && parenthesisCount)
			{
			GetNextToken();
			parenthesisCount--;
			}
	if (!(0 == parenthesisCount))
		{
		throw ErrUnBalancedParenthesis;
		}
	std::wstring vLangIdStr;
	vLangIdStr = ConvertToString(langId,std::dec);

	// Construct the formatted argument string
	argsString = L"\\sys\\install\\supportedlanguage\\?";
	argsString += vLangIdStr;

	return argsString;
	}


std::wstring CPackageParser::ConvertToString(const TInt32 aValue, std::ios_base& (*aBase)(std::ios_base&))
	{
	std::wostringstream wOutStream;

	if((wOutStream << aBase << aValue).fail())
		{
		throw ErrBadIntegerToStringConversion;
		};

	return wOutStream.str();
	}

DWORD CPackageParser::ParseOption(const SParseOpt* options, DWORD dwNumOptions, DWORD* pdwOptions)
// Parse the options part of an input line
	{
	DWORD option=0;
	ExpectToken(ALPHA_TOKEN);
	// Look for the option
	for(WORD wLoop = 0; wLoop < dwNumOptions; wLoop++)
		{
		if(CompareTwoString(iTokenValue.pszString,(wchar_t*) options[wLoop].pszOpt) == 0)
			{
			option=options[wLoop].dwOpt;
			break;
			}
		}
	if(wLoop == dwNumOptions)
		throw ErrBadOption;
	*pdwOptions |= option;
	GetNextToken ();
	return option;
	}

void CPackageParser::ExpectToken(TOKEN aToken)
	{
	if (iToken!=aToken)
		{
		wchar_t msg[255]=L"Expected ";
		if (aToken<=LAST_TOKEN)
			wcsncat(msg,GetTokenText(aToken),wcslen(GetTokenText(aToken)));
		else
			{
			wchar_t tmp[2]={(wchar_t)aToken,0};
			wcsncat(msg,tmp,wcslen(tmp));
			}
		wcsncat(msg,L" read ",wcslen(L" read "));
		if (iToken<=LAST_TOKEN)
			wcsncat(msg,GetTokenText(iToken), wcslen(GetTokenText(iToken)));
		else
			{
			wchar_t tmp[2]={(wchar_t)iToken,0};
			wcsncat(msg,tmp,wcslen(tmp));
			}
		iObserver.DoErrMsg(msg);
		throw ErrUnexpectedToken;
		}
	}

const _TCHAR* CPackageParser::GetTokenText(TOKEN aToken)
	{
	switch(aToken)
		{
		case NUMERIC_TOKEN:
			return L"numeric value";
		case ALPHA_TOKEN:
			return L"alphanumeric value";
		case QUOTED_STRING_TOKEN:
			return L"quoted string";
		case AND_TOKEN:
			return L"AND";
		case OR_TOKEN:
			return L"OR";
		case NOT_TOKEN:
			return L"NOT";
		case EXISTS_TOKEN:
			return L"EXISTS";
		case GE_TOKEN:
			return L">=";
		case LE_TOKEN:
			return L"<=";
		case NE_TOKEN:
			return L"<>";
		case IF_TOKEN:
			return L"IF";
		case ELSEIF_TOKEN:
			return L"ELSEIF";
		case ELSE_TOKEN:
			return L"ELSE";
		case ENDIF_TOKEN:
			return L"ENDIF";
		default:
			return L"?";
		}
	}

void CPackageParser::GetNextToken (const TBool aDisablePkgKeywordCheck)
// lexical analyzer
	{
	// skip any white space & newLine's
	while (iPkgChar == '\n' || isspace(iPkgChar) || iPkgChar == 0xA0)
		{
		if (iPkgChar == '\n')
			{
			iObserver.SetLineNumber(++iLineNo);
			}
		GetNextChar();
		}

	if (iPkgChar == '\0')
		iToken=EOF_TOKEN;
	else if (IsNumericToken())
		{
		GetNumericToken();
		iToken=NUMERIC_TOKEN;
		}
	else if (isalpha(iPkgChar))
		{ // have some alphanumeric text
		GetAlphaNumericToken();
		iToken=ALPHA_TOKEN;
		// check if it is a PKG keyword only when the keyword check is not 
 		// disabled by setting the paramter "aDisablePkgKeywordCheck"
		if (!aDisablePkgKeywordCheck)
			{
			for(WORD wLoop = 0; wLoop < NUMPARSETOKENS; wLoop++)
				{
				if(CompareTwoString(iTokenValue.pszString, (wchar_t*)KTokens[wLoop].pszOpt) == 0)
					{
					iToken=KTokens[wLoop].dwOpt;
					break;
					}
				}
			}
		}
	else if (iPkgChar == '\"')
		{ // have a quoted string
		GetStringToken();
		iToken=QUOTED_STRING_TOKEN;
		}
	else if (iPkgChar == '>')
		{
		GetNextChar();
		if (iPkgChar == '=')
			{
			iToken=GE_TOKEN;
			GetNextChar();
			}
		else
			iToken='>';
		}
	else if (iPkgChar == '<')
		{
		// check if start of an escaped string, e.g. <123>"abc"
		if (GetStringToken())
			iToken=QUOTED_STRING_TOKEN;
		else
			{
			GetNextChar();
			if (iPkgChar == '=')
				{
				iToken=LE_TOKEN;
				GetNextChar();
				}
			else if (iPkgChar == '>')
				{
				iToken=NE_TOKEN;
				GetNextChar();
				}
			else
				iToken='<';
			}
		}
	else
		{
		iToken = iPkgChar;
		GetNextChar();
		}
	}

bool CPackageParser::GetStringToken()
// Purpose  : Parse a quoted string from the input line
// Inputs   : m_pkgPtr    - The string to parse
//			  pszString   - The output string
//            wMaxLength  - The max length of pszString
	{
	DWORD wCount = 0;
	bool done=false;
	bool finished=false;
	DWORD escapeChars = 0;

	while (!finished)
		{
		if (iPkgChar == '\"')
			{
			GetNextChar();
			while(iPkgChar && iPkgChar != '\"')
				{
				if(wCount < (MAX_STRING - 1))
					iTokenValue.pszString[wCount++] = iPkgChar;
				else //We dont want the string with length greater than MAX_STRING to be cut off silently
					throw ErrBadString;
				GetNextChar();
				}
			if(iPkgChar == '\0')
				throw ErrBadString;
			GetNextChar();
			done=true;
			}
		if (iPkgChar == '<')
			{
			iTokenValue.pszString[wCount] = L'\0';
			escapeChars=ParseEscapeChars();
			if (escapeChars>0)
				{
				done=true;
				wCount+=escapeChars;
				if (wCount>=MAX_STRING) wCount=MAX_STRING-1;
				}
			}
		if (escapeChars==0 || iPkgChar != '\"')
			finished=true;
		}

	iTokenValue.pszString[wCount] = L'\0';
	return done;
	}

WORD CPackageParser::ParseEscapeChars()
	{
	WORD found=0;
	WCHAR temp[MAX_STRING];
	while (iPkgChar == '<')
		{
		wcscpy(temp,iTokenValue.pszString);
		DWORD fileOffset=::SetFilePointer(iFileHandle, 0L, NULL, FILE_CURRENT);
		try
			{
			GetNextChar();
			GetNumericToken();
			if (iPkgChar=='>')
				found++;
			else
				{
				::SetFilePointer(iFileHandle, fileOffset, NULL, FILE_BEGIN);
				break;
				}
			}
		catch (TParseException)
			{
			wcscpy(iTokenValue.pszString,temp);
			::SetFilePointer(iFileHandle, fileOffset, NULL, FILE_BEGIN);
			break;
			}
		DWORD num=iTokenValue.dwNumber;
		// watch for CP1252 escapes which aren't appropriate for UNICODE
		if (num>=0x80 && num<=0x9F) throw ErrInvalidEscape;
		DWORD len=wcslen(temp);
		wcscpy(iTokenValue.pszString,temp);
		if (len+2<=MAX_STRING)
			{
			iTokenValue.pszString[len]=(WCHAR)num;
			len++;
			iTokenValue.pszString[len]='\0';
			}
		GetNextChar();
		}
	return found;
	}

void CPackageParser::GetAlphaNumericToken()
// Purpose  : Parse an alphanumeric string from the input line
// Inputs   : m_pkgPtr    - The string to parse
//			  pszString   - The output string
//            wMaxLength  - The max length of pszString
	{
	WORD wCount = 0;
	while(iPkgChar && (isalnum(iPkgChar) || ((iPkgChar) == '_')))
		{
		if(wCount < (MAX_STRING - 1))
			iTokenValue.pszString[wCount++] = iPkgChar;
		GetNextChar();
		}
	iTokenValue.pszString[wCount] = L'\0';
	}

bool CPackageParser::IsNumericToken()
// Purpose : Determines if the next lexeme is a numeric token
	{
	bool lexemeIsNumber = false;
	if (iswdigit(iPkgChar))
		lexemeIsNumber = true;
	else if (iPkgChar == '+' || iPkgChar == '-')
		{
		// we may have a number but we must look ahead one char to be certain

		WCHAR oldChar = iPkgChar;
		DWORD fileOffset=::SetFilePointer(iFileHandle, 0L, NULL, FILE_CURRENT);
		GetNextChar();
		lexemeIsNumber = iswdigit(iPkgChar) != FALSE;
		iPkgChar = oldChar;
		::SetFilePointer(iFileHandle,fileOffset,NULL,FILE_BEGIN);
		}

	return lexemeIsNumber;
	}


void CPackageParser::GetNumericToken()
// Purpose  : Parse a number from the input line
// Inputs   : m_pkgPtr    - The string to parse
//			  pdwNumber  - The output number
	{
	WCHAR temp[MAX_STRING];
	LPWSTR end;
	bool hexString = false;
	DWORD dwBytesRead;
	DWORD fileOffset=::SetFilePointer(iFileHandle, 0L, NULL, FILE_CURRENT);

	temp[0]=iPkgChar;
	if (!::ReadFile(iFileHandle, &temp[1], (MAX_STRING-2)*sizeof(WCHAR), &dwBytesRead, NULL) ||
		dwBytesRead==0)
		throw ErrReadFailed;
	temp[1+dwBytesRead/sizeof(WCHAR)]='\0';
	hexString = (!CompareNString(temp, L"0x", 2) || !CompareNString(&temp[1], L"0x", 2));

	iTokenValue.dwNumber = wcstoul(temp, &end, (hexString) ? 16 : 10);

	if (end==temp) throw ErrReadFailed;
	if (errno==ERANGE)
		throw ErrNumberOutOfRange;
	::SetFilePointer(iFileHandle, fileOffset+(end-temp-1)*sizeof(WCHAR), NULL, FILE_BEGIN);
	GetNextChar();
	}

void CPackageParser::GetNextChar()
	{
	DWORD dwBytesRead;
	if (!::ReadFile(iFileHandle, (LPVOID)&iPkgChar, sizeof(WCHAR), &dwBytesRead, NULL) ||
		dwBytesRead!=sizeof(WCHAR))
		iPkgChar='\0';
	}

void CPackageParser::ValidateVersion(TInt32& major, TInt32 &minor, TInt32 &build)
	{
	if((major > 127) || (minor > 99) || (build > 32767))
		{
		iObserver.DoMsg(L"Warning : The valid version number ranges are : (Major: 0..127) (Minor: 0..99 ) (Build: 0..32,767).");
		}
	}

bool CPackageParser::DoesExist(LPWSTR pszFile, DWORD *pdwSize)
// Purpose  : Attempt to determine whether the file exists (w. or W.out the search path), and gets
//		      it's file size.
// Inputs   : pszFile	-	The file to find (as a UNICODE string)
//			  pdwSize   - store its size here (set to zero if not found)
// Returns  : Yes or No

	{
	bool fFound = false;
	*pdwSize = 0;

	try
		{
		HANDLE hFile = ::MakeSISOpenFile(pszFile, GENERIC_READ, OPEN_EXISTING);
		if (hFile != INVALID_HANDLE_VALUE)
			{
			*pdwSize = ::GetFileSize(hFile, NULL);
			::CloseHandle(hFile);
			fFound = true;
			}
		else
			{
			// If we are using a search directory
			if(iSearchDir[0] != '\0')
				{

				if (wcslen (iSearchDir) + wcslen (pszFile) < PATHMAX - 1)
					{
					wchar_t pszNewPath[PATHMAX];
					wcscpy(pszNewPath, iSearchDir);
					wcscat(pszNewPath, pszFile);
					HANDLE hFile = MakeSISOpenFile(pszNewPath, GENERIC_READ, OPEN_EXISTING);
					if (hFile != INVALID_HANDLE_VALUE)
						{
						*pdwSize = ::GetFileSize(hFile, NULL);
						CloseHandle(hFile);
						wcscpy(pszFile, pszNewPath);
						fFound = true;
						}
					}
				}
			}
		}
	catch (TUtilsException excp)
		{
		// ignore error if writing a stub file
		if (!iMakeStub) throw excp;
		iObserver.DoVerbage(L"warning :file does not exist");
		fFound=true;
		}
	return fFound;
	}

bool CPackageParser::DoesFileExist (std::wstring& aFileName, TUint64& aSize)

	{
	if (aFileName.empty ()) return false;
	bool found = false;
	aSize = 0;

	try
		{
		HANDLE hFile = ::MakeSISOpenFile (aFileName.c_str (), GENERIC_READ, OPEN_EXISTING);
		if (hFile == INVALID_HANDLE_VALUE)
			{
			// If we are using a search directory
			if(iSearchDir[0] != '\0')
				{
				std::wstring newPath (std::wstring (iSearchDir) + aFileName);
				hFile = ::MakeSISOpenFile (newPath.c_str (), GENERIC_READ, OPEN_EXISTING);
				if (hFile != INVALID_HANDLE_VALUE) aFileName = newPath;
				}
			}
		if (hFile != INVALID_HANDLE_VALUE)
			{
			aSize  = GetSizeOfFile(hFile);
			found = true;
			}
		}
	catch (TUtilsException excp)
		{
		// ignore error if writing a stub file
		if (!iMakeStub) throw excp;
		iObserver.DoVerbage(L"warning :file does not exist");
		found=true;
		}
	return found;
	}

TUint32 CPackageParser::Find(wchar_t* aWhat, CSISFileDescription& aFileDesciption)
	{
	TUint32 ret = -1;
	if ( iSrcFiles.find(aWhat) != iSrcFiles.end() )
		{
		aFileDesciption = iSrcFiles[aWhat];
		ret = aFileDesciption.FileIndex();
		}
	return ret;
	}

void CPackageParser::Set(std::wstring& aWhat, CSISFileDescription& aData)
	{
	iSrcFiles[aWhat] = aData;
	}

void CPackageParser::ReportInterpretSisError(TInterpretSisException aException)
	{
	iValidSISFile = false;
	iObserver.AddInterpretSisError(aException);
	}

void CPackageParser::ComputeAndSetHash(CSISFileDescription& aFileDesc)
	{
	const CSISDataUnit& dataUnit = iSISXWriter.DataUnit();
	const CSISFileData& fileData  = dataUnit.FileData(aFileDesc.FileIndex());
	// Compute the hash data for the file. 
	CSISHash tempSisHash;	  	
	TUint8 digest [SHA_DIGEST_LENGTH];
	memset (&digest, 0, SHA_DIGEST_LENGTH);
#ifdef GENERATE_ERRORS
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugHashError))
		{
		for (unsigned index = 0; index < SHA_DIGEST_LENGTH; index++)
			{
			digest [index] = static_cast <TUint8> (rand () & 0xFF);
			}
		}
	else
#endif // GENERATE_ERRORS
		{
		SHA1 (fileData.Data (), fileData.UncompressedSize (), digest);
		}
 	aFileDesc.SetHash(digest, SHA_DIGEST_LENGTH);
	}

