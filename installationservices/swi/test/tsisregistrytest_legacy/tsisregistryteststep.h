/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __TSISREGISTRYSTEP_H__
#define __TSISREGISTRYSTEP_H__

#include <test/testexecutestepbase.h>

// Step names
_LIT(KSetCenRepSetting, "SetCenRepSetting");
_LIT(KInstalledUids, "InstalledUids");
_LIT(KInstalledPackages, "InstalledPackages");
_LIT(KRemovablePackages, "RemovablePackages");
_LIT(KInRomNonRemovablePackages, "InRomNonRemovablePackages");
_LIT(KInstalledDrives, "InstalledDrives");
_LIT(KIsUidInstalled, "IsUidInstalled");
_LIT(KIsPackageInstalled, "IsPackageInstalled");
_LIT(KIsControllerInstalled, "IsControllerInstalled");
_LIT(KGetControllers, "GetControllers");
_LIT(KEntryUidOpenClose, "EntryUidOpenClose");
_LIT(KEntryNameOpenClose, "EntryNameOpenClose");
_LIT(KEntryPackageOpenClose, "EntryPackageOpenClose");
_LIT(KEntryMethods, "EntryMethods");
_LIT(KWritableEntryMethods, "WritableEntryMethods");
_LIT(KPoliceSids,"PoliceSids");
_LIT(KEntrySids, "EntrySids");
_LIT(KEntryDelete, "EntryDelete");
_LIT(KBackupRegistry, "BackupRegistry");
_LIT(KCreateTestRegistry, "CreateTestRegistry");
_LIT(KSaveGeneratedRegistry, "SaveGeneratedRegistry");
_LIT(KDeleteRegistry, "DeleteRegistry");
_LIT(KRestoreRegistry, "RestoreRegistry");
_LIT(KSidFileName, "SidToFileName");
_LIT(KModifiableFile, "ModifiableFile");
_LIT(KFileHash, "FileHash");
_LIT(KIsSidPresentStep, "IsSidPresent");
_LIT(KIsPackagePresentStep, "IsPackagePresent");
_LIT(KIntegrityCheckStep, "IntegrityCheck");
_LIT(KDateAndTimeIntegrityCheckStep, "DateAndTimeIntegrityCheck");
_LIT(KLoggingFileInfo, "LoggingFileInfo");
_LIT(KRomStubPerformanceStep, "RomStubPerformanceCheck");
_LIT(KCorruptCacheStep, "CorruptCacheStep");
_LIT(KCorruptCacheRecoverStep, "CorruptCacheRecoverStep");
_LIT(KEmbeddingPackageStep, "EmbeddingPackageStep");
_LIT(KChangeLocale, "ChangeLocale");
_LIT(KRegistryFiles, "RegistryFiles");

// Base class for all steps
class CSisRegistryTestStepBase : public CTestStep
	{
protected:
	// Extension of config parser to allow you to get array of descriptors
	void GetStringArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TPtrC>& aArray);
	TBool GetUidFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUid& aUid);
	void GetUidArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TUid>& aArray);
	void GetIntArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TInt>& aArray);
	};

// Backup Registry Step
class CBackupRegistryStep : public CSisRegistryTestStepBase
	{
public:
	CBackupRegistryStep();
	virtual TVerdict doTestStepL();
	};

// Create Test Registry Step
class CCreateTestRegistryStep : public CSisRegistryTestStepBase
	{
public:
	CCreateTestRegistryStep();
	virtual TVerdict doTestStepL();
	};

// Delete Registry Step
class CDeleteRegistryStep : public CSisRegistryTestStepBase
	{
public:
	CDeleteRegistryStep();
	virtual TVerdict doTestStepL();
	};

// Restore Registry Step
class CRestoreRegistryStep : public CSisRegistryTestStepBase
	{
public:
	CRestoreRegistryStep();
	virtual TVerdict doTestStepL();
	};

// Save Registry Step
class CSaveRegistryStep : public CSisRegistryTestStepBase
	{
public:
	CSaveRegistryStep();
	virtual TVerdict doTestStepL();
	};


#endif

