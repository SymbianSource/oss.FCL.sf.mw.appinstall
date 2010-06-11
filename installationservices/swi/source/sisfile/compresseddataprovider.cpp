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
* Definition of the Swi::CCompressedDataProvider
*
*/


#include <ezdecompressor.h>
#include <ezstream.h>

#include "compresseddataprovider.h"

using namespace Swi;

/*static*/ CCompressedDataProvider* CCompressedDataProvider::NewLC(MSisDataProvider& aDataProvider,
															       TInt aBufferSize)
	{
	CCompressedDataProvider* self = new(ELeave) CCompressedDataProvider(aDataProvider);
	CleanupStack::PushL(self);
	self->ConstructL(aBufferSize);
	return self;
	}

/*static*/ CCompressedDataProvider* CCompressedDataProvider::NewL(MSisDataProvider& aDataProvider,
																  TInt aBufferSize)
	{
	CCompressedDataProvider* self = NewLC(aDataProvider, aBufferSize);
	CleanupStack::Pop(self);
	return self;
	}

CCompressedDataProvider::CCompressedDataProvider(MSisDataProvider& aDataProvider) : iDataProvider(aDataProvider), 
																					iInputDescriptor(NULL,0), 
																					iOutputDescriptor(NULL,0),
																					iMore(ETrue)
	{	
	}

void CCompressedDataProvider::ConstructL(TInt aBufferSize)
	{
	TInt bufferSize;
	if (aBufferSize > KDeflateBufferMaxSize)
		{
		bufferSize = KDeflateBufferMaxSize;
		}
	else
		{
		bufferSize = aBufferSize;
		}
	
	iInputBuffer = HBufC8::NewL(bufferSize);
    iOutputBuffer = HBufC8::NewL(bufferSize);
	iInputDescriptor.Set(iInputBuffer->Des());
	iOutputDescriptor.Set(iOutputBuffer->Des());
	iDecompressor = CEZDecompressor::NewL(*this);
	iBuffer = CBufFlat::NewL(bufferSize);
	}

CCompressedDataProvider::~CCompressedDataProvider()
	{
	delete iDecompressor;
    delete iInputBuffer;
    delete iOutputBuffer;
    delete iBuffer;	
	}
    
TInt CCompressedDataProvider::Read(TDes8& aDes)
	{
	return Read(aDes, aDes.MaxSize());
	}

TInt CCompressedDataProvider::Read(TDes8& aDes, TInt aLength)
	{ 	
	if (aLength < 0)                  // Cannot read a negative amount of data!
		{
		return KErrArgument;
		}

	if (aLength > aDes.MaxSize())     // The descriptor is not big enough
		{
		return KErrOverflow;
		}

	TInt needed = aLength;
	
	if (aLength > iBuffer->Size())    // will still need to decompress more
		{
		iBuffer->Read(0, aDes, iBuffer->Size()); // read any buffered data
		needed -= iBuffer->Size();
		iBuffer->Reset(); // all data has been read, free the memory
		}
	else
		{  	                          // enough data in the buffer no need to decompress more
		iBuffer->Read(0, aDes, aLength);	// read the whole length from buffered data
		iBuffer->Delete(0, aLength); // delete the data that has been read from the buffer
		return KErrNone;		
		}
	
	// Not enough data buffered yet... 
	// We need to uncompress more data! (if available)

	if (!iMore)  // no more data to uncompress
		{
		return KErrNone;
		}

	// If we are here we used up all the buffered data!!

	while (iMore)
		{
		TRAPD(error, iMore = iDecompressor->InflateL());
		if(KErrNone != error)
			{
			return error;
			}
		
		TInt currentLength = aDes.Length();	
		
		if (needed > iBuffer->Size())    // will still need to decompress more
			{
			// Append the new data to the descriptor
			aDes.SetLength(currentLength + iBuffer->Size());
			TPtr8 rest = aDes.MidTPtr(currentLength, iBuffer->Size()); 
			iBuffer->Read(0, rest, iBuffer->Size()); // read the buffered data
			needed -= iBuffer->Size();
			iBuffer->Reset(); // all data has been read, free the memory
			}
		else
			{  	                          // enough data in the buffer no need to decompress more
			// Append the new data to the descriptor
			aDes.SetLength(currentLength + needed);
			TPtr8 rest = aDes.MidTPtr(currentLength, needed); 
			iBuffer->Read(0, rest, needed); // read the needed amount from buffered data
			iBuffer->Delete(0, needed); // delete the data that has been read from the buffer
			return KErrNone;		
			}

		if ((!iMore) || (needed==0))  // no more data to uncompress or needed
			{
			return KErrNone;
			}
		}
	return KErrGeneral;
	}

TInt CCompressedDataProvider::Seek(TSeek /*aMode*/, TInt64& /*aPos*/)
	{
	// we cannot seek on a compressed stream at the moment!
	return KErrNotSupported;
	}


void CCompressedDataProvider::InitializeL(CEZZStream& aZStream)
	{
	User::LeaveIfError(iDataProvider.Read(iInputDescriptor));

	aZStream.SetInput(iInputDescriptor);
	aZStream.SetOutput(iOutputDescriptor);
	}

void CCompressedDataProvider::NeedInputL(CEZZStream& aZStream)
	{
	User::LeaveIfError(iDataProvider.Read(iInputDescriptor));
	aZStream.SetInput(iInputDescriptor);
	}

void CCompressedDataProvider::NeedOutputL(CEZZStream& aZStream)
	{
	TPtrC8 od = aZStream.OutputDescriptor();
	iBuffer->InsertL(iBuffer->Size(), od);    // Should really check we are not blowing everything up!!!
	iOutputDescriptor.SetLength(0); // reset since data now stored in buffer
	aZStream.SetOutput(iOutputDescriptor);
	}

void CCompressedDataProvider::FinalizeL(CEZZStream& aZStream)
	{
	TPtrC8 od = aZStream.OutputDescriptor();
	iBuffer->InsertL(iBuffer->Size(), od);
	}
