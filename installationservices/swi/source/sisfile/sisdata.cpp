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
* Definition of the Swi::Sis::CData
*
*/


/**
 @file
 @internalComponent
*/
#include "sisdata.h"
#include "sisdataunit.h"

namespace Swi
{
namespace Sis
{

/*static*/ CData* CData::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CData* self = new(ELeave) CData(aDataProvider);
	CleanupStack::PushL(self);
	self->ConstructL(aBytesRead, aTypeReadBehaviour);
	return self;
	}

/*static*/ CData* CData::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CData* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CData::CData(MSisDataProvider& aDataProvider) : iDataProvider(aDataProvider)
	{
	}

CData::~CData()
	{
	iDataUnits.ResetAndDestroy();	
	}

void CData::ConstructL(TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 arrayBytes = 0;	
	CField::ConstructL(iDataProvider, EFieldTypeData, aBytesRead, aTypeReadBehaviour);	

	// Remember the offset where the data begins
	User::LeaveIfError(iDataProvider.Seek(ESeekCurrent, iOffset));
	ReadMemberArrayL(iDataProvider, iDataUnits, EFieldTypeDataUnit, arrayBytes, EReadType);

	TInt64 fieldOffset = 0;
#ifdef SIS_CRC_CHECK_ENABLED
	// Calculate CRC of header and field data by reading the entire header and field
	fieldOffset = iOffset - HeaderSize();
	User::LeaveIfError(iDataProvider.Seek(ESeekStart, fieldOffset));
	CField::CalculateCrcL(iDataProvider, HeaderSize() + Length() + PaddingSize(), iCrc );
#else
	fieldOffset = iOffset + Length() + PaddingSize();
    User::LeaveIfError(iDataProvider.Seek(ESeekStart, fieldOffset));
#endif
	aBytesRead += Length() + PaddingSize();
	}

void CData::ReadDataL(RFile& aFile, TInt aFileIndex, TInt aDataUnit)
	{
	if ((aFileIndex < 0) || ((aDataUnit < 0) || (aDataUnit >= iDataUnits.Count())))
		{
		User::Leave(KErrCorrupt);
		}
	iDataUnits[aDataUnit]->ExtractDataFileL(aFile, aFileIndex);
	}

void CData::ReadDataL(RFile& aFile, TInt aFileIndex, TInt aDataUnit, TInt64 aLength)
	{
	if ((aFileIndex < 0) || ((aDataUnit < 0) || (aDataUnit >= iDataUnits.Count())))
		{
		User::Leave(KErrCorrupt);
		}
	iDataUnits[aDataUnit]->ExtractDataFileL(aFile, aFileIndex, aLength);
	}

void CData::AppendStubDataFieldL(RWriteStream& aWriteStream)
	{
	// The data field has the following structure
	
	//	Object 				Object Size (bytes)	Value
	//	FieldType 			4					EFieldTypeData		
	//  FieldLength			4					12
	//  data				12					The empty Data unit array 
	
	//  The empty data unit array inside the Data field has the following structure
	
	//	Object 				Object Size (bytes)	Value
	//  FieldType			4					EFieldTypeArray
	//	FieldLength			4					4
	// 	ArrayType			4					EFieldTypeDataUnit
	
	
	// append the EFieldTypeData header to the stream
	TBuf8<3 * sizeof(TInt32)> header;
	CField::CreateHeader(EFieldTypeData, TInt64(12), header);
	aWriteStream.WriteL(header);
	
	// append an empty array header to the stream
	CField::CreateHeader(EFieldTypeArray, TInt64(4), header);
	aWriteStream.WriteL(header);
	
	// append the array type to the stream
	TInt32 arrayType;
	TPckg<TInt32> pckg(arrayType);
	arrayType = static_cast<TInt32>(EFieldTypeDataUnit);
	aWriteStream.WriteL(pckg);
	}


} //namespace Sis

} //namespace Swi


