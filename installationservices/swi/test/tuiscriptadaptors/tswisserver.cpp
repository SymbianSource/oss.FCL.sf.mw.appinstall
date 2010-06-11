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
 * @file
 *
 * SWIS test server implementation
 */

#include "tswisserver.h"
#include "tswisstep.h"
#include "tswisasyncstep.h"
#include "forceremove.h"
#include "filldrive.h"
#include "runningappsteps.h"
#include "swifileutilitysteps.h"

_LIT(KServerName, "tuiscriptadaptors");

/**
 * Called inside the MainL() function to create and start the CTestServer 
 * derived server.
 * @return Instance of the test server
 */
CSwisTestServer* CSwisTestServer::NewL()
	{
	CSwisTestServer *server = new(ELeave) CSwisTestServer();
	CleanupStack::PushL(server);
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
	CActiveScheduler::Install(sched);
	CSwisTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CSwisTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/**
 * Server entry point
 * @return Standard Epoc error code on exit
 */
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
 * Implementation of CTestServer pure virtual
 * @return A CTestStep derived instance
 */
CTestStep* CSwisTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if (aStepName == KSwisInstallStep)	// Install with file name
		testStep = new CSwisInstallStep(CSwisInstallStep::EUseFileName);
	else if (aStepName == KSwisInstallPerformanceStep)
	    testStep = new CSwisInstallStep(CSwisInstallStep::ECheckInstallPerformance);
	else if (aStepName == KSwisInstallFHStep)	// Install with file handle
		testStep = new CSwisInstallStep(CSwisInstallStep::EUseFileHandle);
	else if (aStepName == KSwisInstallMemStep)	// Install with CDesDataProvider
		testStep = new CSwisInstallStep(CSwisInstallStep::EUseMemory); 
		// The EUseMemory Install Step is obsolete (24/02/2005) Substituted by the new Capabilities Tests
	else if (aStepName == KSwisInstallCAFStep)	// Install with CCafDataProvider
		testStep = new CSwisInstallStep(CSwisInstallStep::EUseCAF);
	else if (aStepName == KSwisInstallOpenFileStep)	// Install with previously opened file
		testStep = new CSwisInstallStep(CSwisInstallStep::EUseOpenFileName);
	else if (aStepName == KSwisCheckedInstallStep) // Install with file name, but don't automatically expect sucess
		testStep = new CSwisInstallStep(CSwisInstallStep::ECheckExitValue);
	// uninstall steps
// Install, Cancel testing	
	else if (aStepName == KSwisInstallCancelStep)	// Install with file name
		testStep = new CSwisInstallStep(CSwisInstallStep::EUseFileName, ETrue);
	else if (aStepName == KSwisInstallFHCancelStep)	// Install with file handle
		testStep = new CSwisInstallStep(CSwisInstallStep::EUseFileHandle, ETrue);
	else if (aStepName == KSwisInstallMemCancelStep)	// Install with CDesDataProvider
		testStep = new CSwisInstallStep(CSwisInstallStep::EUseMemory, ETrue);
	else if (aStepName == KSwisInstallCAFCancelStep)	// Install with CCafDataProvider
		testStep = new CSwisInstallStep(CSwisInstallStep::EUseCAF, ETrue);
// Uninstall
	else if (aStepName == KSwisUninstallStep)
		testStep = new CSwisUninstallStep(CSwisUninstallStep::EByUid);
	else if (aStepName == KSwisUninstallAugStep)
		testStep = new CSwisUninstallStep(CSwisUninstallStep::EByPackage);
// Uninstall, Cancel testing
	else if (aStepName == KSwisUninstallCancelStep)
		testStep = new CSwisUninstallStep(CSwisUninstallStep::EByUid, ETrue);
	else if (aStepName == KSwisUninstallAugCancelStep)
		testStep = new CSwisUninstallStep(CSwisUninstallStep::EByPackage, ETrue);
// Async Install/Uninstall	
	else if (aStepName == KSwisInstallAsyncStep) 
			testStep = new CSwisAsyncInstallStep(CSwisAsyncInstallStep::EUseFileName);
	else if (aStepName == KSwisInstallAsyncFHStep) 
			testStep = new CSwisAsyncInstallStep(CSwisAsyncInstallStep::EUseFileHandle);
	else if (aStepName == KSwisInstallAsyncCAFStep) 
			testStep = new CSwisAsyncInstallStep(CSwisAsyncInstallStep::EUseCAF);
	else if (aStepName == KSwisInstallAsyncCancelStep) 
			testStep = new CSwisAsyncInstallStep(CSwisAsyncInstallStep::EUseFileName, Swi::Test::ECancelTimerFromStart);
	else if (aStepName == KSwisInstallAsyncCancelAfterDialogsStep) 
			testStep = new CSwisAsyncInstallStep(CSwisAsyncInstallStep::EUseFileName, Swi::Test::ECancelAfterDialog);
	else if (aStepName == KSwisUninstallAsyncStep) 
			testStep = new CSwisAsyncUninstallStep(CSwisAsyncUninstallStep::EByUid);
	else if (aStepName == KSwisUninstallAsyncCancelStep) 
			testStep = new CSwisAsyncUninstallStep(CSwisAsyncUninstallStep::EByUid, Swi::Test::ECancelTimerFromStart);
	else if (aStepName == KSwisUninstallAsyncCancelAfterDialogsStep) 
			testStep = new CSwisAsyncUninstallStep(CSwisAsyncUninstallStep::EByUid, Swi::Test::ECancelAfterDialog);
    else if (aStepName == KTrustStatus)
	    	testStep = new CTrustStatusStep();
    else if (aStepName == KPirCheck)
		    testStep = new CPostInstallRevocationCheckStep();
// Support step for ROM stub PU testing
	else if (aStepName == KSwisForceRemove)
			testStep = new CForceRemove();
// Support step to fill drive for tests of insufficient space
	else if (aStepName == KSwisFillDrive)
			testStep = new CFillDrive();
// Support step to shut down app left running by testrun step
	else if (aStepName == KSwisShutdownApp)
			testStep = new CShutdownApp();
// Support step to check if process is running
	else if (aStepName == KSwisFindRunningExe)
			testStep = new CFindRunningExe();
// Support step for MMC operations
	else if (aStepName == KSwisMmcFormatStep)	// MMC operations
		testStep = new CSwisMmcStep(CSwisMmcStep::EFormat);
	else if (aStepName == KSwisMmcMountStep)	// MMC operations
		testStep = new CSwisMmcStep(CSwisMmcStep::EMount);
	else if (aStepName == KSwisMmcUnMountStep)	// MMC operations
		testStep = new CSwisMmcStep(CSwisMmcStep::EUnMount);
	else if (aStepName == KSwisMmcCopyFiles)	// MMC operations
		testStep = new CSwisMmcStep(CSwisMmcStep::ECopyFiles);
	else if (aStepName == KSwisMmcCheckFiles)	// MMC operations
		testStep = new CSwisMmcStep(CSwisMmcStep::ECheckFiles);
	else if (aStepName == KSwisMmcResetTags)	// MMC operations
		testStep = new CSwisMmcStep(CSwisMmcStep::EResetTags);
	else if (aStepName == KSwisMmcDeleteFiles)	// MMC operations
 		testStep = new CSwisMmcStep(CSwisMmcStep::EDeleteFiles);
	else if (aStepName == KSwisMmcGetNumFilesFiles)	// MMC operations
 		testStep = new CSwisMmcStep(CSwisMmcStep::EGetNumFiles);
	else if (aStepName == KAskRegistry)    // Ask the Registry for App	
		testStep = new CSwisMmcStep(CSwisMmcStep::EAskReg);
	else if (aStepName == KSetClearReadOnly)    // Set or clear read only bit	
		testStep = new CSwisMmcStep(CSwisMmcStep::ESetClearReadOnly);
	else if (aStepName == KQuerySidViaApparcPlugin)
		testStep = new CSwisMmcStep(CSwisMmcStep::EQuerySidViaApparcPlugin);
	else if (aStepName == KEnsureSwisNotRunningStep)
		testStep = new CEnsureSwisNotRunning;
	else if (aStepName == KSwiCopyFilesStep)
		testStep = new CSwiCopyFilesStep();
	else if (aStepName == KSwiDeleteFilesStep)
		testStep = new CSwiDeleteFilesStep();
// Support step for removal and listing of uninstalled application
	else if (aStepName == KSwisListUninstallPkgsStep)	// Listing Uninstalled Pkgs
		testStep = new CSwisUninstallPkgsStep(CSwisUninstallPkgsStep::EListPkgs);
	else if (aStepName == KSwisRemoveUninstallPkgsStep)	// Removing Uninstalled apps
		testStep = new CSwisUninstallPkgsStep(CSwisUninstallPkgsStep::ERemovePkgs);
	else if (aStepName == KSwisGetPackageDetails)	// Get Package Details
		testStep = new CSwisUninstallPkgsStep(CSwisUninstallPkgsStep::EPkgDetails);
	else if (aStepName == KSwisGetPublishedUidArrayStep)  // Get Published Package Uid's Array
	        testStep = new CSwisCheckPublishUidStep();
	else if (aStepName == KSwisRemoveWithLastDependent)  // Set RemoveWithLastDependent
		testStep = new CSwisSetRemoveWithLastDependent();
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	else if (aStepName == KCheckSCRFieldStep)	// Check the SCR components' 'Origin Verified', 'Known Revoked', 'DRM Ptotected' field values.
		testStep = new CCheckScrFieldStep();
	else if (aStepName == KCheckSCRCompPropertyStep)	// Check the SCR component property value.
		testStep = new CCheckScrCompPropertyStep();
	#endif

	return testStep;

	}
