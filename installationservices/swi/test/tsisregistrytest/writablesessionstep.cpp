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
#include "writablesessionstep.h"
#include "sisregistrypackage.h"
#include "sisregistrywritableentry.h"
#include "sisregistrywritablesession.h"
#include "installtypes.h"
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/sts/sts.h>
#else
#include "integrityservices.h"
#endif
#include "cleanuputils.h"
#include "dessisdataprovider.h"
#include "siscontroller.h"
#include "application.h"
#include "userselections.h"
#include "sisregistryaccess_client.h"

_LIT(KUid, "uid");
_LIT(KPackage, "package");
_LIT(KVendor, "vendor");
_LIT(KSisFile, "sis");

using namespace Swi;

/////////////////////////////////////////////////////////////////////
// CSisRegistryStep
/////////////////////////////////////////////////////////////////////
TVerdict CSisRegistryWritableSessionStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	// Install an active scheduler
	CActiveScheduler* s = new (ELeave) CActiveScheduler;
	s->Install(s);
	
	INFO_PRINTF1(_L("Connecting to SisRegistry.."));
	User::LeaveIfError(iSisRegistry.Connect());
	INFO_PRINTF1(_L("Connected!"));
	StartTimer();
	return TestStepResult();
	}

TVerdict CSisRegistryWritableSessionStep::doTestStepPostambleL()
	{
	StopTimerAndPrintResultL();
	// Remove the installed active scheduler
	CActiveScheduler* s = CActiveScheduler::Current();
	s->Install(NULL);
	delete s;
	INFO_PRINTF1(_L("Disconnecting to SisRegistry.."));
	iSisRegistry.Close();
	
	__UHEAP_MARKEND;

	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////
// CPoliceSidsStep - tests specific to Sid access methods
/////////////////////////////////////////////////////////////////////
CPoliceSidsStep::CPoliceSidsStep()
	{
	SetTestStepName(KPoliceSids);
	}

TVerdict CPoliceSidsStep::doTestStepL()
	{
	// this test checks that this client would not be permitted to 
	// perform the operations - the  drive state update methods are
	// particularly important - no caps are required and hence policing on sid 
	// is required. 
	// the attempt to delete registry entries is important as a reassurance that 
	// the both capability and sid based policing are enforced
	TInt res;
	SetTestStepResult(EPass);
	TInt drive = 1;
	// add drive 
	TRAP(res, iSisRegistry.AddDriveL(drive));
	INFO_PRINTF3(_L("Add drive %d returned %d"), drive, res);
	if (res != KErrPermissionDenied)
		{
		SetTestStepResult(EFail);
		}
	
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// remove drive 
	TRAP(res, iSisRegistry.RemoveDriveL(drive));
	INFO_PRINTF3(_L("Remove drive %d returned %d"), drive, res);
	if (res != KErrPermissionDenied)
		{
		SetTestStepResult(EFail);
		}
#endif
		
	RPointerArray<CSisRegistryPackage> packages;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(packages);
	
	iSisRegistry.InstalledPackagesL(packages);
	
	for(TInt index = 0; index < packages.Count(); index++)
		{
		INFO_PRINTF3(_L("(Vendor Name:) %S (Package Name:) %S"), 
														&packages[index]->Vendor(), 
														&packages[index]->Name());
		
		TRAP(res, iSisRegistry.DeleteEntryL(*packages[index], 0))
		INFO_PRINTF3(_L("DeleteEntryL %d returned %d"), index, res);

		if (res != KErrPermissionDenied)
			{
			SetTestStepResult(EFail);
			}
		}	

	CleanupStack::PopAndDestroy(&packages);

 	SetTestStepResult(EPass);
 	return TestStepResult();
	}	

/////////////////////////////////////////////////////////////////////
// CWritableEntryMethodsStep - tests specific to entry methods
/////////////////////////////////////////////////////////////////////
CWritableEntryMethodsStep::CWritableEntryMethodsStep()
	{
	SetTestStepName(KWritableEntryMethods);
	}

TVerdict CWritableEntryMethodsStep::doTestStepL()
	{
	RPointerArray<CSisRegistryPackage> packages;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(packages);
	
	iSisRegistry.InstalledPackagesL(packages);
	
	for(TInt index = 0; index < packages.Count(); index++)
		{
		INFO_PRINTF3(_L("(Vendor Name:) %S (Package Name:) %S"), 
														&packages[index]->Vendor(), 
														&packages[index]->Name());
		
		Swi::RSisRegistryWritableEntry entry;
		CleanupClosePushL(entry);
		TInt res = entry.OpenL(iSisRegistry, *packages[index]);
		if (res == KErrNone)
			{
			PrintWritableEntryContents(entry);	
			}	
		CleanupStack::PopAndDestroy(&entry);
		}	

	CleanupStack::PopAndDestroy(&packages);

 	SetTestStepResult(EPass);
 	return TestStepResult();
	}	

void CWritableEntryMethodsStep::PrintWritableEntryContents(Swi::RSisRegistryWritableEntry &aEntry)
	{
	INFO_PRINTF1(_L("-------------------------------------\n")); 
	// get the vendor; package name 
	HBufC* vendor = aEntry.UniqueVendorNameL();
	CleanupStack::PushL(vendor);
	HBufC* package = aEntry.PackageNameL();
	CleanupStack::PushL(package);
	INFO_PRINTF3(_L("Package: %S from Vendor: %S"), package, vendor); 
	CleanupStack::PopAndDestroy(package);
	CleanupStack::PopAndDestroy(vendor);

	// get the localised vendor name 
	HBufC* vendorLocal = aEntry.LocalizedVendorNameL();
	CleanupStack::PushL(vendorLocal);
	INFO_PRINTF2(_L("Localised Vendor Name: %S"), vendorLocal); 
	CleanupStack::PopAndDestroy(vendorLocal);

	// get uid
	TUid uid = aEntry.UidL();
	INFO_PRINTF3(_L("Package Uid: %d 0x%08x"), uid.iUid, uid.iUid);
	
	// get the version
	TVersion packageVersion = aEntry.VersionL(); 
	INFO_PRINTF4(_L("Version  %d %d %d"), packageVersion.iMajor, packageVersion.iMinor, packageVersion.iBuild );
	
	// get sids	
	RArray<TUid> sids;
	CleanupClosePushL(sids);
	aEntry.SidsL(sids);
	if (sids.Count() <= 0)
		{
		INFO_PRINTF1(_L("No sids installed!"));
		}
	else 
		{
		for (TInt index = 0; index < sids.Count(); index++)
			{
			INFO_PRINTF2(_L("Sid: 0x%08x was found!"), sids[index]);
			}
		}
	CleanupStack::PopAndDestroy(&sids);

	// get language
	TLanguage language = aEntry.LanguageL();
	INFO_PRINTF3(_L("Package Language: %d 0x%08x"), language, language);

	// get property
	TInt32 propertyKey = 10;
	TInt32 propertyValue = aEntry.PropertyL(propertyKey);
	INFO_PRINTF3(_L("Package property: key %d  value %d"), propertyKey, propertyValue);

	// is present - currenty relies on default value
	TBool isPresent = aEntry.IsPresentL();
	INFO_PRINTF2(_L("Package present is  %d"), isPresent);
  	
  	// is signed
	TBool isSigned = aEntry.IsSignedL();
	INFO_PRINTF2(_L("Package signed is  %d"), isSigned);

	if (isSigned)
		{
		RPointerArray<HBufC8> certChains;
		CleanupResetAndDestroy<RPointerArray<HBufC8> >::PushL(certChains);
		aEntry.CertificateChainsL(certChains);
		INFO_PRINTF2(_L("Package signed with %d chains"), certChains.Count());
		CleanupStack::PopAndDestroy(&certChains);
		}
		
		// check Trust
		switch(aEntry.TrustL())
			{
			case ESisPackageUnsignedOrSelfSigned:
				INFO_PRINTF1(_L("Packaged Trust: Unsigned or self signed"));
				break;
			case ESisPackageCertificateChainNoTrustAnchor:
				INFO_PRINTF1(_L("Packaged Trust: Certificate Chain has no trust anchor in Cert store"));
				break;
			case ESisPackageCertificateChainValidatedToTrustAnchor:
				INFO_PRINTF1(_L("Packaged Trust: Certificate Chain Validated to Trust Anchor in Cert store"));
				break;
			case ESisPackageChainValidatedToTrustAnchorOCSPTransientError:
				INFO_PRINTF1(_L("Packaged Trust: Certificate Chain Validated and OCSP Transient Error"));
				break;
			case ESisPackageChainValidatedToTrustAnchorAndOCSPValid:
				INFO_PRINTF1(_L("Packaged Trust: Certificate Chain Validated and OCSP Valid"));
				break;
			case ESisPackageBuiltIntoRom:
				INFO_PRINTF1(_L("Packaged Trust: Built into ROM"));
				break;
			case ESisPackageValidationFailed:
				INFO_PRINTF1(_L("Packaged Trust: Certificate Chain Validation Failed"));
				break;
			default:
				INFO_PRINTF1(_L("Packaged Trust: Unknown"));
				break;
			};
  
	// Time at which the trust was established
  	TTime timeStamp = aEntry.TrustTimeStampL();
  	TBuf <32> dateTimeText;
	TRAP_IGNORE(timeStamp.FormatL(dateTimeText, _L("%H%:1%T%:2%S on %1%/1%2%/2%3")));
	INFO_PRINTF2(_L("Package Trust Established: %S"), &dateTimeText);


    // is in rom
	TBool inRom = aEntry.IsInRomL();
	INFO_PRINTF2(_L("Package inRom is  %d"), inRom);

    // is an augmentation
	TBool isAug = aEntry.IsAugmentationL();
	INFO_PRINTF2(_L("Package isAug is  %d"), isAug);
  	// is preinstalled
	/*  	
    Sis::TInstallType installType = aEntry.InstallTypeL();
	INFO_PRINTF3(_L("Package type is  %d %S"), 
							installType, &installTypeNames[installType]);
	*/
  	// get list of files
  	RPointerArray<HBufC> files;
  	CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(files);
  	aEntry.FilesL(files);
 	INFO_PRINTF2(_L("Package has %d files"), files.Count());
 	for (TInt i=0; i<files.Count(); i++)
	 	{
	 	INFO_PRINTF3(_L("File %d - file path %S"), i+1, files[i]);	
	 	}
			  	
  	CleanupStack::PopAndDestroy(&files);
  	
    // get the drive selected by the user
 	TChar setDrive = aEntry.SelectedDriveL();
	TUint setdrive(setDrive);
	INFO_PRINTF2(_L("User selected drive is %c"), setdrive); 
	
	INFO_PRINTF1(_L("-------------------------------------\n")); 
	}	
	
/////////////////////////////////////////////////////////////////////
// CEntryDeleteUidStep - delete a operation
/////////////////////////////////////////////////////////////////////
CEntryDeleteStep::CEntryDeleteStep()
	{
	SetTestStepName(KEntryDelete);
	}

TVerdict CEntryDeleteStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	RArray<TUid> deleteUids;
	CleanupClosePushL(deleteUids);
	GetUidArrayFromConfigL(ConfigSection(), KUid, deleteUids);
	
	RArray<TPtrC> deletePackageNames;
	CleanupClosePushL(deletePackageNames);
	GetStringArrayFromConfigL(ConfigSection(), KPackage, deletePackageNames);
	
	RArray<TPtrC> deleteVendorNames;
	CleanupClosePushL(deleteVendorNames);
	GetStringArrayFromConfigL(ConfigSection(), KVendor, deleteVendorNames);
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TInt64 transactionID = -1;
	Usif::RStsSession stsSession;
	CleanupClosePushL(stsSession);
#else
	TInt64 transactionID = 1;
	_LIT(KIntegrityServicesPath, "\\sys\\install\\integrityservices\\");
#endif

	for (TInt index = 0; index < deleteUids.Count(); index++)
		{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		transactionID = stsSession.CreateTransactionL();
#else
		CIntegrityServices* integrityService = CIntegrityServices::NewLC(transactionID, KIntegrityServicesPath);
#endif
		INFO_PRINTF3(_L("Transaction %d Deleting registry entry with Uid 0x%08x"),
									I64LOW(transactionID), deleteUids[index]);

		CSisRegistryPackage* package = CSisRegistryPackage::NewLC(deleteUids[index], deletePackageNames[index], deleteVendorNames[index]);
		TRAP_IGNORE(iSisRegistry.DeleteEntryL(*package, transactionID));
		CleanupStack::PopAndDestroy(package);

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		stsSession.CommitL();
#else
		integrityService->CommitL();
		CleanupStack::PopAndDestroy(integrityService);
#endif
		// should not be able to find this entry
		if(!iSisRegistry.IsInstalledL(deleteUids[index]))
			{
			SetTestStepResult(EPass);
			}
		}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	CleanupStack::PopAndDestroy(4, &deleteUids); //deletePackageNames, deleteVendorNames, stsSession
#else
	CleanupStack::PopAndDestroy(3, &deleteUids); //deletePackageNames, deleteVendorNames
#endif
	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////
// CAddEntryStep - Adds a registry entry representing a given package
/////////////////////////////////////////////////////////////////////
CAddEntryStep::CAddEntryStep()
	{
	SetTestStepName(KAddEntryStep);
	}

TVerdict CAddEntryStep::doTestStepL()
	{
	TPtrC sisFile;	
	if(!GetStringFromConfig(ConfigSection(), KSisFile, sisFile))
		return PrintErrorAndReturnFailL(_L("SIS file couldn't be found in the configuration!"));
	
	HBufC8 *controllerData = GetControllerFromSisLC(sisFile);
	const TInt controllerOffset = 4;
	// The registry does not store the first 4 bytes of the controller - see CInstallationProcessor
	TPtrC8 controllerDataPtr = controllerData->Mid(controllerOffset);
			
	RSisRegistryAccessSession sras;
	User::LeaveIfError(sras.Connect());
	CleanupClosePushL(sras);
	sras.AddEntryL(controllerDataPtr, iTimeMeasuredExternally);
	CleanupStack::PopAndDestroy(2, controllerData); // controllerData, sras
	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////
// CUpdateEntryStep - Updates the registry entry representing a given package
/////////////////////////////////////////////////////////////////////////////
CUpdateEntryStep::CUpdateEntryStep()
	{
	SetTestStepName(KUpdateEntryStep);
	}

TVerdict CUpdateEntryStep::doTestStepL()
	{
	TPtrC sisFile;	
	if(!GetStringFromConfig(ConfigSection(), KSisFile, sisFile))
		return PrintErrorAndReturnFailL(_L("SIS file couldn't be found in the configuration!"));

	HBufC8 *controllerData = GetControllerFromSisLC(sisFile);
	const TInt controllerOffset = 4;
	// The registry does not store the first 4 bytes of the controller - see CInstallationProcessor
	TPtrC8 controllerDataPtr = controllerData->Mid(controllerOffset);
		
	RSisRegistryAccessSession sras;
	User::LeaveIfError(sras.Connect());
	CleanupClosePushL(sras);
	sras.UpdateEntryL(controllerDataPtr, iTimeMeasuredExternally);
	CleanupStack::PopAndDestroy(2, controllerData); // controllerData, sras
	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////
// CSingleEntryDeleteStep - Deletes the registry entry representing a given package uid
/////////////////////////////////////////////////////////////////////////////
CSingleEntryDeleteStep::CSingleEntryDeleteStep()
	{
	SetTestStepName(KDeleteSingleEntry);
	}

TVerdict CSingleEntryDeleteStep::doTestStepL()
	{
	TUid uid;
	if(!GetUidFromConfig(ConfigSection(), KUid, uid))
		return PrintErrorAndReturnFailL(_L("Uid is missing in the configuration file!"));
		
	TPtrC package;	
	if(!GetStringFromConfig(ConfigSection(), KPackage, package))
		return PrintErrorAndReturnFailL(_L("Package couldn't be found in the configuration!"));
	
	TPtrC vendor;	
	if(!GetStringFromConfig(ConfigSection(), KVendor, vendor))
		return PrintErrorAndReturnFailL(_L("Vendor couldn't be found in the configuration!"));
	
	CSisRegistryPackage *sisPackage = CSisRegistryPackage::NewLC(uid, package, vendor);
	
	RSisRegistryAccessSession sras;
	User::LeaveIfError(sras.Connect());
	CleanupClosePushL(sras);
	sras.DeleteEntryL(*sisPackage, iTimeMeasuredExternally);
	CleanupStack::PopAndDestroy(2, sisPackage); // sisPackage, sras
	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////
// CPackageExistsInRomStep - Returns ETrue if any ROM stub in the filesystem has the package uid specified
/////////////////////////////////////////////////////////////////////////////
CPackageExistsInRomStep::CPackageExistsInRomStep()
	{
	SetTestStepName(KIsPackageExistInRomStep);
	}

TVerdict CPackageExistsInRomStep::doTestStepL()
	{
	TUid uid;
	if(!GetUidFromConfig(ConfigSection(), KUid, uid))
		{
		ERR_PRINTF1(_L("Uid is missing in the configuration file!"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	TBool ret = iSisRegistry.PackageExistsInRomL(uid);

	return TestStepResult();
	}



