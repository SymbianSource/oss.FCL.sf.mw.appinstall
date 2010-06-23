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
* SWIS test step implementation
*
*/


/**
 @file
*/

#include "tswisstep.h"
#include "testutilclientswi.h"
#include <test/testexecutelog.h>
#include <swi/asynclauncher.h>

#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include <swi/sisregistrypackage.h>

#include "cleanuputils.h"
#include "tswisasyncstep.h"


#include "dessisdataprovider.h"
#include "cafsisdataprovider.h"
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <apsidchecker.h>
#endif
#include <e32math.h>

using namespace Swi;
using namespace Swi::Test;

const TTimeIntervalMicroSeconds32 KTIME=2000000000; // Under 100000 tests won't be able to finish and fail
const TTimeIntervalMicroSeconds32 KTIMEDIALOGDELAY=20000; 
TTimeIntervalMicroSeconds32 KTIMETOCANCEL=30000; 
const TTimeIntervalMicroSeconds32 KDefaultTimeToCancelAfterDialog=0; 

//
// CSwisAsyncInstallStep
//
_LIT(KExpectedHash, "hash");
_LIT(KExpectedDialog, "dialog");
_LIT(KErrCode, "errorcode");

CSwisAsyncStep::~CSwisAsyncStep()
{
	if(iAsyncManager)
		{
		iAsyncManager->Cancel();
		delete iAsyncManager;
		iAsyncManager=0;
		}

	delete iUi;
	iUi=0;
	}

CSwisAsyncStep::CSwisAsyncStep(TSwiCancelType aCancelTestType)
	: iCancelTestType(aCancelTestType)
	{
	}

void CSwisAsyncStep::GetCancelDelay()
	{
	TInt cancelDelay;
	if (GetIntFromConfig(ConfigSection(), _L("maxcanceldelay"), cancelDelay))
		{
		// Get pseudo-random delay up to specified maximum
		TTime universalTime;
		universalTime.UniversalTime();
		TInt64 now = universalTime.Int64();
		TInt rand = Math::Rand(now);
		iCancelDelay = rand % cancelDelay;
		INFO_PRINTF3(_L("Specified maximum async cancel delay is %d microseconds, chosen pseudo-random delay is %d microseconds."), 
				cancelDelay, iCancelDelay.Int());
		}
	else
		{
		if (GetIntFromConfig(ConfigSection(), _L("canceldelay"), cancelDelay))
			{
			iCancelDelay = cancelDelay;
			}
		else
			{
			iCancelDelay = (iCancelTestType == ECancelAfterDialog) ?
							KDefaultTimeToCancelAfterDialog : KTIMETOCANCEL;
			}
		INFO_PRINTF2(_L("Delay for async cancel is %d microseconds"), 
					iCancelDelay.Int());
		}
	}

void CSwisAsyncStep::RunOperation()
	{
	if(iCancelTestType == ECancelTimerFromStart) 
		{
		iAsyncManager->DoIt(iCancelDelay, true);
		INFO_PRINTF1(_L("Invoking CancelOperation()"));
		iAsyncManager->CancelOperation();
		}
	else if(iCancelTestType == ECancelAfterDialog) 
		{
		iAsyncManager->DoIt(KTIME);
		iAsyncManager->DoIt(iCancelDelay, true);
		INFO_PRINTF1(_L("Invoking CancelOperation()"));
		iAsyncManager->CancelOperation();
		}
	iAsyncManager->DoIt(KTIME);
	}

void CSwisAsyncInstallStep::Stop(TInt aError)
	{
    if(aError!=KErrNone && (aError != KErrCancel || iCancelTestType != ECancelAfterDialog))
		{
		SetTestStepError(aError);
		SetTestStepResult(EFail);
		}
	iAsyncManager->Stop();	
	}


