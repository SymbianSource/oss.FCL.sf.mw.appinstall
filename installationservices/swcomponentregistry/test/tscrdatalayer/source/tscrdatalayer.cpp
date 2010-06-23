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
* Implements the test server of the Software Component Registry test harness
*
*/


#include "tscrdatalayerserver.h" 			// TEF Test Server
#include "datalayersteps.h"

CScrDataLayerTestServer* CScrDataLayerTestServer::NewL()
/**
	Called inside the MainL() function to create and start the CTestServer derived server.
	@return Instance of the test server
 */
	{	
	CScrDataLayerTestServer* server = new (ELeave) CScrDataLayerTestServer();
	CleanupStack::PushL(server);
	server->ConstructL(KScrDataLayerTestServerName);
	CleanupStack::Pop(server);
	return server;
	}
	
	
CScrDataLayerTestServer::~CScrDataLayerTestServer()
	{	
	}


LOCAL_C void MainL()
	{	
	CActiveScheduler *scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	CScrDataLayerTestServer* server = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,server = CScrDataLayerTestServer::NewL());
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

CTestStep* CScrDataLayerTestServer::CreateTestStep(const TDesC& aStepName)
/**
	Creates a new test step.
	@param aStepName The name of the requested test step.
	@return A pointer to the object of the requested test step.
 */
	{
	// Initialise test step object to NULL if no TEF steps are assigned
	CTestStep* testStep = NULL;
	
	if(aStepName == KScrDataLayerStep)
		testStep = new CScrTestDataLayer(*this);

	return testStep;
	}
