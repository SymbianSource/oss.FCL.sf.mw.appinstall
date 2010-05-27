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
#include "filesisdataprovider.h"
#include "siscontents.h"
#include "sisparser.h"

/////////////////////////////////////////////////////////////////////
// defs, Constants used by test steps
/////////////////////////////////////////////////////////////////////

_LIT(KSisRegistryPath, "\\sys\\install\\sisregistry\\");
_LIT(KBackupPath, "\\sys\\install\\backup\\");
_LIT(KTestRegistryPath, "z:\\tswi\\tsisregistrytest\\data\\");
_LIT(KSaveRegistryPath, "\\tswi\\tsisregistrytest\\generated\\");

_LIT(KScrDbFileName, "scr.db");
_LIT(KScrDbFilePath, "\\sys\\install\\scr\\");

_LIT(KKeyFormat, "-%02d");

/////////////////////////////////////////////////////////////////////
// Tags used by test steps
/////////////////////////////////////////////////////////////////////

void CSisRegistryTestStepBase:: MarkAsPerformanceStep()
	{
	iIsPerformanceTest = ETrue;
	}

void CSisRegistryTestStepBase::PrintPerformanceLog(TTime aTime)
	{
	_LIT(KPerformanceTestInfo, "PERFORMANCE_LOG_INFORMATION");
	TDateTime timer = aTime.DateTime();
	INFO_PRINTF6(_L("%S,%d:%d:%d:%d"), &KPerformanceTestInfo(), timer.Hour(), timer.Minute(), timer.Second(), timer.MicroSecond());
	}

TVerdict CSisRegistryTestStepBase::PrintErrorAndReturnFailL(const TDesC& aMsg)
	{
	ERR_PRINTF1(aMsg);
	SetTestStepResult(EFail);
	return TestStepResult();
	}

void CSisRegistryTestStepBase::StartTimer()
	{
	if(!iIsPerformanceTest)
		return; // Do nothing if not performance test
	iStartTime.HomeTime();
	PrintPerformanceLog(iStartTime);
	}

void CSisRegistryTestStepBase::StopTimerAndPrintResultL()
	{
	if(!iIsPerformanceTest)
		return; // Do nothing if not performance test
	
	TTime endTime;
	endTime.HomeTime();
	PrintPerformanceLog(endTime);
	
	TTimeIntervalMicroSeconds duration = endTime.MicroSecondsFrom(iStartTime);
	TInt actualDuration = I64INT(duration.Int64())/1000; // in millisecond
	
	if(iTimeMeasuredExternally)
		{ // if the time has been measured externally, update the actual duration value with that.
		actualDuration = iTimeMeasuredExternally;
		}
	
	// Performance related names
	_LIT(KMaxDurationName, "MaxDuration");
	_LIT(KMaxTestCaseDuration, "TEST_CASE_MAXIMUM_ALLOWED_DURATION");
	_LIT(KActualTestCaseDuration, "TEST_CASE_ACTUAL_DURATION");
	
	TInt maxDuration = 0;
	if(!GetIntFromConfig(ConfigSection(), KMaxDurationName, maxDuration))
		{
		ERR_PRINTF2(_L("%S could not be found in configuration."), &KMaxDurationName());
		User::Leave(KErrNotFound);
		}
	else
		{
		INFO_PRINTF3(_L("%S,%d"), &KMaxTestCaseDuration(), maxDuration);
		INFO_PRINTF3(_L("%S,%d"), &KActualTestCaseDuration(), actualDuration);
		}
	
	if(actualDuration <= maxDuration)
		{
		INFO_PRINTF2(_L("This test meets performance requirement (Duration=%d)."), actualDuration);
		}
	else
		{
		ERR_PRINTF2(_L("This test does not meet performance requirement (Duration=%d)."), actualDuration);
		SetTestStepResult(EFail);
		}
	}

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

