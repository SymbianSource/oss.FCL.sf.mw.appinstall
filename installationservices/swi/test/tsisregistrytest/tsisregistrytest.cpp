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

TBool IsPerformanceTestStep(const TDesC& aStepName, TPtrC& aStrippedName)
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

CTestStep* CTestSisRegistry::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	TBool performanceStep = EFalse;
	TPtrC strippedStepName;
	performanceStep = IsPerformanceTestStep(aStepName, strippedStepName);
	
	CTestStep* name = NULL;
	if (strippedStepName == KInstalledUids)
		{
		name = new CInstalledUidsStep;
		}
	if (strippedStepName == KInstalledPackages)
		{
		name = new CInstalledPackagesStep;
		}
	if (strippedStepName == KRemovablePackages)
		{
		name = new CRemovablePackagesStep;
		}	
	if (strippedStepName == KInRomNonRemovablePackages)
		{
		name = new CInRomNonRemovablePackage;
		}
    if (strippedStepName == KSetCenRepSetting)
        {
        name = new CSetCenRepSettingStep; 
        }
	if (strippedStepName == KInstalledDrives)
		{
		name = new CInstalledDrives;
		}		
	else if (strippedStepName == KIsUidInstalled)
		{
		name = new CIsUidInstalledStep;	
		}
	else if (strippedStepName == KIsPackageInstalled)
		{
		name = new CIsPackageInstalledStep;
		}
	if (strippedStepName == KGetControllers)
		{
		name = new CGetControllersStep;	
		}
	if (strippedStepName == KIsControllerInstalled)
		{
		name = new CIsControllerInstalledStep;	
		}
	else if (strippedStepName == KEntryUidOpenClose)
		{
		name = new CEntryUidOpenCloseStep;
		}
	else if (strippedStepName == KEntryPackageOpenClose)
		{
		name = new CEntryPackageOpenCloseStep;
		}
	else if (strippedStepName == KEntryNameOpenClose)
		{
		name = new CEntryNameOpenCloseStep;
		}	
	else if (strippedStepName == KEntrySids)
		{
		name = new CEntrySidsStep;
		}
	else if (strippedStepName == KEntryMethods)
		{
		name = new CEntryMethodsStep;
		}
	else if (strippedStepName == KWritableEntryMethods)
		{
		name = new CWritableEntryMethodsStep;
		}
	else if (strippedStepName == KPoliceSids)
		{
		name = new CPoliceSidsStep;
		}		
	else if (strippedStepName == KEntryDelete)
		{
		name = new CEntryDeleteStep;
		}
	else if (strippedStepName == KBackupRegistry)
		{
		name = new CBackupRegistryStep;
		}
	else if (strippedStepName == KCreateTestRegistry)
		{
		name = new CCreateTestRegistryStep;
		}
	else if (strippedStepName == KDeleteRegistry)
		{
		name = new CDeleteRegistryStep;
		}
	else if (strippedStepName == KRestoreRegistry)
		{
		name = new CRestoreRegistryStep;
		}
	else if (strippedStepName == KSidFileName)
		{
		name = new CSidFileStep;
		}
 	else if (strippedStepName == KModifiableFile)
 		{
 		name = new CModifiableFileStep;
 		}
 	else if (strippedStepName == KFileHash)
 		{
 		name = new CFileHashStep;
 		}		
 	else if (strippedStepName == KSaveGeneratedRegistry)
 		{
 		name = new CSaveRegistryStep;
 		}
 	else if (strippedStepName == KIsSidPresentStep)
 		{
 		name = new CIsSidPresentStep;
 		}
 	else if (strippedStepName == KIsPackagePresentStep)
 		{
 		name = new CIsPackagePresentStep;
 		}
 	else if (strippedStepName == KIntegrityCheckStep)
 		{
 		name = new CIntegrityCheckStep;
 		}	
 	else if (strippedStepName == KLoggingFileInfo)
  		{
  		name = new CLoggingFileInfoStep;
   		}
 	else if (strippedStepName == KRomStubPerformanceStep)
 		{
 		name = new CRomStubPerformanceStep;
 		}	
 	else if (strippedStepName == KCorruptCacheStep)
 		{
 		name = new CCorruptCacheStep;
 		}
  	else if (strippedStepName == KCorruptCacheRecoverStep)
 		{
 		name = new CCorruptCacheRecoverStep;
 		}
	else if (strippedStepName == KDateAndTimeIntegrityCheckStep)
		{
		name = new CIntegrityDateAndTimeCheckStep;
		}
	else if (strippedStepName == KEmbeddingPackageStep)
		{
		name = new CEmbeddingPackageStep;
		}
	else if (strippedStepName == KCreateTokenByCopy)
		{
		name = new CCreateTokenByCopy;
		}
	else if (strippedStepName == KCreateTokenFromStream)
		{
		name = new CCreateTokenFromStream;
		}
	else if (strippedStepName == KCertificateChainListStep)
		{
		name = new CCertificateChainListStep;
		}
	else if (strippedStepName == KSidToPackage)
		{
		name = new CSidToPackageStep;
		}
	else if (strippedStepName == KIsSidPresentSingleStep)
		{
		name = new CIsSidPresentSingleStep;
		}
	else if (strippedStepName == KAddEntryStep)
		{
		name = new CAddEntryStep;
		}
	else if (strippedStepName == KUpdateEntryStep)
		{
		name = new CUpdateEntryStep;
		}
	else if (strippedStepName == KIsPackageExistInRomStep)
		{
		name = new CPackageExistsInRomStep;
		}
	else if (strippedStepName == KFileDescriptionsStep)
		{
		name = new CFileDescriptionsStep;
		}
	else if (strippedStepName == KIsAugmentationStep)
		{
		name = new CIsAugmentationStep;
		}
	else if (strippedStepName == KFilesStep)
		{
		name = new CFilesStep;
		}
	else if (strippedStepName == KAugmentationsStep)
		{
		name = new CAugmentationsStep;
		}
	else if (strippedStepName == KPackageStep)
		{
		name = new CPackageStep;
		}
	else if (strippedStepName == KDependentPackagesStep)
		{
		name = new CDependentPackagesStep;
		}
	else if (strippedStepName == KDependenciesStep)
		{
		name = new CDependenciesStep;
		}
	else if (strippedStepName == KEmbeddedPackagesStep)
		{
		name = new CEmbeddedPackagesStep;
		}
	else if (strippedStepName == KDeleteSingleEntry)
		{
		name = new CSingleEntryDeleteStep;
		}
	else if (strippedStepName == KCreateSisRegistryObject)
		{
		name = new CCreateSisRegistryObject;
		}
	else if (strippedStepName == KIsFileRegisteredStep)
		{
		name = new CIsFileRegisteredStep;
		}	
	else if (strippedStepName == KGetComponentIdForUidStep)
		{
		name = new CGetComponentIdForUidStep;
		}	
	else if (strippedStepName == KSisRegistryDependencyStep)
		{
		name = new CSisRegistryDependencyStep;
		}
	else if (strippedStepName == KSisRegistryFileDescStep)
		{
		name = new CSisRegistryFileDescStep;
		}			
	else if (strippedStepName == KControllerInfoStep)
		{
		name = new CControllerInfoStep;
		}		
	else if (strippedStepName == KSisRegistryPropertyStep)
		{
		name = new CSisRegistryPropertyStep;
		}	
	else if (strippedStepName == KApplicationManagerStep)
		{
		name = new CSisApplicationManagerStep;
		}			
	else if (aStepName == KChangeLocale)
		{
		name = new CChangeLocaleStep;
		}
	else if (aStepName == KCAddAppRegInfoStep)
	    {
        name = new CAddAppRegInfoStep;
	    }
	else if (aStepName == KCRemoveAppRegInfoStep)
        {
        name = new CRemoveAppRegInfoStep;
        }
	else if (aStepName == KCheckAppRegData)
	    {
        name = new CheckAppRegData;
	    }
	if(performanceStep && name)
		{
		CSisRegistryTestStepBase *baseTestStep = dynamic_cast<CSisRegistryTestStepBase*>(name);
		baseTestStep->MarkAsPerformanceStep();
		}
	
   	return name;
	}
