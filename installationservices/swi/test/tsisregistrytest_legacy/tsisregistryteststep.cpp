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
 
#include <test/testexecutelog.h>
#include "tsisregistryteststep.h"
#include "testutilclientswi.h"

/////////////////////////////////////////////////////////////////////
// defs, Constants used by test steps
/////////////////////////////////////////////////////////////////////

_LIT(KSisRegistryPath, "\\sys\\install\\sisregistry\\");
_LIT(KBackupPath, "\\sys\\install\\backup\\");
_LIT(KTestRegistryPath, "z:\\tswi\\tsisregistrytest\\data\\");
_LIT(KSaveRegistryPath, "\\tswi\\tsisregistrytest\\generated\\");


_LIT(KKeyFormat, "-%02d");

/////////////////////////////////////////////////////////////////////
// Tags used by test steps
/////////////////////////////////////////////////////////////////////

void CSisRegistryTestStepBase::GetStringArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TPtrC>& aArray)
	{
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
			TInt error = aArray.Append(val);

			INFO_PRINTF2(_L("String name: %S"), &val);

			User::LeaveIfError(error);
			}
		} while (cont);

	INFO_PRINTF2(_L("Element count: %d"), i-1);
	CleanupStack::PopAndDestroy(buf);
	}

TBool CSisRegistryTestStepBase::GetUidFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUid& aUid)
	{
	TInt val;
	if(GetHexFromConfig(aSectName, aKeyName, val))
		{
		aUid = TUid::Uid(val);
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

void CSisRegistryTestStepBase::GetUidArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TUid>& aArray)
	{
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
		TUid uid;

		cont = GetUidFromConfig(aSectName, ptr, uid);
		if (cont)
			{
			User::LeaveIfError(aArray.Append(uid));
			}
		} while (cont);

	INFO_PRINTF2(_L("Element count: %d"), i-1);
	CleanupStack::PopAndDestroy(buf);
	}

void CSisRegistryTestStepBase::GetIntArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TInt>& aArray)
	{
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
		TInt val;

		cont = GetIntFromConfig(aSectName, ptr, val);
		if (cont)
			{
			User::LeaveIfError(aArray.Append(val));
			}
		} while (cont);

	INFO_PRINTF2(_L("Element count: %d"), i-1);
	CleanupStack::PopAndDestroy(buf);
	}

/////////////////////////////////////////////////////////////////////
// CBackupRegistryStep
/////////////////////////////////////////////////////////////////////
CBackupRegistryStep::CBackupRegistryStep()
	{
	SetTestStepName(KBackupRegistry);
	}

TVerdict CBackupRegistryStep::doTestStepL()
	{
	RTestUtilSessionSwi util;
	User::LeaveIfError(util.Connect());
	CleanupClosePushL(util);
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
		
	TBuf<128> backUpPath (sysDriveName);
	backUpPath.Append(KBackupPath);

	(void) util.RmDir(backUpPath);

	TBuf<128> sisRegistryPath (sysDriveName);
	sisRegistryPath.Append(KSisRegistryPath);
	TInt err = util.Move(sisRegistryPath, backUpPath);
	if(err != KErrNone && err != KErrPathNotFound && err != KErrNotFound)
		{
		User::Leave(err);
		}

	CleanupStack::PopAndDestroy(&util);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////
// CCreateTestRegistryStep
/////////////////////////////////////////////////////////////////////
CCreateTestRegistryStep::CCreateTestRegistryStep()
	{
	SetTestStepName(KCreateTestRegistry);
	}

TVerdict CCreateTestRegistryStep::doTestStepL()
	{
	RTestUtilSessionSwi util;
	User::LeaveIfError(util.Connect());
	CleanupClosePushL(util);
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> sisRegistryPath (sysDrive.Name());
	sisRegistryPath.Append(KSisRegistryPath);

	User::LeaveIfError(util.Copy(KTestRegistryPath, sisRegistryPath));

	CleanupStack::PopAndDestroy(&util);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////
// CDeleteRegistryStep
/////////////////////////////////////////////////////////////////////
CDeleteRegistryStep::CDeleteRegistryStep()
	{
	SetTestStepName(KDeleteRegistry);
	}

TVerdict CDeleteRegistryStep::doTestStepL()
	{
	RTestUtilSessionSwi util;
	User::LeaveIfError(util.Connect());
	CleanupClosePushL(util);
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> sisRegistryPath (sysDrive.Name());
	sisRegistryPath.Append(KSisRegistryPath);

	TInt err = util.RmDir(sisRegistryPath);
	if(err != KErrNone && err != KErrPathNotFound && err != KErrNotFound)
		{
		User::Leave(err);
		}
	
	CleanupStack::PopAndDestroy(&util);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////
// CRestoreRegistryStep
/////////////////////////////////////////////////////////////////////
CRestoreRegistryStep::CRestoreRegistryStep()
	{
	SetTestStepName(KRestoreRegistry);
	}

TVerdict CRestoreRegistryStep::doTestStepL()
	{
	RTestUtilSessionSwi util;
	User::LeaveIfError(util.Connect());
	CleanupClosePushL(util);
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());

	TBuf<128> backUpPath (sysDriveName);
	backUpPath.Append(KBackupPath);
	
	TBuf<128> sisRegistryPath (sysDriveName);
	sisRegistryPath.Append(KSisRegistryPath);
	
	TInt err = util.Move(backUpPath, sisRegistryPath);
	if(err != KErrNone && err != KErrPathNotFound && err != KErrNotFound)
		{
		User::Leave(err);
		}
	
	CleanupStack::PopAndDestroy(&util);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////
// CSaveRegistryStep
/////////////////////////////////////////////////////////////////////
CSaveRegistryStep::CSaveRegistryStep()
	{
	SetTestStepName(KSaveGeneratedRegistry);
	}

TVerdict CSaveRegistryStep::doTestStepL()
	{
	RTestUtilSessionSwi util;
	User::LeaveIfError(util.Connect());
	CleanupClosePushL(util);
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName(sysDrive.Name());
	
	TFileName saveRegistryPath (sysDriveName);
	saveRegistryPath.Append(KSaveRegistryPath);

	TInt err = util.RmDir(saveRegistryPath);
	TFileName sisRegistryPath (sysDriveName);
	sisRegistryPath.Append(KSisRegistryPath);
	
	User::LeaveIfError(util.Copy(sisRegistryPath, saveRegistryPath));
	
	CleanupStack::PopAndDestroy(&util);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

