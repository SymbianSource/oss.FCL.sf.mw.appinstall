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
* Implements the basic test step for the Software Component Registry test harness
*
*/


#include "tscrstep.h"
#include "tscrserver.h"
#include "tscrdefs.h"
#include <f32file.h>
#include <scs/cleanuputils.h>

using namespace Usif;

CScrTestStep::CScrTestStep(CScrTestServer& aParent)
// Constructor.
	: iParent(aParent)
	{	
	}

CScrTestStep::~CScrTestStep()
// Destructor.
	{
	iScrSession.Close();
	}

void CScrTestStep:: MarkAsPerformanceStep()
	{
	iIsPerformanceTest = ETrue;
	}

void CScrTestStep::PrintPerformanceLog(TTime aTime)
	{
	TDateTime timer = aTime.DateTime();
	INFO_PRINTF6(_L("%S,%d:%d:%d:%d"), &KPerformanceTestInfo(), timer.Hour(), timer.Minute(), timer.Second(), timer.MicroSecond());
	}

void CScrTestStep::StartTimer()
	{
	iStartTime.HomeTime();
	PrintPerformanceLog(iStartTime);
	}

void CScrTestStep::StopTimerAndPrintResultL()
	{
	TTime endTime;
	endTime.HomeTime();
	PrintPerformanceLog(endTime);
	
	TTimeIntervalMicroSeconds duration = endTime.MicroSecondsFrom(iStartTime);
	TInt actualDuration = I64INT(duration.Int64())/1000; // in millisecond
	
	if(iTimeMeasuredExternally)
		{ // if the time is measured by the scr accessor, update the actual time with the external time.
		actualDuration = iTimeMeasuredExternally;
		}
	
	TInt maxDuration = 0;
	if(!GetIntFromConfig(ConfigSection(), KMaxDurationName, maxDuration))
		{
		ERR_PRINTF2(_L("%S could not be found in configuration."), &KMaxDurationName());
		User::Leave(KErrNotFound);
		}
	else
		{
		INFO_PRINTF3(_L("%S,%d"), &KMaxTestCaseDuration(), maxDuration);
		INFO_PRINTF3(_L("%S,%d"), &KActualTestCaseDuration(), actualDuration);
		}
	
	if(actualDuration <= maxDuration)
		{
		INFO_PRINTF2(_L("This test meets performance requirement (Duration=%d)."), actualDuration);
		}
	else
		{
		ERR_PRINTF2(_L("This test does not meet performance requirement (Duration=%d)."), actualDuration);
		SetTestStepResult(EFail);
		}
	}

void CScrTestStep::ImplTestStepPreambleL()
/**
 	From COomTestStep.
 */
	{
	User::LeaveIfError(iScrSession.Connect());
	if(iIsPerformanceTest)
		{
		StartTimer();
		}
	}

void CScrTestStep::ImplTestStepL()
/**
	From COomTestStep.
 */
	{
	// empty
	}


void CScrTestStep::ImplTestStepPostambleL()
/**
 	From COomTestStep.
 */
	{
	if(iIsPerformanceTest)
		{
		StopTimerAndPrintResultL();
		}
	}


RScsClientBase* CScrTestStep::ClientHandle()
/**
 	From COomTestStep.
 */
	{
	return &iScrSession;
	}

void CScrTestStep::PrintErrorL(const TDesC& aMsg, TInt aErrNum,...)
	{
	VA_LIST list;
	VA_START(list, aErrNum);
	
	RBuf msgBuf;
	msgBuf.CreateL(MAX_SCR_ERROR_MESSAGE_LENGTH);
	msgBuf.CleanupClosePushL();
	msgBuf.AppendFormatList(aMsg, list);
	
	ERR_PRINTF1(msgBuf);
	SetTestStepResult(EFail);

	CleanupStack::PopAndDestroy(&msgBuf);
	User::Leave(aErrNum);
	}

//--------------------Code for properties' comparison

TBool operator ==(const TDriveList& aLhsEntry, const TDriveList& aRhsEntry)
	{
	// If two drives are equal, each byte of them must be zero or non-zero together. 
	// Otherwise, the drive lists are different.
	for(TInt driveNumber=EDriveA; driveNumber<=EDriveZ; ++driveNumber)
		{ 
		if((aLhsEntry[driveNumber] || aRhsEntry[driveNumber]) && !(aLhsEntry[driveNumber] && aRhsEntry[driveNumber]))
			return EFalse;
		}
	return ETrue;
	}

TBool operator ==(const CComponentEntry& aLhsEntry, const CComponentEntry& aRhsEntry)
	{
	return (aLhsEntry.ComponentId() == aRhsEntry.ComponentId() && 
			aLhsEntry.Name() == aRhsEntry.Name()&& 
			aLhsEntry.Vendor() == aRhsEntry.Vendor() && 
			aLhsEntry.SoftwareType() == aRhsEntry.SoftwareType() &&
			aLhsEntry.GlobalId() == aRhsEntry.GlobalId() &&
			aLhsEntry.IsRemovable() == aRhsEntry.IsRemovable() && 
			aLhsEntry.IsDrmProtected() == aRhsEntry.IsDrmProtected() && 
			aLhsEntry.IsHidden() == aRhsEntry.IsHidden() && 
			aLhsEntry.IsKnownRevoked() == aRhsEntry.IsKnownRevoked() && 
			aLhsEntry.IsOriginVerified() == aRhsEntry.IsOriginVerified() && 
			aLhsEntry.ComponentSize() == aRhsEntry.ComponentSize() &&
			aLhsEntry.ScomoState() == aRhsEntry.ScomoState() && 
			aLhsEntry.InstalledDrives() == aRhsEntry.InstalledDrives() && 
			aLhsEntry.Version() == aRhsEntry.Version());
	}

