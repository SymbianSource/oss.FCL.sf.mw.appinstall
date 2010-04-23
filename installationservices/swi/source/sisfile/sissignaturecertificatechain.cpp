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
* Definition of the Swi::Sis::CSignatureCertificateChain
*
*/


/**
 @file sissignaturecertificatechain.cpp
*/

#include "sissignaturecertificatechain.h"
#include "siscertificatechain.h"

using namespace Swi::Sis;

EXPORT_C /*static*/ CSignatureCertificateChain* CSignatureCertificateChain::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CSignatureCertificateChain* self = new(ELeave) CSignatureCertificateChain();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CSignatureCertificateChain* CSignatureCertificateChain::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CSignatureCertificateChain* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CSignatureCertificateChain* CSignatureCertificateChain::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CSignatureCertificateChain* self = new (ELeave) CSignatureCertificateChain;
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C CSignatureCertificateChain* CSignatureCertificateChain::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CSignatureCertificateChain* self = CSignatureCertificateChain::NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CSignatureCertificateChain::CSignatureCertificateChain()
	{
	}

EXPORT_C CSignatureCertificateChain::~CSignatureCertificateChain()
	{
	delete iCertificateChain;
	iSignatures.ResetAndDestroy();
	}

void CSignatureCertificateChain::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeSignatureCertificateChain, aBytesRead, aTypeReadBehaviour);

	ReadMemberArrayL(aDataProvider, iSignatures, EFieldTypeSignature, aBytesRead);
	iCertificateChain=CCertificateChain::NewL(aDataProvider, aBytesRead);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}
	
void CSignatureCertificateChain::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeSignatureCertificateChain, aBytesRead, aTypeReadBehaviour);

	ReadMemberArrayL(aDataProvider, iSignatures, EFieldTypeSignature, aBytesRead);
	iCertificateChain=CCertificateChain::NewL(aDataProvider, aBytesRead);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);	
	}

