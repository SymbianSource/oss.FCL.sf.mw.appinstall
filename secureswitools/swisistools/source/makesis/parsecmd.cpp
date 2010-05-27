/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* handles parsing of makesis command line args
* INCLUDES
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#include "utils.h"
#include "utility_interface.h"
#include "parsecmd.h"
#include "utility.h"
#include "siscontents.h"

// ===========================================================================
// CParseCmd
// Responsable for processing and maintaining the command line options
// ===========================================================================

CParseCmd::CParseCmd ()
:iOptions (0L),
iShowSyntax (false),
iShowSSL (false),
iInterpretSisReport (false)
	{
	iDir   [0] = '\0';
	iSource[0] = '\0';
	iTarget[0] = '\0';
	}

BOOL CParseCmd::ParseCommandLine(int argc, _TCHAR *argv[])
// Purpose  : Proceses the command line, and options
// Inputs   : argc, argv - command line as passed to the process
	{
	// Test the number of arguments
	if(argc == 1)
		throw ErrInsufficientArgs;
	
	int wCount = 1;
	// Parse the command line
	// Check for options
	while(wCount < argc)
		{
		if((argv[wCount][0] != '-') && ((argv[wCount][0] != '/') || (argv[wCount][0] != '\\')))
			break;
		else
			{
			if(argv[wCount][1] == '\0')
				throw ErrBadCommandFlag;

			// cope with multiple arguments following the '-' or '/'
			int wCharacter = 1;
			while (argv[wCount][wCharacter] !='\0')
				{
				switch(argv[wCount][wCharacter])
					{
					case 'a' : // Verbose
					case 'A' : 
#if defined (_DEBUG)
						iOptions |= EOptDump;
#endif // _DEBUG
						break;
#ifdef GENERATE_ERRORS
					case 'b' :
					case 'B' :
						{
						for (int index = wCharacter+1; argv [wCount] [index]; index++)
							{
							switch(argv [wCount] [index])
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
								case 'k' :
								case 'K' :
									CSISFieldRoot::SetBug (CSISFieldRoot::EBugUnknownData);
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
								case 'p' :
								case 'P' :
									CSISFieldRoot::SetBug (CSISFieldRoot::EBugEmptyCaps);
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
					case 'c' : // Report InterpretSis errors
					case 'C' :
						iInterpretSisReport = true;
						break;
					case 'd' : // Search directory
					case 'D' : iOptions |= EOptDirectory;
						SetDirectory(&argv[wCount][wCharacter+1]);
						// Change next char to a NUL so we don't process the directory as if it were an option string.
						argv[wCount][wCharacter+1] = '\0';
						break;
					case 'h' : // Help. i.e Show Syntax	
					case 'H' : 
						iShowSyntax = true;
						return TRUE;
					case 'i' :
					case 'I' :
						iShowSSL = true;
						return TRUE;
					case 'p' : // Use Password for Private Key Decryption	
					case 'P' : iOptions |= EOptPassword;
						break;
#ifdef GENERATE_ERRORS
					case 'q' :
					case 'Q' :
						CSISFieldRoot::SetBugStart (atoi (wstring2string (&argv [wCount] [wCharacter+1]).c_str ()));
						wCharacter++;
						break;
					case 'r' :
					case 'R' :
						CSISFieldRoot::SetBugRepeat (atoi (wstring2string (&argv [wCount] [wCharacter+1]).c_str ()));
						wCharacter++;
						break;
#endif // GENERATE_ERRORS
					case 's' :
					case 'S' :
						iOptions |= EOptMakeStub;
						CSISContents::SetStub (CSISContents::EStubROM);
						break;
					case 'v' : // Verbose
					case 'V' : iOptions |= EOptVerbose;
						break;
					case 'x' :
					case 'X' :
						{
						for (int index = wCharacter+1; argv [wCount][index]; index++)
							{
							switch(argv[wCount][index])
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
					default  : // Uh-Oh...
						throw ErrBadCommandFlag;
					}
				wCharacter++;
				}
			wCount++;
			}
		}
	// Get the source and (optionally) target filenames
	if(wCount < argc)
		{
		SetSource(argv[wCount++]);
		}
	else
		throw ErrNoSourceFile;
	
	if(wCount < argc)
		{
		SetTarget(argv[wCount]);
		}
	
	return TRUE;
	}

void CParseCmd::SetDirectory(const wchar_t* aPath)
// Purpose  : Sets the search directory
// Inputs   : pszCmdLine - the path
	{
	wcsncpy(iDir, aPath, PATHMAX - 1);
	DWORD len=wcslen(iDir);
	wchar_t *pBuffer = iDir;
	wchar_t *pCurrent = pBuffer;

	while (pBuffer && *pBuffer && (pCurrent = wcschr(pBuffer,L'\\')) != NULL)
		{
		*pCurrent = L'/';
		pBuffer = pCurrent + 1;
		} 
	if(len>0 && iDir[len - 1] != '/')
		{
		iDir[len] = '/';
		iDir[len+1] = '\0';
		}
	}

void CParseCmd::SetTarget(const wchar_t* aTarget)
// Purpose  : Sets the target filename
// Inputs   : aTarget - The target filename
	{
	//Conversion of the filename consisting backward slashes with forward slashes
	const wchar_t *pBuffer = aTarget;
	wchar_t *pCurrent = const_cast<wchar_t*>(pBuffer);

	while (pBuffer && *pBuffer && (pCurrent = wcschr(pBuffer,L'\\')) != NULL)
		{
		*pCurrent = L'/';
		pBuffer = pCurrent + 1;
		} 
	// Check that the filename is valid
	if (wcslen(aTarget) < wcslen(DESTFILE) + 1) // Check that we have at least one character for the filename, and 4 for the extension: ".sis"
		throw ErrBadTargetFile;
	else
		{
		if(CompareTwoString((wchar_t*)&aTarget[wcslen(aTarget) - (wcslen(DESTFILE))],DESTFILE) != 0 )
			throw ErrBadTargetFile;
		else
			{
			wcsncpy(iTarget, aTarget, PATHMAX-1);
			iTarget [PATHMAX - 1] = 0;	
			}
		}
	}

void CParseCmd::SetSource(const wchar_t* aSource)
// Purpose  : Sets the source filename
// Inputs   : aSource - The source filename
	{
	// Check that the filename is valid
	if (wcslen(aSource) <= wcslen(SOURCEFILE))
		throw ErrBadSourceFile;
	else
		{
		if(CompareTwoString((wchar_t*)&aSource[wcslen(aSource) - (wcslen(SOURCEFILE))],SOURCEFILE) != 0 )
			throw ErrBadSourceFile;
		else
			{
			wcsncpy(iSource, aSource, PATHMAX-1);
			iTarget [PATHMAX - 1] = 0;
			}
		}
	}

const wchar_t* CParseCmd::TargetFile()
// Returns the destination file. If none was set then it mugnes the source filename into
// something acceptable
	{
	// Has a destination file been set ?
	if(wcslen(iTarget) < 1)
		{
		wcscpy(iTarget, iSource);
		iTarget[wcslen(iTarget) - (wcslen(SOURCEFILE))] = '\0';
		wcscat(iTarget, DESTFILE);
		}
	return iTarget;
	}
