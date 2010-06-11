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
* appmanagersteps.cpp
*
*/


#include "pluginmanagementsteps.h"

using namespace Usif;

// -----------CScrAddSoftwareTypeStep-----------------

CScrAddSoftwareTypeStep::CScrAddSoftwareTypeStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrAddSoftwareTypeStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	User::LeaveIfError(iScrAccessor.Connect());
	}

void CScrAddSoftwareTypeStep::ImplTestStepL()
	{
	TPtrC operationType;
	if (!GetStringFromConfig(ConfigSection(), _L("OperationType"), operationType))
		PrintErrorL(_L("Operation type could not be found in configuration."), KErrNotFound);
	
	_LIT(KNonLocalizedSwTypeName, "NonLocalized");
	_LIT(KLocalizedSwTypeName, "Localized");
	_LIT(KSwTypeNoMimeType, "NoMimeType");
	_LIT(KLocalizedSwTypeDiffPluginUid, "LocalizedWithDifferentPluginUid");
	_LIT(KLocalizedSwTypeExtraName, "LocalizedWithExtraName");
	_LIT(KLocalizedSwTypeMissingName, "LocalizedWithMissingName");
	_LIT(KLocalizedSwTypeExtraMime, "LocalizedExtraMime");
	_LIT(KLocalizedSwTypeMissingMime, "LocalizedMissingMime");
	_LIT(KMultipleSidAndLauncherExecutable, "MultipleSidAndLauncherExecutable");
	
	RScrAccessor::TAccessorOperationResult opResult = RScrAccessor::EOpSucessful;
	
	if(KNonLocalizedSwTypeName() == operationType)
		{
		iScrAccessor.AddSoftwareTypeL(RScrAccessor::EAddNonLocalizedSoftwareType, opResult, iTimeMeasuredExternally);
		}
	else if(KLocalizedSwTypeName() == operationType)
		{
		iScrAccessor.AddSoftwareTypeL(RScrAccessor::EAddLocalizedSoftwareType, opResult, iTimeMeasuredExternally);
		}
	else if(KSwTypeNoMimeType() == operationType)
		{
		TRAPD(err, iScrAccessor.AddSoftwareTypeL(RScrAccessor::EAddSofwtareTypeWithoutMimeTypes, opResult, iTimeMeasuredExternally));
		if(KErrArgument != err)
			{
			ERR_PRINTF3(_L("The result of the function (%d) is not expected (%d)!"), err, KErrArgument);
			SetTestStepResult(EFail);
			}
		}
	else if(KLocalizedSwTypeDiffPluginUid() == operationType)
		{
		iScrAccessor.AddSoftwareTypeL(RScrAccessor::EAddLocalizedSoftwareTypeWithDifferentPluginUid, opResult, iTimeMeasuredExternally);
		}
	else if(KLocalizedSwTypeExtraName() == operationType)
		{
		iScrAccessor.AddSoftwareTypeL(RScrAccessor::EAddLocalizedSoftwareTypeWithExtraName, opResult, iTimeMeasuredExternally);
		}
	else if(KLocalizedSwTypeMissingName() == operationType)
		{
		iScrAccessor.AddSoftwareTypeL(RScrAccessor::EAddLocalizedSoftwareTypeWithMissingName, opResult, iTimeMeasuredExternally);
		}
	else if(KLocalizedSwTypeExtraMime() == operationType)
		{
		iScrAccessor.AddSoftwareTypeL(RScrAccessor::EAddLocalizedSoftwareTypeWithExtraMime, opResult, iTimeMeasuredExternally);
		}
	else if(KLocalizedSwTypeMissingMime() == operationType)
		{
		iScrAccessor.AddSoftwareTypeL(RScrAccessor::EAddLocalizedSoftwareTypeWithMissingMime, opResult, iTimeMeasuredExternally);
		}
	else if(KMultipleSidAndLauncherExecutable() == operationType)
	    {
	    iScrAccessor.AddSoftwareTypeL(RScrAccessor::EAddMultipleSidWithLauncherExecutable, opResult, iTimeMeasuredExternally);
	    }
	else
		{
		ERR_PRINTF2(_L("The operation type (%S) couldn't be recognised!"), &operationType);
		SetTestStepResult(EFail);
		}
	
	if(opResult != RScrAccessor::EOpSucessful)
		{
		SetTestStepResult(EFail);
		switch(opResult)
			{
			case RScrAccessor::EOpFailSwTypeNotFound:
				ERR_PRINTF1(_L("The newly added software type couldn't be found in SCR!"));
				break;
			case RScrAccessor::EOpFailUnexpectedPluginUid:
				ERR_PRINTF1(_L("The plugin Uid of the newly added software type doesn't match the expected one!"));
				break;
			default:
				ERR_PRINTF1(_L("Unknown test result received!"));
			}
		}
	}

void CScrAddSoftwareTypeStep::ImplTestStepPostambleL()
	{
	iScrAccessor.Close();
	CScrTestStep::ImplTestStepPostambleL();
	}

// -----------CScrDeleteSoftwareTypeStep-----------------

CScrDeleteSoftwareTypeStep::CScrDeleteSoftwareTypeStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrDeleteSoftwareTypeStep::ImplTestStepPreambleL()
	{
	CScrTestStep::ImplTestStepPreambleL();
	User::LeaveIfError(iScrAccessor.Connect());
	}

void CScrDeleteSoftwareTypeStep::ImplTestStepL()
	{
	RScrAccessor::TAccessorOperationResult opResult = RScrAccessor::EOpSucessful;
	
	iScrAccessor.DeleteSoftwareTypeL(opResult, iTimeMeasuredExternally);
	
	if(opResult != RScrAccessor::EOpSucessful)
		{
		SetTestStepResult(EFail);
		switch(opResult)
			{
			case RScrAccessor::EOpFailUnexpectedMimeTypeNum:
				ERR_PRINTF1(_L("The number of deleted MIME types is unexpected!"));
				break;
			case RScrAccessor::EOpFailReturnedUnexpectedMimeType:
				ERR_PRINTF1(_L("One of the returned deleted MIME type is unexpected!"));
				break;
			case RScrAccessor::EOpFailSwTypeStillExists:
				ERR_PRINTF1(_L("The deleted sofwtare type still exists in SCR!"));
				break;
			default:
				ERR_PRINTF1(_L("Unknown test result received!"));	
			}
		}
	}

void CScrDeleteSoftwareTypeStep::ImplTestStepPostambleL()
	{
	iScrAccessor.Close();
	CScrTestStep::ImplTestStepPostambleL();
	}
