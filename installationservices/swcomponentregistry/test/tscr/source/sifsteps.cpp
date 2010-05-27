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
* Implements the test steps for SIF-oriented APIs in the SCR
*
*/


#include "sifsteps.h"
#include <scs/cleanuputils.h>
#include "tscrdefs.h"

using namespace Usif;

// -----------CScrGetPluginStep-----------------

CScrGetPluginStep::CScrGetPluginStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrGetPluginStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}


void CScrGetPluginStep::ImplTestStepL()
	{
	TUid expectedPluginUid = GetPluginUidFromConfigL();
	TUid retPluginUid = TUid::Null();
	
	TPtrC mimeType;
	if(GetMimeTypeFromConfigL(mimeType))
		{
		retPluginUid = iScrSession.GetPluginUidL(mimeType);
		}
	else
		{
		TComponentId componentId = GetComponentIdL();
		retPluginUid = iScrSession.GetPluginUidL(componentId);
		}
	
	if(retPluginUid != expectedPluginUid)
		{
		ERR_PRINTF3(_L("Returned plugin Uid (%d) doesn't match with the expected one(%d)."), retPluginUid.iUid, expectedPluginUid.iUid);
		SetTestStepResult(EFail);
		}
	}

void CScrGetPluginStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}

TUid CScrGetPluginStep::GetPluginUidFromConfigL()
	{
	TInt pluginUid;
	if (!GetIntFromConfig(ConfigSection(), KPluginUidName, pluginUid))
			PrintErrorL(_L("Plugin UID was not found!"), KErrNotFound);	
	return TUid::Uid(pluginUid);
	}

TBool CScrGetPluginStep::GetMimeTypeFromConfigL(TPtrC& aMimeType)
	{
	return GetStringFromConfig(ConfigSection(), KMimeTypeName, aMimeType);
	}

// -----------CScrSetScomoStateStep-----------------

CScrSetScomoStateStep::CScrSetScomoStateStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrSetScomoStateStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	}

void CScrSetScomoStateStep::ImplTestStepL()
	{
	TInt componentId = GetComponentIdL();
	
	TScomoState scomoState;
	if (!GetScomoStateFromConfigL(scomoState, _L("ScomoState")))
		PrintErrorL(_L("ScomoState attribute not found"), KErrNotFound);
	
	iScrSession.SetScomoStateL(componentId, scomoState);
	}

void CScrSetScomoStateStep::ImplTestStepPostambleL()
	{
	CScrTestStep::ImplTestStepPostambleL();
	}
