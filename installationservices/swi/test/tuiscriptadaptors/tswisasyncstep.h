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
* SWIS test async step declaration
*
*/


/**
 @file
*/

#ifndef TSWISASYNCSTEP_H
#define TSWISASYNCSTEP_H
#include "tswisstep.h"
#include "tuiscriptadaptors.h"


class CAsyncManager;
class CStepScheduler;
class CSwisAsyncStep;
class CWatchdog;

class CSwisAsyncStep : public CSwisTestStep 
	{
	
public:
	virtual void Stop (TInt aError )=0;
	virtual TVerdict doTestStepL();
	virtual ~CSwisAsyncStep();
	
protected:
	void GetCancelDelay();
	void RunOperation();
	virtual void doTestStepBodyL()=0;

	CSwisAsyncStep(Swi::Test::TSwiCancelType aCancelTestType);
	CAsyncManager *iAsyncManager;
	TFileName iXmlFileName; // name of the file containing dialog data
	TTimeIntervalMicroSeconds32 iCancelDelay;
	Swi::Test::TSwiCancelType iCancelTestType;
	Swi::Test::CUIScriptAdaptor* iUi; // XML script-based UI implementation

};


class CSwisAsyncInstallStep : public CSwisAsyncStep
	{
public:
	enum TInstallType { EUseFileHandle, EUseFileName, EUseCAF, EUseOpenFileName};

	CSwisAsyncInstallStep(TInstallType aInstallType, Swi::Test::TSwiCancelType aCancelTestType = Swi::Test::ENoCancel);
	~CSwisAsyncInstallStep();
	virtual void Stop(TInt aError);
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

private:
	TInt DoInstallL(Swi::CInstallPrefs& aInstallPrefs);
	virtual void doTestStepBodyL();
private:
	TFileName iSisFileName; // name of the file to install
	TInstallType iInstallType; // use file handle to install
	TPtrC iExpectedHash;
	TInt iExpectedDialog;
	TInt iKErrCode;
	TBool iUseDeviceLanguages;
	RArray<TInt> iDeviceLanguages;
	};


/**
 * Asynchronous uninstallation test step class
 * @test
 * @internalComponent
 */

class CSwisAsyncUninstallStep : public CSwisAsyncStep
	{
public:
	enum TAsyncUninstallType { EByUid, EByPackage };
	
	CSwisAsyncUninstallStep(TAsyncUninstallType aType, Swi::Test::TSwiCancelType aCancelTestType = Swi::Test::ENoCancel);
	// virtual ~CSwisAsyncUninstallStep();
	
	virtual void Stop(TInt aError);
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	TInt DoUninstallL();
	virtual void doTestStepBodyL();

private:
	TUid iUid;
	TPtrC iVendorName;
	TPtrC iPackageName;
	TAsyncUninstallType iType;
	TBool *iFinished;
	TInt iExpectedDialog;
	TInt iKErrCode;
	};

/**
 * Manager of installation steps
 * This class waits for the async install/uninstall Active objects to finish doing something
 * @test
 * @internalComponent
 */
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
class CAppSidChecker;
#endif
class CAsyncManager : public CActive
	{
public:
	static CAsyncManager *NewL(CSwisAsyncStep* parent);
	
	~CAsyncManager();
	
	void DoIt(TTimeIntervalMicroSeconds32 aTimeout, bool aTimeoutIsOk = false);
	void Stop();


	void InstallL(Swi::MUiHandler& aUiHandler,
				  const TFileName& aFileName, 
				  const Swi::CInstallPrefs& aInstallPrefs);
	
	void InstallL(Swi::MUiHandler& aUiHandler, 
				  Swi::MSisDataProvider& aDataProvider,
				  const Swi::CInstallPrefs& aInstallPrefs);

	void InstallL(Swi::MUiHandler& aUiHandler, 
				  RFile& aFileHandle,
				  const Swi::CInstallPrefs& aInstallPrefs);
	
	void InstallL(Swi::MUiHandler& aUiHandler, const TFileName& aFileName,
				  const Swi::CInstallPrefs& aInstallPrefs, 
				  const RArray<TInt> aDeviceSupportedLanguages);
			
	void InstallL(Swi::MUiHandler& aUiHandler, RFile& aFileHandle, 
				  const Swi::CInstallPrefs& aInstallPrefs, 
				  const RArray<TInt> aDeviceSupportedLanguages);
				
	void InstallL(Swi::MUiHandler& aUiHandler, Swi::MSisDataProvider& aDataProvider, 
				  const Swi::CInstallPrefs& aInstallPrefs, 
				  const RArray<TInt> aDeviceSupportedLanguages);

	void UninstallL(Swi::MUiHandler& aUiHandler, 
					const TUid& aUid);

	void UninstallL(Swi::MUiHandler& aUiHandler,
					const Swi::CSisRegistryPackage& aPackage);
	
	void CancelOperation();


	// From CActive
	void RunL();
	void DoCancel();	
	TInt RunError(TInt aError);

	
private:
	CAsyncManager(const CAsyncManager&); // prevent copy construct
	const CAsyncManager& operator=(const CAsyncManager&); // prevent assignment

	CAsyncManager(CSwisAsyncStep* aStep);
	void ConstructL();

	static TInt StaticNotificationCallback(TAny *aAny);
	TInt NotificationCallback();

	void StopIfDone();

	CSwisAsyncStep* iStep; // Our parent step

	CStepScheduler* iScheduler;

	CWatchdog* iWatchdog;

	Swi::CAsyncLauncher* iAsyncLauncher;
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	CAppSidChecker *iSwiSidChecker;
#endif
	TInt iNotificationCount;
	TInt iBootMode;
	};


/**
 * Scheduler step
 * This class waits for the async install/uninstall Active objects to finish doing something
 * @test
 * @internalComponent
 */
class CStepScheduler : public CActiveScheduler
	{
public:
	virtual void Error(TInt aError) const;
	void SetStep(CSwisAsyncStep* aStep) { iStep = aStep; }
	
protected:
	CSwisAsyncStep* iStep;
	};

class CWatchdog : public CActive 
	{
public:
	static CWatchdog* NewL();
	~CWatchdog();
	void Wait(TTimeIntervalMicroSeconds32 aTimeout, bool aTimeoutIsOk = false);
	
	void RunL();
	void DoCancel();
protected:
	CWatchdog();
	void ConstructL();
	
private:
	RTimer iTimer;
	TTime iFromTime;
	bool iTimeoutIsOk;
	};


_LIT(KSwisInstallAsyncStep, "InstallAsyncStep");
_LIT(KSwisInstallAsyncFHStep, "InstallAsyncFHStep");
_LIT(KSwisInstallAsyncCAFStep, "InstallAsyncCAFStep");
_LIT(KSwisInstallAsyncCancelStep, "InstallAsyncCancelStep");
_LIT(KSwisUninstallAsyncStep, "UninstallAsyncStep");
_LIT(KSwisUninstallAsyncCancelStep, "UninstallAsyncCancelStep");
_LIT(KSwisInstallAsyncCancelAfterDialogsStep, "InstallAsyncCancelAfterDialogsStep");
_LIT(KSwisUninstallAsyncCancelAfterDialogsStep, "UninstallAsyncCancelAfterDialogsStep");

#endif // TSWISASYNCSTEP_H
