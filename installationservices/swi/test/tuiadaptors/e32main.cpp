/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* E32MAIN
* Entry point for Test Console Server
*
*/


#include <e32std.h>
#include <e32base.h>
#include "consoleclientserver.h"
#include "consoleserver.h"

// Perform server initialisation, in particular creation of the scheduler and 
// server and then run the scheduler
static void RunServerL()
	{
	// Naming the server thread after the server helps to debug panics
	User::LeaveIfError(RThread().RenameMe(Swi::Test::KConsoleServer));
	// Create and install the active scheduler we need
	CActiveScheduler* s= new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	// Create the server and leave it on the cleanup stack
	Swi::Test::CConsoleServer::NewLC();
	// Init complete, signal the client
	RProcess::Rendezvous(KErrNone);
	// Ready to run
	CActiveScheduler::Start();
	// Cleanup server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

//
// Server process entry point
//

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt err=KErrNoMemory;
	if (cleanup)
		{
		TRAP(err, RunServerL());
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return err;
	}
