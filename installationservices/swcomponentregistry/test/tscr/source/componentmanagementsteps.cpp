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
* Implements the test steps for component management APIs in the SCR 
*
*/


#include "componentmanagementsteps.h"
#include "tscrdefs.h"
#include <scs/cleanuputils.h>

using namespace Usif;


// -----------CScrAddComponentStep-----------------

CScrAddComponentStep::CScrAddComponentStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrAddComponentStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

Usif::TComponentId CScrAddComponentStep::AddLocalisableComponentL()
	{
	RPointerArray<CLocalizableComponentInfo> componentInfo;
	CleanupResetAndDestroyPushL(componentInfo);
	GetLocalisedComponentsFromConfigL(componentInfo);
	
	TPtrC swTypeName;
	GetSoftwareTypeNameL(swTypeName);
	
	TScrComponentOperationType operationType(EScrCompInstall);
	TInt opTypeInt(0);
	if(GetIntFromConfig(ConfigSection(), KOperationType, opTypeInt))
		{
		operationType = static_cast<TScrComponentOperationType>(opTypeInt);
		}
		
	TComponentId componentId;
	TPtrC globalIdName;
	if(GetGlobalIdNameL(globalIdName))
		componentId = iScrSession.AddComponentL(componentInfo, swTypeName, &globalIdName,operationType);
	else
		componentId = iScrSession.AddComponentL(componentInfo, swTypeName,NULL,operationType);
	
	CleanupStack::PopAndDestroy(&componentInfo);
	return componentId;
	}

void CScrAddComponentStep::ImplTestStepL()
	{		
	TBool isLocalisable(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("IsLocalisable"), isLocalisable);
	TComponentId componentId(0);
	
	if (isLocalisable)
		componentId = AddLocalisableComponentL();
	else
		componentId = AddNonLocalisableComponentL(iScrSession);
	
	TPtrC version;
	if (GetStringFromConfig(ConfigSection(), KVersionName, version))
		{
		// Version param is provided. Set the version of the component.
		iScrSession.SetComponentVersionL(componentId, version);
		}
			
	AppendSharedComponentIdL(componentId);
	}


void CScrAddComponentStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrSetComponentPropertyStep-----------------

CScrSetComponentPropertyStep::CScrSetComponentPropertyStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrSetComponentPropertyStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrSetComponentPropertyStep::ImplTestStepL()
	{
	TInt componentId = GetComponentIdL();

	RPointerArray<CPropertyEntry> properties;
	CleanupResetAndDestroyPushL(properties);
	GetPropertiesFromConfigL(properties, ETrue); // ETrue means that the localizable properties are supported.
	TInt count = properties.Count();
	for(TInt i=0; i<count; ++i)
		{
		CPropertyEntry::TPropertyType propertyType = properties[i]->PropertyType();
		switch(propertyType)
			{
			case CPropertyEntry::EBinaryProperty:
				{
				CBinaryPropertyEntry* binaryProperty = dynamic_cast<CBinaryPropertyEntry*>(properties[i]);
				iScrSession.SetComponentPropertyL(componentId, binaryProperty->PropertyName(), binaryProperty->BinaryValue());
				break;
				}
			case CPropertyEntry::ELocalizedProperty:
				{
				CLocalizablePropertyEntry *locProperty = static_cast<CLocalizablePropertyEntry*>(properties[i]);
				iScrSession.SetComponentPropertyL(componentId, locProperty->PropertyName(), locProperty->StrValue(), locProperty->LocaleL());
				break;
				}
			case CPropertyEntry::EIntProperty:
				{
				CIntPropertyEntry *intProperty = dynamic_cast<CIntPropertyEntry*>(properties[i]);
				iScrSession.SetComponentPropertyL(componentId, intProperty->PropertyName(), intProperty->Int64Value());
				}
			}// switch
		}// for
	
	CleanupStack::PopAndDestroy(&properties);
	}

void CScrSetComponentPropertyStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrRegisterFileStep-----------------

CScrRegisterFileStep::CScrRegisterFileStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrRegisterFileStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrRegisterFileStep::ImplTestStepL()
	{
	RPointerArray<HBufC> fileList;
	CleanupResetAndDestroyPushL(fileList);
	GetFileNameListFromConfigL(fileList);
	
	TInt componentId = GetComponentIdL();
	
	TInt fileCount = fileList.Count();
	
	for(TInt i=0; i<fileCount; ++i)
		{
		// Fetch the Boolean flag which would tell SCR that the registered file needn't affect the drive list used by the component. The flag is optional with default ETrue
		_LIT(KConsiderDrivesPrefix, "ConsiderDrives");
		TBuf<MAX_SCR_PARAM_LENGTH> considerDrivesParamName = KConsiderDrivesPrefix();
		GenerateIndexedAttributeNameL(considerDrivesParamName, i);
		TBool considerFileInDriveList(ETrue);
		(void)GetBoolFromConfig(ConfigSection(), considerDrivesParamName, considerFileInDriveList);
		
		iScrSession.RegisterComponentFileL(componentId, *fileList[i], considerFileInDriveList);
		}
	CleanupStack::PopAndDestroy(&fileList);
	}

void CScrRegisterFileStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrAddFilePropertyStep-----------------
CScrAddFilePropertyStep::CScrAddFilePropertyStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrAddFilePropertyStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrAddFilePropertyStep::ImplTestStepL()
	{
	TInt componentId = GetComponentIdL();
	
	TPtrC fileName;
	GetFileNameFromConfigL(fileName);	
	
	RPointerArray<CPropertyEntry> propertiesAdded;
	CleanupResetAndDestroyPushL(propertiesAdded);
	GetPropertiesFromConfigL(propertiesAdded);
	
	TInt count = propertiesAdded.Count();
	for(TInt i=0; i<count; ++i)
		{
		CPropertyEntry::TPropertyType propertyType = propertiesAdded[i]->PropertyType();
		switch(propertyType)
			{
			case CPropertyEntry::EBinaryProperty:
				{
				CBinaryPropertyEntry *binaryProperty = dynamic_cast<CBinaryPropertyEntry*>(propertiesAdded[i]);
				iScrSession.SetFilePropertyL(componentId, fileName, binaryProperty->PropertyName(), binaryProperty->BinaryValue());
				break;
				}
			case CPropertyEntry::EIntProperty:
				{
				CIntPropertyEntry *intProperty = dynamic_cast<CIntPropertyEntry*>(propertiesAdded[i]);
				iScrSession.SetFilePropertyL(componentId, fileName, intProperty->PropertyName(), intProperty->IntValue());
				break;
				}
			case CPropertyEntry::ELocalizedProperty:
				{
				PrintErrorL(_L("Localizable properties are not supported for files!"), KErrAbort);
				}								
			}// switch
		}// for
	
	CleanupStack::PopAndDestroy(&propertiesAdded);
	}

void CScrAddFilePropertyStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrSetCommonPropertyStep-----------------
CScrSetCommonPropertyStep::CScrSetCommonPropertyStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrSetCommonPropertyStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrSetCommonPropertyStep::ImplTestStepL()
	{
	TInt componentId = GetComponentIdL();
	
	TPtrC version;
	if(!GetStringFromConfig(ConfigSection(), KVersionName, version))
		PrintErrorL(_L("Version was not found!"), KErrNotFound);

	TBool isRemovable;
	if(!GetBoolFromConfig(ConfigSection(), KRemovableName, isRemovable))
		PrintErrorL(_L("Removable was not found!"), KErrNotFound);

	TBool isDrmProtected;
	if(!GetBoolFromConfig(ConfigSection(), KDrmProtectedName, isDrmProtected))
		PrintErrorL(_L("DrmProtected flag was not found!"), KErrNotFound);

	TBool isHidden;
	if(!GetBoolFromConfig(ConfigSection(), KHiddenName, isHidden))
		PrintErrorL(_L("Hidden flag was not found!"), KErrNotFound);

	TBool isKnownRevoked;
	if(!GetBoolFromConfig(ConfigSection(), KKnownRevokedName, isKnownRevoked))
		PrintErrorL(_L("KnownRevoked flag was not found!"), KErrNotFound);

	TBool isOriginVerified;
	if(!GetBoolFromConfig(ConfigSection(), KOriginVerifiedName, isOriginVerified))
		PrintErrorL(_L("OriginVerified flag was not found!"), KErrNotFound);

	TInt64 componentSize(0);
	if(!Get64BitIntegerFromConfigL(KComponentSizeName(), componentSize))
		PrintErrorL(_L("ComponentSize was not found!"), KErrNotFound);
	
	TLanguage locale(ELangNone);
	if(!GetLocaleFromConfigL(locale))
		PrintErrorL(_L("Locale was not found!"), KErrNotFound);
	
	TPtrC componentName;
	GetComponentNameFromConfigL(componentName);

	TPtrC vendorName;
	GetVendorNameFromConfigL(vendorName);

	StartTimer();
    iScrSession.SetComponentVersionL(componentId, version);
    iScrSession.SetIsComponentRemovableL(componentId, isRemovable);
    iScrSession.SetIsComponentDrmProtectedL(componentId, isDrmProtected);
    iScrSession.SetIsComponentHiddenL(componentId, isHidden);
    iScrSession.SetIsComponentKnownRevokedL(componentId, isKnownRevoked);
    iScrSession.SetIsComponentOriginVerifiedL(componentId, isOriginVerified);
    iScrSession.SetComponentSizeL(componentId, componentSize);
    iScrSession.SetComponentNameL(componentId, componentName, locale);
    iScrSession.SetVendorNameL(componentId, vendorName, locale);	
	
	}

void CScrSetCommonPropertyStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrDeleteComponentPropertyStep-----------------
CScrDeleteComponentPropertyStep::CScrDeleteComponentPropertyStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrDeleteComponentPropertyStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrDeleteComponentPropertyStep::ImplTestStepL()
	{
	TInt componentId = GetComponentIdL();		
	RPointerArray<CPropertyEntry> propertiesToBeDeleted;
	CleanupResetAndDestroyPushL(propertiesToBeDeleted);
	GetPropertiesFromConfigL(propertiesToBeDeleted);
	
	TInt propCount = propertiesToBeDeleted.Count();
	for(TInt i=0; i<propCount; ++i)
		{
		iScrSession.DeleteComponentPropertyL(componentId, propertiesToBeDeleted[i]->PropertyName());
		}
	CleanupStack::PopAndDestroy(&propertiesToBeDeleted);
	}

void CScrDeleteComponentPropertyStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrDeleteFilePropertyStep-----------------
CScrDeleteFilePropertyStep::CScrDeleteFilePropertyStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrDeleteFilePropertyStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrDeleteFilePropertyStep::ImplTestStepL()
	{
	TInt componentId = GetComponentIdL();

	TPtrC propertyName;
	if (!GetStringFromConfig(ConfigSection(), KPropertyNameParam, propertyName))
		PrintErrorL(_L("Property was not found!"), KErrNotFound);	
	
	TPtrC fileName;
	GetFileNameFromConfigL(fileName);			

	iScrSession.DeleteFilePropertyL(componentId, fileName, propertyName);
	}

void CScrDeleteFilePropertyStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrUnregisterFileStep-----------------
CScrUnregisterFileStep::CScrUnregisterFileStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrUnregisterFileStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrUnregisterFileStep::ImplTestStepL()
	{
	TInt componentId = GetComponentIdL();
	
	TPtrC fileName;
	GetFileNameFromConfigL(fileName);			

	iScrSession.UnregisterComponentFileL(componentId, fileName);
	}

void CScrUnregisterFileStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrDeleteComponentStep-----------------
CScrDeleteComponentStep::CScrDeleteComponentStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrDeleteComponentStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrDeleteComponentStep::ImplTestStepL()
	{
	TInt componentId = GetComponentIdL();
	iScrSession.DeleteComponentL(componentId);
	}

void CScrDeleteComponentStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrDeleteAllComponentsStep-----------------
CScrDeleteAllComponentsStep::CScrDeleteAllComponentsStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrDeleteAllComponentsStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrDeleteAllComponentsStep::ImplTestStepL()
	{
	RArray<TInt> componentList;
	CleanupClosePushL(componentList);
	ReadAllSharedComponentIdsL(componentList);
	
	TInt componentCount = componentList.Count();
	for(TInt i=0; i<componentCount; ++i)
		{
		TRAPD(err,iScrSession.DeleteComponentL(componentList[i]));
		if(KErrNone == err)
			{
			INFO_PRINTF2(_L("Component Id %d has been deleted."), componentList[i]);
			}
		else if(KErrNotFound == err)
			{ // Since some components might have been deleted in the previous test cases, ignore this error.
			INFO_PRINTF2(_L("Component Id %d has already been deleted."), componentList[i]);
			}
		else
			{
			ERR_PRINTF3(_L("Component Id %d couldn't be deleted. Error Number:%d"),componentList[i],err);
			SetTestStepResult(EFail);
			}
		};
	CleanupStack::PopAndDestroy(&componentList);
	
	TBuf<MAX_SCR_BUFFER_LENGTH> componentIdBuf = KNullDesC();
	WriteSharedDataL(KComponentIdName, componentIdBuf, ESetText);
	}

void CScrDeleteAllComponentsStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrInstallComponentStep-----------------

