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
*
*/


#include <f32file.h>
#include <usif/sif/sifcommon.h>
#include <s32mem.h>

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

	iChildren.Close();
	}

EXPORT_C CComponentInfo::CNode* CComponentInfo::CNode::NewLC(const TDesC& aSoftwareTypeName, const TDesC& aComponentName,
															const TDesC& aVersion, const TDesC& aVendor, TScomoState aScomoState,
															TInstallStatus aInstallStatus, TComponentId aComponentId,
															const TDesC& aGlobalComponentId, TAuthenticity aAuthenticity,
															const TCapabilitySet& aUserGrantableCaps, TInt aMaxInstalledSize,
															TBool aHasExe,
															RPointerArray<CNode>* aChildren)
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
	: iExternalBuffer(NULL), iExternalBufferPtr(NULL, 0, 0), iDeferredInternalization(EFalse), iExternalizedSize(sizeof(TInt))
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

void COpaqueNamedParams::VerifyExternalizedSizeForNewParamL(TInt aNameSize, TInt aValueSize) const
	{
	const TInt load = 2*sizeof(TInt) + aNameSize + aValueSize;
	if (aNameSize > KMaxDescriptorLength || aValueSize > KMaxDescriptorLength ||
		iExternalizedSize + load > KMaxExternalizedSize)
		{
		User::Leave(KErrOverflow);
		}
	iExternalizedSize += load;
	}

void COpaqueNamedParams::VerifyExternalizedSizeForExistingParamL(TInt aOldValueSize, TInt aNewValueSize) const
	{
	const TInt diff = aNewValueSize - aOldValueSize;
	if (aNewValueSize > KMaxDescriptorLength ||
		iExternalizedSize + diff > KMaxExternalizedSize)
		{
		User::Leave(KErrOverflow);
		}
	iExternalizedSize += diff;
	}

EXPORT_C void COpaqueNamedParams::AddStringL(const TDesC& aName, const TDesC& aValue)
	{
	HBufC* value = HBufC::NewLC(aValue.Length());
	TPtr bufValue(value->Des());
	bufValue.Copy(aValue);

	const TInt len = iParams.Count();
	for (TInt i=0; i<len; ++i)
		{
		if (iParams[i].iName->CompareF(aName) == 0)
			{
			VerifyExternalizedSizeForExistingParamL(iParams[i].iValue->Size(), value->Size());
			delete iParams[i].iValue;
			iParams[i].iValue = value;
			CleanupStack::Pop(value);
			return;
			}
		}

	VerifyExternalizedSizeForNewParamL(aName.Size(), aValue.Size());

	HBufC* name = HBufC::NewLC(aName.Length());
	TPtr bufName(name->Des());
	bufName.Copy(aName);

	TItem item = {name, value};
	iParams.AppendL(item);

	CleanupStack::Pop(2, value);
	}

EXPORT_C void COpaqueNamedParams::AddIntL(const TDesC& aName, TInt aValue)
	{
  	// Assumption: the code below won't be compiled in __KERNEL_MODE__ so HBufC is always defined as HBufC16
  	TBuf<sizeof(TInt)/2> buf;
  	buf.Copy(reinterpret_cast<TUint16*>(&aValue), sizeof(TInt)/2);
	AddStringL(aName, buf);
	}

EXPORT_C void COpaqueNamedParams::GetNamesL(RPointerArray<HBufC>& aNames) const
	{
	InternalizeFromExternalBufferL();

	const TInt len = iParams.Count();
	for (TInt i=0; i<len; ++i)
		{
		const TDesC& ref = *iParams[i].iName;
		HBufC* name = HBufC::NewLC(ref.Length());
		TPtr bufName(name->Des());
		bufName.Copy(ref);
		aNames.AppendL(name);
		CleanupStack::Pop(name);
		}
	}
	
EXPORT_C void COpaqueNamedParams::ExternalizeL(RWriteStream& aStream) const
	{
	InternalizeFromExternalBufferL();

	TInt len = iParams.Count();
	aStream.WriteInt32L(len);
	for (TInt i=0; i<len; ++i)
		{
		aStream << *iParams[i].iName;
		aStream << *iParams[i].iValue;
		}
	}

EXPORT_C void COpaqueNamedParams::InternalizeL(RReadStream& aStream)
	{
	Cleanup();
	ConstInternalizeL(aStream);
	}

void COpaqueNamedParams::ConstInternalizeL(RReadStream& aStream) const
	{
	RArray<TItem>& refParams = const_cast<RArray<TItem>&>(iParams);
	
	TInt len = aStream.ReadInt32L();
	for (TInt i=0; i<len; ++i)
		{
		HBufC* name = HBufC::NewLC(aStream, KMaxDescriptorLength);
		HBufC* value = HBufC::NewLC(aStream, KMaxDescriptorLength);

		// We need to update iExternalizedSize here because its value must correspond to the params beind added from aStream
		VerifyExternalizedSizeForNewParamL(name->Size(), value->Size());

		TItem item = {name, value};
		refParams.AppendL(item);

		CleanupStack::Pop(2, name);
		}
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
	iExternalBuffer = HBufC8::NewL(KMaxExternalizedSize);
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

		ConstCleanup();
		
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
	
	const TInt len = iParams.Count();
	for (TInt i=0; i<len; ++i)
		{
		if (iParams[i].iName->CompareF(aName) == 0)
			{
			return *iParams[i].iValue;
			}
		}
	return KNullDesC;    
	}

EXPORT_C TBool COpaqueNamedParams::GetIntByNameL(const TDesC& aName, TInt& aValue) const
	{
	InternalizeFromExternalBufferL();

	const TDesC& value = StringByNameL(aName);
	if (value == KNullDesC)
		{
		return EFalse;
		}
	aValue = *(reinterpret_cast<const TUint*>(value.Ptr()));
	return ETrue;
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
	iExternalizedSize = sizeof(TInt);

	const TInt len = iParams.Count();
	for (TInt i=0; i<len; ++i)
		{
		delete iParams[i].iName;
		delete iParams[i].iValue;
		}
	RArray<TItem>& refParams = const_cast<RArray<TItem>&>(iParams);
	refParams.Reset();
	}

	void COpaqueNamedParams::CleanupExternalBuffer() const
	{
	delete iExternalBuffer;
	iExternalBuffer = NULL;
	iDeferredInternalization = EFalse;
	}
