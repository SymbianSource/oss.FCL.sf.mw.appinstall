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

#include "launchprocess.h"
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <windows.h>
#include <algorithm>


int const KMajorVersion = 1 ;
int const KMinorVersion = 0 ; 

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
  Contains the command line options
  @internalComponent
 */
class Options
{
public:

	Options(int argc, char** argv);
	~Options();
	static void			DisplayUsage ();
	bool				List () const { return iList; }
	static void			DisplayError( const Exceptions& aError);
	std::string			SISFile () const { return iSISFile; }
	bool				Version () const { return iVersion; }
	std::string			CertstoreFile () const {return iCertstore ; }
	static const char*	CommandName () { return "DumpChainValidityTool"; }
	
private:

	void				DisplayVersion () const;
	void				CheckCmdLineExceptions();

	
private:

	bool						iList;
	bool						iVersion;
	bool						iHelpFlag;
	bool						iDATFileSpecified;
	bool						iSISFileSpecified;
	bool						iDirectorySpecified;
	std::string					iDATFile;
	std::string					iCertstore;
	std::string					iSISFile;
};

#endif	
