/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
* These streams use memory for storage. If data becomes huge, they will need amending to use temporary files.
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __BASEFILE_H__
#define __BASEFILE_H__

#include "utility_interface.h"


#include <sstream>

#include "basetype.h"



class TSISStream
	{
public:
	typedef unsigned int	size_type;
	typedef int				pos_type;

public:
	/**
	 * Default constructor
	 */
	TSISStream ();
	/**
	 * Destructor
	 */
	~TSISStream ();
	
public:
	/**
	 * Read the content from the stream.
	 * @param aBuffer buffer to which data needs to be read.
	 * @param aSize size of data to be read.
	 */
	void read (TUint8* aBuffer, const size_type aSize);
	/**
	 * Write the content into the stream
	 * @param aBuffer from which the data needs to be written to the stream
	 * @param aSize amount of data needs to be written.
	 */
	void write (const TUint8* aBuffer, const size_type aSize);

	/**
	 * @return returns the current stream pointer.
	 */
	pos_type tell () const;
	/**
	 * Seek the stream pointer.
	 * @param aPos offset by which the pointer needs to be moved.
	 * @param aRel From where the offset needs to be moved.
	 */
	void seek (const pos_type aPos, const std::ios_base::seekdir aRel = std::ios_base::beg);

	/**
	 * @return stream length
	 */
	size_type length () const;

	/**
	 * Write the content of the stream into the file.
	 * @param aFile file handle.
	 */
	bool exportfile (HANDLE aFile);
	/**
	 * Read the content from the file and write it into the stream
	 * @param aFile file handle
	 * @param filesize file size to be read.
	 */
	bool import (HANDLE aFile, TUint64* filesize = NULL);

	/**
	 * Reset all read-write pointers
	 */
	void reset ();
	/**
	 * @return buffer pointed by the stream.
	 */
	const TUint8* data () const;

	/**
	 * @return Maximum buffer size allocated to this stream
	 */
	static TUint64 MaxBufferSize ();

private:
	void reserve (const size_type aSize);

private:
	TUint8*				iBuffer;
	size_type			iSize;
	size_type			iDataLength;
	pos_type			iOffset;
	static size_type 	iChunk;
	};




inline TSISStream::pos_type TSISStream::tell () const
	{
	return iOffset;
	}

inline TSISStream::size_type TSISStream::length () const
	{
	return iDataLength;
	}

inline const TUint8* TSISStream::data () const
	{
	return iBuffer;
	}

inline TUint64 TSISStream::MaxBufferSize ()
	{
	return UINT_MAX;
	}
	




inline TSISStream& operator >> (TSISStream& aFile, TUint8& aValue)
	{
	aFile.read (reinterpret_cast <TUint8*> (&aValue), sizeof (TUint8));
	return aFile;
	}

inline TSISStream& operator << (TSISStream& aFile, const TUint8 aValue)
	{
	aFile.write (reinterpret_cast <const TUint8*> (&aValue), sizeof (TUint8));
	return aFile;
	}

inline TSISStream& operator >> (TSISStream& aFile, TUint16& aValue)
	{
	aFile.read (reinterpret_cast <TUint8*> (&aValue), sizeof (TUint16));
	return aFile;
	}

inline TSISStream& operator << (TSISStream& aFile, const TUint16 aValue)
	{
	aFile.write (reinterpret_cast <const TUint8*> (&aValue), sizeof (TUint16));
	return aFile;
	}

inline TSISStream& operator >> (TSISStream& aFile, TUint32& aValue)
	{
	aFile.read (reinterpret_cast <TUint8*> (&aValue), sizeof (TUint32));
	return aFile;
	}

inline TSISStream& operator << (TSISStream& aFile, const TUint32 aValue)
	{
	aFile.write (reinterpret_cast <const TUint8*> (&aValue), sizeof (TUint32));
	return aFile;
	}

inline TSISStream& operator >> (TSISStream& aFile, TInt32& aValue)
	{
	aFile.read (reinterpret_cast <TUint8*> (&aValue), sizeof (TInt32));
	return aFile;
	}

inline TSISStream& operator << (TSISStream& aFile, const TInt32 aValue)
	{
	aFile.write (reinterpret_cast <const TUint8*> (&aValue), sizeof (TInt32));
	return aFile;
	}

inline TSISStream& operator >> (TSISStream& aFile, TUint64& aValue)
	{
	aFile.read (reinterpret_cast <TUint8*> (&aValue), sizeof (TUint64));
	return aFile;
	}

inline TSISStream& operator << (TSISStream& aFile, const TUint64 aValue)
	{
	aFile.write (reinterpret_cast <const TUint8*> (&aValue), sizeof (TUint64));
	return aFile;
	}



#endif // __BASEFILE_H__

