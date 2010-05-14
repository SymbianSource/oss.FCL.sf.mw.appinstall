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
* This file provides implementation for CTSwiConsoleServer
*
*/


#include <e32base.h>
#include <eikenv.h>

#include "tswiconsoleserver.h"
#include "tswiconsolestep.h"
#include "tswiconsoleutilitystep.h"

_LIT(KServerName,"tswiconsole");

CTSwiConsoleServer* CTSwiConsoleServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTSwiConsoleServer * server = new (ELeave) CTSwiConsoleServer();
	CleanupStack::PushL(server);
	
	server-> ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTSwiConsoleServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTSwiConsoleServer::NewL());
	if(KErrNone == err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
		
	delete server;
	delete sched;
	}

/**
 * @return - Standard Epoc error code on exit
 */
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt err = KErrNone;
	if (cleanup)
		{
		TRAP(err, MainL());
		delete cleanup;
		}

	__UHEAP_MARKEND;
	return err;
	}

CTestStep* CTSwiConsoleServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	if(aStepName == KTSwiConsoleStep)
		{
		testStep = new CTSwiConsoleStep();
		}
	else if(aStepName == KTSwiConsoleUtilityStep)
		{
		testStep = new CTSwiConsoleUtilityStep();
		}
	
	return testStep;
	}

