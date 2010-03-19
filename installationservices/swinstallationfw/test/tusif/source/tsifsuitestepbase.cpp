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
 @internalTechnology 
*/

#include "tsifsuitestepbase.h"
#include "tsifsuitedefs.h"
#include <usif/scr/scr.h>

using namespace Usif;

TComponentId Usif::FindComponentInScrL(const TDesC& aName, const TDesC& aVendor)
	{
	RSoftwareComponentRegistry scr;
	User::LeaveIfError(scr.Connect());
	CleanupClosePushL(scr);

	RSoftwareComponentRegistryView scrView;
	CComponentFilter* filter = CComponentFilter::NewLC();
	filter->SetNameL(aName);
	filter->SetVendorL(aVendor);

	scrView.OpenViewL(scr, filter);
	CleanupClosePushL(scrView);

	CComponentEntry* component = scrView.NextComponentL();
	TComponentId componentId = 0;
	if (component != NULL)
		{
		componentId = component->ComponentId();
		delete component;
		}

	CleanupStack::PopAndDestroy(3, &scr);
	
	return componentId;
	}

CSifSuiteStepBase::~CSifSuiteStepBase()
	{
	}

CSifSuiteStepBase::CSifSuiteStepBase()
	{
	}

void CSifSuiteStepBase::ImplTestStepPreambleL()
/**
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	INFO_PRINTF1(_L("I am in doTestStepPreambleL() of the class CSifSuiteStepBase!"));

	SetTestStepResult(EPass);
	}

void CSifSuiteStepBase::ImplTestStepPostambleL()
/**
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	INFO_PRINTF1(_L("I am in doTestStepPostambleL() of the class CSifSuiteStepBase!"));
	}
