/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* manages type and length for SIS structures
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __HEADER_H__
#define __HEADER_H__


#include <assert.h>

#include "fieldroot.h"
#include "numeric.h"


class CSISHeader : public CSISFieldRoot

	{
private:
	void BigToSmalls (CSISUInt32& aSmall, CSISUInt32& aBig) const;
	static void RawBigToSmalls (const TUint64& aSize, CSISUInt32& aSmall, CSISUInt32& aBig);

protected:
	bool IsBigHeader () const;

public:
	CSISHeader ();
	CSISHeader (const TFieldType aType);
	CSISHeader (const TFieldType aType, const TFieldSize aSize);

public:
	/**
	 * Internalize the class
	 * @param aFile File stream from where data needs to be read.
	 * @param aContainerSize size of the content to be read.
	 * @param aArrayType Type of the array 
	 */
	virtual void Read (TSISStream& aFile, const TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType = CSISFieldRoot::ESISUndefined);
	/**
	 * Skip the file reading for this field. Read pointer will be moved to the
	 * next field to be read.
	 * @param aFile stream for which the data read should be skipped.
	 * @param aContainerSize size of the data to be skipped. 
	 */
	virtual void Skip (TSISStream& aFile, const TFieldSize& aContainerSize) const;
	/**
	 * Externalize the class
	 * @param aFile File stream to which the data needs to be written.
	 * @param aIsArrayElement whether the structure is part of an array or not. 
	 */
	virtual void Write (TSISStream& aFile, const bool aIsArrayElement) const;  
	/**
	 * Retrieves the size of the structure excluding the header.
	 * @param aInsideArray - whether the structure is part of an array or not. 
	 * @return byte count.
	 */
	virtual TFieldSize ByteCount (const bool aInsideArray) const;
	/**
	 * Dump the entire content in hex format into the stream
	 */
	virtual void Dump (std::ostream& aStream, const int aLevel) const;
	/**
	 * Calculates CRC of the content
	 * @param aCRC CRC value of the content
	 * @param aIsArrayElement whether the structure is part of an array or not. 
	 */ 
	virtual void CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const;

	/**
	 * It checks the field type and returns whether the field type is a
	 * known type or not.
	 */
	bool IsKnownType () const;
	/**
	 * @return Data field type
	 */
	CSISFieldRoot::TFieldType DataType () const;
	/**
	 * @param data type
	 */
	CSISFieldRoot::TFieldSize DataSize () const;
	/**
	 * Reads the header and skips the data from the stream
	 * @param aSize size of the field is returned in this variable 
	 * @param aFile File stream.
	 * @param aContainerSize size of the content.
	 * @param aArrayType Type of the array 
	 */
	static TUint32 RawRead (TUint64& aSize, 
							TSISStream& aFile, 
							const TFieldSize& aContainerSize,
							const CSISFieldRoot::TFieldType aArrayType = CSISFieldRoot::ESISUndefined);
	/**
	 * writes the header into the stream
	 * @param aType field type.
	 * @param aSize size of the field.
	 * @param aFile File stream to which the data needs to be written.
	 * @param aIsArrayElement whether the structure is part of an array or not. 
	 */
	static void RawWrite (const TUint32 aType, const TUint64& aSize, TSISStream& aFile, const bool aIsArrayElement);
	/**
	 * @return Number of bytes required to store the length of the field
	 */
	static TFieldSize RawByteCount (const TUint64& aSize, const bool aInsideArray);
	/**
	 * Calculates CRC of the header
	 * @param aType field type.
	 * @param aSize size of the field.
	 * @param aCRC CRC value
	 * @param aIsArrayElement whether the structure is part of an array or not. 
	 */
	static void RawCalculateCrc (const TUint32 aType, const TUint64& aSize, TCRC& aCRC, const bool aIsArrayElement);
	/**
	 * Checks whether the field length is big number (requires 8 bytes) or not (4 bytes)
	 */
	static bool IsBigHeader (const TUint64& aSize);

	/**
	 * Checks whether the next type is an expected type or not.
	 */
	static bool IsNextAsExpected (	TSISStream& aFile, CSISHeader& aHeader,
									const CSISFieldRoot::TFieldType aExpectedType,
									const CSISFieldRoot::TFieldType aArrayType,
									const bool aPeek);

private:
	CSISFieldRoot::TFieldType	iDataType;
	CSISFieldRoot::TFieldSize	iDataSize;

	};




inline CSISHeader::CSISHeader () : 
		iDataType (CSISFieldRoot::ESISUndefined), 
		iDataSize (0)
	{
	}


inline CSISHeader::CSISHeader (const TFieldType aType) :
		iDataType (aType), 
		iDataSize (0)
	{
	}


inline CSISHeader::CSISHeader (const TFieldType aType, const TFieldSize aSize) :
		iDataType (aType), 
		iDataSize (aSize)
	{
	}


inline bool CSISHeader::IsKnownType () const
	{ 
	return	(iDataType > CSISFieldRoot::ESISUndefined) &&
			(iDataType < CSISFieldRoot::ESISUnknown); 
	}


inline CSISFieldRoot::TFieldType CSISHeader::DataType () const
	{
	return iDataType; 
	}


inline CSISFieldRoot::TFieldSize CSISHeader::DataSize () const
	{
	return iDataSize; 
	}

inline void CSISHeader::Skip (TSISStream& aFile, const TFieldSize& aContainerSize) const
	{
	assert (false);	// if you skip the header, you skip the data which tells you how much to skip
	}

#endif // __HEADER_H__
