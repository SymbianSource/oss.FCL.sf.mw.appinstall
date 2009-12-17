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
* Definition of the Swi::Sis::CContents
*
*/


#include <e32std.h>
#include <s32mem.h>
#include "sisinstallerrors.h"
#include "siscontents.h"
#include "siscrc.h"
#include "siscompressed.h"
#include "sisdata.h"
#include "sisblob.h"

#include "sisdataprovider.h"
#include "compresseddataprovider.h"

using namespace Swi;
using namespace Swi::Sis;

// This line is commented so that SWI does not enforce CRC
// checking. If CRC checking was enforced existing SIS
// files created before DEF063815 was fixed would fail
// Uncommenting this line would cause a data compatibility break

// #define SIS_CRC_CHECK_ENABLED


/*static*/ CContents* CContents::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CContents* self = new(ELeave) CContents(aDataProvider);
	CleanupStack::PushL(self);
	self->ConstructL(aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}
 
CContents::~CContents()
	{
	delete iCompressed;
	delete iData;
	delete iDataCrc;
	delete iControllerCrc;	
	}

CContents::CContents(MSisDataProvider& aDataProvider) : iDataProvider(aDataProvider)
	{
	}

void CContents::ConstructL(TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	
	CField::ConstructL(iDataProvider, EFieldTypeContents, aBytesRead, aTypeReadBehaviour);	

	TFieldType fieldType;
	CField::ReadEnumL<TFieldType,TUint32>(iDataProvider, fieldType, aBytesRead);

	if (fieldType == EFieldTypeControllerCRC)
		{
		// Controller Checksum present
		iControllerCrc = CCrc::NewL(iDataProvider, aBytesRead, EAssumeType);
		
		CField::ReadEnumL<TFieldType,TUint32>(iDataProvider, fieldType, aBytesRead);
		}	

	if (fieldType == EFieldTypeDataCRC)
		{
		// Data Checksum present
		iDataCrc = CCrc::NewL(iDataProvider, aBytesRead, EAssumeType);
		
		CField::ReadEnumL<TFieldType,TUint32>(iDataProvider, fieldType, aBytesRead);
		}

	if (fieldType != EFieldTypeCompressed)
		{
		User::Leave(KErrSISUnexpectedFieldType);
		}

	iCompressed = CCompressed::NewL(iDataProvider, aBytesRead, EAssumeType);
	iData = CData::NewL(iDataProvider, aBytesRead);

#ifdef SIS_CRC_CHECK_ENABLED
	// This line is commented because existing SIS files had an
	// incorrect CRC so enforcing the checksum now would be a data
	// compatibility break.
	// See  DEF057005
	CheckCrcL();	
#endif
	}
	
EXPORT_C HBufC8* CContents::ReadControllerL() const
	{
	return iCompressed->ReadControllerDataL();
	}
	
EXPORT_C void CContents::ReadDataL(RFile& aFile, TInt aFileIndex, TInt aDataUnit)
	{
	iData->ReadDataL(aFile, aFileIndex, aDataUnit);
	}
	
EXPORT_C void CContents::ReadDataL(RFile& aFile, TInt aFileIndex, TInt aDataUnit, TInt64 aLength)
	{
	iData->ReadDataL(aFile, aFileIndex, aDataUnit, aLength);
	}

void CContents::WriteStubFieldsL(RFile& aFile, MSisDataProvider& aDataProvider)
	{
	// create a buffer to receive the fields of the CSisContent for the stub
	CBufFlat* buf = CBufFlat::NewL(250);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// Skip over the SisContents header
	CContents* self = new(ELeave) CContents(aDataProvider);
	CleanupStack::PushL(self);
	self->ReadHeaderL();
	TInt64 maxLength = self->Length() + self->PaddingSize();
	CleanupStack::PopAndDestroy(self);

	// Extract the relevant fields from the SISContents, write them 
	// to the buffer via the stream	
	ExtractSisStubFieldsL(writeStream, aDataProvider, maxLength);
	CData::AppendStubDataFieldL(writeStream);
	
	CleanupStack::PopAndDestroy(&writeStream);
	TPtr8 sisContentsBuf = buf->Ptr(0);

	TBuf8<3 * sizeof(TInt32)> header;
	CField::CreateHeader(EFieldTypeContents, TInt64(sisContentsBuf.Length()), header);
	// Write sis contents header
	User::LeaveIfError(aFile.Write(header));
	// Write sis contents
	User::LeaveIfError(aFile.Write(sisContentsBuf));
	// Write sis contents padding,
	TInt lengthMod4 = (header.Length() + sisContentsBuf.Length()) % 4;
	
	TBuf8<1> padding;
	padding.Append(0);
	while(lengthMod4 > 0)
		{
		User::LeaveIfError(aFile.Write(padding));
		lengthMod4--;
		}

	// finished
	CleanupStack::PopAndDestroy(buf);
	}

void CContents::ExtractSisStubFieldsL(RWriteStream& aWriteStream, MSisDataProvider& aDataProvider, TInt64& aMaxLength)
	{
	TInt64 bytesRead = 0;
	
	// write the sis file to the stream but skip the Data and DataCRC fields
	// This assumes all the other fields are correct, no checking is performed
	TFieldType fieldType(EFieldTypeContents);
	while(fieldType != EFieldTypeData)
		{
		// get field type from data provider
		CField::ReadEnumL<TFieldType,TUint32>(aDataProvider, fieldType, bytesRead);
		
		if (fieldType == EFieldTypeDataCRC)
			{
			// read data checksum in order to skip over it
			CCrc* crc = CCrc::NewL(aDataProvider, bytesRead, EAssumeType);
			delete crc;
			crc = NULL;
			}
		else if (fieldType == EFieldTypeData)
			{
			// read data field in order to skip over it
			// could be many megabytes so we don't want to create
			// it as a blob
			CData* data = CData::NewL(aDataProvider, bytesRead, EAssumeType);
			delete data;
			data = NULL;
			}
		else
			{
			// load the field as a blob, we don't care whats inside
			CBlob* blob = CBlob::NewLC(aDataProvider, bytesRead, EAssumeType);
	
			// Write the field to the output stream
			// Cant do this inside CSisBlob because it's TCB in SisController.mmp
			// and the streaming classes are not TCB			
			
			// Create a header for the fieldType with a given length
			TBuf8<3 * sizeof(TInt32)> header;
			CField::CreateHeader(fieldType, TInt64(blob->Length()), header);
			aWriteStream.WriteL(header);
			aWriteStream.WriteL(blob->Data());
			for(TInt i = 0; i < blob->PaddingSize(); i++)
				{
				// write out zero padding at the end of the SisField
				aWriteStream.WriteInt8L(0);
				}			
			CleanupStack::PopAndDestroy(blob);
			}
		if(bytesRead > aMaxLength)
			{
			// we have read past the end of the SisContents field
			User::Leave(KErrCorrupt);
			}
		}
	}

void CContents::ReadHeaderL()
	{
	// This just skips over the header without actually reading any of the data
	TInt64 bytesRead =0;
	CField::ConstructL(iDataProvider, EFieldTypeContents, bytesRead, EReadType);	
	}
	
EXPORT_C void CContents::CheckCrcL()
	{
	if(iControllerCrc && iControllerCrc->Checksum() != iCompressed->Crc())
		{
		User::Leave(KErrCorrupt);
		}

	if(iDataCrc && iDataCrc->Checksum() != iData->Crc())
		{
		User::Leave(KErrCorrupt);
		}
	}
