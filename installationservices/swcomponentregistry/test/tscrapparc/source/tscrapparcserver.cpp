/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tscrapparcserver.h"             // TEF Test Server
#include "appreginfoapparcsteps.h"


CScrApparcTestServer* CScrApparcTestServer::NewL()
/**
    Called inside the MainL() function to create and start the CTestServer derived server.
    @return Instance of the test server
 */
    {   
    CScrApparcTestServer* server = new (ELeave) CScrApparcTestServer();
    CleanupStack::PushL(server);    
    server->ConstructL(KScrApparcTestServerName);
    CleanupStack::Pop(server);
    return server;
    }
    
    
CScrApparcTestServer::~CScrApparcTestServer()
/**
    Destructor.
 */
    {
    
    }


LOCAL_C void MainL()
    {
    CActiveScheduler *scheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(scheduler);

    CScrApparcTestServer* server = NULL;
    
    //Create the CScrApparcTestServer derived server
    TRAPD(err,server = CScrApparcTestServer::NewL());
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

CTestStep* CScrApparcTestServer::CreateTestStep(const TDesC& aStepName)    
    {    
    CTestStep* testStep = NULL;
    
    if (aStepName == KScrApplicationRegistrationViewSubsessionStep)
        testStep = new CScrGetApplicationRegistrationViewSubsessionStep();
           
    if (aStepName == KScrMultipleSubsessionsForAppRegistryViewStep)
        testStep = new CScrMultipleSubsessionsForAppRegistryViewStep();
    
    return testStep;
    }

