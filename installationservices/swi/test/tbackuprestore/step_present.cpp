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


#include "tbackuprestorestep.h"
#include <test/testexecutelog.h>

#include <f32file.h>
#include "swi/sisregistrysession.h"
#include "sisregistrywritableentry.h"


CTStepCheckPresent::CTStepCheckPresent()
	{
	SetTestStepName(KTStepCheckPresent);
	}

TVerdict CTStepCheckPresent::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	Swi::RSisRegistrySession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);

	while (NextPackage())
		{
		TBool installed = session.IsInstalledL(PackageUID());
	
		if (!installed)
			{
			INFO_PRINTF2(_L("Package %d is not present"), PackageUID().iUid);
			SetTestStepResult(EFail);
			}
		else
			{
			// If the selected drive is specified in the config section
			// we need to check it.
			TUint expectedDrive(0);
			TRAPD(err, expectedDrive = ExpectedDriveL())
			if (err == KErrNone)
				{
				Swi::RSisRegistryWritableEntry registryEntry;
				TInt error=registryEntry.Open(session, PackageUID());
				CleanupClosePushL(registryEntry);
				if (err == KErrNone)
					{
					TUint actualDrive = registryEntry.SelectedDriveL();
					if (expectedDrive != actualDrive)
						{
						ERR_PRINTF3(_L("Actual selected drive %c does not match expected drive %c"),
									actualDrive, expectedDrive);
						SetTestStepResult(EFail);
						}
					}
				CleanupStack::PopAndDestroy(&registryEntry);
				}
			}

		while (NextFile())
			{
			if (!Exists(InstalledFileName()))
				{
				TPtrC filename =  InstalledFileName();
				INFO_PRINTF2(_L("Entry of %S failed"), &filename);
				SetTestStepResult(EFail);
				}
			else
				{
				// the file is present, check if it should have a hash or not
				// and if it is or is not present.
				_LIT(KBinPath, "\\sys\\bin\\");
				
				TPtrC filename =  InstalledFileName();
				TParsePtrC parse(filename);
				TParse hashFile;
				TDriveUnit sysDrive(RFs::GetSystemDrive());
				TBuf<64> hashPath (sysDrive.Name());
				_LIT(KHashPath, "\\sys\\hash\\");
				hashPath.Append(KHashPath);
				hashFile.Set(hashPath, &parse.FullName(), NULL);
				
				if (parse.Path().CompareF(KBinPath) == 0)
					{
					// hash should exist.
					if (!Exists(hashFile.FullName()))
						{
						INFO_PRINTF2(_L("Hash file %S should exist, but doesn't."), &hashFile.FullName());
						SetTestStepResult(EFail);
						}
					}
				else
					{
					// hash should not exist.
					if (Exists(hashFile.FullName()))
						{
						INFO_PRINTF2(_L("Hash file %S shouldn't exist, but does."), &hashFile.FullName());
						SetTestStepResult(EFail);
						}
					}
				}
			}
			
		// check the data files exist too
		while (NextDataFile())
			{
			if (!Exists(DataFileName()))
				{
				TPtrC filename = DataFileName();
				INFO_PRINTF2(_L("Entry of %S failed"), &filename);
				SetTestStepResult(EFail);
				}
			}
			
		}
	
	CleanupStack::PopAndDestroy(&session);	
	return TestStepResult();
	}
