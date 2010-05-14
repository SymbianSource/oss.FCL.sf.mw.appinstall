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

#ifndef __DATALOCATOR_H_
#define __DATALOCATOR_H_

#include <strstream>
#include "pfsdump.h"

class Stream;

/**
 * The DataLocator Class provides functions which locates and provides the infostream (containing 
 * the metadata of the certificates) present in the swicertstore.
 * @internalComponent 
 */

class DataLocator
{
private:

	strstream* iStrStream;
	char*	   iBuffer;

public:

	DataLocator(Stream& aStream, ifstream& aFile);

	~DataLocator();
	
	/**
	  Returns the infostream(containing metadata for instance capabilities , trust status, manadatory status 
	  etc) of the certificates in the swicertstore.
	 */
	istream& GetStream() const {return *iStrStream;}
};

#endif //__DATALOCATOR_H_