/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "swilaunchercaptest.h"
#include "sislauncherclient.h"
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "swtypereginfo.h"
#include "sislauncherclient.h"
#endif
#include <e32def.h>

_LIT(KSwiLauncherCapTestName, "Swi Launcher capability test");

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
_LIT(KSwiLauncherInstallServerUtilCapTestName, "Swi Launcher Utils for InstallServer capability test");
_LIT(KSwiLauncherSisRegistryServerUtilCapTestName, "Swi Launcher Utils for SisRegistryServer capability test");
_LIT(KSisLauncherSwiSidCapTest, "Sis Launcher API's which require SWI Sid");
const TUid KInstallServerUid = {0x101F7295};
const TUid KSisRegistryServerUid = {0x10202DCA};
#endif



//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

CSwiLauncherCapTest* CSwiLauncherCapTest::NewL()
	{
	CSwiLauncherCapTest* self=new(ELeave) CSwiLauncherCapTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSwiLauncherCapTest::CSwiLauncherCapTest()
	{
	SetCapabilityRequired(ECapabilityTCB);
	}
	
void CSwiLauncherCapTest::ConstructL()
	{
	SetNameL(KSwiLauncherCapTestName);
	}

void CSwiLauncherCapTest::RunTestL()
	{
	Swi::RSisLauncherSession launcher;
	CleanupClosePushL(launcher);
		
	if (KErrNone != CheckFailL(launcher.Connect(), _L("Connect")))
		{
		// if we get here we didn't connect and expected not to, but cannot carry on with tests
		CleanupStack::PopAndDestroy(&launcher);
		return;
		}

	TInt err=KErrNone;
	
	TRAP(err, launcher.RunExecutableL(_L("exe"), ETrue));
	CheckFailL(err, _L("RunExecutableL"));


	RFs fs;
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(fs.ShareProtected());
	CleanupClosePushL(fs);
		
	RFile file;
	CleanupClosePushL(file);
	
	_LIT(KFilePath ,"\\tswi\\swicaptests\\data\\testdoc.txt");
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> testDocFileOnSysDrive = sysDrive.Name();
	testDocFileOnSysDrive.Append(KFilePath);
	
	User::LeaveIfError(file.Open(fs, testDocFileOnSysDrive , EFileShareExclusive|EFileRead));
	
	TRAP(err, launcher.StartDocumentL(file, ETrue));
	CheckFailL(err, _L("StartDocumentL"));
		
	TRAP(err, launcher.StartByMimeL(file, _L8("mime"), ETrue));
	CheckFailL(err, _L("StartByMimeL"));
	
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(&fs);

	RArray<TUid> uidList;
	TRAP(err, launcher.ShutdownL(uidList, 10));
	CheckFailL(err, _L("ShutdownL"));

	CleanupStack::PopAndDestroy(&launcher);
	}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
CSwiLauncherInstallServerUtilCapTest* CSwiLauncherInstallServerUtilCapTest::NewL()
	{
	CSwiLauncherInstallServerUtilCapTest* self = new (ELeave) CSwiLauncherInstallServerUtilCapTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSwiLauncherInstallServerUtilCapTest::CSwiLauncherInstallServerUtilCapTest()
	{
	SetCapabilityRequired(ECapabilityTCB);
	SetSidRequired(KInstallServerUid);
	}
	
void CSwiLauncherInstallServerUtilCapTest::ConstructL()
	{
	SetNameL(KSwiLauncherInstallServerUtilCapTestName);
	}

void CSwiLauncherInstallServerUtilCapTest::RunTestL()
	{
	Swi::RSisLauncherSession launcher;
	CleanupClosePushL(launcher);

	if (launcher.Connect() != KErrNone)
		{
		CleanupStack::PopAndDestroy(&launcher);
		return;
		}

	TInt err=KErrNone;

	RFs fs;
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(fs.ShareProtected());
	CleanupClosePushL(fs);
	
	RFile file;
	CleanupClosePushL(file);
	
	_LIT(KFilePath ,"\\tswi\\swicaptests\\data\\testdoc.txt");
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> testDocFileOnSysDrive = sysDrive.Name();
	testDocFileOnSysDrive.Append(KFilePath);
	
	User::LeaveIfError(file.Open(fs, testDocFileOnSysDrive , EFileShareExclusive|EFileRead));
	
	RCPointerArray<Usif::CSoftwareTypeRegInfo> regInfoArray;
	CleanupClosePushL(regInfoArray);
	
	TRAP(err, launcher.ParseSwTypeRegFileL(file, regInfoArray));
	CheckFailL(err, _L("ParseSwTypeRegFileL"));
	
	RCPointerArray<HBufC8> mimeTypes;
	CleanupClosePushL(mimeTypes);
	
	TRAP(err, launcher.RegisterSifLauncherMimeTypesL(mimeTypes));
	CheckFailL(err, _L("RegisterSifLauncherMimeTypesL"));
	
	CleanupStack::PopAndDestroy(5, &launcher); // file, regInfoArray, fs, mimeTypes
	}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

CSwiLauncherSisRegistryServerUtilCapTest* CSwiLauncherSisRegistryServerUtilCapTest::NewL()
	{
	CSwiLauncherSisRegistryServerUtilCapTest* self = new (ELeave) CSwiLauncherSisRegistryServerUtilCapTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSwiLauncherSisRegistryServerUtilCapTest::CSwiLauncherSisRegistryServerUtilCapTest()
	{
	SetCapabilityRequired(ECapabilityTCB);
	SetSidRequired(KSisRegistryServerUid);
	}
	
void CSwiLauncherSisRegistryServerUtilCapTest::ConstructL()
	{
	SetNameL(KSwiLauncherSisRegistryServerUtilCapTestName);
	}

void CSwiLauncherSisRegistryServerUtilCapTest::RunTestL()
	{
	Swi::RSisLauncherSession launcher;
	CleanupClosePushL(launcher);
	
	const RArray<Swi::TAppUpdateInfo> appUpdateInfo;
	
	if (launcher.Connect() != KErrNone)
		{
		CleanupStack::PopAndDestroy(&launcher);
		return;
		}

	TInt err=KErrNone;

	RCPointerArray<HBufC8> mimeTypes;
	CleanupClosePushL(mimeTypes);
	
	TRAP(err, launcher.UnregisterSifLauncherMimeTypesL(mimeTypes));
	CheckFailL(err, _L("UnregisterSifLauncherMimeTypesL"));
    
    TRAP(err, launcher.NotifyNewAppsL(appUpdateInfo));
    CheckFailL(err, _L("NotifyNewAppsL - update apparc"));
	
	CleanupStack::PopAndDestroy(2, &launcher); // mimeTypes
	}

//CSisLauncherSwiSidTest----------------------------------------------------------------------------------------------------------------------------------

CSisLauncherSwiSidTest* CSisLauncherSwiSidTest::NewL()
    {
    CSisLauncherSwiSidTest* self = new (ELeave) CSisLauncherSwiSidTest();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CSisLauncherSwiSidTest::CSisLauncherSwiSidTest()
    {
    SetCapabilityRequired(ECapabilityTCB);
    SetSidRequired(KInstallServerUid);
    }
    
void CSisLauncherSwiSidTest::ConstructL()
    {
    SetNameL(KSisLauncherSwiSidCapTest);
    }

void CSisLauncherSwiSidTest::RunTestL()
    {
    Swi::RSisLauncherSession launcher;
    CleanupClosePushL(launcher);
    const RPointerArray<Usif::CApplicationRegistrationData> appRegData;
    TInt err = launcher.Connect();
    if (err != KErrNone)
        {
        CleanupStack::PopAndDestroy(&launcher);
        return;
        }
    err=KErrNone;
    TRAP(err, launcher.NotifyNewAppsL(appRegData));
    CheckFailL(err, _L("NotifyNewAppsL - force registration"));
    CleanupStack::PopAndDestroy(&launcher);
    }

#endif