TBool operator !=(const CComponentEntry& aLhsEntry, const CComponentEntry& aRhsEntry)
	{
	return !operator ==(aLhsEntry, aRhsEntry);
	}

TBool operator ==(CLocalizablePropertyEntry& lhsEntry, CLocalizablePropertyEntry& rhsEntry)
	{
	return lhsEntry.StrValue() == rhsEntry.StrValue();
	}

TBool operator ==(CBinaryPropertyEntry& lhsEntry, CBinaryPropertyEntry& rhsEntry)
	{
	return lhsEntry.BinaryValue() == rhsEntry.BinaryValue();
	}

TBool operator ==(CIntPropertyEntry& lhsEntry, CIntPropertyEntry& rhsEntry)
	{
	return lhsEntry.Int64Value() == rhsEntry.Int64Value();
	}

template <class T> TBool CompareProperty(CPropertyEntry& lhsEntry, CPropertyEntry& rhsEntry)
	{
	// Cannot use reference dynamic_cast - Symbian does not support std::bad_cast or any other exception	
	T& lhsSubclassEntry = static_cast<T &>(lhsEntry);
	T& rhsSubclassEntry = static_cast<T &>(rhsEntry);
	return (lhsSubclassEntry == rhsSubclassEntry);	
	}

TBool operator ==(CPropertyEntry& lhsEntry, CPropertyEntry& rhsEntry)
	{
	CPropertyEntry::TPropertyType lhsPropertyType = lhsEntry.PropertyType();
	CPropertyEntry::TPropertyType rhsPropertyType = rhsEntry.PropertyType();
	if (lhsPropertyType != rhsPropertyType)
		return EFalse;
	
	switch (rhsPropertyType)
		{
		case CPropertyEntry::EBinaryProperty:
			{
			return CompareProperty<CBinaryPropertyEntry>(lhsEntry, rhsEntry);
			}
		case CPropertyEntry::ELocalizedProperty:
			{
			return CompareProperty<CLocalizablePropertyEntry>(lhsEntry, rhsEntry);
			}			
		case CPropertyEntry::EIntProperty:
			{
			return CompareProperty<CIntPropertyEntry>(lhsEntry, rhsEntry);
			}
		default:
			ASSERT(0);
		}
	ASSERT(0);
	return ETrue; // Should not reach here
	}

TBool operator !=(CPropertyEntry& lhsEntry, CPropertyEntry& rhsEntry)
	{
	return !operator ==(lhsEntry, rhsEntry);
	}


TBool CScrTestStep::ComparePropertiesL(RPointerArray<CPropertyEntry>& aFoundProperties, RPointerArray<CPropertyEntry>& aExpectedProperties)
	{
	TInt foundPropertiesCount = aFoundProperties.Count();
	
	if (aExpectedProperties.Count() != foundPropertiesCount)
		{
		ERR_PRINTF3(_L("The number of expected properties %d did not match the number of found properties %d."), aExpectedProperties.Count(), foundPropertiesCount);
		return EFalse; 
		}
	
	for (TInt i = 0; i < foundPropertiesCount; ++i)
		{
		if (*aFoundProperties[i] != *aExpectedProperties[i])
			{
			ERR_PRINTF2(_L("Property %d did not match."), i);
			return EFalse;
			}
		}	
	return ETrue;
	}

TBool operator ==(const CGlobalComponentId& lhsEntry, const CGlobalComponentId& rhsEntry)
	{
	return (lhsEntry.GlobalIdName() == rhsEntry.GlobalIdName()) && 
		   (lhsEntry.SoftwareTypeName() == rhsEntry.SoftwareTypeName());
	}

TBool operator !=(const CGlobalComponentId& lhsEntry, const CGlobalComponentId& rhsEntry)
	{
	return !operator==(lhsEntry,rhsEntry);
	}

TBool operator ==(const CVersionedComponentId& lhsEntry, const CVersionedComponentId& rhsEntry)
	{
	return (lhsEntry.GlobalId() == rhsEntry.GlobalId()) &&
		   ((lhsEntry.VersionFrom()?*lhsEntry.VersionFrom():KNullDesC()) == (rhsEntry.VersionFrom()?*rhsEntry.VersionFrom():KNullDesC())) &&
		   ((lhsEntry.VersionTo()?*lhsEntry.VersionTo():KNullDesC()) == (rhsEntry.VersionTo()?*rhsEntry.VersionTo():KNullDesC()));
	}

TBool operator !=(CVersionedComponentId& lhsEntry, CVersionedComponentId& rhsEntry)
	{
	return !operator==(lhsEntry,rhsEntry);
	}

TBool CScrTestStep::CompareVersionedComponentIdsL(RPointerArray<CVersionedComponentId>& aFoundVerCompIdList, RPointerArray<CVersionedComponentId>& aExpectedVerCompIdList)
	{
	TInt foundVerCompIdsCount = aFoundVerCompIdList.Count();
	TInt expectedVerCompIdsCount = aExpectedVerCompIdList.Count();
	
	if (foundVerCompIdsCount != expectedVerCompIdsCount)
		{
		ERR_PRINTF3(_L("The number of expected versioned component Ids %d did not match the number of found ones %d."), expectedVerCompIdsCount, foundVerCompIdsCount);
		return EFalse; 
		}
	
	for (TInt i = 0; i < foundVerCompIdsCount; ++i)
		{
		if (*aFoundVerCompIdList[i] != *aExpectedVerCompIdList[i])
			{
			ERR_PRINTF2(_L("Versioned Component Id %d did not match."), i);
			return EFalse;
			}
		}	
	
	return ETrue;
	}

