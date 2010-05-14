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


#include "appmanagersteps.h"
#include "tscrdefs.h"

using namespace Usif;

// -----------CScrIsMediaPresentStep-----------------

CScrIsMediaPresentStep::CScrIsMediaPresentStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrIsMediaPresentStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrIsMediaPresentStep::ImplTestStepL()
	{
	TInt componentId = GetComponentIdL();
	TBool expectedMediaPresent(EFalse);
	if (!GetBoolFromConfig(ConfigSection(), _L("ExpectedMediaPresent"), expectedMediaPresent))
		{
		PrintErrorL(_L("ExpectedMediaPresent property not found"), KErrNotFound);
		}
	TBool foundMediaPresent = iScrSession.IsMediaPresentL(componentId);
	if (foundMediaPresent != expectedMediaPresent)
		{
		ERR_PRINTF4(_L("Mismatch for IsMediaPresentL for component id %d. Expected %d and the result was %d."), componentId, expectedMediaPresent, foundMediaPresent);
		SetTestStepResult(EFail);
		}
	}

void CScrIsMediaPresentStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrIsComponentOrphanedStep-----------------

CScrIsComponentOrphanedStep::CScrIsComponentOrphanedStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrIsComponentOrphanedStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrIsComponentOrphanedStep::ImplTestStepL()
	{
	TInt componentId = GetComponentIdL();
	TBool expectedComponentOrphaned(EFalse);
	if (!GetBoolFromConfig(ConfigSection(), _L("ExpectedComponentOrphaned"), expectedComponentOrphaned))
		{
		PrintErrorL(_L("ExpectedComponentOrphaned property not found"), KErrNotFound);
		}
	TBool foundComponentOrphaned = iScrSession.IsComponentOrphanedL(componentId);
	if (foundComponentOrphaned != expectedComponentOrphaned)
		{
		ERR_PRINTF4(_L("Mismatch for IsComponentOrphanedL for component id %d. Expected %d and the result was %d."), componentId, expectedComponentOrphaned, foundComponentOrphaned);
		SetTestStepResult(EFail);
		}
	}

void CScrIsComponentOrphanedStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrCompareVersionsStep-----------------

CScrCompareVersionsStep::CScrCompareVersionsStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrCompareVersionsStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrCompareVersionsStep::ImplTestStepL()
	{
	HBufC *lVer = GetVersionFromConfigL(KVersionLeft());
	CleanupStack::PushL(lVer);
	
	HBufC *rVer = GetVersionFromConfigL(KVersionRight());
	CleanupStack::PushL(rVer);
	
	if(!lVer || !rVer)
		PrintErrorL(_L("Missing version!"), KErrNotFound);
	
	TInt expectedResult;
	if (!GetIntFromConfig(ConfigSection(), KExpectedResult, expectedResult))
		PrintErrorL(_L("The expected result param could not be found in configuration."), KErrNotFound);
	
	TInt retrievedResult = iScrSession.CompareVersionsL(*lVer, *rVer);
	
	if(retrievedResult > 0)
		retrievedResult = 1;
	else if(retrievedResult < 0)
		retrievedResult = -1;
	
	if(retrievedResult != expectedResult)
		PrintErrorL(_L("The retrieved result (%d) is different from the expected one (%d)."), KErrNotFound, retrievedResult, expectedResult);
	
	CleanupStack::PopAndDestroy(2, lVer); // lVer, rVer
	}

void CScrCompareVersionsStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrIsComponentOnReadOnlyDriveStep-----------------

CScrIsComponentOnReadOnlyDriveStep::CScrIsComponentOnReadOnlyDriveStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrIsComponentOnReadOnlyDriveStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrIsComponentOnReadOnlyDriveStep::ImplTestStepL()
	{
	TInt componentId = GetComponentIdL();
	TBool expectedROMResult(EFalse);
	if (!GetBoolFromConfig(ConfigSection(), _L("ExpectedROMResult"), expectedROMResult))
		{
		PrintErrorL(_L("ExpectedROMResult property not found"), KErrNotFound);
		}
	
	TBool result = iScrSession.IsComponentOnReadOnlyDriveL(componentId);
	if (result != expectedROMResult)
		{
		ERR_PRINTF4(_L("Mismatch for IsComponentOnReadOnlyDriveL for component id %d. Expected %d and the result was %d."), componentId, expectedROMResult, result);
		SetTestStepResult(EFail);
		}
	}

void CScrIsComponentOnReadOnlyDriveStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrComponentPresenceStep-----------------

CScrComponentPresenceStep::CScrComponentPresenceStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrComponentPresenceStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrComponentPresenceStep::ImplTestStepL()
	{
	TInt componentId = GetComponentIdL();
	
	TBool compPresenceValue(ETrue);
	if (!GetBoolFromConfig(ConfigSection(), _L("CompPresenceValue"), compPresenceValue))
		{
		PrintErrorL(_L("CompPresenceValue property not found"), KErrNotFound);
		}
		
	TBool setCompPresence(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("SetCompPresence"), setCompPresence);
	if (setCompPresence)
		{		
		iScrSession.SetIsComponentPresentL(componentId, compPresenceValue);
		}
	
	TBool result = iScrSession.IsComponentPresentL(componentId);
	if (result != compPresenceValue)
		{
		ERR_PRINTF4(_L("Mismatch for IsComponentPresentL for component id %d. Expected %d and the result was %d."), componentId, compPresenceValue, result);
		SetTestStepResult(EFail);
		}
	}

void CScrComponentPresenceStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}