CSwisAsyncInstallStep::CSwisAsyncInstallStep(TInstallType aInstallType, TSwiCancelType aCancelTestType)
	:  CSwisAsyncStep(aCancelTestType), iInstallType(aInstallType)
	{
	// Call base class method to set up the human readable name for logging
	
	switch (aInstallType)
		{
		case EUseFileHandle:
			SetTestStepName(KSwisInstallFHStep);
			break;
		
		case EUseFileName:
			SetTestStepName(KSwisInstallStep);
			break;
		
		case EUseCAF:
			SetTestStepName(KSwisInstallCAFStep);
			break;

		case EUseOpenFileName:
			SetTestStepName(KSwisInstallOpenFileStep);
			break;	
		}
	}

CSwisAsyncInstallStep::~CSwisAsyncInstallStep()
	{
	iDeviceLanguages.Close();
	}


/**
 * Override of base class virtual. Prepares for the test run of SWIS
 * @return TVerdict code
 */
TVerdict CSwisAsyncInstallStep::doTestStepPreambleL()
	{
	// get step parameters
	TPtrC str;

	if (!GetStringFromConfig(ConfigSection(), _L("sis"), str))
		{
		ERR_PRINTF1(_L("Missing SIS file name"));
		SetTestStepError(KErrBadName);
		SetTestStepResult(EFail);
		}
	else
		{
		iSisFileName.Copy(str);
		if (!GetStringFromConfig(ConfigSection(), _L("script"), str))
			{
			ERR_PRINTF1(_L("Missing XML file name"));
			SetTestStepError(KErrBadName);
			SetTestStepResult(EFail);
			}
		else
			{
			iXmlFileName.Copy(str);
			INFO_PRINTF3(_L("Installing '%S' using script '%S'"), 
				&iSisFileName, &iXmlFileName);
			// create UI handler and populate the answers from XML file
			delete iUi;
			if(GetIntFromConfig(ConfigSection(),KExpectedDialog,iExpectedDialog) && GetIntFromConfig(ConfigSection(), KErrCode,iKErrCode ) && GetStringFromConfig(ConfigSection(),KExpectedHash,iExpectedHash))
				{
				iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger(),_L("NULL"),iExpectedHash, iExpectedDialog, iKErrCode);				
				}
			else if (GetIntFromConfig(ConfigSection(),KExpectedDialog,iExpectedDialog) && GetIntFromConfig(ConfigSection(), KErrCode,iKErrCode ))        
		    	{
		    	iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger(),_L("NULL"),_L("NULL"),iExpectedDialog,iKErrCode);
		    	}
			else if (GetStringFromConfig(ConfigSection(),KExpectedHash,iExpectedHash))	
		        {
		        iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger(),_L("NULL"), iExpectedHash,NULL, NULL);
		        }
		    else
		    	{
		        iUi=CUIScriptAdaptor::NewL(iXmlFileName, Logger());
				}

			// If this is a cancel test, get the delay to use.
			if (iCancelTestType != ENoCancel)
				{
				GetCancelDelay();
				}
			}
		}
	GetDevSupportedLanguagesL(iDeviceLanguages);
	if (iDeviceLanguages.Count() > 0)
		iUseDeviceLanguages = ETrue;


	return TestStepResult();
	}

/**
 * Override of base class pure virtual
 * @return TVerdict code
 */
TVerdict CSwisAsyncStep::doTestStepL()
	{
	if (TestStepResult()!=EPass)
		{
		return TestStepResult();
		}

	INFO_PRINTF1(TestStepName());

	// Trap all parts of the test step which can leave, to work around
	// TestExecute's interpretation of certain leave codes as reasons to retry.
	
	TRAPD(err,doTestStepBodyL());

	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Test step body left with %d."), err);
		// Can't set error to KErrServerBusy, or Testexecute will retry.
		SetTestStepResult((err != KErrServerBusy) ? static_cast<TVerdict>(err) : EFail);
		}

	return TestStepResult();
	}

