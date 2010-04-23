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
* Definition of the Swi::Sis::CString
*
*/


/**
 @file sisstring.cpp
*/

#include <e32def.h>
#include "sisstring.h"
#include "sisfieldtypes.h"
#include "sisdataprovider.h"
#include "sisinstallerrors.h"

using namespace Swi::Sis;

EXPORT_C /*static*/ CString* CString::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CString* self = new(ELeave) CString();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CString* CString::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CString* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CString* CString::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CString* self = CString::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;	
	}

EXPORT_C CString* CString::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CString* self = new (ELeave) CString;
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

CString::CString()
	{
	}

EXPORT_C CString::~CString()
	{
	delete iData;
	}

void CString::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeString, aBytesRead, aTypeReadBehaviour);

	// If string has an Odd Length, something is wrong.
	if (I64LOW(Length()) & 0x1)
		{
		User::Leave(KErrSISInvalidStringLength);
		}
	
	if (I64HIGH(Length()))
		{
		// Length is either -ve or to big for a string
		User::Leave(KErrSISStringInvalidLength);
		}
	TInt32 stringLength=I64LOW(Length()) / 2;
	iData= HBufC::NewL(stringLength);
	
	TPtr8 p(const_cast<TUint8*>(reinterpret_cast<const TUint8*>(iData->Des().Ptr())) , stringLength*2);
	p.SetLength(stringLength*2);
	CField::CheckedReadL(aDataProvider, p, stringLength*2, aBytesRead);
	iData->Des().SetLength(stringLength);
	iDataPtr.Set(*iData);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}
	
	
void CString::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	CField::ConstructL(aDataProvider, EFieldTypeString, aBytesRead, aTypeReadBehaviour);

	// If string has an Odd Length, something is wrong.
	if (I64LOW(Length()) & 0x1)
		{
		User::Leave(KErrSISInvalidStringLength);
		}
	
	if (I64HIGH(Length()))
		{
		// Length is either -ve or to big for a string
		User::Leave(KErrSISStringInvalidLength);
		}
	TInt32 stringLength=I64LOW(Length());
	
	// get a pointer to the underlying data.
	const TUint8* ptr = aDataProvider.ReadL(stringLength).Ptr();
	aBytesRead += stringLength;
	
	// cast it to a TUint16 and construct the pointer from it.
	stringLength /= 2;
	const TUint16* uPtr = reinterpret_cast<const TUint16*>(ptr);
	iDataPtr.Set(uPtr, stringLength);
	
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}
