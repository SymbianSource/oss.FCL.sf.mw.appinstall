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
* Definition of the Swi::Sis::CField
*
*/


#include <e32std.h>
#include "sisfieldtypes.h"
#include "sisfield.h"
#include "sisdataprovider.h"
#include "sisinstallerrors.h"

using namespace Swi::Sis;


CField::~CField()
	{
	}

CField::CField()
	{
	
	}
	
void CField::ConstructL(MSisDataProvider& aDataProvider, TFieldType aFieldType, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	ReadTypeL(aDataProvider, aFieldType, aBytesRead, aTypeReadBehaviour);
	ReadLengthL(aDataProvider, aBytesRead);
	}
	
void CField::ConstructL(TPtrProvider& aDataProvider, TFieldType aFieldType, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	ReadTypeL(aDataProvider, aFieldType, aBytesRead, aTypeReadBehaviour);
	ReadLengthL(aDataProvider, aBytesRead);
	}
	
// see GT0188.251 Section 2.12.1.2
void CField::ReadLengthL(MSisDataProvider& aDataProvider, TInt64& aBytesRead)
	{
	TUint32 length = 0;
	
	// read the first length bytes from the field	
	CField::ReadTTypeL<TUint32>(aDataProvider,length, aBytesRead);
	
	// MSB set in length means length is 63 bits not 31 bits
	if (length & 0x80000000)
		{
		TUint32 high = 0;
		TUint32 low = length; // the low bytes are the length we just read
		
		// reset MSB bit in low bytes
		low &= ~ 0x80000000;
		
		// read high bytes
		CField::ReadTTypeL<TUint32>(aDataProvider, high, aBytesRead); 
		
		// check if the LSB is set in the high bytes, this is actually the MSB of the low bytes
		if(high & 1)
			{
			// carry bit back from LSB high bytes to MSB low bytes
			low |= 0x80000000;
			}
		
		// clear LSB of high bytes and rotate right
		high &= ~1;
		high >>= 1;
					
		iLength=MAKE_TINT64(high, low); // relies on TFieldLength being TInt64 typedef
		iHeaderSize=sizeof(TInt32) * 3;
		}
	else
		{
		iLength=TFieldLength(TInt(length));
		iHeaderSize=sizeof(TInt32) * 2;
		}
	}

void CField::ReadTypeL(MSisDataProvider& aDataProvider, TFieldType aFieldType, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	if (aTypeReadBehaviour==EReadType)
		{
		// Read type
		CField::ReadEnumL<TFieldType,TInt32>(aDataProvider, iType, aBytesRead);

		if (iType != aFieldType)
			{
			User::Leave(KErrSISUnexpectedFieldType);
			}
		}
	else
		{
		// Set type
		iType=TFieldType(aFieldType);
		}
	}

void CField::ReadLengthL(TPtrProvider& aDataProvider, TInt64& aBytesRead)
	{
	TUint32 length = 0;
	
	// read the first length bytes from the field	
	CField::ReadTTypeL<TUint32>(aDataProvider,length, aBytesRead);
	
	// MSB set in length means length is 63 bits not 31 bits
	if (length & 0x80000000)
		{
		TUint32 high = 0;
		TUint32 low = length; // the low bytes are the length we just read
		
		// reset MSB bit in low bytes
		low &= ~ 0x80000000;
		
		// read high bytes
		CField::ReadTTypeL<TUint32>(aDataProvider, high, aBytesRead); 
		
		// check if the LSB is set in the high bytes, this is actually the MSB of the low bytes
		if(high & 1)
			{
			// carry bit back from LSB high bytes to MSB low bytes
			low |= 0x80000000;
			}
		
		// clear LSB of high bytes and rotate right
		high &= ~1;
		high >>= 1;
					
		iLength=MAKE_TINT64(high, low); // relies on TFieldLength being TInt64 typedef
		iHeaderSize=sizeof(TInt32) * 3;
		}
	else
		{
		iLength=TFieldLength(TInt(length));
		iHeaderSize=sizeof(TInt32) * 2;
		}
	}

void CField::ReadTypeL(TPtrProvider& aDataProvider, TFieldType aFieldType, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	if (aTypeReadBehaviour==EReadType)
		{
		// Read type
		CField::ReadEnumL<TFieldType,TInt32>(aDataProvider, iType, aBytesRead);

		if (iType != aFieldType)
			{
			User::Leave(KErrSISUnexpectedFieldType);
			}
		}
	else
		{
		// Set type
		iType=TFieldType(aFieldType);
		}
	}

EXPORT_C TFieldLength CField::Length() const
	{
	return iLength;	
	}

TFieldType CField::FieldType() const
	{
	return iType;
	}

void CField::EnsureAlignedL(MSisDataProvider& aDataProvider, TInt64 aBytesConsumed, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	// It doesn't matter whether we're actually in an array or not for the
	// padding size since the difference is 4 bytes (==0 mod 4)
	
	TInt paddingBytes= HeaderSize() + Length() + PaddingSize() - aBytesConsumed - (aTypeReadBehaviour==EAssumeType ? 4 : 0);

	// Tempering in SISX file may cause the padding bytes to be equal or more then 4 bytes 
	// but it can be strictly between 0 to 3 only.
	// So we put a boundary here and if that fails we say corrupt sis file.
	if (paddingBytes >=0 && paddingBytes <4)
		{
		TBuf8<4> padding;
		CField::CheckedReadL(aDataProvider, padding, paddingBytes, aBytesRead);
		}
	else
		{
		User::Leave(KErrCorrupt);			
		}
	}

