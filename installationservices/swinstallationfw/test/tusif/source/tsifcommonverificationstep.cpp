/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology 
*/

#include "tsifcommonverificationstep.h"
#include "tsifsuitedefs.h"
#include <swi/sisregistrysession.h>
#include <swi/sisregistrypackage.h>
#include <ct/rcpointerarray.h>
#include <usif/scr/scr.h>
#include <apgcli.h>
#include <apacmdln.h>

using namespace Usif;

namespace
	{
	TBool CheckPathExistenceL(const TDesC& filePath)
		{
		TBool exists = EFalse;

		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		TEntry entry;
		TInt error = fs.Entry(filePath, entry);
		if (error == KErrNone)
			{
			exists = ETrue;
			}
		else if (error != KErrPathNotFound && error != KErrNotFound)
			{
			User::Leave(error);
			}
		
		CleanupStack::PopAndDestroy(&fs);
		return exists;
		}
	}

CSifCommonVerificationStep::~CSifCommonVerificationStep()
/**
* Destructor
*/
	{
	INFO_PRINTF1(_L("Cleanup in CSifCommonVerificationStep::~CSifCommonVerificationStep()"));
	}

CSifCommonVerificationStep::CSifCommonVerificationStep()
/**
* Constructor
*/
	{
	}

void CSifCommonVerificationStep::ImplTestStepPreambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
	{
	INFO_PRINTF1(_L("I am in CSifCommonVerificationStep::ImplTestStepPreambleL()."));
	}

TScomoState CSifCommonVerificationStep::GetScomoStateL(TComponentId aComponentId)
	{
	RSoftwareComponentRegistry scr;
	User::LeaveIfError(scr.Connect());
	CleanupClosePushL(scr);

	CComponentEntry* entry = CComponentEntry::NewLC();
	if (!scr.GetComponentL(aComponentId, *entry))
		{
		INFO_PRINTF1(_L("CSifCommonVerificationStep::GetScomoStateL() failed!"));
		CleanupStack::PopAndDestroy(2, &scr);
		User::Leave(KErrNotFound);
		}

	TScomoState scomoState = entry->ScomoState();
	CleanupStack::PopAndDestroy(2, &scr);

	return scomoState;
	}

