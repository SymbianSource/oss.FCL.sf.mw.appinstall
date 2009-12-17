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
* Implements the test server of the Security Utils test harness
*
*/




#include "tsecurityutils.h" 				// TEF Steps Test Code
#include "tsecurityutilsserver.h"

CSecurityUtilsTestServer* CSecurityUtilsTestServer::NewL()
/**
	Called inside the MainL() function to create and start the CTestServer derived server.
	@return Instance of the test server
 */
	{	
	CSecurityUtilsTestServer* server = new (ELeave) CSecurityUtilsTestServer();
	CleanupStack::PushL(server);
	server->ConstructL(_L("tsecurityutils"));
	CleanupStack::Pop(server);
	return server;
	}
	

LOCAL_C void MainL()
	{
	
	CActiveScheduler *scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	CSecurityUtilsTestServer* server = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,server = CSecurityUtilsTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		scheduler->Start();
		}
		
	delete server;
	delete scheduler;
	}


GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Process entry point. Called by client using RProcess API
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
	

CTestStep* CSecurityUtilsTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if(aStepName == KSecurityUtilsStep)
		testStep = new CSecurityUtilsStep();

	return testStep;
	}