void CScrTestStep::CompareComponentIdsL(RArray<TComponentId>& aFoundComponents, RArray<TComponentId>& aExpectedComponents)
	{
	TInt foundComponentsCount = aFoundComponents.Count();
	if (foundComponentsCount != aExpectedComponents.Count())
		{
		ERR_PRINTF3(_L("The number of expected components %d did not match the number of found components %d."), aExpectedComponents.Count(), foundComponentsCount);
		SetTestStepResult(EFail);
		return;			
		}

	aFoundComponents.Sort();
	aExpectedComponents.Sort();	
	for (TInt i = 0; i < foundComponentsCount; ++i)
		{
		if (aFoundComponents[i] < aExpectedComponents[i])
			{
			ERR_PRINTF2(_L("Component %d was not expected in the test."), aFoundComponents[i]);
			SetTestStepResult(EFail);
			break;
			}
		else if (aExpectedComponents[i] < aFoundComponents[i])
			{
			ERR_PRINTF2(_L("Component %d was expected, but not found."), aExpectedComponents[i]);
			SetTestStepResult(EFail);
			break;			
			}
		}	
	}


// Helper functions for getting commong attributes from configuration

void CScrTestStep::GetSoftwareTypeNameL(TPtrC& aSwTypeName)
	{
	if (!GetStringFromConfig(ConfigSection(), KSoftwareTypeName, aSwTypeName))
		PrintErrorL(_L("Software Type was not found!"), KErrNotFound);
	}

TBool CScrTestStep::GetGlobalIdNameL(TPtrC& aGlobalIdName)
	{
	if (!GetStringFromConfig(ConfigSection(), KGlobalIdName, aGlobalIdName))
		return EFalse;
	return ETrue;
	}

Usif::CGlobalComponentId* CScrTestStep::GetGlobalComponentIdLC()
	{
	TPtrC globalIdName;
	if (!GetGlobalIdNameL(globalIdName))
		PrintErrorL(_L("Global Id Name was not found!"), KErrNotFound);
		
	TPtrC swTypeName;
	GetSoftwareTypeNameL(swTypeName);
		
	return CGlobalComponentId::NewLC(globalIdName, swTypeName);
	}

Usif::CGlobalComponentId* CScrTestStep::GetGlobalComponentIdLC(const TDesC& aGlobalIdName, const TDesC& aSwTypeName)
	{
	TPtrC globalIdName;
	if (!GetStringFromConfig(ConfigSection(), aGlobalIdName, globalIdName))
		PrintErrorL(_L("Global Id Name was not found!"), KErrNotFound);
	
	TPtrC swTypeName;
	if (!GetStringFromConfig(ConfigSection(), aSwTypeName, swTypeName))
		PrintErrorL(_L("Software Type Name was not found!"), KErrNotFound);
	
	return CGlobalComponentId::NewLC(globalIdName, swTypeName);
	}

HBufC* CScrTestStep::GetVersionFromConfigL(const TDesC& aVersionName)
	{
	TPtrC version(KNullDesC());
	if(!GetStringFromConfig(ConfigSection(), aVersionName, version))
		return NULL;
	return version.AllocL();
	}

void CScrTestStep::GetExpectedVersionedComponentIdListL(RPointerArray<CVersionedComponentId>& aVerCompIdList)
	{
	TInt globalIdCount(0);
	if(!GetIntFromConfig(ConfigSection(), KGlobalIdCount, globalIdCount))
			PrintErrorL(_L("Global Id count was not found!"), KErrNotFound);
			
	TBuf<MAX_SCR_PARAM_LENGTH> paramGlobalIdName, paramSwTypeName, paramVersionFrom, paramVersionTo;
	
	for(TInt i=0; i<globalIdCount; ++i)
		{
		paramGlobalIdName = KGlobalIdName;	
		paramSwTypeName = KSoftwareTypeName;
		paramVersionFrom = KVersionFrom;
		paramVersionTo = KVersionTo;
		
		GenerateIndexedAttributeNameL(paramGlobalIdName, i);
		GenerateIndexedAttributeNameL(paramSwTypeName, i);
		GenerateIndexedAttributeNameL(paramVersionFrom, i);
		GenerateIndexedAttributeNameL(paramVersionTo, i);
		
		TPtrC globalIdName;
		if (!GetStringFromConfig(ConfigSection(), paramGlobalIdName, globalIdName))
			{
			ERR_PRINTF2(_L("The %S param could not be found in configuration."), &paramGlobalIdName);
			User::Leave(KErrNotFound);
			}
		
		TPtrC swTypeName;
		if (!GetStringFromConfig(ConfigSection(), paramSwTypeName, swTypeName))
			{
			ERR_PRINTF2(_L("The %S param could not be found in configuration."), &paramSwTypeName);
			User::Leave(KErrNotFound);
			}
		
		HBufC *versionFrom = GetVersionFromConfigL(paramVersionFrom);
		if(versionFrom)
			CleanupStack::PushL(versionFrom);
		HBufC *versionTo = GetVersionFromConfigL(paramVersionTo);
		if(versionTo)
			CleanupStack::PushL(versionTo);
				
		CGlobalComponentId *globalId = CGlobalComponentId::NewLC(globalIdName, swTypeName);
		CVersionedComponentId *verCompId = CVersionedComponentId::NewLC(*globalId, versionFrom, versionTo);
		aVerCompIdList.AppendL(verCompId);
		CleanupStack::Pop(verCompId); // owned by the array
		CleanupStack::PopAndDestroy(globalId);
		if(versionTo)
			CleanupStack::PopAndDestroy(versionTo);
		if(versionFrom)
			CleanupStack::PopAndDestroy(versionFrom);
		}
	}

