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
* CControllerInfo - implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include<s32strm.h>
#include "controllerinfo.h"
#include "hashcontainer.h"
#include "versionutil.h"

using namespace Swi;

EXPORT_C CControllerInfo* CControllerInfo::NewL(TVersion aVersion,
 	const CHashContainer& aHashContainer, TInt aOffset)
 	{
 	CControllerInfo* self = CControllerInfo::NewLC(aVersion, aHashContainer, aOffset);
 	CleanupStack::Pop(self);
 	return self;
 	}
 
EXPORT_C CControllerInfo* CControllerInfo::NewLC(TVersion aVersion,
 	const CHashContainer& aHashContainer, TInt aOffset)
 	{
 	CControllerInfo* self = new(ELeave) CControllerInfo(aVersion, aOffset);
 	CleanupStack::PushL(self);
 	self->ConstructL(aHashContainer);
 	return self;
 	}

EXPORT_C CControllerInfo* CControllerInfo::NewL(TVersion aVersion, const CHashContainer& aHashContainer)
	{
	CControllerInfo* self = CControllerInfo::NewLC(aVersion, aHashContainer, 0);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CControllerInfo* CControllerInfo::NewLC(TVersion aVersion, const CHashContainer& aHashContainer)
	{
	CControllerInfo* self = new(ELeave) CControllerInfo(aVersion, 0);
	CleanupStack::PushL(self);
	self->ConstructL(aHashContainer);
	return self;
	}

EXPORT_C CControllerInfo* CControllerInfo::NewL(RReadStream& aStream)
	{
	CControllerInfo* self = CControllerInfo::NewLC(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CControllerInfo* CControllerInfo::NewLC(RReadStream& aStream)
	{
	CControllerInfo* self = new(ELeave) CControllerInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

EXPORT_C CControllerInfo* CControllerInfo::NewL(const CControllerInfo& aControllerInfo)
	{
	CControllerInfo* self = CControllerInfo::NewLC(aControllerInfo);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CControllerInfo* CControllerInfo::NewLC(const CControllerInfo& aControllerInfo)
	{
	CControllerInfo* self = new(ELeave) CControllerInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aControllerInfo);
	return self;
	}

CControllerInfo::CControllerInfo(TVersion aVersion, TInt aOffset) 
 	: iVersion(aVersion), iOffset(aOffset)

	{
	}

CControllerInfo::CControllerInfo()
	{
	}

CControllerInfo::~CControllerInfo()
	{
	delete iHashContainer;
	}

void CControllerInfo::ConstructL(const CHashContainer& aHashContainer)
	{
	iHashContainer = CHashContainer::NewL(aHashContainer.Algorithm(), aHashContainer.Data());
	}
	
void CControllerInfo::ConstructL(RReadStream& aStream)
	{
	InternalizeL(aStream);
	}
	
void CControllerInfo::ConstructL(const CControllerInfo& aControllerInfo)
	{
	iVersion = aControllerInfo.Version();
	iOffset = aControllerInfo.Offset();
	iHashContainer = CHashContainer::NewL(aControllerInfo.HashContainer().Algorithm()
										,aControllerInfo.HashContainer().Data());
	}
EXPORT_C void CControllerInfo::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iVersion;
 	aStream.WriteInt32L(iOffset);	
	iHashContainer->ExternalizeL(aStream);
	}

EXPORT_C void CControllerInfo::InternalizeL(RReadStream& aStream)
	{
	aStream >> iVersion;
 	iOffset = aStream.ReadInt32L();	
	iHashContainer = CHashContainer::NewL(aStream);
	}
