/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "runningappsteps.h"
#include "tuiscriptadaptors.h"
#include <test/testexecutelog.h>
#ifndef SWI_TEXTSHELL_ROM
	#include <apgwgnam.h>
	#include <w32std.h>
	#include <apgtask.h>
	const TInt KAppShutdownTimeout = 10000000;
#endif

// This test step shuts down an app based on UID.
// Note that it only works for gui-based apps which
// handle shutdown events.

TVerdict CShutdownApp::doTestStepL()
	{
	TInt uidValue;
	_LIT(KUidKey, "uid");
	if (!GetHexFromConfig(ConfigSection(), KUidKey, uidValue))
		{
		ERR_PRINTF1(_L("Missing uid"));
		SetTestStepResult(EFail);
		return EFail;
		}
	TInt connectErr = KErrNotFound;
#ifndef SWI_TEXTSHELL_ROM
	TUid appUid;
	appUid.iUid = uidValue;

	TInt wgId=0;
	RWsSession wsSession;
	connectErr = wsSession.Connect();
	if (connectErr != KErrNotFound)
		{
		User::LeaveIfError(connectErr);
		CleanupClosePushL(wsSession);
		
		CApaWindowGroupName* wgName = CApaWindowGroupName::NewL(wsSession);
		CleanupStack::PushL(wgName);
		CApaWindowGroupName::FindByAppUid(appUid, wsSession, wgId);
		
		if (wgId == KErrNotFound)
			{
			ERR_PRINTF2(_L("Running app with UID 0x%x not found."), appUid.iUid);		
			User::Leave(KErrNotFound);
			}
		
		wgName->ConstructFromWgIdL(wgId);
		
		if(wgName->RespondsToShutdownEvent())
			{
			TApaTask task(wsSession);
			task.SetWgId(wgId);
			RThread thread;
			TInt err=thread.Open(task.ThreadId());
			CleanupClosePushL(thread);
			if (!err)
				{
				RProcess process;
				thread.Process(process);
				CleanupClosePushL(process);
				
				TRequestStatus processStatus;
				process.Rendezvous(processStatus);
					
				task.SendSystemEvent(EApaSystemEventShutdown);

				RTimer timer;
				CleanupClosePushL(timer);
				TRequestStatus timerStatus;
				
				User::LeaveIfError(timer.CreateLocal());
				timer.After(timerStatus, KAppShutdownTimeout);

				User::WaitForRequest(processStatus,timerStatus);

				if (processStatus==KRequestPending)
					{
					// timer completed so give up
					process.RendezvousCancel(processStatus);
					User::WaitForRequest(processStatus);
					ERR_PRINTF2(_L("App with UID 0x%x timed out on shutdown."), appUid.iUid);		
					User::Leave(KErrTimedOut);
					}
				else if (timerStatus==KRequestPending)
					{
					// Rendezvous completed so cancel timer
					timer.Cancel();
					User::WaitForRequest(timerStatus);
					}
				else
					{
					 // Both have completed, so prevent stray request
					User::WaitForRequest(processStatus,timerStatus);
					}
					
				CleanupStack::PopAndDestroy(2, &process);
				INFO_PRINTF2(_L("App with UID 0x%x shut down."), appUid.iUid);
				SetTestStepResult(EPass);

				}
			CleanupStack::PopAndDestroy(&thread);
			}
		else 
			{
			ERR_PRINTF2(_L("App with UID %x does not handle shutdown events."), appUid.iUid);
			SetTestStepResult(EFail);
			}
		CleanupStack::PopAndDestroy(2, &wsSession);
	}
#endif
	if (connectErr == KErrNotFound)
		{
		// emulator tests running in textshell or in textshell ROM (#def SWI_TEXTSHELL_ROM)
		ERR_PRINTF1(_L("Ignoring shutdown checks in Textshell Rom"));
		}
	return TestStepResult();
	}

// This test step checks if there is a running process with a
// UID value specified in the ini file.

TVerdict CFindRunningExe::doTestStepL()
	{
	TInt uidValue;
	_LIT(KUidKey, "uid");
	if (!GetHexFromConfig(ConfigSection(), KUidKey, uidValue))
		{
		ERR_PRINTF1(_L("Cannot read UID from config section."));
		SetTestStepResult(EFail);
		return EFail;
		}

	TUid exeUid;
	exeUid.iUid = uidValue;

	SetTestStepResult(EFail);
	TInt loop=0;
	for(;(loop < 20)&&(TestStepResult() == EFail);loop++)
		{
		TFindProcess findProcess;
		TFullName fullName;
		while((findProcess.Next(fullName) == KErrNone) && (TestStepResult() == EFail))
			{
			RProcess process;
			TInt ret=KErrNone;
			TInt count=0;
			do
			{
			count++;
 			ret=process.Open(findProcess);
			if (ret!=KErrNone)
				{
				User::After(200000);
				}
			}while (ret!=KErrNone && count<=10);
			if (ret!=KErrNone)
				{
				User::Leave(ret);
				}
			TUidType uid = process.Type();
			TExitType exitType = process.ExitType();
			process.Close();
			if(uid[2] == exeUid)
				{
					if (exitType == EExitPending)
						SetTestStepResult(EPass);
				}
			}
			User::After(1000000);
		}

	if (TestStepResult() == EPass)
		{
		INFO_PRINTF2(_L("Process with UID 0x%x running."), exeUid.iUid);
		}
	else
		{
		ERR_PRINTF2(_L("No process with UID 0x%x running."), exeUid.iUid);		
		}

	return TestStepResult();
	}

TVerdict CEnsureSwisNotRunning::doTestStepL()
	{
	_LIT(KInstallServerName, "!InstallServer");
	TInt delaytime = 10; // wait a maximum of 10 seconds for SWI to dissapear
	while (delaytime--)
		{
		TFullName serverName;
		TFindServer find(KInstallServerName);
		if (KErrNotFound == find.Next(serverName))
			{
			SetTestStepResult(EPass);
			return EPass;
			}
		User::After(1000000); // wait a second until the next test
		}
		
	// erk, installserver still running after ten seconds!
	SetTestStepResult(EFail);
	return EFail;
	}
