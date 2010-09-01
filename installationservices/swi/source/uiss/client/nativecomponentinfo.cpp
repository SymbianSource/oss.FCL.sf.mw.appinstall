/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file defines the component info structure of the Symbian native components.
*
*/


/**
 @file
 @internalTechnology
 @released
*/
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

#include "nativecomponentinfo.h"

namespace
	{
	TInt PackCapabilitySet(const TCapabilitySet& aCapSet)
		{
		TInt caps=0;
		for (TInt c=0; c<ECapability_Limit; ++c)
			{
			if (aCapSet.HasCapability(TCapability(c)))
				{
				caps+=(1<<c);
				}
			}
		return caps;
		}

	void UnpackCapabilitySet(TInt aPackedCapSet, TCapabilitySet& aCapSet)
		{
		for (TInt c=0; c<ECapability_Limit; ++c)
			{
			const TInt cap = 1<<c;
			if (aPackedCapSet&cap)
				{
				aCapSet.AddCapability(TCapability(c));
				}
			}
		}
	}

namespace Swi
{
CNativeComponentInfo::CNativeComponentInfo() : iAuthenticity(Usif::ENotAuthenticated)
	{
	iUserGrantableCaps.SetEmpty();
	}

EXPORT_C CNativeComponentInfo::~CNativeComponentInfo()
	{	
	delete iComponentName;
	delete iGlobalComponentId;
	delete iVersion;
	delete iVendor;
	
	iChildren.Close();
	}

EXPORT_C CNativeComponentInfo* CNativeComponentInfo::NewL()
	{
	return new (ELeave) CNativeComponentInfo;
	}

EXPORT_C CNativeComponentInfo* CNativeComponentInfo::NewLC()
	{
	CNativeComponentInfo* self = new (ELeave) CNativeComponentInfo;
	CleanupStack::PushL(self);
	return self;
	}
	
EXPORT_C void CNativeComponentInfo::ExternalizeL(RWriteStream& aStream) const
	{	
	aStream.WriteInt8L(static_cast<TUint8>(iComponentName->Length()));
	aStream.WriteL(*iComponentName);
			
	aStream.WriteInt8L(static_cast<TUint8>(iVersion->Length()));
	aStream.WriteL(*iVersion);
		
	aStream.WriteInt8L(static_cast<TUint8>(iVendor->Length()));
	aStream.WriteL(*iVendor);
	
	aStream.WriteInt8L(static_cast<TUint8>(iGlobalComponentId->Length()));
	aStream.WriteL(*iGlobalComponentId);	
	
	aStream.WriteInt32L(iScomoState);
	aStream.WriteInt32L(iInstallStatus);
	aStream.WriteInt32L(iComponentId);
	aStream.WriteInt32L(static_cast<TInt>(iAuthenticity));
	aStream.WriteInt32L(PackCapabilitySet(iUserGrantableCaps));
	aStream.WriteInt32L(iMaxInstalledSize);
	aStream.WriteInt32L(iHasExe);
	
	// Recursively internalize all other childrens as well.
	const TInt numChildren = iChildren.Count();
	aStream.WriteInt32L(numChildren);
	for (TInt child = 0; child < numChildren; ++child)
		{
		iChildren[child]->ExternalizeL(aStream);
		}
	}
	
EXPORT_C void CNativeComponentInfo::InternalizeL(RReadStream& aStream)
	{	
	TUint8 nextContentLength = 0;
	TPtr bufPtr(0,0);
	
	// Read ComponentName
	nextContentLength = aStream.ReadUint8L();
	iComponentName = HBufC::NewL(nextContentLength);
	bufPtr.Set(iComponentName->Des());
	aStream.ReadL(bufPtr, nextContentLength);		
		
	// Read Version
	nextContentLength = aStream.ReadUint8L();	
	iVersion = HBufC::NewL(nextContentLength);
	bufPtr.Set(iVersion->Des());
	aStream.ReadL(bufPtr, nextContentLength);

	// Read Vendor	
	nextContentLength = aStream.ReadUint8L();
	iVendor = HBufC::NewL(nextContentLength);
	bufPtr.Set(iVendor->Des());
	aStream.ReadL(bufPtr, nextContentLength);		

	// Read GlobalComponentId
	nextContentLength = aStream.ReadUint8L();
	iGlobalComponentId = HBufC::NewL(nextContentLength);
	bufPtr.Set(iGlobalComponentId->Des());
	aStream.ReadL(bufPtr, nextContentLength);
	
	iScomoState = static_cast<Usif::TScomoState>(aStream.ReadInt32L());
	iInstallStatus = static_cast<Usif::TInstallStatus>(aStream.ReadInt32L());
	iComponentId = aStream.ReadInt32L();
	iAuthenticity = static_cast<Usif::TAuthenticity>(aStream.ReadInt32L());
	TInt capsBitMask = aStream.ReadInt32L();
	UnpackCapabilitySet(capsBitMask, iUserGrantableCaps);	
	iMaxInstalledSize = aStream.ReadInt32L();
	iHasExe = aStream.ReadInt32L();
	
	// Recursively internalize all other childrens as well.
	const TInt numChildren = aStream.ReadInt32L();
	for (TInt child = 0; child < numChildren; ++child)
		{
		CNativeComponentInfo* nativeCompInfo = CNativeComponentInfo::NewLC();		
		nativeCompInfo->InternalizeL(aStream);
		iChildren.AppendL(nativeCompInfo);
		CleanupStack::Pop(nativeCompInfo);
		}
	}
}
#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
