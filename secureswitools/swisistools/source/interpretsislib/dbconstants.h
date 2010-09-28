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
* DbConstants - Contains all names for accessing properties from the database.
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef DBCONSTANTS_H
#define DBCONSTANTS_H

#include <string>
#include "sisinfo.h"
#include "sisregistryobject.h"

namespace DbConstants
{
	extern std::string	ComponentUid;
	extern std::wstring	CompInstallType;
	extern std::wstring	CompDependencyPackageCount;
	extern std::wstring	CompDependencyComponentId;
	extern std::wstring	CompDependencyFromVersion;
	extern std::wstring	CompDependencyToVersion;
	extern std::wstring CompEmbeddedPackagePresent;
	extern std::wstring	CompEmbeddedPackageCount;
	extern std::wstring	CompEmbeddedPackageUid;
	extern std::wstring	CompEmbeddedPackageName;
	extern std::wstring	CompEmbeddedPackageVendor;
	extern std::wstring	CompEmbeddedPackageIndex;
	extern std::wstring	CompPropertyCount;
	extern std::wstring	CompPropertyKey;
	extern std::wstring	CompPropertyValue;

	// file description  - component property

	extern std::wstring	WCFileName;
	extern std::wstring	WCFileMimeType;
	extern std::wstring	WCFileSisFileOperation;
	extern std::wstring	WCFileFileOperationOptions;
	extern std::wstring	WCFileUncompressedLength;
	extern std::wstring	WCFileIndex;
	extern std::wstring	WCFileSid;
	extern std::wstring	WCFileHashAlgorithm;
	extern std::wstring	WCFileHashData;
	
	// file description  - file property
	extern std::wstring	FileMimeType;
	extern std::wstring	FileSisFileOperation;
	extern std::wstring	FileFileOperationOptions;
	extern std::wstring	FileUncompressedLength;
	extern std::wstring	FileIndex;
	extern std::wstring	FileSid;
	extern std::wstring	FileHashAlgorithm;
	extern std::wstring	FileHashData;
	
	// file description properties
	extern std::wstring	CompWildCardFileCount;
	extern std::wstring	CompSidFileName;

	// file description - Registration File Properties
	extern std::wstring	CompAppFile;
	extern std::wstring	CompAppUid;
	extern std::wstring CompTypeId;
	extern std::wstring	CompAttributes;
	extern std::wstring	CompHidden;
	extern std::wstring	CompEmbeddable;
	extern std::wstring	CompNewfile;
	extern std::wstring	CompLaunch;
	extern std::wstring	CompDefaultScreenNumber;
	extern std::wstring	CompOpaqueData;

	// file description - Localization File Properties
	extern std::wstring	CompShortCaption;
	extern std::wstring	CompCaption;
	extern std::wstring	CompNumberOfIcons;
	extern std::wstring	CompLocale;
	extern std::wstring	CompIconFile;
	extern std::wstring	CompGroupName;
	extern std::wstring	CompUID;
	extern std::wstring	CompScreenMode;
	
	// inROM
	extern std::wstring	CompInRom;
	extern std::wstring	CompIsSigned;
	extern std::wstring	CompSignedBySuCert;
	extern std::wstring	CompDeletablePreinstalled;
	extern std::wstring	CompTrust;
	extern std::wstring	CompIsRemoveWithLastDependent;
	extern std::wstring	CompTrustTimeStamp;
	extern std::wstring	CompStubFileName;

	// trust status
	extern std::wstring	CompTrustValidationStatus;
	extern std::wstring	CompTrustRevocationStatus;
	extern std::wstring	CompTrustResultDate;
	extern std::wstring	CompTrustLastCheckDate;
	extern std::wstring	CompTrustIsQuarantined;
	extern std::wstring	CompTrustQuarantinedDate;

	// InstallChainIndices
	extern std::wstring	CompInstallChainIndexCount;
	extern std::wstring	CompInstallChainIndex;
	
	// SisRegistryPackage
	extern std::wstring	CompSidCount;
	extern std::wstring CompSidsPresent;
	extern std::wstring	CompSid;

	// ControllerInfo
	extern std::wstring	CompControllerCount;
	extern std::wstring	CompControllerVersion;
	extern std::wstring	CompControllerOffset;
	extern std::wstring	CompControllerHashAlgorithm;
	extern std::wstring	CompControllerHashData;

	extern std::wstring	CompLanguage;
	extern std::wstring	CompSelectedDrive;

	extern std::wstring	CompUid;
	extern std::wstring	CompUniqueVendorName;
	extern std::wstring	CompIndex;

	// These defaults need to be synchronised with the ones in scrdbconstants.h. They are an optimisation which makes component and file properties optional in the case
	// they have default values. This saves SCR DB size and access times
	static const CSISInfo::TSISInstallationType KDefaultInstallType = CSISInfo::EInstInstallation;	
	static const bool KDefaultIsInRom = true;
	static const bool KDefaultIsDeletablePreInstalled = false;
	static const bool KDefaultIsSigned = false;
	static const int KDefaultTrustValue = SisRegistryObject::KSisPackageCertificateChainValidatedToTrustAnchor;
	static const int KDefaultRemoveWithLastDependent = 0;
	static const bool KDefaultIsSignedBySuCert = false;
	static const int KDefaultSidCount = 1;
	static const int KDefaultControllerCount = 1;
	static const int KDefaultEmbeddedPackageCount = 0;
	static const int KDefaultCompPropertyCount = 0;
	static const int KDefaultWildCardFileCount = 0;
	static const int KDefaultChainIndex = 1; 	
	static const int KDefaultInstalledDrive = 0; 	
	static const int KDefaultInstalledLanguage = CSISLanguage::ELangEnglish;
}	

#endif // DBCONSTANTS_H
