// Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// aplappinforeader.h
//
/** 
* @file aplappinforeader.h
*
* @internalComponent
* @released
*/
#ifndef __APLAPPINFOREADER_H__
#define __APLAPPINFOREADER_H__


#include "apaid.h"
#include <vector>
#include "apmstd.h"
#include "barsc2.h"
#include "barsread2.h"
#include "e32lang.h"
#include "aplapplistitem.h"
#include "filedescription.h"

using namespace std;

/**
Reads application information from a combination of registration file,
optional localisable resource file and optional icon file.

@internalComponent
*/
class CAppInfoReader
	{
public:
	static CAppInfoReader* NewL(const std::string& aRegistrationFileName, TUid aAppUid, const std::string& aLocalizeFilePath);
	TBool ReadL();
	TBool ReadL(const std::vector<FileDescription*>& aFileDescription, std::string& aRomPath, int aInRom);
	~CAppInfoReader();
public:
	Ptr16* AppBinaryFullName();
	TUidType AppBinaryUidType() const;
	TUid AppUid() const;
	void Capability(TAppCapability& aCapabilityBuf) const;
	TUint DefaultScreenNumber() const;
	std::vector<CAppViewData*>* Views();
	std::vector<CAppLocalizableInfo*> LocalizableInfo();
	std::vector<CAppLocalOpaqueDataInfo*> GetOpaqueDataArray();
	std::vector<Ptr16*>* OwnedFiles();
	
	Ptr16* Caption();
	Ptr16* ShortCaption();
	TInt NumOfAppIcons() const;
	Ptr16* IconFileName();
	TBool NonMbmIconFile() const;
	
	Ptr16* LocalisableResourceFileName();
	TLanguage AppLanguage() const;
	std::vector<TAppServiceInfo*>* ServiceArray(TInt& aIndexOfFirstOpenService);
	Ptr16* CreateFullIconFileNameL(const PtrC16* aIconFileName) const;
	
private:
	CAppInfoReader(const std::string& aRegistrationFileName, TUid aAppUid, const std::string& aLocalizeFilePath);
	void ReadMimeTypesSupportedL(RResourceReader& aResourceReader, std::vector<TDataTypeWithPriority*>& aMimeTypesSupported);
	void ReadMandatoryInfoL(RResourceReader& aResourceReader);
	TInt ReadNonLocalisableInfoL(RResourceReader& aResourceReader, TUint& aLocalisableResourceId);
	TInt ReadNonLocalisableOptionalInfoL(RResourceReader& aResourceReader, CResourceFile* aRegistrationFile);
	TInt ReadLocalisableInfoLoopL(CResourceFile& aResourceFile, TUint aResourceId, TBool& aUseDefaultIcons, TInt& iLocale);
	void ReadOpaqueDataL(TUint aResourceId, CResourceFile* aRegistrationFile, TUint32 aServiceUid);
	void ReadAppSecurityInfo();
		
private:
	TUid iAppUid;
	Ptr16* iAppBinaryFullName;
	TUidType iAppBinaryUidType;
	TAppCapability iCapability;
	TUint iDefaultScreenNumber;
	Ptr16* iCaption;
	Ptr16* iShortCaption;
	TInt iNumOfAppIcons;
	std::vector<CAppViewData*>* iViewDataArray;
	std::vector<Ptr16*>* iOwnedFileArray;
	Ptr16* iIconFileName;
	TBool iNonMbmIconFile; // ETrue if icon filename is not an MBM file, however, EFalse does not necessarily mean it is an MBM file
	Ptr16* iLocalisableResourceFileName;
	TLanguage iApplicationLanguage;
	std::vector<TAppServiceInfo*>* iServiceArray;
	TInt iIndexOfFirstOpenService;
	TBool iOpenServiceIsLegacy;
	std::vector<CAppLocalOpaqueDataInfo*> iOpaqueDataArray;
private:
	const std::string& iRegistrationFileName;
	const std::string& iDrivePath;
	std::vector<CAppLocalizableInfo*> iAppLocalizableInfo;
	TBool iHasWriteDeviceDataCap;
    TBool iIsSidTrusted;
    // This flag is used to determine if app security info was allready read
    TBool iSecurityInfoHasBeenRead;
	};

#endif	// __APLAPPINFOREADER_H__
