/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <f32file.h>
#include <usif/sif/sifcommon.h>
#include <s32mem.h>
#include <scs/cleanuputils.h>
#include <scs/streamingarray.h>
#include "sifcommon_internal.h"

using namespace Usif;

/**
	The COpaqueNamedParams and CComponentInfo containers implement the deferred internalization
	logic. This mechanism is necessary because these containers may be passed through IPC between
	the SIF client and server. Hence, they must be externalized and internalized on both sides.
	However, The SIF client doesn't implement an active object and therefore it cannot internalize
	parameters returned by the SIF server. This de-serialization is done by the deferred internalization
	logic when the user accesses them first time.
	Hence, all the get methods, for example Name(), GetIntByName(), StringByNameL(), call the
	InternalizeFromExternalBufferL() method which calls ConstInternalizeL() in order to unpack
	the data recived from the SIF server if there is any. ConstInternalizeL() is a const method despite
	the fact it modifies class'es members. This is done to keep the get methods const as expected by
	the users.
 */

// ##########################################################################################

namespace
	{
	TInt PackCapabilitySet(const TCapabilitySet& aCapSet)
		{
		TInt caps=0;
		for (TInt c=0; c<ECapability_Limit; ++c)
			{
			if (aCapSet.HasCapability(TCapability(c)))
				{
				caps += (1<<c);
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

// ##########################################################################################
 
EXPORT_C TSecurityContext::TSecurityContext(const RMessage2& aMessage): iMsg(aMessage)
	{
	}

EXPORT_C TBool TSecurityContext::HasCapability(TCapability aCapability) const
	{
	return iMsg.HasCapability(aCapability);
	}

EXPORT_C void TSecurityContext::HasCapabilityL(TCapability aCapability) const
	{
	iMsg.HasCapabilityL(aCapability);
	}

EXPORT_C TSecureId TSecurityContext::SecureId() const
	{
	return iMsg.SecureId();
	}

// ##########################################################################################

CComponentInfo::CApplicationInfo::CApplicationInfo()
    {
    }

EXPORT_C CComponentInfo::CApplicationInfo::~CApplicationInfo()
    {
    delete iName;
    delete iGroupName;
    delete iIconFileName;
    }

EXPORT_C CComponentInfo::CApplicationInfo* CComponentInfo::CApplicationInfo::NewLC(const TUid& aAppUid, const TDesC& aName, const TDesC& aGroupName, const TDesC& aIconFileName)
    {
        _LIT(emptyString,"");
        // Leave if aName, aGroupName or aIconFileName exceeds KMaxDescriptorLength
       if ((&aName != NULL && aName.Length() > KMaxDescriptorLength) || 
               (&aGroupName != NULL && aGroupName.Length() > KMaxDescriptorLength) ||
               (&aIconFileName != NULL && aIconFileName.Length() > KMaxDescriptorLength))
            {
            User::Leave(KErrOverflow);
            }
        
       CComponentInfo::CApplicationInfo* self = new (ELeave) CComponentInfo::CApplicationInfo();
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

EXPORT_C const TUid& CComponentInfo::CApplicationInfo::AppUid() const
    {
    return iAppUid;
    }

EXPORT_C const TDesC& CComponentInfo::CApplicationInfo::Name() const
    {
    return *iName;
    }

EXPORT_C const TDesC& CComponentInfo::CApplicationInfo::GroupName() const
    {
    return *iGroupName;
    }

EXPORT_C const TDesC& CComponentInfo::CApplicationInfo::IconFileName() const            
    {
    return *iIconFileName;
    }

CComponentInfo::CApplicationInfo* CComponentInfo::CApplicationInfo::NewL(RReadStream& aStream)
    {    
    CApplicationInfo* self = new (ELeave) CApplicationInfo();
    CleanupStack::PushL(self);
    
    self->iAppUid = TUid::Uid(aStream.ReadInt32L());
    self->iName = HBufC::NewL(aStream, KMaxDescriptorLength);
    self->iGroupName = HBufC::NewL(aStream, KMaxDescriptorLength);
    self->iIconFileName = HBufC::NewL(aStream, KMaxDescriptorLength);
    
    CleanupStack::Pop(self);
    return self;
    }

void CComponentInfo::CApplicationInfo::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L(iAppUid.iUid);    
    aStream << *iName;
    aStream << *iGroupName;
    aStream << *iIconFileName;
    }
            
// ##########################################################################################

CComponentInfo::CNode::CNode() : iAuthenticity(ENotAuthenticated)
	{
	iUserGrantableCaps.SetEmpty();
	}

EXPORT_C CComponentInfo::CNode::~CNode()
	{
	delete iSoftwareTypeName;
	delete iComponentName;
	delete iGlobalComponentId;
	delete iVersion;
	delete iVendor;

	iApplications.Close();
	iChildren.Close();	
	}

EXPORT_C CComponentInfo::CNode* CComponentInfo::CNode::NewLC(const TDesC& aSoftwareTypeName, const TDesC& aComponentName,
															const TDesC& aVersion, const TDesC& aVendor, TScomoState aScomoState,
															TInstallStatus aInstallStatus, TComponentId aComponentId,
															const TDesC& aGlobalComponentId, TAuthenticity aAuthenticity,
															const TCapabilitySet& aUserGrantableCaps, TInt aMaxInstalledSize,
															TBool aHasExe, TBool aIsDriveSelectionRequired,
															RPointerArray<CApplicationInfo>* aApplications, RPointerArray<CNode>* aChildren)
	{
	// We leave if aSoftwareTypeName, aComponentName, aVersion, aVendor or aGlobalComponentId exceeds KMaxDescriptorLength
	if (aSoftwareTypeName.Length() > KMaxDescriptorLength ||
		aComponentName.Length() > KMaxDescriptorLength ||
		aVersion.Length() > KMaxDescriptorLength ||
		aVendor.Length() > KMaxDescriptorLength ||
		aGlobalComponentId.Length() > KMaxDescriptorLength)
		{
		User::Leave(KErrOverflow);
		}
		

	// Since the max heap buffer length is 2K, which is more than (num of parameters * KMaxDescriptorLength), we just assert this condition
	__ASSERT_ALWAYS(aSoftwareTypeName.Length() + aComponentName.Length() + aVersion.Length() + aVendor.Length() + 
					aGlobalComponentId.Length() <= KMaxHeapBufLength, User::Leave(KErrOverflow));

	// Create an instance of CNode
	CNode* self = new (ELeave) CNode();
	CleanupStack::PushL(self);

	// Copy Software Type Name, Component Name, Version, aVendor, aGlobalComponentId
	self->iSoftwareTypeName = aSoftwareTypeName.AllocL();
	self->iComponentName = aComponentName.AllocL();
	self->iVersion = aVersion.AllocL();
	self->iVendor = aVendor.AllocL();
	self->iGlobalComponentId = aGlobalComponentId.AllocL();

	// Others
	self->iScomoState = aScomoState;
	self->iInstallStatus = aInstallStatus;
	self->iComponentId = aComponentId;
	self->iAuthenticity = aAuthenticity;
	self->iUserGrantableCaps = aUserGrantableCaps;
	self->iMaxInstalledSize = aMaxInstalledSize;
	self->iHasExe = aHasExe;
	self->iIsDriveSelectionRequired = aIsDriveSelectionRequired;

	if(aApplications != NULL)
        {
        for (TInt i=aApplications->Count()-1; i>=0; --i)
            {
            self->iApplications.InsertL((*aApplications)[i], 0);
            aApplications->Remove(i);
            }
        }
	
	// Embedded Components
	if(aChildren != NULL)
		{
		for (TInt i=aChildren->Count()-1; i>=0; --i)
			{
			self->iChildren.InsertL((*aChildren)[i], 0);
			aChildren->Remove(i);
			}
		}
	
	return self;
	}

EXPORT_C void CComponentInfo::CNode::AddChildL(CComponentInfo::CNode* aChild)
	{
	if (aChild == NULL)
		{
		User::Leave(KErrArgument);
		}

	iChildren.AppendL(aChild);
	}

CComponentInfo::CNode* CComponentInfo::CNode::NewL(RReadStream& aStream)
	{
	// Create an instance of CNode
	CNode* self = new (ELeave) CNode();
	CleanupStack::PushL(self);
	
	self->iSoftwareTypeName = HBufC::NewL(aStream, KMaxDescriptorLength);
	self->iComponentName = HBufC::NewL(aStream, KMaxDescriptorLength);
	self->iVersion = HBufC::NewL(aStream, KMaxDescriptorLength);
	self->iVendor = HBufC::NewL(aStream, KMaxDescriptorLength);
	self->iGlobalComponentId = HBufC::NewL(aStream, KMaxDescriptorLength);
	self->iScomoState = static_cast<TScomoState>(aStream.ReadInt32L());
	self->iInstallStatus = static_cast<TInstallStatus>(aStream.ReadInt32L());
	self->iComponentId = aStream.ReadInt32L();
	self->iAuthenticity = static_cast<TAuthenticity>(aStream.ReadInt32L());
	UnpackCapabilitySet(aStream.ReadInt32L(), self->iUserGrantableCaps);
	self->iMaxInstalledSize = aStream.ReadInt32L();
	self->iHasExe = aStream.ReadInt32L();
	self->iIsDriveSelectionRequired = aStream.ReadInt32L();	
	
	const TInt numApplications = aStream.ReadInt32L();
    for (TInt i=0; i<numApplications; ++i)
        {
        CApplicationInfo* app = CApplicationInfo::NewL(aStream);
        CleanupStack::PushL(app);
        self->iApplications.AppendL(app);
        CleanupStack::Pop(app);
        }
	
	const TInt numChildren = aStream.ReadInt32L();
	for (TInt i=0; i<numChildren; ++i)
		{
		CNode* node = CNode::NewL(aStream);
		CleanupStack::PushL(node);
		self->iChildren.AppendL(node);
		CleanupStack::Pop(node);
		}
	
	CleanupStack::Pop(self);
	return self;
	}

void CComponentInfo::CNode::ExternalizeL(RWriteStream& aStream) const
	{
	ASSERT (iSoftwareTypeName!=NULL && iComponentName!=NULL && iVersion!=NULL && iVendor!=NULL && iGlobalComponentId!=NULL);

	aStream << *iSoftwareTypeName;
	aStream << *iComponentName;
	aStream << *iVersion;
	aStream << *iVendor;
	aStream << *iGlobalComponentId;

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
	
	const TInt numChildren = iChildren.Count();
	aStream.WriteInt32L(numChildren);
	for (TInt i=0; i<numChildren; ++i)
		{
		iChildren[i]->ExternalizeL(aStream);
		}
	}

EXPORT_C const TDesC& CComponentInfo::CNode::SoftwareTypeName() const
	{
	return *iSoftwareTypeName;
	}

EXPORT_C const TDesC& CComponentInfo::CNode::ComponentName() const
	{
	return *iComponentName;
	}

EXPORT_C const TDesC& CComponentInfo::CNode::Version() const
	{
	return *iVersion;
	}

EXPORT_C const TDesC& CComponentInfo::CNode::Vendor() const
	{
	return *iVendor;
	}

EXPORT_C TScomoState CComponentInfo::CNode::ScomoState() const
	{
	return iScomoState;
	}

EXPORT_C TInstallStatus CComponentInfo::CNode::InstallStatus() const
	{
	return iInstallStatus;
	}

EXPORT_C TComponentId CComponentInfo::CNode::ComponentId() const
	{
	return iComponentId;
	}

EXPORT_C const TDesC& CComponentInfo::CNode::GlobalComponentId() const
	{
	return *iGlobalComponentId;
	}

EXPORT_C TAuthenticity CComponentInfo::CNode::Authenticity() const
	{
	return iAuthenticity;
	}

EXPORT_C const TCapabilitySet& CComponentInfo::CNode::UserGrantableCaps() const
	{
	return iUserGrantableCaps;
	}

EXPORT_C TInt CComponentInfo::CNode::MaxInstalledSize() const
	{
	return iMaxInstalledSize;
	}

EXPORT_C TBool CComponentInfo::CNode::HasExecutable() const
	{
	return iHasExe;
	}

EXPORT_C TBool CComponentInfo::CNode::DriveSeletionRequired() const
    {
    return iIsDriveSelectionRequired;
    }

EXPORT_C const RPointerArray<CComponentInfo::CApplicationInfo>& CComponentInfo::CNode::Applications() const
    {
    return iApplications;
    }

EXPORT_C const RPointerArray<CComponentInfo::CNode>& CComponentInfo::CNode::Children() const
	{
	return iChildren;
	}

// ##########################################################################################

CComponentInfo::CComponentInfo() : iExternalBufferPtr(NULL, 0, 0)
	{
	}

EXPORT_C CComponentInfo::~CComponentInfo()
	{
	delete iRootNode;

	CleanupExternalBuffer();
	}

EXPORT_C CComponentInfo* CComponentInfo::NewL()
	{
	return new (ELeave) CComponentInfo;
	}

EXPORT_C CComponentInfo* CComponentInfo::NewLC()
	{
	CComponentInfo* self = new (ELeave) CComponentInfo;
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C const CComponentInfo::CNode& CComponentInfo::RootNodeL() const
	{
	InternalizeFromExternalBufferL();
	
	if (iRootNode == NULL)
		{
		User::Leave(KErrNotFound);
		}
	
	return *iRootNode;
	}

EXPORT_C void CComponentInfo::SetRootNodeL(CNode* aRootNode)
	{
	InternalizeFromExternalBufferL();
	
	if (aRootNode == NULL)
		{
		User::Leave(KErrArgument);
		}
	
	delete iRootNode;
	iRootNode = aRootNode;
	}

EXPORT_C void CComponentInfo::SetRootNodeAsChildL(CNode& aParentNode)
	{
	InternalizeFromExternalBufferL();
	
	if (iRootNode == NULL)
		{
		User::Leave(KErrNotFound);
		}
	
	aParentNode.AddChildL(iRootNode);
	iRootNode = NULL;
	}

EXPORT_C void CComponentInfo::ExternalizeL(RWriteStream& aStream) const
	{
	InternalizeFromExternalBufferL();
	
	aStream.WriteInt8L(iRootNode != NULL);
	
	if (iRootNode != NULL)
		{
		iRootNode->ExternalizeL(aStream);
		}
	}

void CComponentInfo::ConstInternalizeL(RReadStream& aStream) const
	{
	CNode*& rootNode = const_cast<CNode*&>(iRootNode);
	delete rootNode;
	rootNode = NULL;
	
	if (aStream.ReadInt8L() != 0)
		{
		rootNode = CNode::NewL(aStream);
		}
	}

void CComponentInfo::CleanupExternalBuffer() const
	{
	delete iExternalBuffer;
	iExternalBuffer = NULL;
	iDeferredInternalization = EFalse;
	}

void CComponentInfo::InternalizeFromExternalBufferL() const
	{
	if (iDeferredInternalization)
		{
		iDeferredInternalization = EFalse;

		RDesReadStream rs(*iExternalBuffer);
		CleanupClosePushL(rs);
		ConstInternalizeL(rs);
		CleanupStack::PopAndDestroy(&rs);

		CleanupExternalBuffer();
		}
	}

EXPORT_C void CComponentInfo::PrepareForIpcL(TIpcArgs& aIpcArgs, TInt aIndex)
	{
	delete iExternalBuffer;
	iExternalBuffer = NULL;

	iExternalBuffer = HBufC8::NewL(KMaxComponentInfoStreamedSize);
	iExternalBufferPtr.Set(iExternalBuffer->Des());
	
	aIpcArgs.Set(aIndex, &iExternalBufferPtr);
	
	iDeferredInternalization = ETrue;
	}

// ##########################################################################################

COpaqueNamedParams::COpaqueNamedParams()
	: iExternalBuffer(NULL), iExternalBufferPtr(NULL, 0, 0), iDeferredInternalization(EFalse), iExternalizedSize(2*sizeof(TInt))
	{
	}

EXPORT_C COpaqueNamedParams::~COpaqueNamedParams()
	{
	Cleanup();
	iParams.Close();
	CleanupExternalBuffer();
	}

EXPORT_C COpaqueNamedParams* COpaqueNamedParams::NewL()
	{
	return new (ELeave) COpaqueNamedParams;
	}

EXPORT_C COpaqueNamedParams* COpaqueNamedParams::NewLC()
	{
	COpaqueNamedParams* self = new (ELeave) COpaqueNamedParams;
	CleanupStack::PushL(self);
	return self;
	}

void COpaqueNamedParams::VerifyExternalizedSizeForNewParamArrayL(TInt aNameSize, TInt aValueSize) const
	{
	const TInt load = 2*sizeof(TInt) + aNameSize + aValueSize;
	if (aNameSize > KMaxOpaqueParamNameDescriptorSize || iExternalizedSize + load > KMaxOpaqueParamsExternalizedSize)
		{
		User::Leave(KErrOverflow);
		}
	iExternalizedSize += load;
	}

void COpaqueNamedParams::VerifyExternalizedSizeForExistingParamArrayL(TInt aOldValueSize, TInt aNewValueSize) const
	{
	const TInt diff = aNewValueSize - aOldValueSize;
	if (iExternalizedSize + diff > KMaxOpaqueParamsExternalizedSize)
		{
		User::Leave(KErrOverflow);
		}
	iExternalizedSize += diff;
	}

EXPORT_C void COpaqueNamedParams::AddStringL(const TDesC& aName, const TDesC& aValue)
	{
	CStringItem* stringItem = CStringItem::NewL(aName, aValue);
	CleanupStack::PushL(stringItem);
	
	AddOpaqueParamL(stringItem);
    CleanupStack::Pop(stringItem);
	}

EXPORT_C void COpaqueNamedParams::AddStringArrayL(const TDesC& aName, const RPointerArray<HBufC>& aValueArray)
    {
    CStringArrayItem* stringArray = CStringArrayItem::NewL(aName, aValueArray);
    CleanupStack::PushL(stringArray);
    
    AddOpaqueParamL(stringArray);
    CleanupStack::Pop(stringArray);
    }

EXPORT_C void COpaqueNamedParams::AddIntL(const TDesC& aName, TInt aValue)
	{
	CIntegerItem* integer = CIntegerItem::NewL(aName, aValue);
    CleanupStack::PushL(integer);
    
    AddOpaqueParamL(integer);
    CleanupStack::Pop(integer);	
	}

EXPORT_C void COpaqueNamedParams::AddIntArrayL(const TDesC& aName, const RArray<TInt>& aValueArray)
    {
    CIntegerArrayItem* integerArray = CIntegerArrayItem::NewL(aName, aValueArray);
    CleanupStack::PushL(integerArray);
    
    AddOpaqueParamL(integerArray);
    CleanupStack::Pop(integerArray);     
    }

void COpaqueNamedParams::AddOpaqueParamL(MOpaqueParam* aItemBase)
    {
    const TInt count = iParams.Count();
        
    for (TInt i=0; i<count; ++i)
        {
        if (iParams[i]->Name().CompareF(aItemBase->Name()) == 0)
            {
            VerifyExternalizedSizeForExistingParamArrayL(iParams[i]->ValueSize(), aItemBase->ValueSize());
            
            delete iParams[i];
            iParams[i] = aItemBase;
            return;
            }
        }

    VerifyExternalizedSizeForNewParamArrayL(aItemBase->Name().Size(), aItemBase->ValueSize());
    iParams.AppendL(aItemBase); 
    }

EXPORT_C void COpaqueNamedParams::GetNamesL(RPointerArray<HBufC>& aNames) const
	{
	InternalizeFromExternalBufferL();

	const TInt len = iParams.Count();
	for (TInt i=0; i<len; ++i)
		{
		HBufC* name = iParams[i]->Name().AllocLC();
		aNames.AppendL(name);
		CleanupStack::Pop(name);
		}
	}
	
EXPORT_C void COpaqueNamedParams::ExternalizeL(RWriteStream& aStream) const
	{
	InternalizeFromExternalBufferL();

	TInt count = iParams.Count();
	aStream.WriteInt32L(count);
	for (TInt i=0; i<count; ++i)
		{
		aStream.WriteInt32L(iParams[i]->Type());
		iParams[i]->ExternalizeL(aStream);
		}
	
	aStream.WriteInt32L(iExternalizedSize);
	}

EXPORT_C void COpaqueNamedParams::InternalizeL(RReadStream& aStream)
	{
	ConstInternalizeL(aStream);
	}

void COpaqueNamedParams::ConstInternalizeL(RReadStream& aStream) const
	{
    ConstCleanup();
	RPointerArray<MOpaqueParam>& refParams = const_cast<RPointerArray<MOpaqueParam>& >(iParams);
	
	TInt count = aStream.ReadInt32L();
	for (TInt i=0; i<count; ++i)
		{
		MOpaqueParam::TType type = static_cast<MOpaqueParam::TType>(aStream.ReadInt32L());
	    MOpaqueParam* param(0);
		switch(type)
		    {
		    case MOpaqueParam::EString:
		        param = CStringItem::NewL(aStream);
		        break;
		        
            case MOpaqueParam::EStringArray:
                param = CStringArrayItem::NewL(aStream);
                break;
                
            case MOpaqueParam::EInteger:
                param = CIntegerItem::NewL(aStream);
                break;     
                
            case MOpaqueParam::EIntegerArray:
                param = CIntegerArrayItem::NewL(aStream);
                break;     
                
            default:
                User::Leave(KErrCorrupt);
		    }
		CleanupStack::PushL(param);
		refParams.AppendL(param);
		CleanupStack::Pop(param);
		}
		
	iExternalizedSize = aStream.ReadInt32L();
	}

EXPORT_C void COpaqueNamedParams::PrepareArgumentsForIpcL(TIpcArgs& aIpcArgs, TInt aIndex) const
	{
	delete iExternalBuffer;
	iExternalBuffer = NULL;
	iExternalBuffer = HBufC8::NewL(iExternalizedSize);
	iExternalBufferPtr.Set(iExternalBuffer->Des());
	
	RDesWriteStream ws(iExternalBufferPtr);
	CleanupClosePushL(ws);
	ExternalizeL(ws);
	ws.CommitL();
	CleanupStack::PopAndDestroy(&ws);
	
	aIpcArgs.Set(aIndex, &iExternalBufferPtr);
	}

EXPORT_C void COpaqueNamedParams::PrepareResultsForIpcL(TIpcArgs& aIpcArgs, TInt aIndex)
	{
	delete iExternalBuffer;
	iExternalBuffer = NULL;
	iExternalBuffer = HBufC8::NewL(KMaxOpaqueParamsExternalizedSize);
	iExternalBufferPtr.Set(iExternalBuffer->Des());
	
	RDesWriteStream ws(iExternalBufferPtr);
	CleanupClosePushL(ws);
	ws.WriteInt32L(0); // Write the initial count of results, so that the object will be valid even if no results are added
	CleanupStack::PopAndDestroy(&ws);
	
	aIpcArgs.Set(aIndex, &iExternalBufferPtr);
	
	iDeferredInternalization = ETrue;
	}

void COpaqueNamedParams::InternalizeFromExternalBufferL() const
	{
	if (iDeferredInternalization)
		{
		iDeferredInternalization = EFalse;
		
		RDesReadStream rs(*iExternalBuffer);
		CleanupClosePushL(rs);
		ConstInternalizeL(rs);
		CleanupStack::PopAndDestroy(&rs);

		CleanupExternalBuffer();
		}
	}

EXPORT_C const TDesC& COpaqueNamedParams::StringByNameL(const TDesC& aName) const
	{
	InternalizeFromExternalBufferL();
	
	const TInt count = iParams.Count();
	for (TInt i=0; i<count; ++i)
		{
		if (iParams[i]->Type() == MOpaqueParam::EString && iParams[i]->Name().CompareF(aName) == 0)
			{
			CStringItem* string = static_cast<CStringItem*>(iParams[i]);
			return string->StringValue();
			}
		}
	
	return KNullDesC;    
	}

EXPORT_C const RPointerArray<HBufC>& COpaqueNamedParams::StringArrayByNameL(const TDesC& aName) const
    {
    InternalizeFromExternalBufferL();
  
    const TInt count = iParams.Count();
    TInt i;
    for (i=0; i<count; ++i)
        {
        if (iParams[i]->Type() == MOpaqueParam::EStringArray && iParams[i]->Name().CompareF(aName) == 0)
            {
            break;
            }
        }
    
    if(i == count)
        {
        User::Leave(KErrNotFound); 
        }
    CStringArrayItem* stringArray = static_cast<CStringArrayItem*>(iParams[i]);  
    return stringArray->StringArrayValue();
    }

EXPORT_C TBool COpaqueNamedParams::GetIntByNameL(const TDesC& aName, TInt& aValue) const
	{
	InternalizeFromExternalBufferL();
	
    const TInt count = iParams.Count();
    for (TInt i=0; i<count; ++i)
        {
        if (iParams[i]->Type() == MOpaqueParam::EInteger && iParams[i]->Name().CompareF(aName) == 0)
            {
            CIntegerItem* integer = static_cast<CIntegerItem*>(iParams[i]); 
            aValue = integer->IntegerValue();
            return ETrue;
            }
        }
    
    return EFalse;
	}

EXPORT_C TInt COpaqueNamedParams::IntByNameL(const TDesC& aName) const
	{
	InternalizeFromExternalBufferL();

	TInt val;
	if (!GetIntByNameL(aName, val))
		{
		User::Leave(KErrNotFound);
		}
	return val;
	}

EXPORT_C  const RArray<TInt>& COpaqueNamedParams::IntArrayByNameL(const TDesC& aName) const
    {  
    InternalizeFromExternalBufferL();
  
    const TInt count = iParams.Count();
    TInt i;
    for (i=0; i<count; ++i)
        {
        if (iParams[i]->Type() == MOpaqueParam::EIntegerArray && iParams[i]->Name().CompareF(aName) == 0)
            {
            break;
            }
        }
    
    if(i == count)
        {
        User::Leave(KErrNotFound); 
        }
    CIntegerArrayItem* integerArray = static_cast<CIntegerArrayItem*>(iParams[i]);      
    return integerArray->IntegerArrayValue();
    }

EXPORT_C TInt COpaqueNamedParams::CountL() const
	{
	InternalizeFromExternalBufferL();
	return iParams.Count();
	}
	
EXPORT_C void COpaqueNamedParams::Cleanup()
	{
	ConstCleanup();
	}

void COpaqueNamedParams::ConstCleanup() const
	{
	// Cleanup internal params
	iExternalizedSize = 2*sizeof(TInt);
	
	RPointerArray<MOpaqueParam>& refParams = const_cast<RPointerArray<MOpaqueParam>&>(iParams);
	refParams.ResetAndDestroy();
	}

	void COpaqueNamedParams::CleanupExternalBuffer() const
	{
	delete iExternalBuffer;
	iExternalBuffer = NULL;
	iDeferredInternalization = EFalse;
	}
	

/*
 * SifCommon internal classes
 */


CItemBase::CItemBase(TType aType):
    iType(aType)
    {
    
    }

CItemBase::TType CItemBase::Type() const
    {
    return iType;
    }

void CItemBase::SetNameL(const TDesC& aName)
    {
    delete iName;
    iName = aName.AllocL();
    }


CItemBase::~CItemBase()
    {
    delete iName;
    }

const HBufC& CItemBase::Name() const
    {
    return *iName;
    }

void CItemBase::SetValueSize(TInt aSize)
    {
    iSize = aSize;
    }

TInt CItemBase::ValueSize() const
    {
    return iSize;
    }

void CItemBase::VerifyExternalizedSizeForParamL(TUint aValueSize) const
    {
    if(aValueSize > KMaxOpaqueParamValueDescriptorSize)
        {
        User::Leave(KErrOverflow);
        }
    }

void CItemBase::ExternalizeL(RWriteStream& aStream) const
    {
    aStream << Name();
    aStream.WriteInt32L(iSize);
    }

void CItemBase::InternalizeL (RReadStream& aStream)
    {
    delete iName;
    iName = HBufC::NewL(aStream, KMaxOpaqueParamNameDescriptorSize);
    iSize = aStream.ReadInt32L();
    }
    
/*
 * CStringItem
 */
CStringItem::CStringItem():
    CItemBase(EString)
    {
    
    }

CStringItem* CStringItem::NewL(const TDesC& aName, const TDesC& aValue)
    {
    CStringItem* self = new(ELeave)CStringItem();
    CleanupStack::PushL(self);
    self->ConstructL(aName, aValue);
    CleanupStack::Pop(self);
    return self;
    }

CStringItem* CStringItem::NewL(RReadStream& aStream)
    {   
    CStringItem *self = new(ELeave)CStringItem();
    CleanupStack::PushL(self);
    self->InternalizeL(aStream);
    CleanupStack::Pop(self);
    return self;
    }

void CStringItem::ConstructL(const TDesC& aName, const TDesC& aValue)
    {
    VerifyExternalizedSizeForParamL(aValue.Size());
    SetNameL(aName);
    iString = aValue.AllocL();
    SetValueSize(iString->Size());
    }

CStringItem::~CStringItem()
    {
    delete iString;
    }

void CStringItem::ExternalizeL(RWriteStream& aStream) const
    {
	CItemBase::ExternalizeL(aStream);
    aStream << *iString;
    }

void CStringItem::InternalizeL(RReadStream& aStream)
    {
    CItemBase::InternalizeL(aStream);
    DeleteObjectZ(iString);
    iString = HBufC::NewL(aStream, KMaxOpaqueParamValueDescriptorSize);
    }

const TDesC& CStringItem::StringValue() const
    {
    return *iString;
    }

const HBufC& CStringItem::Name() const
    {
    return CItemBase::Name(); 
    }

MOpaqueParam::TType CStringItem::Type() const
    {
    return CItemBase::Type();
    }

TInt CStringItem::ValueSize() const
    {
    return CItemBase::ValueSize();
    }
/*
 * CStringArrayItem
 */

CStringArrayItem::CStringArrayItem():
    CItemBase(EStringArray)
    {
    
    }

CStringArrayItem* CStringArrayItem::NewL(const TDesC& aName, const RPointerArray<HBufC>& aValueArray)
    {
    CStringArrayItem* self = new(ELeave)CStringArrayItem();
    CleanupStack::PushL(self);
    self->ConstructL(aName, aValueArray);
    CleanupStack::Pop(self);    
    return self;
    }

CStringArrayItem* CStringArrayItem::NewL(RReadStream& aStream)
    {   
    CStringArrayItem *self = new(ELeave)CStringArrayItem();
    CleanupStack::PushL(self);    
    self->InternalizeL(aStream);
    CleanupStack::Pop(self);
    return self;
    }

void CStringArrayItem::ConstructL(const TDesC& aName, const RPointerArray<HBufC>& aValueArray)
    {
    SetNameL(aName);
    for(TInt i=0; i< aValueArray.Count(); ++i)
        {
        VerifyExternalizedSizeForParamL(aValueArray[i]->Size());
        HBufC* value = aValueArray[i]->AllocLC();
        iStringArray.AppendL(value);
        SetValueSize(ValueSize()+ value->Size());
        CleanupStack::Pop();
        }
    }

void CStringArrayItem::ExternalizeL(RWriteStream& aStream) const
    {
    CItemBase::ExternalizeL(aStream);
    ExternalizePointersArrayL(iStringArray, aStream);
    }

void CStringArrayItem::InternalizeL(RReadStream& aStream)
    {
    CItemBase::InternalizeL(aStream);
    iStringArray.ResetAndDestroy();
    InternalizePointersArrayL(iStringArray, aStream);
    }

const RPointerArray<HBufC>& CStringArrayItem:: StringArrayValue() const
    {
    return iStringArray;
    }

const HBufC& CStringArrayItem::Name() const
    {
    return CItemBase::Name(); 
    }

MOpaqueParam::TType CStringArrayItem::Type() const
    {
    return CItemBase::Type();
    }

TInt CStringArrayItem::ValueSize() const
    {
    return CItemBase::ValueSize();
    }

CStringArrayItem::~CStringArrayItem()
    {
    iStringArray.ResetAndDestroy();
    }

/*
 * CIntegerItem
 */

CIntegerItem::CIntegerItem(TInt aValue):
    CItemBase(EInteger),
    iInteger(aValue)
    {
    
    }

CIntegerItem* CIntegerItem::NewL(const TDesC& aName, TInt aValue)
    {
    CIntegerItem* self = new(ELeave)CIntegerItem(aValue);
    CleanupStack::PushL(self);
    self->ConstructL(aName);
    CleanupStack::Pop(self);  
    return self;
    }

CIntegerItem* CIntegerItem::NewL(RReadStream& aStream)
    {   
    CIntegerItem *self = new(ELeave)CIntegerItem(0);
    CleanupStack::PushL(self);
    self->InternalizeL(aStream);
    CleanupStack::Pop(self);
    return self;
    }

void CIntegerItem::ConstructL(const TDesC& aName)
    {
    SetNameL(aName);
    SetValueSize(sizeof(TInt));
    }

void CIntegerItem::ExternalizeL(RWriteStream& aStream) const
    {
    CItemBase::ExternalizeL(aStream);
    aStream.WriteInt32L(iInteger);
    }

void CIntegerItem::InternalizeL(RReadStream& aStream)
    {
    CItemBase::InternalizeL(aStream);
    iInteger = aStream.ReadInt32L();
    }

TInt CIntegerItem::IntegerValue() const
    {
    return iInteger;
    }

const HBufC& CIntegerItem::Name() const
    {
    return CItemBase::Name(); 
    }

MOpaqueParam::TType CIntegerItem::Type() const
    {
    return CItemBase::Type();
    }

TInt CIntegerItem::ValueSize() const
    {
    return CItemBase::ValueSize();
    }

CIntegerItem::~CIntegerItem()
    {
    }

/*
 * CIntegerArrayItem
 */

CIntegerArrayItem::CIntegerArrayItem():
    CItemBase(EIntegerArray)
    {
    
    }

CIntegerArrayItem* CIntegerArrayItem::NewL(const TDesC& aName, const RArray<TInt>& aValueArray)
    {
    CIntegerArrayItem* self = new(ELeave)CIntegerArrayItem();
    CleanupStack::PushL(self);
    self->ConstructL(aName, aValueArray);
    CleanupStack::Pop(self);    
    return self;
    }

CIntegerArrayItem* CIntegerArrayItem::NewL(RReadStream& aStream)
    {   
    CIntegerArrayItem *self = new(ELeave)CIntegerArrayItem();
    CleanupStack::PushL(self);    
    self->InternalizeL(aStream);
    CleanupStack::Pop(self);
    return self;
    }


void CIntegerArrayItem::ConstructL(const TDesC& aName, const RArray<TInt>& aValueArray)
    {
    SetNameL(aName);
    
    for(TInt i=0; i<aValueArray.Count(); ++i)
        {
        iIntegerArray.AppendL(aValueArray[i]);
        SetValueSize(ValueSize()+sizeof(TInt));
        }
    }

void CIntegerArrayItem::ExternalizeL(RWriteStream& aStream) const
    {
    CItemBase::ExternalizeL(aStream);
    ExternalizeFixedLengthArrayL(iIntegerArray, aStream);
    }

void CIntegerArrayItem::InternalizeL(RReadStream& aStream)
    {
    CItemBase::InternalizeL(aStream);
    iIntegerArray.Reset();
    InternalizeFixedLengthArrayL(iIntegerArray, aStream);
    }

const RArray<TInt>& CIntegerArrayItem::IntegerArrayValue() const
    {
    return iIntegerArray;
    }

const HBufC& CIntegerArrayItem::Name() const
    {
    return CItemBase::Name(); 
    }

MOpaqueParam::TType CIntegerArrayItem::Type() const
    {
    return CItemBase::Type();
    }

TInt CIntegerArrayItem::ValueSize() const
    {
    return CItemBase::ValueSize();
    }

CIntegerArrayItem::~CIntegerArrayItem()
    {
    iIntegerArray.Close();
    }
