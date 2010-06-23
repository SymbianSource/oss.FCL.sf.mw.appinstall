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
* Definition of the Swi::CHashContainer
*
*/




/**
 @file 
*/

#include <e32def.h>
#include <s32strm.h>

#include "sishash.h"
#include "hashcontainer.h"

using namespace Swi;

EXPORT_C CHashContainer* CHashContainer::NewL(const Sis::CHash& aHash)
	{
	CHashContainer* self = NewLC(aHash);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CHashContainer* CHashContainer::NewLC(const Sis::CHash& aHash)
	{
	CHashContainer* self = new(ELeave) CHashContainer();
	CleanupStack::PushL(self);
	self->ConstructL(aHash);
	return self;
	}

EXPORT_C CHashContainer* CHashContainer::NewL(const CMessageDigest::THashId aAlgorithm, const TDesC8& aData)
	{
	CHashContainer* self = NewLC(aAlgorithm, aData);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CHashContainer* CHashContainer::NewLC(const CMessageDigest::THashId aAlgorithm, const TDesC8& aData)
	{
	CHashContainer* self = new(ELeave) CHashContainer(aAlgorithm);
	CleanupStack::PushL(self);
	self->ConstructL(aData);
	return self;
	}

EXPORT_C CHashContainer* CHashContainer::NewL(RReadStream& aStream)
	{
	CHashContainer* self = NewLC(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CHashContainer* CHashContainer::NewLC(RReadStream& aStream)
	{
	CHashContainer* self = new(ELeave) CHashContainer();
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

CHashContainer::CHashContainer()
	{
	}

CHashContainer::CHashContainer(const CMessageDigest::THashId aAlgorithm)
	: iAlgorithm(aAlgorithm)
	{
	}

EXPORT_C CHashContainer::~CHashContainer()
	{
	delete iData;
	}

void CHashContainer::ConstructL(const TDesC8& aData)
	{
	iData = aData.AllocL();
	}

void CHashContainer::ConstructL(const Sis::CHash& aHash)
	{
	// in this case one has to translate from Sis::EHashAlg*** to CMessageDigest::E*** 
	// algorithms one would need clear translation as iAlgorithm is of type CMessageDigest::THashId.
	// In the preceeding code CHashContainer(const CMessageDigest::THashId aAlgorithm) no 
	// such translation is needed. Note this issue for code maintenance i.e. 
	// when adding other algorithms  
	switch (aHash.Algorithm())
		{
		case Sis::EHashAlgSHA1:
			iAlgorithm = CMessageDigest::ESHA1;
			break;

		default:
			User::Leave(KErrNotSupported);			
			break;
		}
		
	iData = aHash.Data().AllocL();
	}

void CHashContainer::ConstructL(RReadStream& aStream)
	{
	InternalizeL(aStream);
	}

EXPORT_C void CHashContainer::InternalizeL(RReadStream& aStream)
	{
	iAlgorithm = static_cast<CMessageDigest::THashId>(aStream.ReadInt32L());
	iData = HBufC8::NewL(aStream, KMaxTInt);
	}

EXPORT_C void CHashContainer::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(static_cast<TInt32>(iAlgorithm));
	aStream << *iData;
	}
