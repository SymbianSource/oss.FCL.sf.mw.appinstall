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


#include "swtypereginfo.h"

using namespace Swi;

CLocalizedSoftwareTypeName::CLocalizedSoftwareTypeName()
	{
	}

CLocalizedSoftwareTypeName::CLocalizedSoftwareTypeName(TLanguage aLocale)
	:	iLocale(aLocale)
	{
	}

EXPORT_C CLocalizedSoftwareTypeName::~CLocalizedSoftwareTypeName()
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
	self->iName = aName.AllocL();
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

EXPORT_C void CLocalizedSoftwareTypeName::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iLocale);
	aStream << *iName;
	}

void CLocalizedSoftwareTypeName::InternalizeL(RReadStream& aStream)
	{
	TLanguage locale = static_cast<TLanguage>(aStream.ReadInt32L());
	iLocale = locale;

	delete iName;
	iName = HBufC::NewL(aStream, KMaxTInt); // No restriction on length
	}

// ##########################################################################################

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
	iMimeTypes.Close();
	iLocalizedSoftwareTypeNames.Close();
	}

EXPORT_C void CSoftwareTypeRegInfo::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iUniqueSoftwareTypeName;
	aStream << iSifPluginUid.iUid;
	
	TUid installerUid = iInstallerSecureId;
	aStream << installerUid.iUid;
	
	TUid executionLayerUid = iExecutionLayerSecureId;
	aStream << executionLayerUid.iUid;
	
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
	}

EXPORT_C TInt CSoftwareTypeRegInfo::ExternalizedSize() const
	{
	TInt size = sizeof(TUid) + sizeof(TSecureId) + sizeof(TSecureId);
	size += sizeof(TInt) + iUniqueSoftwareTypeName->Size();
	
	// MIME types
	const TInt numMimeTypes = iMimeTypes.Count();
	size += sizeof(TInt);
	for (TInt i=0; i<numMimeTypes; ++i)
		{
		size += sizeof(TInt) + iMimeTypes[i]->Size();
		}
	 
	// Localized names
	const TInt numLocalizedNames = iLocalizedSoftwareTypeNames.Count();
	size += sizeof(TInt);
	for (TInt i=0; i<numLocalizedNames; ++i)
		{
		size += sizeof(TInt) + sizeof(TInt) + iLocalizedSoftwareTypeNames[i]->Name().Size();
		}

	return size;
	}

void CSoftwareTypeRegInfo::InternalizeL(RReadStream& aStream)
	{
	ASSERT(iUniqueSoftwareTypeName == NULL);
	
	iUniqueSoftwareTypeName = HBufC::NewL(aStream, EUniqueSwTypeNameMaxLength);
	
	iSifPluginUid = TUid::Uid(aStream.ReadInt32L());
	iInstallerSecureId = TUid::Uid(aStream.ReadInt32L());
	iExecutionLayerSecureId = TUid::Uid(aStream.ReadInt32L());
	
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

// ##########################################################################################

EXPORT_C void SoftwareTypeRegInfoUtils::SerializeArrayL(const RPointerArray<CSoftwareTypeRegInfo>& aSwTypeRegInfoArray, RBuf8& aSerializedArray)
	{
	TInt bufLen = sizeof(TInt);
	const TInt count = aSwTypeRegInfoArray.Count();
	for (TInt i=0; i<count; ++i)
		{
		bufLen += aSwTypeRegInfoArray[i]->ExternalizedSize();
		}

	aSerializedArray.ReAllocL(aSerializedArray.Length()+bufLen);
	RDesWriteStream ws(aSerializedArray);
	CleanupClosePushL(ws);

	ws.WriteInt32L(count);
	for (TInt i=0; i<count; ++i)
		{
		const CSoftwareTypeRegInfo& info = *aSwTypeRegInfoArray[i];
		ws << info;
		}

	ws.CommitL();
	CleanupStack::PopAndDestroy(&ws);
	}

EXPORT_C void SoftwareTypeRegInfoUtils::UnserializeArrayL(RReadStream& aStream, RPointerArray<CSoftwareTypeRegInfo>& aSwTypeRegInfoArray)
	{
	CleanupResetAndDestroyPushL(aSwTypeRegInfoArray);
	const TInt numElems = aStream.ReadInt32L();
	for (TInt i=0; i<numElems; ++i)
		{
		CSoftwareTypeRegInfo* info = CSoftwareTypeRegInfo::NewL(aStream);
		CleanupStack::PushL(info);
		aSwTypeRegInfoArray.AppendL(info);
		CleanupStack::Pop(info);
		}
	CleanupStack::Pop(&aSwTypeRegInfoArray);
	}

EXPORT_C void SoftwareTypeRegInfoUtils::SerializeUniqueSwTypeNamesL(const RPointerArray<CSoftwareTypeRegInfo>& aSwTypeRegInfoArray, RBuf& aSerializedNames)
	{
	const TInt numNames = aSwTypeRegInfoArray.Count();
	
	TInt bufLen = 0;
	for (TInt i=0; i<numNames; ++i)
		{
		const CSoftwareTypeRegInfo& info = *aSwTypeRegInfoArray[i];
		bufLen += info.UniqueSoftwareTypeName().Length() + sizeof(TChar);
		}
	
	aSerializedNames.ReAllocL(aSerializedNames.Length()+bufLen);
	
	for (TInt i=0; i<numNames; ++i)
		{
		const CSoftwareTypeRegInfo& info = *aSwTypeRegInfoArray[i];
		aSerializedNames.Append(info.UniqueSoftwareTypeName());
		aSerializedNames.Append(static_cast<TChar>(KUniqueNameSeparator));
		}
	}

EXPORT_C void SoftwareTypeRegInfoUtils::UnserializeUniqueSwTypeNamesL(const TDesC& aSerializedNames, RArray<TPtrC>& aUniqueSwTypeNames)
	{
	CleanupClosePushL(aUniqueSwTypeNames);
	TPtrC buf(aSerializedNames);
	for (;;)
		{
		const TInt sep = buf.Locate(static_cast<TChar>(KUniqueNameSeparator));
		if (sep != KErrNotFound)
			{
			aUniqueSwTypeNames.AppendL(buf.Left(sep));
			buf.Set(buf.Mid(sep+1));
			}
		else
			{
			if (buf.Length() > 0)
				{
				User::Leave(KErrGeneral);
				}
			break;
			}
		}
	CleanupStack::Pop(&aUniqueSwTypeNames);
	}

EXPORT_C void SoftwareTypeRegInfoUtils::ExtractMimeTypesL(const RPointerArray<CSoftwareTypeRegInfo>& aSwTypeRegInfoArray, RPointerArray<HBufC8>& aMimeTypes)
	{
	for (TInt i=0; i<aSwTypeRegInfoArray.Count(); ++i)
		{
		const RPointerArray<HBufC>& mimeTypes = aSwTypeRegInfoArray[i]->MimeTypes();
		for (TInt m=0; m<mimeTypes.Count(); ++m)
			{
			const TDesC& mimeType = *mimeTypes[m];
			HBufC8* mimeType8 = HBufC8::NewLC(mimeType.Length());
			mimeType8->Des().Copy(mimeType);

			aMimeTypes.AppendL(mimeType8);

			CleanupStack::Pop(mimeType8);
			}
		}
	}
