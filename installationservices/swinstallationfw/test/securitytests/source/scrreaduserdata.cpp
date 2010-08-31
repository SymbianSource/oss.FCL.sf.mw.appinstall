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

#include "scrreaduserdata.h"
#include <scs/cleanuputils.h>
#include <usif/scr/scr.h>
#include <e32def.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "screntries_internal.h"
#endif

_LIT(KScrReadUserDataSecName, "SCR ReadUserData APIs test");

using namespace Usif;

CScrReadUserDataSecTest* CScrReadUserDataSecTest::NewL()
	{
	CScrReadUserDataSecTest* self=new(ELeave) CScrReadUserDataSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CScrReadUserDataSecTest::CScrReadUserDataSecTest()
	{
	SetCapabilityRequired(ECapabilityReadUserData);
	}
	
void CScrReadUserDataSecTest::ConstructL()
	{
	SetNameL(KScrReadUserDataSecName);
	}

void CScrReadUserDataSecTest::RunTestL()
	{
	RSoftwareComponentRegistry scrSession;
	User::LeaveIfError(scrSession.Connect());
	CleanupClosePushL(scrSession);

	TInt err(0);
	TComponentId testComponentId(1);
	// Check subsession APIs
	CComponentFilter* componentFilter = CComponentFilter::NewLC();

	RSoftwareComponentRegistryView compRegistryView;
	TRAP(err, compRegistryView.OpenViewL(scrSession, componentFilter));
	CheckFailL(err, _L("CComponentFilter::OpenViewL"));
	if (err == KErrNone)
		{
		// In a negative test, we should fail opening a sub-session. In this case we cannot continue with NextComponentL
		TRAP(err, compRegistryView.NextComponentL());
		CheckFailL(err, _L("CComponentFilter::NextComponentL"));		
		compRegistryView.Close();
		}

	RArray<TComponentId> componentIds;
	TRAP(err, scrSession.GetComponentIdsL(componentIds));
	if(KErrNone==err && componentIds.Count()!=0)
	    testComponentId=componentIds[0];       //The testComponentId retrieved, is used in the subsequent tests to get its localized information
	CleanupClosePushL(componentIds);
	CheckFailL(err, _L("RSoftwareComponentRegistry::GetComponentIdsL"));

	RPointerArray<Usif::CLocalizableComponentInfo> compLocalizedInfoArray;
	CleanupResetAndDestroyPushL(compLocalizedInfoArray);
	TRAP(err, scrSession.GetComponentLocalizedInfoL(testComponentId, compLocalizedInfoArray));
	CheckFailL(err, _L("RSoftwareComponentRegistry::GetComponentLocalizedInfoL"));
	CleanupStack::Pop(&compLocalizedInfoArray);
	compLocalizedInfoArray.ResetAndDestroy();
	
	
	RArray<TLanguage> matchingSupportedLanguages;
	CleanupClosePushL(matchingSupportedLanguages);
	TRAP(err, scrSession.GetComponentSupportedLocalesListL(testComponentId, matchingSupportedLanguages));
	CheckFailL(err, _L("RSoftwareComponentRegistry::GetComponentSupportedLocalesListL"));
    CleanupStack::PopAndDestroy(&matchingSupportedLanguages);
	
	RPointerArray<CScrLogEntry> logEntries;
	TRAP(err, scrSession.RetrieveLogEntriesL(logEntries));
	logEntries.ResetAndDestroy();
	CheckFailL(err, _L("RSoftwareComponentRegistry::RetrieveLogEntriesL"));
		
	CleanupStack::PopAndDestroy(2, componentFilter); // componentIds,  componentFilter  

	CleanupStack::PopAndDestroy(&scrSession);
	}
