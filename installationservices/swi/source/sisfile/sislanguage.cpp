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
* Definition of the Swi::Sis::CLanguage
*
*/


/**
 @file sislanguage.cpp
*/

#include "sislanguage.h"

using namespace Swi::Sis;


EXPORT_C /*static*/ CLanguage* CLanguage::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CLanguage* self = new(ELeave) CLanguage();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CLanguage* CLanguage::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CLanguage* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CLanguage* CLanguage::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CLanguage* self = new (ELeave) CLanguage;
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C CLanguage* CLanguage::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CLanguage* self = CLanguage::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CLanguage::CLanguage()
	{
	}

EXPORT_C CLanguage::~CLanguage()
	{
	}

void CLanguage::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeLanguage, aBytesRead, aTypeReadBehaviour);

	CField::ReadEnumL<TLanguage,TUint32>(aDataProvider, iLanguage, aBytesRead);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}
	
void CLanguage::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeLanguage, aBytesRead, aTypeReadBehaviour);

	CField::ReadEnumL<TLanguage,TUint32>(aDataProvider, iLanguage, aBytesRead);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}