void CSifCommonVerificationStep::ImplTestStepL()
/**
* @return - TVerdict code
* Override of base class pure virtual
* Our implementation only gets called if the base class doTestStepPreambleL() did
* not leave. That being the case, the current test result value will be EPass.
*/
	{
	INFO_PRINTF1(_L("I am in CSifCommonVerificationStep::ImplTestStepL()."));

	// Read the configuration from the ini file
	TBool verifyExistence = ETrue;
	if (!GetBoolFromConfig(ConfigSection(), KTe_VerifyPackageExistence, verifyExistence))
		{
		SetTestStepResult(EFail);
		INFO_PRINTF2(_L("Failed to read %S from ini file."), &KTe_VerifyPackageExistence);
		return;
		}

	TInt exeReturnCode = 0;
	TInt refScomoState = 0;
	TPtrC componentName, componentVendor, executableName;
	if (!GetStringFromConfig(ConfigSection(), KTe_ComponentName, componentName) ||
		!GetStringFromConfig(ConfigSection(), KTe_ComponentVendor, componentVendor) ||
		(verifyExistence && !GetIntFromConfig(ConfigSection(), KTe_ScomoState, refScomoState)))
		{
		SetTestStepResult(EFail);
		INFO_PRINTF4(_L("Failed to read %S, %S or %S from ini file."), &KTe_ComponentName, &KTe_ComponentVendor, &KTe_ScomoState);
		return;
		}

	TBool launchExecutable = GetStringFromConfig(ConfigSection(), KTe_ExecutableName, executableName);
	if (launchExecutable && verifyExistence)
		{
		if (!GetIntFromConfig(ConfigSection(), KTe_ExecutableReturnCode, exeReturnCode))
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("Failed to read %S from ini file."), &KTe_ExecutableReturnCode);
			return;
			}
		}

	// Look for a component in the SCR
	TScomoState scomoState = EDeactivated;
	TBool foundInScr = EFalse;
	TComponentId componentId = FindComponentInScrL(componentName, componentVendor);
	if (componentId != 0)
		{
		foundInScr = ETrue;
		scomoState = GetScomoStateL(componentId);
		}

	// Look for a component in the SIS Registry
	TBool checkSisRegistry = ETrue;
	GetBoolFromConfig(ConfigSection(), KTe_CheckSisRegistry, checkSisRegistry);
	TBool foundInSisRegistry = EFalse;
	if (checkSisRegistry)
		{
		Swi::RSisRegistrySession sisRegistry;
		User::LeaveIfError(sisRegistry.Connect());
		CleanupClosePushL(sisRegistry);

		RCPointerArray<Swi::CSisRegistryPackage> installedPackages;
		sisRegistry.InstalledPackagesL(installedPackages);
		CleanupClosePushL(installedPackages);

		const TInt numPackages = installedPackages.Count();
		for (TInt i=0; i<numPackages; ++i)
			{
			const TDesC& name = installedPackages[i]->Name();
			const TDesC& vendor = installedPackages[i]->Vendor();
			if (name == componentName && vendor == componentVendor)
				{
				foundInSisRegistry = ETrue;
				break;
				}
			}
		CleanupStack::PopAndDestroy(2, &sisRegistry);
		}

	// Launch the executable
	TInt apaErr = KErrNone;
	TRequestStatus launchStatus;
	if (launchExecutable)
		{
		RApaLsSession apa;
		apaErr = apa.Connect();
		if (apaErr != KErrNone)
			{
			INFO_PRINTF2(_L("Failed to connect to the application architecture server, error code: %d"), apaErr);
			User::Leave(apaErr);
			}
		CleanupClosePushL(apa);

		CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
		cmdLine->SetExecutableNameL(executableName);

		TThreadId threadId;
		apaErr = apa.StartApp(*cmdLine, threadId, &launchStatus);
		if (apaErr == KErrNone)
			{
			INFO_PRINTF3(_L("Launched: %S with error code: %d"), &executableName, launchStatus.Int());
			User::WaitForRequest(launchStatus);
			}
		else if (apaErr != KErrNotFound)
			{
			INFO_PRINTF2(_L("Failed to call RApaLsSession::StartApp(), error code: %d"), apaErr);
			User::Leave(apaErr);
			}
		CleanupStack::PopAndDestroy(2, &apa);
		}

	// Look for the component's files
	TInt numDefinedFiles = 0;
	TInt numFoundFiles = 0;
	const TInt maxComponentFiles = 16;
	for (TInt i=0; i<maxComponentFiles; ++i)
		{
		// Read the name of a file
		HBufC* filePattern = HBufC::NewLC(KTe_PackageFile.iTypeLength+KMaxIntDigits);
		TPtr filePatternPtr(filePattern->Des());
		filePatternPtr.Copy(KTe_PackageFile);
		filePatternPtr.AppendNum(i);
		TPtrC filePath;
		if (!GetStringFromConfig(ConfigSection(), *filePattern, filePath))
			{
			CleanupStack::PopAndDestroy(filePattern);
			break;
			}
		++numDefinedFiles;

		// Verify file existence
		if (CheckPathExistenceL(filePath))
			{
			++numFoundFiles;
			}
		
		CleanupStack::PopAndDestroy(filePattern);
		}

	// Calculate the test step result
	TVerdict verdict = EFail;
	if (verifyExistence)
		{
		if (foundInScr && scomoState == refScomoState && checkSisRegistry == foundInSisRegistry && numDefinedFiles == numFoundFiles)
			{
			if (launchExecutable)
				{
				if (exeReturnCode == launchStatus.Int())
					{
					verdict = EPass;
					}
				}
			else
				{
				verdict = EPass;
				}
			}
		}
	else // verify NonExistence
		{
		if (!foundInScr && numFoundFiles == 0)
			{
			verdict = EPass;

			if (checkSisRegistry && foundInSisRegistry)
				{
				verdict = EFail;
				}
			if (launchExecutable && apaErr != KErrNotFound)
				{
				verdict = EFail;
				}
			}
		}

	SetTestStepResult(verdict);

	INFO_PRINTF1(_L("================== Begin Of Common Verification Steps =================="));
	INFO_PRINTF3(_L("Verification of existence/non-existence: (%d/%d)"), verifyExistence, !verifyExistence);
	INFO_PRINTF3(_L("Component name: %S, vendor: %S"), &componentName, &componentVendor);
	INFO_PRINTF2(_L("Found in Scr = %d"), foundInScr);
	INFO_PRINTF2(_L("Found in Swi = %d"), foundInSisRegistry);
	INFO_PRINTF2(_L("Scomo state = %d"), scomoState);
	INFO_PRINTF2(_L("Exe return code = %d"), exeReturnCode);
	INFO_PRINTF3(_L("Test executable: %S launched with error code = %d"), &executableName, apaErr);
	INFO_PRINTF3(_L("Component files: defined = %d, found = %d"), numDefinedFiles, numFoundFiles);
	INFO_PRINTF1(_L("=================== End Of Common Verification Steps ==================="));
	}

void CSifCommonVerificationStep::ImplTestStepPostambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
	{	
	}
