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

#include "scrcomponentspecific.h"

#include <usif/scr/scr.h>
#include <usif/scr/appregentries.h>
#include <scs/cleanuputils.h>
#include <e32def.h>

_LIT(KScrCompSecName, "SCR Component-specific APIs test");

using namespace Usif;

CScrComponentSpecificSecTest* CScrComponentSpecificSecTest::NewL()
	{
	CScrComponentSpecificSecTest* self=new(ELeave) CScrComponentSpecificSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CScrComponentSpecificSecTest::CScrComponentSpecificSecTest()
	{
	SetSidRequired(TUid::Uid(0x10285BC9)); // Use tscr SID as the one required for modifying dummy components
	}
	
void CScrComponentSpecificSecTest::ConstructL()
	{
	SetNameL(KScrCompSecName);
	}

void CScrComponentSpecificSecTest::RunTestL()
	{
	RSoftwareComponentRegistry scrSession;
	User::LeaveIfError(scrSession.Connect());
	CleanupClosePushL(scrSession);

	// The transaction APIs are tested separately. If this command fails, then we can proceed, as the following operations would fail also, and no transaction management would be required
	TRAP_IGNORE(scrSession.CreateTransactionL()); 
	_LIT(KStringPropertyName, "StringName");
	
	TComponentId componentId(0);
	TRAPD(err, componentId = scrSession.AddComponentL(_L("TestComponent"), _L("TestVendor"), _L("dummy"))); // "dummy" is a reserved test software type name
	CheckFailL(err, _L("AddComponentL"));
	
	// We rely on the fact that at least one component exists in the DB, created by the script. If we failed
	// to create a legitimate component of our own (negative security test), then we attempt doing operations
	// on that component (they should fail also)
	if (err != KErrNone)
		componentId = 1; 
	
	TRAP(err, scrSession.SetComponentPropertyL(componentId, KStringPropertyName, _L8("PropertyValue")));
	CheckFailL(err, _L("SetComponentPropertyL - string"));
		
	TRAP(err, scrSession.SetComponentPropertyL(componentId, _L("IntName"), 1));
	CheckFailL(err, _L("SetComponentPropertyL - int"));	
		
	TRAP(err, scrSession.SetComponentPropertyL(componentId, _L("LocalName"), _L("PropertyValue"), ELangEnglish));
	CheckFailL(err, _L("SetComponentPropertyL - locale"));
		
	TRAP(err, scrSession.SetComponentNameL(componentId, _L("TestComponentName")));
	CheckFailL(err, _L("SetComponentNameL"));	
	
	TRAP(err, scrSession.SetVendorNameL(componentId, _L("TestComponentVendor")));
	CheckFailL(err, _L("SetVendorNameL"));	
	
	TRAP(err, scrSession.SetComponentVersionL(componentId, _L("1.0")));
	CheckFailL(err, _L("SetComponentVersionL"));
	
	TRAP(err, scrSession.SetComponentSizeL(componentId, 1));
	CheckFailL(err, _L("SetComponentSizeL"));	
	
	TRAP(err, scrSession.DeleteComponentPropertyL(componentId, KStringPropertyName));
	CheckFailL(err, _L("DeleteComponentPropertyL"));	
		
	TRAP(err, scrSession.SetScomoStateL(componentId, Usif::EActivated));
	CheckFailL(err, _L("SetScomoStateL"));	

	TRAP(err, scrSession.SetIsComponentRemovableL(componentId, EFalse));
	CheckFailL(err, _L("SetIsComponentRemovableL"));		
	
	TRAP(err, scrSession.SetIsComponentDrmProtectedL(componentId, EFalse));
	CheckFailL(err, _L("SetIsComponentDrmProtectedL"));
	
	TRAP(err, scrSession.SetIsComponentHiddenL(componentId, EFalse));
	CheckFailL(err, _L("SetIsComponentHiddenL"));
	
	TRAP(err, scrSession.SetIsComponentKnownRevokedL(componentId, EFalse));
	CheckFailL(err, _L("SetIsComponentKnownRevokedL"));
	
	TRAP(err, scrSession.SetIsComponentOriginVerifiedL(componentId, EFalse));
	CheckFailL(err, _L("SetIsComponentOriginVerifiedL"));
	
	TRAP(err, scrSession.SetIsComponentPresentL(componentId, EFalse));
	CheckFailL(err, _L("SetIsComponentPresentL"));
	
	TRAP(err, scrSession.DeleteComponentL(componentId));
	CheckFailL(err, _L("DeleteComponentL"));	
		
	RPointerArray<CLocalizableComponentInfo> localisableInfoArray;
	CleanupResetAndDestroyPushL(localisableInfoArray);
	CLocalizableComponentInfo* localisableInfo = CLocalizableComponentInfo::NewLC(_L("TestComponent"), _L("TestVendor"), static_cast<TLanguage>(1)); 
	localisableInfoArray.AppendL(localisableInfo);
	CleanupStack::Pop(localisableInfo);
		
	TRAP(err, scrSession.AddComponentL(localisableInfoArray, _L("dummy")));
	CheckFailL(err, _L("AddComponentL - localisable"));
	CleanupStack::PopAndDestroy(&localisableInfoArray);
	
	CGlobalComponentId *dependantGlobalId = CGlobalComponentId::NewLC(_L("DependantGlobalId"), _L("dummy"));
	CGlobalComponentId *supplierGlobalId = CGlobalComponentId::NewLC(_L("DependantGlobalId"), _L("plain"));
	CVersionedComponentId *supplierVerCompId = CVersionedComponentId::NewLC(*supplierGlobalId);
	TRAP(err, scrSession.AddComponentDependencyL(*supplierVerCompId, *dependantGlobalId));
	CheckFailL(err, _L("AddComponentDependencyL"));
	
	TRAP(err, scrSession.DeleteComponentDependencyL(*supplierGlobalId, *dependantGlobalId));
	CheckFailL(err, _L("DeleteComponentDependencyL"));
	CleanupStack::PopAndDestroy(3, dependantGlobalId); // dependantGlobalId, supplierGlobalId, supplierVerCompId
	
	//TODO: The addition of AppRegInfo needs to be optimized
	RPointerArray<HBufC> ownedFileArray;
    RPointerArray<Usif::CServiceInfo> serviceInfoArray;
    RPointerArray<Usif::CLocalizableAppInfo> localizableAppInfoArray;
    RPointerArray<Usif::CPropertyEntry> appPropertyArray;
	RPointerArray<Usif::COpaqueData> opaqueDataArray;
    TUid appUid = TUid::Uid(123454321);
     
    TApplicationCharacteristics appCharacteristics;
    const Usif::CApplicationRegistrationData* appRegData = Usif::CApplicationRegistrationData::NewLC(ownedFileArray, serviceInfoArray, localizableAppInfoArray, appPropertyArray, opaqueDataArray, appUid, _L("SomeFile"), appCharacteristics, 0);
    TRAP(err, scrSession.AddApplicationEntryL(componentId, *appRegData));
	CleanupStack::PopAndDestroy();
	CheckFailL(err, _L("AddApplicationEntryL"));
	
	TRAP(err, scrSession.DeleteApplicationEntriesL(componentId));
	CheckFailL(err, _L("DeleteApplicationEntryL"));

	// Verify transaction APIs - can be invoked only by an installer
	TRAP(err, scrSession.CreateTransactionL());
	CheckFailL(err, _L("CreateTransactionL"));
	
	TRAP(err, scrSession.RollbackTransactionL());
	CheckFailL(err, _L("RollbackTransactionL"));	
	
	TRAP(err, scrSession.CommitTransactionL());
	CheckFailL(err, _L("CommitTransactionL"));	
		
	CleanupStack::PopAndDestroy(&scrSession);
	}

