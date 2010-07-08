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
* Implements the test server of the Software Component Registry test harness
*
*/


#include "tscrserver.h" 			// TEF Test Server
#include "tscrstep.h" 				// TEF Steps Test Code
#include "componentmanagementsteps.h"
#include "componentquerysteps.h"
#include "transactionmanagementsteps.h"
#include "subsessionsteps.h"
#include "sifsteps.h"
#include "appmanagersteps.h"
#include "deletescrdbstep.h"
#include "pluginmanagementsteps.h"
#include "installloghistorysteps.h"
#include "appreginfosteps.h"

CScrTestServer* CScrTestServer::NewL()
/**
	Called inside the MainL() function to create and start the CTestServer derived server.
	@return Instance of the test server
 */
	{	
	CScrTestServer* server = new (ELeave) CScrTestServer();
	CleanupStack::PushL(server);
	User::LeaveIfError(server->iFs.Connect());
	User::LeaveIfError(server->iFs.ShareProtected());
	User::LeaveIfError(server->iFs.CreatePrivatePath(server->iFs.GetSystemDrive()));
	server->ConstructL(KScrTestServerName);
	CleanupStack::Pop(server);
	return server;
	}
	
	
CScrTestServer::~CScrTestServer()
/**
 	Destructor.
 */
	{
	iFs.Close();	
	}


LOCAL_C void MainL()
	{
	
	CActiveScheduler *scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	CScrTestServer* server = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,server = CScrTestServer::NewL());
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

TBool CScrTestServer::IsPerformanceTestStep(const TDesC& aStepName, TPtrC& aStrippedName)
	{
	_LIT(KPerformanceStepKeyWord, "Performance-");
	if (KErrNotFound == aStepName.FindF(KPerformanceStepKeyWord()))
		{
		aStrippedName.Set(aStepName);
		return EFalse;
		}
	
	TInt len = KPerformanceStepKeyWord().Length();
	aStrippedName.Set(aStepName.Mid(len));
	return ETrue;
	}

