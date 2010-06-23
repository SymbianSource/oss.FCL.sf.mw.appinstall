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

#ifndef __DUMPSWIREGISTRYTOOL_H__
#define __DUMPSWIREGISTRYTOOL_H__

#pragma warning( disable : 4786) 

#include "options.h"
#include "streamreader.h"
#include "registrytoken.h"
#include "registrypackage.h"
#include "siscontrollerinfo.h"
#include "sisproperty.h"
#include "sisdependency.h"
#include "sisembeds.h"
#include "sisfiledescription.h"
#include "osinterface.h"

#include <stdlib.h>
#include <string>
#include <algorithm>
#include <map>
#include <fstream>
#include <iostream>

class RegistryToken;
class StreamReader;
class Options;
class RegistryPackage;
class SISControllerInfo;

/**
 * The DumpSWIRegistryTool class provides functions to extract the registry contents.
 * @internalComponent 
 */

class DumpRegistryTool
{
public:

	DumpRegistryTool();

	~DumpRegistryTool();
	/**
	  Calls the ExtractFilesFromDirectory(),which provides a vector containing all the .reg files present in the 
	  package directory specified,and iterates this vector one by one and calls ExtractRegistry() ,to extract the information
	  from that particular .reg file.
	 */
	void Run(const Options& aOptions);

private:
	
	/**
	  Extracts the registry file contents.
	  @param	aFile	.reg file .
	  @return	true	If registry extraction is successful.This status is used to determine whether
	 					the registry file is empty or not.
	 */
	bool ExtractRegistry(const char* aFile , const Options& aOptions);

	/**
	  Extracts the top level information about a package namely it's install type , whether 
	  it is ROM Based , trust status , preinstalled status , signed status.
	 */
	void ExtractPackageInformation(StreamReader& aReader);

	/**
	  Extracts the information about sis dependencies,sis embeds, sis file description,sis properties , 
	  validation status,revocation status and installed chain indices.
	 */
	void ExtractControllerInformation(StreamReader& aReader);

	/**
	  Checks whether the user specified registry entries(Pkg Name , Pkg UID and Vendor Name) matches
	  the one present in the registry file.
	  @param	aUserSpecifiedData	Registry entries(Pkg Name , Pkg UID and Vendor Name) specified
	 								on command line.
	  @param	aFileSpecifiedData	The package uid,name and vendor name as present in the registry file.
	  @return	1					If the user and file data matches.
	 */
	int CompareUserAndFileSpecifiedData(const string& aUserSpecifiedData , const string& aFileData); 

	/**
	  Displays the top level information about a package namely it's install type , whether 
	  it is ROM Based , trust status , preinstalled status , signed status.
	  */
	void DisplayPackageInformation();
    
private:
	
	bool iRegistryEntryValid ;
	ifstream iFile ; 
	unsigned short int iFileMajorVersion ;
	unsigned short int iFileMinorVersion ;
	char* iPkgVendorLocalizedName ;
	unsigned int iPkgInstallType ;
	unsigned int iRomBasedPkg ;				
	unsigned int iPkgPreinstalledStatus ;	
	unsigned int iPkgSignedStatus;			
	unsigned int iPkgTrust ;				
	unsigned int iNotUsed ; 
	unsigned int iTimeStampl ;
	unsigned int iTimeStamph ;
	map<int,string> iInstallType ;
	map<int,string> iPackageTrust ;
	map<int,string> iValidationStatus ;
	map<int,string> iRevocationStatus ;
};

extern DumpRegistryTool tool;
#endif

