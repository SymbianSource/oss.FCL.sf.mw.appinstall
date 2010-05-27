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

#ifndef	__STREAMREADER_H__
#define	__STREAMREADER_H__

#ifdef DUMPSWICERTSTORETOOL
#include "pfsdump.h"
#include "options.h"
#endif

#ifdef DUMPSWIREGISTRYTOOL
#include "options.h"
#include "dumpswiregistrytool.h"
#include <fstream>
#endif

#include <iostream>
using std::istream;

class Stream;

/**
 * The StreamReader class provides functions which reads 8 bit , 16 bit , 32 bit values and 
 * descriptors from a given stream.
 * @internalComponent 
 */

class StreamReader
{
public:

	StreamReader(istream& aFile);

	StreamReader(const char* aRegFile);

	~StreamReader();

	/**
	  This method determines the length of the file and reads the data as a block into a buffer,
	  also checks whether file is empty .
	  @return	true	If file is empty.
	  */  
	bool Initialise();

	/**
	  Reads 32 bit value from the stream and interprets it as an unsigned int.
	  */
	unsigned int ReadInt32();

	/**
	  Reads 16 bit value from the stream and interprets it as an unsigned short int.
	  */
	unsigned short int ReadInt16();

	/**
	  Reads 8 bit value from the stream and interprets it as an unsigned char.
	  */
	unsigned char ReadInt8();

	/**Overloaded function used to obtain the length and type (ascii or unicode) of a descriptor . 
	   @param	aIsWide				Set,if descriptor is in unicode format.
	   @param	aConstReadPtr		In case where only length of the descriptor has to be determined,
									the pointer should be rewinded back to beginning of the descriptor,which is the 
									case when aConstReadPtr is set.
	   @return	Length of the descriptor.
	  */
	int GetDescriptorInfo(bool& aIsWide, const bool& aConstReadPtr = false);

	/**
	  Reads the descriptor.
	  */

	char* ReadDescriptor();
	
	/**
	  This function reads the first 8 bits of the descriptor and returns the cardinality value, 
	  based on which the length of string can be obtained. After the peek function reads the 
	  cardinality value it will rewind back the position at the beginning of descriptor to be read.
	 */  
	unsigned char Peek();	

	
	/**
	  This method creates a target descriptor to hold the resultant utf8 descriptor and 
	  calls method which converts utf16 descriptor to utf8 descriptor.
	  @param	aSource		utf16 encoded descriptor.
	  @param	aSrcLength	Length of the utf16 encoded descriptor.	
	  
	 */
	char* UTF16toUTF8Convert(wchar_t* aSource, int aSrcLength);


private:

	istream* iFileStream;
	//streamOwnership is to differentiate between dumpswicertstoretool and  dumpswiregistrytool.
	//true for dumpswiregistrytool.
	char* iBuf;
	char* iCurrentPos;
	int iLength;
	bool iStreamOwnership;
};
#endif
