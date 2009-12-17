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
* Definition of the Swi::Sis::CDate
*
*/


/**
 @file sisdate.cpp
*/

#include "sisdate.h"

using namespace Swi::Sis;

EXPORT_C /*static*/ CDate* CDate::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CDate* self = new(ELeave) CDate();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CDate* CDate::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CDate* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDate* CDate::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CDate* self = CDate::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDate* CDate::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CDate* self = new (ELeave) CDate;
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

CDate::CDate()
	{
	}

EXPORT_C CDate::~CDate()
	{
	}

void CDate::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeDate, aBytesRead, aTypeReadBehaviour);

	CField::ReadTTypeL(aDataProvider, iYear, aBytesRead);
	CField::ReadTTypeL(aDataProvider, iMonth, aBytesRead);
	CField::ReadTTypeL(aDataProvider, iDay, aBytesRead);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}
	
void CDate::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeDate, aBytesRead, aTypeReadBehaviour);

	CField::ReadTTypeL(aDataProvider, iYear, aBytesRead);
	CField::ReadTTypeL(aDataProvider, iMonth, aBytesRead);
	CField::ReadTTypeL(aDataProvider, iDay, aBytesRead);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}

