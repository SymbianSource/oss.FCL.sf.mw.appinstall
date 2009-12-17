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

#ifndef __SISDEPENDENCY_H__
#define __SISDEPENDENCY_H__
#pragma warning( disable : 4786) 

#include "streamreader.h"
#include "dumpswiregistrytool.h"

class StreamReader;

/**
 * The SISDependency class extracts the version range information of the SISX packages 
 * which needs to be installed to satisy this dependency.
 * @internalComponent 
 */

class SISDependency
{
public:

	SISDependency();

	~SISDependency();

	/**
	   Extracts the version range information of the SISX packages 
	   which needs to be installed to satisy this dependency.
	 */
	void ExtractDependency(StreamReader& aReader);


	/**
	   Displays the version range information of the SISX packages 
	   which needs to be installed to satisy this dependency.
	 */
	void DisplayDependencyInfo(); 

private:

	unsigned int iDependencyUid;
	int iFromMajorVersion;
	int iFromMinorVersion;
	int iFromBuild;
	int iToMajorVersion;
	int iToMinorVersion;
	int iToBuild;
};

#endif