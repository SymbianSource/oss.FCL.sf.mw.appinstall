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
* Definition of the Swi::Sis::CPrerequisite
*
*/


/**
 @file sisprerequisites.cpp
*/

#include "sisprerequisites.h"
#include "sisdependency.h"

using namespace Swi::Sis;

EXPORT_C /*static*/ CPrerequisites* CPrerequisites::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CPrerequisites* self = new(ELeave) CPrerequisites();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CPrerequisites* CPrerequisites::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CPrerequisites* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CPrerequisites* CPrerequisites::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CPrerequisites* self = new (ELeave) CPrerequisites;
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C CPrerequisites* CPrerequisites::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CPrerequisites* self = CPrerequisites::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CPrerequisites::CPrerequisites()
	{
	}

EXPORT_C CPrerequisites::~CPrerequisites()
	{
	iTargetDevices.ResetAndDestroy();
	iDependencies.ResetAndDestroy();
	}

void CPrerequisites::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypePrerequisites, aBytesRead, aTypeReadBehaviour);

	ReadMemberArrayL(aDataProvider, iTargetDevices, EFieldTypeDependency, aBytesRead);
	ReadMemberArrayL(aDataProvider, iDependencies, EFieldTypeDependency, aBytesRead);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}
	
void CPrerequisites::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypePrerequisites, aBytesRead, aTypeReadBehaviour);

	ReadMemberArrayL(aDataProvider, iTargetDevices, EFieldTypeDependency, aBytesRead);
	ReadMemberArrayL(aDataProvider, iDependencies, EFieldTypeDependency, aBytesRead);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}

