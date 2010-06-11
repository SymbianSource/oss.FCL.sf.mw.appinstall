/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* ThreadMonitor - closes started threads after a specified timeout
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include <e32std.h>
#ifndef SWI_TEXTSHELL_ROM
#include <eikenv.h>
#include <apgwgnam.h>
#include <apgtask.h>
#include <w32std.h>
#endif
#include "threadmonitor.h"
#include "log.h"

using namespace Swi;

CThreadMonitor* CThreadMonitor::NewL(TThreadId aTThreadId, RWsSession* aWsSession)
    {
	CThreadMonitor* self = CThreadMonitor::NewLC(aTThreadId, aWsSession);
    CleanupStack::Pop(self);
    return self;
    }

CThreadMonitor* CThreadMonitor::NewLC(TThreadId aTThreadId, RWsSession* aWsSession)
    {
	CThreadMonitor* self = new (ELeave) CThreadMonitor(aTThreadId, aWsSession);
	CleanupStack::PushL(self);
	self->ConstructL();
    return self;
    }

CThreadMonitor::CThreadMonitor(TThreadId aTThreadId, RWsSession* aWsSession)
				: iTThreadId(aTThreadId), iWsSession(aWsSession)
     {
     }

CThreadMonitor::~CThreadMonitor()
    {
    iThread.Close();
    }

void CThreadMonitor::ConstructL()
	{
	TInt err = iThread.Open(iTThreadId);
	if(err == KErrNone)
		{
		iThreadAlreadyClosed = EFalse;
		}
	else if (err == KErrNotFound)
		{
		iThreadAlreadyClosed = ETrue;
		}
	else
		{
		User::Leave(err);
		}
	}


void CThreadMonitor::SyncShutdownL(TInt aDelay, TBool aKillOnTimeout, TInt aApplicationShutdownTimeout)
    {
    DEBUG_PRINTF(_L8("Sis Launcher Server - Waiting for application to terminate."));
    
    if(!iThreadAlreadyClosed)
    	{
		// make the timing loop 'time' dependent (see PDEF107100)
		TTime startTime;
		startTime.UniversalTime();
		TTime currentTime = startTime;

		// If swipolicy.ini has -1 set for RunWaitTimeoutSeconds
		// aDelay will be -1000000.  In this case we wait as long
		// as it takes for the thread to exit.
		TBool noTimeout = (aDelay == -1000000) ? ETrue : EFalse;
		while ((iThread.ExitType() == EExitPending) && 
		((currentTime.MicroSecondsFrom(startTime) < aDelay) || noTimeout))
			{
			User::After(1000000); // 1 second
			currentTime.UniversalTime();
			} 

		// We may need to shut down or kill the app/process.
		if (iThread.ExitType() == EExitPending && aKillOnTimeout)
			{
			#ifdef SWI_TEXTSHELL_ROM
				(void) aApplicationShutdownTimeout;
				DEBUG_PRINTF(_L8("Sis Launcher Server - Application still running after timeout. graceful shutdown skipped."));
			#else
			// iWsSession will be NULL if running textshell in emulator.
			// Here no graceful shutdown will be tried.
			if(NULL != iWsSession)
				{
				DEBUG_PRINTF(_L8("Sis Launcher Server - Application still running after timeout. Attempting graceful shutdown."));

				// Attempt graceful shutdown first if this is an app which handles
				// shutdown events.
				CApaWindowGroupName* wgName = CApaWindowGroupName::NewL(*iWsSession);
				CleanupStack::PushL(wgName);

				TInt wgId = iWsSession->FindWindowGroupIdentifier(0, iThread.Id());
				if (wgId != KErrNotFound)
					{
					wgName->ConstructFromWgIdL(wgId);
					if(wgName->RespondsToShutdownEvent())
						{
						
						TRequestStatus threadStatus;
						iThread.Logon(threadStatus);

						TApaTask task(*iWsSession);
						task.SetWgId(wgId);
						task.SendSystemEvent(EApaSystemEventShutdown);

						RTimer timer;
						CleanupClosePushL(timer);
						TRequestStatus timerStatus;
						User::LeaveIfError(timer.CreateLocal());
						timer.After(timerStatus, aApplicationShutdownTimeout);

						User::WaitForRequest(threadStatus,timerStatus);

						if (threadStatus==KRequestPending)
							{
							// Timer completed, cancel rendezvous, process will
							// be killed below.
							iThread.LogonCancel(threadStatus);
							}
						else if (timerStatus==KRequestPending)
							{
							// Rendezvous completed so cancel timer
							timer.Cancel();
							}
						
						// Catch second request.
						User::WaitForRequest(threadStatus,timerStatus);
						CleanupStack::PopAndDestroy(&timer);
						}

					}
				CleanupStack::PopAndDestroy(wgName);
				}
			#endif
			}

		// If we haven't managed to shut it down gracefully, we need to kill
		// the process.
		if (iThread.ExitType() == EExitPending) 
			{
			DEBUG_PRINTF(_L8("Sis Launcher Server - Application still running. Killing it."));
			
			RProcess process;
			User::LeaveIfError(iThread.Process(process));
			process.Kill(KErrNone);
			process.Close();
			}
    	}
    }

