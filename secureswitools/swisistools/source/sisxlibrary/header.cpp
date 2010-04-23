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
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#include "header.h"
#include "utility.h"



static const TUint32 KBigBit			= 0x80000000;
static const TUint32 KSmallMask			= ~KBigBit;
static const unsigned short KSizeShift	= 31;
static const TUint32 KHalfMask			= 0xFFFFFFFF;


void CSISHeader::BigToSmalls (CSISUInt32& aSmall, CSISUInt32& aBig) const
	{
#ifdef GENERATE_ERRORS
	assert (IsBigHeader () || CSISFieldRoot::IsBugSet (CSISFieldRoot::EBug32As64));
#else // GENERATE_ERRORS
	assert (IsBigHeader ());
#endif // GENERATE_ERRORS

	RawBigToSmalls (iDataSize, aSmall, aBig);
	}


bool CSISHeader::IsBigHeader () const
	{
	return (iDataSize >= KBigBit);
	}

bool CSISHeader::IsBigHeader (const TUint64& aSize)
	{
	return (aSize >= KBigBit);
	}

void CSISHeader::Read (TSISStream& aFile, const TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType)
	{
	iDataType = static_cast <CSISFieldRoot::TFieldType> (
				RawRead (iDataSize, aFile, aContainerSize, aArrayType));
	}

void CSISHeader::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
	RawWrite (static_cast <TUint32> (iDataType), iDataSize, aFile, aIsArrayElement);
	}
	
CSISHeader::TFieldSize CSISHeader::ByteCount (const bool aInsideArray) const
	{
	return RawByteCount (iDataSize, aInsideArray);
	}

void CSISHeader::Dump (std::ostream& aStream, const int aLevel) const
	{
	aStream << "|" << iDataType << "," << iDataSize << "|";
	}

void CSISHeader::CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const
	{
	RawCalculateCrc (iDataType, iDataSize, aCRC, aIsArrayElement);
	}


void CSISHeader::RawBigToSmalls (const TUint64& aSize, CSISUInt32& aSmall, CSISUInt32& aBig)
	{
#ifdef GENERATE_ERRORS
	assert ((aSize >= KBigBit) || CSISFieldRoot::IsBugSet (CSISFieldRoot::EBug32As64));
#else
	assert (aSize >= KBigBit);
#endif		
				// this isn't a big number
	aSmall = static_cast <TUint32> ((aSize & KSmallMask) | KBigBit);
	aBig = static_cast <TUint32> ((aSize >> KSizeShift) & KHalfMask);
	}


TUint32 CSISHeader::RawRead (	TUint64& aSize, 
								TSISStream& aFile, 
								const TFieldSize& aContainerSize,
								const CSISFieldRoot::TFieldType aArrayType)
	{
	TUint32 type = static_cast <TUint32> (aArrayType);
	for (;;)
		{
		ReadFiller (aFile);
		if (aArrayType == CSISFieldRoot::ESISUndefined)
			{
			aFile >> type;
			}
		CSISUInt32 smaller;
		smaller.Read (aFile, aContainerSize, aArrayType);
		if ((smaller & KBigBit) == 0) 
			{
			aSize = smaller;
			}
		else
			{
			smaller = smaller & KSmallMask;
			CSISUInt32 bigger;
			bigger.Read (aFile, aContainerSize, aArrayType);
			aSize = smaller + (bigger << KSizeShift);
			}
		if (type < CSISFieldRoot::ESISUnknown)
			{
			return type;
			}
		aFile.seek (aSize, std::ios_base::cur);
		}
	}

void CSISHeader::RawWrite (const TUint32 aType, const TUint64& aSize, TSISStream& aFile, const bool aIsArrayElement)
	{
	WriteFiller (aFile);
	if (! aIsArrayElement) 
		{
#ifdef GENERATE_ERRORS
		if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugDuffFieldType))
			{
			aFile << static_cast <TUint32> (rand ());
			}
		else
#endif // GENERATE_ERRORS
			{
			aFile << aType;
			}
		}
	CSISUInt32 smaller;
#ifdef GENERATE_ERRORS
	if ((aSize < KBigBit) && ! CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBug32As64))
#else // GENERATE_ERRORS
	if (aSize < KBigBit)
#endif //  GENERATE_ERRORS
		{
#ifdef GENERATE_ERRORS
		if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInvalidLength))
			{
			smaller = static_cast <TUint32> (rand () & KSmallMask);
			}
		else
		if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugNegativeLength))
			{
			smaller = KSmallMask;
			}
		else
#endif // GENERATE_ERRORS
			{
			smaller = static_cast <TUint32> (aSize & KSmallMask);
			}
		smaller.Write (aFile, false);
		}
	else
		{
		CSISUInt32 bigger;
		RawBigToSmalls (aSize, smaller, bigger);
		smaller.Write (aFile, false);
		bigger.Write (aFile, false);
		}
	}

CSISHeader::TFieldSize CSISHeader::RawByteCount (const TUint64& aSize, const bool aInsideArray)
	{
	CSISHeader::TFieldSize size (aInsideArray ? 0 : CSISFieldRoot::SizeOfFieldType ());
	if (aSize >= KBigBit) 
		{
		size += sizeof (TUint32) * 2;
		}
	else 
		{
		size += sizeof (TUint32);
		}
	return size;
	}

void CSISHeader::RawCalculateCrc (const TUint32 aType, const TUint64& aSize, TCRC& aCRC, const bool aIsArrayElement)
	{
	if (! aIsArrayElement) 
		{
		CSISUInt32 (aType).CalculateCrc (aCRC, false);
		}
	if (aSize < KBigBit) 
		{
		CSISUInt32 (static_cast <TUint32> (aSize & KSmallMask)).CalculateCrc (aCRC, false);
		}
	else
		{
		CSISUInt32 smaller;
		CSISUInt32 bigger;
		RawBigToSmalls (aSize, smaller, bigger);
		smaller.CalculateCrc (aCRC, false);
		bigger.CalculateCrc (aCRC, false);
		}
	}


bool CSISHeader::IsNextAsExpected (	TSISStream& aFile, CSISHeader& aHeader,
										const CSISFieldRoot::TFieldType aExpectedType,
										const CSISFieldRoot::TFieldType aArrayType,
										const bool aPeek)
	{
	TSISStream::pos_type pos = aFile.tell ();
	ReadFiller (aFile);
	aHeader.Read (aFile, sizeof (CSISHeader), aArrayType);
	bool reply = (aHeader.DataType () == aExpectedType);
	if (aPeek || ! reply) 
		{
		aFile.seek (pos);
		}
	return reply;
	}