CTestStep* CScrTestServer::CreateTestStep(const TDesC& aStepName)
/**
	Creates a new test step.
	@param aStepName The name of the requested test step.
	@return A pointer to the object of the requested test step.
 */
	{
	TBool performanceStep = EFalse;
	TPtrC strippedStepName;
	performanceStep = IsPerformanceTestStep(aStepName, strippedStepName);
	
	// Initialise test step object to NULL if no TEF steps are assigned
	CTestStep* testStep = NULL;
	
	if (strippedStepName == KScrAddComponentStep)
		testStep = new CScrAddComponentStep(*this);
	else if (strippedStepName == KScrSetComponentPropertyStep)
		testStep = new CScrSetComponentPropertyStep(*this);
	else if (strippedStepName == KScrRegisterFileStep)
		testStep = new CScrRegisterFileStep(*this);
	else if (strippedStepName == KScrAddFilePropertyStep)
		testStep = new CScrAddFilePropertyStep(*this);
	else if (strippedStepName == KScrSetCommonPropertyStep)
		testStep = new CScrSetCommonPropertyStep(*this);
	else if (strippedStepName == KScrDeleteComponentPropertyStep)
		testStep = new CScrDeleteComponentPropertyStep(*this);	
	else if (strippedStepName == KScrDeleteFilePropertyStep)
		testStep = new CScrDeleteFilePropertyStep(*this);	
	else if (strippedStepName == KScrUnregisterFileStep)
		testStep = new CScrUnregisterFileStep(*this);
	else if (strippedStepName == KScrDeleteComponentStep)
		testStep = new CScrDeleteComponentStep(*this);
	else if (strippedStepName == KScrDeleteAllComponentsStep)
		testStep = new CScrDeleteAllComponentsStep(*this);
	else if (strippedStepName == KScrApplyTransactionStep)
		testStep = new CScrApplyTransactionStep(*this);
	else if (strippedStepName == KScrCreateTransactionStep)
		testStep = new CScrCreateTransactionStep(*this);
	else if (strippedStepName == KScrManageAnotherTransactionStep)
		testStep = new CScrManageAnotherTransactionStep(*this);	
	else if (strippedStepName == KScrGetComponentStep)
		testStep = new CScrGetComponentStep(*this);
	else if (strippedStepName == KScrGetComponentLocalizedStep)
	        testStep = new CScrGetComponentLocalizedStep(*this);
	else if (strippedStepName == KScrGetFilePropertiesStep)
		testStep = new CScrGetFilePropertiesStep(*this);	
	else if (strippedStepName == KScrGetFilePropertyStep)
		testStep = new CScrGetFilePropertyStep(*this);	
	else if (strippedStepName == KScrGetFileComponentsStep)
		testStep = new CScrGetFileComponentsStep(*this);
	else if (strippedStepName == KScrGetComponentPropertiesStep)
		testStep = new CScrGetComponentPropertiesStep(*this);
	else if (strippedStepName == KScrGetComponentIdsStep)
		testStep = new CScrGetComponentIdsStep(*this);
	else if (strippedStepName == KScrGetComponentPropertyStep)
		testStep = new CScrGetComponentPropertyStep(*this);
	else if (strippedStepName == KScrComponentRegistrySubsessionStep)
		testStep = new CScrComponentRegistrySubsessionStep(*this);
	else if (strippedStepName == KScrFileListSubsessionStep)
		testStep = new CScrFileListSubsessionStep(*this);
	else if (strippedStepName == KScrGetPluginStep)
		testStep = new CScrGetPluginStep(*this);
	else if (strippedStepName == KScrSetScomoStateStep)
		testStep = new CScrSetScomoStateStep(*this);	
	else if (strippedStepName == KScrIsMediaPresentStep)
		testStep = new CScrIsMediaPresentStep(*this);		
	else if (strippedStepName == KScrInstallComponentStep)
		testStep = new CScrInstallComponentStep(*this);
	else if (strippedStepName == KScrGetComponentFilesCountStep)
		testStep = new CScrGetComponentFilesCountStep(*this);
	else if (strippedStepName == KScrAddComponentDependencyStep)
		testStep = new CScrAddComponentDependencyStep(*this);
	else if (strippedStepName == KScrDeleteComponentDependencyStep)
		testStep = new CScrDeleteComponentDependencyStep(*this);
	else if (strippedStepName == KScrGetComponentWithGlobalIdStep)
		testStep = new CScrGetComponentWithGlobalIdStep(*this);
	else if (strippedStepName == KScrGetComponentIdStep)
		testStep = new CScrGetComponentIdStep(*this);
	else if (strippedStepName == KScrGetGlobalIdList)
		testStep = new CScrGetGlobalIdListStep(*this);	
	else if (strippedStepName == KScrDeleteDbFileStep)
		testStep = new CScrDeleteDbFileStep(*this);
	else if (strippedStepName == KScrAddSoftwareTypeStep)
		testStep = new CScrAddSoftwareTypeStep(*this);
	else if (strippedStepName == KScrDeleteSoftwareTypeStep)
		testStep = new CScrDeleteSoftwareTypeStep(*this);
	else if (strippedStepName == KScrIsComponentOrphaned)
		testStep = new CScrIsComponentOrphanedStep(*this);
	else if (strippedStepName == KScrCopyDbFileStep)
		testStep = new CScrCopyDbFileStep(*this);
	else if (strippedStepName == KScrGetLogEntriesStep)
		testStep = new CScrGetLogEntriesStep(*this);
	else if (strippedStepName == KScrSetLogFileReadOnlyAttrStep)
		testStep = new CScrSetLogFileReadOnlyAttrStep(*this);
	else if (strippedStepName == KScrCreateLogFileStep)
		testStep = new CScrCreateLogFileStep(*this);
	else if (strippedStepName == KScrCompareVersions)
		testStep = new CScrCompareVersionsStep(*this);
	else if (strippedStepName == KScrIsComponentOnReadOnlyDrive)
		testStep = new CScrIsComponentOnReadOnlyDriveStep(*this);
	else if (strippedStepName == KScrComponentPresence)
		testStep = new CScrComponentPresenceStep(*this);
  	else if (strippedStepName == KScrValuesNegativeStep)
  		testStep = new CScrValuesNegativeStep(*this);		
  	else if (strippedStepName == KScrOutOfMemoryStep)
  		testStep = new SCROufOfMemory(*this);
  	else if (strippedStepName == KScrAddApplicationEntry)
  	    testStep = new CScrAddApplicationEntryStep(*this);
  	else if (strippedStepName == KScrDeleteApplicationEntry)
	  	testStep = new CScrDeleteApplicationEntriesStep(*this);
	else if (strippedStepName == KSCRGetAppServicesUids)
  	    testStep = new CSCRGetAppServicesUids(*this);
	else if (strippedStepName == KSCRGetAppForDataType)
	    testStep = new CSCRGetAppForDataType(*this);	    
	else if (strippedStepName == KSCRGetAppForDataTypeAndService)
	    testStep = new CSCRGetAppForDataTypeAndService(*this);	    
	else if (strippedStepName == KSCRGetDefaultScreenNumber)
	    testStep = new CSCRGetDefaultScreenNumber(*this);
    else if (strippedStepName == KSCRGetNumberOfDefinedIcons)
        testStep = new CSCRGetNumberOfDefinedIcons(*this);	    
    else if (strippedStepName == KSCRGetApplicationLanguage)
        testStep = new CSCRGetApplicationLanguage(*this);	
    else if (strippedStepName == KScrGetAppOwnedFilesEntry)
        testStep = new CScrGetAppOwnedFilesEntryStep(*this);
    else if (strippedStepName == KScrGetAppCapabilityEntry)
        testStep = new CScrGetAppCharacteristicsEntryStep(*this);
    else if (strippedStepName == KScrGetAppIconEntry)
        testStep = new CScrGetAppIconEntryStep(*this);
    else if (strippedStepName == KScrGetAppViewIconEntry)
        testStep = new CScrGetAppViewIconEntryStep(*this);	
	else if (strippedStepName == KScrGetAppViewInfoStep)
            testStep = new CScrGetAppViewsStep(*this);
	else if (strippedStepName == KScrAppViewSubsessionStep)
  		testStep = new CAppInfoView(*this);
    else if (strippedStepName == KScrGetAppServiceInfoStep)
        testStep = new CScrGetAppServiceInfoStep(*this);
    else if (strippedStepName == KScrGetComponentIdForAppStep)
        testStep = new CScrGetComponentIdForApp(*this);
    else if (strippedStepName == KScrGetAppUidsForComponentStep)
        testStep = new CScrGetAppUidsForComponent(*this);
	else if (strippedStepName == KScrAddConcurrentApplicationEntryStep)
        testStep = new CScrAddConcurrentApplicationEntryStep(*this);	
    else if (strippedStepName == KScrGetAppInfoStep)
        testStep = new CScrGetAppInfoStep(*this);
    else if (strippedStepName == KScrComponentOnEMMC)
           testStep = new CScrEMMCComponentStep(*this);
    else if (strippedStepName == KScrComponentPresentForNameVendor)
         testStep = new CScrComponentPresentForNameVendorStep(*this);
	if(performanceStep)
		{
		CScrTestStep *scrTestStep = dynamic_cast<CScrTestStep*>(testStep);
		scrTestStep->MarkAsPerformanceStep();
		}
	
	return testStep;
	}
