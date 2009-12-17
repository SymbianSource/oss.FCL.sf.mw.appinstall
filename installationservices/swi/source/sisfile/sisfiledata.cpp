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
* Definition of the Swi::Sis::CFileData
*
*/


#include "sisfiledata.h"
#include "siscompressed.h"

using namespace Swi::Sis;

/*static*/ CFileData* CFileData::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CFileData* self = new(ELeave) CFileData();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

/*static*/ CFileData* CFileData::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CFileData* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	} 

CFileData::CFileData()
	{
	}

CFileData::~CFileData()
	{
	delete iCompressedFile;	
	}

void CFileData::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeFileData, aBytesRead, aTypeReadBehaviour);

	iCompressedFile = CCompressed::NewL(aDataProvider, aBytesRead, EReadType); // we cannot assume the type here!
		
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}

void CFileData::ExtractDataFieldL(RFile& aFile)
	{
	iCompressedFile->ExtractDataFieldL(aFile);
	}

void CFileData::ExtractDataFieldL(RFile& aFile, TInt64 aLength)
	{
	iCompressedFile->ExtractDataFieldL(aFile, aLength);
	}

