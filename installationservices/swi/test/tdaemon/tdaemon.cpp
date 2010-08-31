/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Example file/test code to demonstrate how to develop a TestExecute Server
* Developers should take this project as a template and substitute their own
* code at the __EDIT_ME__ tags
*
*/


#include <e32std.h>

#include "tdaemon.h"

// Tests
#include "steps/watcherstep.h"
#include "steps/daemonstep.h"


namespace Swi
{

namespace Test
{
_LIT(KServerName,"tdaemon");

CTestDaemon* CTestDaemon::NewL()
	{
	CTestDaemon * server = new (ELeave) CTestDaemon();
	CleanupStack::PushL(server);
	User::LeaveIfError(server->iFs.Connect());

	// Make the file session sharable
	User::LeaveIfError(server->iFs.ShareAuto());

	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}
	
CTestStep* CTestDaemon::CreateTestStep(const TDesC& aStepName)
/*
 * return a CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	// Create a new step depending on the name, if we run out of memory we return NULL
	// since we cannot leave.
	if (aStepName == KDriveWatcherStep)
		return new CWatcherStep();
	else if (aStepName == KDaemonStep)
		return new CDaemonStep();
	else if (aStepName == KAddDriveStep)
	    return new CAddDriveStep();
	
	return NULL;
	}

} // namespace Swi::Test

} //namespace Swi


LOCAL_C void MainL()
/*
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	// __EDIT_ME__ Your server name
	Swi::Test::CTestDaemon* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = Swi::Test::CTestDaemon::NewL());
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
/*
 * return standard error code on exit
 */
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
