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
* Defines the common constant definitions for the Software Component Registry test harness
*
*/


/**
 @file 
 @internalComponent
 @test
*/

#ifndef TSCRDEFS_H
#define TSCRDEFS_H

const TInt MAX_SCR_ERROR_MESSAGE_LENGTH = 256;
const TInt MAX_SCR_BUFFER_LENGTH = 64;
const TInt MAX_SCR_PARAM_LENGTH = 32;
const TInt MAX_SCR_PARAM_VALUE_LENGTH = 10;

/** The path where data files live. */
_LIT(KDataFilesPath, "z:\\tusif\\tscr\\data\\");

// Common attribute names

// Component related attribute names
_LIT(KComponentsCountName, "ComponentsCount");
_LIT(KComponentLocaleName, "ComponentLocale");
_LIT(KComponentIdOffsetName, "ComponentIdOffset");
_LIT(KComponentIdName, "ComponentId");
_LIT(KComponentName, "ComponentName");
_LIT(KVendorName, "VendorName");
_LIT(KRemovableName, "Removable");
_LIT(KDrmProtectedName, "DrmProtected");
_LIT(KHiddenName, "Hidden");
_LIT(KKnownRevokedName, "KnownRevoked");
_LIT(KOriginVerifiedName, "OriginVerified");
_LIT(KComponentSizeName, "ComponentSize");
_LIT(KComponentScomoStateName, "ScomoState");
_LIT(KInstalledDrivesName, "InstalledDrives");
_LIT(KVersionName, "Version");
_LIT(KSoftwareTypeName, "SoftwareType");
_LIT(KGlobalIdName, "GlobalIdName");
_LIT(KGlobalIdCount,"GlobalIdCount");
_LIT(KOperationType, "OperationType");
_LIT(KLogsCountParamName, "LogsCount");
// appreg related attribute names
_LIT(KOwnedFileName, "FileName");
_LIT(KServiceUid, "ServiceUid");
_LIT(KServiceOpaqueLocale, "ServiceOpaqueLocale");
_LIT(KServiceOpaqueData, "ServiceOpaqueData");
_LIT(KAppOpaqueDataLocale, "AppOpaqueLocale");
_LIT(KAppOpaqueData, "AppOpaqueData");
_LIT(KServiceDataTypePriority, "ServiceDataTypePriority");
_LIT(KServiceDataType, "ServiceDataType");
_LIT(KLocShortCaption, "LocShortCaption");
_LIT(KLocAppLanguage, "LocAppLanguage");
_LIT(KLocGroupName, "LocGroupName");
_LIT(KLocCaption, "LocCaption");
_LIT(KLocIconFileName, "LocIconFileName");
_LIT(KLocNumberOfAppIcons, "LocNumberOfAppIcons");
_LIT(KVwUid, "VwUid");
_LIT(KVwScreenMode, "VwScreenMode");
_LIT(KVwCaption, "VwCaption");
_LIT(KVwIconFileName, "VwIconFileName");
_LIT(KVwNumberOfAppIcons, "VwNumberOfAppIcons");
_LIT(KPropertyLocale, "PropertyLocale");
_LIT(KPropertyName, "PropertyName");
_LIT(KPropertyIntValue, "PropertyIntValue");
_LIT(KPropertyStrValue, "PropertyStrValue");
_LIT(KViewDataCount, "ViewDataCount");
_LIT(KServiceOpaqueDataInfoCount, "ServiceOpaqueDataInfoCount");
_LIT(KServiceDataTypeCount,"ServiceDataTypeCount");
_LIT(KAppUid, "AppUid");
_LIT(KNoOfComponents, "NoOfComponents");
_LIT(KFormatDrive, "FormatDrive");

// File related attribute names
_LIT(KSetSizeName, "SetSize");
_LIT(KFileName, "FileName");
_LIT(KFileCount, "FileCount");

// Property related attribute names
_LIT(KIntPropertyParamName, "IntProperty");
_LIT(KStringPropertyParamName, "StringProperty");
_LIT(KPropertiesCountParamName, "PropertiesCount");
_LIT(KPropertyTypeParam, "PropertyType");
_LIT(KPropertyNameParam, "PropertyName");
_LIT(KPropertyValueParam, "PropertyValue");
_LIT(KPropertyLocaleParam, "PropertyLocale");

// Plug-in related attribute names
_LIT(KMimeTypeName, "MimeType");
_LIT(KPluginUidName, "PluginUid");

// Performance related names
_LIT(KMaxDurationName, "MaxDuration");
_LIT(KMaxTestCaseDuration, "TEST_CASE_MAXIMUM_ALLOWED_DURATION");
_LIT(KActualTestCaseDuration, "TEST_CASE_ACTUAL_DURATION");
_LIT(KPerformanceTestInfo, "PERFORMANCE_LOG_INFORMATION");

// Dependency related names
_LIT(KGlobalIdType, "GlobalIdType");
_LIT(KSupplierGlobalIdName, "SupplierGlobalIdName");
_LIT(KSupplierSwTypeName, "SupplierSwTypeName");
_LIT(KDependantGlobalIdName, "DependantGlobalIdName");
_LIT(KDependantSwTypeName, "DependantSwTypeName");
_LIT(KVersionFrom, "VersionFrom");
_LIT(KVersionTo, "VersionTo");

// Version comparetor names
_LIT(KVersionLeft, "VersionLeft");
_LIT(KVersionRight, "VersionRight");
_LIT(KExpectedResult, "ExpectedResult");

// Constants
const TChar KComponentIdDelimeter = ',';

//Apparc related attributes
_LIT(KAppEntriesCountName, "AppEntriesCount");
// TAppRegInfo related attribute  names
_LIT(KIntTAppInfoUID, "TAppInfoUID");
_LIT(KStringTAppInfoFileName, "TAppInfoFileName");
_LIT(KStringTAppInfoCaption, "TAppInfoCaption");
_LIT(KStringTAppInfoShortCaption, "TAppInfoShortCaption");


#endif /* TSCRDEFS_H */
