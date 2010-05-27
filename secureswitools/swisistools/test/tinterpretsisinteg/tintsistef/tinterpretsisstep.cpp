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
*
*/


/**
 @file tinterpretsisstep.cpp
*/
#include "tinterpretsisstep.h"
#include <test/testexecutelog.h>

#include "sisregistrysession.h"
#include "sisregistryentry.h"

CInterpretsisStep::~CInterpretsisStep()
	{
	}

CInterpretsisStep::CInterpretsisStep()
	{
	SetTestStepName(KInterpretsisStep);
	}

TVerdict CInterpretsisStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Inside doTestStepPreambleL()"));
	return TestStepResult();
	}

TVerdict CInterpretsisStep::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("Inside doTestStepPostambleL()"));
	return TestStepResult();
	}

TVerdict CInterpretsisStep::doTestStepL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Inside doTestStepL()"));
	TInt uid;
	GetHexFromConfig(ConfigSection(), _L("uid"), uid);
	TUid regUid = {uid};
	INFO_PRINTF1(_L("Inside doTestStep"));
	Swi::RSisRegistrySession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
	Swi::RSisRegistryEntry entry;
	User::LeaveIfError(entry.Open(session,regUid));
	CleanupClosePushL(entry); 
	TBool val=entry.RemovableL();
	if(val)
	 	{
		INFO_PRINTF1(_L("NR flag unset"));
		}
		else
		{
		INFO_PRINTF1(_L("NR flag set"));
		}
	entry.Close();
	CleanupStack::PopAndDestroy(&entry);
	CleanupStack::PopAndDestroy(&session);	
	__UHEAP_MARKEND;
	return TestStepResult();
	
	}


