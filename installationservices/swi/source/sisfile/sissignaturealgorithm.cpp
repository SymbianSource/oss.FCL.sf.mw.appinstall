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
* Definition of the Swi::Sis::CSignatureAlgorithm
*
*/


/**
 @file sissignaturealgorithm.cpp
*/

#include <e32def.h>
#include "sissignaturealgorithm.h"
#include "sisfieldtypes.h"
#include "sisstring.h"

using namespace Swi::Sis;

EXPORT_C /*static*/ CSignatureAlgorithm* CSignatureAlgorithm::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CSignatureAlgorithm* self = new(ELeave) CSignatureAlgorithm();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CSignatureAlgorithm* CSignatureAlgorithm::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CSignatureAlgorithm* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSignatureAlgorithm* CSignatureAlgorithm::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CSignatureAlgorithm* self = new (ELeave) CSignatureAlgorithm;
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C CSignatureAlgorithm* CSignatureAlgorithm::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CSignatureAlgorithm* self = CSignatureAlgorithm::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CSignatureAlgorithm::CSignatureAlgorithm()
	{
	}

EXPORT_C CSignatureAlgorithm::~CSignatureAlgorithm()
	{
	delete iAlgorithmIdentifier;
	}

void CSignatureAlgorithm::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeSignatureAlgorithm, aBytesRead, aTypeReadBehaviour);

	iAlgorithmIdentifier=CString::NewL(aDataProvider, aBytesRead);
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}
	
void CSignatureAlgorithm::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeSignatureAlgorithm, aBytesRead, aTypeReadBehaviour);

	iAlgorithmIdentifier=CString::NewL(aDataProvider, aBytesRead);
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	}