void CSwisAsyncInstallStep::doTestStepBodyL()
	{
	// launch the installation
	CInstallPrefs* prefs=CInstallPrefs::NewLC();

	// get OCSP responder URI from INI file
	TPtrC uri;
	if (GetStringFromConfig(ConfigSection(), _L("ocsp_uri"), uri))
		{
		const TUint KMaxUriLength=512;
		TBuf8<KMaxUriLength> temp;
		temp.Copy(uri);
		prefs->SetRevocationServerUriL(temp);
		}

	TInt ocspFlag=0;
	if (GetIntFromConfig(ConfigSection(), _L("ocsp_flag"), ocspFlag))
		{
		prefs->SetPerformRevocationCheck(ocspFlag ? ETrue : EFalse);
		}

	if(iCancelTestType == ECancelTimerFromStart)
		{
		iUi->SetDialogDelay(KTIMEDIALOGDELAY);
		}

 	TInt timeout;
 	if(GetIntFromConfig(ConfigSection(), _L("timeout"), timeout))
 		{
 		KTIMETOCANCEL = timeout;
 		}

	RTestUtilSessionSwi util;
	User::LeaveIfError(util.Connect());
	CleanupClosePushL(util);	

	if (iCancelTestType == ECancelAfterDialog)
		{
		// Cancel testing
		TBool cancelled = EFalse;
		TInt dialogNumber = 0;
		iUi->SetCancelDialogTesting(ECancelAfterDialog);
	
		RArray<TPtrC> fileNumExist;
		RArray<TPtrC> fileNumNonExist;
		GetFileNamesForCheck(fileNumExist, fileNumNonExist, ECancelType);

		while (ETrue)
			{
			iUi->ResetCurrentDialogNumber();
			iUi->SetCancelDialogNumber(dialogNumber);
			TInt error=DoInstallL(*prefs);

			if (error != KErrNone && iUi->InstallationCancelled())
				{
				cancelled=ETrue;

				// Error ought to be KErrCancel
				if (error != KErrCancel)
					{
					ERR_PRINTF4(_L("Installation returned %d, KErrCancel (%d) expected, after dialog %d"), error, KErrCancel, dialogNumber);
					User::Leave(error);	
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Installation cancellation after dialog %d successful"), dialogNumber);
					// Files which are labelled as existing should not exist since we cancelled.
					CheckIfFilesExist(ETrue,fileNumExist,util);
					CheckIfFilesExist(EFalse,fileNumNonExist,util);
					
					// we might have failed the test already so stop.
					if (TestStepResult()!=EPass)
						{
						break;
						}
					// else continue on to next loop
					}
				}
			else
				{
				if(error != KErrNone)
					{
					ERR_PRINTF2(_L("DoInstallL returned %d"), error);
					User::Leave(error);
					}
				// No Error, so must have finished installation
				break;
				}

			// fail at next dialog
			++dialogNumber;
			}

		// Only check files if this is the last step and we didn't cancel
		if (!cancelled)
			{
			WARN_PRINTF1(_L("Installation completed with no error and no cancel."));
			// Files which are labelled as existing should exist since we didn't cancel.
			CheckIfFilesExist(EFalse,fileNumExist,util);
			CheckIfFilesExist(ETrue,fileNumNonExist,util);
			}
		}
	else
		{
		DoInstallL(*prefs);		
		}
	CleanupStack::PopAndDestroy(&util);	
	CleanupStack::PopAndDestroy(prefs);
	}

