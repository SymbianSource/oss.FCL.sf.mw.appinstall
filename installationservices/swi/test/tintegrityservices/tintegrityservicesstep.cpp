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


/**
 @file 
 @test
 @internalTechnology
*/
 
#include "tintegrityservicesstep.h"
#include "dummyintegrityservices.h"
#include <test/testexecutelog.h>
#include <s32file.h>
#include "testutilclientswi.h"


/////////////////////////////////////////////////////////////////////
// Tags used by test steps
/////////////////////////////////////////////////////////////////////

_LIT(KRemoveFile, "removefile");
_LIT(KTempFile, "temporaryfile");
_LIT(KAddFile, "addfile");
_LIT(KFailType, "failtype");
_LIT(KFailPosition, "failposition");
_LIT(KFailFileName, "failfilename");
_LIT(KPresent, "present");
_LIT(KAbsent, "absent");
_LIT(KCleanupFile, "cleanupfile");
_LIT(KCleanupDirectory, "cleanupdirectory");
_LIT(KArbitraryOpFile, "arbitraryopfile");
_LIT(KOperation, "operation");

using namespace Swi;
using namespace Swi::Test;

/////////////////////////////////////////////////////////////////////
// CIntegrityServicesStep
/////////////////////////////////////////////////////////////////////
TVerdict CIntegrityServicesStepBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	// Install an active scheduler
	CActiveScheduler* s = new (ELeave) CActiveScheduler;
	s->Install(s);
	
	User::LeaveIfError(iIntegritySession.Connect());

	ReadFailureSettings();
	return TestStepResult();
	}

TVerdict CIntegrityServicesStepBase::doTestStepPostambleL()
	{
	// Remove the installed active scheduler
	CActiveScheduler* s = CActiveScheduler::Current();
	s->Install(NULL);
	delete s;

	iIntegritySession.Close();

	__UHEAP_MARKEND;

	return TestStepResult();
	}

void CIntegrityServicesStepBase::GetStringArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TPtrC>& aArray)
	{
	_LIT(KKeyFormat, "-%02d");
	HBufC* buf = HBufC::NewLC(aKeyName.Length() + KKeyFormat().Length());
	TPtr ptr(buf->Des());
	INFO_PRINTF2(_L("Parsing attribute: %S"), &aKeyName);

	TInt i = 0;
	TBool cont = ETrue;
	do
		{
		++i;
		ptr = aKeyName;
		ptr.AppendFormat(KKeyFormat(), i);
		TPtrC val;

		cont = GetStringFromConfig(aSectName, ptr, val);
		if (cont)
			{
			User::LeaveIfError(aArray.Append(val));
			}
		} while (cont);

	INFO_PRINTF2(_L("Element count: %d"), i-1);
	CleanupStack::PopAndDestroy(buf);
	}

TBool CIntegrityServicesStepBase::CheckFilesL()
	{
	TInt result = ETrue;
	RTestUtilSessionSwi testutil;
	User::LeaveIfError(testutil.Connect());
	CleanupClosePushL(testutil);
	RArray<TPtrC> fileArray;
	CleanupClosePushL(fileArray);
	
	GetStringArrayFromConfigL(ConfigSection(), KPresent, fileArray);
	for (TInt file = 0; file < fileArray.Count();file++)
		{
		if(testutil.FileExistsL(fileArray[file]))
			{
			INFO_PRINTF2(_L("%S was found"), &fileArray[file]);
			}
		else
			{
			ERR_PRINTF2(_L("%S was not found"), &fileArray[file]);
			result = EFalse;
			}
		}
	fileArray.Reset();
	
	GetStringArrayFromConfigL(ConfigSection(), KAbsent, fileArray);
	for (TInt file = 0; file < fileArray.Count();file++)
		{
		if(testutil.FileExistsL(fileArray[file]))
			{
			ERR_PRINTF2(_L("%S was found"), &fileArray[file]);
			result = EFalse;
			}
		else
			{
			INFO_PRINTF2(_L("%S was not found"), &fileArray[file]);
			}
		}
	fileArray.Reset();
	CleanupStack::PopAndDestroy(&fileArray);
	
	CleanupStack::PopAndDestroy(&testutil);
	return result;
	}