void CScrTestStep::AppendSharedComponentIdL(Usif::TComponentId aComponentId)
	{
	TBuf<MAX_SCR_BUFFER_LENGTH> componentIdBuf;
	ReadSharedDataL(KComponentIdName, componentIdBuf);
	componentIdBuf.AppendNum(aComponentId);
	componentIdBuf.Append(KComponentIdDelimeter);
	WriteSharedDataL(KComponentIdName, componentIdBuf, ESetText);
	}

void CScrTestStep::ReadAllSharedComponentIdsL(RArray<TInt>& aComponentList)
	{
	TBuf<MAX_SCR_BUFFER_LENGTH> componentIdBuf;
	ReadSharedDataL(KComponentIdName, componentIdBuf);
		
	TLex parser(componentIdBuf);
	TChar currentChar;
	
	parser.Mark();
	while(!parser.Eos())
		{
		currentChar = parser.Get();
		if(KComponentIdDelimeter == currentChar)
			{
			TPtrC token = parser.MarkedToken();
			TLex tokenLex(token);
			TInt componentId;
			tokenLex.Val(componentId);
			aComponentList.AppendL(componentId);
			parser.Mark();
			}
		}
	}

TInt CScrTestStep::ReadSharedComponentIdL(TInt aOffset)
	{
	RArray<TInt> componentList;
	CleanupClosePushL(componentList);
	ReadAllSharedComponentIdsL(componentList);
	
	TInt maxListIndex = componentList.Count() - 1;
	if(aOffset<0 || aOffset>maxListIndex)
		PrintErrorL(_L("Component Id offset is not valid!"), KErrArgument);
	
	TInt componentId = componentList[maxListIndex - aOffset];
	CleanupStack::PopAndDestroy(&componentList);
	return componentId;
	}

TBool CScrTestStep::GetLocaleFromConfigL(const TDesC& aParamName, TLanguage &aLocale)
	{
	TInt localeParam;
	if (!GetIntFromConfig(ConfigSection(), aParamName, localeParam))
		return EFalse;
	aLocale = static_cast<TLanguage>(localeParam);
	return ETrue;
	}

TBool CScrTestStep::GetLocaleFromConfigL(TLanguage &aLocale)
	{
	return GetLocaleFromConfigL(KPropertyLocaleParam, aLocale);
	}

TInt CScrTestStep::GetComponentIdL()
	{
	TInt componentId(0);
	if(GetIntFromConfig(ConfigSection(), KComponentIdName, componentId))
		return componentId;
	
	TInt componentIdOffset;
	if (!GetIntFromConfig(ConfigSection(), KComponentIdOffsetName, componentIdOffset))
		PrintErrorL(_L("ComponentId Offset was not found!"), KErrNotFound);
	
	componentId = ReadSharedComponentIdL(componentIdOffset);
	INFO_PRINTF2(_L("Component Id %d"), componentId);
	return componentId;
	}

void CScrTestStep::GetFileNameFromConfigL(TPtrC& aFileName)
	{
	if (!GetStringFromConfig(ConfigSection(), KFileName, aFileName))
		PrintErrorL(_L("FileName was not found!"), KErrNotFound);
	}

void CScrTestStep::GetFileNameListFromConfigL(RPointerArray<HBufC>& aFileList)
	{
	TInt fileCount(0);
	if(!GetIntFromConfig(ConfigSection(), KFileCount, fileCount))
		PrintErrorL(_L("File count was not found!"), KErrNotFound);
		
	TBuf<MAX_SCR_PARAM_LENGTH> paramName;
	for(TInt i=0; i<fileCount; ++i)
		{
		paramName = KFileName;	
		GenerateIndexedAttributeNameL(paramName, i);
		TPtrC fileName;
		if (!GetStringFromConfig(ConfigSection(), paramName, fileName))
			{
			ERR_PRINTF2(_L("The File Name param %S could not be found in configuration."), &paramName);
			User::Leave(KErrNotFound);
			}
		HBufC* fileNameBuf = fileName.AllocLC();
		aFileList.AppendL(fileNameBuf);
		CleanupStack::Pop(fileNameBuf);
		}
	}

void CScrTestStep::GetComponentNameFromConfigL(TPtrC& aComponentName)
	{
	if(!GetStringFromConfig(ConfigSection(), KComponentName, aComponentName))
		PrintErrorL(_L("Component name was not found!"), KErrNotFound);
	}

void CScrTestStep::GetVendorNameFromConfigL(TPtrC& aVendorName)
	{
	if(!GetStringFromConfig(ConfigSection(), KVendorName, aVendorName))
		PrintErrorL(_L("Vendor name was not found!"), KErrNotFound);
	}