TInt CSwisAsyncInstallStep::DoInstallL(CInstallPrefs& aInstallPrefs)
	{
	delete iAsyncManager;
	iAsyncManager=NULL;
	iAsyncManager=CAsyncManager::NewL(this);
	
	switch (iInstallType)
		{
		case EUseFileName: // Tests added
			{
			if (iUseDeviceLanguages)
				iAsyncManager->InstallL(*iUi, iSisFileName, aInstallPrefs, iDeviceLanguages);
			else	
				iAsyncManager->InstallL(*iUi, iSisFileName, aInstallPrefs);
			RunOperation();
			}; break;
		case EUseOpenFileName:
			{
			RFs fs;
			User::LeaveIfError(fs.Connect());
			fs.ShareProtected();
			CleanupClosePushL(fs);
			RFile file;
			User::LeaveIfError(file.Open(fs, iSisFileName, EFileShareReadersOnly));
			CleanupClosePushL(file);
			if (iUseDeviceLanguages)
				iAsyncManager->InstallL(*iUi, iSisFileName, aInstallPrefs, iDeviceLanguages);
			else
				iAsyncManager->InstallL(*iUi, iSisFileName, aInstallPrefs); 
			RunOperation();
			CleanupStack::PopAndDestroy(2, &fs);
			};break;
		case EUseFileHandle: // Tests added
			{
			RFs fs;
			User::LeaveIfError(fs.Connect());
			fs.ShareProtected();
			CleanupClosePushL(fs);
			RFile file;
			User::LeaveIfError(file.Open(fs, iSisFileName, 0));
			CleanupClosePushL(file);
			if (iUseDeviceLanguages)
				iAsyncManager->InstallL(*iUi, file, aInstallPrefs, iDeviceLanguages);
			else
				iAsyncManager->InstallL(*iUi, file, aInstallPrefs); 
			RunOperation();
			CleanupStack::PopAndDestroy(2, &fs);
			};break;
		case EUseCAF:
			{
			CCafSisDataProvider* dataProvider=CCafSisDataProvider::NewLC(iSisFileName);
			iAsyncManager->InstallL(*iUi, *dataProvider, aInstallPrefs); 
			RunOperation();
			CleanupStack::PopAndDestroy(dataProvider);
			};break;
		default: 
			User::Leave(KErrArgument);
		}
	return iAsyncManager->iStatus.Int();
	}
	

/**
 * Override of base class virtual
 * @return TVerdict code
 */
TVerdict CSwisAsyncInstallStep::doTestStepPostambleL()
	{
	if (iCancelTestType == ENoCancel)
		{
		RArray<TPtrC> fileNumExist;
		RArray<TPtrC> fileNumNonExist;
		GetFileNamesForCheck(fileNumExist, fileNumNonExist, ESimpleType);
		
		RTestUtilSessionSwi util;
		User::LeaveIfError(util.Connect());
		CleanupClosePushL(util);
		
		CheckIfFilesExist(ETrue,fileNumExist,util);
		CheckIfFilesExist(EFalse,fileNumNonExist,util);
		CleanupStack::PopAndDestroy(&util);
		}

	CheckPubSubStatusIdle();
	return TestStepResult();
	}


//
// CSwisAsyncUninstallStep
//

CSwisAsyncUninstallStep::CSwisAsyncUninstallStep(TAsyncUninstallType aType, TSwiCancelType aCancelTestType)
	: CSwisAsyncStep(aCancelTestType), iType(aType)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSwisUninstallAsyncStep);
	}

	
/**
 * Override of base class virtual. Prepares for the test run of SWIS
 * @return TVerdict code
 */
