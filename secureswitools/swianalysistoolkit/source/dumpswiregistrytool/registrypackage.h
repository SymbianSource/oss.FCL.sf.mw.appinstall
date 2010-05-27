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

#ifndef __REGISTRYPACKAGE_H__
#define __REGISTRYPACKAGE_H__

#pragma warning( disable : 4786) 

#include "streamreader.h"
#include "dumpswiregistrytool.h"

class StreamReader ;

/**
 * The RegistryPackage class extracts the package uid , package name and vendor name
 * and displays the same.
 * @internalComponent 
 */

class RegistryPackage
{
public:

	RegistryPackage() ;

	~RegistryPackage() ;

	/**
	   Extracts the package uid , package name , vendor name from the registry file .
	 */
	void ExtractPackage(StreamReader& aReader);

	/**
	   Returns Package UID as read from the registry file .
	 */
	unsigned int	PkgUID ()  const { return iUid ; }

	/**
	   Returns Package Name as read from the registry file .
	 */
	char*			PackageName ()  const { return iPkgName ; }

	/**
	   Returns Vendor Name as read from the registry file .
	 */
	char*			Vendor () const  { return iVendorName ; }

	/**
	   Returns Package Index as read from the registry file .
	 */
	unsigned int 	Index () const { return iPkgIndex ; }

	/**
	   Displays the Package UID , Package Name and Vendor Name .
	 */
	void DisplayPackage() ;
  
private:

	unsigned int iUid ;
	char* iPkgName ; 
	char* iVendorName ;
	unsigned int iPkgIndex ;
};

#endif