HBufC8* CSisRegistryTestStepBase::GetControllerFromSisLC(const TDesC& aSisFileName)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile file;
	User::LeaveIfError(file.Open(fs, aSisFileName, EFileRead));
	CleanupClosePushL(file);

	// Create a file data provider
	MSisDataProvider* dataProvider = CFileSisDataProvider::NewLC(file);

	// Extract raw controller data into a buffer
	TInt64 pos(0);
	User::LeaveIfError(dataProvider->Seek(ESeekStart, pos));
	Sis::CContents *contents = Sis::Parser::ContentsL(*dataProvider);
	CleanupStack::PushL(contents);
	HBufC8* ret = contents->ReadControllerL();
	CleanupStack::PopAndDestroy(4, &fs); //contents, dataProvier, file
	CleanupStack::PushL(ret);
	return ret;
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
		
	TBuf<128> backupDbPath(sysDriveName);
	backupDbPath.Append(KBackupPath);
	backupDbPath.Append(KScrDbFileName);
	
	TBuf<128> scrDbPath(sysDriveName);
	scrDbPath.Append(KScrDbFilePath);
	scrDbPath.Append(KScrDbFileName);
			
	// Copy the current SCR in to backup location
	err = util.Move(scrDbPath, backupDbPath);
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
		
	TBuf<128> emptyDbPath(KTestRegistryPath);
	emptyDbPath.Append(KScrDbFileName);
	
	TBuf<128> scrDbPath(sysDrive.Name());
	scrDbPath.Append(KScrDbFilePath);
	scrDbPath.Append(KScrDbFileName);
	
	TBuf<128> exesDbFile(sysDrive.Name());
	exesDbFile.Append(KSisRegistryPath);
	exesDbFile.Append(KScrDbFileName);
	
	// Remove the extra SCR DB file copied from KTestRegistryPath to sisRegistryPath
	util.Delete(exesDbFile);
			
	// Copy the empty SCR DB in place
	User::LeaveIfError(util.Copy(emptyDbPath, scrDbPath));
	
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
	// Wait up to 30 seconds to ensure both the sisregistry server and SCRServer
	// have shut down
	_LIT(KSisRegistryServerName, "!SisRegistryServer");
	_LIT(KScrServerName, "!ScrServer");
	TInt delaytime = 30; 

	while (delaytime-- > 0)
		{
		TFullName serverName;
		TFindServer find(KSisRegistryServerName);
		if (KErrNotFound == find.Next(serverName))
			{
			find.Find(KScrServerName);
			if (KErrNotFound == find.Next(serverName))
				{
				break;
				}
			}
		User::After(1000000); // wait a second until the next test
		}
		
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
			
	TBuf<128> scrDbPath(sysDrive.Name());
	scrDbPath.Append(KScrDbFilePath);
	scrDbPath.Append(KScrDbFileName);
			
	// Delete the current SCR DB
	err = util.Delete(scrDbPath);
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
		
	TBuf<128> backupDbPath(sysDriveName);
	backupDbPath.Append(KSisRegistryPath);
	backupDbPath.Append(KScrDbFileName);
	
	TBuf<128> scrDbPath(sysDriveName);
	scrDbPath.Append(KScrDbFilePath);
	scrDbPath.Append(KScrDbFileName);

	// SCR DB already copied from backup path to sisregistrypath,
	// so, copy the file from there to SCR path.
	err = util.Move(backupDbPath, scrDbPath);
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
	// Wait up to 30 seconds to ensure both the sisregistry server and SCRServer
	// have shut down
	_LIT(KSisRegistryServerName, "!SisRegistryServer");
	_LIT(KScrServerName, "!ScrServer");
	TInt delaytime = 30; 

	while (delaytime-- > 0)
		{
		TFullName serverName;
		TFindServer find(KSisRegistryServerName);
		if (KErrNotFound == find.Next(serverName))
			{
			find.Find(KScrServerName);
			if (KErrNotFound == find.Next(serverName))
				{
				break;
				}
			}
		User::After(1000000); // wait a second until the next test
		}
	
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
		
	TBuf<128> dbSavePath(sysDriveName);
	dbSavePath.Append(KSaveRegistryPath);
	dbSavePath.Append(KScrDbFileName);
	
	TBuf<128> scrDbPath(sysDriveName);
	scrDbPath.Append(KScrDbFilePath);
	scrDbPath.Append(KScrDbFileName);

	// Save the SCR DB to the saving location
	User::LeaveIfError(util.Copy(scrDbPath, dbSavePath));
	
	CleanupStack::PopAndDestroy(&util);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}