CComponentEntry* CScrTestStep::GetComponentEntryFromConfigLC(TBool aIsSingle, TInt aIndex)
	{
	// First, we need to generate the property name
	TBuf<MAX_SCR_PARAM_LENGTH> componentIdParam, componentIdOffsetParam, componentNameParam, componentVendorParam, softwareTypeNameParam;
	TBuf<MAX_SCR_PARAM_LENGTH> globalIdParam, installedDrivesParam, isRemovableParam, componentSizeParam, scomoStateParam, versionParam;
	TBuf<MAX_SCR_PARAM_LENGTH> isDrmProtectedParam, isHiddenParam, isKnownRevokedParam, isOriginVerifiedParam; 
	
	componentIdParam = KComponentIdName;
	componentIdOffsetParam = KComponentIdOffsetName;
	componentNameParam = KComponentName;
	componentVendorParam = KVendorName;
	softwareTypeNameParam = KSoftwareTypeName;
	globalIdParam = KGlobalIdName;
	isRemovableParam = KRemovableName;
	isDrmProtectedParam = KDrmProtectedName;
	isHiddenParam = KHiddenName;
	isKnownRevokedParam = KKnownRevokedName;
	isOriginVerifiedParam = KOriginVerifiedName;
	componentSizeParam = KComponentSizeName;
	scomoStateParam = KComponentScomoStateName;
	installedDrivesParam = KInstalledDrivesName;
	versionParam = KVersionName;
	
	if (!aIsSingle)
		{
		GenerateIndexedAttributeNameL(componentIdParam, aIndex);
		GenerateIndexedAttributeNameL(componentIdOffsetParam, aIndex);
		GenerateIndexedAttributeNameL(componentNameParam, aIndex);
		GenerateIndexedAttributeNameL(componentVendorParam, aIndex);
		GenerateIndexedAttributeNameL(softwareTypeNameParam, aIndex);
		GenerateIndexedAttributeNameL(globalIdParam, aIndex);
		GenerateIndexedAttributeNameL(isRemovableParam, aIndex);
		GenerateIndexedAttributeNameL(isDrmProtectedParam, aIndex);
		GenerateIndexedAttributeNameL(isHiddenParam, aIndex);
		GenerateIndexedAttributeNameL(isKnownRevokedParam, aIndex);
		GenerateIndexedAttributeNameL(isOriginVerifiedParam, aIndex);
		GenerateIndexedAttributeNameL(componentSizeParam, aIndex);
		GenerateIndexedAttributeNameL(scomoStateParam, aIndex);
		GenerateIndexedAttributeNameL(installedDrivesParam, aIndex);
		GenerateIndexedAttributeNameL(versionParam, aIndex);
		}
		
	TComponentId componentId(0);
	if (!GetIntFromConfig(ConfigSection(), componentIdParam, componentId))
		{
		TInt componentIdOffset;
		if (!GetIntFromConfig(ConfigSection(), componentIdOffsetParam, componentIdOffset))
			PrintErrorL(_L("The component entry params %S and %S could not be found in configuration."), KErrNotFound, &componentIdParam, &componentIdOffsetParam);
		
		componentId = ReadSharedComponentIdL(componentIdOffset);
		INFO_PRINTF2(_L("Component Id %d"), componentId);
		}
		
	TPtrC componentName;
	if (!GetStringFromConfig(ConfigSection(), componentNameParam, componentName))
		PrintErrorL(_L("The component entry param %S could not be found in configuration."), KErrNotFound, &componentNameParam);
			
	TPtrC componentVendor;
	if (!GetStringFromConfig(ConfigSection(), componentVendorParam, componentVendor))
		PrintErrorL(_L("The component entry param %S could not be found in configuration."), KErrNotFound, &componentVendorParam);
	
	TPtrC softwareTypeName;
	if (!GetStringFromConfig(ConfigSection(), softwareTypeNameParam, softwareTypeName))
		PrintErrorL(_L("The component entry param %S could not be found in configuration."), KErrNotFound, &softwareTypeNameParam);
	
	TPtrC globalId(KNullDesC);
	(void)GetStringFromConfig(ConfigSection(), globalIdParam, globalId); // This is an optional attribute. It is not crucial even if it is not provided.
	
	TBool isRemovable;
	if (!GetBoolFromConfig(ConfigSection(), isRemovableParam, isRemovable))
		PrintErrorL(_L("The component entry param %S could not be found in configuration."), KErrNotFound, &isRemovableParam);
		
	TInt64 componentSize;
	if (!Get64BitIntegerFromConfigL(componentSizeParam, componentSize))
		PrintErrorL(_L("The component entry param %S could not be found in configuration."), KErrNotFound, &componentSizeParam);	
		
	TInt scomoStateValue;
	if (!GetIntFromConfig(ConfigSection(), scomoStateParam, scomoStateValue))
		PrintErrorL(_L("The component entry param %S could not be found in configuration."), KErrNotFound, &scomoStateParam);
	TScomoState scomoState = static_cast<TScomoState>(scomoStateValue);
			
	TDriveList driveList;
	if(!GetInstalledDrivesFromConfigL(driveList, installedDrivesParam))
		PrintErrorL(_L("The component entry param %S could not be found in configuration."), KErrNotFound, &installedDrivesParam);
		
	TPtrC version;
	if (!GetStringFromConfig(ConfigSection(), versionParam, version))
		PrintErrorL(_L("The component entry param %S could not be found in configuration."), KErrNotFound, &versionParam);
	
	TBool isDrmProtected = EFalse;
	(void)GetBoolFromConfig(ConfigSection(), isDrmProtectedParam, isDrmProtected); // This is an optional attribute. It is not crucial even if it is not provided.

	TBool isHidden = EFalse;
	(void)GetBoolFromConfig(ConfigSection(), isHiddenParam, isHidden); // This is an optional attribute. It is not crucial even if it is not provided.
	
	TBool isKnownRevoked = EFalse;
	(void)GetBoolFromConfig(ConfigSection(), isKnownRevokedParam, isKnownRevoked); // This is an optional attribute. It is not crucial even if it is not provided.
		
	TBool isOriginVerified = EFalse;
	(void)GetBoolFromConfig(ConfigSection(), isOriginVerifiedParam, isOriginVerified); // This is an optional attribute. It is not crucial even if it is not provided.
	
	_LIT(KInstallTime, "20080706:112000");	
	
	return CComponentEntry::NewLC(componentId, componentName, componentVendor, softwareTypeName, globalId, isRemovable, componentSize, scomoState, driveList, version, KInstallTime, isDrmProtected, isHidden, isKnownRevoked, isOriginVerified);
	}

