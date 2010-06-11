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
*
*/


/**
 @file
*/

#include "uisscaptest.h"

#include "sislauncherclient.h"

#include <e32def.h>

_LIT(KUissCapTestName, "Swi Launcher capability test");


CUissCapTest* CUissCapTest::NewL()
	{
	CUissCapTest* self=new(ELeave) CUissCapTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CUissCapTest::CUissCapTest()
	{
	SetCapabilityRequired(ECapabilityTCB);
	}
	
void CUissCapTest::ConstructL()
	{
	SetNameL(KUissCapTestName);
	}

void CUissCapTest::RunTestL()
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
	
	User::LeaveIfError(file.Open(fs, testDocFileOnSysDrive , EFileShareExclusive|EFileWrite));	
	
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

