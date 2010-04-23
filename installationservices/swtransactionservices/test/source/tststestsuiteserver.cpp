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
#include "tststestsuitestepcommon.h"
#include "tststestsuiteserver.h"
#include "tststestsuitestep.h"
#include "tintegrityservicesstep.h"

using namespace Usif;

LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CStsTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CStsTestServer::NewL());
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

_LIT(KServerName,"tststestserver");

CStsTestServer* CStsTestServer::NewL()
/**
 @return - Instance of the test server
 Called inside the MainL() function to create and start the
 CTestServer derived server.
 */
	{
	CStsTestServer * server = new (ELeave) CStsTestServer();
	CleanupStack::PushL(server);
	server-> ConstructL();
	CleanupStack::Pop(server);
	return server;
	}

void CStsTestServer::ConstructL()
	{
	CTestServer::ConstructL(KServerName);
	User::LeaveIfError(iTestutil.Connect());
	iTestutil.ShareAuto();
	}

CStsTestServer::~CStsTestServer()
	{
	for(TInt i=0; i<maxNumberOfSessions; ++i)
		{
		iSessions[i].Close(); //close the session in case it is open
		}
	iTestutil.Close();
	}

CTestStep* CStsTestServer::CreateTestStep(const TDesC& aStepName)
/**
 @return - A CTestStep derived instance
 Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	if(aStepName == KCreateTransactionStep)
		testStep = new CCreateTransactionStep(*this);
	else if(aStepName == KOpenTransactionStep)
		testStep = new COpenTransactionStep(*this);
	else if(aStepName == KCloseTransactionStep)
		testStep = new CCloseTransactionStep(*this);
	else if(aStepName == KRegisterNewFileStep)
		testStep = new CRegisterNewFileStep(*this);
	else if(aStepName == KCreateNewFileStep)
		testStep = new CCreateNewFileStep(*this);
	else if(aStepName == KRemoveStep)
		testStep = new CRemoveStep(*this);
	else if(aStepName == KRegisterTemporaryStep)
		testStep = new CRegisterTemporaryStep(*this);
	else if(aStepName == KCreateTemporaryStep)
		testStep = new CCreateTemporaryStep(*this);
	else if(aStepName == KOverwriteStep)
		testStep = new COverwriteStep(*this);
	else if(aStepName == KCommitStep)
		testStep = new CCommitStep(*this);
	else if(aStepName == KRollbackStep)
		testStep = new CRollbackStep(*this);
	else if(aStepName == KRollbackAllPendingStep)
		testStep = new CRollbackAllPendingStep(*this);
	else if(aStepName == KCheckFilesStep)
		testStep = new CCheckFilesStep(*this);
	else if(aStepName == KCheckFileModeChangeStep)
		testStep = new CheckFileModeChangeStep(*this);
	else if(aStepName == KFileOperationsStep)
		testStep = new CFileOperationsStep(*this);
	else if(aStepName == KFileModeTestStep)
		testStep = new CFileModeTestStep(*this);	
		
	//legacy integrity services support
	else if (aStepName == KInstall)
		testStep = new CInstallStep(*this);
	else if (aStepName == KRecover)
		testStep = new CRecoverStep(*this);
	else if (aStepName == KCleanup)
		testStep = new CCleanupStep(*this);
	else if (aStepName == KCheck)
		testStep = new CCheckStep(*this);

	//integrity services unit tests
	else if (aStepName == KJournalFileUnitTest)
		testStep = new CJournalFileUnitTest(*this);
	else if (aStepName == KJournalFileUnitTestAdd)
		testStep = new CJournalFileUnitTestAdd(*this);
	else if (aStepName == KJournalFileUnitTestTemp)
		testStep = new CJournalFileUnitTestTemp(*this);
	else if (aStepName == KJournalFileUnitTestRemove)
		testStep = new CJournalFileUnitTestRemove(*this);
	else if (aStepName == KJournalFileUnitTestRead)
		testStep = new CJournalFileUnitTestRead(*this);
	else if (aStepName == KJournalFileUnitTestEvent)
		testStep = new CJournalFileUnitTestEvent(*this);
	else if (aStepName == KJournalUnitTest)
		testStep = new CJournalUnitTest(*this);
	else if (aStepName == KJournalUnitTestInstall)
		testStep = new CJournalUnitTestInstall(*this);
	else if (aStepName == KJournalUnitTestRollback)
		testStep = new CJournalUnitTestRollback(*this);
	else if (aStepName == KIntegrityservicesUnitTestInstall)
		testStep = new CIntegrityServicesTestInstall(*this);
	else if (aStepName == KIntegrityservicesUnitTestRollback)
		testStep = new CIntegrityServicesTestRollback(*this);	
	else if (aStepName == KCreateLongFileNameTestStep)
		testStep = new CCreateLongFileNameTestStep(*this);	
	return testStep;
	}

TStsTransactionId CStsTestServer::CreateTransactionL(TInt aPos)
	{
	RStsSession& s = GetSessionL(aPos); //leaves if aPos is out of boundaries
	TStsTransactionId id(0);
	//see remark below at OpenTransactionL()
	id = s.CreateTransactionL();
	s.ShareAuto();
	return id;
	}

void CStsTestServer::OpenTransactionL(TInt aPos, TStsTransactionId aTransactionID)
	{
	RStsSession& s = GetSessionL(aPos); //leaves if aPos is out of boundaries
	/*
	 If OpenTransactionL() leaves for some reasons (eg: in one of the testcases invalid id is supplied
	 so it is expected to leave with KErrNotFound) s.ShareAuto() is not invoked, that's s is not shared
	 in the context of the process. As leaving causes the corresponding teststep's thread to be killed the
	 connection between the client and the server is destroyed by the kernel leaving s's iHandle to point to and invalid object.
	 As a result any subsequent API call on s causes panic therefore we need to clean up properly in case of leave. 
	 */
	TRAPD(err, s.OpenTransactionL(aTransactionID));
	if(err != KErrNone)
		{
		CloseTransactionL(aPos);
		User::Leave(err);
		}
	s.ShareAuto();
	}