void CField::CheckedReadL(MSisDataProvider& aDataProvider, TDes8& aBuffer, TInt64& aBytesRead)
	{
	User::LeaveIfError(aDataProvider.Read(aBuffer, aBuffer.Length()));
	aBytesRead+=aBuffer.Length();
	}

void CField::CheckedReadL(MSisDataProvider& aDataProvider, TDes8& aBuffer, TInt aSize, TInt64& aBytesRead)
	{
	User::LeaveIfError(aDataProvider.Read(aBuffer, aSize));
	
	if (aBuffer.Length() != aSize)
		{
		User::Leave(KErrSISFieldBufferTooShort);
		}
	aBytesRead+=aSize;
	}

void CField::SkipUnknownFieldsL(MSisDataProvider& aDataProvider, TInt64 aBytesConsumed, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 skipSize=Length() - aBytesConsumed + HeaderSize() - (aTypeReadBehaviour==EAssumeType ? 4 : 0);
	
	if (skipSize==0)
		{
		return;
		}
	else if (skipSize < 0)
		{
		// this means we have over-read by -skipSize bytes
		TInt overRead=-skipSize;
		
		// If we've only over-read into the padding then it's ok since an extra field
		// may have been added where padding was before, we don't need to skip
		// any unknown sisfields since we know we're into the padding
		if (overRead <= PaddingSize())
			{
			return;
			}
			
		// otherwise it's an invalid sisfile. 
		User::Leave(KErrSISFieldLengthInvalid);
		}
		
	TInt64 curPos = 0;
	User::LeaveIfError(aDataProvider.Seek(ESeekCurrent, curPos));
	TInt64 newPos = skipSize; 
	User::LeaveIfError(aDataProvider.Seek(ESeekCurrent, newPos));
	if (curPos + skipSize != newPos)
		{
		User::Leave(KErrSISFieldLengthInvalid);
		}	

	aBytesRead += skipSize;
	}


void CField::CreateHeader(TFieldType aFieldType, TFieldLength aFieldLength, TDes8& aHeader)
	{
	aHeader.Zero();
	
	// define the integer / buffer pair used to append TInt32 integers to the header
	TUint32 value;
	TPckg<TUint32> pckg(value);
	
	// Append the field type to the header
	value = static_cast<TUint32>(aFieldType);
	aHeader.Append(pckg);
	
	// get the high and low 32 bit parts of the length
	TUint32 high = I64HIGH(aFieldLength);
	TUint32 low = I64LOW(aFieldLength);
	
	// If it's >= 2 ^ 31 it will need to be written to the file in two 32 bit parts
	if(high != 0 || low & 0x80000000)
		{
		// rotate the high value left ready to carry the bit 31 value from low
		high <<= 1;
		if(low & 0x80000000)
			{
			// carry from low if necessary
			high |= 1;
			}
		
		// number is > 2^31 so must set MSB to flag this
		low |= 0x80000000;

		// append low 32 bits
		value = low;
		aHeader.Append(pckg);
		
		// append high 32 bits
		value = high;
		aHeader.Append(pckg);			
		}
	else
		{
		// it's less than 2^31 so we can write it out in one TInt32
		value = low;
		aHeader.Append(pckg);	
		}
	}
		
void CField::CalculateCrcL(MSisDataProvider& aDataProvider, const TInt64 aMaxLength, TUint16& aCrc)
	{
	// This function may appear inefficient but the data for which
	// a CRC is calculated is not all in memory so reading from the
	// data provider is somewhat inevitable
	TBuf8 <512> buffer;
	TInt length = 0;
	TInt readLength = buffer.MaxLength();
	
	// Do the CRC calculation for the data
	while(length < aMaxLength)
		{
		buffer.Zero();
		if(length + buffer.MaxLength() > aMaxLength)
			{
			readLength = aMaxLength - length;
			}
		User::LeaveIfError(aDataProvider.Read(buffer, readLength));
		
		// If EOF is reached before aMaxLength has been read, the
		// length specified in the header is incorrect.
		if (buffer.Length() == 0)
			{
			User::Leave(KErrSISFieldLengthInvalid);
			}
		length += buffer.Length();
		Mem::Crc(aCrc, buffer.Ptr(), buffer.Length());
		}

	// Do CRC of any padding
	TUint8 padding[4] = { 0, 0, 0, 0 };
	Mem::Crc(aCrc, padding, aMaxLength % 4);	
	}

void CField::EnsureAlignedL(TPtrProvider& aDataProvider, TInt64 aBytesConsumed, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	// It doesn't matter whether we're actually in an array or not for the
	// padding size since the difference is 4 bytes (==0 mod 4)
	
	TInt paddingBytes= HeaderSize() + Length() + PaddingSize() - aBytesConsumed - (aTypeReadBehaviour==EAssumeType ? 4 : 0);
	if (paddingBytes)
		{
		aDataProvider.ReadL(paddingBytes);
		aBytesRead += paddingBytes;
		}
	}

void CField::SkipUnknownFieldsL(TPtrProvider& aDataProvider, TInt64 aBytesConsumed, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 skipSize=Length() - aBytesConsumed + HeaderSize() - (aTypeReadBehaviour==EAssumeType ? 4 : 0);
	
	if (skipSize==0)
		{
		return;
		}
	else if (skipSize < 0)
		{
		// this means we have over-read by -skipSize bytes
		TInt overRead=-skipSize;
		
		// If we've only over-read into the padding then it's ok since an extra field
		// may have been added where padding was before, we don't need to skip
		// any unknown sisfields since we know we're into the padding
		if (overRead <= PaddingSize())
			{
			return;
			}
			
		// otherwise it's an invalid sisfile. 
		User::Leave(KErrSISFieldLengthInvalid);
		}
		
	aDataProvider.ReadL(skipSize);	
	aBytesRead += skipSize;
	}