CScrInstallComponentStep::CScrInstallComponentStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrInstallComponentStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrInstallComponentStep::ImplTestStepL()
	{
	// BEGIN transaction
	iScrSession.CreateTransactionL();
	
	// Add component record with localizable names
	RPointerArray<CLocalizableComponentInfo> localNames;
	CleanupResetAndDestroyPushL(localNames);
	CLocalizableComponentInfo *localName1 = CLocalizableComponentInfo::NewLC(_L("name1"), _L("vendor1"),ELangEnglish);
	localNames.AppendL(localName1);
	CleanupStack::Pop(localName1);
	CLocalizableComponentInfo *localName2 = CLocalizableComponentInfo::NewLC(_L("name2"), _L("vendor2"),ELangAmerican);
	localNames.AppendL(localName2);
	CleanupStack::Pop(localName2);
	TComponentId compId = iScrSession.AddComponentL(localNames, _L("plain"));
	CleanupStack::PopAndDestroy(&localNames);
	
	// Set common component attributes
	iScrSession.SetComponentVersionL(compId, _L("2.1.3"));
	iScrSession.SetComponentSizeL(compId, 2346);
	iScrSession.SetIsComponentRemovableL(compId, EFalse);
	
	// Set custom component attributes
	TBuf<10> propName;
	for(TInt i=0; i<27; ++i)
		{
		propName.Copy(_L("intprop"));
		propName.AppendNum(i);
		iScrSession.SetComponentPropertyL(compId, propName, i);
		}
	for(TInt i=0; i<6; ++i)
		{
		propName.Copy(_L("strprop"));
		propName.AppendNum(i);
		iScrSession.SetComponentPropertyL(compId, propName, propName);
		}
	
	// Register files
	TBuf<50> fileName;
	for(TInt i=0; i<10; ++i)
		{
		fileName.Copy(_L("c:\\file"));
		fileName.AppendNum(i);
		fileName.Append(_L(".txt"));
		iScrSession.RegisterComponentFileL(compId, fileName);
		}
	
	// Set file properties
	for(TInt i=0; i<6; ++i)
		{
		propName.Copy(_L("fintprop"));
		propName.AppendNum(i);
		iScrSession.SetFilePropertyL(compId, fileName, propName, i);
		}
	TBuf8<50> propValue;
	for(TInt i=0; i<3; ++i)
		{
		propName.Copy(_L("fstrprop"));
		propName.AppendNum(i);
		propValue.Copy(_L8("propValue"));
		propValue.AppendNum(i);
		iScrSession.SetFilePropertyL(compId, fileName, propName, propValue);
		}
		
	// Commit transaction
	iScrSession.CommitTransactionL();
	}
	
void CScrInstallComponentStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrAddComponentDependencyStep-----------------

CScrAddComponentDependencyStep::CScrAddComponentDependencyStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrAddComponentDependencyStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrAddComponentDependencyStep::ImplTestStepL()
	{		
	CGlobalComponentId *supplierGlobalId  = GetGlobalComponentIdLC(KSupplierGlobalIdName(), KSupplierSwTypeName());
	
	HBufC *versionFrom = GetVersionFromConfigL(KVersionFrom);
	if(versionFrom)
		CleanupStack::PushL(versionFrom);
	HBufC *versionTo = GetVersionFromConfigL(KVersionTo);
	if(versionTo)
		CleanupStack::PushL(versionTo);
	CVersionedComponentId *supplierVersionedId = CVersionedComponentId::NewL(*supplierGlobalId, versionFrom, versionTo);
	if(versionTo)
		CleanupStack::PopAndDestroy(versionTo);
	if(versionFrom)
		CleanupStack::PopAndDestroy(versionFrom);
	CleanupStack::PopAndDestroy(supplierGlobalId);
	CleanupStack::PushL(supplierVersionedId);
	
	CGlobalComponentId *dependantGlobalId = GetGlobalComponentIdLC(KDependantGlobalIdName(), KDependantSwTypeName());
	iScrSession.AddComponentDependencyL(*supplierVersionedId, *dependantGlobalId);
	CleanupStack::PopAndDestroy(2, supplierVersionedId); // supplierVersionedId, dependantGlobalId
	}


void CScrAddComponentDependencyStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrDeleteComponentDependencyStep-----------------

CScrDeleteComponentDependencyStep::CScrDeleteComponentDependencyStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrDeleteComponentDependencyStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrDeleteComponentDependencyStep::ImplTestStepL()
	{		
	CGlobalComponentId *supplierGlobalId  = GetGlobalComponentIdLC(KSupplierGlobalIdName(), KSupplierSwTypeName());
	CGlobalComponentId *dependantGlobalId = GetGlobalComponentIdLC(KDependantGlobalIdName(), KDependantSwTypeName());
	iScrSession.DeleteComponentDependencyL(*supplierGlobalId, *dependantGlobalId);
	CleanupStack::PopAndDestroy(2, supplierGlobalId); // supplierGlobalId, dependantGlobalId
	}


void CScrDeleteComponentDependencyStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}
