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

#include <string>
#include <vector>
#include <iomanip>
using namespace std;

int const KMajorVersion = 1;
int const KMinorVersion = 0;
int const KLength = 8;
int const KRadix = 16;
 
/**
   Exceptions raised during program execution.
   @internalComponent
 */

enum Exceptions
	{
	EUnknownOption,
	EMissingRegistryEntry,
	EMissingPkgUID,
	EInvalidOption,
	EInvalidPackageUID,
	ESetEnvironmentVariable,
	EInvalidFile
	};

/**
   Contains the command line options .
   @internalComponent
 */

class Options
{
public:

	Options(int argc, char** argv);

	~Options();

	/**
	   Displays the exception messages .
	   @param	aErr	Exception values as defined in the enum.
	 */
	static void			DisplayError (const Exceptions& aErr);

	/**
	   Displays the help message .
	 */
	static void			DisplayUsage ();

	/**
	   Returns true if -r option is specified .
	 */
	bool				IsRegistryEntrySpecified() const  {return iRegistryEntrySpecified; }

	/**
	  Returns true if -p option is specified.
	 */
	bool				IsPkgUIDSpecified() const { return iPkgUIDSpecified; }

	/**
	   Returns Package UID as specified on command line .
	 */
	const string&			PkgUID ()  const { return iPkgUID ; } 

	/**
	   Returns Registry File Path .
	 */
	const string&			RegistryFilePath () const { return iRegistryFilePath ; }

	/**
	   Returns Package Name as specified on command line .
	 */
	const string&			PkgName ()  const { return iPkgName; } 

	/**
	   Returns Vendor Name as specified on command line .
	 */
	 const string&			VendorName() const  { return iVendorName; } 

	static const char*	CommandName ()  { return "DumpSWIRegistryTool"; }
	
private:

	/**
	   Displays the version of the tool .
	 */
	void				DisplayVersion ();

	/**
	   Checks for any command line exceptions .
	 */
	void				CheckCmdLineExceptions() ;

private:

	bool				iVersion;
	bool				iHelpFlag;
	bool				iPkgUIDSpecified;
	bool				iRegistryEntrySpecified;
	string				iPkgUID;
	string				iPkgName;
	string				iVendorName;
	string				iRegistryFilePath;
};

#endif	
