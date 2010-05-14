/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology 
*/

#include "tsifsuiteserver.h"
#include "tsifoperationstep.h"
#include "tsifgenericbrowserinstallstep.h"
#include "tsifcommonverificationstep.h"
#include "tsifcommonunitteststep.h"
#include "tsifnotifierteststep.h"

using namespace Usif;

_LIT(KSifServerName,"tUsifSuite");
CSifSuiteServer* CSifSuiteServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CSifSuiteServer * server = new (ELeave) CSifSuiteServer();
	CleanupStack::PushL(server);

	server->ConstructL(KSifServerName);
	CleanupStack::Pop(server);
	return server;
	}


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CSifSuiteServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CSifSuiteServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}



GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
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


CTestStep* CSifSuiteServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	if(aStepName == KSifInstallStep)
		testStep = new CSifInstallStep();
	else if(aStepName == KSifGetComponentInfoStep)
		testStep = new CSifGetComponentInfoStep();
	else if(aStepName == KSifUninstallStep)
		testStep = new CSifUninstallStep();
	else if(aStepName == KSifActivateStep)
		testStep = new CSifActivateStep(ETrue);
	else if(aStepName == KSifDeactivateStep)
		testStep = new CSifActivateStep(EFalse);
	else if(aStepName == KSifGenericBrowserInstallStep)
		testStep = new CSifGenericBrowserInstallStep();
	else if(aStepName == KSifCommonVerificationStep)
		testStep = new CSifCommonVerificationStep();
	else if(aStepName == KSifMultipleInstallStep)
		testStep = new CSifMultipleInstallStep();
	else if(aStepName == KSifCommonUnitTestStep)
		testStep = new CSifCommonUnitTestStep();
    else if(aStepName == KSifSubscribeTestStep)
        testStep = new CSifSubscribeTestStep();
	else if(aStepName == KSifPublishTestStep)
        testStep = new CSifPublishTestStep();

	return testStep;
	}
