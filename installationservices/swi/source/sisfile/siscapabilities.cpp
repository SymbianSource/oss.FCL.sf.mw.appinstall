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
* Definition of the Swi::Sis::CCapabilities
*
*/


#include <e32def.h>
#include "siscapabilities.h"
#include "sisfieldtypes.h"

using namespace Swi::Sis;

EXPORT_C /*static*/ CCapabilities* CCapabilities::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CCapabilities* self = new(ELeave) CCapabilities();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CCapabilities* CCapabilities::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CCapabilities* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CCapabilities* CCapabilities::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CCapabilities* self = new (ELeave) CCapabilities;
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C CCapabilities* CCapabilities::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CCapabilities* self = CCapabilities::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CCapabilities::CCapabilities()
	{
	}

EXPORT_C CCapabilities::~CCapabilities()
	{
	delete iData;
	}

void CCapabilities::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeCapabilities, aBytesRead, aTypeReadBehaviour);

	// Length has to be a multiple of four bytes 
	if ((Length() >= KMaxTInt / 2)||(Length() < 0) || (I64LOW(Length()) & 0x3))
		{
		User::Leave(KErrSISFieldLengthInvalid);
		}
	
	iData= HBufC8::NewMaxL(I64INT(Length()));
	
	TPtr8 p(iData->Des());
	iDataPtr.Set(*iData);
	CField::CheckedReadL(aDataProvider, p, aBytesRead);

	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}
	
void CCapabilities::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeCapabilities, aBytesRead, aTypeReadBehaviour);

	// Length has to be a multiple of four bytes 
	if ((Length() >= KMaxTInt / 2)||(Length() < 0) || (I64LOW(Length()) & 0x3))
		{
		User::Leave(KErrSISFieldLengthInvalid);
		}
	
	iDataPtr.Set(aDataProvider.ReadL(I64LOW(Length())));
	aBytesRead += I64LOW(Length());

	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}

