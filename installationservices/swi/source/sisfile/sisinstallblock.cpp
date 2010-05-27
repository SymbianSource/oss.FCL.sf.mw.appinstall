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
* Definition of the Swi::Sis::CInstallBlock
*
*/


/**
 @file sisinstallblock.cpp
*/

#include "sisinstallblock.h"

using namespace Swi::Sis;

EXPORT_C /*static*/ CInstallBlock* CInstallBlock::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CInstallBlock* self = new(ELeave) CInstallBlock();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CInstallBlock* CInstallBlock::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CInstallBlock* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CInstallBlock* CInstallBlock::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CInstallBlock* self = new (ELeave) CInstallBlock;
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C CInstallBlock* CInstallBlock::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CInstallBlock* self = CInstallBlock::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CInstallBlock::CInstallBlock()
	{
	}

EXPORT_C CInstallBlock::~CInstallBlock()
	{
	iFileDescriptions.ResetAndDestroy();
	iEmbeddedControllers.ResetAndDestroy();
	iIfStatements.ResetAndDestroy();
	}

void CInstallBlock::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeInstallBlock, aBytesRead, aTypeReadBehaviour);

	ReadMemberArrayL(aDataProvider, iFileDescriptions, EFieldTypeFileDescription, aBytesRead);

	ReadMemberArrayL(aDataProvider, iEmbeddedControllers, EFieldTypeController, aBytesRead);

	ReadMemberArrayL(aDataProvider, iIfStatements, EFieldTypeIf, aBytesRead);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}

void CInstallBlock::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeInstallBlock, aBytesRead, aTypeReadBehaviour);

	ReadMemberArrayL(aDataProvider, iFileDescriptions, EFieldTypeFileDescription, aBytesRead);

	ReadMemberArrayL(aDataProvider, iEmbeddedControllers, EFieldTypeController, aBytesRead);

	ReadMemberArrayL(aDataProvider, iIfStatements, EFieldTypeIf, aBytesRead);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}
