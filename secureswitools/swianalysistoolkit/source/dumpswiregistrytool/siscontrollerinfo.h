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

#ifndef __SISCONTROLLERINFO_H__
#define __SISCONTROLLERINFO_H__
#pragma warning( disable : 4786) 

#include "streamreader.h"
#include "dumpswiregistrytool.h"
#include "registrytoken.h"
#include <map>
#include <vector>

class StreamReader;

/**
 * The SISControllerInfo class extracts the sis contoller information (major,minor version and algorithm)and 
 * displays the the same .
 * @internalComponent 
 */
class SISControllerInfo
{
public:

    SISControllerInfo();

	~SISControllerInfo();
	
	/**
	   Extracts the SISContoller information (Major , Minor version and Algorithm).
	 */
	void ExtractControllerInfo(StreamReader& aReader);

	/**
	   Displays the SISContoller information (Major , Minor version and Algorithm).
	 */
	void DisplayControllerInfo(int);  

private:

	unsigned int iMajor;
	unsigned int iMinor;
	unsigned int iBuild;
	unsigned int iOffset;
	unsigned int iAlgorithmType ;
	map<int,string> iAlgorithms;
	
};

#endif;
