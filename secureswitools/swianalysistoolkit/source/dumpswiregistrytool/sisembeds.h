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

#ifndef __SISEMBEDS_H__
#define __SISEMBEDS_H__
#pragma warning( disable : 4786) 

#include "streamreader.h"

class StreamReader;

/**
 * The SISEmbeds class extracts the registry entry of all the embedded packages.
 * @internalComponent 
 */

class SISEmbeds
{
public:

	SISEmbeds();

	~SISEmbeds();

	/**
	   Extracts the registry entry (package uid ,package name and vendor name)
	   of all the embedded packages.
	 */
	void ExtractEmbeds(StreamReader& aReader);

	/**
	   Displays the registry entry (package uid ,package name and vendor name)
	   of all the embedded packages.
	 */
	void DisplayEmbeddedInfo();

private:

	unsigned int iEmbeddedPkgUid;
	char* iEmbeddedPkgName;
	char* iEmbeddedVendorName;
	int iEmbeddedPkgIndex;

};

#endif

