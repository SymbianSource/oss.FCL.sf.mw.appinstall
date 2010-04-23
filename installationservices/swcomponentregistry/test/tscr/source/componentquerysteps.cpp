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
* Implements test steps for component query APIs in the SCR
*
*/


#include "componentquerysteps.h"
#include <scs/cleanuputils.h>
#include <usif/scr/screntries.h>
#include "tscrdefs.h"

using namespace Usif;

// -----------CScrGetComponentStep-----------------

CScrGetComponentStep::CScrGetComponentStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrGetComponentStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrGetComponentStep::ImplTestStepL()
	{
	CComponentEntry *componentExpected = GetComponentEntryFromConfigLC();	
	CComponentEntry *componentRetrieved = CComponentEntry::NewL();
	CleanupStack::PushL(componentRetrieved);
	
	TLanguage locale = KUnspecifiedLocale;
	GetLocaleFromConfigL(KComponentLocaleName, locale);
	// Start timer just before invocation of SCR API
	StartTimer();
	if(!iScrSession.GetComponentL(componentExpected->ComponentId(), *componentRetrieved, locale))
		{
		ERR_PRINTF2(_L("Component Id %d doesn't exist!."), componentExpected->ComponentId());
		User::Leave(KErrNotFound);
		}
	
	TTime time = componentRetrieved->InstallTimeL(); // For the sake of coverage print install time
	INFO_PRINTF2(_L("Component InstallTime is %ld."), componentRetrieved->InstallTimeL().Int64());
	
	if(*componentExpected != *componentRetrieved)
		{
		ERR_PRINTF5(_L("Returned component entry values(%S,%S) don't match with the expected ones(%S,%S)."),&componentRetrieved->Name(),&componentRetrieved->Vendor(),&componentExpected->Name(),&componentExpected->Vendor());
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(2, componentExpected); // componentExpected, componentRetrieved
	}

void CScrGetComponentStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrGetComponentLocalizedStep-----------------

CScrGetComponentLocalizedStep::CScrGetComponentLocalizedStep(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CScrGetComponentLocalizedStep::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CScrGetComponentLocalizedStep::ImplTestStepL()
    {
    TComponentId expectedComponentId = GetComponentIdL();
    RPointerArray<Usif::CLocalizableComponentInfo> compLocalizedInfoArrayExpected;
    CleanupResetAndDestroyPushL(compLocalizedInfoArrayExpected);
    
    GetLocalisedComponentsFromConfigL(compLocalizedInfoArrayExpected);
    
    RPointerArray<Usif::CLocalizableComponentInfo> compLocalizedInfoArrayRetrieved;
    // Start timer just before invocation of SCR API
    StartTimer();
    CleanupResetAndDestroyPushL(compLocalizedInfoArrayRetrieved);
    iScrSession.GetComponentLocalizedInfoL(expectedComponentId, compLocalizedInfoArrayRetrieved);
    if(compLocalizedInfoArrayExpected.Count()!=compLocalizedInfoArrayRetrieved.Count())
        {
        ERR_PRINTF1(_L("Returned component entry count doesnt match the expected count")); 
        SetTestStepResult(EFail);
        }
    else
        {
        for(TInt i=0;i<compLocalizedInfoArrayExpected.Count();i++)
            {
            if((compLocalizedInfoArrayExpected[i]->NameL()!=compLocalizedInfoArrayExpected[i]->NameL())||(compLocalizedInfoArrayExpected[i]->VendorL()!=compLocalizedInfoArrayExpected[i]->VendorL())||(compLocalizedInfoArrayExpected[i]->Locale()!=compLocalizedInfoArrayExpected[i]->Locale()))
                {
                ERR_PRINTF7(_L("Returned component entry values(%S,%S,%d) don't match with the expected ones(%S,%S,%d)."),&(compLocalizedInfoArrayRetrieved[i]->NameL()),&(compLocalizedInfoArrayRetrieved[i]->VendorL()),compLocalizedInfoArrayRetrieved[i]->Locale(),&(compLocalizedInfoArrayExpected[i]->NameL()),&(compLocalizedInfoArrayExpected[i]->VendorL()),compLocalizedInfoArrayExpected[i]->Locale());
                SetTestStepResult(EFail);
                }
            }
        }           
        
    CleanupStack::Pop(2,&compLocalizedInfoArrayExpected); //compLocalizedInfoArrayExpected ,compLocalizedInfoArrayRetrieved 
    compLocalizedInfoArrayRetrieved.ResetAndDestroy();
    compLocalizedInfoArrayExpected.ResetAndDestroy();
    }
    
void CScrGetComponentLocalizedStep::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }



// -----------CScrGetFilePropertiesStep-----------------

CScrGetFilePropertiesStep::CScrGetFilePropertiesStep(CScrTestServer& aParent) : CScrTestStep(aParent)
	{
	}

void CScrGetFilePropertiesStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrGetFilePropertiesStep::ImplTestStepL()
	{
	TComponentId componentId = GetComponentIdL();
	
	TPtrC fileName;
	GetFileNameFromConfigL(fileName);	
	
	RPointerArray<CPropertyEntry> expectedFileProperties;
	CleanupResetAndDestroyPushL(expectedFileProperties);	
	GetPropertiesFromConfigL(expectedFileProperties);	

	RPointerArray<CPropertyEntry> retrievedFileProperties;
	CleanupResetAndDestroyPushL(retrievedFileProperties);
	// Start timer just before invocation of SCR API
	StartTimer();
	iScrSession.GetFilePropertiesL(componentId, fileName, retrievedFileProperties);
	
	if(!ComparePropertiesL(expectedFileProperties, retrievedFileProperties))
		{
		ERR_PRINTF1(_L("Retrieved file properties don't match with the expected ones."));
		SetTestStepResult(EFail);	
		}
	CleanupStack::PopAndDestroy(2, &expectedFileProperties); // expectedFileProperties, retrievedFileProperties
	}

void CScrGetFilePropertiesStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}


