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
* This file defines the CSoftwareTypeRegInfo class used for installation of a Layered Eexecution Environment.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SWTYPEREGINFO_H
#define SWTYPEREGINFO_H

#include <e32base.h>
#include <s32mem.h>
#include <ct/rcpointerarray.h>

namespace Swi
	{
	_LIT(KTxtSoftwareTypeComponentProperty, "softwareTypes");

	class CLocalizedSoftwareTypeName : public CBase
		{
	public:
		IMPORT_C static CLocalizedSoftwareTypeName* NewL(const TDesC& aName, TLanguage aLocale);
		IMPORT_C static CLocalizedSoftwareTypeName* NewLC(const TDesC& aName, TLanguage aLocale);
		IMPORT_C static CLocalizedSoftwareTypeName* NewL(RReadStream& aStream);
		
		IMPORT_C virtual ~CLocalizedSoftwareTypeName();
		
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
		
		inline const TDesC& Name() const;
		inline TLanguage Locale() const;
		
	private:
		CLocalizedSoftwareTypeName();
		CLocalizedSoftwareTypeName(TLanguage aLocale);

		void InternalizeL(RReadStream& aStream);
		
	private:
		HBufC* iName;
		TLanguage iLocale;
		};

// ##########################################################################################

	class CSoftwareTypeRegInfo : public CBase
		{
	public:
		IMPORT_C static CSoftwareTypeRegInfo* NewL(const TDesC& aUniqueSoftwareTypeName);
		IMPORT_C static CSoftwareTypeRegInfo* NewL(RReadStream& aStream);

		IMPORT_C virtual ~CSoftwareTypeRegInfo();
		
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
		IMPORT_C TInt ExternalizedSize() const;

		inline void SetSifPluginUid(TUid aUid);
		inline void SetInstallerSecureId(TSecureId aSecureId);
		inline void SetExecutionLayerSecureId(TSecureId aSecureId);
		IMPORT_C void SetMimeTypeL(const TDesC& aMimeType);
		IMPORT_C void SetLocalizedSoftwareTypeNameL(TLanguage aLanguage, const TDesC& aName);

		inline const TDesC& UniqueSoftwareTypeName() const;
		inline TUid SifPluginUid() const;
		inline TSecureId InstallerSecureId() const;
		inline TSecureId ExecutionLayerSecureId() const;
		inline const RPointerArray<HBufC>& MimeTypes() const;
		inline const RPointerArray<CLocalizedSoftwareTypeName>& LocalizedSoftwareTypeNames() const;

	private:
		CSoftwareTypeRegInfo();
		void InternalizeL(RReadStream& aStream);

		enum { EUniqueSwTypeNameMaxLength = 32 }; 

		HBufC* iUniqueSoftwareTypeName;
		TUid iSifPluginUid;
		TSecureId iInstallerSecureId;
		TSecureId iExecutionLayerSecureId;
		RCPointerArray<HBufC> iMimeTypes;
		RCPointerArray<CLocalizedSoftwareTypeName> iLocalizedSoftwareTypeNames;
		};

// ##########################################################################################

	class SoftwareTypeRegInfoUtils
		{
	public:
		IMPORT_C static void SerializeArrayL(const RPointerArray<CSoftwareTypeRegInfo>& aSwTypeRegInfoArray, RBuf8& aSerializedArray);
		IMPORT_C static void UnserializeArrayL(RReadStream& aStream, RPointerArray<CSoftwareTypeRegInfo>& aSwTypeRegInfoArray);
		IMPORT_C static void SerializeUniqueSwTypeNamesL(const RPointerArray<CSoftwareTypeRegInfo>& aSwTypeRegInfoArray, RBuf& aSerializedNames);
		IMPORT_C static void UnserializeUniqueSwTypeNamesL(const TDesC& aSerializedNames, RArray<TPtrC>& aUniqueSwTypeNames);
		IMPORT_C static void ExtractMimeTypesL(const RPointerArray<CSoftwareTypeRegInfo>& aSwTypeRegInfoArray, RPointerArray<HBufC8>& aMimeTypes);
	public:
		enum { KUniqueNameSeparator = 0x1E };  // 'record separator'
		};

// ##########################################################################################

	inline const TDesC& CLocalizedSoftwareTypeName::Name() const
		{
		return *iName;
		}

	inline TLanguage CLocalizedSoftwareTypeName::Locale() const
		{
		return iLocale;
		}

// ##########################################################################################

	inline void CSoftwareTypeRegInfo::SetSifPluginUid(TUid aUid)
		{
		iSifPluginUid = aUid;
		}

	inline void CSoftwareTypeRegInfo::SetInstallerSecureId(TSecureId aSecureId)
		{
		iInstallerSecureId = aSecureId;
		}

	inline void CSoftwareTypeRegInfo::SetExecutionLayerSecureId(TSecureId aSecureId)
		{
		iExecutionLayerSecureId = aSecureId;
		}

	inline const TDesC& CSoftwareTypeRegInfo::UniqueSoftwareTypeName() const
		{
		if (iUniqueSoftwareTypeName == NULL)
			{
			return KNullDesC;
			}
		return *iUniqueSoftwareTypeName;
		}

	inline TUid CSoftwareTypeRegInfo::SifPluginUid() const
		{
		return iSifPluginUid;
		}

	TSecureId CSoftwareTypeRegInfo::InstallerSecureId() const
		{
		return iInstallerSecureId;
		}

	TSecureId CSoftwareTypeRegInfo::ExecutionLayerSecureId() const
		{
		return iInstallerSecureId;
		}

	const RPointerArray<HBufC>& CSoftwareTypeRegInfo::MimeTypes() const
		{
		return iMimeTypes;
		}

	const RPointerArray<CLocalizedSoftwareTypeName>& CSoftwareTypeRegInfo::LocalizedSoftwareTypeNames() const
		{
		return iLocalizedSoftwareTypeNames;
		}
	}

#endif // SWTYPEREGINFO_H
