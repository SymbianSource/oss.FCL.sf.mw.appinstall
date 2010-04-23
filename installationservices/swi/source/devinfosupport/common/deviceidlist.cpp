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
* Simple Device Id list container.
* @internalComponent
* @released
*
*/


#include "deviceidlist.h"
#include <s32mem.h>
#include <etelmm.h>

static const int KMaxDeviceIdLength = RMobilePhone::KPhoneSerialNumberSize;

namespace Swi
{
/**
Implementation of the MStreamBuf interface that throws away all
data written to it but keeps track of how many bytes have been
written to it.  It does not support reading.
*/
class TNullBuf : public MStreamBuf
	{
public:
	inline TNullBuf();
	inline TUint BytesWritten();
private:
	inline virtual void DoWriteL(const TAny* aPtr,TInt aLength);
private:
	TUint iBytesWritten;
	};

inline TNullBuf::TNullBuf() : iBytesWritten(0) 
	{
	}

inline TUint TNullBuf::BytesWritten() 
	{
	return iBytesWritten;
	}

inline void TNullBuf::DoWriteL(const TAny*,TInt aLength)
	{
	iBytesWritten += aLength;
	}

/**
A write stream that throws away all its input, but keeps track of how many
bytes have been written to it.  It is used for determining the amount of
memory needed to store externalised objects.
*/
class RNullWriteStream : public RWriteStream
	{
public:
	inline RNullWriteStream();
	inline TUint BytesWritten();
private:
	TNullBuf iSink;
	};

inline RNullWriteStream::RNullWriteStream()
	{
	Attach(&iSink);
	}

inline TUint RNullWriteStream::BytesWritten()
	{
	return iSink.BytesWritten();
	}

EXPORT_C CDeviceIdList* CDeviceIdList::NewL()
	{
	CDeviceIdList* self = NewLC();
	CleanupStack::Pop(self);
	return self;	
	}

EXPORT_C CDeviceIdList* CDeviceIdList::NewLC()
	{
	CDeviceIdList* self = new (ELeave) CDeviceIdList;
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CDeviceIdList* CDeviceIdList::NewL(const TDesC8& aPackedList)
	{
	CDeviceIdList* self = NewLC(aPackedList);
	CleanupStack::Pop(self);
	return self;	
	}
EXPORT_C CDeviceIdList* CDeviceIdList::NewLC(const TDesC8& aPackedList)
	{
	CDeviceIdList* self = NewLC();
	RDesReadStream stream(aPackedList);
	CleanupClosePushL(stream);
	stream >> *self;
	CleanupStack::PopAndDestroy();
	return self;	
	}

CDeviceIdList::CDeviceIdList()
	{
	}

EXPORT_C CDeviceIdList::~CDeviceIdList()
	{
	iDeviceIds.ResetAndDestroy();
	}

EXPORT_C void CDeviceIdList::AppendL(const TDesC& aDeviceId)
	{
	if (aDeviceId.Length() > KMaxDeviceIdLength)
		{
		User::Leave(KErrArgument);
		}
	HBufC* myDeviceId = aDeviceId.AllocLC();
	iDeviceIds.AppendL(myDeviceId);
	CleanupStack::Pop(myDeviceId);
	}

EXPORT_C void CDeviceIdList::ExternalizeL(RWriteStream& aStream) const
	{
	TInt count = iDeviceIds.Count();
	TCardinality card = count;
	aStream << card;
	for (TInt i = 0; i < count; ++i)
		{
		TPtr p = iDeviceIds[i]->Des();
		aStream << p;
		}
	}

EXPORT_C void CDeviceIdList::InternalizeL(RReadStream& aStream)
	{
	iDeviceIds.ResetAndDestroy();
	TCardinality c;
	aStream >> c;
	for (TInt i = 0; i < c; i++)
		{
		HBufC* s = HBufC::NewLC(aStream, KMaxDeviceIdLength);
		iDeviceIds.AppendL(s);
		CleanupStack::Pop(s);
		}
	}

EXPORT_C HBufC8* CDeviceIdList::PackL() const
	{
	// use the nullStream to count the number of bytes the packed
	// array will use so we can allocate a buffer of the right length.
	RNullWriteStream nullStream;
	CleanupClosePushL(nullStream);
	nullStream << *this;
	TUint bytesWritten = nullStream.BytesWritten();
	CleanupStack::PopAndDestroy(&nullStream);
	HBufC8 *buffer = HBufC8::NewLC(bytesWritten);
	TPtr8 bufferDes = buffer->Des();
	RDesWriteStream writeStream(bufferDes);
	CleanupClosePushL(writeStream);
	writeStream << *this;
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::Pop(buffer);
	return buffer;
	}

EXPORT_C const RPointerArray<HBufC>& CDeviceIdList::DeviceIds() const
	{
	return iDeviceIds;
	}

}
