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
* Definition of the Swi::Sis::CBlob
*
*/


#include <e32def.h>
#include "sisblob.h"
#include "sisfieldtypes.h"

using namespace Swi::Sis;

EXPORT_C /*static*/ CBlob* CBlob::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CBlob* self = new(ELeave) CBlob();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CBlob* CBlob::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CBlob* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CBlob* CBlob::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CBlob* self = new (ELeave) CBlob;
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C CBlob* CBlob::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CBlob* self = CBlob::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CBlob::CBlob()
	{
	}

EXPORT_C CBlob::~CBlob()
	{
	delete iData;
	}

void CBlob::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeBlob, aBytesRead, aTypeReadBehaviour);

	if ((Length() >= KMaxTInt / 2)||(Length() < 0))
		{
		User::Leave(KErrSISFieldLengthInvalid);
		}
	iData= HBufC8::NewMaxL(I64INT(Length()));
	
	TPtr8 p(iData->Des());
	CField::CheckedReadL(aDataProvider, p, aBytesRead);
	iDataPtr.Set(*iData);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}

void CBlob::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeBlob, aBytesRead, aTypeReadBehaviour);

	if ((Length() >= KMaxTInt / 2)||(Length() < 0))
		{
		User::Leave(KErrSISFieldLengthInvalid);
		}
	iDataPtr.Set(aDataProvider.ReadL(I64INT(Length())));
	aBytesRead += I64INT(Length());
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}
