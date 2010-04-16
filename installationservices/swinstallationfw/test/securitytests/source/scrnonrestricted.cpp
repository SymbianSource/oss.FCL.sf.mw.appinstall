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
*
*/


/**
 @file
 @test
*/

#include "scrnonrestricted.h"
#include <usif/scr/appregentries.h>
#include <usif/scr/scr.h>
#include <scs/cleanuputils.h>
#include <e32def.h>

_LIT(KScrNonRestrictedSecName, "SCR non-restricted APIs test");

using namespace Usif;

CScrNonRestrictedSecTest* CScrNonRestrictedSecTest::NewL()
	{
	CScrNonRestrictedSecTest* self=new(ELeave) CScrNonRestrictedSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CScrNonRestrictedSecTest::CScrNonRestrictedSecTest()
	{
	}
	
void CScrNonRestrictedSecTest::ConstructL()
	{
	SetNameL(KScrNonRestrictedSecName);
	}

_LIT(KFileName, "c:\test.fil");
_LIT(KPropertyName, "DummyProperty");

void CScrNonRestrictedSecTest::RunTestL()
	{

	// Throughout the tests we assume that at least one component is present, i.e. that component id 1 is valid
	const TComponentId KExistingComponentId = 1;	
	
	RSoftwareComponentRegistry scrSession;
	User::LeaveIfError(scrSession.Connect());
	CleanupClosePushL(scrSession);
	
	TInt err(0);
	
	// Check component query APIs
	
	CComponentEntry* entry = CComponentEntry::NewLC();
	TRAP(err, scrSession.GetComponentL(KExistingComponentId, *entry));
	CheckFailL(err, _L("GetComponent"));
	CleanupStack::PopAndDestroy(entry);

	RArray<TComponentId> componentIds;
	TRAP(err, scrSession.GetComponentsL(KFileName, componentIds));
	CleanupClosePushL(componentIds);
	CheckFailL(err, _L("GetComponentsL"));
	CleanupStack::PopAndDestroy(&componentIds);	
	
	RPointerArray<CPropertyEntry> properties;
	CleanupResetAndDestroyPushL(properties);
	TRAP(err, scrSession.GetFilePropertiesL(KExistingComponentId, KFileName, properties));
	CheckFailL(err, _L("GetFilePropertiesL"));
	CleanupStack::PopAndDestroy(&properties);
	
	CPropertyEntry* propertyEntry(NULL);
	TRAP(err, propertyEntry = scrSession.GetFilePropertyL(KExistingComponentId, KFileName, KPropertyName));
	CleanupStack::PushL(propertyEntry);
	CheckFailL(err, _L("GetFilePropertyL"));
	CleanupStack::PopAndDestroy(propertyEntry);
	
	propertyEntry = NULL;
	TRAP(err, propertyEntry = scrSession.GetComponentPropertyL(KExistingComponentId, KPropertyName));
	CleanupStack::PushL(propertyEntry);
	CheckFailL(err, _L("GetComponentPropertyL"));
	CleanupStack::PopAndDestroy(propertyEntry);
	
	propertyEntry = NULL;
	TRAP(err, propertyEntry = scrSession.GetComponentPropertyL(KExistingComponentId, KPropertyName, ELangEnglish));
	CleanupStack::PushL(propertyEntry);
	CheckFailL(err, _L("GetComponentPropertyL - localised"));
	CleanupStack::PopAndDestroy(propertyEntry);	
	
	TRAP(err, scrSession.GetComponentPropertiesL(KExistingComponentId, properties));
	CleanupResetAndDestroyPushL(properties);
	CheckFailL(err, _L("GetFilePropertiesL"));
	CleanupStack::PopAndDestroy(&properties);
	
	TRAP(err, scrSession.GetComponentPropertiesL(KExistingComponentId, properties, ELangEnglish));
	CleanupResetAndDestroyPushL(properties);
	CheckFailL(err, _L("GetFilePropertiesL - localised"));
	CleanupStack::PopAndDestroy(&properties);
	
	TRAP(err, scrSession.GetComponentFilesCountL(KExistingComponentId));
	CheckFailL(err, _L("GetComponentFilesCountL"));
	
	entry = NULL;
	TRAP(err, entry = scrSession.GetComponentL(_L("GlobalId"), _L("dummy")));
	CheckFailL(err, _L("GetComponentL - global id"));
	if(entry)
		delete entry;
	
	TRAP(err, scrSession.GetComponentIdL(_L("GlobalId"), _L("dummy")));
	CheckFailL(err, _L("GetComponentIdL"));
	
	RPointerArray<CVersionedComponentId> globalIdList;
	CGlobalComponentId *dependantGlobalId = CGlobalComponentId::NewLC(_L("DependantGlobalId"), _L("dummy"));
	TRAP(err, scrSession.GetSupplierComponentsL(*dependantGlobalId, globalIdList));
	CheckFailL(err, _L("GetSupplierComponentsL"));
	CleanupStack::PopAndDestroy(dependantGlobalId);
	globalIdList.Reset();
	
	CGlobalComponentId *supplierGlobalId = CGlobalComponentId::NewLC(_L("SupplierGlobalId"), _L("dummy"));
	TRAP(err, scrSession.GetDependantComponentsL(*supplierGlobalId, globalIdList));
	CheckFailL(err, _L("GetDependantComponentsL"));
	CleanupStack::PopAndDestroy(supplierGlobalId);
	globalIdList.ResetAndDestroy();
	
	// Check application manager APIs
	
	TRAP(err, scrSession.IsMediaPresentL(1));
	CheckFailL(err, _L("IsMediaPresentL"));
	
	TRAP(err, scrSession.IsComponentOrphanedL(KExistingComponentId));
	CheckFailL(err, _L("IsComponentOrphanedL"));
	
	TRAP(err, scrSession.CompareVersionsL(_L("1.2"), _L("3.2")));
	CheckFailL(err, _L("CompareVersionsL"));

	TRAP(err, scrSession.IsComponentOnReadOnlyDriveL(KExistingComponentId));
	CheckFailL(err, _L("IsComponentOnReadOnlyDriveL"));
	
	TRAP(err, scrSession.IsComponentPresentL(KExistingComponentId));
	CheckFailL(err, _L("IsComponentPresentL"));
		
	// Check sub-session APIs
	RSoftwareComponentRegistryFilesList compFilesListView;
	TRAP(err, compFilesListView.OpenListL(scrSession, KExistingComponentId));
	CheckFailL(err, _L("RSoftwareComponentRegistryFilesList::OpenListL"));
	CleanupClosePushL(compFilesListView);

	TRAP(err, compFilesListView.NextFileL());
	CheckFailL(err, _L("RSoftwareComponentRegistryFilesList::NextFileL"));
	
	CleanupStack::PopAndDestroy(&compFilesListView);
	
	
	// Check sub-session APIs for InfoQueryApp		
	
	TUid appUid = TUid::Uid(305419896);
	
	HBufC* filename = NULL;
	RRegistrationInfoForApplication AppRegInfoQueryAppSubSession;
	AppRegInfoQueryAppSubSession.OpenL(scrSession,appUid);
	
	TRAP(err,AppRegInfoQueryAppSubSession.GetAppViewIconL(appUid,filename));
	CheckFailL(err, _L("RApplicationRegistrationInfoQueryApp::GetAppViewIcon"));
	if(NULL != filename)
	    delete filename;
	
	TRAP(err,AppRegInfoQueryAppSubSession.GetAppIconL(filename));
	CheckFailL(err, _L("RApplicationRegistrationInfoQueryApp::GetAppIcon"));
	if(NULL != filename)
	    delete filename;
	
	Usif::TApplicationCharacteristics appCharacteristics ;
	TRAP(err,AppRegInfoQueryAppSubSession.GetAppCharacteristicsL(appCharacteristics));
	CheckFailL(err, _L("RApplicationRegistrationInfoQueryApp::GetAppCharacteristics"));
	
	RPointerArray<HBufC> appOwnedFilesArray; 
	TRAP(err,AppRegInfoQueryAppSubSession.GetAppOwnedFilesL(appOwnedFilesArray));
	CheckFailL(err, _L("RApplicationRegistrationInfoQueryApp::GetAppOwnedFiles"));
	    
	
    RRegistrationInfoForApplication infoQuerySubSession;
    CleanupClosePushL(infoQuerySubSession); 
    
    infoQuerySubSession.OpenL(scrSession, appUid);

    RPointerArray<Usif::CAppViewData> appViewInfoArray;
    CleanupResetAndDestroyPushL(appViewInfoArray);   

    TRAP(err,infoQuerySubSession.GetAppViewsL(appViewInfoArray));
    CheckFailL(err, _L("RRegistrationInfoForApplication::GetAppViewsL"));

    appViewInfoArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy(2,&infoQuerySubSession); 

	//for RSoftwareComponentRegistryAppInfoView

	CAppInfoFilter* appinfoFilter = CAppInfoFilter::NewL();
	CleanupStack::PushL(appinfoFilter);
	
	RApplicationInfoView  appInfoViewsubSession;
	CleanupClosePushL(appInfoViewsubSession);
	TRAP(err, appInfoViewsubSession.OpenViewL(scrSession,appinfoFilter));
	CheckFailL(err, _L("RSoftwareComponentRegistryAppInfoView::OpenAppInfoViewL"));
	
	RPointerArray<Usif::TAppRegInfo> appRegInfoSet;
     
	TRAP(err, appInfoViewsubSession.GetNextAppInfoL(5, appRegInfoSet));
    CheckFailL(err, _L("RSoftwareComponentRegistryAppInfoView::GetNextAppInfoL"));
 
	TInt count = appRegInfoSet.Count();
	Usif::TAppRegInfo* tp=NULL;
	for (TInt i = 0; i<count; i++ )
	    {
	    tp=appRegInfoSet[i];
	    delete tp;
	    }
	appRegInfoSet.Close();
	CleanupStack::PopAndDestroy(2,appinfoFilter);
    
	//For GetAppServiceInfo
	RApplicationRegistrationInfo appRegInfoQuerySubSession;
    CleanupClosePushL(appRegInfoQuerySubSession); 
    appRegInfoQuerySubSession.OpenL(scrSession);
	
	RPointerArray<Usif::CServiceInfo> appServiceInfoArray;
    CleanupResetAndDestroyPushL(appServiceInfoArray);  

	CAppServiceInfoFilter* appServiceInfoFilter = CAppServiceInfoFilter::NewLC();

	TRAP(err,appRegInfoQuerySubSession.GetServiceInfoL(appServiceInfoFilter, appServiceInfoArray));
    CheckFailL(err, _L("RApplicationRegistrationInfo::GetServiceInfoL"));

    CleanupStack::PopAndDestroy(3, &appRegInfoQuerySubSession); 

	//GetComponentIdForAppL
    TUid testAppUid;
    testAppUid.iUid = 1;  
	TRAP(err, scrSession.GetComponentIdForAppL(testAppUid));
    CheckFailL(err, _L("RSoftwareComponentRegistry::GetComponentIdForAppL"));

	//GetAppUidsForComponentL
	TComponentId compId = 1;
	RArray<TUid> appUids;
	TRAP(err, scrSession.GetAppUidsForComponentL(compId, appUids));
	CheckFailL(err, _L("RSoftwareComponentRegistry::GetAppUidsForComponentL"));
	appUids.Close();
    
	CleanupStack::PopAndDestroy(&scrSession);
        
	}

