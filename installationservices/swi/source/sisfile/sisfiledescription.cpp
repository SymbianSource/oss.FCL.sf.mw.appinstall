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
* Definition of the Swi::Sis::CFileDescription
*
*/


/**
 @file sisfiledescription.cpp
*/

#include "sisfiledescription.h"
#include "sisstring.h"
#include "sishash.h"
#include "siscapabilities.h"

using namespace Swi::Sis;

EXPORT_C /*static*/ CFileDescription* CFileDescription::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CFileDescription* self = new(ELeave) CFileDescription();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CFileDescription* CFileDescription::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CFileDescription* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CFileDescription* CFileDescription::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CFileDescription* self = new (ELeave) CFileDescription;
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C CFileDescription* CFileDescription::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CFileDescription* self = CFileDescription::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CFileDescription::CFileDescription()
	{
	}

EXPORT_C CFileDescription::~CFileDescription()
	{
	delete iTarget;
	delete iMimeType;
	delete iCapabilities;
	delete iHash;
	}

void CFileDescription::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeFileDescription, aBytesRead, aTypeReadBehaviour);

	iTarget=CString::NewL(aDataProvider, aBytesRead);
	iMimeType=CString::NewL(aDataProvider, aBytesRead);

	TFieldType fieldType;
	CField::ReadEnumL<TFieldType,TUint32>(aDataProvider, fieldType, aBytesRead);
	
	if (fieldType == EFieldTypeCapabilities)
		{
		// capabilities present
		iCapabilities=CCapabilities::NewL(aDataProvider, aBytesRead, EAssumeType);
		iHash=CHash::NewL(aDataProvider, aBytesRead);
		}
	else if (fieldType == EFieldTypeHash)
		{
		iHash=CHash::NewL(aDataProvider, aBytesRead, EAssumeType);
		}
	else
		{
		User::Leave(KErrSISUnexpectedFieldType);
		}
	
	CField::ReadEnumL<TSISFileOperation,TUint32>(aDataProvider, iOperation, aBytesRead);
	CField::ReadEnumL<TSISFileOperationOptions ,TUint32>(aDataProvider, iOperationOptions, aBytesRead);
	
	CField::ReadTTypeL(aDataProvider, iDataLength, aBytesRead);
	CField::ReadTTypeL(aDataProvider, iUncompressedLength, aBytesRead);

	CField::ReadTTypeL(aDataProvider, iIndex, aBytesRead);
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}
	
void CFileDescription::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeFileDescription, aBytesRead, aTypeReadBehaviour);

	iTarget=CString::NewL(aDataProvider, aBytesRead);
	iMimeType=CString::NewL(aDataProvider, aBytesRead);

	TFieldType fieldType;
	CField::ReadEnumL<TFieldType,TUint32>(aDataProvider, fieldType, aBytesRead);
	
	if (fieldType == EFieldTypeCapabilities)
		{
		// capabilities present
		iCapabilities=CCapabilities::NewL(aDataProvider, aBytesRead, EAssumeType);
		iHash=CHash::NewL(aDataProvider, aBytesRead);
		}
	else if (fieldType == EFieldTypeHash)
		{
		iHash=CHash::NewL(aDataProvider, aBytesRead, EAssumeType);
		}
	else
		{
		User::Leave(KErrSISUnexpectedFieldType);
		}
	
	CField::ReadEnumL<TSISFileOperation,TUint32>(aDataProvider, iOperation, aBytesRead);
	CField::ReadEnumL<TSISFileOperationOptions ,TUint32>(aDataProvider, iOperationOptions, aBytesRead);
	
	CField::ReadTTypeL(aDataProvider, iDataLength, aBytesRead);
	CField::ReadTTypeL(aDataProvider, iUncompressedLength, aBytesRead);

	CField::ReadTTypeL(aDataProvider, iIndex, aBytesRead);
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}