void CIntegrityServicesStepBase::ReadFailureSettings()
	{		
	TPtrC failType;
	if(!GetStringFromConfig(ConfigSection(), KFailType, failType))
		{
		failType.Set(KNullDesC);
		}
		
	TPtrC failPosition;
	if(!GetStringFromConfig(ConfigSection(), KFailPosition, failPosition))
		{
		failPosition.Set(KNullDesC);
		}
		
	TPtrC failFileName;
	if(!GetStringFromConfig(ConfigSection(), KFailFileName, failFileName))
		{
		failFileName.Set(KNullDesC);
		}
	
	iIntegritySession.SetSimulatedFailure(failType, failPosition, failFileName);
	}
	
void CIntegrityServicesStepBase::doInstallL()
	{
	RArray<TPtrC> fileArray;
	CleanupClosePushL(fileArray);
	
	GetStringArrayFromConfigL(ConfigSection(), KRemoveFile, fileArray);
	for (TInt file = 0; file < fileArray.Count();file++)
		{
		INFO_PRINTF2(_L("RemovingFile: %S"), &fileArray[file]);
		
		iIntegritySession.RemoveL(fileArray[file]);
		}
	fileArray.Reset();
	
	
	GetStringArrayFromConfigL(ConfigSection(), KAddFile, fileArray);
	for (TInt file = 0; file < fileArray.Count();file++)
		{
		INFO_PRINTF2(_L("AddingFile: %S"), &fileArray[file]);
		iIntegritySession.AddL(fileArray[file]);
		iIntegritySession.CreateNewTestFileL(fileArray[file]);
		}
	fileArray.Reset();

	GetStringArrayFromConfigL(ConfigSection(), KTempFile, fileArray);
	for (TInt file = 0; file < fileArray.Count();file++)
		{
		INFO_PRINTF2(_L("TemporaryFile: %S"), &fileArray[file]);
		iIntegritySession.TemporaryL(fileArray[file]);
		iIntegritySession.CreateTempTestFileL(fileArray[file]);
		}
	fileArray.Reset();

	RArray<TPtrC> operationArray;
	CleanupClosePushL(operationArray);
	
	GetStringArrayFromConfigL(ConfigSection(), KArbitraryOpFile, fileArray);
	GetStringArrayFromConfigL(ConfigSection(), KOperation, operationArray);
	TInt fileCount = fileArray.Count();
	
	if (fileCount != operationArray.Count())
		{
		ERR_PRINTF3(_L("Number of files (%d) does not match number of operations (%d)"), fileCount, operationArray.Count());
		SetTestStepResult(EFail);
		}
	else
		{
		for (TInt file = 0; file < fileCount;file++)
			{
			if (operationArray[file].CompareF(KAddFile) == 0)
				{
				INFO_PRINTF2(_L("AddingFile: %S"), &fileArray[file]);
				iIntegritySession.AddL(fileArray[file]);
				iIntegritySession.CreateNewTestFileL(fileArray[file]);
				}
			else if (operationArray[file].CompareF(KRemoveFile) == 0)
				{
				INFO_PRINTF2(_L("RemovingFile: %S"), &fileArray[file]);
				iIntegritySession.RemoveL(fileArray[file]);
				}
			else if (operationArray[file].CompareF(KTempFile) == 0)
				{
				INFO_PRINTF2(_L("TemporaryFile: %S"), &fileArray[file]);
				iIntegritySession.TemporaryL(fileArray[file]);
				iIntegritySession.CreateTempTestFileL(fileArray[file]);
				}
			else
				{
				ERR_PRINTF3(_L("Operation %S not understood, skipping file %S"), &operationArray[file], &fileArray[file]);
				}
			}
		}
	CleanupStack::PopAndDestroy(2, &fileArray);
	
	iIntegritySession.CommitL();
	}

void CIntegrityServicesStepBase::doRecoverL()
	{
	INFO_PRINTF1(_L("CIntegrityServicesStepBase::doRecoverL"));
	iIntegritySession.RollBackL(EFalse);
	}

void CIntegrityServicesStepBase::doCleanupL()
	{
	RArray<TPtrC> fileArray;
	CleanupClosePushL(fileArray);
	
	RTestUtilSessionSwi testutil;
	User::LeaveIfError(testutil.Connect());
	CleanupClosePushL(testutil);
	
	GetStringArrayFromConfigL(ConfigSection(), KCleanupFile, fileArray);
	for (TInt file = 0; file < fileArray.Count();file++)
		{
		TInt err = testutil.Delete(fileArray[file]);
		if(err != KErrNone && err != KErrPathNotFound && err != KErrNotFound)
			{
			User::Leave(err);
			}
		}
	fileArray.Reset();

	GetStringArrayFromConfigL(ConfigSection(), KCleanupDirectory, fileArray);
	for (TInt file = 0; file < fileArray.Count();file++)
		{
		TInt err = testutil.RmDir(fileArray[file]);
		if(err != KErrNone && err != KErrPathNotFound && err != KErrNotFound)
			{
			User::Leave(err);
			}
		}
	fileArray.Reset();

	CleanupStack::PopAndDestroy(&testutil);
	CleanupStack::PopAndDestroy(&fileArray);
	}

