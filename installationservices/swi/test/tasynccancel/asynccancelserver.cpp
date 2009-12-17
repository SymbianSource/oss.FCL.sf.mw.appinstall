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


#include "asynccancelserver.h"
#include "asynccancelstep.h"

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


CConsoleBase* gConsole;


_LIT(KServerName,"asynccancelserver");

CAsyncCancelServer* CAsyncCancelServer::NewL()
	{
	CAsyncCancelServer* server = new (ELeave) CAsyncCancelServer();
	CleanupStack::PushL(server);
	// CServer base class call
	// Name the server using the system-wide unique string
	// Clients use this to create server sessions.

    // Use ConstructL if you wish to use Server based logging else use 
	// StartL instead.
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CTestStep* CAsyncCancelServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	// Test step name contstant in the test step header file
	// Created "just in time"
	// Just one created here but create as many as required.
	if(aStepName == KAsyncCancelStep)
		{		
		testStep = new CAsyncCancelStep();
		}
	return testStep;
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
	CActiveScheduler::Install(sched);
	CAsyncCancelServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CAsyncCancelServer::NewL());
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

// End of file
