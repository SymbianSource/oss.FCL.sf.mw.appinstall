/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <usif/scr/appregentries.h>
#include <usif/scr/screntries.h>
#include <scs/streamingarray.h>
#include <scs/cleanuputils.h>

using namespace Usif;
///////////////////////
//TApplicationCharacteristics
///////////////////////
EXPORT_C TApplicationCharacteristics::TApplicationCharacteristics()
    {
    iEmbeddability = ENotEmbeddable;
    iSupportsNewFile = EFalse;
    iAppIsHidden = EFalse;
    iLaunchInBackground = EFalse;
    iGroupName = KNullDesC;
    iAttributes = 0;
    }

EXPORT_C void TApplicationCharacteristics::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L((TInt)iEmbeddability);
    aStream.WriteInt32L(iSupportsNewFile);
    aStream.WriteInt32L(iAppIsHidden);
    aStream.WriteInt32L(iLaunchInBackground);
    aStream.WriteInt32L(iGroupName.Length());
    aStream.WriteL(iGroupName);
    aStream.WriteUint32L(iAttributes);
    }

EXPORT_C void TApplicationCharacteristics::InternalizeL(RReadStream& aStream) 
    {
    iEmbeddability = (TAppEmbeddability)aStream.ReadInt32L();
    iSupportsNewFile = aStream.ReadInt32L();
    iAppIsHidden = aStream.ReadInt32L();
    iLaunchInBackground = aStream.ReadInt32L();
    TInt groupNameLength = aStream.ReadInt32L();
    aStream.ReadL(iGroupName, groupNameLength);
    iAttributes = aStream.ReadUint32L();
    }

///////////////////////
//CEmbeddabilityFilter
///////////////////////
EXPORT_C TEmbeddableFilter::TEmbeddableFilter(): iEmbeddabilityFlags(0)
    {
    
    }

EXPORT_C TUint TEmbeddableFilter::EmbeddabilityFlags() const
    {
    return iEmbeddabilityFlags;
    }

void TEmbeddableFilter::SetEmbeddabilityFlags(TUint aEmbeddabilityFlags)
    {
    iEmbeddabilityFlags = aEmbeddabilityFlags;
    }

EXPORT_C void TEmbeddableFilter::AddEmbeddability(TApplicationCharacteristics::TAppEmbeddability aEmbeddability)
    {
    iEmbeddabilityFlags |= (1 << aEmbeddability);
    }

EXPORT_C TBool TEmbeddableFilter::MatchesEmbeddability(TApplicationCharacteristics::TAppEmbeddability aEmbeddability) const
    {
    TUint embeddabilityFlag = (1 << aEmbeddability);
    return ((embeddabilityFlag & iEmbeddabilityFlags) == 0) ? EFalse : ETrue;
    }

//////////////////
//CAppInfoFilter
//////////////////

CAppInfoFilter::CAppInfoFilter()
        {
        
        }

CAppInfoFilter::~CAppInfoFilter()
        {
        
        }

EXPORT_C CAppInfoFilter* CAppInfoFilter::NewLC()
        {
        CAppInfoFilter *self = new(ELeave) CAppInfoFilter();
        CleanupStack::PushL(self);
        self->ConstructL();
        return self;
        }


EXPORT_C CAppInfoFilter* CAppInfoFilter::NewL()
        {
        CAppInfoFilter *self = CAppInfoFilter::NewLC();
        CleanupStack::Pop(self);
        return self;
        }

EXPORT_C CAppInfoFilter* CAppInfoFilter::NewL(RReadStream& aStream)
        {   
        CAppInfoFilter *self = CAppInfoFilter::NewLC();
        self->InternalizeL(aStream);
        CleanupStack::Pop(self);
        return self;
        }

void CAppInfoFilter::ConstructL()
        {
        iSetFlag = EAllApps;
        }

EXPORT_C void CAppInfoFilter::ExternalizeL(RWriteStream& aStream) const
        {
        aStream.WriteUint16L(iSetFlag);
        aStream.WriteInt32L(iScreenMode);
        aStream.WriteUint32L(iEmbeddabilityFilter.EmbeddabilityFlags());
        aStream.WriteUint32L(iCapabilityAttributeMask);
        aStream.WriteUint32L(iCapabilityAttributeValue);          
        aStream.WriteUint32L(iServiceUid.iUid);
        aStream.WriteUint32L(iAppUid.iUid);
        }

void CAppInfoFilter::InternalizeL(RReadStream& aStream)
    {
    iSetFlag = aStream.ReadUint16L();
    iScreenMode = aStream.ReadInt32L();  
    iEmbeddabilityFilter.SetEmbeddabilityFlags(aStream.ReadUint32L());
    iCapabilityAttributeMask  = aStream.ReadUint32L();
    iCapabilityAttributeValue = aStream.ReadUint32L();
    iServiceUid = TUid::Uid(aStream.ReadUint32L());
    iAppUid = TUid::Uid(aStream.ReadUint32L());
    }

EXPORT_C void CAppInfoFilter::SetAllApps()
        {
        iSetFlag = EAllApps;
        }

EXPORT_C void CAppInfoFilter::SetAllApps(const TInt aScreenMode)
        {
        iScreenMode = aScreenMode;
        iSetFlag = EAllAppsWithScreenMode;
        }