TVerdict CSwisAsyncUninstallStep::doTestStepPreambleL()
	{
	// get step parameters
	TInt uid=0;

	TPtrC str;
	if (!GetStringFromConfig(ConfigSection(), _L("script"), str))
		{
		ERR_PRINTF1(_L("Missing XML file name"));
		SetTestStepResult(EFail);
		return EFail;
		}

	if (!GetHexFromConfig(ConfigSection(), _L("uid"), uid))
		{
		ERR_PRINTF1(_L("Missing uid"));
		SetTestStepResult(EFail);
		return EFail;
		}

	iUid.iUid=uid;
		
	if (iType==EByPackage)
		{
		TPtrC vendorName;
		if (!GetStringFromConfig(ConfigSection(), _L("vendorName"), vendorName))
			{
			ERR_PRINTF1(_L("Missing Package Name"));
			SetTestStepResult(EFail);
			return EFail;
			}
		iVendorName.Set(vendorName);
		
		TPtrC packageName;
		if (!GetStringFromConfig(ConfigSection(), _L("packageName"), packageName))
			{
			ERR_PRINTF1(_L("Missing Vendor Name"));
			SetTestStepResult(EFail);
			return EFail;
			}
		iPackageName.Set(packageName);
			
		iXmlFileName.Copy(str);
		INFO_PRINTF5(_L("Uninstalling %D, %S, %S  using script '%S'"), 
			iUid.iUid, &iPackageName, &iVendorName, &iXmlFileName);
		// create UI handler and populate the answers from XML file
		if (GetIntFromConfig(ConfigSection(),KExpectedDialog,iExpectedDialog) && GetIntFromConfig(ConfigSection(), KErrCode,iKErrCode ))
		    {
		    iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger(),_L("NULL"),_L("NULL"),iExpectedDialog,iKErrCode);
		    }
		else
			{
			iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger());	
			}   
		}
	else if (iType==EByUid)
		{
		iXmlFileName.Copy(str);
		INFO_PRINTF3(_L("Uninstalling '%D' using script '%S'"), 
			iUid.iUid, &iXmlFileName);
		// create UI handler and populate the answers from XML file
		if (GetIntFromConfig(ConfigSection(),KExpectedDialog,iExpectedDialog) && GetIntFromConfig(ConfigSection(), KErrCode,iKErrCode ))
		    {
		    iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger(),_L("NULL"),_L("NULL"),iExpectedDialog,iKErrCode);
		    }
		else
			{
			iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger());	
			}
		
		}

	// If this is a cancel test, get the delay to use.
	if (iCancelTestType != ENoCancel)
		{
		GetCancelDelay();
		}

	return TestStepResult();
	}

TInt CSwisAsyncUninstallStep::DoUninstallL()
	{

	delete iAsyncManager;
	iAsyncManager=NULL;
	iAsyncManager=CAsyncManager::NewL(this);
	
	if (iType==EByUid)
		{
		iAsyncManager->UninstallL(*iUi, iUid); 
		RunOperation();
		}
	else if (iType==EByPackage)
		{
		// Provisional fix to work around registry problem		
		// Go through list of packages from base package to get augmentations.
		CSisRegistryPackage* uninstallPackage=CSisRegistryPackage::NewLC(iUid, iPackageName, iVendorName);
		
		RSisRegistrySession registrySession;
		User::LeaveIfError(registrySession.Connect());
		CleanupClosePushL(registrySession);

		RSisRegistryEntry registryEntry;
	
		User::LeaveIfError(registryEntry.Open(registrySession, iUid));
		CleanupClosePushL(registryEntry);

		CSisRegistryPackage* package=registryEntry.PackageL();
		CleanupStack::PushL(package);
		
		if (*package==*uninstallPackage)
			{
			iAsyncManager->UninstallL(*iUi, *package); 
			RunOperation();
			}
		else
			{
			// check augmenations
			RPointerArray<CSisRegistryPackage> augmentationPackages;
			CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(augmentationPackages);

			registryEntry.AugmentationsL(augmentationPackages);
			for (TInt i=0; i < augmentationPackages.Count(); ++i)
				{
				if (*augmentationPackages[i]==*uninstallPackage)
					{
					iAsyncManager->UninstallL(*iUi, *augmentationPackages[i]); 
					RunOperation();
					break;
					}
				}
			CleanupStack::PopAndDestroy(&augmentationPackages);
			}	
		
		CleanupStack::PopAndDestroy(3, &registrySession);
		CleanupStack::PopAndDestroy(uninstallPackage);
		}
	return iAsyncManager->iStatus.Int();
	}