// -----------CScrGetFilePropertyStep-----------------

CScrGetFilePropertyStep::CScrGetFilePropertyStep(CScrTestServer& aParent) : CScrTestStep(aParent)
	{
	}

void CScrGetFilePropertyStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrGetFilePropertyStep::ImplTestStepL()
	{
	TComponentId componentId = GetComponentIdL();
	
	CPropertyEntry *expectedProperty = GetPropertyFromConfigLC(ETrue);
	
	TPtrC fileName;
	GetFileNameFromConfigL(fileName);	
	// Start timer just before invocation of SCR API
	StartTimer();	
	CPropertyEntry *retrievedProperty = iScrSession.GetFilePropertyL(componentId, fileName, expectedProperty->PropertyName());
	
	if(!retrievedProperty)
		{
		ERR_PRINTF1(_L("The requested file property couldn't be found!"));
		User::Leave(KErrNotFound);
		}
	CleanupStack::PushL(retrievedProperty);
		
	if(*retrievedProperty != *expectedProperty)
		{
		ERR_PRINTF3(_L("The retrieved property(%S) doesn't match with the expected one(%S)."), &retrievedProperty->PropertyName(), &expectedProperty->PropertyName());
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(2, expectedProperty); // expectedProperty, retrievedProperty
	}

void CScrGetFilePropertyStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrGetFileComponentsStep-----------------

CScrGetFileComponentsStep::CScrGetFileComponentsStep(CScrTestServer& aParent) : CScrTestStep(aParent)
	{
	}

void CScrGetFileComponentsStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrGetFileComponentsStep::ImplTestStepL()
	{
	TPtrC fileName;
	GetFileNameFromConfigL(fileName);

	RArray<TComponentId> expectedComponents;
	CleanupClosePushL(expectedComponents);
	GetComponentIdsFromConfigL(expectedComponents);
	
	RArray<TComponentId> foundComponents;
	CleanupClosePushL(foundComponents);
	// Start timer just before invocation of SCR API
	StartTimer();
	iScrSession.GetComponentsL(fileName, foundComponents);
	
	CompareComponentIdsL(foundComponents, expectedComponents);
	
	CleanupStack::PopAndDestroy(2, &expectedComponents); // expectedComponents, foundComponents
	}

void CScrGetFileComponentsStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrGetComponentPropertyStep-----------------

CScrGetComponentPropertyStep::CScrGetComponentPropertyStep(CScrTestServer& aParent) : CScrTestStep(aParent)
	{
	}

void CScrGetComponentPropertyStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrGetComponentPropertyStep::ImplTestStepL()
	{
	TComponentId componentId = GetComponentIdL();
	
	CPropertyEntry *expectedProperty = GetPropertyFromConfigLC(ETrue,0,ETrue);
	CPropertyEntry *retrievedProperty(0);
	// Start timer just before invocation of SCR API
	StartTimer();
	switch(expectedProperty->PropertyType())
		{
		case CPropertyEntry::EBinaryProperty:
		case CPropertyEntry::EIntProperty:
			{
			retrievedProperty = iScrSession.GetComponentPropertyL(componentId, expectedProperty->PropertyName());
			break;
			}
		case CPropertyEntry::ELocalizedProperty:
			{
			CLocalizablePropertyEntry *locProperty = static_cast<CLocalizablePropertyEntry*>(expectedProperty);
			retrievedProperty = iScrSession.GetComponentPropertyL(componentId, locProperty->PropertyName(), locProperty->LocaleL());
			break;
			}
		}// switch
	
	if(!retrievedProperty)
		{
		ERR_PRINTF1(_L("The requested component property couldn't be found!"));
		User::Leave(KErrNotFound);
		}
	CleanupStack::PushL(retrievedProperty);
	
	if(*retrievedProperty != *expectedProperty)
		{
		ERR_PRINTF3(_L("The retrieved property(%S) doesn't match with the expected one(%S)."), &retrievedProperty->PropertyName(), &expectedProperty->PropertyName());
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(2, expectedProperty); // expectedProperty, retrievedProperty
	}

void CScrGetComponentPropertyStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}


// -----------CScrGetComponentPropertiesStep-----------------

CScrGetComponentPropertiesStep::CScrGetComponentPropertiesStep(CScrTestServer& aParent) : CScrTestStep(aParent)
	{
	}

void CScrGetComponentPropertiesStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrGetComponentPropertiesStep::ImplTestStepL()
	{
	TComponentId componentId = GetComponentIdL();

	RPointerArray<CPropertyEntry> expectedProperties;
	CleanupResetAndDestroyPushL(expectedProperties);	
	GetPropertiesFromConfigL(expectedProperties, ETrue); // ETrue means that localizable properties are supported
	
	RPointerArray<CPropertyEntry> foundProperties;
	CleanupResetAndDestroyPushL(foundProperties);
	
	TLanguage locale = static_cast<TLanguage>(0);
	// Start timer just before invocation of SCR API
	StartTimer();
	if(GetLocaleFromConfigL(locale))
		{
		iScrSession.GetComponentPropertiesL(componentId, foundProperties, locale);
		}
	else
		{
		iScrSession.GetComponentPropertiesL(componentId, foundProperties);
		}

	if (!ComparePropertiesL(foundProperties, expectedProperties))
		{
		ERR_PRINTF1(_L("The retrieved properties are different from the expected ones."));
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(2, &expectedProperties); // expectedProperties, foundProperties	
	}

void CScrGetComponentPropertiesStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrGetComponentIdsStep-----------------

CScrGetComponentIdsStep::CScrGetComponentIdsStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrGetComponentIdsStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrGetComponentIdsStep::ImplTestStepL()
	{
	CComponentFilter *componentFilter = ReadComponentFilterFromConfigLC();
	INFO_PRINTF1(_L("Read component filter from configuration"));
	
	// Get expected list of component ids from config
	RArray<TComponentId> expectedComponentIds;
	CleanupClosePushL(expectedComponentIds);
	GetComponentIdsFromConfigL(expectedComponentIds);
			
	RArray<TComponentId> foundComponentIds;
	CleanupClosePushL(foundComponentIds);
	// Start timer just before invocation of SCR API
	StartTimer();
	iScrSession.GetComponentIdsL(foundComponentIds, componentFilter);
		
	CompareComponentIdsL(foundComponentIds, expectedComponentIds);
		
	CleanupStack::PopAndDestroy(3, componentFilter); // foundComponentIds, expectedComponentIds, componentFilter
	}

void CScrGetComponentIdsStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrGetComponentFilesCountStep-----------------

CScrGetComponentFilesCountStep::CScrGetComponentFilesCountStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrGetComponentFilesCountStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrGetComponentFilesCountStep::ImplTestStepL()
	{
	TComponentId componentId = GetComponentIdL();
	TInt expectedFilesCountTmp(0);
	if (!GetIntFromConfig(ConfigSection(), _L("ExpectedFilesCount"), expectedFilesCountTmp))
		{
		PrintErrorL(_L("ExpectedFilesCount property was not found!"), KErrNotFound);
		}
	if (expectedFilesCountTmp < 0)
		{
		PrintErrorL(_L("ExpectedFilesCount is negative!"), KErrArgument);
		}
	TUint expectedFilesCount = static_cast<TUint>(expectedFilesCountTmp);
	
	// Start timer just before invocation of SCR API
	StartTimer();
	TUint foundFilesCount = iScrSession.GetComponentFilesCountL(componentId);
	
	if (foundFilesCount != expectedFilesCount)
		{
		ERR_PRINTF4(_L("The retrieved files count is different from the expected one for component %d. Expected %d, but found %d"),
				componentId, foundFilesCount, expectedFilesCount);
		SetTestStepResult(EFail);
		}
	}

void CScrGetComponentFilesCountStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}


// -----------CScrGetComponentWithGlobalIdStep-----------------

CScrGetComponentWithGlobalIdStep::CScrGetComponentWithGlobalIdStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrGetComponentWithGlobalIdStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrGetComponentWithGlobalIdStep::ImplTestStepL()
	{
	CComponentEntry *componentExpected = GetComponentEntryFromConfigLC();	
	TLanguage locale = KUnspecifiedLocale;
	GetLocaleFromConfigL(KComponentLocaleName, locale);
	
	// Start timer just before invocation of SCR API
	StartTimer();
	CComponentEntry *componentRetrieved = iScrSession.GetComponentL(componentExpected->GlobalId(), componentExpected->SoftwareType());
	if(!componentRetrieved)
		{
		INFO_PRINTF3(_L("Global Id (%S,%S) doesn't exist!."), &componentExpected->GlobalId(), &componentExpected->SoftwareType());
		User::Leave(KErrNotFound);
		}
	CleanupStack::PushL(componentRetrieved);
	
	if(*componentExpected != *componentRetrieved)
		{
		ERR_PRINTF5(_L("Returned component entry values(%S,%S) don't match with the expected ones(%S,%S)."),&componentRetrieved->Name(),&componentRetrieved->Vendor(),&componentExpected->Name(),&componentExpected->Vendor());
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(2, componentExpected); // componentExpected, componentRetrieved
	}

void CScrGetComponentWithGlobalIdStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrGetComponentIdStep-----------------

CScrGetComponentIdStep::CScrGetComponentIdStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrGetComponentIdStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrGetComponentIdStep::ImplTestStepL()
	{
	CGlobalComponentId *globalId = GetGlobalComponentIdLC();
	TComponentId expectedCompId = GetComponentIdL();
	
	// Start timer just before invocation of SCR API
	StartTimer();
	TComponentId retrievedCompId = iScrSession.GetComponentIdL(globalId->GlobalIdName(), globalId->SoftwareTypeName());
	CleanupStack::PopAndDestroy(globalId);
	
	if(expectedCompId != retrievedCompId)
		{
		ERR_PRINTF3(_L("Returned component id (%d) don't match with the expected one(%d)."), retrievedCompId, expectedCompId);
		SetTestStepResult(EFail);
		}
	}

void CScrGetComponentIdStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrGetGlobalIdListStep-----------------

CScrGetGlobalIdListStep::CScrGetGlobalIdListStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrGetGlobalIdListStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrGetGlobalIdListStep::ImplTestStepL()
	{
	CGlobalComponentId *globalId = GetGlobalComponentIdLC();
	
	TBool noVerification = EFalse;
	GetBoolFromConfig(ConfigSection(), _L("NoVerification"), noVerification);
	RPointerArray<CVersionedComponentId> expectedVerCompIdList;
	CleanupResetAndDestroyPushL(expectedVerCompIdList);
	if(!noVerification)
		{
		GetExpectedVersionedComponentIdListL(expectedVerCompIdList);
		}
	
	RPointerArray<CVersionedComponentId> retrievedVerCompIdList;
	CleanupResetAndDestroyPushL(retrievedVerCompIdList);
	
	TPtrC globalIdType;
	if (!GetStringFromConfig(ConfigSection(), KGlobalIdType, globalIdType))
		PrintErrorL(_L("Global Id Type was not found!"), KErrNotFound);

	// Start timer just before invocation of SCR API
    StartTimer();
	
	if(!globalIdType.CompareF(_L("Dependant")))
		iScrSession.GetSupplierComponentsL(*globalId, retrievedVerCompIdList);
	else if (!globalIdType.CompareF(_L("Supplier")))
		iScrSession.GetDependantComponentsL(*globalId, retrievedVerCompIdList);
	else {
		 ERR_PRINTF1(_L("The global Id type couldn't be identified."));
		 SetTestStepResult(EFail);
		 }
	
	if (!noVerification && !CompareVersionedComponentIdsL(retrievedVerCompIdList, expectedVerCompIdList))
		{
		ERR_PRINTF1(_L("The retrieved global Ids are different from the expected ones."));
		SetTestStepResult(EFail);
		}

	CleanupStack::PopAndDestroy(3, globalId); // depGlobalId, expectedGlobalIdList, retrievedGlobalIdList
	}

void CScrGetGlobalIdListStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

	
// -----------CScrValuesNegativeStep-----------------
CScrValuesNegativeStep::CScrValuesNegativeStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrValuesNegativeStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrValuesNegativeStep::ImplTestStepL()
	{
	CComponentFilter* componentFilter = CComponentFilter::NewLC();
	TDriveList driveList;
	// Set an empty drive list
	componentFilter->SetInstalledDrivesL(driveList);
	RSoftwareComponentRegistryView subSession;
	CleanupClosePushL(subSession);	
	// Start timer just before invocation of SCR API
	StartTimer();
	TRAPD(err, subSession.OpenViewL(iScrSession, componentFilter));	
	if (err != KErrArgument)
		{
		ERR_PRINTF2(_L("Received incorrect error %d on missing param in filter."), err);
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(2, componentFilter); // subSession
	}

void CScrValuesNegativeStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrOom-----------------
SCROufOfMemory::SCROufOfMemory(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	iFailRate = 0;		
	}

void SCROufOfMemory::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void SCROufOfMemory::ImplTestStepL()
	{
	TInt i = 5000;		
	while(i--)
		{    
		iScrSession.SetServerHeapFail(++iFailRate);
		TRAPD(err, ExecuteL());
		if (err != KErrNone || err != KErrNoMemory)		    
			{
			INFO_PRINTF2(_L("Error in oom Test : %d "),err);
			}	
		iScrSession.ResetServerHeapFail();		
		}
	
	}

void SCROufOfMemory::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

void SCROufOfMemory::ApplicationRegistrationOperationsL(Usif::RSoftwareComponentRegistry& aScrSession)
    {    
    TComponentId compId = 0;
    TBool newComponentAdded(EFalse);
   
    // Checking if have already inserted the component
    TBool isMultiApp(EFalse);
    GetBoolFromConfig(ConfigSection(), _L("IsMultiApp"), isMultiApp);
    if (isMultiApp)
        {
        INFO_PRINTF1(_L("Checking if the component is already present"));
        CGlobalComponentId *globalId = GetGlobalComponentIdLC();
        TRAP_IGNORE(compId = aScrSession.GetComponentIdL(globalId->GlobalIdName(), globalId->SoftwareTypeName()););
        CleanupStack::PopAndDestroy(globalId);
        }

    // Try adding the app to a component. The add operation will fail if the component is not present.
    if (!compId) 
        {
        if (GetIntFromConfig(ConfigSection(), _L("UseComponentId"), compId))
            INFO_PRINTF2(_L("Adding a new AppRegInfo to component %d"), compId);
        }
    
    // The component is not present already
    if (!compId)   
        {
        INFO_PRINTF1(_L("Adding a new component."));
        compId = AddNonLocalisableComponentL(aScrSession);
        newComponentAdded = ETrue;
        }

    INFO_PRINTF1(_L("Get reg info from config file."));
    const CApplicationRegistrationData* appRegData = GetAppRegInfoFromConfigLC();
    TRAPD(err, aScrSession.AddApplicationEntryL(compId, *appRegData));  
    
    if (err != KErrNone && err != KErrAlreadyExists )
        {
        if (newComponentAdded)
            {
            // We are going to delete the component with no apps
            aScrSession.DeleteComponentL(compId);
            }
        User::Leave(err);
        }

    CleanupStack::PopAndDestroy();
    
    //Read from DB the AppRegInfo
    CAppInfoFilter* appInfoFilter=NULL ;
    INFO_PRINTF1(_L("Read appinfo filter from configuration"));
    TRAP(err, ReadAppInfoFilterFromConfigL(&appInfoFilter));    
    if (KErrNotFound==err)
        {
        delete appInfoFilter;
        appInfoFilter=NULL;
        }   
    
    CleanupStack::PushL(appInfoFilter);
    RApplicationInfoView  subSession;
    CleanupClosePushL(subSession);
    subSession.OpenViewL(aScrSession,appInfoFilter);
    if(appInfoFilter)
        {        
        delete appInfoFilter;
        appInfoFilter = NULL;
        }
    
    RPointerArray<Usif::TAppRegInfo> appRegInfoSet;   
    do
      {
      subSession.GetNextAppInfoL(5, appRegInfoSet);       
      appRegInfoSet.ResetAndDestroy();
      }while(0 != appRegInfoSet.Count()); 
    
    CleanupStack::Pop(&subSession);
    subSession.Close();
    CleanupStack::Pop();
        
    // Delete the component
    aScrSession.DeleteComponentL(compId);
    }


void SCROufOfMemory::ExecuteL()
	{
	TBool isAppInfoOomTest(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("IsAppInfoOomTest"), isAppInfoOomTest);
	//Connect to the SCR server
    Usif::RSoftwareComponentRegistry scr;
    TInt err = scr.Connect();
    
    if (err != KErrNone)
        {
        INFO_PRINTF1(_L("Failed to connect to the SCR server"));
        User::Leave(err);
        }
    CleanupClosePushL(scr);

	INFO_PRINTF2(_L("HeapFail set for %d allocation"),iFailRate);

    if(isAppInfoOomTest)
        {
        ApplicationRegistrationOperationsL(scr);
        }
    else
        {
        // Create an SCR view
        RSoftwareComponentRegistryView scrView;
        scrView.OpenViewL(scr);
        CleanupClosePushL(scrView);
    
        // Iterate over the matching components
        CComponentEntry* entry = CComponentEntry::NewLC();
        while (scrView.NextComponentL(*entry))
            {
            RPointerArray<Usif::CLocalizableComponentInfo> compLocalizedInfoArray;
            CleanupResetAndDestroyPushL(compLocalizedInfoArray);
            scr.GetComponentLocalizedInfoL(entry->ComponentId(), compLocalizedInfoArray);
            CleanupStack::Pop(&compLocalizedInfoArray);
            compLocalizedInfoArray.ResetAndDestroy();
            }
        
        scrView.Close();
        CleanupStack::PopAndDestroy();
        CleanupStack::Pop();
        }	
    CleanupStack::PopAndDestroy();
	}