EXPORT_C void CAppInfoFilter::SetEmbeddableApps()
        {
        iSetFlag = EGetEmbeddableApps;
        }

EXPORT_C void CAppInfoFilter::SetEmbeddableApps(const TInt aScreenMode)   
        {
        iScreenMode = aScreenMode;
        iSetFlag = EGetEmbeddableAppsWithSreenMode;
        }

EXPORT_C void CAppInfoFilter::SetEmbeddabilityFilter(TEmbeddableFilter& aEmbeddabilityFilter)
        {
        iEmbeddabilityFilter = aEmbeddabilityFilter;
        iSetFlag = EGetFilteredAppsWithEmbeddabilityFilter;        
        }

EXPORT_C void CAppInfoFilter::SetEmbeddabilityFilterWithScreenMode(TEmbeddableFilter& aEmbeddabilityFilter, const TInt aScreenMode)
        {
        iEmbeddabilityFilter = aEmbeddabilityFilter;
        iScreenMode = aScreenMode;
        iSetFlag = EGetFilteredAppsWithEmbeddabilityFilterWithScreenMode;           
        }
EXPORT_C void CAppInfoFilter::SetServerApps(const TUid aServiceUid)
        {
        iServiceUid = aServiceUid;
        iSetFlag = EGetServerApps;
        }
EXPORT_C void CAppInfoFilter::SetServerApps(const TUid aServiceUid,const TInt aScreenMode)
        {
        iServiceUid = aServiceUid;
        iScreenMode = aScreenMode;
        iSetFlag = EGetServerAppsWithScreenMode;
        }

EXPORT_C void CAppInfoFilter::SetCapabilityAttributeMaskAndValue(const TInt aCapabilityAttributeMask, const TInt aCapabilityAttributeValue)
        {
        iCapabilityAttributeMask = aCapabilityAttributeMask;
        iCapabilityAttributeValue =  aCapabilityAttributeValue;
        iSetFlag =  EGetFilteredAppsWithCapabilityMaskAndValue;
        }

EXPORT_C void CAppInfoFilter::SetCapabilityAttributeMaskAndValue(const TInt aCapabilityAttributeMask, const TInt aCapabilityAttributeValue, const TInt aScreenMode)
        {
        iCapabilityAttributeMask = aCapabilityAttributeMask;
        iCapabilityAttributeValue =  aCapabilityAttributeValue;
        iScreenMode = aScreenMode;
        iSetFlag = EGetFilteredAppsWithCapabilityMaskAndValueWithScreenMode;
        }

////////////////////////
//CAppServiceInfoFilter
////////////////////////

CAppServiceInfoFilter::CAppServiceInfoFilter()
    {
        
    }

CAppServiceInfoFilter::~CAppServiceInfoFilter()
    {
    delete iDataType;
    }

