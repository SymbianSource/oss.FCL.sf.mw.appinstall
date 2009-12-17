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

#ifndef __SISPROPERTY_H__
#define __SISPROPERTY_H__
#pragma warning( disable : 4786) 

#include "streamreader.h"

/**
 * The SISProperty class extracts the properties of SISX package(key,value pair associated with SISX package)
 * @internalComponent 
 */

class SISProperty
{
public:

    SISProperty();

	~SISProperty();

	/**
	   Extracts the properties of SISX package(key,value pair associated with SISX package).
	 */
	void ExtractSISProperty(StreamReader& aReader);

	/**
	   Displays the properties of SISX package(key,value pair associated with SISX package).
	 */
	void DisplaySISProperty();

private:

	int iKey;
	int iValue;
};

#endif