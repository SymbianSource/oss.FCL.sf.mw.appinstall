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
* Definition of the Swi::Sis::CInfo
*
*/


/**
 @file sisinfo.cpp
*/

#include "sisinfo.h"
#include "sisuid.h"
#include "sisversion.h"
#include "sisdatetime.h"

using namespace Swi::Sis;


EXPORT_C /*static*/ CInfo* CInfo::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CInfo* self = new(ELeave) CInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CInfo* CInfo::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CInfo* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CInfo* CInfo::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CInfo* self = CInfo::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CInfo* CInfo::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CInfo* self = new (ELeave) CInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

CInfo::CInfo()
	{
	}

EXPORT_C CInfo::~CInfo()
	{
	delete iUid;
	delete iUniqueVendorName;
	delete iVersion;
	delete iDateTime;
	
	iNames.ResetAndDestroy();
	iVendorNames.ResetAndDestroy();
	}

void CInfo::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeInfo, aBytesRead, aTypeReadBehaviour);
	
	iUid=CUid::NewL(aDataProvider, aBytesRead);

	iUniqueVendorName=CString::NewL(aDataProvider, aBytesRead);
	
	ReadMemberArrayL(aDataProvider, iNames, EFieldTypeString, aBytesRead);

	ReadMemberArrayL(aDataProvider, iVendorNames, EFieldTypeString, aBytesRead);

	iVersion=CVersion::NewL(aDataProvider, aBytesRead);
	
	iDateTime=CDateTime::NewL(aDataProvider, aBytesRead);
	
	CField::ReadEnumL<TInstallType,TUint8>(aDataProvider, iInstallType, aBytesRead);
	
	CField::ReadEnumL<TInstallFlags,TUint8>(aDataProvider, iInstallFlags, aBytesRead);
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}
	
void CInfo::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeInfo, aBytesRead, aTypeReadBehaviour);
	
	iUid=CUid::NewL(aDataProvider, aBytesRead);
	iUniqueVendorName=CString::NewL(aDataProvider, aBytesRead);
	
	ReadMemberArrayL(aDataProvider, iNames, EFieldTypeString, aBytesRead);
	ReadMemberArrayL(aDataProvider, iVendorNames, EFieldTypeString, aBytesRead);
	
	iVersion=CVersion::NewL(aDataProvider, aBytesRead);
	iDateTime=CDateTime::NewL(aDataProvider, aBytesRead);
	
	CField::ReadEnumL<TInstallType,TUint8>(aDataProvider, iInstallType, aBytesRead);
	CField::ReadEnumL<TInstallFlags,TUint8>(aDataProvider, iInstallFlags, aBytesRead);
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}



