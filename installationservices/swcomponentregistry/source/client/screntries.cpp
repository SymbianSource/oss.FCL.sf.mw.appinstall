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
* Implement entry classes for Software Components Registry interface.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "screntries_internal.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "screntries.h"
#include "usiflog.h"
#include <scs/streamingarray.h>
#include <scs/cleanuputils.h>
#include <usif/scr/screntries_platform.h>

using namespace Usif;

		
///////////////////////
// CGlobalComponentId
///////////////////////

CGlobalComponentId::CGlobalComponentId()
	{
	// empty
	}

EXPORT_C CGlobalComponentId::~CGlobalComponentId()
	{
	delete iGlobalIdName;
	delete iSoftwareTypeName;
	}

EXPORT_C CGlobalComponentId* CGlobalComponentId::NewLC()
	{
	CGlobalComponentId *self = new(ELeave) CGlobalComponentId();
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CGlobalComponentId* CGlobalComponentId::NewL(const TDesC& aGlobalIdName, const TDesC& aUniqueSwTypeName)
	{
	CGlobalComponentId *self = CGlobalComponentId::NewLC(aGlobalIdName, aUniqueSwTypeName);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CGlobalComponentId* CGlobalComponentId::NewLC(const TDesC& aGlobalIdName, const TDesC& aUniqueSwTypeName)
	{
	CGlobalComponentId *self = CGlobalComponentId::NewLC();
	self->ConstructL(aGlobalIdName, aUniqueSwTypeName);
	return self;
	}

EXPORT_C CGlobalComponentId* CGlobalComponentId::NewL(RReadStream& aStream)
	{
	CGlobalComponentId *self = CGlobalComponentId::NewLC();
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

void CGlobalComponentId::ConstructL(const TDesC& aGlobalIdName, const TDesC& aSoftwareTypeName)
	{
	iGlobalIdName = aGlobalIdName.AllocL();
	iSoftwareTypeName = aSoftwareTypeName.AllocL();
	}

EXPORT_C void CGlobalComponentId::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iGlobalIdName;
	aStream << *iSoftwareTypeName;
	}

EXPORT_C void CGlobalComponentId::InternalizeL(RReadStream& aStream)
	{
	// Read global id name
	DeleteObjectZ(iGlobalIdName);
	iGlobalIdName = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	// Read software type name
	DeleteObjectZ(iSoftwareTypeName);
	iSoftwareTypeName = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	}

EXPORT_C const TDesC& CGlobalComponentId::GlobalIdName() const
	{
	return *iGlobalIdName;
	}

EXPORT_C const TDesC& CGlobalComponentId::SoftwareTypeName() const
	{
	return *iSoftwareTypeName;
	}


////////////////////////
// CVersionedComponentId
////////////////////////

CVersionedComponentId::CVersionedComponentId()
	{
	// empty
	}

EXPORT_C CVersionedComponentId::~CVersionedComponentId()
	{
	delete iGlobalId;
	delete iVersionFrom;
	delete iVersionTo;
	}

EXPORT_C CVersionedComponentId* CVersionedComponentId::NewLC()
	{
	CVersionedComponentId *self = new(ELeave) CVersionedComponentId();
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CVersionedComponentId* CVersionedComponentId::NewL(const CGlobalComponentId& aGlobalId, const TDesC* aVersionFrom, const TDesC* aVersionTo)
	{
	CVersionedComponentId *self = CVersionedComponentId::NewLC(aGlobalId, aVersionFrom, aVersionTo);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CVersionedComponentId* CVersionedComponentId::NewLC(const CGlobalComponentId& aGlobalId, const TDesC* aVersionFrom, const TDesC* aVersionTo)
	{
	CVersionedComponentId *self = CVersionedComponentId::NewLC();
	self->ConstructL(aGlobalId, aVersionFrom, aVersionTo);
	return self;
	}

void CVersionedComponentId::ConstructL(const CGlobalComponentId& aGlobalId, const TDesC* aVersionFrom, const TDesC* aVersionTo)
	{
	iGlobalId = CGlobalComponentId::NewL(aGlobalId.GlobalIdName(), aGlobalId.SoftwareTypeName());
	
	if(aVersionFrom)
		iVersionFrom = aVersionFrom->AllocL();
	else
		iVersionFrom = KNullDesC().AllocL();
	
	if(aVersionTo)
		iVersionTo = aVersionTo->AllocL();
	else
		iVersionTo = KNullDesC().AllocL();
	}

EXPORT_C CVersionedComponentId* CVersionedComponentId::NewL(RReadStream& aStream)
	{
	CVersionedComponentId *self = CVersionedComponentId::NewLC();
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CVersionedComponentId::InternalizeL(RReadStream& aStream)
	{
	// Read global id 
	DeleteObjectZ(iGlobalId);
	iGlobalId = CGlobalComponentId::NewL(aStream);
	// Read version from
	DeleteObjectZ(iVersionFrom);
	iVersionFrom = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	// Read version to
	DeleteObjectZ(iVersionTo);
	iVersionTo = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	}

EXPORT_C void CVersionedComponentId::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iGlobalId;
	aStream << *iVersionFrom;
	aStream << *iVersionTo;
	}

EXPORT_C const CGlobalComponentId& CVersionedComponentId::GlobalId() const
	{
	return *iGlobalId;
	}
		
EXPORT_C const TDesC* CVersionedComponentId::VersionFrom() const
	{
	if(!iVersionFrom->Length())
		return NULL;
	else
		return iVersionFrom;
	}
		
EXPORT_C const TDesC* CVersionedComponentId::VersionTo() const
	{
	if(!iVersionTo->Length())
		return NULL;
	else
		return iVersionTo;
	}


///////////////////////
// CComponentEntry
///////////////////////

CComponentEntry::CComponentEntry()
	{
	// empty
	}

EXPORT_C CComponentEntry::~CComponentEntry()
	{
	delete iName;
	delete iVendor;
	delete iSwType;
	delete iGlobalId;
	delete iVersion;
	delete iInstallTime;
	}

EXPORT_C CComponentEntry* CComponentEntry::NewL()
	{
	CComponentEntry *self = CComponentEntry::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CComponentEntry* CComponentEntry::NewLC()
	{
	CComponentEntry *self = new(ELeave) CComponentEntry();
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CComponentEntry* CComponentEntry::NewL(RReadStream& aStream)
	{
	CComponentEntry *self = CComponentEntry::NewLC();
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CComponentEntry* CComponentEntry::NewL(const TComponentId& aId, const TDesC& aName, const TDesC& aVendor, const TDesC& aSwType,
		  const TDesC& aGlobalId, TBool aRemovable, TInt64 aSize, const TScomoState& aScomoState, const TDriveList& aInstalledDrives, 
		  const TDesC& aVersion, const TDesC& aInstallTime, TBool aDrmProtected, TBool aHidden, TBool aKnownRevoked, TBool aOriginVerified)
	{
	CComponentEntry *self = CComponentEntry::NewLC(aId, aName, aVendor, aSwType, aGlobalId, aRemovable, aSize, aScomoState, aInstalledDrives, 
												   aVersion, aInstallTime, aDrmProtected, aHidden, aKnownRevoked, aOriginVerified);
	CleanupStack::Pop(self);
	return self;
	}
		
EXPORT_C CComponentEntry* CComponentEntry::NewLC(const TComponentId& aId, const TDesC& aName, const TDesC& aVendor, const TDesC& aSwType, 
			const TDesC& aGlobalId, TBool aRemovable, TInt64 aSize, const TScomoState& aScomoState, const TDriveList& aInstalledDrives, 
			const TDesC& aVersion, const TDesC& aInstallTime, TBool aDrmProtected, TBool aHidden, TBool aKnownRevoked, TBool aOriginVerified)		
	{
	CComponentEntry *self = new(ELeave) CComponentEntry();
	CleanupStack::PushL(self);
	self->ConstructL(aId, aName, aVendor, aSwType, aGlobalId, aRemovable, aSize, aScomoState, aInstalledDrives, aVersion, aInstallTime,
					 aDrmProtected, aHidden, aKnownRevoked, aOriginVerified);
	return self;
	}

void CComponentEntry::ConstructL(const TComponentId& aId, const TDesC& aName, const TDesC& aVendor, const TDesC& aSwType, const TDesC& aGlobalId,
				TBool aRemovable, TInt64 aSize, const TScomoState& aScomoState, const TDriveList& aInstalledDrives, 
				const TDesC& aVersion, const TDesC& aInstallTime, TBool aDrmProtected, TBool aHidden, TBool aKnownRevoked, TBool aOriginVerified)
	{
	iComponentId = aId;
	iName = aName.AllocL();
	iVendor = aVendor.AllocL();
	iSwType = aSwType.AllocL();
	iGlobalId = aGlobalId.AllocL();
	iRemovable = aRemovable;
	iSize = aSize;
	iScomoState = aScomoState;
	iInstalledDrives.Copy(aInstalledDrives);
	iVersion = aVersion.AllocL();
	iInstallTime = aInstallTime.AllocL();
	iDrmProtected = aDrmProtected;
	iHidden = aHidden;
	iKnownRevoked = aKnownRevoked;
	iOriginVerified = aOriginVerified;
	}

void WriteInt64L(RWriteStream& aStream, TInt64 aValue)
	{
	aStream.WriteInt32L(I64LOW(aValue));
	aStream.WriteInt32L(I64HIGH(aValue));
	}

EXPORT_C void CComponentEntry::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << TCardinality(iComponentId); // Write the compressed value by using cardinality class
	aStream << *iName;
	aStream << *iVendor;
	aStream << *iSwType;
	aStream << *iGlobalId;
	aStream << TCardinality(iRemovable);
	WriteInt64L(aStream, iSize);
	aStream << TCardinality(iScomoState);
	aStream << iInstalledDrives;
	aStream << *iVersion;
	aStream << *iInstallTime;
	aStream << TCardinality(iDrmProtected);
	aStream << TCardinality(iHidden);
	aStream << TCardinality(iKnownRevoked);
	aStream << TCardinality(iOriginVerified);
	}

TInt64 ReadInt64L(RReadStream& aStream)
	{
	TInt32 low32Bit = aStream.ReadInt32L(); 
	TInt32 high32Bit = aStream.ReadInt32L();
	return MAKE_TINT64(high32Bit, low32Bit);
	}

EXPORT_C void CComponentEntry::InternalizeL(RReadStream& aStream)
	{
	TCardinality c;
	// Read component id
	aStream >> c;
	iComponentId = TComponentId(c);
	// Read component name
	DeleteObjectZ(iName);
	iName = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	// Read vendor name
	DeleteObjectZ(iVendor);
	iVendor = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	// Read software type
	DeleteObjectZ(iSwType);
	iSwType = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	// Read global id
	DeleteObjectZ(iGlobalId);
	iGlobalId = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	// Read if it is in removable
	aStream >> c;
	iRemovable = c;
	// Read install-time component size
	iSize = ReadInt64L(aStream);	
	// Read scomo state
	aStream >> c;
	iScomoState = static_cast<TScomoState>(static_cast<TInt>(c));
	// Read installed drives
	aStream >> iInstalledDrives;
	// Read version
	DeleteObjectZ(iVersion);
	iVersion = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	// Read install-time of the component
	DeleteObjectZ(iInstallTime);
	iInstallTime = HBufC::NewL(aStream, KMaxTInt); // No restriction on length	
	// Read if it is DRM protected
	aStream >> c;
	iDrmProtected = c;
	// Read if it is hidden
	aStream >> c;
	iHidden = c;
	// Read if it is known revoked
	aStream >> c;
	iKnownRevoked = c;
	// Read if it is origin verified
	aStream >> c;
	iOriginVerified = c;
	}
		
EXPORT_C const TComponentId& CComponentEntry::ComponentId() const
	{
	return iComponentId;
	}

EXPORT_C const TDesC& CComponentEntry::Name() const
	{
	return *iName;
	}

EXPORT_C const TDesC& CComponentEntry::Vendor() const
	{
	return *iVendor;
	}

EXPORT_C const TDesC& CComponentEntry::SoftwareType() const
	{
	return *iSwType;
	}

EXPORT_C const TDesC& CComponentEntry::GlobalId() const
	{
	return *iGlobalId;
	}

EXPORT_C TBool CComponentEntry::IsRemovable() const
	{
	return iRemovable;
	}

EXPORT_C TInt64 CComponentEntry::ComponentSize() const
	{
	return iSize;
	}

EXPORT_C const TScomoState& CComponentEntry::ScomoState() const
	{
	return iScomoState;
	}
		
EXPORT_C const TDriveList& CComponentEntry::InstalledDrives() const
	{
	return iInstalledDrives;
	}
		
EXPORT_C const TDesC& CComponentEntry::Version() const
	{
	return *iVersion;
	}

EXPORT_C TTime CComponentEntry::InstallTimeL() const
	{
	TTime time;
	User::LeaveIfError(time.Set(*iInstallTime));
	return time;
	}

EXPORT_C TBool CComponentEntry::IsDrmProtected() const
	{
	return iDrmProtected;
	}

EXPORT_C TBool CComponentEntry::IsHidden() const
	{
	return iHidden;
	}

EXPORT_C TBool CComponentEntry::IsKnownRevoked() const
	{ 
	return iKnownRevoked;
	}

EXPORT_C TBool CComponentEntry::IsOriginVerified() const
	{
	return iOriginVerified;
	}

///////////////////////
// CPropertyEntry
///////////////////////

CPropertyEntry::CPropertyEntry()
	{
	// empty
	}

EXPORT_C CPropertyEntry::~CPropertyEntry()
	{
	delete iPropertyName;
	}
    
void CPropertyEntry::ConstructL(const TDesC& aPropertyName)
	{
	iPropertyName = aPropertyName.AllocL();
	}

EXPORT_C const TDesC& CPropertyEntry::PropertyName() const
	{
	return *iPropertyName;
	}

EXPORT_C CPropertyEntry* CPropertyEntry::NewL(RReadStream& aStream)
	{
	TCardinality c;
	aStream >> c;
	TPropertyType propType = TPropertyType(TInt(c));
	CPropertyEntry *property(0);
	switch(propType)
		{
	case EBinaryProperty:
		property = CBinaryPropertyEntry::NewLC();
		break;
	case EIntProperty:
		property = CIntPropertyEntry::NewLC();
		break;
	case ELocalizedProperty:
		property = CLocalizablePropertyEntry::NewLC();
		break;
	default:
		DEBUG_PRINTF2(_L("CPropertyEntry has got an invalid property type = %d"), propType);
		User::Leave(KErrCorrupt);
		}
	property->DoInternalizeL(aStream);
	property->iPropertyName = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	CleanupStack::Pop(property);
	return property;
	}

EXPORT_C void CPropertyEntry::ExternalizeL(RWriteStream& aStream) const
	{
	TPropertyType t = this->PropertyType();
	aStream << TCardinality(t);
	this->DoExternalizeL(aStream);
	aStream << *iPropertyName;
	}


///////////////////////
// CIntPropertyEntry
///////////////////////

CIntPropertyEntry::CIntPropertyEntry()
	{
	// empty
	}

CIntPropertyEntry::CIntPropertyEntry(TInt64 aPropertyIntValue)
	:	CPropertyEntry(), iIntValue(aPropertyIntValue)
	{
	// empty
	}

EXPORT_C CIntPropertyEntry::~CIntPropertyEntry()
	{
	// empty
	}

EXPORT_C CIntPropertyEntry* CIntPropertyEntry::NewL(const TDesC& aPropertyName, TInt64 aPropertyValue)
	{
	CIntPropertyEntry *self = CIntPropertyEntry::NewLC(aPropertyName, aPropertyValue);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CIntPropertyEntry* CIntPropertyEntry::NewLC(const TDesC& aPropertyName, TInt64 aPropertyValue)
	{
	CIntPropertyEntry *self = new (ELeave) CIntPropertyEntry(aPropertyValue);
	CleanupStack::PushL(self);
	self->ConstructL(aPropertyName);
	return self;
	}

void CIntPropertyEntry::ConstructL(const TDesC& aPropertyName)
	{
	CPropertyEntry::ConstructL(aPropertyName);
	}

CIntPropertyEntry* CIntPropertyEntry::NewLC()
	{
	CIntPropertyEntry *self = new(ELeave) CIntPropertyEntry();
	CleanupStack::PushL(self);
	return self;
	}
		
void CIntPropertyEntry::DoExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(I64LOW(iIntValue));
	aStream.WriteInt32L(I64HIGH(iIntValue));
	}

void CIntPropertyEntry::DoInternalizeL(RReadStream& aStream)
	{
	TInt32 low32Bit = aStream.ReadInt32L(); 
	TInt32 high32Bit = aStream.ReadInt32L();
	iIntValue = MAKE_TINT64(high32Bit, low32Bit);	
	}

EXPORT_C CPropertyEntry::TPropertyType CIntPropertyEntry::PropertyType() const
	{
	return CPropertyEntry::EIntProperty;
	}

EXPORT_C TInt CIntPropertyEntry::IntValue()
	{
	return static_cast<TInt>(iIntValue);
	}

EXPORT_C TInt64 CIntPropertyEntry::Int64Value()
	{
	return iIntValue;
	}

///////////////////////
// CBinaryPropertyEntry
///////////////////////

CBinaryPropertyEntry::CBinaryPropertyEntry()
	{
	// empty
	}

EXPORT_C CBinaryPropertyEntry::~CBinaryPropertyEntry()
	{
	delete iValue;
	}
	
EXPORT_C CBinaryPropertyEntry* CBinaryPropertyEntry::NewL(const TDesC& aPropertyName, const TDesC8& aPropertyValue)
	{
	CBinaryPropertyEntry *self = CBinaryPropertyEntry::NewLC(aPropertyName, aPropertyValue);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CBinaryPropertyEntry* CBinaryPropertyEntry::NewLC(const TDesC& aPropertyName, const TDesC8& aPropertyValue)
	{
	CBinaryPropertyEntry *self = new(ELeave) CBinaryPropertyEntry();
	CleanupStack::PushL(self);
	self->ConstructL(aPropertyName, aPropertyValue);
	return self;
	}

void CBinaryPropertyEntry::ConstructL(const TDesC& aPropertyName, const TDesC8& aPropertyValue)
	{
	CPropertyEntry::ConstructL(aPropertyName);
	iValue = aPropertyValue.AllocL();
	}

CBinaryPropertyEntry* CBinaryPropertyEntry::NewLC()
	{
	CBinaryPropertyEntry *self = new(ELeave) CBinaryPropertyEntry();
	CleanupStack::PushL(self);
	return self;
	}
			
void CBinaryPropertyEntry::DoExternalizeL(RWriteStream &aStream) const	
	{
	aStream << *iValue;
	}

void CBinaryPropertyEntry::DoInternalizeL(RReadStream& aStream)
	{
	DeleteObjectZ(iValue);
	iValue = HBufC8::NewL(aStream, KMaxTInt); // No restriction on length
	}

EXPORT_C CBinaryPropertyEntry::TPropertyType CBinaryPropertyEntry::PropertyType() const
	{
	return CBinaryPropertyEntry::EBinaryProperty;
	}
			
EXPORT_C const TDesC8& CBinaryPropertyEntry::BinaryValue()
	{
	return *iValue;
	}	

///////////////////////
// CLocalizablePropertyEntry
///////////////////////

CLocalizablePropertyEntry::CLocalizablePropertyEntry()
	{
	// empty
	}

CLocalizablePropertyEntry::CLocalizablePropertyEntry(TLanguage aLocale) : iLocale(aLocale)
	{
	// empty
	}

EXPORT_C CLocalizablePropertyEntry::~CLocalizablePropertyEntry()
	{
	delete iStrValue;
	}

EXPORT_C CLocalizablePropertyEntry* CLocalizablePropertyEntry::NewL(const TDesC& aPropertyName, const TDesC& aPropertyValue, TLanguage aLocale)
	{
	CLocalizablePropertyEntry *self = CLocalizablePropertyEntry::NewLC(aPropertyName, aPropertyValue, aLocale);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CLocalizablePropertyEntry* CLocalizablePropertyEntry::NewLC( const TDesC& aPropertyName, const TDesC& aPropertyValue, TLanguage aLocale)
	{
	CLocalizablePropertyEntry *self = new(ELeave) CLocalizablePropertyEntry(aLocale);
	CleanupStack::PushL(self);
	self->ConstructL(aPropertyName, aPropertyValue);
	return self;
	}

CLocalizablePropertyEntry* CLocalizablePropertyEntry::NewLC()
	{
	CLocalizablePropertyEntry *self = new(ELeave) CLocalizablePropertyEntry();
	CleanupStack::PushL(self);
	return self;
	}

void CLocalizablePropertyEntry::ConstructL(const TDesC& aPropertyName, const TDesC& aPropertyValue)
	{
	CPropertyEntry::ConstructL(aPropertyName);
	iStrValue = aPropertyValue.AllocL();
	}

void CLocalizablePropertyEntry::DoExternalizeL(RWriteStream &aStream) const
	{
	aStream << *iStrValue;
	aStream.WriteInt32L(iLocale);
	}

void CLocalizablePropertyEntry::DoInternalizeL(RReadStream& aStream)
	{
	iStrValue = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	iLocale = TLanguage(aStream.ReadInt32L());
	}

EXPORT_C CPropertyEntry::TPropertyType CLocalizablePropertyEntry::PropertyType() const
	{
	return CPropertyEntry::ELocalizedProperty;
	}

EXPORT_C TLanguage CLocalizablePropertyEntry::LocaleL() const
	{
	return iLocale;
	}

EXPORT_C const TDesC& CLocalizablePropertyEntry::StrValue() const
	{
	return *iStrValue;
	}

///////////////////////
// CLocalizableCommonData
///////////////////////

CLocalizableCommonData::CLocalizableCommonData()
	{
	// empty
	}

CLocalizableCommonData::CLocalizableCommonData(TLanguage aLocale)
	:	iLocale(aLocale)
	{
	// empty
	}

CLocalizableCommonData::~CLocalizableCommonData()
	{
	// empty
	}

EXPORT_C TLanguage CLocalizableCommonData::Locale() const
	{
	return iLocale;
	}

void CLocalizableCommonData::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(Locale());
	}

void CLocalizableCommonData::InternalizeL(RReadStream& aStream)
	{
	TLanguage locale = static_cast<TLanguage>(aStream.ReadInt32L());
	iLocale = locale;
	}

/////////////////////////////
// CLocalizableComponentInfo
/////////////////////////////

CLocalizableComponentInfo::CLocalizableComponentInfo()
	{
	// empty
	}

CLocalizableComponentInfo::CLocalizableComponentInfo(TLanguage aLocale)
	:	CLocalizableCommonData(aLocale)
	{
	// empty
	}

CLocalizableComponentInfo::~CLocalizableComponentInfo()
	{
	delete iName;
	delete iVendor;
	}

EXPORT_C CLocalizableComponentInfo* CLocalizableComponentInfo::NewL(const TDesC& aName, const TDesC& aVendor, TLanguage aLocale)
	{
	CLocalizableComponentInfo *self = CLocalizableComponentInfo::NewLC(aName, aVendor, aLocale);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CLocalizableComponentInfo* CLocalizableComponentInfo::NewLC(const TDesC& aName, const TDesC& aVendor, TLanguage aLocale)
	{
	CLocalizableComponentInfo *self = new(ELeave) CLocalizableComponentInfo(aLocale);
	CleanupStack::PushL(self);
	self->ConstructL(aName, aVendor);
	return self;
	}

EXPORT_C CLocalizableComponentInfo* CLocalizableComponentInfo::NewL(RReadStream& aStream)
	{
	CLocalizableComponentInfo* self = new(ELeave) CLocalizableComponentInfo();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CLocalizableComponentInfo::ConstructL(const TDesC& aName, const TDesC& aVendor)
	{
	iName = aName.AllocL();
	iVendor = aVendor.AllocL();
	}


EXPORT_C void CLocalizableComponentInfo::ExternalizeL(RWriteStream& aStream) const
	{
	CLocalizableCommonData::ExternalizeL(aStream);
	aStream << *iName;
	aStream << *iVendor;
	}
		
EXPORT_C void CLocalizableComponentInfo::InternalizeL(RReadStream& aStream)
	{
	CLocalizableCommonData::InternalizeL(aStream);
	
	delete iName;
	iName = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	
	delete iVendor;
	iVendor = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	}

EXPORT_C const TDesC& CLocalizableComponentInfo::NameL() const
	{
	return *iName;
	}

EXPORT_C const TDesC& CLocalizableComponentInfo::VendorL() const
	{
	return *iVendor;
	}
			
/////////////////////////////
// TCustomAccessInfo
/////////////////////////////

EXPORT_C TCustomAccessInfo::TCustomAccessInfo(TSecureId aSecureId, TAccessMode aAccessMode)
    {
    iSecureId = aSecureId;
    iAccessMode = aAccessMode;
    }

EXPORT_C TSecureId TCustomAccessInfo::SecureId() const
    {
    return iSecureId;
    }

EXPORT_C TAccessMode TCustomAccessInfo::AccessMode() const
    {
    return iAccessMode;
    }

/////////////////////////////
// CSoftwareTypeRegInfo
/////////////////////////////

EXPORT_C CSoftwareTypeRegInfo* CSoftwareTypeRegInfo::NewL(const TDesC& aUniqueSoftwareTypeName)
	{
	CSoftwareTypeRegInfo* self = new (ELeave) CSoftwareTypeRegInfo();
	CleanupStack::PushL(self);
	self->iUniqueSoftwareTypeName = HBufC::NewL(aUniqueSoftwareTypeName.Length());
	self->iUniqueSoftwareTypeName->Des().Copy(aUniqueSoftwareTypeName);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSoftwareTypeRegInfo* CSoftwareTypeRegInfo::NewL(RReadStream& aStream)
	{
	CSoftwareTypeRegInfo* self = new (ELeave) CSoftwareTypeRegInfo();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

CSoftwareTypeRegInfo::CSoftwareTypeRegInfo()
	{
	}

EXPORT_C CSoftwareTypeRegInfo::~CSoftwareTypeRegInfo()
	{
	delete iUniqueSoftwareTypeName;
	iCustomAccessList.Close();
	iMimeTypes.Close();
	iLocalizedSoftwareTypeNames.Close();
	delete iLauncherExecutable;
	}

EXPORT_C void CSoftwareTypeRegInfo::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iUniqueSoftwareTypeName;
	aStream << iSifPluginUid.iUid;
	
	//Sids
	const TInt numSids = iCustomAccessList.Count();
	aStream.WriteInt32L(numSids);
	for (TInt i=0; i< numSids; ++i)
	    {
	    TUid sid = iCustomAccessList[i].SecureId();
	    aStream << sid.iUid;
	    aStream.WriteInt32L(iCustomAccessList[i].AccessMode());
	    }
	
	// MIME types
	const TInt numMimeTypes = iMimeTypes.Count();
	aStream.WriteInt32L(numMimeTypes);
	for (TInt i=0; i<numMimeTypes; ++i)
		{
		aStream << *iMimeTypes[i];
		}
	 
	// Localized names
	const TInt numLocalizedNames = iLocalizedSoftwareTypeNames.Count();
	aStream.WriteInt32L(numLocalizedNames);
	for (TInt i=0; i<numLocalizedNames; ++i)
		{
		aStream << *iLocalizedSoftwareTypeNames[i];
		}

	// Launcher executable
	aStream << *iLauncherExecutable;
	}

void CSoftwareTypeRegInfo::InternalizeL(RReadStream& aStream)
	{
	ASSERT(iUniqueSoftwareTypeName == NULL);
	
	iUniqueSoftwareTypeName = HBufC::NewL(aStream, EUniqueSwTypeNameMaxLength);
	
	iSifPluginUid = TUid::Uid(aStream.ReadInt32L());
	
	// Sids
	const TInt numSids = aStream.ReadInt32L();
	for (TInt i=0; i<numSids; ++i)
	    {
	    TSecureId secId(TUid::Uid(aStream.ReadInt32L()));
	    TCustomAccessInfo customAccessInfo(secId, static_cast<TAccessMode>(aStream.ReadInt32L()));
	    iCustomAccessList.AppendL(customAccessInfo);
	    }
	        
	// MIME types
	const TInt numMimeTypes = aStream.ReadInt32L();
	for (TInt i=0; i<numMimeTypes; ++i)
		{
		HBufC* mimeType = HBufC::NewLC(aStream, EUniqueSwTypeNameMaxLength);
		iMimeTypes.AppendL(mimeType);
		CleanupStack::Pop(mimeType);
		}
	
	// Localized names
	const TInt numLocalizedNames = aStream.ReadInt32L();
	for (TInt i=0; i<numLocalizedNames; ++i)
		{
		CLocalizedSoftwareTypeName* name = CLocalizedSoftwareTypeName::NewL(aStream);
		CleanupStack::PushL(name);
		iLocalizedSoftwareTypeNames.AppendL(name);
		CleanupStack::Pop(name);
		}

	iLauncherExecutable = HBufC::NewL(aStream, ELauncherExecutableMaxLength);
	}

EXPORT_C void CSoftwareTypeRegInfo::SetMimeTypeL(const TDesC& aMimeType)
	{
	HBufC* mimeType = HBufC::NewLC(aMimeType.Length());
	mimeType->Des().Copy(aMimeType);
	iMimeTypes.AppendL(mimeType);
	CleanupStack::Pop(mimeType);
	}

EXPORT_C void CSoftwareTypeRegInfo::SetLocalizedSoftwareTypeNameL(TLanguage aLanguage, const TDesC& aName)
	{
	CLocalizedSoftwareTypeName* name = CLocalizedSoftwareTypeName::NewLC(aName, aLanguage);
	iLocalizedSoftwareTypeNames.AppendL(name);
	CleanupStack::Pop(name);
	}

EXPORT_C void CSoftwareTypeRegInfo::SetLauncherExecutableL(const TDesC& aLauncherExecutable)
    {
    HBufC* launcherExecutable = HBufC::NewLC(aLauncherExecutable.Length());
    launcherExecutable->Des().Copy(aLauncherExecutable);
    iLauncherExecutable = launcherExecutable;
    CleanupStack::Pop(launcherExecutable);
    }

/////////////////////////////
// CComponentFilter
/////////////////////////////

CComponentFilter::CComponentFilter()
	{
	// empty 
	}

CComponentFilter::~CComponentFilter()
	{
	delete iName;
	delete iVendor;
	delete iSwType;
	delete iFile;
	iPropertyList.ResetAndDestroy();
	iPropertyOperatorList.Reset();
	}

EXPORT_C CComponentFilter* CComponentFilter::NewL()
	{
	CComponentFilter *self = CComponentFilter::NewLC();
	CleanupStack::Pop(self);
	return self;
	}
		
EXPORT_C CComponentFilter* CComponentFilter::NewLC()
	{
	CComponentFilter *self = new(ELeave) CComponentFilter();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
		
EXPORT_C CComponentFilter* CComponentFilter::NewL(RReadStream& aStream)
	{
	CComponentFilter *self = CComponentFilter::NewLC();
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

void CComponentFilter::ConstructL()
	{
	iName   = KNullDesC().AllocL();
	iVendor = KNullDesC().AllocL();
	iSwType = KNullDesC().AllocL();
	iFile   = KNullDesC().AllocL();
	}

EXPORT_C void CComponentFilter::SetNameL(const TDesC& aName)
	{
	DeleteObjectZ(iName);
	iName = aName.AllocL();
	iSetFlag |= EName;
	}
		
EXPORT_C void CComponentFilter::SetVendorL(const TDesC& aVendor)
	{
	DeleteObjectZ(iVendor);
	iVendor = aVendor.AllocL();
	iSetFlag |= EVendor;
	}

EXPORT_C void CComponentFilter::SetSoftwareTypeL(const TDesC& aSwType)
	{
	DeleteObjectZ(iSwType);
	iSwType = aSwType.AllocL();
	iSetFlag |= ESoftwareType;
	}

EXPORT_C void CComponentFilter::SetRemovable(TBool aValue)
	{
	iRemovable = aValue;
	iSetFlag |= ERemovable;
	}

EXPORT_C void CComponentFilter::SetDrmProtected(TBool aValue)
	{
	iDrmProtected = aValue;
	iSetFlag |= EDrmProtected;
	}

EXPORT_C void CComponentFilter::SetHidden(TBool aValue)
	{
	iHidden = aValue;
	iSetFlag |= EHidden;
	}

EXPORT_C void CComponentFilter::SetKnownRevoked(TBool aValue)
	{
	iKnownRevoked = aValue;
	iSetFlag |= EKnownRevoked;
	}

EXPORT_C void CComponentFilter::SetOriginVerified(TBool aValue)
	{
	iOriginVerified = aValue;
	iSetFlag |= EOriginVerified;
	}

EXPORT_C void CComponentFilter::SetScomoStateL(TScomoState aScomoState)
	{
	iScomoState = aScomoState;
	iSetFlag |= EScomoState;
	}
		
EXPORT_C void CComponentFilter::SetInstalledDrivesL(const TDriveList& aDrives)
	{
	iInstalledDrives.Copy(aDrives);
	iSetFlag |= EInstalledDrive;
	}
					
EXPORT_C void CComponentFilter::AddPropertyL(const TDesC& aName, const TDesC& aValue, TLanguage aLocale,
        TDbOperator aNameOperator, TDbOperator aValueOperator)
	{
	CPropertyEntry *prop = CLocalizablePropertyEntry::NewLC(aName, aValue, aLocale);
	iPropertyList.AppendL(prop);
	CleanupStack::Pop(prop);
	iPropertyOperatorList.AppendL(TPropertyOperator(aNameOperator, aValueOperator));
	iSetFlag |= EProperty;
	}

EXPORT_C void CComponentFilter::AddPropertyL(const TDesC& aName, TInt64 aValue, TDbOperator aNameOperator, 
        TDbOperator aValueOperator)
	{
	CPropertyEntry *prop = CIntPropertyEntry::NewLC(aName, aValue);
	iPropertyList.AppendL(prop);
	CleanupStack::Pop(prop);
	iPropertyOperatorList.AppendL(TPropertyOperator(aNameOperator, aValueOperator));
	iSetFlag |= EProperty;
	}

EXPORT_C void CComponentFilter::AddPropertyL(const TDesC& aName, const TDesC8& aValue, TDbOperator aNameOperator, 
        TDbOperator aValueOperator)
	{
	CPropertyEntry *prop = CBinaryPropertyEntry::NewLC(aName, aValue);
	iPropertyList.AppendL(prop);
	CleanupStack::Pop(prop);
	iPropertyOperatorList.AppendL(TPropertyOperator(aNameOperator, aValueOperator));
	iSetFlag |= EProperty;	
	}

EXPORT_C void CComponentFilter::SetFileL(const TDesC& aFile)
	{
	DeleteObjectZ(iFile);
	iFile = aFile.AllocL();
	iSetFlag |= EFile;
	}
		
EXPORT_C void CComponentFilter::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint16L(iSetFlag);
	aStream << *iName;
	aStream << *iVendor;
	aStream << *iSwType;
	aStream << TCardinality(iScomoState);
	aStream << iInstalledDrives;
	aStream << TCardinality(iRemovable);
	ExternalizePointersArrayL(iPropertyList, aStream);
	ExternalizeFixedLengthArrayL(iPropertyOperatorList, aStream);
	aStream << *iFile;
	aStream << TCardinality(iDrmProtected);
	aStream << TCardinality(iHidden);
	aStream << TCardinality(iKnownRevoked);
	aStream << TCardinality(iOriginVerified);
	}
		
void CComponentFilter::InternalizeL(RReadStream& aStream)
	{
	iSetFlag = aStream.ReadUint16L();
	DeleteObjectZ(iName);
	iName = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	DeleteObjectZ(iVendor);
	iVendor = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	DeleteObjectZ(iSwType);
	iSwType = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	TCardinality c;
	aStream >> c;
	iScomoState = static_cast<TScomoState>(static_cast<TInt>(c));
	aStream >> iInstalledDrives;
	aStream >> c;
	iRemovable = c;
	iPropertyList.Reset();
	InternalizePointersArrayL(iPropertyList, aStream);
	iPropertyOperatorList.Reset();
	InternalizeFixedLengthArrayL(iPropertyOperatorList, aStream);
	DeleteObjectZ(iFile);
	iFile = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	// Read if it is DRM protected
	aStream >> c;
	iDrmProtected = c;
	// Read if it is hidden
	aStream >> c;
	iHidden = c;
	// Read if it is known revoked
	aStream >> c;
	iKnownRevoked = c;
	// Read if it is origin verified
	aStream >> c;
	iOriginVerified = c;
	}

/////////////////////////////
// CLocalizedSoftwareTypeName
/////////////////////////////

CLocalizedSoftwareTypeName::CLocalizedSoftwareTypeName()
	{
	// empty
	}

CLocalizedSoftwareTypeName::CLocalizedSoftwareTypeName(TLanguage aLocale)
	:	CLocalizableCommonData(aLocale)
	{
	// empty
	}

CLocalizedSoftwareTypeName::~CLocalizedSoftwareTypeName()
	{
	delete iName;
	}

EXPORT_C CLocalizedSoftwareTypeName* CLocalizedSoftwareTypeName::NewL(const TDesC& aName, TLanguage aLocale)
	{
	CLocalizedSoftwareTypeName *self = CLocalizedSoftwareTypeName::NewLC(aName, aLocale);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CLocalizedSoftwareTypeName* CLocalizedSoftwareTypeName::NewLC(const TDesC& aName, TLanguage aLocale)
	{
	CLocalizedSoftwareTypeName *self = new(ELeave) CLocalizedSoftwareTypeName(aLocale);
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	return self;
	}

EXPORT_C CLocalizedSoftwareTypeName* CLocalizedSoftwareTypeName::NewL(RReadStream& aStream)
	{
	CLocalizedSoftwareTypeName* self = new(ELeave) CLocalizedSoftwareTypeName();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CLocalizedSoftwareTypeName::ConstructL(const TDesC& aName)
	{
	iName = aName.AllocL();
	}


EXPORT_C void CLocalizedSoftwareTypeName::ExternalizeL(RWriteStream& aStream) const
	{
	CLocalizableCommonData::ExternalizeL(aStream);
	aStream << *iName;
	}
		
EXPORT_C void CLocalizedSoftwareTypeName::InternalizeL(RReadStream& aStream)
	{
	CLocalizableCommonData::InternalizeL(aStream);
	
	delete iName;
	iName = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	}

EXPORT_C const TDesC& CLocalizedSoftwareTypeName::NameL() const
	{
	return *iName;
	}


////////////////
// CScrLogEntry
////////////////

CScrLogEntry::CScrLogEntry()
	{
	//empty
	}

CScrLogEntry::~CScrLogEntry()
	{
	delete iComponentName;
	delete iSwTypeName;
	delete iVersion;   
	delete iGlobalId;
	}


EXPORT_C CScrLogEntry* CScrLogEntry::NewL(const TDesC& aComponentName, const TDesC& aUniqueSwTypeName, const TDesC& aGlobalId, const TDesC& aVersion, const TScrComponentOperationType aCompOpType)
	{
	CScrLogEntry *self = CScrLogEntry::NewLC(aComponentName, aUniqueSwTypeName, aGlobalId, aVersion, aCompOpType);
	CleanupStack::Pop(self);
	return self;
	}
		
EXPORT_C CScrLogEntry* CScrLogEntry::NewLC(const TDesC& aComponentName, const TDesC& aUniqueSwTypeName, const TDesC& aGlobalId, const TDesC& aVersion, const TScrComponentOperationType aCompOpType)
	{
	CScrLogEntry *self = new(ELeave) CScrLogEntry();
	CleanupStack::PushL(self);
	self->ConstructL(aComponentName, aUniqueSwTypeName, aGlobalId, aVersion, aCompOpType);
	return self;
	}

EXPORT_C CScrLogEntry* CScrLogEntry::NewLC(RReadStream& aStream)
	{
	CScrLogEntry *self = new(ELeave) CScrLogEntry();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	return self;
	}
		
void CScrLogEntry::ConstructL(const TDesC& aComponentName, const TDesC& aUniqueSwTypeName, const TDesC& aGlobalId, const TDesC& aVersion, const TScrComponentOperationType aCompOpType)
	{
	iComponentName = aComponentName.AllocL();
	iSwTypeName = aUniqueSwTypeName.AllocL();
	iGlobalId = aGlobalId.AllocL();
	iVersion = aVersion.AllocL();
	iOpType = aCompOpType;
	iRecordTime.HomeTime();
	}
		
	
EXPORT_C void CScrLogEntry::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iComponentName;
	aStream << *iSwTypeName;
	aStream << *iGlobalId; 
	aStream << *iVersion; 
	aStream.WriteInt32L(static_cast<TInt>(iOpType)); 
	WriteInt64L(aStream, iRecordTime.Int64());
	}
		 
	
EXPORT_C void CScrLogEntry::InternalizeL(RReadStream& aStream) 
	{
	DeleteObjectZ(iComponentName);
	iComponentName = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	DeleteObjectZ(iSwTypeName);
	iSwTypeName = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	DeleteObjectZ(iGlobalId);
	iGlobalId = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	DeleteObjectZ(iVersion);
	iVersion = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	iOpType = static_cast<TScrComponentOperationType>(aStream.ReadInt32L());
	iRecordTime = TTime(ReadInt64L(aStream));
	}
	    
	  
EXPORT_C const TDesC& CScrLogEntry::ComponentName() const
	{
	return *iComponentName;
	}

EXPORT_C const TDesC& CScrLogEntry::SoftwareTypeName() const
	{
	return *iSwTypeName;
	}

EXPORT_C const TDesC& CScrLogEntry::GlobalId() const
	{
	return *iGlobalId;
	}
	
EXPORT_C const TDesC& CScrLogEntry::ComponentVersion() const
	{
	return *iVersion;
	}
	
EXPORT_C const TTime CScrLogEntry::OperationTime() const
	{
	return iRecordTime;
	}
	
EXPORT_C TScrComponentOperationType CScrLogEntry::OperationType() const
	{
	return iOpType;
	}

EXPORT_C TComponentId CScrLogEntry::ComponentId() const
	{
	return iComponentId;
	}

EXPORT_C CComponentFilter::TPropertyOperator::TPropertyOperator(TDbOperator aNameOperator, TDbOperator aValueOperator):
        iNameOperator(aNameOperator),
        iValueOperator(aValueOperator)
    {
    }

void CComponentFilter::TPropertyOperator::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L(iNameOperator);
    aStream.WriteInt32L(iValueOperator);
    }

EXPORT_C void CComponentFilter::TPropertyOperator::InternalizeL(RReadStream& aStream)
    {
    iNameOperator = static_cast<TDbOperator>(aStream.ReadInt32L());
    iValueOperator = static_cast<TDbOperator>(aStream.ReadInt32L());
    }

EXPORT_C CComponentFilter::TDbOperator CComponentFilter::TPropertyOperator::NameOperator() const
    {
    return iNameOperator;
    }

EXPORT_C CComponentFilter::TDbOperator CComponentFilter::TPropertyOperator::ValueOperator() const
    {
    return iValueOperator;
    }
