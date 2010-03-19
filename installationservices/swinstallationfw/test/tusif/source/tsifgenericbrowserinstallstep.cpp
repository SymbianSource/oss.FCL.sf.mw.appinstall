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

#include "tsifgenericbrowserinstallstep.h"
#include "tsifsuitedefs.h"
#include <apgcli.h>

using namespace Usif;

CSifGenericBrowserInstallStep::~CSifGenericBrowserInstallStep()
/**
* Destructor
*/
	{
	INFO_PRINTF1(_L("Cleanup in CSifGenericBrowserInstallStep::~CSifGenericBrowserInstallStep()"));
	}

CSifGenericBrowserInstallStep::CSifGenericBrowserInstallStep()
/**
* Constructor
*/
	{
	}

void CSifGenericBrowserInstallStep::ImplTestStepPreambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
	{
	INFO_PRINTF1(_L("I am in CSifGenericBrowserInstallStep::ImplTestStepPreambleL()."));
	}


void CSifGenericBrowserInstallStep::ImplTestStepL()
/**
* @return - TVerdict code
* Override of base class pure virtual
* Our implementation only gets called if the base class doTestStepPreambleL() did
* not leave. That being the case, the current test result value will be EPass.
*/
	{
	INFO_PRINTF1(_L("I am in CSifGenericBrowserInstallStep::ImplTestStepL()."));

	TPtrC fileName;
	if(!GetStringFromConfig(ConfigSection(),KTe_PackageFile, fileName))
		{
		INFO_PRINTF1(_L("Failed to read package file name from ini file, error: KErrNotFound"));
		User::Leave(KErrNotFound);
		}
		
	TBool installByFileHandle = EFalse;
	if(!GetBoolFromConfig(ConfigSection(),KTe_OperationByFileHandle, installByFileHandle))
		{
		INFO_PRINTF1(_L("Failed to read KTe_InstallByFileHandle from ini file, error: KErrNotFound"));
		User::Leave(KErrNotFound);
		}

	RApaLsSession apa;
	User::LeaveIfError(apa.Connect());
	CleanupClosePushL(apa);

	TRequestStatus status;
	if (installByFileHandle)
		{
		RFs fs;
		RFile file;
		User::LeaveIfError(fs.Connect());
		fs.ShareProtected();
		CleanupClosePushL(fs);
		User::LeaveIfError(file.Open(fs, fileName, EFileRead | EFileShareReadersOnly));
		CleanupClosePushL(file);

		TThreadId threadId;
		TInt err = apa.StartDocument(file, threadId, &status);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Failed to call RApaLsSession::StartDocument(), error code: %d"), err);
			User::Leave(err);
			}
		User::WaitForRequest(status);
		CleanupStack::PopAndDestroy(2, &fs);
		}
	else
		{
		TThreadId threadId;
		TInt err = apa.StartDocument(fileName, threadId);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Failed to call RApaLsSession::StartDocument(), error code: %d"), err);
			User::Leave(err);
			}

		RThread thread;
		err = thread.Open(threadId);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Failed to call RThread::Open(), error code: %d"), err);
			User::Leave(err);
			}

		thread.Rendezvous(status);
		User::WaitForRequest(status);
		}

	CleanupStack::PopAndDestroy(&apa);

	if (status != iExpectedResult)
		{
		INFO_PRINTF3(_L("Expected result doesn't match: current: %d expected: %d"), status.Int(), iExpectedResult);
		SetTestStepResult(EFail);
		}
	else
		{
		SetTestStepResult(EPass);
		}
	}



void CSifGenericBrowserInstallStep::ImplTestStepPostambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
	{	
	}
