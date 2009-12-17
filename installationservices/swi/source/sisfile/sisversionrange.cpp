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
* Definition of the Swi::Sis::CVersionRange
*
*/


/**
 @file sisversionrange.cpp
*/

#include "sisversionrange.h"
#include "sisversion.h"

using namespace Swi::Sis;

EXPORT_C /*static*/ CVersionRange* CVersionRange::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CVersionRange* self = new(ELeave) CVersionRange();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CVersionRange* CVersionRange::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CVersionRange* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CVersionRange* CVersionRange::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CVersionRange* self = new (ELeave) CVersionRange;
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C CVersionRange* CVersionRange::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CVersionRange* self = CVersionRange::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CVersionRange::CVersionRange()
	{
	}

EXPORT_C CVersionRange::~CVersionRange()
	{
	delete iFrom;
	delete iTo;
	}

void CVersionRange::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeVersionRange, aBytesRead, aTypeReadBehaviour);

	iFrom=CVersion::NewL(aDataProvider, aBytesRead);
	
	if (Length() > iFrom->Length() + iFrom->HeaderSize() + iFrom->PaddingSize()) // Length of CVersion
		{
		iTo=CVersion::NewL(aDataProvider, aBytesRead);
		}
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);;
	}
	
void CVersionRange::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeVersionRange, aBytesRead, aTypeReadBehaviour);

	iFrom=CVersion::NewL(aDataProvider, aBytesRead);
	
	if (Length() > iFrom->Length() + iFrom->HeaderSize() + iFrom->PaddingSize()) // Length of CVersion
		{
		iTo=CVersion::NewL(aDataProvider, aBytesRead);
		}
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);;
	}

