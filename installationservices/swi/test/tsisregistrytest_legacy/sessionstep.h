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
#ifndef __TSESSIONSTEP_H__
#define __TSESSIONSTEP_H__

#include "tsisregistryteststep.h"
#include "sisregistrysession.h"
#include "sisregistryentry.h"
#include <x509cert.h>

using namespace Swi;

class CSisRegistrySessionStep : public CSisRegistryTestStepBase
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
protected:
	Swi::RSisRegistrySession iSisRegistry;
	};

// Check embedding package for a given package
class CEmbeddingPackageStep : public CSisRegistrySessionStep
	{
public:
	CEmbeddingPackageStep();
	virtual TVerdict doTestStepL();
	};

// Check Uids
class CInstalledUidsStep : public CSisRegistrySessionStep
	{
public:
	CInstalledUidsStep();
	virtual TVerdict doTestStepL();
	};

// Set Central Reposatory Value 
class CSetCenRepSettingStep : public CSisRegistrySessionStep
    {
public:
    CSetCenRepSettingStep();
    virtual TVerdict doTestStepL();
    };

// Check filename for sid
class CSidFileStep : public CSisRegistrySessionStep
	{
public:
	CSidFileStep();
	virtual TVerdict doTestStepL();
	};
	
// Check whether a file is tagged as modifiable.
class CModifiableFileStep : public CSisRegistrySessionStep
	{
public:
	CModifiableFileStep();
	virtual TVerdict doTestStepL();
	};
	
// Checks the hash of a file.
class CFileHashStep : public CSisRegistrySessionStep
	{
public:
	CFileHashStep();
	virtual TVerdict doTestStepL();
	};

// Check Packages
class CInstalledPackagesStep : public CSisRegistrySessionStep
	{
public:
	CInstalledPackagesStep();
	virtual TVerdict doTestStepL();
	};

// Check Removable Packages
class CRemovablePackagesStep : public CSisRegistrySessionStep
	{
public:
	CRemovablePackagesStep();
	virtual TVerdict doTestStepL();
	};
	
// Check In ROM Non-Removable Packages
class CInRomNonRemovablePackage : public CSisRegistrySessionStep
	{
public:
	CInRomNonRemovablePackage();
	virtual TVerdict doTestStepL();
	};	
	
// Check Drives for installed application
class CInstalledDrives : public CSisRegistrySessionStep
	{
public:
	CInstalledDrives();
	virtual TVerdict doTestStepL();
	};
	
// Check installed UIDs	
class CIsUidInstalledStep : public CSisRegistrySessionStep
	{
public:
	CIsUidInstalledStep();
	virtual TVerdict doTestStepL();
	};
	
// Check installed Packages	
class CIsPackageInstalledStep : public CSisRegistrySessionStep
	{
public:
	CIsPackageInstalledStep();
	virtual TVerdict doTestStepL();
	};

// check installed controllers by supplying a controller
class CIsControllerInstalledStep : public CSisRegistrySessionStep
	{
public:
	CIsControllerInstalledStep();
	virtual TVerdict doTestStepL();
	}; 
	
// check installed controllers by getting controllers
class CGetControllersStep : public CSisRegistrySessionStep
	{
public:
	CGetControllersStep();
	virtual TVerdict doTestStepL();
	}; 
		
//CEntryUidOpenCloseStep
class CEntryUidOpenCloseStep : public CSisRegistrySessionStep
	{
public:
	CEntryUidOpenCloseStep();
	virtual TVerdict doTestStepL();
	};
	
//CEntryPackageOpenCloseStep
class CEntryPackageOpenCloseStep : public CSisRegistrySessionStep
	{
public:
	CEntryPackageOpenCloseStep();
	virtual TVerdict doTestStepL();
	};
	
//CEntryNameOpenCloseStep
class CEntryNameOpenCloseStep : public CSisRegistrySessionStep
	{
public:
	CEntryNameOpenCloseStep();
	virtual TVerdict doTestStepL();
	};	

class CEntryMethodsStep : public CSisRegistrySessionStep
	{
public:
	CEntryMethodsStep();
	virtual TVerdict doTestStepL();
	};

// CEntrySidsStep 
class CEntrySidsStep : public CSisRegistrySessionStep
	{
public:
	CEntrySidsStep();
	virtual TVerdict doTestStepL();
	};
	

class CIsSidPresentStep : public CSisRegistrySessionStep
/**
	Iterates through every installed package and tests that
	its executables' SIDs are reported as present, as indicated by
	RSisRegistrySession::IsSidPresentL.  Then tests non-present
	SIDs are not reported as being present.
 */
	{
public:
	CIsSidPresentStep();
	virtual TVerdict doTestStepL();
	};
	

class CIsPackagePresentStep : public CSisRegistrySessionStep
	{
public:
	CIsPackagePresentStep();
	virtual TVerdict doTestStepL();
	};
	
// CIntegrityCheckStep	checks the sis controller for certificate validation
class CIntegrityCheckStep : public CSisRegistrySessionStep
	{
public:
	CIntegrityCheckStep();
	~CIntegrityCheckStep();
 	void InitializeL();
	RPointerArray<CX509Certificate>iCertArray;
	virtual TVerdict doTestStepL();
	HBufC8* ReadFileLC(const TDesC& aDatapath);
	RFs fs;
	};

// CIntegrityDateAndTimeCheckStep checks the sis controller for certificate validation with the different error settings for validity period checks
class CIntegrityDateAndTimeCheckStep : public CIntegrityCheckStep
	{
public:
	CIntegrityDateAndTimeCheckStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();	
	
private:
	TUid iPackageUid;
	TInt iExpectedResult;
	TTime iTime;	//The time the test is setting the system time to.
	TTime iOriginalTime;	//The time just before the system time is changed.
	};
	
class CLoggingFileInfoStep : public CSisRegistrySessionStep
/**
Test for logging file 
*/
	{
public:
	CLoggingFileInfoStep();
 	virtual TVerdict doTestStepL();
	};

class CRomStubPerformanceStep : public CSisRegistrySessionStep
	{
public:
	CRomStubPerformanceStep();
	virtual TVerdict doTestStepL();
	};
	
class CCorruptCacheStep : public CSisRegistrySessionStep
	{
public:
	CCorruptCacheStep();
	virtual TVerdict doTestStepL();
	};

class CCorruptCacheRecoverStep : public CSisRegistrySessionStep
	{
public:
	CCorruptCacheRecoverStep();
	virtual TVerdict doTestStepL();
	};

class CChangeLocaleStep : public CSisRegistrySessionStep
	{
public:
	CChangeLocaleStep();
	virtual TVerdict doTestStepL();
	};

// CRegistryFilesStep 
class CRegistryFilesStep : public CSisRegistrySessionStep
    {
public:
    CRegistryFilesStep();
    virtual TVerdict doTestStepL();
    };

#endif

