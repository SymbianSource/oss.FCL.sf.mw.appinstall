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

#ifndef __SISFILEDESCRIPTION_H__
#define __SISFILEDESCRIPTION_H__
#pragma warning( disable : 4786) 

#include "streamreader.h"
#include <algorithm>
#include <map>
#include <string>

class StreamReader;

/**
 * The SISFileDescription class extracts the information of the files in the SISX package.
 * @internalComponent 
 */

class SISFileDescription
{
public:

    SISFileDescription();

	~SISFileDescription();

	/**
	   Extracts the information of the files in the SISX package.
	 */
	void ExtractSISFileDescription(StreamReader& aReader);

	/**
	   Displays the information of the files in the SISX package.
	 */
	void DisplaySISFileDescription(int);

private:

	char* iTarget;
	char* iMimeType;
	unsigned int iOperation;
	unsigned int iOperationOptions;
	unsigned int iAlgorithm;
	char* iHash;
	unsigned int iUncompressedLengthl;
	unsigned int iUncompressedLengthh;
	int iIndex;
	unsigned int iSid;
	map<int, string> iAlgorithms;
};

#endif