/////////////////////////////////////////////////////////////////////
// CInstallStep
/////////////////////////////////////////////////////////////////////
CInstallStep::CInstallStep()
	{
	SetTestStepName(KInstall);
	}

TVerdict CInstallStep::doTestStepL()
	{
	TRAPD(err, doInstallL());
	
	if(err == KErrNone || err == KIntegrityServicesSimulatedBatteryFailure)
		{
		if(CheckFilesL())
			{
			SetTestStepResult(EPass);
			}
		else
			{
			SetTestStepResult(EFail);
			}
		}
	else
		{
		User::Leave(err);
		}
	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////
// CRecoverStep
/////////////////////////////////////////////////////////////////////
CRecoverStep::CRecoverStep()
	{
	SetTestStepName(KRecover);
	}

TVerdict CRecoverStep::doTestStepL()
	{
	// begin recovery
	TRAPD(err, doRecoverL());
	
	if(err == KErrNone || err == KIntegrityServicesSimulatedBatteryFailure)
		{
		if(CheckFilesL())
			{
			SetTestStepResult(EPass);
			}
		else
			{
			SetTestStepResult(EFail);
			}
		}
	else
		{
		User::Leave(err);
		}

	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////
// CCleanupStep
/////////////////////////////////////////////////////////////////////
CCleanupStep::CCleanupStep()
	{
	SetTestStepName(KCleanup);
	}

TVerdict CCleanupStep::doTestStepL()
	{
	doCleanupL();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////
// COOMStep
/////////////////////////////////////////////////////////////////////
COOMStep::COOMStep()
	{
	SetTestStepName(KOOM);
	}

TVerdict COOMStep::doTestStepL()
	{
	const TInt maxFailCount = 200;

	__UHEAP_MARK;

	TInt installFailCount = 0;
	TInt installResult = KErrNoMemory;

	while(installResult != KErrNone && installFailCount < maxFailCount)
		{
		INFO_PRINTF2(_L("OOM InstallFailCount: %d"), installFailCount);
				
		__UHEAP_MARK;
		
		__UHEAP_FAILNEXT(installFailCount);

		TRAP(installResult, doInstallL());
		
		__UHEAP_MARKEND;

		__UHEAP_RESET;
		
		if(installResult == KErrNone)
			{
			if(CheckFilesL())
				{
				SetTestStepResult(EPass);
				}
			else
				{
				SetTestStepResult(EFail);
				}

			doCleanupL();
			}
		else if(installResult == KErrNoMemory)
			{
			// recover any partial installation, again testing oom
			TInt recoverFailCount = 0;
			TInt recoverResult = KErrNoMemory;
			
			while(recoverResult != KErrNone && recoverFailCount < maxFailCount)
				{
				INFO_PRINTF2(_L("OOM RecoverFailCount: %d"), recoverFailCount);
		
				__UHEAP_MARK;
		
				__UHEAP_FAILNEXT(recoverFailCount);

				TRAP(recoverResult, doRecoverL());
			
				__UHEAP_MARKEND;

				__UHEAP_RESET;
		
				// Fail further into the test
				recoverFailCount++;
				}
			}
		else
			{
			User::Leave(installResult);
			}
		
		
		// Fail further into the test
		installFailCount++;
		}
	__UHEAP_MARKEND;
				
	SetTestStepResult(EPass);
	return TestStepResult();
	}
	
/////////////////////////////////////////////////////////////////////
// CRecoverStep
/////////////////////////////////////////////////////////////////////
CCheckStep::CCheckStep()
	{
	SetTestStepName(KCheck);
	}

TVerdict CCheckStep::doTestStepL()
	{
	// Just check for files
	if(CheckFilesL())
		{
		SetTestStepResult(EPass);
		}
	else
		{
		SetTestStepResult(EFail);
		}
		
	return TestStepResult();
	}