void CScrTestStep::GenerateIndexedAttributeNameL(TDes& aInitialAttributeName, TInt aIndex)
	{
	const TInt MAX_INT_STR_LEN = 8;
	TBuf<MAX_INT_STR_LEN> integerAppendStr;
	integerAppendStr.Format(_L("%d"), aIndex);
	aInitialAttributeName.Append(integerAppendStr);
	}

TBool CScrTestStep::Get64BitIntegerFromConfigL(const TDesC& aConfigKeyName, TInt64& aRetVal)
	{
	TPtrC int64Str;
	if (!GetStringFromConfig(ConfigSection(), aConfigKeyName, int64Str))
		return EFalse;

	TLex lex(int64Str);
	User::LeaveIfError(lex.Val(aRetVal));
	return ETrue;
	}

CPropertyEntry* CScrTestStep::GetPropertyFromConfigLC(TBool aIsSingle, TInt aIndex, TBool aSupportLocalized)
	{
	// This function can be used for getting a single property, which is defined by attributes such as PropertyName etc.
	// or a property in a set - in this case the properties would be PropertyType0, PropertyName0 etc.
	// aIsSingle defines the working mode. aIndex applies only if aIsSingle is false
	
	// First, we need to generate the property name
	TBuf<MAX_SCR_PARAM_LENGTH> propertyTypeParam, propertyNameParam, propertyValueParam, propertyLocaleParam;
	propertyTypeParam = KPropertyTypeParam;
	propertyNameParam = KPropertyNameParam;
	propertyValueParam = KPropertyValueParam;
	propertyLocaleParam = KPropertyLocaleParam;
	if (!aIsSingle)
		{
		GenerateIndexedAttributeNameL(propertyTypeParam, aIndex);
		GenerateIndexedAttributeNameL(propertyNameParam, aIndex);
		GenerateIndexedAttributeNameL(propertyValueParam, aIndex);
		GenerateIndexedAttributeNameL(propertyLocaleParam, aIndex);
		}

	TPtrC propertyName;
	if (!GetStringFromConfig(ConfigSection(), propertyNameParam, propertyName))
		{
		ERR_PRINTF2(_L("The property name param %S could not be found in configuration."), &propertyNameParam);
		User::Leave(KErrNotFound);
		}
	
	TInt propertyTypeInt;
	if (!GetIntFromConfig(ConfigSection(), propertyTypeParam, propertyTypeInt))
		{
		ERR_PRINTF2(_L("The property type param %S could not be found in configuration."), &propertyTypeParam);
		User::Leave(KErrNotFound);
		}
	
	CPropertyEntry::TPropertyType propertyType = static_cast<CPropertyEntry::TPropertyType>(propertyTypeInt);
	
	CPropertyEntry* propertyEntry(NULL);
	switch (propertyType)
		{
		case CPropertyEntry::EBinaryProperty:
		case CPropertyEntry::ELocalizedProperty:
			{
			TPtrC propertyStrValue;
			if (!GetStringFromConfig(ConfigSection(), propertyValueParam, propertyStrValue))
				{
				ERR_PRINTF2(_L("The property value param %S could not be found in configuration."), &propertyValueParam);
				User::Leave(KErrNotFound);
				}
			if(!aSupportLocalized || propertyType == CPropertyEntry::EBinaryProperty)
				{
				HBufC8* buffer8Bit = ConvertBufferTo8bitL(propertyStrValue);
				CleanupStack::PushL(buffer8Bit);
				propertyEntry = CBinaryPropertyEntry::NewL(propertyName, *buffer8Bit);
				CleanupStack::PopAndDestroy(buffer8Bit);
				break;
				}
			// Handle ELocalProperty case
			TLanguage locale;
			if (!GetLocaleFromConfigL(propertyLocaleParam, locale))
				{
				ERR_PRINTF2(_L("The property locale param %S could not be found in configuration."), &propertyLocaleParam);
				User::Leave(KErrNotFound);	
				}
			propertyEntry = CLocalizablePropertyEntry::NewL(propertyName, propertyStrValue, locale);
			break;						
			}
		case CPropertyEntry::EIntProperty:
			{
			TInt64 int64Value;
			if (!Get64BitIntegerFromConfigL(propertyValueParam, int64Value))
				{
				ERR_PRINTF2(_L("The integer param %S could not be found in configuration."), &propertyValueParam);
				User::Leave(KErrNotFound);
				}
			propertyEntry = CIntPropertyEntry::NewL(propertyName, int64Value);
			break;
			}			
		}
	CleanupStack::PushL(propertyEntry);
	return propertyEntry;
	}

void CScrTestStep::GetPropertiesFromConfigL(RPointerArray<CPropertyEntry>& aProperties, TBool aSupportLocalized)
	{
	TInt propertiesCount = 0;
	if (!GetIntFromConfig(ConfigSection(), KPropertiesCountParamName, propertiesCount))
		PrintErrorL(_L("Properties count was not found!"), KErrNotFound);
	
	if (propertiesCount < 0)
		PrintErrorL(_L("Properties count is negative!"), KErrNotFound);
	
	for (TInt i = 0; i < propertiesCount; ++i)
		{
		CPropertyEntry *propertyEntry = GetPropertyFromConfigLC(EFalse, i, aSupportLocalized);
		User::LeaveIfError(aProperties.Append(propertyEntry));
		CleanupStack::Pop(propertyEntry);
		}
	}

