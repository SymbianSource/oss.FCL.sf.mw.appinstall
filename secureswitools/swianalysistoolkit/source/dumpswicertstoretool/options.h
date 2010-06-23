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


/**
 @file 
 @internalComponent 
*/

#ifndef	__OPTIONS_H__
#define	__OPTIONS_H__
#pragma warning(disable: 4786)

#include <iomanip>
#include <string>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <Shlwapi.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int const KMajorVersion = 1;
int const KMinorVersion = 0;
typedef std::vector <std::string> StringVector ; 

/**
 * Exceptions raised while processing the command line
 * @internalComponent
 */
enum Exceptions
	{
	EUnknownOption = 1,
	EInvalidFile,
	ECannotOpenFile,
	ENotAPermanentFileStore,
	EInvalidCertificateChain,
	};

/**
 * Contains the command line options
 * @internalComponent
 */
class Options
{
public:

	Options(int argc, char** argv );

	~Options();

	static void			DisplayError (Exceptions aErr);

	static void			DisplayUsage ();

	bool				GenerateCertificate()  const { return iCertificate ; }

	bool				CBasedWritableCertstore()  const {  return iCBasedCertstore ; }

	bool				ROMCertstore()  const {  return iROMCertstore ;}

	bool				CreateCCIFile()  const { return iCreateCCI; }
	
	bool				CreateDetailCCIFile()  const { return iCreateDetailCCI; }

	const string&		ROMCertstoreFileName ()  const { return iROMCertstoreFileName; }

	const std::vector<std::string>& GetDirectoryFiles() const { return iDirFiles; }

private:

	void				DisplayVersion ();

	static const char*	CommandName ()  { return "DumpSWICertstoreTool"; }
	
private:
	
	bool						iVersion;
	bool						iHelpFlag;
	bool						iROMCertstore;
	bool						iCBasedCertstore;
	int							iNoOfDirectories;
	bool						iCertificate;
	bool						iCreateCCI;
	bool						iCreateDetailCCI;
	string						iROMCertstoreFileName;
	std::vector<std::string>	iDirFiles;
};
#endif	
