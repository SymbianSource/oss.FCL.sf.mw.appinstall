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
 @test
 @internalComponent
*/
#include "tststestsuiteserverrollbackall.h"
#include "tststestsuitesteprollbackall.h"

LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CStsTestServerRollbackAll* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CStsTestServerRollbackAll::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}


TInt E32Main()
/**
 @return - Standard Epoc error code on process exit
 Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}

_LIT(KServerName,"tststestserverrollbackall");

CStsTestServerRollbackAll* CStsTestServerRollbackAll::NewL()
/**
 @return - Instance of the test server
 Called inside the MainL() function to create and start the
 CTestServer derived server.
 */
	{
	CStsTestServerRollbackAll * server = new (ELeave) CStsTestServerRollbackAll();
	CleanupStack::PushL(server);
	server-> ConstructL();
	CleanupStack::Pop(server);
	return server;
	}

void CStsTestServerRollbackAll::ConstructL()
	{
	CTestServer::ConstructL(KServerName);
	}

CTestStep* CStsTestServerRollbackAll::CreateTestStep(const TDesC& aStepName)
/**
 @return - A CTestStep derived instance
 Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	if(aStepName == KRollbackAllPendingStep)
		testStep = new CRollbackAllPendingStep();
	return testStep;
	}
