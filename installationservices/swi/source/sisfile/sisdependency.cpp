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
* Definition of the Swi::Sis::CDependency
*
*/


/**
 @file 
*/

#include "sisdependency.h"
#include "sisuid.h"
#include "sisversionrange.h"
#include "sisstring.h"

using namespace Swi::Sis;

EXPORT_C /*static*/ CDependency* CDependency::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CDependency* self = new(ELeave) CDependency();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CDependency* CDependency::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CDependency* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CDependency* CDependency::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CDependency* self = new (ELeave) CDependency;
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C CDependency* CDependency::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CDependency* self = CDependency::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CDependency::CDependency()
	{
	}

EXPORT_C CDependency::~CDependency()
	{
	delete iUid;
	delete iVersionRange;
	iNames.ResetAndDestroy();
	}

void CDependency::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeDependency, aBytesRead, aTypeReadBehaviour);

	iUid=CUid::NewL(aDataProvider, aBytesRead);
	
	TFieldType fieldType;
	CField::ReadEnumL<TFieldType,TUint32>(aDataProvider, fieldType, aBytesRead);

	if (fieldType==EFieldTypeVersionRange)
		{
		iVersionRange=CVersionRange::NewL(aDataProvider, aBytesRead, EAssumeType);
		
		ReadMemberArrayL(aDataProvider, iNames, EFieldTypeString, aBytesRead);
		}
	else if (fieldType==EFieldTypeArray)
		{
		ReadMemberArrayL(aDataProvider, iNames, EFieldTypeString, aBytesRead, EAssumeType);
		}
	else
		{
		User::Leave(KErrSISUnexpectedFieldType);
		}
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}
	
void CDependency::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeDependency, aBytesRead, aTypeReadBehaviour);

	iUid=CUid::NewL(aDataProvider, aBytesRead);
	
	TFieldType fieldType;
	CField::ReadEnumL<TFieldType,TUint32>(aDataProvider, fieldType, aBytesRead);

	if (fieldType==EFieldTypeVersionRange)
		{
		iVersionRange=CVersionRange::NewL(aDataProvider, aBytesRead, EAssumeType);
		
		ReadMemberArrayL(aDataProvider, iNames, EFieldTypeString, aBytesRead);
		}
	else if (fieldType==EFieldTypeArray)
		{
		ReadMemberArrayL(aDataProvider, iNames, EFieldTypeString, aBytesRead, EAssumeType);
		}
	else
		{
		User::Leave(KErrSISUnexpectedFieldType);
		}
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}

