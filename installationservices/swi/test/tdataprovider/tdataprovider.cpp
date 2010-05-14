/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file contains the implementation of the tdataprovider test-server for testing  CCafSisDataProvider, CDesDataProvider and CFileSisDataProvider class.
*
*/


/**
 @file tdataprovider.cpp
 @internalTechnology
*/


#include "tdataprovider.h"
#include "tdataproviderstep.h"


_LIT(KServerName,"tdataprovider");

CDataProviderServer* CDataProviderServer::NewL()
 	 {
	CDataProviderServer* server = new (ELeave) CDataProviderServer();
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

	CActiveScheduler* sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	
	CDataProviderServer* server = NULL;
	TRAPD(err,server = CDataProviderServer::NewL());
	if(!err)
		{
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



CTestStep* CDataProviderServer::CreateTestStep(const TDesC& aStepName)
	{

	CTestStep* testStep = NULL;
		
	if(aStepName == KDataProviderStep)
		{
		testStep = new CDataProviderStep();
		}

	return testStep;
	}
