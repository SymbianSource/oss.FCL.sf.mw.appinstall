/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Definition of the Swi::Sis::CCompressed
*
*/


#include "siscompressed.h"
#include "sisfiledata.h"
#include "compresseddataprovider.h"

using namespace Swi;
using namespace Swi::Sis;

const TInt KMaxTemporaryBufferSize = 0x8000;

/*static*/ CCompressed* CCompressed::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CCompressed* self = new (ELeave) CCompressed(aDataProvider);
	CleanupStack::PushL(self);
	self->ConstructL(aBytesRead, aTypeReadBehaviour);
	return self;
	}

/*static*/ CCompressed* CCompressed::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CCompressed* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CCompressed::CCompressed(MSisDataProvider& aDataProvider) : iDataProvider(aDataProvider)
	{
	}

CCompressed::~CCompressed()
	{
	delete iCompressedReader;
	}

void CCompressed::ConstructL(TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{	
	CField::ConstructL(iDataProvider, EFieldTypeCompressed, aBytesRead, aTypeReadBehaviour);	
	// Get the current offset
	User::LeaveIfError(iDataProvider.Seek(ESeekCurrent, iOffset));
	TInt64 fieldOffset = 0;

#ifdef SIS_CRC_CHECK_ENABLED

	// Calculate CRC of header and field data
	fieldOffset = iOffset - HeaderSize();
	User::LeaveIfError(iDataProvider.Seek(ESeekStart, fieldOffset));
	CField::CalculateCrcL(iDataProvider, HeaderSize() + Length() + PaddingSize(), iCrc );
#else
	fieldOffset = iOffset + Length() + PaddingSize();
	User::LeaveIfError(iDataProvider.Seek(ESeekStart, fieldOffset));
#endif

	aBytesRead += Length() + PaddingSize();
	}


MSisDataProvider& CCompressed::DataProviderL()
	{
	TInt64 bytesRead=0;
	
	User::LeaveIfError(iDataProvider.Seek (ESeekStart, iOffset));

	TCompressionAlgorithm alg;

	CField::ReadEnumL<TCompressionAlgorithm,TUint32>(iDataProvider, alg, bytesRead);
	
	CField::ReadTTypeL(iDataProvider, iUncompressedLength, bytesRead);
	
	// SISX defines the maximum to be 2^31 - 1 bytes. However, RHeap::Alloc will panic if you 
	// ask for KMaxTInt / 2 or more bytes and at the same time length shouldn't be negative.
	if ((iUncompressedLength >= KMaxTInt / 2) || (iUncompressedLength < 0))  
		{
		User::Leave(KErrSISFieldLengthInvalid);
		}

	TFieldLength compressedLength = Length() - sizeof(TUint32) - sizeof(TFieldLength); // 12 = length(TCompressionAlgorithm) + length (TFieldLength)

	switch (alg)
		{
		case ECompressDeflate:
			{
			if (iCompressedReader)
				{
				delete iCompressedReader;
				iCompressedReader = NULL;
				}
			iCompressedReader = CCompressedDataProvider::NewL(iDataProvider, I64INT(compressedLength));
			return *iCompressedReader;
			}

		case ECompressNone:
			{
			if (iUncompressedLength != compressedLength)
				{
				User::Leave(KErrSISFieldLengthInvalid);
				}
			return iDataProvider;
			}
			
		default:
			{
			User::Leave(KErrSISCompressionNotSupported);
			}			
		}
	return iDataProvider;   // Will not get here
	}

void CCompressed::ExtractDataFieldL(RFile& aFile)
	{
	MSisDataProvider& provider = DataProviderL();
	ExtractDataFieldL(provider, aFile, iUncompressedLength);
	}

void CCompressed::ExtractDataFieldL(RFile& aFile, TInt64 aLength)
	{
	if (iCurrentDataProvider==NULL)
		{
		iBytesExtracted=0;
		iCurrentDataProvider=&DataProviderL();
		}
	ExtractDataFieldL(*iCurrentDataProvider, aFile, aLength);
	}

void CCompressed::ExtractDataFieldL(MSisDataProvider& aDataProvider, RFile& aFile, TInt64 aLength)
	{
	HBufC8* buffer = HBufC8::NewMaxLC(KMaxTemporaryBufferSize);

	TInt32 endPosition = iBytesExtracted + I64INT(aLength);
	
	TPtr8 bufferPtr(buffer->Des());
	
	while (buffer->Length() > 0 && iBytesExtracted != endPosition)
		{
		TInt requestSize = ((endPosition - iBytesExtracted) < KMaxTemporaryBufferSize) ? (endPosition - iBytesExtracted) : KMaxTemporaryBufferSize;
		User::LeaveIfError(aDataProvider.Read(bufferPtr, requestSize));
		User::LeaveIfError(aFile.Write(*buffer));
		iBytesExtracted += buffer->Length();
		}

		
	if (iBytesExtracted != endPosition)
		{
		User::Leave(KErrSISFieldBufferTooShort);
		}
		
	if(iBytesExtracted == iUncompressedLength)
		{
		// reached end of field, nothing left to extract so free memory
		delete iCompressedReader;
		iCompressedReader = NULL;
		iCurrentDataProvider = NULL;
		}
		
	CleanupStack::PopAndDestroy(buffer);
	}


HBufC8* CCompressed::ReadControllerDataL()
	{
	MSisDataProvider& provider = DataProviderL();

	HBufC8* controller = HBufC8::NewMaxLC(iUncompressedLength);
	TPtr8 controllerBuf(controller->Des());

	TInt err = provider.Read(controllerBuf, iUncompressedLength);

	User::LeaveIfError(err);
	
 	if (controllerBuf.Length() != iUncompressedLength)
		{
		User::Leave(KErrSISFieldBufferTooShort);
		}	
		
	CleanupStack::Pop(controller); // pass on ownership

	return controller;
	}
