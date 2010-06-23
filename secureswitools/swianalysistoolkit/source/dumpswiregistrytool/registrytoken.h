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

#ifndef __REGISTRYTOKEN_H__
#define __REGISTRYTOKEN_H__

#pragma warning( disable : 4786) 

#include "streamreader.h"
#include "dumpswiregistrytool.h"
#include "registrypackage.h"
#include "siscontrollerinfo.h"
#include <vector>
#include <map>

class StreamReader;
class RegistryPackage;
class SISControllerInfo;

/**
 * The RegistryToken class extracts the package version,the language supported by the package ,the selected
 * drives information and display the same.
 * /@internalComponent 
 */

class RegistryToken
{
public:

	RegistryToken();

	~RegistryToken();

	/**
	   Extracts the package version,the language supported by the package ,the selected
	   drives information .
	 */
	void ExtractRegistryToken(StreamReader& aReader, RegistryPackage& aPackage);

	/**
	   Displays the package version,the language supported by the package ,the selected
	   drives information .
	 */
	void DisplayRegistryToken ();

	/**
	   Returns the siscontroller count.
	 */
	unsigned int GetControllerCount ()  const { return iSISControllerCount ;}

	SISControllerInfo* ControllerInformation () const { return iSISControllerInfo ;}
    
private:

	SISControllerInfo* iSISControllerInfo;
	unsigned int iDrives;
	unsigned int iCompletelyPresent;
	unsigned int iSidCount;
	unsigned int iSISControllerCount;
	unsigned int iMajor;
	unsigned int iMinor;
	unsigned int iBuild;
	unsigned int iLanguage;
	unsigned int iSelectedDrive;
	unsigned int iUnused1;
	unsigned int iUnused2;
	map <int,string> iLanguages;
};

#endif