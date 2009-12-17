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
* Definition of the Swi::Sis::CCertificateChain
*
*/


/**
 @file siscertificatechain.cpp
*/

#include "siscertificatechain.h"

using namespace Swi::Sis;

EXPORT_C /*static*/ CCertificateChain* CCertificateChain::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CCertificateChain* self = new(ELeave) CCertificateChain();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CCertificateChain* CCertificateChain::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CCertificateChain* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CCertificateChain* CCertificateChain::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CCertificateChain* self = new (ELeave) CCertificateChain;
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C CCertificateChain* CCertificateChain::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CCertificateChain* self = CCertificateChain::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CCertificateChain::CCertificateChain()
	{
	}

EXPORT_C CCertificateChain::~CCertificateChain()
	{
	delete iData;
	}

void CCertificateChain::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeCertificateChain, aBytesRead, aTypeReadBehaviour);

	iData=CBlob::NewL(aDataProvider, aBytesRead);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}
	
void CCertificateChain::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeCertificateChain, aBytesRead, aTypeReadBehaviour);

	iData=CBlob::NewL(aDataProvider, aBytesRead);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}

