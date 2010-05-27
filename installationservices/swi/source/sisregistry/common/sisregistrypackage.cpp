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
* sisregistrydependency.h
* CSisRegistryPackage class implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include <s32strm.h>
#include "sisregistrypackage.h"

using namespace Swi;


EXPORT_C CSisRegistryPackage* CSisRegistryPackage::NewL(TUid aUid, const TDesC& aName, const TDesC& aVendor)
	{
	CSisRegistryPackage* self = CSisRegistryPackage::NewLC(aUid, aName, aVendor);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSisRegistryPackage* CSisRegistryPackage::NewLC(TUid aUid, const TDesC& aName, const TDesC& aVendor)
	{
	CSisRegistryPackage* self = new(ELeave) CSisRegistryPackage(aUid);
	CleanupStack::PushL(self);
	self->ConstructL(aName, aVendor);
	return self;
	}

EXPORT_C CSisRegistryPackage* CSisRegistryPackage::NewL(RReadStream& aStream)
	{
	CSisRegistryPackage* self = CSisRegistryPackage::NewLC(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSisRegistryPackage* CSisRegistryPackage::NewLC(RReadStream& aStream)
	{
	CSisRegistryPackage* self = new(ELeave) CSisRegistryPackage();
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

EXPORT_C CSisRegistryPackage* CSisRegistryPackage::NewL(const CSisRegistryPackage& aPackage)
	{
	CSisRegistryPackage* self = CSisRegistryPackage::NewLC(aPackage);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSisRegistryPackage* CSisRegistryPackage::NewLC(const CSisRegistryPackage& aPackage)
	{
	CSisRegistryPackage* self = new(ELeave) CSisRegistryPackage();
	CleanupStack::PushL(self);
	self->ConstructL(aPackage);
	return self;
	}
	
CSisRegistryPackage::CSisRegistryPackage(TUid aUid) : iUid(aUid)
	{
	}

CSisRegistryPackage::CSisRegistryPackage()
	{
	}

EXPORT_C CSisRegistryPackage::~CSisRegistryPackage()
	{
	delete iPackageName;
	delete iVendorName;
	}

void CSisRegistryPackage::ConstructL(const TDesC& aName, const TDesC& aVendor)
	{
	iPackageName = aName.AllocL();
	iVendorName  = aVendor.AllocL();
	iIndex       = UnInitializedIndex;
	}
	
void CSisRegistryPackage::ConstructL(const CSisRegistryPackage& aPackage)
	{
	iPackageName = aPackage.Name().AllocL();
	iVendorName  = aPackage.Vendor().AllocL();
	iUid		 = aPackage.iUid;
	iIndex       = aPackage.iIndex;
	}
	
void CSisRegistryPackage::ConstructL(RReadStream& aStream)
	{
	InternalizeL(aStream);
	}

EXPORT_C void CSisRegistryPackage::InternalizeL(RReadStream& aStream)
	{
	iUid.iUid = aStream.ReadInt32L();
	// there cannot be a sensible size limitation as names are strings
	iPackageName = HBufC::NewL(aStream, KMaxTInt);
	iVendorName = HBufC::NewL(aStream, KMaxTInt);
	iIndex = aStream.ReadInt32L();
	}
	
EXPORT_C void CSisRegistryPackage::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iUid.iUid);
	aStream << *iPackageName;
	aStream << *iVendorName;
	aStream.WriteInt32L(iIndex);
	}
	
EXPORT_C TBool CSisRegistryPackage::operator==(const CSisRegistryPackage& aOther) const
	{
	if ((iUid.iUid == aOther.iUid.iUid)	&&
	    (*iPackageName == *aOther.iPackageName) &&
	    (*iVendorName  == *aOther.iVendorName))
		{
		return ETrue;	
		}
	return EFalse;
	}

EXPORT_C void CSisRegistryPackage::SetNameL(const TDesC& aName)
	{
		delete iPackageName;
		iPackageName = aName.AllocL();
	}

