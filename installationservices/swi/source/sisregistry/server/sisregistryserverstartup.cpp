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
* SisRegistry server startup code implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#include "sisregistryserver.h"

#include "log.h"

using namespace Swi;

static void RunServerL()
//
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
	{
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(User::RenameThread(KSisRegistryName));
	//
	// create and install the active scheduler we need
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	//
	// create the server (leave it on the cleanup stack)
	CSisRegistryServer::NewLC();
	//
	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);
	//
	// Ready to run
	CActiveScheduler::Start();
	//
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

TInt E32Main()
//
// Server process entry-point
//
	{
	__UHEAP_MARK;
	DEBUG_PRINTF(_L8("SIS Registry Server - Starting Server"));
	//
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt err = KErrNoMemory;
	if (cleanup)
		{
		TRAP(err, RunServerL());
		delete cleanup;
		}
	//
	DEBUG_PRINTF(_L8("SIS Registry Server - Server Shutting Down"));
	__UHEAP_MARKEND;
	return err;
	}

