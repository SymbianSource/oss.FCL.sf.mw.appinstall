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
*
*/


/**
 @file
 @internalComponent
*/
#include <e32base.h>
#include <f32file.h>

#include "daemon.h"
#include "daemonbehaviour.h"
#include "log.h"

namespace Swi
{

/**
 * Daemon main function
 */
void MainL()
	{
	CDaemonBehaviour* daemonBehaviour=CDaemonBehaviour::NewLC();
	CDaemon::NewLC(*daemonBehaviour);
	
	RProcess::Rendezvous(KErrNone);
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2, daemonBehaviour);	
	}

} // namespace Swi

/**
 * Entry Point, sets up the cleanup stack and calls the main function.
 * @return Always returns KErrNone even if SwiDemon has failed to start.
 * Note: The break request to fix this problem (BR 1846) was rejected by 
 * the SCB 17/5/2006 so this cannot be fixed.
 * see http://smglinx.intra/twiki/bin/view/BR/1846 for more details.
 */
GLDEF_C TInt E32Main()
	{
	DEBUG_PRINTF(_L8("SWI Daemon - Starting Daemon"));
	
	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler=new CActiveScheduler();

	if(cleanup == NULL || scheduler==NULL)
		{
		delete scheduler;
		delete cleanup;
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);


	// workaround for DEF056843 to ignore return code when this leaves 
	// unable to remove this (supposed to be temporary workaround)
	// due to BR 1846 (see above)
	TRAP_IGNORE(Swi::MainL());

	delete scheduler;
	delete cleanup;
	
	DEBUG_PRINTF(_L8("SWI Daemon - Daemon Exiting"));
	return KErrNone;
	}

