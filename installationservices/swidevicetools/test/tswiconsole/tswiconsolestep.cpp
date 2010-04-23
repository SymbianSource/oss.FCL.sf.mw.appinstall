/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file provides implementation for CTSwiConsoleStep
*
*/


#include <test/testexecutelog.h>
#include "tswiconsolestep.h"

_LIT(KOption, "option");
_LIT(KExpectedResult, "ExpectedResult");
_LIT(KSwiConsole, "z:\\sys\\bin\\swiconsole.exe");

CTSwiConsoleStep::CTSwiConsoleStep()
	{
	SetTestStepName(KTSwiConsoleStep);
	}

TVerdict CTSwiConsoleStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	TPtrC appOption;
	if(!GetStringFromConfig(ConfigSection(),KOption, appOption))
		{
		ERR_PRINTF1(_L("Missing Option"));
		SetTestStepError(KErrBadName);
		return EFail;
		}
		
	TInt expectedResult = KErrNone;
	if(!GetIntFromConfig(ConfigSection(),KExpectedResult, expectedResult))
		{
		ERR_PRINTF1(_L("Missing Expected result"));
		SetTestStepError(KErrBadName);
		return EFail;
		}
	
	RProcess process;
	TInt err = process.Create(KSwiConsole, appOption);
	process.Resume();
	
	TRequestStatus status;
	process.Logon(status);
	User::WaitForRequest(status);
	
	TInt exitReason = process.ExitReason();
	
	INFO_PRINTF3(_L("Returned Result %d is compare with expected Result %d"), &exitReason,&expectedResult);
	
	if(exitReason == expectedResult)
		{
		SetTestStepResult(EPass);
		}
	else 
		{
		SetTestStepError(exitReason);
		SetTestStepResult(EFail);
		}
	
	process.Close();
	
	
	return TestStepResult();
	}
