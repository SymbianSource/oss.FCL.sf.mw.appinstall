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
* scrdbconstants.cpp
* Constants used for the SCR related operations.
*
*/


/**
 @file 
 @released
 @internalComponent
*/
 
#ifndef __SCRDBCONSTANTS_H__
#define __SCRDBCONSTANTS_H__

#include "installtypes.h"
#include "sispackagetrust.h"
#include "sistruststatus.h"
#include "sisregistrypackage.h"

const TInt KBigBufferSize = 40;
const TInt KSmlBufferSize = 25;
const TInt KCompUidSize = 8;

// Global id is nothing but the package UID for all non-SP and non-PP type packages.
_LIT(KGlobalIdFormat, "%x");
// For SP and PP type packages global Id is the combination of package Uid and name.
_LIT(KPatchGlobalIdFormat, "%x_%S");

_LIT(KEmptyString, "");
_LIT(KVersionFormat, "%d.%d.%d");
_LIT(KCompUniqueVendorName, "CompUniqueVendorName");
_LIT(KCompLanguage, "CompLanguage");
_LIT(KCompSelectedDrive, "CompSelectedDrive");
_LIT(KCompSidCount, "CompSidCount");
_LIT(KCompSidsPresent, "CompSidsPresent");
_LIT(KCompSidFormat, "CompSid%d");
_LIT(KCompSidFileNameFormat, "CompSidFileName%d");
_LIT(KCompControllerCount, "CompControllerCount");
_LIT(KVersionNameFormat, "CompControllerVersion%d");
_LIT(KOffsetNameFormat, "CompControllerOffset%d");
_LIT(KHashDataNameFormat, "CompControllerHashData%d");
_LIT(KHashAlgorithmNameFormat, "CompControllerHashAlgorithm%d");
_LIT(KFileHashAlgorithm, "FileHashAlgorithm");
_LIT(KFileHashData, "FileHashData");
_LIT(KCompInstallType, "CompInstallType");
_LIT(KCompInRom, "CompInRom");
_LIT(KCompDeletablePreinstalled, "CompDeletablePreinstalled");
_LIT(KCompIsSigned, "CompIsSigned");
_LIT(KCompTrust, "CompTrust");
_LIT(KCompIsRemoveWithLastDependent, "CompIsRemoveWithLastDependent");
_LIT(KCompTrustTimeStamp, "CompTrustTimeStamp");	
_LIT(KCompSignedBySuCert, "CompSignedBySuCert");
_LIT(KCompUid, "CompUid");
_LIT(KCompEmbeddedPackagePresent, "CompEmbeddedPackagePresent");
_LIT(KCompEmbeddedPackageCount, "CompEmbeddedPackageCount");
_LIT(KCompEmbeddedPackageUid, "CompEmbeddedPackageUid%d");
_LIT(KCompEmbeddedPackageName, "CompEmbeddedPackageName%d");
_LIT(KCompEmbeddedPackageVendor, "CompEmbeddedPackageVendor%d");
_LIT(KCompEmbeddedPackageIndex, "CompEmbeddedPackageIndex%d");
_LIT(KCompPropertyCount, "CompPropertyCount");
_LIT(KCompInstallChainIndexCount, "CompInstallChainIndexCount");
_LIT(KCompInstallChainIndex, "CompInstallChainIndex%d");
_LIT(KNativeSoftwareType, "native");
_LIT(KCompIndex, "CompIndex");
_LIT(KCompTrustValidationStatus, "CompTrustValidationStatus");
_LIT(KCompTrustRevocationStatus, "CompTrustRevocationStatus");
_LIT(KCompTrustResultDate, "CompTrustResultDate");
_LIT(KCompTrustLastCheckDate, "CompTrustLastCheckDate");
_LIT(KCompTrustIsQuarantined, "CompTrustIsQuarantined");
_LIT(KCompTrustQuarantinedDate, "CompTrustQuarantinedDate");
_LIT(KCompPropertyKey, "CompPropertyKey%d");
_LIT(KCompPropertyValue, "CompPropertyValue%d");
_LIT(KFileMimeType, "FileMimeType");
_LIT(KFileSisFileOperation, "FileSisFileOperation");
_LIT(KFileFileOperationOptions, "FileFileOperationOptions");	
_LIT(KFileSid, "FileSid");
_LIT(KStubFileName, "StubFileName");

_LIT(KCompWildCardFileCount, "CompWildCardFileCount");
_LIT(KCompWildCardFile, "WCFileName%d");

// Default values of some SWI common/custom component properties
// If the corresponding property value of a component is default, then the property won't be set in the SCR database
// This optimization provides both size and speed benefits.
// The current settings are designed to favour in ROM components, as the recent N devices (N96 etc.) have > 150 in ROM native components
static const Swi::Sis::TInstallType KDefaultInstallType = Swi::Sis::EInstInstallation;
static const TBool KDefaultIsInRom = ETrue;				  
static const TBool KDefaultIsDeletablePreInstalled = EFalse;  
static const TBool KDefaultIsSigned = EFalse; 				  
static const Swi::TSisPackageTrust KDefaultTrustValue = Swi::ESisPackageBuiltIntoRom; 
static const TInt KDefaultRemoveWithLastDependent = 0;				 
static const TBool KDefaultIsSignedBySuCert = EFalse; 		 				 
static const TInt KDefaultSidCount = 1;					     
static const TInt KDefaultControllerCount = 1;				 
static const TInt KDefaultEmbeddedPackageCount = 0;		     
static const TInt KDefaultCompPropertyCount = 0;			  
static const Swi::TValidationStatus KDefaultValidationStatus = Swi::EPackageInRom; 
static const Swi::TRevocationStatus  KDefaultRevocationStatus = Swi::EOcspNotPerformed; 
static const TInt KDefaultGeneralDate = 0; 					  	
static const TInt KDefaultIsQuarantined = 0; 
static const TInt KDefaultChainIndex = 0; 
static const TInt KDefaultWildCardFileCount = 0;
static const TInt KDefaultVersionMajor = 1;
static const TInt KDefaultVersionMinor = 0;
static const TInt KDefaultVersionBuild = 0;
static const TInt KDefaultComponentIndex = Swi::CSisRegistryPackage::PrimaryIndex;
static const TBool KDefaultIsDrmProtected = EFalse;
static const TBool KDefaultIsComponentHidden = EFalse;

#endif