void CScrTestStep::GetComponentIdsFromConfigL(RArray<TComponentId>& aComponentIds)
	{
	TInt componentsCount = 0;
	if (!GetIntFromConfig(ConfigSection(), KComponentsCountName, componentsCount))
		PrintErrorL(_L("Components count was not found!"), KErrNotFound);
		
	TBuf<MAX_SCR_PARAM_LENGTH> paramName;
	 
	for (TInt i = 0; i < componentsCount; ++i)
		{
		paramName = KComponentIdOffsetName;	
		GenerateIndexedAttributeNameL(paramName, i);
		TInt componentIdOffset(0);
		if (!GetIntFromConfig(ConfigSection(), paramName, componentIdOffset))
			{
			ERR_PRINTF2(_L("The component id param %S could not be found in configuration."), &paramName);
			User::Leave(KErrNotFound);
			}
		TInt componentId = ReadSharedComponentIdL(componentIdOffset);
		aComponentIds.AppendL(componentId);
		}
	}

void CScrTestStep::GetComponentLocalizablesFromConfigL(TInt aIndex, TPtrC& aName, TPtrC& aVendor, TLanguage& aLocale)
	{
	TBuf<20> vendorParamName, componentParamName, localeParamName;
	vendorParamName = KVendorName;
	GenerateIndexedAttributeNameL(vendorParamName, aIndex);
			
	componentParamName = KComponentName;
	GenerateIndexedAttributeNameL(componentParamName, aIndex);
			
	localeParamName = KComponentLocaleName;
	GenerateIndexedAttributeNameL(localeParamName, aIndex);
			
	if (!GetStringFromConfig(ConfigSection(), componentParamName, aName))
		PrintErrorL(_L("Localised component name was not found!"), KErrNotFound);
	if (!GetStringFromConfig(ConfigSection(), vendorParamName, aVendor))
		PrintErrorL(_L("Localised vendor name was not found!"), KErrNotFound);
	if (!GetLocaleFromConfigL(localeParamName, aLocale))
		PrintErrorL(_L("Locale was not found!"), KErrNotFound);
	}

void CScrTestStep::GetLocalisedComponentsFromConfigL(RPointerArray<Usif::CLocalizableComponentInfo>& aComponentInfo)
	{
	TInt localesCount = 0;
	if (!GetIntFromConfig(ConfigSection(), _L("LocalesCount"), localesCount))
		PrintErrorL(_L("Locales count was not found!"), KErrNotFound);	
	
	for (TUint i = 0; i < localesCount; ++i)
		{
		TPtrC componentName, vendorName;
		TLanguage locale;
		GetComponentLocalizablesFromConfigL(i, componentName, vendorName, locale);
		
		CLocalizableComponentInfo* localisableComponentInfo(0);
		// If-condition is used for the sake of coverage 
		if(i)
			{
			localisableComponentInfo = CLocalizableComponentInfo::NewLC(componentName, vendorName, locale);
			}
		else
			{
			localisableComponentInfo = CLocalizableComponentInfo::NewL(componentName, vendorName, locale);
			CleanupStack::PushL(localisableComponentInfo);
			}
		aComponentInfo.AppendL(localisableComponentInfo);
		CleanupStack::Pop(localisableComponentInfo);
		}	
	}

TBool CScrTestStep::GetInstalledDrivesFromConfigL(TDriveList& aDriveList, const TDesC& aAttributeName)
	{
	TPtrC installedDrives;
	if (!GetStringFromConfig(ConfigSection(), aAttributeName, installedDrives))
		{
		return EFalse;
		}
	
	TLex parser(installedDrives);
	TChar currentChar;
	aDriveList.FillZ(KMaxDrives);
	
	parser.Mark();
	while(!parser.Eos())
		{
		currentChar = parser.Get();
		if(KComponentIdDelimeter == currentChar || parser.Eos())
			{
			TPtrC token = parser.MarkedToken();
			TLex tokenLex(token);		
			TInt driveNumber(0);
			driveNumber = token[0] - 'A';
			if(driveNumber < EDriveA || driveNumber > EDriveZ)
				{
				ERR_PRINTF3(_L("%S contains an unexpected token(%S)."), &aAttributeName, &token);
				User::Leave(KErrArgument);
				}
			++aDriveList[driveNumber];
			parser.Mark();
			}
		}
	return ETrue;
	}

TBool CScrTestStep::GetScomoStateFromConfigL(TScomoState& aScomoState, const TDesC& aAttributeName)
	{
	TInt tempScomoState(0);
	if (!GetIntFromConfig(ConfigSection(), aAttributeName, tempScomoState))
		return EFalse;
		
	aScomoState = static_cast<TScomoState>(tempScomoState);
	return ETrue;
	}


TInt CScrTestStep::GetSetSizeFromConfigL()
	{
	TInt setSize(1);
	GetIntFromConfig(ConfigSection(), KSetSizeName, setSize);
	if (setSize < 1)
		PrintErrorL(_L("Invalid set size in component filter definition"), KErrArgument);
	return setSize;
	}

