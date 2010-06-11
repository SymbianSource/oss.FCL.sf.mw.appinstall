/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Declares entry classes for Software Components Registry interface.
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef SCRENTRIES_PARTNER_H
#define SCRENTRIES_PARTNER_H

#include <e32base.h>
#include <s32strm.h>
#include <f32file.h> 
#include <usif/usifcommon.h>
#include <usif/scr/screntries.h>
#include <ct/rcpointerarray.h>

namespace Usif
	{
    class CLocalizedSoftwareTypeName;

    enum TAccessMode
        {
        ETransactionalSid   = 0x0001,    // Sids requiring component/application transactional support, usually Installers/Execution layers
        EMaxAccessMode      = 0xFFFF
        };
    
	NONSHARABLE_CLASS (TCustomAccessInfo)
		{
	public:
	    IMPORT_C TSecureId SecureId() const;
	    IMPORT_C TAccessMode AccessMode() const;
	    IMPORT_C TCustomAccessInfo(TSecureId aSecureId, TAccessMode aAccessMode);
			    
	private:
	    TAccessMode iAccessMode;
	    TSecureId iSecureId;
		};
		
	NONSHARABLE_CLASS (CSoftwareTypeRegInfo) : public CBase
		{
	public:
		IMPORT_C static CSoftwareTypeRegInfo* NewL(const TDesC& aUniqueSoftwareTypeName);
		IMPORT_C static CSoftwareTypeRegInfo* NewL(RReadStream& aStream);

		IMPORT_C virtual ~CSoftwareTypeRegInfo();
		
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

		inline void SetSifPluginUid(TUid aUid);
		inline void SetCustomAccessL(TCustomAccessInfo aCustomAccessListInfo);
		inline void SetCustomAccessL(TSecureId aSecureId, TAccessMode aAccessMode);
		IMPORT_C void SetMimeTypeL(const TDesC& aMimeType);
		IMPORT_C void SetLocalizedSoftwareTypeNameL(TLanguage aLanguage, const TDesC& aName);
		IMPORT_C void SetLauncherExecutableL(const TDesC& aLauncherExecutable);

		inline const TDesC& UniqueSoftwareTypeName() const;
		inline TUid SifPluginUid() const;
		inline RArray<TCustomAccessInfo> CustomAccessList() const;
		
		inline const RPointerArray<HBufC>& MimeTypes() const;
		inline const RPointerArray<CLocalizedSoftwareTypeName>& LocalizedSoftwareTypeNames() const;
		inline const TDesC& LauncherExecutable() const;

	private:
		CSoftwareTypeRegInfo();
		void InternalizeL(RReadStream& aStream);

		enum { EUniqueSwTypeNameMaxLength = 64,
               ELauncherExecutableMaxLength = 512}; 

		HBufC* iUniqueSoftwareTypeName;
		TUid iSifPluginUid;
		RArray<TCustomAccessInfo> iCustomAccessList;
		RCPointerArray<HBufC> iMimeTypes;
		RCPointerArray<CLocalizedSoftwareTypeName> iLocalizedSoftwareTypeNames;
		HBufC* iLauncherExecutable;
		};

	inline void CSoftwareTypeRegInfo::SetSifPluginUid(TUid aUid)
	    {
	    iSifPluginUid = aUid;
	    }

	inline void CSoftwareTypeRegInfo::SetCustomAccessL(TCustomAccessInfo aCustomAccessList)
	    {
	    iCustomAccessList.AppendL(aCustomAccessList);
	    }

    inline void CSoftwareTypeRegInfo::SetCustomAccessL(TSecureId aSecureId, TAccessMode aAccessMode)
        {
        iCustomAccessList.AppendL(TCustomAccessInfo(aSecureId, aAccessMode));
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

	inline RArray<TCustomAccessInfo> CSoftwareTypeRegInfo::CustomAccessList() const
        {
	    return iCustomAccessList;
	    }
	
	const RPointerArray<HBufC>& CSoftwareTypeRegInfo::MimeTypes() const
	    {
	    return iMimeTypes;
	    }

	inline const RPointerArray<CLocalizedSoftwareTypeName>& CSoftwareTypeRegInfo::LocalizedSoftwareTypeNames() const
	    {
	    return iLocalizedSoftwareTypeNames;
	    }
	
	inline const TDesC& CSoftwareTypeRegInfo::LauncherExecutable() const
	    {
	    if (iLauncherExecutable == NULL)
	        {
	        return KNullDesC;
	        }
	    return *iLauncherExecutable;
	    }
	    
	} // End of namespace Usif

#endif // SCRENTRIES_H
