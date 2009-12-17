/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of CInstallPrefs class.
*
*/


/**
 @file
*/

#include "swi/launcher.h"

namespace Swi
{

//
// CInstallPrefs implementation
//

EXPORT_C CInstallPrefs* CInstallPrefs::NewLC()
	{
	CInstallPrefs* self = new(ELeave) CInstallPrefs;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CInstallPrefs* CInstallPrefs::NewLC(RReadStream& aStream)
	{
	CInstallPrefs* self = new(ELeave) CInstallPrefs;
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

EXPORT_C CInstallPrefs* CInstallPrefs::NewL()
	{
	CInstallPrefs* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CInstallPrefs* CInstallPrefs::NewL(RReadStream& aStream)
	{
	CInstallPrefs* self = NewLC(aStream);
	CleanupStack::Pop(self);
	return self;
	}

void CInstallPrefs::ConstructL()
	{
	iRevocationServerUri = HBufC8::NewL(0);
	}

void CInstallPrefs::ConstructL(RReadStream& aStream)
	{
	InternalizeL(aStream);
	}

CInstallPrefs::CInstallPrefs()
	{
	}

EXPORT_C CInstallPrefs::~CInstallPrefs()
	{
	delete iRevocationServerUri;
	}

EXPORT_C void CInstallPrefs::InternalizeL(RReadStream& aStream)
	{
	iPerformRevocationCheck = aStream.ReadUint8L();
	TUint16 uriLength = aStream.ReadUint16L();
	delete iRevocationServerUri;
	iRevocationServerUri = 0;
	iRevocationServerUri=HBufC8::NewL(uriLength);
	TPtr8 p(iRevocationServerUri->Des());
	aStream.ReadL(p, uriLength);
	}
	
EXPORT_C void CInstallPrefs::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint8L(static_cast<TUint8>(iPerformRevocationCheck));
	aStream.WriteUint16L(static_cast<TUint16>(iRevocationServerUri->Length()));
	aStream.WriteL(*iRevocationServerUri);
	}
	
} // namespace Swi
