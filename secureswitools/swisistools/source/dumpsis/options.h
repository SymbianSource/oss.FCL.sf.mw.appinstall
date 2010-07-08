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


/**
 @file
 @internalComponent
 @released
*/

#ifndef	__OPTIONS_H__
#define	__OPTIONS_H__

#include <iomanip>

using std::ostream;

int const MajorVersion=1;
int const MinorVersion=3;
int const BuildVersion=0;
/**
Exceptions raised while processing the command line
@internalComponent
*/
enum CmdLineException
	{
	ECmdLineUnknownOption,
	ECmdLineMissingSIS,
	ECmdLineMultipleSIS,
	ECmdLineNoDirArgument,
	ECmdLineDirIsFile,
	ECmdLineNoPermission,
	ECmdLineInvalidDir,
	ECmdLineOtherDirFailure,
	ECmdLineMissingParamFile,
	ECmdLineInvalidSystemDrive,
	ECmdLineNoSisArgument,
	ECmdLineMissingParams
	};

/**
Contains the command line options
@internalComponent
*/
class Options
	{
public:
	Options(int argc, wchar_t** argv);

	static void			DisplayError (CmdLineException err);
	static void			DisplayUsage ();
	static void			DisplayVersion ();

	bool				Verbose () const { return iVerboseFlag; }
	bool				Version () const { return iVersion; }
	bool				ExtractFiles () const { return iExtractFilesFlag; }
	bool				PauseOnExit () const { return iPauseOnExit; }

	/**
	Return the iList exe flag to which is set to true when -l is specified 
	as command line option
	@return Flag status of list option
	*/
	bool			Listexe() const { return iList; }

	/**
	Return the iCreateECI flag to which is set to true when -l -y is specified 
	as command line option
	@return Flag status of createECI
	*/
	bool			CreateECI() const { return iCreateECI; }
	

	static const char*	CommandName () { return "DumpSIS"; }

	const std::wstring&	SISFileName () const { return iSISFileName; }
	const std::wstring&	ExtractDirectory() { return iExtractDirectory; }

private:
	void GetExtractDir();

private:
	bool				iVerboseFlag;
	bool				iVersion;
	bool				iHelpFlag;
	bool				iExtractFilesFlag;
	bool				iPauseOnExit;
	bool				iList;
	bool				iCreateECI;
	std::wstring		iSISFileName;
	std::wstring		iExtractDirectory;
	};

#endif	/* __OPTIONS_H__ */
