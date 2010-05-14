/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* tsis.cpp
* Example file/test code to demonstrate how to develop a TestExecute Server
* Developers should take this project as a template and substitute their own
* code at the __EDIT_ME__ tags
*
*/


#include <e32std.h>

#include "tsis.h"
#include "recognize.h"
#include "parsesisfile.h"
#include "verifyintegrity.h"
#include "verifysignature.h"
#include "performocsp.h"

// Tests

namespace Swi
{
namespace Sis
{
namespace Test
{

_LIT(KServerName,"tsis");

CTestParserServer* CTestParserServer::NewL()
	{
	CTestParserServer* server = new (ELeave) CTestParserServer();
	CleanupStack::PushL(server);
	User::LeaveIfError(server->iFs.Connect());

	// Make the file session sharable
	User::LeaveIfError(server->iFs.ShareAuto());

	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

/*
 * return a CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */	
CTestStep* CTestParserServer::CreateTestStep(const TDesC& aStepName)
	{
	// Create a new step depending on the name, if we run out of memory we return NULL
	// since we cannot leave.
	if (aStepName == KParseStep)
		{
		return new CParseStep(*this);
		}
	if (aStepName == KVerifySignatureStep)
		{
		return new CVerifySignatureStep(*this);		
		}				
	if (aStepName == KPerformOCSPStep)
		{
		return new CPerformOCSPStep(*this);		
		}				
	return NULL;
	}

} // namespace Swi::Sis::Test
	
} // namespace Swi::Sis

} //namespace Swi



LOCAL_C void MainL()
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
	Swi::Sis::Test::CTestParserServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = Swi::Sis::Test::CTestParserServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/*
 * return standard error code on exit
 */
GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	return KErrNone;
    }

