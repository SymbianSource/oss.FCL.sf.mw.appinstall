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


#ifndef __T_BACKUP_RESTORE_STEP_H__
#define __T_BACKUP_RESTORE_STEP_H__

#include <test/testexecutestepbase.h>
#include "tbackuprestore.h"

_LIT(KTStepInstall,					"INSTALL");
_LIT(KTStepUninstall,				"UNINSTALL");
_LIT(KTStepBackup,					"BACKUP");
_LIT(KTStepRestore,					"RESTORE");
_LIT(KTStepRestoreNoCommit,			"RESTORENOCOMMIT");
_LIT(KTStepRestoreCorruptController,"RESTORECORRUPTCONTROLLER");
_LIT(KTStepRestoreExtraExecutable,	"RESTOREEXTRAEXECUTABLE");
_LIT(KTStepCheckPresent,			"PRESENT");
_LIT(KTStepCheckAbsent,				"ABSENT");
_LIT(KTStepClean,					"CLEAN");


class CTBackupRestoreStepBase : public CTestStep
	{
protected:
	CTBackupRestoreStepBase();
	~CTBackupRestoreStepBase();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	// Iterates through SIS files within a step
	TBool	NextPackage();
	// The UID of the current SIS file
	TUid	PackageUID();
	// The filename of the current SIS file
	TPtrC	SISFileNameL();
	// The expected selected drive of the current package
	TChar	ExpectedDriveL();
	// Iterates through files within a SIS file
	TBool	NextFile();
	TBool	NextDataFile();
	TFileName	BackupFileName();
	// The location where the current file is [to be] installed
	TPtrC	InstalledFileName();
	TPtrC	DataFileName();
	// The location where the current file is not [to be] installed
	TPtrC	WrongInstalledFileName();
	// Utility method
	TBool	Exists(TPtrC aFileName);
	// Method for creating empty files
	void TouchL(const TDesC& aFileName);
	// Utility method
	void	CopyFileL(TPtrC aSourceFileName, TPtrC aDestFileName);
	// Utility method for storing backup metadata
	void	WriteMetaDataL(HBufC8* aMetaData);
	// Utility method for recovering backup metadata
	HBufC8*	ReadMetaDataL(TInt aBytesToCrop = 0);

	// test if we should write some garbage to the file, just test
	// if we can/can not restore a file that would fail its hash check
	TBool ModifyFile();

	// Get the length in bytes to crop from the end of the metadata on restore.
	TInt MetaDataCropLength();
	// utility method to Chek current step result against list of expected result codes.
	void CheckExpectedResultCodesL();
	
 	// Get the device supported languages from the config section.
 	//
	void GetDevSupportedLanguagesL(RArray<TInt>& aDeviceLanguages);

protected:
	//Added for devicesupported language PREQ2344
	TBool			iUseDeviceLanguages;		
	RArray<TInt>	iDeviceLanguages;			

private:
	TInt	iCurrentPackageNumber;
	TInt	iCurrentFileNumber;
	TInt	iDataFileNumber;
	RFs		iFs;
	};

class CTStepInstall : public CTBackupRestoreStepBase
	{
public:
	CTStepInstall();
	virtual TVerdict doTestStepL();
	};

class CTStepUninstall : public CTBackupRestoreStepBase
	{
public:
	CTStepUninstall();
	virtual TVerdict doTestStepL();
	};

class CTStepBackup : public CTBackupRestoreStepBase
	{
public:
	CTStepBackup();
	virtual TVerdict doTestStepL();
	};

class CTStepRestore : public CTBackupRestoreStepBase
	{
public:
	CTStepRestore();
	virtual TVerdict doTestStepL();
private:
	void RestoreL();
	};

class CTStepRestoreNoCommit : public CTBackupRestoreStepBase
	{
public:
	CTStepRestoreNoCommit();
	virtual TVerdict doTestStepL();
	};

class CTStepRestoreCorruptController : public CTBackupRestoreStepBase
	{
public:
	CTStepRestoreCorruptController();
	virtual TVerdict doTestStepL();
	};

class CTStepRestoreExtraExecutable : public CTBackupRestoreStepBase
	{
public:
	CTStepRestoreExtraExecutable();
	virtual TVerdict doTestStepL();
	};

class CTStepCheckPresent : public CTBackupRestoreStepBase
	{
public:
	CTStepCheckPresent();
	virtual TVerdict doTestStepL();
	};

class CTStepCheckAbsent : public CTBackupRestoreStepBase
	{
public:
	CTStepCheckAbsent();
	virtual TVerdict doTestStepL();
	};

class CTStepClean : public CTBackupRestoreStepBase
	{
public:
	CTStepClean();
	virtual TVerdict doTestStepL();
	};
#endif	/* __T_BACKUP_RESTORE_STEP_H__ */