Usif::TComponentId CScrTestStep::AddNonLocalisableComponentL()
	{
	TPtrC componentName;
	GetComponentNameFromConfigL(componentName);
	TPtrC vendorName;;
	GetVendorNameFromConfigL(vendorName);
	TPtrC swTypeName;
	GetSoftwareTypeNameL(swTypeName);
	
	TScrComponentOperationType operationType(EScrCompInstall);
	TInt opTypeInt(0);
	if(GetIntFromConfig(ConfigSection(), KOperationType, opTypeInt))
		{
		operationType = static_cast<TScrComponentOperationType>(opTypeInt);
		}
		
	Usif::TComponentId componentId;
	TPtrC globalIdName;
	if(GetGlobalIdNameL(globalIdName))
		componentId = iScrSession.AddComponentL(componentName, vendorName, swTypeName, &globalIdName, operationType);
	else
		componentId = iScrSession.AddComponentL(componentName, vendorName, swTypeName, NULL, operationType);
	
	return componentId;
	}

void CScrTestStep::ReadFilterPropertiesL(CComponentFilter* aFilter, TInt aPropertiesCount)
	{
	for (TInt i = 0; i < aPropertiesCount; ++i)
		{		
		TBuf<MAX_SCR_PARAM_LENGTH> propertyNameParam, intAttributeParam, strAttributeParam, localeAttributeParam;
		
		propertyNameParam = _L("FilterPropertyName");
		GenerateIndexedAttributeNameL(propertyNameParam, i);
				
		TPtrC propertyName;
		if (!GetStringFromConfig(ConfigSection(), propertyNameParam, propertyName))
			{			
			PrintErrorL(_L("Property name for property was not found in filter"), KErrNotFound);
			}		
		
		intAttributeParam = _L("FilterIntProperty");
		GenerateIndexedAttributeNameL(intAttributeParam, i);		
		
		strAttributeParam = _L("FilterStringProperty");
		GenerateIndexedAttributeNameL(strAttributeParam, i);
		
		localeAttributeParam = _L("FilterPropertyLocale");
		GenerateIndexedAttributeNameL(localeAttributeParam, i);		
		
		TPtrC propertyStrValue;		
		TInt64 int64Value;
		
		if (Get64BitIntegerFromConfigL(intAttributeParam, int64Value))
			{			
			aFilter->AddPropertyL(propertyName, int64Value);
			continue;
			}
		
		if (!GetStringFromConfig(ConfigSection(), strAttributeParam, propertyStrValue))
			continue;

		TLanguage locale;
		if (GetLocaleFromConfigL(localeAttributeParam, locale))
			{
			aFilter->AddPropertyL(propertyName, propertyStrValue, locale);
			}
		else
			{
			HBufC8* buffer8bit = ConvertBufferTo8bitL(propertyStrValue);
			CleanupStack::PushL(buffer8bit);
			aFilter->AddPropertyL(propertyName, *buffer8bit);
			CleanupStack::PopAndDestroy(buffer8bit);
			}									
		} 
	}

CComponentFilter* CScrTestStep::ReadComponentFilterFromConfigLC()
	{
	CComponentFilter* componentFilter = CComponentFilter::NewLC();
	
	TPtrC filterName;
	if (GetStringFromConfig(ConfigSection(), _L("FilterName"), filterName))
		componentFilter->SetNameL(filterName);
	
	TPtrC filterVendor;
	if (GetStringFromConfig(ConfigSection(), _L("FilterVendor"), filterVendor))
		componentFilter->SetVendorL(filterVendor);
	
	TPtrC filterSwType;
	if (GetStringFromConfig(ConfigSection(), _L("FilterSoftwareType"), filterSwType))
	componentFilter->SetSoftwareTypeL(filterSwType);
	
	TScomoState scomoState;
	if (GetScomoStateFromConfigL(scomoState, _L("FilterScomoState")))
		componentFilter->SetScomoStateL(scomoState);
	
	TDriveList filterInstalledDrives;
	_LIT(KFilterInstalledDrives, "FilterDrivesList");
	if (GetInstalledDrivesFromConfigL(filterInstalledDrives, KFilterInstalledDrives()))
		componentFilter->SetInstalledDrivesL(filterInstalledDrives);
	
	TBool filterIsRemovable(EFalse);
	if (GetBoolFromConfig(ConfigSection(), _L("FilterIsRemovable"), filterIsRemovable))
		componentFilter->SetRemovable(filterIsRemovable);
	
	TInt propertiesCount(0);
	if (GetIntFromConfig(ConfigSection(), _L("FilterPropertiesCount"), propertiesCount))
		{
		ReadFilterPropertiesL(componentFilter, propertiesCount);
		}
		
	TPtrC filterFile;
	if (GetStringFromConfig(ConfigSection(), _L("FilterFile"), filterFile))
		componentFilter->SetFileL(filterFile);
	
	TBool filterIsDrmProtected(EFalse);
	if (GetBoolFromConfig(ConfigSection(), _L("FilterIsDrmProtected"), filterIsDrmProtected))
		componentFilter->SetDrmProtected(filterIsDrmProtected);
	
	TBool filterIsHidden(EFalse);
	if (GetBoolFromConfig(ConfigSection(), _L("FilterIsHidden"), filterIsHidden))
		componentFilter->SetHidden(filterIsHidden);
	
	TBool filterIsKnownRevoked(EFalse);
	if (GetBoolFromConfig(ConfigSection(), _L("FilterIsKnownRevoked"), filterIsKnownRevoked))
		componentFilter->SetKnownRevoked(filterIsKnownRevoked);
	
	TBool filterIsOriginVerified(EFalse);
	if (GetBoolFromConfig(ConfigSection(), _L("FilterIsOriginVerified"), filterIsOriginVerified))
		componentFilter->SetOriginVerified(filterIsOriginVerified);
	
	return componentFilter;
	}