void CStsTestServer::CloseTransactionL(TInt aPos)
	{
	GetSessionL(aPos).Close();
	}

void CStsTestServer::RegisterNewL(TInt aPos, const TDesC& aFileName)
	{
	GetSessionL(aPos).RegisterNewL(aFileName);
	}

void CStsTestServer::CreateNewL(TInt aPos, const TDesC& aFileName, RFile &aFile, TUint aFileMode)
	{
	GetSessionL(aPos).CreateNewL(aFileName, aFile, aFileMode);
	}

void CStsTestServer::RemoveL(TInt aPos, const TDesC& aFileName)
	{
	GetSessionL(aPos).RemoveL(aFileName);
	}

void CStsTestServer::RegisterTemporaryL(TInt aPos, const TDesC& aFileName)
	{
	GetSessionL(aPos).RegisterTemporaryL(aFileName);
	}

void CStsTestServer::CreateTemporaryL(TInt aPos, const TDesC& aFileName, RFile &aFile, TUint aFileMode)
	{
	GetSessionL(aPos).CreateTemporaryL(aFileName, aFile, aFileMode);
	}

void CStsTestServer::OverwriteL(TInt aPos, const TDesC& aFileName, RFile &aFile, TUint aFileMode)
	{
	GetSessionL(aPos).OverwriteL(aFileName, aFile, aFileMode);
	}

void CStsTestServer::CommitL(TInt aPos)
	{
	GetSessionL(aPos).CommitL();
	CloseTransactionL(aPos);
	}

void CStsTestServer::RollBackL(TInt aPos)
	{
	GetSessionL(aPos).RollBackL();
	CloseTransactionL(aPos);
	}

RStsSession& CStsTestServer::GetSessionL(TInt aPos)
	{
	if(aPos <0 || aPos>=maxNumberOfSessions)
		{
		ERR_PRINTF2(_L("Position %d is out of boundaries!"), aPos);
		User::Leave(KErrArgument);
		}
	return iSessions[aPos];
	}

TStsTransactionId CStsTestServer::GetTransactionIDL(TInt aPos)
	{
	return GetSessionL(aPos).TransactionIdL();
	}