EXPORT_C CAppServiceInfoFilter* CAppServiceInfoFilter::NewLC()
    {
    CAppServiceInfoFilter *self = new(ELeave) CAppServiceInfoFilter();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


EXPORT_C CAppServiceInfoFilter* CAppServiceInfoFilter::NewL()
    {
    CAppServiceInfoFilter *self = CAppServiceInfoFilter::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CAppServiceInfoFilter* CAppServiceInfoFilter::NewL(RReadStream& aStream)
    {   
    CAppServiceInfoFilter *self = CAppServiceInfoFilter::NewLC();
    self->InternalizeL(aStream);
    CleanupStack::Pop(self);
    return self;
    }

void CAppServiceInfoFilter::ConstructL()
    {
    DeleteObjectZ(iDataType);
    iDataType = KNullDesC().AllocL();
    
    }

EXPORT_C void CAppServiceInfoFilter::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteUint16L(iSetFlag);
    aStream.WriteUint32L(iAppUid.iUid);
    aStream.WriteUint32L(iServiceUid.iUid);
    aStream << *iDataType;
    }

void CAppServiceInfoFilter::InternalizeL(RReadStream& aStream)
    {
    iSetFlag = aStream.ReadUint16L();
    iAppUid = TUid::Uid(aStream.ReadUint32L());
    iServiceUid = TUid::Uid(aStream.ReadUint32L());
    DeleteObjectZ(iDataType);
    iDataType = HBufC::NewL(aStream, KMaxTInt);

    }


EXPORT_C void CAppServiceInfoFilter::SetAppUidForServiceInfo (const TUid aAppUid)
    {
    iAppUid = aAppUid;
    iSetFlag = EGetServiceInfoForApp;
    }

EXPORT_C void CAppServiceInfoFilter::SetServiceUidForServiceImplementations (const TUid aServiceUid)
    {
    iServiceUid = aServiceUid;
    iSetFlag = EGetServiceImplementationForServiceUid;
    }

EXPORT_C void CAppServiceInfoFilter::SetServiceUidAndDatatTypeForServiceImplementationsL (const TUid aServiceUid, const TDesC& aDataType)
    {
    iServiceUid = aServiceUid;
    DeleteObjectZ(iDataType);
    iDataType = aDataType.AllocL();
    iSetFlag = EGetServiceImplementationForServiceUidAndDatatType;
    }

EXPORT_C void CAppServiceInfoFilter::SetAppUidAndServiceUidForOpaqueData (const TUid aAppUid, const TUid aServiceUid)
    {
    iAppUid = aAppUid;
    iServiceUid = aServiceUid;
    iSetFlag = EGetOpaqueDataForAppWithServiceUid;
    }

///////////////////////
// COpaqueData
///////////////////////

COpaqueData::COpaqueData()
    {
    // empty
    }

COpaqueData::~COpaqueData()
    {
    delete iOpaqueData;
    }

EXPORT_C COpaqueData* COpaqueData::NewL()
    {
    COpaqueData *self = COpaqueData::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C COpaqueData* COpaqueData::NewLC()
    {
    COpaqueData *self = new(ELeave) COpaqueData();
    CleanupStack::PushL(self);
    self->ConstructL(KNullDesC8(), TLanguage(0));
    return self;
    }

EXPORT_C COpaqueData* COpaqueData::NewL(TDesC8& aOpaqueData, TLanguage aLanguage)
    {
    COpaqueData *self = COpaqueData::NewLC(aOpaqueData, aLanguage);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C COpaqueData* COpaqueData::NewLC(TDesC8& aOpaqueData, TLanguage aLanguage)
    {
    COpaqueData *self = new(ELeave) COpaqueData();
    CleanupStack::PushL(self);
    self->ConstructL(aOpaqueData, aLanguage);
    return self;
    }

EXPORT_C COpaqueData* COpaqueData::NewL(RReadStream& aStream)
    {
    COpaqueData *self = COpaqueData::NewLC();
    self->InternalizeL(aStream);
    CleanupStack::Pop(self);
    return self;
    }

void COpaqueData::ConstructL(const TDesC8& aOpaqueData, TLanguage aLanguage)
    {  
	//if the length of opaque data is more than 4k, we will truncate it to 4k.
    const TInt KMaxOpaqueDataLength = 4096;    
    if (aOpaqueData.Length() > KMaxOpaqueDataLength)
        {
        iOpaqueData = HBufC8::NewL(KMaxOpaqueDataLength);
        iOpaqueData->Des().Copy(aOpaqueData.Ptr(),KMaxOpaqueDataLength);
        }
    else
        {
        iOpaqueData = aOpaqueData.AllocL();
        }
    iLanguage = aLanguage;
    }

EXPORT_C HBufC8& COpaqueData::OpaqueData()
    {
    return *iOpaqueData;
    }

EXPORT_C TLanguage COpaqueData::Language()
    {
    return iLanguage;
    }


EXPORT_C void COpaqueData::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L(iOpaqueData->Length());
    aStream << *iOpaqueData;
    aStream.WriteUint16L(iLanguage);
    }

EXPORT_C void COpaqueData::InternalizeL(RReadStream& aStream)
    {
    DeleteObjectZ(iOpaqueData);
    
    TInt32 opaqueDataLength = aStream.ReadInt32L();
    iOpaqueData = HBufC8::NewL(aStream, opaqueDataLength);
    iLanguage = (TLanguage) aStream.ReadUint16L();
    }

///////////////////////
// CApplicationRegistrationData
///////////////////////

CApplicationRegistrationData::CApplicationRegistrationData()
	{
	// empty
	}

CApplicationRegistrationData::~CApplicationRegistrationData()
	{
	iOwnedFileArray.ResetAndDestroy();
    iServiceArray.ResetAndDestroy();
    iLocalizableAppInfoList.ResetAndDestroy();
    iAppPropertiesArray.ResetAndDestroy();
    iOpaqueDataArray.ResetAndDestroy();
	delete iAppFile;									
	}

EXPORT_C CApplicationRegistrationData* CApplicationRegistrationData::NewL()
	{
	CApplicationRegistrationData *self = CApplicationRegistrationData::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CApplicationRegistrationData* CApplicationRegistrationData::NewLC()
	{
	CApplicationRegistrationData *self = new(ELeave) CApplicationRegistrationData();
	CleanupStack::PushL(self);
	self->ConstructL(KNullDesC());
	return self;
	}

EXPORT_C CApplicationRegistrationData* CApplicationRegistrationData::NewL(RReadStream& aStream)
	{
	CApplicationRegistrationData *self = CApplicationRegistrationData::NewLC();
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CApplicationRegistrationData* CApplicationRegistrationData::NewL(RPointerArray<HBufC>& aOwnedFileArray, RPointerArray<CServiceInfo>& aServiceArray, RPointerArray<CLocalizableAppInfo>& aLocalizableAppInfoList,
                                              RPointerArray<CPropertyEntry>& aAppPropertiesArray, TUid aAppUid, 
                                              const TDesC& aAppFile)
    {
    CApplicationRegistrationData *self = CApplicationRegistrationData::NewLC(aOwnedFileArray, aServiceArray, aLocalizableAppInfoList,
                                              aAppPropertiesArray, aAppUid, aAppFile);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CApplicationRegistrationData* CApplicationRegistrationData::NewLC(RPointerArray<HBufC>& aOwnedFileArray, RPointerArray<CServiceInfo>& aServiceArray, RPointerArray<CLocalizableAppInfo>& aLocalizableAppInfoList,
                                               RPointerArray<CPropertyEntry>& aAppPropertiesArray, TUid aAppUid, 
                                               const TDesC& aAppFile)
    {
    CApplicationRegistrationData *self = new(ELeave) CApplicationRegistrationData();
    CleanupStack::PushL(self);
    self->ConstructL(aOwnedFileArray, aServiceArray, aLocalizableAppInfoList,
                     aAppPropertiesArray, aAppUid, aAppFile);
    return self;
    }

EXPORT_C CApplicationRegistrationData* CApplicationRegistrationData::NewL(
        RPointerArray<HBufC>& aOwnedFileArray, RPointerArray<
                Usif::CServiceInfo>& aServiceArray, RPointerArray<
                Usif::CLocalizableAppInfo>& aLocalizableAppInfoList,
        RPointerArray<Usif::CPropertyEntry>& aAppPropertiesArray,
        RPointerArray<Usif::COpaqueData>& aOpaqueDataArray, TUid aAppUid,
        const TDesC& aAppFile, TApplicationCharacteristics& aCharacteristics,
        TInt aDefaultScreenNumber, TInt aTypeId)
    {
    CApplicationRegistrationData *self = CApplicationRegistrationData::NewLC(
            aOwnedFileArray, aServiceArray, aLocalizableAppInfoList,
            aAppPropertiesArray, aOpaqueDataArray, aAppUid, aAppFile,
            aCharacteristics, aDefaultScreenNumber, aTypeId);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CApplicationRegistrationData* CApplicationRegistrationData::NewLC(RPointerArray<HBufC>& aOwnedFileArray, RPointerArray<Usif::CServiceInfo> &aServiceArray, RPointerArray<Usif::CLocalizableAppInfo>& aLocalizableAppInfoList,
                                               RPointerArray<Usif::CPropertyEntry>& aAppPropertiesArray, 
                                               RPointerArray<Usif::COpaqueData>& aOpaqueDataArray, TUid aAppUid, 
                                               const TDesC& aAppFile, TApplicationCharacteristics& aCharacteristics,
                                               TInt aDefaultScreenNumber, TInt aTypeId)
	{
	CApplicationRegistrationData *self = new(ELeave) CApplicationRegistrationData();
	CleanupStack::PushL(self);
	self->ConstructL(aOwnedFileArray, aServiceArray, aLocalizableAppInfoList,
                     aAppPropertiesArray, aOpaqueDataArray, aAppUid, aAppFile, aCharacteristics,
                     aDefaultScreenNumber, aTypeId);
	return self;
	}
	
void CApplicationRegistrationData::ConstructL(RPointerArray<HBufC>& aOwnedFileArray, RPointerArray<Usif::CServiceInfo> &aServiceArray, RPointerArray<Usif::CLocalizableAppInfo>& aLocalizableAppInfoList,
                                               RPointerArray<Usif::CPropertyEntry>& aAppPropertiesArray, 
                                               RPointerArray<Usif::COpaqueData>& aOpaqueDataArray, TUid aAppUid, 
                                               const TDesC& aAppFile, TApplicationCharacteristics& aCharacteristics, 
                                               TInt aDefaultScreenNumber, TInt aTypeId)
	{
	iOwnedFileArray = aOwnedFileArray;
    iServiceArray = aServiceArray;
    iLocalizableAppInfoList = aLocalizableAppInfoList;
    iAppPropertiesArray = aAppPropertiesArray;
    iOpaqueDataArray = aOpaqueDataArray;
	iAppUid = aAppUid;
	DeleteObjectZ(iAppFile);
	iAppFile = aAppFile.AllocL();
	iTypeId = aTypeId;
	iCharacteristics.iAttributes = aCharacteristics.iAttributes;
	iCharacteristics.iAppIsHidden = aCharacteristics.iAppIsHidden;
	iCharacteristics.iEmbeddability = aCharacteristics.iEmbeddability;
	iCharacteristics.iSupportsNewFile = aCharacteristics.iSupportsNewFile;
	iCharacteristics.iLaunchInBackground = aCharacteristics.iLaunchInBackground;
	iCharacteristics.iGroupName = aCharacteristics.iGroupName;
	iDefaultScreenNumber = aDefaultScreenNumber;							
	}

void CApplicationRegistrationData::ConstructL(RPointerArray<HBufC>& aOwnedFileArray, RPointerArray<Usif::CServiceInfo> &aServiceArray, RPointerArray<Usif::CLocalizableAppInfo>& aLocalizableAppInfoList,
                                               RPointerArray<Usif::CPropertyEntry>& aAppPropertiesArray, TUid aAppUid, 
                                               const TDesC& aAppFile)
    {
    iOwnedFileArray = aOwnedFileArray;
    iServiceArray = aServiceArray;
    iLocalizableAppInfoList = aLocalizableAppInfoList;
    iAppPropertiesArray = aAppPropertiesArray;
    iAppUid = aAppUid;
    DeleteObjectZ(iAppFile);
    iAppFile = aAppFile.AllocL();  
    }

void CApplicationRegistrationData::ConstructL(const TDesC& aAppFile)
    {
    DeleteObjectZ(iAppFile);
    iAppFile = aAppFile.AllocL(); 
    }

EXPORT_C void CApplicationRegistrationData::ExternalizeL(RWriteStream& aStream) const
	{
	ExternalizePointersArrayL(iOwnedFileArray, aStream);
	ExternalizePointersArrayL(iServiceArray, aStream);
	ExternalizePointersArrayL(iLocalizableAppInfoList, aStream);
	ExternalizePointersArrayL(iAppPropertiesArray, aStream);
	ExternalizePointersArrayL(iOpaqueDataArray, aStream);
	aStream << iAppUid;
	aStream << *iAppFile;
	aStream.WriteInt32L(iTypeId);
	aStream << iCharacteristics;
	aStream.WriteInt32L(iDefaultScreenNumber);
	}
	
EXPORT_C void CApplicationRegistrationData::InternalizeL(RReadStream& aStream)
	{
	iOwnedFileArray.ResetAndDestroy();
	InternalizePointersArrayL(iOwnedFileArray, aStream);
	iServiceArray.ResetAndDestroy();
	InternalizePointersArrayL(iServiceArray, aStream);
	iLocalizableAppInfoList.ResetAndDestroy();
	InternalizePointersArrayL(iLocalizableAppInfoList, aStream);
	iAppPropertiesArray.ResetAndDestroy();
	InternalizePointersArrayL(iAppPropertiesArray, aStream);
	iOpaqueDataArray.ResetAndDestroy();
	InternalizePointersArrayL(iOpaqueDataArray, aStream);

	aStream >> iAppUid;

	DeleteObjectZ(iAppFile);
	iAppFile = HBufC::NewL(aStream, KMaxTInt);

	iTypeId = aStream.ReadInt32L();
	aStream >> iCharacteristics;
	iDefaultScreenNumber = aStream.ReadInt32L();
	}

EXPORT_C const TUid CApplicationRegistrationData::AppUid() const
	{
	return iAppUid;
	}

EXPORT_C const TDesC& CApplicationRegistrationData::AppFile() const
	{
	return *iAppFile;
	}

EXPORT_C TInt CApplicationRegistrationData::TypeId() const
    {
    return iTypeId;
    }

EXPORT_C TUint CApplicationRegistrationData::Attributes() const
	{
	return iCharacteristics.iAttributes;
	}

EXPORT_C TBool CApplicationRegistrationData::Hidden() const
	{
	return iCharacteristics.iAppIsHidden;
	}

EXPORT_C TApplicationCharacteristics::TAppEmbeddability CApplicationRegistrationData::Embeddability() const
	{
	return iCharacteristics.iEmbeddability;
	}

EXPORT_C TBool CApplicationRegistrationData::NewFile() const
	{
	return iCharacteristics.iSupportsNewFile;
	}

EXPORT_C TBool CApplicationRegistrationData::Launch() const
	{
	return iCharacteristics.iLaunchInBackground;
	}

EXPORT_C const TDesC& CApplicationRegistrationData::GroupName() const
	{
	return iCharacteristics.iGroupName;
	}

EXPORT_C TInt CApplicationRegistrationData::DefaultScreenNumber() const
	{
	return iDefaultScreenNumber;
	}

EXPORT_C const RPointerArray<HBufC>& CApplicationRegistrationData::OwnedFileArray() const
	{
	return iOwnedFileArray;
	}

EXPORT_C const RPointerArray<CServiceInfo>& CApplicationRegistrationData::ServiceArray() const
	{
	return iServiceArray;
	}

EXPORT_C const RPointerArray<CLocalizableAppInfo>& CApplicationRegistrationData::LocalizableAppInfoList() const
	{
	return iLocalizableAppInfoList;
	}

EXPORT_C const RPointerArray<CPropertyEntry>& CApplicationRegistrationData::AppProperties() const
	{
	return iAppPropertiesArray;
	}

EXPORT_C const RPointerArray<COpaqueData>& CApplicationRegistrationData::AppOpaqueData() const
    {
    return iOpaqueDataArray;
    }

///////////////////////
// CDataType
///////////////////////

CDataType::CDataType()
	{
	// empty
	}

CDataType::~CDataType()
	{
    delete iType;
	}

EXPORT_C CDataType* CDataType::NewL()
	{
	CDataType *self = CDataType::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDataType* CDataType::NewLC()
	{
	CDataType *self = new(ELeave) CDataType();
	CleanupStack::PushL(self);
	self->ConstructL(0, KNullDesC());
	return self;
	}

EXPORT_C CDataType* CDataType::NewL(RReadStream& aStream)
	{
	CDataType *self = CDataType::NewLC();
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDataType* CDataType::NewL(TInt aPriority, const TDesC& aType)
	{
	CDataType *self = CDataType::NewLC(aPriority, aType);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDataType* CDataType::NewLC(TInt aPriority, const TDesC& aType)
	{
	CDataType *self = new(ELeave) CDataType();
	CleanupStack::PushL(self);
	self->ConstructL(aPriority, aType);
	return self;
	}

void CDataType::ConstructL(TInt aPriority, const TDesC& aType)
	{
	iPriority = aPriority;
	DeleteObjectZ(iType);
    iType = aType.AllocL();
    }

EXPORT_C void CDataType::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iPriority);
	aStream << *iType;
	}
			
EXPORT_C void CDataType::InternalizeL(RReadStream& aStream)
	{
	iPriority = aStream.ReadInt32L();
	DeleteObjectZ(iType);
	iType = HBufC::NewL(aStream, KMaxTInt);
	}

EXPORT_C TInt CDataType::Priority() const
	{
	return iPriority;
	}

EXPORT_C const TDesC& CDataType::Type() const
	{
	return *iType;
	}

///////////////////////
// CServiceInfo
///////////////////////

CServiceInfo::CServiceInfo()
	{
	// empty
	}

CServiceInfo::~CServiceInfo()
	{
	iDataTypes.ResetAndDestroy();
	iOpaqueDataArray.ResetAndDestroy();
   }

EXPORT_C CServiceInfo* CServiceInfo::NewL()
	{
	CServiceInfo *self = CServiceInfo::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CServiceInfo* CServiceInfo::NewLC()
	{
	CServiceInfo *self = new(ELeave) CServiceInfo();
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CServiceInfo* CServiceInfo::NewL(RReadStream& aStream)
	{
	CServiceInfo *self = CServiceInfo::NewLC();
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CServiceInfo* CServiceInfo::NewL(const TUid aUid, RPointerArray<COpaqueData>& aOpaqueDataArray, RPointerArray<CDataType>& aDataTypes)
	{
	CServiceInfo *self = CServiceInfo::NewLC(aUid, aOpaqueDataArray, aDataTypes);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CServiceInfo* CServiceInfo::NewLC(const TUid aUid, RPointerArray<COpaqueData>& aOpaqueDataArray, RPointerArray<CDataType>& aDataTypes)
	{
	CServiceInfo *self = new(ELeave) CServiceInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aUid, aOpaqueDataArray, aDataTypes);
	return self;
	}
	
void CServiceInfo::ConstructL(const TUid aUid, RPointerArray<COpaqueData>& aOpaqueDataArray, RPointerArray<CDataType>& aDataTypes)
	{
	iUid = aUid;
	iOpaqueDataArray = aOpaqueDataArray;
    iDataTypes = aDataTypes;
    }

EXPORT_C void CServiceInfo::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iUid;
	ExternalizePointersArrayL(iOpaqueDataArray, aStream);
	ExternalizePointersArrayL(iDataTypes, aStream);
	}
		
EXPORT_C void CServiceInfo::InternalizeL(RReadStream& aStream)
	{
	aStream >> iUid;
	iOpaqueDataArray.Reset();
	InternalizePointersArrayL(iOpaqueDataArray, aStream);
	InternalizePointersArrayL(iDataTypes, aStream);
	}

EXPORT_C const TUid CServiceInfo::Uid() const
	{
	return iUid;
	}

EXPORT_C const RPointerArray<CDataType>& CServiceInfo::DataTypes() const
	{
	return iDataTypes;
	}

EXPORT_C const RPointerArray<COpaqueData>& CServiceInfo::OpaqueData() const
    {
    return iOpaqueDataArray;
    }

///////////////////////
// CLocalizableAppInfo
///////////////////////

CLocalizableAppInfo::CLocalizableAppInfo()
	{
	// empty
	}

CLocalizableAppInfo::~CLocalizableAppInfo()
	{
	delete iShortCaption;
    delete iGroupName;
	delete iCaptionAndIconInfo;
	iViewDataList.ResetAndDestroy();
	}

EXPORT_C CLocalizableAppInfo* CLocalizableAppInfo::NewL()
	{
	CLocalizableAppInfo *self = CLocalizableAppInfo::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CLocalizableAppInfo* CLocalizableAppInfo::NewLC()
	{
	CLocalizableAppInfo *self = new(ELeave) CLocalizableAppInfo();
	CleanupStack::PushL(self);
	self->ConstructL(KNullDesC(), KNullDesC());
	return self;
	}

EXPORT_C CLocalizableAppInfo* CLocalizableAppInfo::NewL(RReadStream& aStream)
	{
	CLocalizableAppInfo *self = CLocalizableAppInfo::NewLC();
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CLocalizableAppInfo* CLocalizableAppInfo::NewL(const TDesC& aShortCaption, TLanguage aApplicationLanguage, const TDesC& aGroupName, CCaptionAndIconInfo* aCaptionAndIconInfo, RPointerArray<CAppViewData>& aViewDataList)
	{
	CLocalizableAppInfo *self = CLocalizableAppInfo::NewLC(aShortCaption, aApplicationLanguage, aGroupName, aCaptionAndIconInfo, aViewDataList);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CLocalizableAppInfo* CLocalizableAppInfo::NewLC(const TDesC& aShortCaption, TLanguage aApplicationLanguage, const TDesC& aGroupName, CCaptionAndIconInfo* aCaptionAndIconInfo, RPointerArray<CAppViewData>& aViewDataList)
	{
	CLocalizableAppInfo *self = new(ELeave) CLocalizableAppInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aShortCaption, aApplicationLanguage, aGroupName, aCaptionAndIconInfo, aViewDataList);
	return self;
	}
	
void CLocalizableAppInfo::ConstructL(const TDesC& aShortCaption, TLanguage aApplicationLanguage, const TDesC& aGroupName, CCaptionAndIconInfo* aCaptionAndIconInfo, RPointerArray<Usif::CAppViewData> aViewDataList)
	{
	DeleteObjectZ(iShortCaption);
	iShortCaption = aShortCaption.AllocL();
	iApplicationLanguage = aApplicationLanguage;
	DeleteObjectZ(iGroupName);
	iGroupName = aGroupName.AllocL();
	iCaptionAndIconInfo = aCaptionAndIconInfo;
	iViewDataList = aViewDataList;	
    }

void CLocalizableAppInfo::ConstructL(const TDesC& aShortCaption, const TDesC& aGroupName)
    {
    DeleteObjectZ(iShortCaption);
    iShortCaption = aShortCaption.AllocL();
    DeleteObjectZ(iGroupName);
    iGroupName = aGroupName.AllocL();
    }


EXPORT_C void CLocalizableAppInfo::ExternalizeL(RWriteStream& aStream) const
	{	
	 aStream << *iShortCaption;
	 aStream << TCardinality(iApplicationLanguage);
     aStream << *iGroupName;
	if(NULL != iCaptionAndIconInfo)
	    {	    
	    aStream.WriteInt8L(ETrue);
	    aStream << *iCaptionAndIconInfo;
	    }
	else
	    {	    
	    aStream.WriteInt8L(EFalse);    
	    }
	ExternalizePointersArrayL(iViewDataList, aStream);
	}
	
EXPORT_C void CLocalizableAppInfo::InternalizeL(RReadStream& aStream) 
	{
	DeleteObjectZ(iShortCaption);
	iShortCaption = HBufC::NewL(aStream, KMaxTInt);
	TCardinality c;
	aStream >> c;
	iApplicationLanguage = static_cast<TLanguage>(static_cast<TInt>(c));
	DeleteObjectZ(iGroupName);
	iGroupName = HBufC::NewL(aStream, KMaxTInt);
	DeleteObjectZ(iCaptionAndIconInfo);
	TBool isCaptionAndIconInfo= aStream.ReadInt8L();
	if(isCaptionAndIconInfo)
	    {
	    iCaptionAndIconInfo = CCaptionAndIconInfo::NewL(aStream);
	    }
	InternalizePointersArrayL(iViewDataList, aStream);
	}

EXPORT_C const TDesC& CLocalizableAppInfo::ShortCaption() const
	{
	return *iShortCaption;
	}

EXPORT_C TLanguage CLocalizableAppInfo::ApplicationLanguage() const
	{
	return iApplicationLanguage;
	}

EXPORT_C const TDesC& CLocalizableAppInfo::GroupName() const
	{
	return *iGroupName;
	}

EXPORT_C const RPointerArray<CAppViewData>& CLocalizableAppInfo::ViewDataList() const
	{
	return iViewDataList;
	}

EXPORT_C const CCaptionAndIconInfo* CLocalizableAppInfo::CaptionAndIconInfo() const
	{
	return iCaptionAndIconInfo;
	}

///////////////////////
// CCaptionAndIconInfo
///////////////////////

CCaptionAndIconInfo::CCaptionAndIconInfo()
	{
	// empty
	}

CCaptionAndIconInfo::~CCaptionAndIconInfo()
	{
	delete iCaption;
	delete iIconFileName;
	}

EXPORT_C CCaptionAndIconInfo* CCaptionAndIconInfo::NewL()
	{
	CCaptionAndIconInfo *self = CCaptionAndIconInfo::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCaptionAndIconInfo* CCaptionAndIconInfo::NewLC()
	{
	CCaptionAndIconInfo *self = new(ELeave) CCaptionAndIconInfo();
	CleanupStack::PushL(self);
	self->ConstructL(KNullDesC(), KNullDesC(), 0);
	return self;
	}

EXPORT_C CCaptionAndIconInfo* CCaptionAndIconInfo::NewL(RReadStream& aStream)
	{
	CCaptionAndIconInfo *self = CCaptionAndIconInfo::NewLC();
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCaptionAndIconInfo* CCaptionAndIconInfo::NewL(const TDesC& aCaption, const TDesC& aIconFileName, TInt aNumOfAppIcons)
	{
	CCaptionAndIconInfo *self = CCaptionAndIconInfo::NewLC(aCaption, aIconFileName, aNumOfAppIcons);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCaptionAndIconInfo* CCaptionAndIconInfo::NewLC(const TDesC& aCaption, const TDesC& aIconFileName, TInt aNumOfAppIcons)
	{
	CCaptionAndIconInfo *self = new(ELeave) CCaptionAndIconInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aCaption, aIconFileName, aNumOfAppIcons);
	return self;
	}
	
void CCaptionAndIconInfo::ConstructL(const TDesC& aCaption, const TDesC& aIconFileName, TInt aNumOfAppIcons)
	{
	DeleteObjectZ(iCaption);
	iCaption = aCaption.AllocL();
	DeleteObjectZ(iIconFileName);
    iIconFileName = aIconFileName.AllocL();
	iNumOfAppIcons = aNumOfAppIcons;
	}

EXPORT_C void CCaptionAndIconInfo::ExternalizeL(RWriteStream& aStream) const
	{
     aStream << *iCaption;
     aStream << *iIconFileName;
	 aStream.WriteInt32L(iNumOfAppIcons);
	}
		
EXPORT_C void CCaptionAndIconInfo::InternalizeL(RReadStream& aStream)
	{
	DeleteObjectZ(iCaption);
	iCaption = HBufC::NewL(aStream, KMaxTInt);
	DeleteObjectZ(iIconFileName);
	iIconFileName = HBufC::NewL(aStream, KMaxTInt);
	iNumOfAppIcons = aStream.ReadInt32L();
	}

EXPORT_C const TDesC& CCaptionAndIconInfo::Caption() const
	{
	return *iCaption;
	}

EXPORT_C const TDesC& CCaptionAndIconInfo::IconFileName() const
	{
	return *iIconFileName;
	}

EXPORT_C TInt CCaptionAndIconInfo::NumOfAppIcons() const
	{
	return iNumOfAppIcons;
	}

///////////////////////
// CAppViewData
///////////////////////

CAppViewData::CAppViewData()
	{
	// empty
	}

CAppViewData::~CAppViewData()
	{
	delete iCaptionAndIconInfo;
	}

EXPORT_C CAppViewData* CAppViewData::NewL()
	{
	CAppViewData *self = CAppViewData::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CAppViewData* CAppViewData::NewLC()
	{
	CAppViewData *self = new(ELeave) CAppViewData();
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CAppViewData* CAppViewData::NewL(RReadStream& aStream)
	{
	CAppViewData *self = CAppViewData::NewLC();
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CAppViewData* CAppViewData::NewL(TUid aUid, TInt aScreenMode, CCaptionAndIconInfo* aCaptionAndIconInfo)
	{
	CAppViewData *self = CAppViewData::NewLC(aUid, aScreenMode, aCaptionAndIconInfo);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CAppViewData* CAppViewData::NewLC(TUid aUid, TInt aScreenMode, CCaptionAndIconInfo* aCaptionAndIconInfo)
	{
	CAppViewData *self = new(ELeave) CAppViewData();
	CleanupStack::PushL(self);
	self->ConstructL(aUid, aScreenMode, aCaptionAndIconInfo);
	return self;
	}
	
void CAppViewData::ConstructL(TUid aUid, TInt aScreenMode, CCaptionAndIconInfo* aCaptionAndIconInfo)
	{
	iUid = aUid;
    iScreenMode = aScreenMode;
	iCaptionAndIconInfo = aCaptionAndIconInfo;
	}

EXPORT_C void CAppViewData::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iUid;
	aStream.WriteInt32L(iScreenMode);
	if(NULL != iCaptionAndIconInfo)
	   {       
	   aStream.WriteInt8L(ETrue);
	   aStream << *iCaptionAndIconInfo;
	   }
	else
	   {       
	   aStream.WriteInt8L(EFalse);    
	   }
	}
			
EXPORT_C void CAppViewData::InternalizeL(RReadStream& aStream)
	{
	aStream >> iUid;
	iScreenMode = aStream.ReadInt32L();
	DeleteObjectZ(iCaptionAndIconInfo);
	TBool isCaptionAndIconInfo= aStream.ReadInt8L();
	if(isCaptionAndIconInfo)
	  {
	  iCaptionAndIconInfo = CCaptionAndIconInfo::NewL(aStream);
	  }
	}

EXPORT_C const TUid CAppViewData::Uid() const
	{
	return iUid;
	}

EXPORT_C TInt CAppViewData::ScreenMode() const
	{
	return iScreenMode;
	}

EXPORT_C const CCaptionAndIconInfo* CAppViewData::CaptionAndIconInfo() const
	{
	return iCaptionAndIconInfo;
	}


///////////////////////
// CLauncherExecutable
///////////////////////

CLauncherExecutable::CLauncherExecutable()
    {
    }

EXPORT_C CLauncherExecutable::~CLauncherExecutable()
    {
    delete iLauncher;
    }

EXPORT_C CLauncherExecutable* CLauncherExecutable::NewL()
    {
    CLauncherExecutable *self = CLauncherExecutable::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CLauncherExecutable* CLauncherExecutable::NewLC()
    {
    CLauncherExecutable *self = new(ELeave) CLauncherExecutable();
    CleanupStack::PushL(self);
    self->ConstructL(0, KNullDesC());
    return self;
    }

EXPORT_C CLauncherExecutable* CLauncherExecutable::NewL(TInt aTypeId, const TDesC& aLauncher)
    {
    CLauncherExecutable *self = CLauncherExecutable::NewLC(aTypeId, aLauncher);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CLauncherExecutable* CLauncherExecutable::NewLC(TInt aTypeId, const TDesC& aLauncher)
    {
    CLauncherExecutable *self = new(ELeave) CLauncherExecutable();
    CleanupStack::PushL(self);
    self->ConstructL(aTypeId, aLauncher);
    return self;
    }

EXPORT_C CLauncherExecutable* CLauncherExecutable::NewL(RReadStream& aStream)
    {
    CLauncherExecutable *self = CLauncherExecutable::NewLC();
    self->InternalizeL(aStream);
    CleanupStack::Pop(self);
    return self;
    }

void CLauncherExecutable::ConstructL(TInt aTypeId, const TDesC& aLauncher)
    {
    iTypeId = aTypeId;
    DeleteObjectZ(iLauncher);
    iLauncher = aLauncher.AllocL();
    }

EXPORT_C const TDesC& CLauncherExecutable::Launcher() const
    {
    return *iLauncher;
    }

EXPORT_C TInt CLauncherExecutable::TypeId() const
    {
    return iTypeId;
    }

EXPORT_C void CLauncherExecutable::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L(iTypeId);
    aStream << *iLauncher;
    }

EXPORT_C void CLauncherExecutable::InternalizeL(RReadStream& aStream)
    {       
    iTypeId = aStream.ReadInt32L();
    
    DeleteObjectZ(iLauncher);
    iLauncher = HBufC::NewL(aStream, KMaxTInt);
    }

