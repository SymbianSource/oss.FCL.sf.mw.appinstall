// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// dictionarycompression.cpp
//
/** 
* @file dictionarycompression.cpp
*
* @internalComponent
* @released
*/

#include "dictionarycompression.h"

RDictionaryCompressionBitStream::RDictionaryCompressionBitStream() :
	iNumberOfBitsUsedForDictionaryTokens(0),
	iOffsetToFirstBit(-1),
	iOffsetToCurrentBit(-1),
	iOffsetOnePastLastBit(-1),
	iOwnsBitBuffer(false),
	iBuffer(NULL)
	{
	}

void RDictionaryCompressionBitStream::OpenL(
				TInt aNumberOfBitsUsedForDictionaryTokens,
				TInt aOffsetToFirstBit,
				TInt aOffsetOnePastLastBit,
				TBool aTransferringOwnershipOfBuffer,
				TUint8* aBuffer)
	{
	iNumberOfBitsUsedForDictionaryTokens = aNumberOfBitsUsedForDictionaryTokens;
	iOffsetToFirstBit = aOffsetToFirstBit;
	iOffsetToCurrentBit = aOffsetToFirstBit;
	iOffsetOnePastLastBit = aOffsetOnePastLastBit;
	iOwnsBitBuffer = aTransferringOwnershipOfBuffer;
	iBuffer = aBuffer;

	assert(aBuffer!=NULL);
	assert(aOffsetToFirstBit >= 0);
	assert(aOffsetToFirstBit<=aOffsetOnePastLastBit);
	}

void RDictionaryCompressionBitStream::Close()
	{
	if (iOwnsBitBuffer)
		{
		iOwnsBitBuffer=EFalse;
		delete [] iBuffer;
		}
	iBuffer=NULL;
	}

TBool RDictionaryCompressionBitStream::EndOfStreamL() const
	{
	assert(iBuffer!=NULL);
	assert(iOffsetToFirstBit >= 0);
	assert(iOffsetToCurrentBit>=iOffsetToFirstBit);
	assert(iOffsetToCurrentBit<=iOffsetOnePastLastBit);
	return iOffsetToCurrentBit>=iOffsetOnePastLastBit;
	}

TInt RDictionaryCompressionBitStream::IndexOfDictionaryEntryL()
	{
	// increments the current bit-position if it returns a value >=0; returns KErrNotFound if the next thing in the stream is plain data rather than the index of a dictionary entry
	assert(iBuffer!=NULL);
	assert(!EndOfStreamL());
	if (!CurrentBitIsOn())
		{
		++iOffsetToCurrentBit;
		return ReadIntegerL(iNumberOfBitsUsedForDictionaryTokens);
		}
	return KErrNotFound;
	}

void RDictionaryCompressionBitStream::ReadL(Ptr8 aBufferToAppendTo,TBool aCalypsoFileFormat)
	{
	// can only be called if IndexOfDictionaryEntry returned a negative value
	assert(iBuffer!=NULL);
	assert(!EndOfStreamL());
	TInt numberOfConsecutivePrefixBits=0;
	TInt i;
	for (i=0; i<4; ++i)
		{
		const TBool currentBitIsOn=CurrentBitIsOn();
		++iOffsetToCurrentBit; // increment this regardless whether the current bit is on
		if (!currentBitIsOn)
			{
			break;
			}
		++numberOfConsecutivePrefixBits;
		}
	assert(numberOfConsecutivePrefixBits>0);
	assert(numberOfConsecutivePrefixBits<=4);
	TInt numberOfBytesToRead = numberOfConsecutivePrefixBits;
	if (numberOfConsecutivePrefixBits==3)
		{
		numberOfBytesToRead=3+ReadIntegerL(3);
		}
	else if (numberOfConsecutivePrefixBits==4)
		{
		numberOfBytesToRead=ReadIntegerL(8);
		if (!aCalypsoFileFormat)
			{
			numberOfBytesToRead+=3+(1<<3);
			}
		}

	const TInt numberOfBitsOffByteBoundary=iOffsetToCurrentBit%8;
	const TUint8* currentByte=iBuffer+(iOffsetToCurrentBit/8);
	assert((numberOfBytesToRead + aBufferToAppendTo.GetLength()) <= aBufferToAppendTo.GetMaxLength());
	for (i=0; i<numberOfBytesToRead; ++i, ++currentByte)
		{
		TUint byte=*currentByte;
		assert(numberOfBitsOffByteBoundary>=0);
		if (numberOfBitsOffByteBoundary>0)
			{
			byte>>=numberOfBitsOffByteBoundary;
			byte|=(*(currentByte+1)<<(8-numberOfBitsOffByteBoundary));
			byte&=0xff;
			}
		aBufferToAppendTo.Append(byte,1);
		}
	iOffsetToCurrentBit+=numberOfBytesToRead*8;
	assert(numberOfBitsOffByteBoundary==iOffsetToCurrentBit%8);
	}

TBool RDictionaryCompressionBitStream::CurrentBitIsOn() const
	{
	// does not increment the current bit-position
	assert(iBuffer!=NULL);
	return iBuffer[iOffsetToCurrentBit/8]&(1<<(iOffsetToCurrentBit%8));
	}

TUint RDictionaryCompressionBitStream::ReadIntegerL(TInt aNumberOfBits)
	{
	// increments the current bit-position
	assert(iBuffer!=NULL);
	TInt integer=0;
	TInt numberOfBitsLeftToRead=aNumberOfBits;
//	FOREVER
	while(1)
		{
		const TInt offsetToFirstBitToReadInCurrentByte=iOffsetToCurrentBit%8;
		const TInt offsetOnePastLastBitToReadInCurrentByte=Min(8,offsetToFirstBitToReadInCurrentByte+numberOfBitsLeftToRead);
		const TInt numberOfBitsReadFromCurrentByte=offsetOnePastLastBitToReadInCurrentByte-offsetToFirstBitToReadInCurrentByte;
		assert(numberOfBitsReadFromCurrentByte>0);
		const TUint bitsReadFromCurrentByte=((iBuffer[iOffsetToCurrentBit/8]>>offsetToFirstBitToReadInCurrentByte)&((1<<numberOfBitsReadFromCurrentByte)-1));
		integer|=(bitsReadFromCurrentByte<<(aNumberOfBits-numberOfBitsLeftToRead));
		iOffsetToCurrentBit+=numberOfBitsReadFromCurrentByte;
		numberOfBitsLeftToRead-=numberOfBitsReadFromCurrentByte;
		assert(numberOfBitsLeftToRead>=0);
		if (numberOfBitsLeftToRead<=0)
			{
			break;
			}
		}
	return integer;
	}

