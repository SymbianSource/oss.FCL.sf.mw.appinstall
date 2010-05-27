/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "trevocationStep.h"
#include <test/testexecutelog.h>

#include <f32file.h>

#include <swi/launcher.h>
#include "tui.h"

CTStepInstall::CTStepInstall()
	{
	SetTestStepName(KTStepInstall);
	}

TVerdict CTStepInstall::doTestStepL()
	{
	//If any test step leaves or panics, the test step thread exits and no further calls are made. 
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
__UHEAP_MARK;		// Check for memory leaks
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	fs.ShareProtected();

	
	//Modified to cater for situations where the user chooses to activate the 
	//revocation checking facility
	
	TBool performRevocationCheck;		// Determines the setting of the 'PerformRevocationCheck' value
	TPtrC ocsp_uriValue;	
	const TUint KMaxUriLength=512;
	TBuf8<KMaxUriLength> tempUriValue;
	//_LIT8(KOcspServerDefaultAddress, "cam-ocsptest01.intra:19003");	
	
	CInstallPrefs* prefs = CInstallPrefs::NewLC();
	
	//Check to see if the user specified a pref
	if (GetBoolFromConfig(ConfigSection(),_L("PerformRevocationCheck"), performRevocationCheck) == EFalse) // the tag was not found
		{
		performRevocationCheck = EFalse;	//Default situation
		}
	
	prefs->SetPerformRevocationCheck(performRevocationCheck);
	
	//Check to see if the user specified an ocsp uri value
	if (GetStringFromConfig(ConfigSection(),_L("ocsp_uri"), ocsp_uriValue) != EFalse) 
		{		
		// the tag 'ocsp_Uri' was found
		
		tempUriValue.Copy(ocsp_uriValue);
		prefs -> SetRevocationServerUriL(tempUriValue);
		}
	//Else use default values
	
		
	TInt expectedResult;
	if (GetIntFromConfig(ConfigSection(),_L("InstallResult"), expectedResult) == EFalse)
		{
		expectedResult = KErrNone;
		}
			
	while (NextPackage())
		{
		TPtrC sisfilename(SISFileNameL());
		//INFO_PRINTF2(_L("Installing %S"), &sisfilename);
		
		TUI ui;
		RFile file;
		User::LeaveIfError(file.Open(fs, sisfilename, EFileRead));

		TInt err = Launcher::Install (ui, file, *prefs);

		if (err != expectedResult)
			{
			INFO_PRINTF3(_L("Expected %d, got %d"), expectedResult, err);
			SetTestStepResult(EFail);
			}
		}
	CleanupStack::PopAndDestroy(2, &fs); // prefs
	
	// Wait for SIS helper to shutdown
	User::After(10000000); 
	
__UHEAP_MARKEND;	
	return TestStepResult();
	}