void CSwisAsyncUninstallStep::doTestStepBodyL()
	{
	if(iCancelTestType == ECancelTimerFromStart)
		{
		iUi->SetDialogDelay(KTIMEDIALOGDELAY);
		}
	
	RTestUtilSessionSwi util;
	User::LeaveIfError(util.Connect());
	CleanupClosePushL(util);
		
	if (iCancelTestType == ECancelAfterDialog)
		{
		// Cancel testing
		TBool cancelled = EFalse;
		TInt dialogNumber = 0;
		iUi->SetCancelDialogTesting(ECancelAfterDialog);
		
		RArray<TPtrC> fileNumExist;
		RArray<TPtrC> fileNumNonExist;
		GetFileNamesForCheck(fileNumExist, fileNumNonExist, ECancelType);
		
		while (ETrue)
			{
			iUi->ResetCurrentDialogNumber();
			iUi->SetCancelDialogNumber(dialogNumber);
			
			TInt error=DoUninstallL();
			
			if (error != KErrNone && iUi->InstallationCancelled())
				{
				cancelled=ETrue;
				
				// Error ought to be KErrCancel
				if (error != KErrCancel)
					{
					ERR_PRINTF4(_L("Uninstallation returned %d, KErrCancel (%d) expected, after dialog %d"), error, KErrCancel, dialogNumber);
					User::Leave(error);	
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Uninstallation cancellation after dialog %d successful"), dialogNumber);
					// Files which are labelled as existing should not exist since we cancelled.
					CheckIfFilesExist(ETrue,fileNumExist,util);
					CheckIfFilesExist(EFalse,fileNumNonExist,util);
					
					// we might have failed the test already so stop.
					if (TestStepResult()!=EPass)
						{
						break;
						}
					// else continue on to next loop
					}
				}
			else
				{
				User::LeaveIfError(error);	
				// No Error, so must have finished installation
				break;
				}
			
			// fail at next dialog
			++dialogNumber;
			}

		// Only check files if this is the last step and we didn't cancel
		if (!cancelled)
			{
			WARN_PRINTF1(_L("Uninstallation completed with no error and no cancel."));
			// Files which are labelled as existing should exist since we didn't cancel.
			CheckIfFilesExist(EFalse,fileNumExist,util);
			CheckIfFilesExist(ETrue,fileNumNonExist,util);
			}
		}
	else
		{
		// No need to check return code here.
		DoUninstallL();
		}
	CleanupStack::PopAndDestroy(&util);	
	}

/**
 * Override of base class virtual
 * @return TVerdict code
 */
TVerdict CSwisAsyncUninstallStep::doTestStepPostambleL()
	{
	RArray<TPtrC> fileNumExist;
	RArray<TPtrC> fileNumNonExist;
	GetFileNamesForCheck(fileNumExist, fileNumNonExist, ESimpleType);
		
	RTestUtilSessionSwi util;
	User::LeaveIfError(util.Connect());
	CleanupClosePushL(util);
		
	CheckIfFilesExist(ETrue,fileNumExist,util);
	CheckIfFilesExist(EFalse,fileNumNonExist,util);	
	CleanupStack::PopAndDestroy(&util);
		
	CheckPubSubStatusIdle();
	return TestStepResult();
	}

void CSwisAsyncUninstallStep::Stop(TInt aError)
{
    if(aError!=KErrNone && (aError != KErrCancel || iCancelTestType != ECancelAfterDialog))
		{
		SetTestStepError(aError);
		SetTestStepResult(EFail);
		}
	iAsyncManager->Stop();
}


//
// CAsyncManager
//

