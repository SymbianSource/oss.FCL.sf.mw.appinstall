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
* CSisRegistryProperty class implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#include <s32strm.h>
#include "sisregistryproperty.h"
#include "sisproperty.h"

using namespace Swi;


EXPORT_C CSisRegistryProperty* CSisRegistryProperty::NewL(const Sis::CProperty& aProperty)
	{
	CSisRegistryProperty* self = NewLC(aProperty);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSisRegistryProperty* CSisRegistryProperty::NewLC(const Sis::CProperty& aProperty)
	{
	CSisRegistryProperty* self = new(ELeave) CSisRegistryProperty();
	CleanupStack::PushL(self);
	self->ConstructL(aProperty);
	return self;
	}

EXPORT_C CSisRegistryProperty* CSisRegistryProperty::NewL(RReadStream& aStream)
	{
	CSisRegistryProperty* self = NewLC(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSisRegistryProperty* CSisRegistryProperty::NewLC(RReadStream& aStream)
	{
	CSisRegistryProperty* self = new(ELeave) CSisRegistryProperty();
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}


void CSisRegistryProperty::ConstructL(const Sis::CProperty& aProperty)
	{
	iKey = aProperty.Key();
	iValue = aProperty.Value();
	}

void CSisRegistryProperty::ConstructL(RReadStream& aStream)
	{
	InternalizeL(aStream);
	}

EXPORT_C void CSisRegistryProperty::InternalizeL(RReadStream& aStream)
	{
	iKey = aStream.ReadInt32L();
	iValue = aStream.ReadInt32L();
	}
	
EXPORT_C void CSisRegistryProperty::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iKey);
	aStream.WriteInt32L(iValue);
	}
