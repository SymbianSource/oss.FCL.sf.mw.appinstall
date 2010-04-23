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

#define private public
#include "launcher.h"
#undef private

#include "sishelpercaptest.h"
#include "sisuihandler.h"


#include "sishelperclient.h"

#include <e32def.h>
#include <e32math.h>


_LIT(KSisHelperCapTestName, "SisHelper capability test");

using namespace Swi;

CSisHelperCapTest* CSisHelperCapTest::NewL()
	{
	CSisHelperCapTest* self=new(ELeave) CSisHelperCapTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSisHelperCapTest::CSisHelperCapTest()
	{
	SetCapabilityRequired(ECapabilityTrustedUI);
	}
	
void CSisHelperCapTest::ConstructL()
	{
	SetNameL(KSisHelperCapTestName);
	}

void CSisHelperCapTest::RunTestL()
	{
	Swi::RSisHelper sisHelper;
	CleanupClosePushL(sisHelper);
	
	// start sishelper
	StartSisHelperL();
	
	TInt err= CheckFailL(sisHelper.Connect(), _L("Connect"));
	
	if (KErrNone != err)
		{
		// if we get here we didn't connect and expected not to, but cannot carry on with tests
		CleanupStack::PopAndDestroy(&sisHelper);
		return;
		}

	
	TRAP(err, sisHelper.OpenDrmContentL(ContentAccess::TIntent(1)));
	CheckFailL(err, _L("OpenDrmContentL"));

	HBufC8* buf=0;
	TRAP(err, 
		{
		buf=sisHelper.SisControllerLC();
		CleanupStack::PopAndDestroy(buf);
		})
	CheckFailL(err, _L("SisControllerLC"));
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	fs.ShareProtected();
	
	RFile file;
	_LIT(KTempFilePath , "\\tswi\\swicaptests\\temp.file");
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	
	TBuf<128> tempFileOnSysDrive = sysDrive.Name();
	tempFileOnSysDrive.Append(KTempFilePath);
	User::LeaveIfError(file.Replace(fs, tempFileOnSysDrive, 0));
	CleanupClosePushL(file);
	RUiHandler ui;
	
	TRAP(err, sisHelper.ExtractFileL(fs, file, 0, 0, ui));
	CheckFailL(err, _L("ExtractFileL"));


	TRAP(err, sisHelper.SetupAsyncExtractionL(fs, file, 0, 0));
	CheckFailL(err, _L("SetupAsyncExtractionL"));

	TRequestStatus s;
	TRequestStatus &a=s;
	TRAP(err, sisHelper.AsyncExtractionL(3, a));
	User::WaitForRequest(a);
	err+=s.Int();
	CheckFailL(err, _L("AsyncExtractionL"));
	
	
	TRAP(err, sisHelper.EndAsyncExtractionL());
	CheckFailL(err, _L("EndAsyncExtractionL"));

	CleanupStack::PopAndDestroy(2, &fs);

	RArray<TChar> driveLetters;
	RArray<TInt64> driveSpaces;
	
	TRAP(err, sisHelper.FillDrivesAndSpacesL(driveLetters, driveSpaces));
	CheckFailL(err, _L("FillDrivesAndSpacesL"));


	TRAP(err, sisHelper.ExecuteDrmIntentL(ContentAccess::TIntent(1)));
	CheckFailL(err, _L("ExecuteDrmIntentL"));
	
	driveLetters.Close();
	driveSpaces.Close();

	CleanupStack::PopAndDestroy(&sisHelper);
	}


void CSisHelperCapTest::StartSisHelperL()
	{
	_LIT(KTestSisFile, "z:\\tswi\\tsis\\data\\cancel.sis");

	RProcess p;
	
	//Launching process
	User::LeaveIfError(p.Create(_L("sishelperstarter.exe"), KNullDesC));

	p.SetParameter(7, KTestSisFile);
	
	
	// Wait for the test to finish
	TRequestStatus s;
	TRequestStatus& a=s;
	p.Rendezvous(a);

	p.Resume();
	User::WaitForRequest(a);
	p.Close();
	}
