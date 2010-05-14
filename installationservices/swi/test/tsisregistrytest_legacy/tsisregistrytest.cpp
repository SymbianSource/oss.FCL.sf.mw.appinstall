/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/
#include "tsisregistrytest.h"
#include "tsisregistryteststep.h"
#include "sessionstep.h"
#include "writablesessionstep.h"

_LIT(KServerName,"tSisRegistryTest");
CTestSisRegistry* CTestSisRegistry::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTestSisRegistry * server = new (ELeave) CTestSisRegistry();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
/**
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
	CTestSisRegistry* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CTestSisRegistry::NewL());
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
 * @return - Standard Epoc error code on exit
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

CTestStep* CTestSisRegistry::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* name = NULL;
	if (aStepName == KInstalledUids)
		{
		name = new CInstalledUidsStep;
		}
	if (aStepName == KInstalledPackages)
		{
		name = new CInstalledPackagesStep;
		}
	if (aStepName == KRemovablePackages)
		{
		name = new CRemovablePackagesStep;
		}	
	if (aStepName == KInRomNonRemovablePackages)
		{
		name = new CInRomNonRemovablePackage;
		}
    if (aStepName == KSetCenRepSetting)
        {
        name = new CSetCenRepSettingStep; 
        }
	if (aStepName == KInstalledDrives)
		{
		name = new CInstalledDrives;
		}		
	else if (aStepName == KIsUidInstalled)
		{
		name = new CIsUidInstalledStep;	
		}
	else if (aStepName == KIsPackageInstalled)
		{
		name = new CIsPackageInstalledStep;
		}
	if (aStepName == KGetControllers)
		{
		name = new CGetControllersStep;	
		}
	if (aStepName == KIsControllerInstalled)
		{
		name = new CIsControllerInstalledStep;	
		}
	else if (aStepName == KEntryUidOpenClose)
		{
		name = new CEntryUidOpenCloseStep;
		}
	else if (aStepName == KEntryPackageOpenClose)
		{
		name = new CEntryPackageOpenCloseStep;
		}
	else if (aStepName == KEntryNameOpenClose)
		{
		name = new CEntryNameOpenCloseStep;
		}	
	else if (aStepName == KEntrySids)
		{
		name = new CEntrySidsStep;
		}
	else if (aStepName == KEntryMethods)
		{
		name = new CEntryMethodsStep;
		}
	else if (aStepName == KWritableEntryMethods)
		{
		name = new CWritableEntryMethodsStep;
		}
	else if (aStepName == KPoliceSids)
		{
		name = new CPoliceSidsStep;
		}		
	else if (aStepName == KEntryDelete)
		{
		name = new CEntryDeleteStep;
		}
	else if (aStepName == KBackupRegistry)
		{
		name = new CBackupRegistryStep;
		}
	else if (aStepName == KCreateTestRegistry)
		{
		name = new CCreateTestRegistryStep;
		}
	else if (aStepName == KDeleteRegistry)
		{
		name = new CDeleteRegistryStep;
		}
	else if (aStepName == KRestoreRegistry)
		{
		name = new CRestoreRegistryStep;
		}
	else if (aStepName == KSidFileName)
		{
		name = new CSidFileStep;
		}
 	else if (aStepName == KModifiableFile)
 		{
 		name = new CModifiableFileStep;
 		}
 	else if (aStepName == KFileHash)
 		{
 		name = new CFileHashStep;
 		}		
 	else if (aStepName == KSaveGeneratedRegistry)
 		{
 		name = new CSaveRegistryStep;
 		}
 	else if (aStepName == KIsSidPresentStep)
 		{
 		name = new CIsSidPresentStep;
 		}
 	else if (aStepName == KIsPackagePresentStep)
 		{
 		name = new CIsPackagePresentStep;
 		}
 	else if (aStepName == KIntegrityCheckStep)
 		{
 		name = new CIntegrityCheckStep;
 		}	
 	else if (aStepName == KLoggingFileInfo)
  		{
  		name = new CLoggingFileInfoStep;
   		}
 	else if (aStepName == KRomStubPerformanceStep)
 		{
 		name = new CRomStubPerformanceStep;
 		}	
 	else if (aStepName == KCorruptCacheStep)
 		{
 		name = new CCorruptCacheStep;
 		}
  	else if (aStepName == KCorruptCacheRecoverStep)
 		{
 		name = new CCorruptCacheRecoverStep;
 		}
	else if (aStepName == KDateAndTimeIntegrityCheckStep)
		{
		name = new CIntegrityDateAndTimeCheckStep;
		}
	else if (aStepName == KEmbeddingPackageStep)
		{
		name = new CEmbeddingPackageStep;		
		}
	else if (aStepName == KChangeLocale)
		{
		name = new CChangeLocaleStep;
		}
	else if (aStepName == KRegistryFiles)
        {
        name = new CRegistryFilesStep;
        }
   	return name;
	}
