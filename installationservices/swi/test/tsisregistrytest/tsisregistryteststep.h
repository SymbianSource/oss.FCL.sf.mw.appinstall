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
_LIT(KCreateTokenByCopy, "CreateTokenByCopy");
_LIT(KCreateTokenFromStream, "CreateTokenFromStream");
_LIT(KCertificateChainListStep, "CertificateChainListStep");
_LIT(KCreateSisRegistryObject, "CreateSisRegistryObject");
_LIT(KSisRegistryDependencyStep, "SisRegistryDependencyStep");
_LIT(KSisRegistryFileDescStep, "SisRegistryFileDescStep");
_LIT(KControllerInfoStep, "ControllerInfoStep");
_LIT(KSisRegistryPropertyStep, "SisRegistryPropertyStep");
_LIT(KIsFileRegisteredStep, "IsFileRegistered");
_LIT(KGetComponentIdForUidStep, "GetComponentIdForUid");

// The following test steps added for performance tests.
// If they are needed to be used for other tests, they should be extended.
// For example, the retrieved results are compared with the expected ones. 
_LIT(KApplicationManagerStep, "ApplicationManager");
_LIT(KSidToPackage, "SidToPackage");
_LIT(KIsSidPresentSingleStep, "IsSidPresentSingle");
_LIT(KAddEntryStep, "AddEntry");
_LIT(KUpdateEntryStep, "UpdateEntry");
_LIT(KDeleteSingleEntry, "DeleteSingleEntry");
_LIT(KIsPackageExistInRomStep, "IsPackageExistInRom");
_LIT(KFileDescriptionsStep, "FileDescriptions");
_LIT(KIsAugmentationStep, "IsAugmentation");
_LIT(KFilesStep, "Files");
_LIT(KAugmentationsStep, "Augmentations");
_LIT(KPackageStep, "Package");
_LIT(KDependentPackagesStep, "DependentPackages");
_LIT(KDependenciesStep, "Dependencies");
_LIT(KEmbeddedPackagesStep, "EmbeddedPackages");
_LIT(KChangeLocale, "ChangeLocale");
_LIT(KCAddAppRegInfoStep,"AddAppRegInfo");
_LIT(KCRemoveAppRegInfoStep,"RemoveAppRegInfo");
_LIT(KCheckAppRegData,"CheckAppRegData");

// Base class for all steps
class CSisRegistryTestStepBase : public CTestStep
	{
public:
	void MarkAsPerformanceStep();
	
protected:
	// Extension of config parser to allow you to get array of descriptors
	void GetStringArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TPtrC>& aArray);
	TBool GetUidFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUid& aUid);
	void GetUidArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TUid>& aArray);
	void GetIntArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TInt>& aArray);
	HBufC8* GetControllerFromSisLC(const TDesC& aSisFileName);
	// Log function
	TVerdict PrintErrorAndReturnFailL(const TDesC& aMsg);
	// Performance test methods
	void StartTimer();
	void StopTimerAndPrintResultL();
	
private:	
	void PrintPerformanceLog(TTime aTime);
protected:
	// For some test cases (AddEntry, UpdateEntry and DeleteEntry), the time is measured externally
	// because they are performed via another helper server (sisregistryaccess_server). To eliminate
	// the time spent for IPC communication and the time spent to connect to the SISRegistry,
	// the actual test time is measured on the sisregistry_access server side and returned to the test code.
	TInt iTimeMeasuredExternally; 
private:
	TBool iIsPerformanceTest;
	TTime iStartTime;
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

