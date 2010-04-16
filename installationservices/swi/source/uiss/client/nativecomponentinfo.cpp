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

CNativeComponentInfo::CNativeApplicationInfo::CNativeApplicationInfo()
    {
    }

EXPORT_C CNativeComponentInfo::CNativeApplicationInfo::~CNativeApplicationInfo()
    {
    delete iName;
    delete iGroupName;
    delete iIconFileName;
    }

EXPORT_C CNativeComponentInfo::CNativeApplicationInfo* CNativeComponentInfo::CNativeApplicationInfo::NewLC(const TUid& aAppUid, const TDesC& aName, const TDesC& aGroupName, const TDesC& aIconFileName)
    {
        _LIT(emptyString,"");
        // Leave if aName, aGroupName or aIconFileName exceeds KMaxDescriptorLength
       if ((&aName != NULL && aName.Length() > KMaxDescriptorLength) || 
               (&aGroupName != NULL && aGroupName.Length() > KMaxDescriptorLength) ||
               (&aIconFileName != NULL && aIconFileName.Length() > KMaxDescriptorLength))
            {
            User::Leave(KErrOverflow);
            }
        
    CNativeComponentInfo::CNativeApplicationInfo* self = new (ELeave) CNativeComponentInfo::CNativeApplicationInfo();
        CleanupStack::PushL(self);

        self->iAppUid = aAppUid;  
        if(&aName == NULL)
            {
            self->iName = emptyString().AllocL();
            }
        else
            {
            self->iName = aName.AllocL();
            }
   
        if(&aGroupName == NULL)
            {
            self->iGroupName = emptyString().AllocL();
            }
        else
            {
            self->iGroupName = aGroupName.AllocL();
            }
        
        if(&aIconFileName == NULL)
            {
            self->iIconFileName = emptyString().AllocL();
            }
        else
            {
            self->iIconFileName = aIconFileName.AllocL();
            }
        
        return self;
    }

EXPORT_C const TUid& CNativeComponentInfo::CNativeApplicationInfo::AppUid() const
    {
    return iAppUid;
    }

EXPORT_C const TDesC& CNativeComponentInfo::CNativeApplicationInfo::Name() const
    {
    return *iName;
    }

EXPORT_C const TDesC& CNativeComponentInfo::CNativeApplicationInfo::GroupName() const
    {
    return *iGroupName;
    }

EXPORT_C const TDesC& CNativeComponentInfo::CNativeApplicationInfo::IconFileName() const            
    {
    return *iIconFileName;
    }

CNativeComponentInfo::CNativeApplicationInfo* CNativeComponentInfo::CNativeApplicationInfo::NewL(RReadStream& aStream)
    {    
    CNativeComponentInfo::CNativeApplicationInfo* self = new (ELeave) CNativeComponentInfo::CNativeApplicationInfo();
    CleanupStack::PushL(self);
    
    self->iAppUid = TUid::Uid(aStream.ReadInt32L());
    self->iName = HBufC::NewL(aStream, KMaxDescriptorLength);
    self->iGroupName = HBufC::NewL(aStream, KMaxDescriptorLength);
    self->iIconFileName = HBufC::NewL(aStream, KMaxDescriptorLength);
    
    CleanupStack::Pop(self);
    return self;
    }

void CNativeComponentInfo::CNativeApplicationInfo::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L(iAppUid.iUid);    
    aStream << *iName;
    aStream << *iGroupName;
    aStream << *iIconFileName;
    }

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
	iApplications.ResetAndDestroy();
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
	aStream.WriteInt32L(iIsDriveSelectionRequired);
	
	const TInt numApplications = iApplications.Count();
	aStream.WriteInt32L(numApplications);
	for (TInt i=0; i<numApplications; ++i)
	    {
	    iApplications[i]->ExternalizeL(aStream);
	    }
	
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
	iIsDriveSelectionRequired = aStream.ReadInt32L();
	
	const TInt numApplications = aStream.ReadInt32L();
	for (TInt i=0; i<numApplications; ++i)
	    {
	    Swi::CNativeComponentInfo::CNativeApplicationInfo* app =  Swi::CNativeComponentInfo::CNativeApplicationInfo::NewL(aStream);
	    CleanupStack::PushL(app);
	    iApplications.AppendL(app);
	    CleanupStack::Pop(app);
	    }
	
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
