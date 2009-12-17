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
* SisLauncher - entry point
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include <e32std.h>
#include <e32base.h>

#include "sislauncherserver.h"
#include "sislauncherclientserver.h"
#include "log.h"

using namespace Swi;

//
// Perform server initialisation, in particular creation of the scheduler and 
// server and then run the scheduler
//
static void RunServerL()
//
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
	{
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(RThread().RenameMe(KSisLauncherServerName));
	//
	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	
	DEBUG_PRINTF(_L("Sis Launcher Server - Starting Server"));
	
	//
	// create the server (leave it on the cleanup stack)
	CSisLauncherServer::NewLC();
	//
	// Initialisation complete, now signal the client

	RProcess::Rendezvous(KErrNone);

	//
	// Ready to run
	CActiveScheduler::Start();
	
	DEBUG_PRINTF(_L("Sis Launcher Server - Stopping Server"));
	
	//
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

//
// Server process entry-point
//
TInt E32Main()
	{
	__UHEAP_MARK;
	//
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	return r;
	}


void PanicClient(const RMessagePtr2& aMessage,TSisLauncherPanic aPanic)
//
// RMessagePtr2::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
//
	{
	_LIT(KPanic,"SisLauncherServer");
	aMessage.Panic(KPanic,aPanic);
	}
