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


#include <c32comm.h>

#if defined (__WINS__)
#define PDD_NAME		_L("ECDRV")
#else
#define PDD_NAME		_L("EUART1")
#define PDD2_NAME		_L("EUART2")
#define PDD3_NAME		_L("EUART3")
#define PDD4_NAME		_L("EUART4")
#endif

#define LDD_NAME		_L("ECOMM")

/**
 * @file tRevocationServer.cpp
 *
 * server implementation
 */

#include "trevocationServer.h"
#include "trevocationStep.h"

_LIT(KServerName,"tRevocation");

/**
 * Called inside the MainL() function to create and start the CTestServer 
 * derived server.
 * @return Instance of the test server
 */
CTRevocationServer* CTRevocationServer::NewL()
	{
	CTRevocationServer * server = new (ELeave) CTRevocationServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit Server Logging.

	//server->StartL(KServerName); 
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

static void InitCommsL()
    {
    TInt ret = User::LoadPhysicalDevice(PDD_NAME);
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);

#ifndef __WINS__
    ret = User::LoadPhysicalDevice(PDD2_NAME);
    ret = User::LoadPhysicalDevice(PDD3_NAME);
    ret = User::LoadPhysicalDevice(PDD4_NAME);
#endif

    ret = User::LoadLogicalDevice(LDD_NAME);
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);
    ret = StartC32();
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);
	}

LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	InitCommsL();
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	CTRevocationServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTRevocationServer::NewL());
	if(!err)
		{
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	CleanupStack::Pop(sched);
	delete server;
	delete sched;
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());	
	delete cleanup;
	return KErrNone;
    }

CTestStep* CTRevocationServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if(aStepName == KTStepInstall)
		{
		testStep = new CTStepInstall();
		}
	else if(aStepName == KTStepUninstall)
		{
		testStep = new CTStepUninstall();
		}
	else if(aStepName == KTStepBackup)
		{
		testStep = new CTStepBackup();
		}
	else if(aStepName == KTStepRestore)
		{
		testStep = new CTStepRestore();
		}
	else if(aStepName == KTStepRestoreNoCommit)
		{
		testStep = new CTStepRestoreNoCommit();
		}
	else if(aStepName == KTStepRestoreCorruptController)
		{
		testStep = new CTStepRestoreCorruptController();
		}
	else if(aStepName == KTStepRestoreExtraExecutable)
		{
		testStep = new CTStepRestoreExtraExecutable();
		}
	else if(aStepName == KTStepCheckPresent)
		{
		testStep = new CTStepCheckPresent();
		}
	else if(aStepName == KTStepCheckAbsent)
		{
		testStep = new CTStepCheckAbsent();
		}
	else if(aStepName == KTStepClean)
		{
		testStep = new CTStepClean();
		}
	else if(aStepName == KTStepPirCheck)
		{
		testStep = new CTStepPirCheck();
		}
	else if(aStepName == KTStepSisRegCheck)
		{
		testStep = new CTStepSisRegCheck();
		}
	return testStep;
	}