CAsyncManager *CAsyncManager::NewL(CSwisAsyncStep *parent)
	{
	CAsyncManager *self=new(ELeave) CAsyncManager(parent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAsyncManager::CAsyncManager(CSwisAsyncStep *aStep) 
	: CActive(EPriorityHigh), 
	  iStep(aStep)
	{
	}

void CAsyncManager::ConstructL()
	{
	iScheduler=new(ELeave) CStepScheduler;
	iScheduler->SetStep(iStep);
	CActiveScheduler::Install(iScheduler);

	CActiveScheduler::Add(this); // Must be after we create the scheduler

	iWatchdog=CWatchdog::NewL();

	iAsyncLauncher=CAsyncLauncher::NewL();
	User::LeaveIfError(RProperty::Get(KUidSystemCategory, KSystemStartupModeKey, iBootMode));
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#ifndef SWI_TEXTSHELL_ROM
	if (iBootMode != KTextShell) 
		{
		iSwiSidChecker = CAppSidChecker::CheckerForAppType(TUid::Uid(0));
		if(!iSwiSidChecker)
			{
			User::Leave(KErrNotSupported);
			}
		iSwiSidChecker->SetRescanCallBackL(TCallBack(CAsyncManager::StaticNotificationCallback, this));	
		}
#else
	// Since in Textshell ROM SSCForStartupMode1_target.rsc was replaces as SSCForStartupMode0.rsc
	// we always get the boot mode as 0 , reset the value to 1.
	iBootMode = KTextShell;
#endif
#else
	iBootMode = KTextShell;
#endif
	iNotificationCount = 0;
	}

CAsyncManager::~CAsyncManager()
	{
	iWatchdog->Cancel();
	Cancel();

	delete iAsyncLauncher;
	iAsyncLauncher=0;

	delete iWatchdog;
	iWatchdog=0;
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#ifndef SWI_TEXTSHELL_ROM
	if (iBootMode != KTextShell)
		{
		iSwiSidChecker->SetRescanCallBackL(TCallBack()); // nb. must be before delete of scheduler...
		delete iSwiSidChecker;
		}
#endif
#endif
	CActiveScheduler::Install(0);
	delete iScheduler;
	iScheduler=0;

	}
	
void CAsyncManager::InstallL(MUiHandler& aUiHandler,
							 const TFileName& aFileName, 
							 const CInstallPrefs& aInstallPrefs)
	{
	iAsyncLauncher->InstallL(aUiHandler, aFileName, aInstallPrefs, iStatus);
	SetActive();
	}

void CAsyncManager::InstallL(MUiHandler& aUiHandler, 
							 MSisDataProvider& aDataProvider,
							 const CInstallPrefs& aInstallPrefs)
	{
	iAsyncLauncher->InstallL(aUiHandler, aDataProvider, aInstallPrefs, iStatus);
	SetActive();
	}

void CAsyncManager::InstallL(MUiHandler& aUiHandler, 
							 RFile& aFileHandle,
							 const CInstallPrefs& aInstallPrefs)
	{
	iAsyncLauncher->InstallL(aUiHandler, aFileHandle, aInstallPrefs, iStatus);
	SetActive();
	}

void CAsyncManager::InstallL(MUiHandler& aUiHandler, const TFileName& aFileName,
				  			 const CInstallPrefs& aInstallPrefs, 
				  			 const RArray<TInt> aDeviceSupportedLanguages)
	{
	iAsyncLauncher->InstallL(aUiHandler, aFileName, aInstallPrefs, aDeviceSupportedLanguages, iStatus);
	SetActive();
	}

void CAsyncManager::InstallL(MUiHandler& aUiHandler, RFile& aFileHandle, 
			  				 const CInstallPrefs& aInstallPrefs, 
			  				 const RArray<TInt> aDeviceSupportedLanguages)
	{
	iAsyncLauncher->InstallL(aUiHandler, aFileHandle, aInstallPrefs, aDeviceSupportedLanguages, iStatus);
	SetActive();
	}
			
void CAsyncManager::InstallL(MUiHandler& aUiHandler, MSisDataProvider& aDataProvider, 
			  				 const CInstallPrefs& aInstallPrefs, 
			  				 const RArray<TInt> aDeviceSupportedLanguages)
	{
	iAsyncLauncher->InstallL(aUiHandler, aDataProvider, aInstallPrefs, aDeviceSupportedLanguages, iStatus);
	SetActive();
	}

void CAsyncManager::UninstallL(MUiHandler& aUiHandler, 
							   const TUid& aUid)
	{
	iAsyncLauncher->UninstallL(aUiHandler, aUid, iStatus);
	SetActive();
	}


void CAsyncManager::UninstallL(MUiHandler& aUiHandler,
							   const CSisRegistryPackage& aPackage)
	{
	iAsyncLauncher->UninstallL(aUiHandler, aPackage, iStatus);
	SetActive();
	}


TInt CAsyncManager::RunError(TInt aError)
	{ 
	iStep->SetTestStepError(aError);
	return KErrNone;
	}
	
void CAsyncManager::DoIt(TTimeIntervalMicroSeconds32 aTimeout,
						 bool aTimeoutIsOk)
	{	
	if(iStatus != KRequestPending)
		{
		// Already done! Probably a cancellation test waited too long
		// and an earlier call let the operation complete...
		return;
		}
	iWatchdog->Cancel();
	iWatchdog->Wait(aTimeout, aTimeoutIsOk);
	iScheduler->Start();
	}

void CAsyncManager::Stop()
	{
	iWatchdog->Cancel();
	Cancel();
	iScheduler->Stop();
	}


	
void CAsyncManager::RunL()
	{
	TInt err=iStatus.Int();
	if(err!=KErrNone)
		{
		iStep->Stop(err);
		return;
		}
	
	if (iBootMode != KTextShell)
		{
		StopIfDone();	
		}
	else if (iBootMode == KTextShell)
		{
		// Install/Uninstall operation complete
		iStep->Stop(KErrNone);
		}
		
	return;
	}


void CAsyncManager::CancelOperation()
{
	iAsyncLauncher->CancelOperation();
}

void CAsyncManager::DoCancel() 
	{
	// Called if someone calls Cancel.

	// We can NOT just call iAsyncLauncher->CancelOperation() and
	// return, because when we return, the framework will block on our
	// iStatus which will not allow the CAsynLauncher (actually the
	// CUissClientHandler active object) to process the cancel
	// request, so we would deadlock.
	
	// Instead we delete it to cause its emergency cleanup to run.
	delete iAsyncLauncher;
	iAsyncLauncher = 0;
	}

TInt CAsyncManager::StaticNotificationCallback(TAny *aAny)
	{
	return reinterpret_cast<CAsyncManager *>(aAny)->NotificationCallback();
	}

TInt CAsyncManager::NotificationCallback()
	{
	++iNotificationCount;
	StopIfDone();
	return ETrue; // Call again.
	}

void CAsyncManager::StopIfDone()
	{
	if(!IsActive() && (iNotificationCount != 0))
		{
		// Install/Uninstall operation complete and had notification so done
		iStep->Stop(KErrNone);
		}
	}


///////////////////////////////////////////////////
// Scheduler 
///////////////////////////////////////////////////


void CStepScheduler::Error(TInt aError) const
	{
	// An active object did a Leave, save its error code and stop scheduling
	iStep->SetTestStepError(aError);
	Stop();
	return;
	}


///////////////////////////////////////////////////
// CWatchdog
///////////////////////////////////////////////////
CWatchdog* CWatchdog::NewL()
	{
	CWatchdog* self=new(ELeave) CWatchdog;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

void CWatchdog::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);
	}
	
CWatchdog::CWatchdog() : CActive(EPriorityNormal)
	{
	iFromTime.HomeTime();
	}
	
CWatchdog::~CWatchdog()
	{
	Cancel();
	iTimer.Close();
	}

void CWatchdog::DoCancel()
	{
	iTimer.Cancel();
	}
	
void CWatchdog::RunL()
	{
	if(!iTimeoutIsOk)
		{
		User::Leave(KErrTimedOut);
		}
	
	CActiveScheduler::Stop();
	}
	
void CWatchdog::Wait(TTimeIntervalMicroSeconds32 aDelay, bool aTimeoutIsOk)
	{
	iTimeoutIsOk = aTimeoutIsOk;
	iTimer.After(iStatus,aDelay);
	SetActive();
	}

// End of file	
	
