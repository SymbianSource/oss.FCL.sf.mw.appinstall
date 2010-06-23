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
#pragma warning( disable : 4786) 

#include <iomanip>
#include <string>
#include <vector>
#include <iostream>
#include <windows.h>
#include <Shlwapi.h>
#include <math.h>

int const KMajorVersion = 1;
int const KMinorVersion = 0;

typedef std::vector <std::string> StringVector;

/**
 * Exceptions raised while processing the command line
 * @internalComponent
 */
enum Exceptions
	{
	EUnknownOption ,
	EMissingDATFile ,
	ESupportingExeError ,
	EMissingSISFile ,
	EUnableToOpenFile ,
	EErrorExecutingSupportingExeDumpsis ,
	EErrorExecutingSupportingExeSignsis ,
	EErrorExecutingSupportingDumpSwiCertstoreExe,
	ECorruptCCIFile ,
	ENotADerBitString ,
	ENotADerSequence ,
	ENotADerInteger ,
	ENotADerUtf8String
	};

/**
 * Contains the command line options
 * @internalComponent
 */
class Options
{
public:

	Options(int argc, char** argv);

	~Options();

	static void					DisplayError (const Exceptions& aErr);

	static void					DisplayUsage ();

	int							UserCapabilities() const { return iUserCapabilities; }

	const std::string&			SISFile () const { return iSisFile ; }

	const std::string&			CertstoreFile () const { return iCertstore ; }

	static const char*			CommandName () { return "DumpInstallFileStatus"; }

private:

	void	DisplayVersion () const ;

	/**
	  Converts the user capabilities specified on the commnad line into an equivalent integer.
	  @param	UserCapabilities	User Grantable Capabilities on command line.
	  @return	integer equivalent of the user specified capabilities.
	 */
	const int		ConvertUserCapsToInt(StringVector& UserCapabilities);

private:

	bool						iVersion;
	bool						iHelpFlag;
	bool						iDATFileSpecified;
	bool						iSISFileSpecified;
	bool						iDirectorySpecified;
	int							iUserCapabilities;
	std::string					iDATFile;
	std::string					iSisFile;
	std::string					iCertstore;
};

#endif	


