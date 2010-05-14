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
* Definition of the Swi::Sis::CDataUnit
*
*/


#include "sisdataunit.h"
#include "sisfiledata.h"

using namespace Swi::Sis;

/*static*/ CDataUnit* CDataUnit::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CDataUnit* self = new(ELeave) CDataUnit();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

/*static*/ CDataUnit* CDataUnit::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CDataUnit* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CDataUnit::CDataUnit()
	{
	}

CDataUnit::~CDataUnit()
	{
	iCompressedFiles.ResetAndDestroy();
	}

void CDataUnit::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeDataUnit, aBytesRead, aTypeReadBehaviour);

	ReadMemberArrayL(aDataProvider, iCompressedFiles, EFieldTypeFileData, aBytesRead, EReadType);
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}
	
void CDataUnit::ExtractDataFileL(RFile& aFile, TInt aFileIndex)
	{	
	if ((aFileIndex < 0) || (aFileIndex >= iCompressedFiles.Count()))
		{
		User::Leave(KErrCorrupt);
		}
	
	iCompressedFiles[aFileIndex]->ExtractDataFieldL(aFile);
	}

void CDataUnit::ExtractDataFileL(RFile& aFile, TInt aFileIndex, TInt64 aLength)
	{
	if ((aFileIndex < 0) || (aFileIndex >= iCompressedFiles.Count()))
		{
		User::Leave(KErrCorrupt);
		}
	iCompressedFiles[aFileIndex]->ExtractDataFieldL(aFile, aLength);
	}
