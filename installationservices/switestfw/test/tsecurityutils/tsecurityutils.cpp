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


#include "tsecurityutils.h"
#include <scs/securityutils.h>

TVerdict CSecurityUtilsStep::doTestStepL()
	{
	TPtrC targetFileName;
	if (!GetStringFromConfig(ConfigSection(), _L("FileName"), targetFileName))
		{
		ERR_PRINTF1(_L("Filename was not found in test section!"));
		User::Leave(KErrNotFound);
		}
	
	TBool tcbRequired(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("ExpectedTCB"), tcbRequired);

	TBool allFilesRequired(EFalse);
	GetBoolFromConfig(ConfigSection(), _L("ExpectedAllFiles"), allFilesRequired);
	
	TInt clientSidInt(0);
	GetHexFromConfig(ConfigSection(), _L("ClientSid"), clientSidInt);

	TSecureId clientSid(clientSidInt);

	TCapabilitySet result = SecCommonUtils::FileModificationRequiredCapabilitiesL(targetFileName, clientSid);
	if (result.HasCapabilities(ECapabilityAllFiles) != allFilesRequired)
		{
		ERR_PRINTF4(_L("SecurityUtils test step failed - expected AllFiles %d, and received %d for file name %S"),
						allFilesRequired, result.HasCapabilities(ECapabilityAllFiles), &targetFileName);
		SetTestStepResult(EFail);
		}
	
	if (result.HasCapabilities(ECapabilityTCB) != tcbRequired)
		{
		ERR_PRINTF4(_L("SecurityUtils test step failed - expected TCB %d, and received %d for file name %S"),
						tcbRequired, result.HasCapabilities(ECapabilityTCB), &targetFileName);
		SetTestStepResult(EFail);
		}	

	return TestStepResult();
	}


