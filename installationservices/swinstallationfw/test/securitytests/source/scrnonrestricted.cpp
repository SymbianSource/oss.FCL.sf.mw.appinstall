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
	
	CleanupStack::PopAndDestroy(&scrSession);
	}

