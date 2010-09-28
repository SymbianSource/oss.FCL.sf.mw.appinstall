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
* Definition of the CInstallationPlanner
*
*/


/**
 @file
*/

#include "siscontroller.h"
#include "sisstring.h"
#include "sisinstallationresult.h"
#include "sissupportedlanguages.h"
#include "sislanguage.h"
#include "installationplanner.h"
#include "plan.h" 
#include "application.h"
#include "userselections.h"
#include "siscontentprovider.h"
#include "sishelperclient.h"
#include "securitypolicy.h"
#include "secutils.h"

#include <swi/msisuihandlers.h>

#include "sisinstallblock.h"
#include "sisproperties.h"
#include "sisproperty.h"
#include "siselseif.h"
#include "sisinfo.h"
#include "sisuid.h"

#include "sisuihandler.h"

#include "expressionevaluator.h"

#include "log.h"

#include "sisregistrywritableentry.h"
#include "sisregistrysession.h"
#include "sisregistrypackage.h"
#include "sisregistryfiledescription.h"
#include "sisregistrywritablesession.h"

#include "sisinstallerrors.h"
#include "cleanuputils.h"

#include "securitycheckutil.h"
#include "sistruststatus.h"
#include "securitypolicy.h"
#include "certchainconstraints.h"

#include "sisregistryserverconst.h"
#include "dessisdataprovider.h"
#include "adornedutilities.h"
#include "sislauncherclient.h"

using namespace Swi;
using namespace Swi::Sis;

_LIT(KSysBin, "\\sys\\bin\\");
_LIT(KPluginPath, "\\resource\\plugins\\");

CInstallationPlanner* CInstallationPlanner::NewL(RSisHelper& aSisHelper,
	RUiHandler& aInstallerUI, const CContentProvider& aContentProvider, 
	CInstallationResult& aInstallationResult)
	{
	CInstallationPlanner* self = CInstallationPlanner::NewLC(aSisHelper,
		aInstallerUI, aContentProvider, aInstallationResult);
	CleanupStack::Pop(self);
	return self;
	}

CInstallationPlanner* CInstallationPlanner::NewLC(RSisHelper& aSisHelper,
	RUiHandler& aInstallerUI, const CContentProvider& aContentProvider,
	CInstallationResult& aInstallationResult)
	{
	CInstallationPlanner* self = new (ELeave) CInstallationPlanner(aSisHelper,
		aInstallerUI, aContentProvider, aInstallationResult);
	CleanupStack::PushL(self);
	self->ConstructL(); 
	return self;
	}

CInstallationPlanner::~CInstallationPlanner()
	{
	iControllersToProcess.Close();
	delete iMainApplication;
	
	delete iExpressionEnvironment;
	delete iExpressionEvaluator;
	iPackagesToBeProcessed.ResetAndDestroy();
	iPackagesProcessed.Close();
	iEclipsableRomFiles.ResetAndDestroy();
	iOverwriteableFiles.ResetAndDestroy();
	iEclipsableOverwriteFiles.Close();
	iFilesFromPlannedControllers.ResetAndDestroy();
	iDrives.Close();
	iDriveSpaces.Close();
	}


CInstallationPlanner::CInstallationPlanner(RSisHelper& aSisHelper,
	RUiHandler& aInstallerUI, const CContentProvider& aContentProvider, 
	CInstallationResult& aInstallationResult) 
	: CPlanner(aInstallerUI, aInstallationResult) ,iSisHelper(aSisHelper), 
	  iContentProvider(aContentProvider), iStubFirstDriveLetter(KNoDriveSelected)
	{
	}

void CInstallationPlanner::ConstructL()
	{
	CPlanner::ConstructL();

	// Setup the application info to the default, until the user selects a language.
	Plan().SetAppInfoL(iContentProvider.DefaultLanguageAppInfoL());	
	
	// Add the main controller as the first in the list of controllers
	iControllersToProcess.AppendL(TControllerAndParentApplication(iContentProvider.Controller(), NULL));
	
	// Find out how much space is available on each drive before
	// installation commences. That way we can keep avaialble space
	// up to date as we plan embedded application installations
	iSisHelper.FillDrivesAndSpacesL(iDrives, iDriveSpaces);
	}

void CInstallationPlanner::FinalisePlanL(void)
	{
	// Transfer ownership of the main application
	Plan().SetApplication(iMainApplication);
	iMainApplication = NULL;
	ProcessRemainingPackagesL();
	// Note that the UninstallationPlanner informs the UI of the final progress
	// bar value here - this needs to be done after post-requisite checks for
	// an installation, since orphans to be overwritten will be planned for
	// removal in the post-requisite checks.
	}

void CInstallationPlanner::ChooseOptionsDialogL(CContentProvider& aContent, CApplication& aApplication)
	{
	TAppInfo info = aContent.LocalizedAppInfoL(iDisplayLanguageIndex);
	RPointerArray<TDesC> optionNames = aContent.LocalizedOptionsL(iDisplayLanguageIndex);
	RArray<TBool> optionSelections;

	CleanupClosePushL(optionNames);
	CleanupClosePushL(optionSelections);
	
	if (optionNames.Count() > 0)
		{
		CDisplayOptions* optionsCmd = CDisplayOptions::NewLC(info, optionNames, optionSelections);

		UiHandler().ExecuteL(*optionsCmd);
		TBool result = optionsCmd->ReturnResult();
		CleanupStack::PopAndDestroy(optionsCmd);

	    if (!result) // User pressed cancel
			{
			AbortL();
			}
			
		aApplication.UserSelections().SetOptionsL(optionSelections);		
		}
	CleanupStack::PopAndDestroy(2, &optionNames); //&optionSelections
	}

void CInstallationPlanner::ChooseDriveDialogL(CContentProvider& aContent, CApplication& aApplication, const TInt64  &aSize)
	{
	TAppInfo info = aContent.LocalizedAppInfoL(iDisplayLanguageIndex);

	TInt selectedDrive=0;	
		
	// Only call the dialog if there is more than one drive to choose from
	if (iDrives.Count() > 1)
		{
		CDisplayDrive* driveCmd = CDisplayDrive::NewLC(info, aSize, iDrives, iDriveSpaces);

		UiHandler().ExecuteL(*driveCmd);
		selectedDrive = driveCmd->ReturnResult();
		CleanupStack::PopAndDestroy(driveCmd);

		if ((selectedDrive < 0)||(selectedDrive >= iDrives.Count()))
			{
			// UI returned an out of bound index, abort installation please...
			AbortL();
			}

		aApplication.UserSelections().SetDrive(iDrives[selectedDrive]);
		}
	else 
		{
		// Only one drive, select it automatically.
		aApplication.UserSelections().SetDrive(iDrives[0]);
		}
	}

void CInstallationPlanner::ReclaimUninstallSpaceL(const CApplication& aApplication)
	{
	// Search through the files to remove and add the space saved
	// back to the free space on given drives.
	for(TInt i = 0; i < aApplication.FilesToRemove().Count(); i++)
		{
		CSisRegistryFileDescription& fileDescription = *aApplication.FilesToRemove()[i];
		TChar drive(fileDescription.Target()[0]);
		drive.UpperCase();
		TInt index = iDrives.Find(drive);
		if(index >= 0)
			{
			iDriveSpaces[index] += fileDescription.UncompressedLength();
			}
		}
	}


void CInstallationPlanner::ChooseLanguageDialogL(CContentProvider& aContent, CApplication& aApplication) 
	{
	if(aContent.AvailableLanguages().Count() > 1)
		{
		// Since we do not know the language to use yet, we use the default!
		TAppInfo info = aContent.DefaultLanguageAppInfoL();

		CDisplayLanguage* langCmd = CDisplayLanguage::NewLC(info, aContent.AvailableLanguages());
		UiHandler().ExecuteL(*langCmd);
		iDisplayLanguageIndex = langCmd->ReturnResult();

		CleanupStack::PopAndDestroy(langCmd);

		if((iDisplayLanguageIndex < 0) || (iDisplayLanguageIndex >= aContent.AvailableLanguages().Count()))
			{
			// UI returned an out of bound index, abort installation please...
			AbortL();
			}
		}
	else
		{
		// Only one language availble, no point asking the user to select it
		iDisplayLanguageIndex = 0;
		}
	aApplication.UserSelections().SetLanguage(aContent.AvailableLanguages()[iDisplayLanguageIndex]);
	//if it's the top level controller store the appinfo for this language in the plan
	if (iContentProvider.Controller().Info().Uid().Uid() == aContent.Controller().Info().Uid().Uid())
		{
		Plan().SetAppInfoL(aContent.LocalizedAppInfoL(iDisplayLanguageIndex));
		}
	}

TLanguage CInstallationPlanner::DisplayLanguage() const
	{
	return (iContentProvider.AvailableLanguages()[iDisplayLanguageIndex]);
	}

	
void CInstallationPlanner::DisplayUpgradeDialogL(const TAppInfo& aCurrentAppInfo, RSisRegistryEntry& aUpgradedApp)
	{
	HBufC* name=aUpgradedApp.PackageNameL();
	CleanupStack::PushL(name);
	HBufC* vendorName=aUpgradedApp.LocalizedVendorNameL();
	CleanupStack::PushL(vendorName);
	TVersion version=aUpgradedApp.VersionL();
	TAppInfo upgradedAppInfo(*name, *vendorName, version);

	CDisplayUpgrade* displayUpgrade=CDisplayUpgrade::NewLC(aCurrentAppInfo, upgradedAppInfo);
	UiHandler().ExecuteL(*displayUpgrade);
	if (!displayUpgrade->ReturnResult())
		{
		User::Leave(KErrCancel); // Installation cancelled
		}
	CleanupStack::PopAndDestroy(3, name);
	}

/// Functions for keeping track of packages which need to be processed

void CInstallationPlanner::NotePackageForProcessingL(CSisRegistryPackage& aPackage, CApplication& aApplication)
	{
	CPackageApplicationPair* processed=CPackageApplicationPair::NewLC(aPackage, aApplication);
	User::LeaveIfError(iPackagesToBeProcessed.Append(processed));
	CleanupStack::Pop(processed);
	}

void CInstallationPlanner::NotePackageProcessedL(TUid aUid)
	{
	// Add pacakge to processed list
	iPackagesProcessed.AppendL(aUid);
	
	// Remove package with UID=aUid
	for (TInt i=0; i < iPackagesToBeProcessed.Count(); ++i)
		{
		if (iPackagesToBeProcessed[i]->Package().Uid()==aUid)
			{
			CPackageApplicationPair* pair=iPackagesToBeProcessed[i];
			iPackagesToBeProcessed.Remove(i);
			delete pair;
			break;
			}
		}
	}

TBool CInstallationPlanner::IsPackageProcessed(TUid aUid) const
	{
	for (TInt i=0; i < iPackagesProcessed.Count(); ++i)
		{
		if (aUid == iPackagesProcessed[i])
			{
			return ETrue;
			}
		}
	return EFalse;
	}

void CInstallationPlanner::ProcessRemainingPackagesL()
	{
	// The packages remaining on the list need to be uninstalled, since they were present
	// on the device as an embedded package, but weren't present in the new install 
	// Also check that nothing depends on this before removing the package.
	for (TInt i=0; i < iPackagesToBeProcessed.Count(); ++i)
		{
		CApplication* application=UninstallPackageL(iPackagesToBeProcessed[i]->Package(), EFalse);
		CleanupStack::PushL(application);
		iPackagesToBeProcessed[i]->Application().AddEmbeddedApplicationL(application);
		CleanupStack::Pop(application); // ownership has been transferred
		}
	}


TBool CInstallationPlanner::IsValidSaUpgradeL(RSisRegistryWritableEntry& aRegistryEntry, const TBool aIsSisNonRemovable, const TBool aControllerFlag)
	{
	TBool isBaseNonRemovable = !aRegistryEntry.RemovableL();
	TBool isBaseSuSigned = aRegistryEntry.IsSignedBySuCertL();
	TBool isBaseInRom = aRegistryEntry.IsInRomL();
	TBool isSameNonRemovableState = (isBaseNonRemovable == aIsSisNonRemovable);
	TBool isRuFlag = aControllerFlag & EInstFlagROMUpgrade;
	TBool isNrFlag = aControllerFlag & EInstFlagNonRemovable;
	if (iIsValidSystemUpgrade)
		{
		// SU packages can upgrade all ROM packages and non-Rom packages regardless of their state. 
		return ETrue;
		}
	else
		{
		// Non-SU packages can upgrade ROM base only if they have the RU (ROM upgrade flag) and not have the NR flag. 
		// For non-ROM packages, they cannot upgrade if and only if the base is removable and 
		// the upgrade is non-removable, or if the base is non-removable and the upgrade is removable.
		// and also cannot upgrade if Upgrade is less trusted than the base package
		return (isBaseInRom ? (isRuFlag && !isNrFlag) : (isSameNonRemovableState && !isBaseSuSigned));	
		}
	}
	
TBool CInstallationPlanner::IsValidEclipsingUpgradeL(const Sis::CController& aController, RSisRegistryWritableEntry& aRegistryEntry,  const TBool aStubExistsInRom)
	{
	TBool paInstall = aController.Info().InstallType() == Sis::EInstPreInstalledApp;
	__ASSERT_DEBUG(!paInstall, User::Leave(KErrNotSupported));
	
	if (aController.Info().Uid().Uid() != aRegistryEntry.UidL())
		{
		return EFalse;	
		}

	if (CurrentController().TrustStatus().ValidationStatus() < EValidatedToAnchor)
		{
		return EFalse;
		}
	
	TBool isBaseInRom = aRegistryEntry.IsInRomL();
	TBool saInstall = aController.Info().InstallType() == EInstInstallation;
	TBool romUpgradeFlag = aController.Info().InstallFlags() & EInstFlagROMUpgrade;
	TBool romUpgradableSA = saInstall && romUpgradeFlag;
	TBool puInstall = aController.Info().InstallType() == Sis::EInstPartialUpgrade;
	if (isBaseInRom && !saInstall)
		{
		return ETrue;
		}
	return (aStubExistsInRom && (romUpgradableSA || puInstall));			 
	}

void CInstallationPlanner::SetDeviceSupportedLanguages(RArray<TInt>& aDeviceSupportedLanguages)
	{
	iDeviceSupportedLanguages = &aDeviceSupportedLanguages;
	}

/// Processing functions

CApplication* CInstallationPlanner::ProcessControllerL(const Sis::CController& aController, TInt aCumulativeDataIndex, RPointerArray<CFileDescription>& aFilesToCapabilityCheck, TUid aParentUid)
	{	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Skip all the chances of leaving, when the planner is in info collection mode.
	if(!IsInInfoCollectionMode())
		{
	#endif
		// Check to see that we are not trying to install a protected package
		// uid with an untrusted controller
		if(aController.TrustStatus().ValidationStatus() < EValidatedToAnchor
			&& SecurityCheckUtil::IsPackageUidProtected(aController.Info().Uid().Uid().iUid))
			{
			// Can't install a protected package Uid when the controller
			// is untrusted
			DisplayErrorL(EUiUIDPackageViolation, KNullDesC);
			User::Leave(KErrSecurityError);
			}
		
		// If this package is signed by a DevCert that constraints SIDs,
		// check the pUID is not in the protected range before installing.
		// Otherwise, do not install this package, since it could be upgraded
		if ( ! aController.CertChainConstraints()->SIDIsValid( aController.Info().Uid().Uid().iUid) )
			{
			// Not allowed to install a protected package when sids are constrained
			// e.g. this can occur when a package is signed by a devcert
			DisplayErrorL(EUiConstraintsExceeded, KNullDesC);
			User::Leave(KErrSecurityError);
			}
			
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		// If this package is set to be 'hidden' and not trusted sufficiently,
		// do not install this package. Notify the user and leave from here.
		if ( (aController.Info().InstallFlags() & EInstFlagHide) && 
			 (aController.TrustStatus().ValidationStatus() < EValidatedToAnchor))
			{
			// Can't install a hidden package signed with a untrusted certificate chain.
			DisplayErrorL(EUiConstraintsExceeded, KNullDesC);
			User::Leave(KErrSecurityError);
			}
		}
	#endif	

	NotePackageProcessedL(aController.Info().Uid().Uid());
	
	// Reset the list of file allowed to be eclipsed and overwritten for
	// every embedded controller in the package
	iEclipsableRomFiles.ResetAndDestroy();
	iOverwriteableFiles.ResetAndDestroy();
	
	CApplication* application=CApplication::NewLC();
	application->SetController(aController);
	
	// Copy the set of device languages to CApplication data member
	application->CopyDeviceSupportedLanguagesL(*iDeviceSupportedLanguages);

	application->SetSisHelper(iSisHelper);
	
	if(aController.Info().InstallFlags() & EInstFlagShutdownApps)
		{
		application->SetShutdownAllApps(ETrue);
		}

	if (iSisHelper.IsStubL())	
		{
		CSecurityPolicy* securityPolicy=CSecurityPolicy::GetSecurityPolicyL();

		if (!(iSisHelper.IsSisFileReadOnlyL()) && securityPolicy->DeletePreinstalledFilesOnUninstall())
			{
			application->SetDeletablePreinstalled(ETrue);
			}
		
		iIsStub = ETrue;
		iIsPropagated = !((aController.Info().InstallType() == EInstPreInstalledPatch) || (aController.Info().InstallType() == EInstPreInstalledApp));
		application->SetStub(iIsPropagated);
		}
	
	Sis::TInstallType type = application->ControllerL().Info().InstallType();
	Sis::TInstallFlags flag = application->ControllerL().Info().InstallFlags();
	TBool ruFlag = (flag & EInstFlagROMUpgrade);
	TBool rejectedType = ( (type == EInstPreInstalledApp  ) || (type == EInstPreInstalledPatch ));
	 
	if ( ruFlag && rejectedType && aController.IsSignedBySuCert())
		{
		// reject PA or PP with RU flag System Upgrade packages ( signed by SU cert)
		DEBUG_PRINTF(_L8("Attempt made to install/preinstall/propagate System Upgrade package with RU flag"));
		User::Leave(KErrInvalidUpgrade);
		}
	
	iIsValidSystemUpgrade = IsValidSystemUpgradeL(*application);
	
	if (iIsValidSystemUpgrade)
		{
		application->SetInstallSuCertBased();
		}

	// Check to see if there is a package installed with this UID
	RSisRegistryWritableSession registrySession;
	User::LeaveIfError(registrySession.Connect());
	CleanupClosePushL(registrySession);
	
	RSisRegistryWritableEntry registryEntry;
	TInt error=registryEntry.Open(registrySession, aController.Info().Uid().Uid());	
	CleanupClosePushL(registryEntry);
	
	TBool isSisNonRemovable = ((aController.Info().InstallFlags() & EInstFlagNonRemovable) == EInstFlagNonRemovable);

	DEBUG_PRINTF2(_L8("Planning controller of type %d for install"), aController.Info().InstallType());
	//Index from the planned controllers for the base controller of the current controller
	TInt baseControllerIndex = 0;
	
	if (error == KErrNone)
		{
		CSisRegistryPackage* package=registryEntry.PackageL();
		CleanupStack::PushL(package);
		application->SetPackageL(*package);
		CleanupStack::PopAndDestroy(package);
		
		TBool isBaseRemovable = registryEntry.RemovableL();
		
		if (aController.Info().InstallType() == EInstInstallation)
			{
			// The package is already on the device so check whether it
			// is a valid upgrade
			// Stub SIS files cannot be used to perform an upgrade.
			TBool validUpgrade = IsValidSaUpgradeL(registryEntry, isSisNonRemovable, aController.Info().InstallFlags());
			if ((iIsPropagated || !validUpgrade))
				{
				DEBUG_PRINTF(_L8("Invalid Upgrade: SA Attempted"));				
				User::Leave(KErrInvalidUpgrade);
				}
			
			application->SetUpgrade(aController);
						
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			// Display the dialog only when the planner is not in info collection mode.
			if (!IsInInfoCollectionMode())
				{			
			#endif
				//if it's the top level controller display a dialog box
				if (iContentProvider.Controller().Info().Uid().Uid() == aController.Info().Uid().Uid())
					{
					DisplayUpgradeDialogL(Plan().AppInfoL(), registryEntry);
					}			
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK					
				}
			#endif
			
				
			/**
			 * Add the embedded packages to a list of packages which are yet to be processed. 
			 * At the end of the processing of all the controllers this list is checked to 
			 * see if theres some controllers which only need to be uninstalled, and the files 
			 * to be removed are added to the plan. 
			 * However this only applies when it's not a partial upgrade.
			 */
			RPointerArray<CSisRegistryPackage> embeddedPackages;
			CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(embeddedPackages);
			registryEntry.EmbeddedPackagesL(embeddedPackages);

			for (TInt i=0; i < embeddedPackages.Count(); ++i)
				{
				NotePackageForProcessingL(*embeddedPackages[i], *application);
				}
			CleanupStack::PopAndDestroy(&embeddedPackages);	
				
			}
		else if (aController.Info().InstallType() == EInstAugmentation)
			{ 
			if (isBaseRemovable && isSisNonRemovable)
				{
				DEBUG_PRINTF(_L8("Invalid Upgrade: SP Attempted"));
				User::Leave(KErrInvalidUpgrade);
				}			
						
			if (iIsPropagated)
				{
				// Treat an augmentation on a removable media stub as a
				// preinstalled patch
				application->SetPreInstalledPatch(aController);
				}
			else
				{
				application->SetAugmentation(aController);
				}
			}
		else if (aController.Info().InstallType() == EInstPreInstalledPatch)
			{
			// NR patches on pre-installed media are invalid
			if (isSisNonRemovable)
			{
				DEBUG_PRINTF(_L8("Invalid Upgrade: NR patches on pre-installed media are invalid"));
				User::Leave(KErrInvalidUpgrade);
			}
			
			application->SetPreInstalledPatch(aController);
			}
		else if (aController.Info().InstallType() == EInstPartialUpgrade)
			{
			// Do not allow partial upgrades to preinstalled applications
			// BR1863.1 and BR1863.2
			if (registryEntry.PreInstalledL())
				{
				User::Leave(KErrInvalidUpgrade);
				}
				
			// Stub SIS files cannot be used to perform an upgrade
			if (iIsPropagated)
				{
				User::Leave(KErrInvalidUpgrade);
				}
			
			// Upgrade package is less trusted than the base package
			if (!iIsValidSystemUpgrade && registryEntry.IsSignedBySuCertL())
				{
				DEBUG_PRINTF(_L8("Invalid Upgrade: PU Attempted"));
				User::Leave(KErrInvalidUpgrade);
				}
			// Partial Upgrade can only be installed to a base package of the same install flag type	
			// PU cannot change the non-removable status of a base package, i.e. a non-removable package 
			// cannot replace a removable one and visa versa. The only exception is ROM stub which is upgradable 
			// and non-removable by definition.
			if ((isBaseRemovable == isSisNonRemovable) && !registryEntry.IsInRomL())
				{
				DEBUG_PRINTF(_L8("Invalid Upgrade: PU Attempted"));
				User::Leave(KErrInvalidUpgrade);
				}	
							
			application->SetPartialUpgrade(aController);
			
			// If the parent application is in ROM, set this type to
			// be a ROM application too, so eclipsing rule
			if (registryEntry.IsInRomL())
				{
				application->SetInROM();
				}
			
			}
		else if (aController.Info().InstallType() == EInstPreInstalledApp)
			{
			
			// We can't install a preinstalled app over the top of 
			// another application.
			User::Leave(KErrInvalidUpgrade);
			
			}
		
		// Preinstalled apps will have errored out by this point,
		// regular installs will have their files removed,
		// others will have the files added to the list of eclipsable ROM files
		// if they satisfy the appropriate conditions.
		PrepareEclipsableFilesListL(aController);
	
		// Check to ensure all upgrades are sufficiently 
		// trusted to upgrade the base package
		// non System Upgrade SP upgrades, to a System Upgrade Package  shall only add files.
		TBool nonSuPkgSpUpgradeoverSu = !iIsValidSystemUpgrade && application->IsAugmentation() && registryEntry.IsSignedBySuCertL();
		if (!SecurityCheckUtil::IsSufficientlyTrusted(aController, 
				registryEntry.TrustStatusL().ValidationStatus()) && !nonSuPkgSpUpgradeoverSu)
			{
			DEBUG_PRINTF(_L8("Application not sufficiently trusted to upgrade its base"));
			User::Leave(KErrInvalidUpgrade);
			}
		}
	else if (error == KErrNotFound)
		{
		// SWI cannot intall non-removable packages if the sis file not signed with the trusted certificate
		if (isSisNonRemovable && !iIsValidSystemUpgrade)
			{
			DEBUG_PRINTF(_L8("Attempt made to install NR sis file which is not trusted by device"));
			User::Leave(KErrNotSupported);			
			}
		
		if (aController.Info().InstallType() == EInstInstallation)
			{
			// The application isn't on the device so an installation can proceed
			
			if(iIsPropagated)
				{
				// Treat an installation inside a stub SIS file as a
				// Pre-Installed App
				application->SetPreInstalledApp(aController);
				}
			else
				{
				application->SetInstall(aController);
				}	
			}
		else if (aController.Info().InstallType() == EInstAugmentation)
			{
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			// Skip all the chances of leaving, when the planner is in info collection mode.
			if(!IsInInfoCollectionMode())
				{
			#endif
				//The application isn't on the device so we have to check if the base 
				//package on the planned list. can't upgrade an augmentation in the list.
				if(!IsPlannedControllerL(aController, baseControllerIndex, aParentUid) ||
					IsAugmentationUpgrade(aController))
					{									
					// we needed the base package on the device, so fail
					DisplayErrorL(EUiMissingBasePackage, KNullDesC);
					User::Leave(KErrMissingBasePackage);					
					}
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
				}
			#endif			
				
			if (iIsPropagated)
				{
				// Treat an augmentation on a removable media stub as a
				// preinstalled patch
				application->SetPreInstalledPatch(aController);
				}
			else
				{
				application->SetAugmentation(aController);
				}
			}
		else if (aController.Info().InstallType() == EInstPartialUpgrade)
			{
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			// Skip all the chances of leaving, when the planner is in info collection mode.
			if(!IsInInfoCollectionMode())
				{
			#endif
				//The application isn't on the device so we have to check if the base 
				//package on the planned list
				if(!IsPlannedControllerL(aController, baseControllerIndex, aParentUid))
					{
					// we needed the base package on the device, so fail
					DisplayErrorL(EUiMissingBasePackage, KNullDesC);
					User::Leave(KErrMissingBasePackage);
					}
				// Stub SIS files cannot be used to perform an upgrade
				if (iIsPropagated)
					{
					User::Leave(KErrInvalidUpgrade);
					}
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
				}
			#endif
			application->SetPartialUpgrade(aController);
			}
		else if (aController.Info().InstallType() == EInstPreInstalledApp)
			{
			// The application isn't on the device so an installation can precede
			application->SetPreInstalledApp(aController);
			}
		else
			{
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			if(!IsInInfoCollectionMode())
				{
			#endif
				// we needed the base package on the device, so fail
				DisplayErrorL(EUiMissingBasePackage, KNullDesC);
				User::Leave(KErrMissingBasePackage);
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
				}
			#endif				
			}
			
		if ((aController.Info().InstallType() == EInstPartialUpgrade) || (aController.Info().InstallType() == EInstAugmentation))		
			{
			TInt baseControllerNum;
			TBool trusted = EFalse;
			//Find the base controller for the current controller
			if((baseControllerNum = GetControllerReference(aController.Info().Uid().Uid())) >= 0)
				{
				if(SecurityCheckUtil::IsSufficientlyTrusted(aController, iControllersToProcess[baseControllerNum].Controller()))
					{
					trusted = ETrue;
					}
				}
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			if(!IsInInfoCollectionMode())
				{
			#endif
				if(!trusted)
					{
					DEBUG_PRINTF(_L8("Application not sufficiently trusted to upgrade its base"));
					User::Leave(KErrInvalidUpgrade);
					}
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
				}
			#endif
			}
		}
	else
		{
		User::LeaveIfError(error);
		}
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Check whether the package content is DRM protected.
	TBool isContentDrmProtected = iSisHelper.IsDrmProtectedL();
	application->SetDrmProtected(isContentDrmProtected);	
	#endif

	// Get the user selections
	CContentProvider* content = CContentProvider::NewLC(application->ControllerL());
	
	// If the count of Device supported languages is not equal to zero, then choose the 
	// language based on the sis supported languages and device supported languages.
	if ( 0 != iDeviceSupportedLanguages->Count()) 
		{
		// Sets the following flags, 
		// a) iHasDeviceSupportdLanguages, indicates whether a SIS file contains the Device
		//	  Language token or not.
		// b) iHasMatchingLanguages, indicates whether there is an exact match(any one) between  
		//	  the languages which device supports and languages which SIS file supports.	
		SetDeviceLanguagesL(aController.InstallBlock(), *application, *content);
		
		// If a SIS file contains Supported language token and iHasMatchingLanguages is TRUE
		// then either current device language is used or the lowest language id supported by 
		// the SIS file will be used else if iHasMatchingLanguages is FALSE then proceed  
		// with user language selection dialog box displaying sis supported languages.
	
		if ( application->IsDeviceSupportedLanguages())
			{
			TInt langIndex = 0;
			if ( application->IsLanguageMatch() ) //exact match 
				{					
				TLanguage currentLanguage = User::Language();    // Language of the current locale
				TBool isExactLanguageMatch = EFalse;
				
				// Check whether the current locale is in the list of available languages
				const RArray<TInt>& matchingLanguages = application->GetMatchingDeviceLanguages();
				const RArray<TLanguage>& availableLanguages = content->AvailableLanguages();
				
				TInt index = availableLanguages.Find(currentLanguage);
				TInt currentLanguageIndex = matchingLanguages.Find(currentLanguage);
	
				if( (index != KErrNotFound && currentLanguageIndex != KErrNotFound))
					{
					langIndex = index;
					isExactLanguageMatch = ETrue;
					}
				else
					{
					TInt leastExactMatchLanguagesIndex = 0;
					for(TInt i=0 ; i< availableLanguages.Count() ; i++)
						{
						TBool isFirstMatch = ETrue;
						if((matchingLanguages.Find(availableLanguages[i])!= KErrNotFound) && ((availableLanguages[i] <= availableLanguages[leastExactMatchLanguagesIndex])||isFirstMatch))
							{
							isFirstMatch = EFalse;
							leastExactMatchLanguagesIndex = i;
							isExactLanguageMatch = ETrue;
							}
						}
					if( isExactLanguageMatch )
						{
						langIndex = leastExactMatchLanguagesIndex;
						}
					else // Intersection of Matching Languages & SIS file's Language header is NULL
						{
						#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
						// Display the dialog only when the planner is not in info collection mode.
						if (!IsInInfoCollectionMode())
							{
						#endif
							ChooseLanguageDialogL(*content, *application);
						#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
							}
						#endif
						}
					}
				if( isExactLanguageMatch )
					{
					application->UserSelections().SetLanguage(content->AvailableLanguages()[langIndex]);
					iDisplayLanguageIndex = langIndex;
					
					//if it's the top level controller store the appinfo for this language in the plan
					if (iContentProvider.Controller().Info().Uid().Uid() == content->Controller().Info().Uid().Uid())
						{
						Plan().SetAppInfoL(content->LocalizedAppInfoL(iDisplayLanguageIndex));
						}
					DEBUG_PRINTF2(_L8("current device Language id is  %d"), content->AvailableLanguages()[langIndex]);
					}
				}
			else // No matching language with device
				{		
				//if no exact match is there the proceed with user language selection dialog box 
				//displaying sis supported languages.
				#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
				// Display the dialog only when the planner is not in info collection mode.
				if (!IsInInfoCollectionMode())
					{
				#endif
					ChooseLanguageDialogL(*content, *application);
				#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
					}
				#endif
				}
			}
		else // No Supported Language Token in SIS file
			{
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			// Display the dialog only when the planner is not in info collection mode.
			if (!IsInInfoCollectionMode())
				{
			#endif
				ChooseLanguageDialogL(*content, *application);
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
				}
			#endif
			}
		
		}
	else //Device Supported Languages is zero. Hence proceed with user language selection dialog box
		{
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		// Display the dialog only when the planner is not in info collection mode.
		if (!IsInInfoCollectionMode())
			{
		#endif
			ChooseLanguageDialogL(*content, *application);
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
			}
		#endif
		}

	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Display the dialog only when the planner is not in info collection mode.
	if (!IsInInfoCollectionMode())
		{
	#endif
		ChooseOptionsDialogL(*content, *application);
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
		}
	#endif
	
	DEBUG_PRINTF4(_L("Application for install; UID: 0x%08x, Name: %S, Vendor: %S"),
		aController.Info().Uid().Uid().iUid, &(aController.Info().Names()[iDisplayLanguageIndex]->Data()),
		&(aController.Info().UniqueVendorName().Data()));
	
	// Check if this is an augmentation upgrade
	// This must be done after the language has been selected, so that we have the localised
	// name of the package.
	if (error == KErrNone && IsAugmentationUpgradeL(aController, registryEntry, registrySession))
		{
		// We have an augmentation upgrade, check that it's OK to upgrade the previous augmentation
		if (!SecurityCheckUtil::IsSufficientlyTrusted(aController, 
				registryEntry.TrustStatusL().ValidationStatus()))
			{
			DEBUG_PRINTF(_L8("Application not sufficiently trusted to upgrade its base"));
			User::Leave(KErrInvalidUpgrade);
			}
			
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		// Display the dialog only when the planner is not in info collection mode.
		if (!IsInInfoCollectionMode())
			{
		#endif
			// if this is the top level controller, display the upgrade dialog.
			if (iContentProvider.Controller().Info().Uid().Uid() == aController.Info().Uid().Uid())
				{
				DisplayUpgradeDialogL(Plan().AppInfoL(), registryEntry);
				}
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK				
			}
		#endif
		
		// The flags now mark this package as both an upgrade _and_ an augmentation
		application->SetUpgrade(aController);
		// entry has changed so change the package in the application
		CSisRegistryPackage* package=registryEntry.PackageL();
		CleanupStack::PushL(package);
		application->SetPackageL(*package);
		CleanupStack::PopAndDestroy(package);
		}

	// add files to the plan which may need to be removed.
	if(error == KErrNone)
		{
		ProcessFilesToRemoveL(*application, registryEntry);
		}

	// Setup the evaluation environment for the user selections
	SetupExpressionEnvironmentL(*application);
	
	// If we doing any kind of upgrade we can assume the space 
	// currently used by files that will be removed will be 
	// available for use by our installation
	ReclaimUninstallSpaceL(*application);

	// Size of this controller only!!
	TInt64 size = iContentProvider.TotalSizeL(aController.InstallBlock(), iExpressionEvaluator, EFalse);

		// If the package is partial upgrade, extract drive from the registry, and dont
		// display the drive selection option except where we are upgrading a ROM stub
		if(error == KErrNone && aController.Info().InstallType() == EInstPartialUpgrade && !registryEntry.IsInRomL())
			{
			TChar drive = registryEntry.SelectedDriveL();
			if(drive == TChar(KNoDriveSelected) && IsUserDriveSelectionRequiredL(aController.InstallBlock()))
				{
				// Base package had hard coded file paths so we can't make
				// any assumptions about where to install the upgrade
				// we need to ask the user where they would like to install 
				// this upgrade
			    #ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			        if(IsInInfoCollectionMode())
			            {
			            application->UserSelections().SetDrive(iSystemDriveChar);
			            const_cast <Sis::CController&>(aController).SetDriveSelectionRequired(ETrue);
			            }
			        else
			            {
			    #endif			            
				ChooseDriveDialogL(*content, *application, size);				
                #ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK                    
                        }
                #endif								
				}
			else
				{
				// install partial upgrade on the same drive as base package
				application->UserSelections().SetDrive(drive);
				}
			}		       		
		else if(error == KErrNotFound && aController.Info().InstallType() == EInstPartialUpgrade &&
		        iCurrentController > 0 && iFilesFromPlannedControllers[baseControllerIndex]->Drive() != TChar(KNoDriveSelected) )
				{
				//Use the base package's drive
				application->UserSelections().SetDrive(iFilesFromPlannedControllers[baseControllerIndex]->Drive());			
				}		
		else if((aController.Info().InstallType() == EInstInstallation
				|| aController.Info().InstallType() == EInstAugmentation)
				&& iIsPropagated)
			{
			// This is a removable media card stub, the files must 
			// be on the same removable drive as the stub SIS file 
			application->UserSelections().SetDrive(iSisHelper.GetSisFileDriveL());
			}
		else if(aController.Info().InstallType() == EInstPreInstalledApp
				|| aController.Info().InstallType() == EInstPreInstalledPatch)
			{
			// This is a preinstalled application or patch. Installation is
			// performed "in place" so the only drive that makes sense to
			// select is the same one that the SIS file lives in.
			// Note: PA/PP stub sis files can only install files on the same
			// drive that the sis file is located on. See DEF084248
			application->UserSelections().SetDrive(iSisHelper.GetSisFileDriveL());
			}
		else if (IsUserDriveSelectionRequiredL(aController.InstallBlock()))
			{
			// User needs to choose which drive will be used for the installation
            #ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
                if(IsInInfoCollectionMode())
                   {
                   application->UserSelections().SetDrive(iSystemDriveChar);
                   const_cast <Sis::CController&>(aController).SetDriveSelectionRequired(ETrue);
                   }
                else
                   {
            #endif                  
            ChooseDriveDialogL(*content, *application, size);                  
            #ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK                   
                   }
            #endif
			}
		
	CleanupStack::PopAndDestroy(content);

	// To accurately display the space avaialble for next controller 
	// we need to subtract the space used by this controller
	TChar drive = application->UserSelections().Drive();
	if(drive != TChar(KNoDriveSelected))
		{
		TInt index = iDrives.FindL(drive);
		iDriveSpaces[index] -= size;
		}

	application->SetAbsoluteDataIndex(aCumulativeDataIndex + aController.DataIndex());
	
	CFilesFromPlannedController* filesList = CFilesFromPlannedController::NewLC(aController.Info().Uid().Uid(), aParentUid, aController.Info().InstallType(), aController.Info().Names()[0]);
	//Set the selected drive for the package being planned
	filesList->SetDrive(drive);
	
	if (aController.Logo()!= NULL)
		{
 		const CFileDescription& aFileDescription = aController.Logo()->FileDescription();
		ProcessFileDescriptionL(aFileDescription, *application, aFilesToCapabilityCheck, *filesList);
		}
			
	// Process the actual controller, then return the application
	ProcessInstallBlockL(aController.InstallBlock(), *application, aFilesToCapabilityCheck, *filesList);

	//Publishing the UID of the associated package.
	TUid publishUid = aController.Info().Uid().Uid();
	if(!(Swi::SecUtils::IsPackageUidPresent(publishUid, iUidList)))
	    {
	    TInt err = Swi::SecUtils::PublishPackageUid(publishUid, iUidList);
	    if(err == KErrNone)
	        {
	        DEBUG_PRINTF2(_L("CInstallationPlanner::ProcessControllerL published Uid %x."), publishUid.iUid);
	        }
	    else if(err == KErrOverflow)
	         {
	         DEBUG_PRINTF2(_L("CInstallationPlanner::ProcessControllerL failed to publish Uid %x as the array, holding the uids, exceeded its upper limit."),publishUid.iUid);
	         }
        else if(err == KErrNotFound)
             {
             DEBUG_PRINTF2(_L("CInstallationPlanner::ProcessControllerL failed to publish Uid %x as the property is not been defined."),publishUid.iUid);
             }
        else
	        {
            DEBUG_PRINTF3(_L("CInstallationPlanner::ProcessControllerL failed to publish Uid %x with error %d."),publishUid.iUid, err);
            User::Leave(err);
	        }
	    }
		
	// Filtering of rsc files from the set of files to be copied and using parser to 
	// extract app uid,foldername,filename and iconfilename 
    #ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    if(IsInInfoCollectionMode())
       {        
        _LIT(KApparcRegDir, "\\private\\10003a3f\\import\\apps\\");	
        _LIT(KApparcRegistrationFileExtn,".rsc");
        
        //Getting the list of files to be added from CApplication Object
        RPointerArray<CSisRegistryFileDescription> listOfFilesToBeAdded = application->FilesToAdd();
        //Stores the registration resource files which are passed to the apparc parser
        RPointerArray<TDesC> regFilesArray;	  
        CleanupResetAndDestroyPushL(regFilesArray);
        RPointerArray<CSisRegistryFileDescription> listOfFilesToBeExtracted;  
        CleanupClosePushL(listOfFilesToBeExtracted);
        RFs fs;	
        RArray<TChar> drives;                               // Array of system drives    
        RArray<TInt64> driveSpaces;                         // Space available on each drive
        CleanupClosePushL(drives);
        CleanupClosePushL(driveSpaces);
        TInt64 currentAvailableDriveSpace = 0 ;         
        TInt64 totalApplicationDataSize = 0;
        CSisRegistryFileDescription* currentFileDescription = NULL;
     
        //Obtain the disk space available on the drives
        iSisHelper.FillDrivesAndSpacesL(drives, driveSpaces);
        TChar systemDrive = RFs::GetSystemDriveChar();
        TInt driveIndex = drives.Find(systemDrive);
        currentAvailableDriveSpace = driveSpaces[driveIndex];            //first drive is 'C' drive only
    
        //Opening a file server session
        User::LeaveIfError(fs.Connect());
        CleanupClosePushL(fs);
        User::LeaveIfError(fs.ShareProtected());
    
        TInt noOfFilesToBeAdded = listOfFilesToBeAdded.Count();
        // Processing each file and checking if a file is an resource file(*_reg.rsc or *.rsc or *.r%d)
        for(TInt i=0 ; i < noOfFilesToBeAdded ; i++)
            {
            HBufC* targetFileName;	    	   	
            targetFileName = listOfFilesToBeAdded[i]->Target().Alloc();	   
            CleanupStack::PushL(targetFileName);
            TParsePtrC filename(*targetFileName);	    	   
            TBool isApparcFile = EFalse;
            HBufC* extension = TParsePtrC(*targetFileName).Ext().AllocLC();
            
            if(!extension->Compare(KApparcRegistrationFileExtn)) // for resource files *_reg.rsc or *.rsc
                {
                isApparcFile = ETrue;
                }
            else
                {
                TInt extnLength = extension->Length();
                HBufC* extn;
                if(extnLength == 4)                            //for localizable resource files with extn like .r01
                    {
                    extn = extension->Right(2).AllocLC();
                    }
                else if(extnLength == 5)
                    {
                    extn = extension->Right(3).AllocLC();       //for localizable resource files with extn like .r101	            
                    }
                else
                    {   
                    CleanupStack::PopAndDestroy(2, targetFileName);  //extension
                    continue;
                    }
    
                //Check to find if the extension is of valid localizable resource files 
                TInt value = 0;
                // Declare the variable
                TLex lex(*extn);
                // Convert the descriptor into the integer number
                TInt err = lex.Val(value);
                if(err == KErrNone)
                  {
                  isApparcFile = ETrue;    
                  }	        
                CleanupStack::PopAndDestroy(extn);	        
                }
            
            //If its an apparc file(rsc) file then add its size to the total application size
            if(isApparcFile)
                {	        
                listOfFilesToBeExtracted.AppendL(listOfFilesToBeAdded[i]);
                totalApplicationDataSize += listOfFilesToBeAdded[i]->UncompressedLength();
                }	    
            CleanupStack::PopAndDestroy(2, targetFileName);  //extension	    
            }                   
            
        //Here we do extraction of rsc files ,before extracting files we check if there is an enough space on the disk(C drive)
        //to extract the files then extract the file to a temporary location and
        //check if it is a registration resource file(using target path) then store it into an array.     
        TInt noOfFilesToBeExtracted = listOfFilesToBeExtracted.Count();            
        DEBUG_PRINTF2(_L("Total number resource files (registration/localizable)to be extracted is %d"), noOfFilesToBeExtracted);            
        if(0 != noOfFilesToBeExtracted)
            {	
            
            //Check if there is enough space to extract the resource (registration or localizable) files    
            if(totalApplicationDataSize > currentAvailableDriveSpace)
                {
                //No memory to extract the file
                User::LeaveIfError(KErrDiskFull);
                }
            
            //Extraction of rsc file to a temporary location and if it is a reg resource filr append t to an array for parsing
            for (TInt i = 0 ; i < noOfFilesToBeExtracted ; i++)
                {
                TFileName resourceFileName;    
                _LIT(KResourceFileNameFmt, "%c:\\resource\\install\\temp\\0x%08x\\%S"); // Pakage Uid  
                TFileName finalToBeExtracted = TParsePtrC(listOfFilesToBeExtracted[i]->Target()).NameAndExt();              	             	                 
                resourceFileName.Format(KResourceFileNameFmt, TUint(systemDrive), aController.Info().Uid().Uid().iUid,
                        &finalToBeExtracted);
                
                 TInt err = fs.MkDirAll(resourceFileName);
                 if (err!= KErrNone && err != KErrAlreadyExists)
                     User::LeaveIfError(err);                                           
                              
                 RFile resourceFile;    
                 User::LeaveIfError(resourceFile.Replace(fs, resourceFileName, 
                     EFileStream|EFileWrite|EFileRead|EFileShareExclusive));
                 CleanupClosePushL(resourceFile);	         	         
                 
                 // Extract resource file to a temporary file.
                 DEBUG_PRINTF2(_L("Current resource file (registration/localizable) to be extraced is %S"), &resourceFileName);
                 User::LeaveIfError(iSisHelper.ExtractFileL(fs, resourceFile,
                         listOfFilesToBeExtracted[i]->Index(), application->AbsoluteDataIndex(), UiHandler()));	 
                 
                 CleanupStack::PopAndDestroy(&resourceFile);
                                  
                 // If target of the file is apparc's private folder then it is registration resource file for an app
                 TParsePtrC filename(listOfFilesToBeExtracted[i]->Target());
                 if (filename.Path().Left(KApparcRegDir().Length()).CompareF(KApparcRegDir) == 0)
                     {
                     HBufC* regResourceFileName = resourceFileName.AllocL();
                     
                     regFilesArray.AppendL(regResourceFileName);   	             
                     }   
                 }
            //Since the files have been extracted the available disk space is reduced
            currentAvailableDriveSpace -= totalApplicationDataSize;                
            }            
    
        DEBUG_PRINTF(_L8("Finished extracting all resource files (registration/localizable) successfuly"));
        //Pass each registration resource file to the parser to fetch the app info  and then if icon file is present fetch the icon file
        //to a temporary location.
            
        TInt noOfRegFilesToBeParsed = regFilesArray.Count();        
        DEBUG_PRINTF2(_L("Total number Registration Resource files to be parsed is %d"), noOfRegFilesToBeParsed);  
        RSisLauncherSession launcher;
        CleanupClosePushL(launcher);
        User::LeaveIfError(launcher.Connect());
        for(TInt i = 0 ; i < noOfRegFilesToBeParsed ; i++)
            {	   	    
            Usif::CApplicationRegistrationData *appData = NULL;	    	    
            CNativeComponentInfo::CNativeApplicationInfo* applicationInfo = NULL;
            RArray<TLanguage> languages;	 
            TFileName iconFile;	    
            // Calling the apparc parser to fetch the app info from the resouce files	
            
            DEBUG_PRINTF2(_L("Current Registration Resource file to be parsed is %S"), regFilesArray[i]);
            
            // Ask the launcher to parse the registration resource file 
            RFile file;
            User::LeaveIfError(file.Open(fs, *regFilesArray[i], EFileRead));
            TBool isForGetCompInfo(ETrue);
            TRAPD(err,appData=launcher.SyncParseResourceFileL(file, isForGetCompInfo));
            file.Close();
            
            DEBUG_PRINTF2(_L("Finished Parsing Registration Resource file %S successfuly"), regFilesArray[i]);
            if(KErrCorrupt == err)	        
                {	                                       
                continue;
                }
            else if(KErrNone != err)
                {	                                             
                User::Leave(err);
                }
            
            CleanupStack::PushL(appData);
            TUid appuid = appData->AppUid();            
            HBufC* finalAppName = TParsePtrC(appData->AppFile()).NameAndExt().AllocLC();
            const RPointerArray<Usif::CLocalizableAppInfo> aLocalizableAppInfoList = appData->LocalizableAppInfoList();
            HBufC* groupName = NULL;
            HBufC* iconFileName = NULL;
            TInt fileSize = 0 ;
            
            //If localizable info for an app is present get the localized group name, else get it from app registration data 
            if(0 == aLocalizableAppInfoList.Count())
                {
                if(appData->GroupName().Length())
                    {
                    groupName = appData->GroupName().AllocLC();
                    DEBUG_PRINTF2(_L("Application Group Name %S"), groupName);
                    }
                //Since locale does not exists no need to extract, create CNativeApplicationInfo without iconFileName
                applicationInfo = Swi::CNativeComponentInfo::CNativeApplicationInfo::NewLC(appuid, *finalAppName, groupName?*groupName:_L(""), _L(""));  
                }
            else
                {
                Usif::CLocalizableAppInfo* localizedInfo = NULL;
                const Usif::CCaptionAndIconInfo* captionAndIconInfo = NULL;
                localizedInfo = aLocalizableAppInfoList[0];
                if(localizedInfo->GroupName().Length())
                    {
                    groupName = localizedInfo->GroupName().AllocLC();
                    }
                captionAndIconInfo = localizedInfo->CaptionAndIconInfo();
                //Check if caption and icon info for an app is present or not, if present extract the icon file.
                if(captionAndIconInfo)
                    {
                    if(captionAndIconInfo->IconFileName().Length())
                        iconFileName = captionAndIconInfo->IconFileName().AllocLC();
                    
                    if(iconFileName != NULL)
                        {                        
                        HBufC* finalIconFileName = TParsePtrC(*iconFileName).NameAndExt().AllocLC();
                        
                        _LIT(KIconFileNameFmt, "%c:\\resource\\install\\icon\\0x%08x\\%S");     // Applicaiton Uid
                        iconFile.Format(KIconFileNameFmt, TUint(systemDrive), appuid.iUid,
                        finalIconFileName);
                        
                        TInt err = fs.MkDirAll(iconFile);
                        if (err!= KErrNone && err != KErrAlreadyExists)
                        User::LeaveIfError(err);
                        
                        //Find from the list of files to be copied , the file description of the icon file returned by the parser
                        for(TInt k = 0; k < listOfFilesToBeAdded.Count() ; k++)
                            {                      
                            currentFileDescription = listOfFilesToBeAdded[k];
                            if(TParsePtrC(currentFileDescription->Target()).NameAndExt().Compare(*finalIconFileName))
                                {
                                break;
                                }
                            }	              
                        //Check if there is enough space to extract the icon file           
                        fileSize = currentFileDescription->UncompressedLength();
                        if(currentAvailableDriveSpace <  fileSize)
                            {
                            //No memory to extract the file
                            User::LeaveIfError(KErrDiskFull);
                            }
                        
                        //Extracting the icon file to a temp location
                        RFile tempIconFile;
                        TInt index = 1;
                        TBuf<10>  integerAppendStr;
                        // Check if file already exists, if yes then create file with another name (e.g. *_1 or *_2)
                        while(1)
                            {	                  
                            err = tempIconFile.Create(fs, iconFile, EFileStream|EFileWrite|EFileRead|EFileShareExclusive);
                            if(err == KErrAlreadyExists)
                                {	                     
                                integerAppendStr.TrimAll();
                                integerAppendStr.Format(_L("%d"), index++);
                                TInt pos = iconFile.Length()-TParsePtrC(iconFile).Ext().Length();
                                iconFile.Insert(pos,integerAppendStr);	                      
                                }
                            else if(err == KErrNone)
                                {
                                //Everthing is fine, proceed	                      
                                break;               
                                }
                            else
                                {
                                tempIconFile.Close();
                                User::Leave(err);
                                }
                            }
                        CleanupClosePushL(tempIconFile);  
                        
                        DEBUG_PRINTF2(_L("Icon file to be extraced is %S"), &iconFile);
                        User::LeaveIfError(iSisHelper.ExtractFileL(fs, tempIconFile, listOfFilesToBeAdded[i]->Index(), application->AbsoluteDataIndex(), UiHandler())); 	              
                        DEBUG_PRINTF(_L8("Finished extracting Icon file successfuly"));
                        //After copy the available disk space is reduced
                        currentAvailableDriveSpace -= fileSize;                        
                        CleanupStack::PopAndDestroy(3,iconFileName);  //file,finalIconFileName,iconFileSize
                        
                        //Create CNativeApplicationInfo with iconFileName
                        applicationInfo = Swi::CNativeComponentInfo::CNativeApplicationInfo::NewLC(appuid, *finalAppName, groupName?*groupName:_L(""), iconFile);                                          
                        }
                    else
                        {
                        //Since iconFileName does not exists no need to extract, create CNativeApplicationInfo without iconName
                        applicationInfo = Swi::CNativeComponentInfo::CNativeApplicationInfo::NewLC(appuid, *finalAppName, groupName?*groupName:_L(""), _L(""));  
                        }
                    }
                }	    

            DEBUG_PRINTF2(_L("Application Uid 0x%08x"), appuid);
            DEBUG_PRINTF2(_L("Application Name %S"), finalAppName);
            if(groupName)
                DEBUG_PRINTF2(_L("Application Group Name %S"), groupName);
            if(iconFile.Length())
                DEBUG_PRINTF2(_L("Application Icon File Name %S"), &iconFile);
                             
            const_cast <Sis::CController&>(aController).AddApplicationInfoL(applicationInfo);
            CleanupStack::Pop(applicationInfo);
            if(groupName)
                CleanupStack::PopAndDestroy(3, appData);	//groupName,finalAppName,appData
            else
                CleanupStack::PopAndDestroy(2, appData);    //finalAppName,appData
            languages.Close();	    
            }
        
        CleanupStack::PopAndDestroy(4,&drives);             //launcher, fs, driveSpaces
        CleanupStack::Pop(&listOfFilesToBeExtracted);
        listOfFilesToBeExtracted.Close(); 
        CleanupStack::PopAndDestroy(&regFilesArray);  
      }       
    #endif    
    
	//Append planned controllers list
	iFilesFromPlannedControllers.AppendL(filesList);
	CleanupStack::Pop(filesList);

	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Some files may need to be questioned of overwrite only when the planner is not in info collection mode.
	if (!IsInInfoCollectionMode())	
	#endif
	//Some files may need to be questioned of overwrite
		WarnEclipseOverWriteL(*application);
 	
 	// Reset the file list array for next controller use
 	iEclipsableOverwriteFiles.Reset();

	ProcessPropertiesL(aController.Properties(), Plan(), *application);
	
	CleanupStack::PopAndDestroy(2, &registrySession); // registryEntry, registrySession

  	CleanupStack::Pop(application); 
	return application;
	}
	
// Prepare the eclipsable files list from the right source according to the upgrade.	
void CInstallationPlanner::PrepareEclipsableFilesListL(const Sis::CController& aController)
	{
	// If the package is allowed to install based on SU Cert rule, then no need to generate 
	// eclipsable file list, it can eclipse any files from rom	
	// before reaching this place, NR upgrade check already done for SA, SP and PU
	if (iIsValidSystemUpgrade)
		{
		return;
		}

	RSisRegistryWritableSession registrySession;
	User::LeaveIfError(registrySession.Connect());
	CleanupClosePushL(registrySession);

	RSisRegistryWritableEntry registryEntry;
	
	TInt error=registryEntry.Open(registrySession, aController.Info().Uid().Uid());		
	CleanupClosePushL(registryEntry);
	if(error != KErrNone)	
		{
		CleanupStack::PopAndDestroy(2, &registrySession);
		return;				
		}
	
	
	CSisRegistryPackage* package = registryEntry.PackageL();
	CleanupStack::PushL(package);
	TBool stubExistsInRom = registrySession.PackageExistsInRomL(package->Uid());	
	CleanupStack::PopAndDestroy(package);
	
	// Break the conditions to be checked against the package in installation.		
	TBool puInstallation = aController.Info().InstallType() == EInstPartialUpgrade;
	TBool saInstallation = aController.Info().InstallType() == EInstInstallation;
	TBool romUpgradeFlag = aController.Info().InstallFlags() & EInstFlagROMUpgrade;
	TBool romUpgradableSA = saInstallation && romUpgradeFlag;
	TBool currentPkgIsInRom = registryEntry.IsInRomL();
		
	// Standard Apps (SA) with ROM Upgrade (RU) flag,
	// Partial upgrades, patches and preinstalled patches are all
	// allowed to eclipse ROM files.					
					
	// When an upgrade happens over a upgrade which has already upgraded the 
	// ROM based package (i.e, an upgrade SA_RU_2 happens over ROM+SA_RU_1), 
	// don't populate the eclipsable files from the SIS registry entry which 
	// may be in-complete. So, directly populate the eclipsable files list from 
	// the corresponding stub SIS file.		
	if (IsValidEclipsingUpgradeL(aController, registryEntry, stubExistsInRom))
		{
		if(!currentPkgIsInRom && stubExistsInRom && (romUpgradableSA || puInstallation))
			{
			registrySession.GetFilesForRomApplicationL(aController.Info().Uid().Uid(), iEclipsableRomFiles);
			}
		// Otherwise, populate the eclipsable files list from SIS registry entry.
		else
			{
			registryEntry.FilesL(iEclipsableRomFiles);
			}
		}
		CleanupStack::PopAndDestroy(2, &registrySession);			
	}

void CInstallationPlanner::SetupExpressionEnvironmentL(const CApplication& aApplication)
	{
	delete iExpressionEnvironment;
	iExpressionEnvironment = NULL;
	
	delete iExpressionEvaluator;
	iExpressionEvaluator=NULL;
		
	iExpressionEnvironment=CSwisExpressionEnvironment::NewL(aApplication);
	iExpressionEvaluator=CExpressionEvaluator::NewL(*iExpressionEnvironment);
	}
		
// Properties
void CInstallationPlanner::ProcessPropertiesL(const Sis::CProperties& aProperties, CPlan& /*aPlan*/, CApplication& aApplication)
	{
	for (TInt i = 0 ; i < aProperties.Count(); ++i)
		{
		// Add the property to the plan
		aApplication.AddPropertyL(CApplication::TProperty(aProperties[i].Key(), aProperties[i].Value()));
		}
	}

TBool CInstallationPlanner::IsUserDriveSelectionRequiredL(const Sis::CInstallBlock& aInstallBlock) const
	{
	// process the file descriptions
	const RPointerArray<CFileDescription>& files=aInstallBlock.FileDescriptions();
  	
  	TInt i;
  	for (i = 0; i < files.Count(); ++i)
  		{
  		CFileDescription& fileDescription = *files[i];
		if (fileDescription.Target().Data().Length() != 0)
			{
			// if a target file is specified check whether the 
			// target drive is hardcoded or the wildcard "!:\"
			if(fileDescription.Target().Data()[0] == '!')
  				{
  				// Target file drive is "!:\"
  				return ETrue;
	  			}
			}
		}

	// process the expressions
	for (i = 0 ; i < aInstallBlock.IfStatements().Count(); ++i)
		{
		Sis::CIf* ifBlock=aInstallBlock.IfStatements()[i];
		
		if (!ifBlock)
			{
			User::Leave(KErrCorrupt);
			}
		if(iExpressionEvaluator->EvaluateL(ifBlock->Expression()).BoolValueL())
		   	{
		   	// If the user has select the option search the install block
		   	// for wildcards
		   	if(IsUserDriveSelectionRequiredL(ifBlock->InstallBlock()))
		   		{
		   		return ETrue;
		   		}
			}
		else
			{
			// else if blocks
			const RPointerArray<CElseIf>& elseIfs = ifBlock->ElseIfs();
		
			for (TInt j = 0 ; j < elseIfs.Count(); ++j)
				{		
				if (!elseIfs[j])
				 	{
					User::Leave(KErrCorrupt);
					}
				if(iExpressionEvaluator->EvaluateL(elseIfs[j]->Expression()).BoolValueL())
					{
					// If the user selected this option, search the installblock
					// for wildcards
					if(IsUserDriveSelectionRequiredL(elseIfs[j]->InstallBlock()))
						{
						return ETrue;
						}
					}
				}
			}
		}
	// no wildcards found, all file target drives must be hardcoded
	return EFalse;
	}

void CInstallationPlanner::SetDeviceLanguagesL(const Sis::CInstallBlock& aInstallBlock, CApplication& aApplication, const CContentProvider& aContent)
	{	
	//Setting up the expression environment which includes setting up the values for User
	//Options available in the sis file,which are used while prossesing the expression block. 
	SetupExpressionEnvironmentL(aApplication);
	RPointerArray<TDesC> optionNames = aContent.LocalizedOptionsL(iDisplayLanguageIndex);
	RArray<TBool> optionSelections;
	CleanupClosePushL(optionNames);
	CleanupClosePushL(optionSelections);
	TInt opt_len = optionNames.Count();
	TInt i = 0;
	for ( i=0; i < opt_len; i++)
		{
			optionSelections.AppendL(EFalse);
		}
	aApplication.UserSelections().SetOptionsL(optionSelections);
	CleanupStack::PopAndDestroy(2, &optionNames); //&optionSelections	
	
	TInt ifStmtCount = aInstallBlock.IfStatements().Count();
	for ( i=0 ; i < ifStmtCount; ++i)
			{
		Sis::CIf* ifBlock=aInstallBlock.IfStatements()[i];
			
			if (!ifBlock)
				{
				User::Leave(KErrCorrupt);
				}
			 iExpressionEvaluator->EvaluateL(ifBlock->Expression());						
			}
	}

// Functions to process files	
void CInstallationPlanner::ProcessInstallBlockL(const Sis::CInstallBlock& aInstallBlock, CApplication& aApplication, RPointerArray<CFileDescription>& aFilesToCapabilityCheck, CFilesFromPlannedController& aPlannedFiles)
	{
	// process the file descriptions
	const RPointerArray<CFileDescription>& files=aInstallBlock.FileDescriptions();
  
  	TInt i;
  	for (i = 0; i < files.Count(); ++i)
  		{
  		ProcessFileDescriptionL(*files[i], aApplication, aFilesToCapabilityCheck, aPlannedFiles);
  		}
  	
	// process the expressions
	for (i = 0 ; i < aInstallBlock.IfStatements().Count(); ++i)
		{
		Sis::CIf* ifBlock=aInstallBlock.IfStatements()[i];
		
		if (!ifBlock)
			{
			User::Leave(KErrCorrupt);
			}
		
		// main expression
		if ( iExpressionEvaluator->EvaluateL(ifBlock->Expression()).BoolValueL() )
			{
			// suppose TextSkip precedes an IF block then TS won't work
			// since TS is only intended to work if it is immediately followed by a line that installs a file.
			iTextSkip = EFalse;
			ProcessInstallBlockL(ifBlock->InstallBlock(), aApplication, aFilesToCapabilityCheck, aPlannedFiles);
			}
		else
			{
			// else if blocks
			const RPointerArray<CElseIf>& elseIfs=ifBlock->ElseIfs();
		
			for (TInt j = 0 ; j < elseIfs.Count(); ++j)
				{				
				if (!elseIfs[j])
				 	{
					User::Leave(KErrCorrupt);
					}
				if (iExpressionEvaluator->EvaluateL(elseIfs[j]->Expression()).BoolValueL())
					{
					iTextSkip = EFalse;
					ProcessInstallBlockL(elseIfs[j]->InstallBlock(), aApplication, aFilesToCapabilityCheck, aPlannedFiles);					
					break;	// stop processing else if blocks
					}
				}
			}
		iTextSkip = EFalse;
		}
		
	// Embedded controllers
	
	for (i = 0 ; i < aInstallBlock.EmbeddedControllers().Count(); ++i)
		{
		if (aInstallBlock.EmbeddedControllers()[i]==NULL)
			{
			User::Leave(KErrCorrupt);
			}

		// Add these to the list of controllers we are going to "plan" as we move the the installmachine state machine
		// OWNERSHIP is NOT TRANSFERRED
		iControllersToProcess.AppendL(TControllerAndParentApplication(*aInstallBlock.EmbeddedControllers()[i], &aApplication));			
		}
	}
	
/** This function examines the list of files we may eclipse to see if the supplied name is among them.
    Here we have to options:
    1.)   file is to be installed to \sys\bin therefore we have to handle adorned filenames
          eclipsing is allowed if the filename that we are trying to install is a variant of any ROM filename 
          marked as eclipsable i.e: if we are trying to install c:\sys\bin\A{000A0001}.dll
          and a ROM stub lists z:\sys\bin\A.dll as an eclipsable file or any z:\sys\bin\A{????????}.dll
          then we are allowed to that so
    2.)   file is not to be installed to sys\bin\
          in this case we mustn't invoke special handling of adorned filenames so we are only
          interested in if that particular file is allowed to be eclipsed i.e: z:\somedirectory\a.txt
          Remark:
             in this case z:\somedirectory\a.txt and z:\somedirectory\a{ABCDEFGH}.txt are considered to be different filenames
             even if they might look to be variations of the same name
	@param aRomFile fully qualified filename in ROM that is to be checked for eclipising (i.e: z:\sys\bin\some.dll)
*/

TBool CInstallationPlanner::IsEclipsableL(const TDesC& aRomFile)
	{
	// exception : SA/SP/PU sis + RU flag + signed by SU root cert can continue installation
	if (iIsValidSystemUpgrade)
		{
		return ETrue;
		}
	TBool isEclipsable = EFalse;
	RBuf romFileName;
	romFileName.CreateL(aRomFile, aRomFile.Size());
	romFileName.CleanupClosePushL();

	TParsePtr parsedFileName(romFileName);
	RBuf path;
	path.CreateL(parsedFileName.Path().Length());
	path.CleanupClosePushL();
	path = parsedFileName.Path();
    TBool goesToSysBin = (path.CompareF(KSysBin) == 0);
	for (TInt i = 0; i < iEclipsableRomFiles.Count(); i++)
		{
		if (aRomFile.MatchF(*(iEclipsableRomFiles[i])) == 0)
			{
			isEclipsable = ETrue;
			break;
			}
		else if ( goesToSysBin )		
			{
			if(Swi::IsAdornedVariationOfL(*(iEclipsableRomFiles[i]), aRomFile))
				{
				isEclipsable = ETrue;
				break;
				}
			}
		}
	CleanupStack::PopAndDestroy(2, &romFileName);
	return isEclipsable;
	}

// This function ensures the file to be installed does not illegally
// eclipse a file already installed on ROM.
TBool CInstallationPlanner::ValidEclipseL(RFs& aFs, TFileName& aFileName, CApplication& aApplication)
	{
    DEBUG_PRINTF2(_L("CInstallationPlanner::ValidEclipseL() called aFileName='%S'"), &aFileName );
	// check every file to be installed for a possible eclipse	
	// This is done is two steps:
	// If installing to \sys\bin then first check for versioned (adorned name) copy of file
	// Then check for the unadorned version
    RArray<TFileName> eclipsableOverwriteFiles;
    CleanupClose<RArray<TFileName> >::PushL(eclipsableOverwriteFiles);
    
    TParsePtr parsedFileName(aFileName);

	// When installing to sys\bin check for adorned names
	RBuf searchNameUnadorned; // Holds the normalized unadorned filename (i.e. c:\sys\bin\a.exe for aFileName equal to c:\sys\bin\a{123456}.exe)
	searchNameUnadorned.CreateL(aFileName.Length()); //fully qualified unadorned search name is never longer than original fully qualified name (i.e aFileName)
	searchNameUnadorned.CleanupClosePushL();

	//Holds all adorned file names that match the adorned filename pattern calculated out of aFileName
	RPointerArray<HBufC> adornedFileNamesFound;
    CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(adornedFileNamesFound);
	RBuf path;
	path.CreateL(parsedFileName.Path().Length());
	path.CleanupClosePushL();
	path = parsedFileName.Path();
	
 	if ( path.CompareF(KSysBin) == 0 )		
		{
		// check for versioned (adorned) filenames
		AdornedProcessingOfFileL(aFs, aFileName, searchNameUnadorned, adornedFileNamesFound);
		}
	else
		{
		searchNameUnadorned = aFileName;
		}
		
	// Second check for the unadorned version
	TChar targetDisk = aFileName[0];
	targetDisk.Fold();
	
	// Find on which disks (if any) this file exists on
	TDriveList driveList;
	User::LeaveIfError(aFs.DriveList(driveList));
	
	TBool result=ETrue;
	TChar disk = 'a' - 1;
	TEntry* entryPtr = new (ELeave) TEntry;
	CleanupStack::PushL(entryPtr);
	
	for (TInt i = 0; i < driveList.Length(); i++)
		{
		disk+=1;
		
		if (driveList[i] == 0)
			{
			continue;
			}
		searchNameUnadorned[0] = disk;

		// check if adorned file version found on this disk
		TBool adornedFound = EFalse;
		TInt nrOfAdornedFilenamesFound = adornedFileNamesFound.Count();
		for (TInt index=0; index < nrOfAdornedFilenamesFound;  ++index)
			{
			TChar drive((*(adornedFileNamesFound[index]))[0]);
			drive.Fold();
			if ( drive == disk )
				{
				adornedFound = ETrue;
				break;
				}
			}

		TBool unadornedFound = aFs.Entry(searchNameUnadorned, *entryPtr) == KErrNone;
		TBool matchFound = unadornedFound || adornedFound;
		if (!matchFound)
			continue; // Nothing relevant found on this drive

		TBool processEclipsingSuspectsFoundOnThisDrive = EFalse;
		
		// The logic varies whether we found an eclipsable/overwritable file on: (1) The same disk as the one we are installing to
		// (2) The Z drive (ROM), (3) Any other drive
		if (disk == targetDisk) // Case (1)
			{
			TBool isOverwrite = aFs.Entry(aFileName, *entryPtr) == KErrNone;
			// At this point, we have 3 possible cases: (1.1) We are correctly overwriting the already present file (a PU), 
			// (1.2) We are incorrectly overwriting the file present, (1.3) We are eclipsing the file (if one of the file names is adorned)
			//Overwriting: it can be that we found either adorned or unadorned filenames on target drive
			//Since we don't know wheter the target filename is adorned or unadorned we have to specifically check
			//wheter we are trying to overwrite a file with that name or not.
			if (isOverwrite)
				{
				// Case 1.1 is handled here - we'll uninstall the old file if we have the right to do it.
				// Case 1.2 will be handled in CPostrequisitesChecker::WouldOverWriteL, as the error will be caught there
				HandleFileOverwritingL(aFileName, aApplication);
				}
			else
				{
				//Case 1.3 is handled here: 
				//we have found a matching filename (either unadorned or adorned); add it to eclipsing suspects
				processEclipsingSuspectsFoundOnThisDrive = ETrue;
				}
			}		
		else if (disk == 'z') // Case (2)
			{
			// we are eclipsing a rom file, check if this is authorised:
			//see implementation of IsEclipsable()
			if (!IsEclipsableL(searchNameUnadorned))
				{
				result = EFalse;
				break;
				}		
			}
		else // Case (3)
			{
			processEclipsingSuspectsFoundOnThisDrive = ETrue;
			}

		//now we need to check whether the unadorned or the adorned filenames found on this drive have to be added to iEclipsableOverwriteFiles
		if(processEclipsingSuspectsFoundOnThisDrive && unadornedFound && !SecurityCheckUtil::IsToBeUninstalledL(searchNameUnadorned, aApplication))
			{
			eclipsableOverwriteFiles.AppendL(searchNameUnadorned);
			}
		if(processEclipsingSuspectsFoundOnThisDrive && adornedFound)
			{
			nrOfAdornedFilenamesFound = adornedFileNamesFound.Count();
			for (TInt index=0; index < nrOfAdornedFilenamesFound; ++index)
				{
				TChar drive((*(adornedFileNamesFound[index]))[0]);
				drive.Fold();
				if ( (drive == disk) && !SecurityCheckUtil::IsToBeUninstalledL(*(adornedFileNamesFound[index]), aApplication))
					{
					eclipsableOverwriteFiles.AppendL(*(adornedFileNamesFound[index]));
					}
				}
			}
		}//end of for (TInt i = 0; i < driveList.Length(); i++)

	//Next step to remove all files from iEclipsableOverwriteFiles that we don't consider
	//blocking files (blocking files =  those that prevent this file from installing)
	//These non blocking files are those owned by the same package UID as the current one except
	//that filename that matches the actual filename ie: aFileName
	//i.e: if aFileName = c:\sys\bin\a.dll and e:\sys\bin\a.dll is found and put in iEclipsableOverwriteFiles earlier or
	//aFileName = c:\sys\bin\a{000A0004}.dll and e:\sys\bin\a{000A0004}.dll  is found and put in iEclipsableOverwriteFiles
	//then these matching filenames should not be taken out so that the installation will fail
	//later in CInstallationPlanner::WarnEclipseOverWriteL()
	//Reason: the installer maintains only one hash per filename, that's only one hash file is calculated for \sys\bin\a.dll and
	//for \sys\bin\a{000A0004}.dll and not individual ones for each instances on different drives
	//If result is FALSE there's a blocking file in ROM so no point in processing iEclipsableOverwriteFiles
	//because the the installation will fail regardless of what iEclipsableOverwriteFiles contains
	if (result && (eclipsableOverwriteFiles.Count() > 0) ) 
		{
		TUid uid = aApplication.ControllerL().Info().Uid().Uid();
		//filter out all suspected files that belong to this package
		SecurityCheckUtil::FilterNonBlockingFilesOfFilenameL(eclipsableOverwriteFiles, aFileName, aApplication.ControllerL().Info().Uid().Uid());
		}
	//iEclipsableOverwriteFiles will be searched through later in CInstallationPlanner::WarnEclipseOverWriteL()
	//looking for files that are not orphaned (ie. belong to any package (SecurityCheckUtil::LookForBlockingFilesL())
	
	for(TInt i = 0; i < eclipsableOverwriteFiles.Count(); i++)
		{
		iEclipsableOverwriteFiles.AppendL(eclipsableOverwriteFiles[i]);
		}
		
	CleanupStack::PopAndDestroy(5, &eclipsableOverwriteFiles); 
	return result;
	}

void CInstallationPlanner::WarnEclipseOverWriteL(CApplication& aApplication)
 	{
 	//Check if we need to pop up dialog to ask client for overwrite or report blocking
 	if (iEclipsableOverwriteFiles.Count()>0)
 		{
 		//See if the swi policy allow overwrite
 		CSecurityPolicy* securityPolicy=CSecurityPolicy::GetSecurityPolicyL();
 		TBool allowOrphanedOverwrite = securityPolicy->AllowOrphanedOverwrite();
 		TBool allowProtectedOrphanOverwrite = securityPolicy->AllowProtectedOrphanOverwrite();
 		
 		if (allowOrphanedOverwrite)
 			{
			// check if there is any orphaned file
 			TInt index = SecurityCheckUtil::LookForBlockingFilesL(iEclipsableOverwriteFiles);
 			if (index != KErrNotFound) // if the list contains some non-orphaned file
 				{
 				DisplayErrorL(EUiBlockingEclipsingFile, iEclipsableOverwriteFiles[index]);
 				User::Leave(KErrInvalidEclipsing);	
 				}
 			else
 				{
 				TInt fileNameCount=iEclipsableOverwriteFiles.Count();
 				// Question user about all the Orphaned files
 				for (TInt i=0; i<fileNameCount; i++)
 					{
					TBool overwrite = EFalse;
					if (! (!allowProtectedOrphanOverwrite &&
						SecurityCheckUtil::IsProtectedDirectoryL(iEclipsableOverwriteFiles[i])))
						{
						CDisplayQuestion* cmd = CDisplayQuestion::NewLC(Plan().AppInfoL(), EQuestionOverwriteFile, iEclipsableOverwriteFiles[i]);
						UiHandler().ExecuteL(*cmd);
						if (cmd->ReturnResult())
							{
							// Add the file to list of files to remove in the
							// processing stage.  It should not actually be
							// removed until after security checks validate
							// that the target which eclipses this file is
							// valid for this package.
							aApplication.RemoveFileL(iEclipsableOverwriteFiles[i]);
							Plan().AddUninstallFileForProgress();
							overwrite = ETrue;
							}
						CleanupStack::PopAndDestroy(cmd);	
						}
					if (!overwrite)
 	 					{
 	 					// User say "no" to overwrite, so blocking
 						DisplayErrorL(EUiBlockingEclipsingFile, iEclipsableOverwriteFiles[i]);
 						User::Leave(KErrInvalidEclipsing);
						break;
 						}
 					}					
 				}			
 			}
 		else
 			{
 			// policy does not not allow overwrite, error Blocking 		
 			DisplayErrorL(EUiBlockingEclipsingFile, iEclipsableOverwriteFiles[0]);
 			User::Leave(KErrInvalidEclipsing);		
 			}
 		}
 	}

// This function is used to process all files in the controllers

void CInstallationPlanner::ProcessFileDescriptionL(const CFileDescription& aFileDescription, CApplication& aApplication, RPointerArray<CFileDescription>& aFilesToCapabilityCheck, CFilesFromPlannedController& aPlannedFiles)
	{
	
	// Check the installation path
	if (aFileDescription.Target().Data().Length() != 0)
		{
		DEBUG_PRINTF2(_L("Processing file for install '%S'"), &(aFileDescription.Target().Data()));

		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);

		CSecurityPolicy* securityPolicy=CSecurityPolicy::GetSecurityPolicyL();
		TText selDrive = aApplication.UserSelections().Drive();
		HBufC* targetFileName = securityPolicy->ResolveTargetFileNameLC(
			aFileDescription.Target().Data(), selDrive);

		// if it's a stub, rewrite it with the user drive selection
		// which includes pre-installed applications or patches.
		if (iIsStub)
			{
			// Previously, for PA and PP, if files to be "installed" were
			// on a different drive to the stub SIS file, this code would
			// leave with KErrSISInvalidTargetFile. See DEF084248/BR1857.1
			// Now, we re-write all stub file references to the drive where
			// the SIS file is located, as requested in INC109356. This allows
			// SIS files with "e:\foo.txt" to install on drives F, G, H, etc.
			// However, if the SIS file contains files which reference more
			// than one drive letter, we refuse to install.
			TPtr targetFileNamePtr(targetFileName->Des());
			
			if (iStubFirstDriveLetter == KNoDriveSelected)
				{
				iStubFirstDriveLetter = targetFileNamePtr[0];
				}
							
			if (iStubFirstDriveLetter == targetFileNamePtr[0])
				{
				targetFileNamePtr[0] = selDrive;
				}				
			else
				{
				User::Leave(KErrSISInvalidTargetFile);
				}
				
			
			targetFileNamePtr[0] = selDrive;
			
			// Additional re-writing happens in CInstallationProcessor::DoStateProcessFilesL()
			// since re-writing here is for checking in this method only
			}

		if(!SecurityCheckUtil::CheckFileName(*targetFileName, iSystemDriveChar) ||
			SecurityCheckUtil::IsSubstedDriveL(fs, *targetFileName))
			{
			DisplayErrorL(EUiInvalidFileName, *targetFileName);
			User::Leave(KErrSISInvalidTargetFile);		
			}

		// For operations other than filenull we need the drive to exist
		// and we need to check for invalid eclipsing.
		if (aFileDescription.Operation() != EOpNull) 
   			{
			// must be writable for ValidEclipseL
			TFileName installName = *targetFileName;

   			// check the target drive actually exists, and is mounted
			TInt driveNo;
			User::LeaveIfError(fs.CharToDrive(installName[0], driveNo));
			TVolumeInfo volInfo;
			if (KErrNone != fs.Volume(volInfo, driveNo))
				{
				DisplayErrorL(EUiDiskNotPresent, aFileDescription.Target().Data());
				User::Leave(KErrSISInvalidTargetFile);
				}
	
			// Check for invalid eclipsing of files already on the device.
			if (!ValidEclipseL(fs, installName, aApplication))
				{
				//The file is in ROM and not eclipsable
				DisplayErrorL(EUiAlreadyInRom, aFileDescription.Target().Data());
				User::Leave(KErrInvalidEclipsing);
				}
		
			// Check for clashes with other applications in the plan.
			CheckFilesFromPlannedControllersL(*targetFileName, aPlannedFiles);
			
			// Add file to list to check in subsequent applications.
			aPlannedFiles.AppendL(targetFileName);
			CleanupStack::Pop(targetFileName);

			aFilesToCapabilityCheck.AppendL(&aFileDescription);
			
			// check for plug-ins
			TInt len = KPluginPath().Length();
			if ((targetFileName->Length() >= len+2) && targetFileName->Mid(2,len).CompareF(KPluginPath) == 0)
				{
				Plan().SetContainsPlugins(ETrue);
				}
			// if it's an apparc file, record it in the plan for use later	
			AddApparcFilesToPlanL(targetFileName->Des());
   			}
		else
	   		{
   			CleanupStack::PopAndDestroy(targetFileName);
 	  		}

		CleanupStack::PopAndDestroy(&fs); // fs

		// it's a file to copy, add it to the current application
		if (!iTextSkip || aFileDescription.Operation() == EOpNull)
			{
			aApplication.AddFileL(aFileDescription, aApplication.UserSelections().Drive());
			Plan().AddInstallFileForProgress(aFileDescription.UncompressedLength());
			}
		else
			{
			aApplication.SkipFileOnInstallL(aFileDescription, aApplication.UserSelections().Drive());
			}				
		}
	
	
	//Add to files to run if we need to
	if (aFileDescription.Operation() == EOpRun)
		{
#ifdef SYMBIAN_SWI_RUN_ON_INSTALL_COMPLETE 

		// Can't run at install AND after install with the same file. FILEMIME compiles to FM,RI
		// we must block this or you can't get just (RUNAFTERINSTALL, FM) and this is the more 
		// common usecase. 
		if (aFileDescription.OperationOptions() & EInstFileRunOptionAfterInstall)
			{
			Plan().RunFileAfterInstallL(aFileDescription, aApplication.UserSelections().Drive(), aApplication.IsStub());
			}
		else if (aFileDescription.OperationOptions() & EInstFileRunOptionInstall)	
        	{			
			aApplication.RunFileOnInstallL(aFileDescription, aApplication.UserSelections().Drive());
			// running the app will force SWI to notify Apparc in advance so the current list
			// of files have already been processed.
			Plan().ResetAppArcRegFiles();
			}
#else 
        if (aFileDescription.OperationOptions() & EInstFileRunOptionInstall)	
			{			
			aApplication.RunFileOnInstallL(aFileDescription, aApplication.UserSelections().Drive());
			// running the app will force SWI to notify Apparc in advance so the current list
			// of files have already been processed.
			Plan().ResetAppArcRegFiles();
			}
#endif // SYMBIAN_SWI_RUN_ON_INSTALL_COMPLETE 
		}
	// Files to display
	else if (aFileDescription.Operation() == EOpText)
		{
		if ( !aApplication.IsStub() )
			aApplication.DisplayFileOnInstallL(aFileDescription, aApplication.UserSelections().Drive());
		}

	// Reset the iTextskip value as this is applicable for the next one file only.
	iTextSkip = EFalse;
	if (aFileDescription.OperationOptions() & EInstFileTextOptionSkipIfNo)
		{
		iTextSkip = ETrue;
		}
	}

void CInstallationPlanner::AddApparcFilesToPlanL(const TDesC& aTargetFileName)
	{
	_LIT(KApparcRegDir, "\\private\\10003a3f\\import\\apps\\");
	TParsePtrC filename(aTargetFileName);
	if (filename.Path().Left(KApparcRegDir().Length()).CompareF(KApparcRegDir) == 0)
		{
		// we're installing a reg file so add it to our list. 
		Plan().AddAppArcRegFileL(aTargetFileName);
		}
	}

void CInstallationPlanner::ProcessFilesToRemoveL(CApplication& aApplication, RSisRegistryWritableEntry& aRegistryEntry)
	{
	TBool romBasedFile(EFalse);
	if (aApplication.IsUpgrade() || aApplication.IsPartialUpgrade())
		{
		// There may be files to remove as part of the plan
		
		RPointerArray<CSisRegistryFileDescription> fileDescriptions;
		CleanupResetAndDestroy<RPointerArray<CSisRegistryFileDescription> >::PushL(fileDescriptions);
		aRegistryEntry.FileDescriptionsL(fileDescriptions);

		TInt count(fileDescriptions.Count());
		for (TInt i=0;i < count; ++i)
			{
			//Consider only non-ROM based files.
			CSisRegistryFileDescription* description = fileDescriptions[i];
			romBasedFile = ((description->Target().Mid(0,1) == _L("z")) ||
							(description->Target().Mid(0,1) == _L("Z")));
			// Skip filenull cases.
			if ((description->Operation() != EOpNull) && !romBasedFile)
				{
				// If this is either an SA upgrade or if its a PU
				// where the file is to be replaced, remove it first.
				if (aApplication.IsUpgrade())
					{
					DEBUG_PRINTF2(_L("Planning file for removal '%S'"), &(description->Target()));
					aApplication.RemoveFileL(*description);
					Plan().AddUninstallFileForProgress();
					}
				else
					{
					DEBUG_PRINTF2(_L("File '%S' may be legally overwritten"), &(description->Target()));
					// create a new copy and append it to the array
					CSisRegistryFileDescription* desc = CSisRegistryFileDescription::NewL(*description);
					CleanupStack::PushL(desc);
					iOverwriteableFiles.AppendL(desc);
					CleanupStack::Pop(desc);
					}
				}
			}
	
		CleanupStack::PopAndDestroy(&fileDescriptions);
		
		}
	}

// find the application is allowed to install based on SU Cert Rules if
// 1. if the sis file is signed by certificate trusted by the device (SU Cert)
// 2. if the sis file is of type SA/SP/PU with RU/RU+NR flag
// 3. the sis file validation status is trusted
TBool CInstallationPlanner::IsValidSystemUpgradeL(const CApplication& aApplication)
	{
	Sis::TInstallType type = aApplication.ControllerL().Info().InstallType();
	Sis::TInstallFlags flag = aApplication.ControllerL().Info().InstallFlags();
	TBool allowedType = (type == EInstInstallation || (type == EInstAugmentation) || type == EInstPartialUpgrade );
	TBool allowedFlag = (flag & EInstFlagROMUpgrade);
	// PA and PP is not allowed
	// Treat an SA/SP/PU on a removable media stub as a preinstalled apps/patch

	return (aApplication.ControllerL().IsSignedBySuCert() && allowedType && allowedFlag && 
			aApplication.ControllerL().TrustStatus().ValidationStatus()
			>= EValidatedToAnchor);
	}


// CInstallationPair::TUidApplicationPair functions

CInstallationPlanner::CPackageApplicationPair::CPackageApplicationPair(CApplication& aApplication)
	: iApplication(aApplication)
	{
	}

void CInstallationPlanner::CPackageApplicationPair::ConstructL(CSisRegistryPackage& aPackage)
	{
	iPackage=CSisRegistryPackage::NewL(aPackage);
	}

CInstallationPlanner::CPackageApplicationPair::~CPackageApplicationPair()
	{
	delete iPackage;
	}

CSisRegistryPackage& CInstallationPlanner::CPackageApplicationPair::Package() const
	{
	return *iPackage;
	}
	
CApplication& CInstallationPlanner::CPackageApplicationPair::Application()
	{
	return iApplication;
	}

CInstallationPlanner::CPackageApplicationPair* CInstallationPlanner::CPackageApplicationPair::NewLC(CSisRegistryPackage& aPackage, CApplication& aApplication)
	{
	CPackageApplicationPair* self=new(ELeave) CPackageApplicationPair(aApplication);
	CleanupStack::PushL(self);
	self->ConstructL(aPackage);
	return self;
	}

//CFilesFromPlannedController class functions
CInstallationPlanner::CFilesFromPlannedController::CFilesFromPlannedController(TUid aUid, TUid aParentUid, Sis::TInstallType aInstallType, Sis::CString* aName)
	:iUid(aUid),iParentUid(aParentUid),iCount(0),iInstallType(aInstallType),iName(aName)
	{
	}
	
CInstallationPlanner::CFilesFromPlannedController::~CFilesFromPlannedController()
	{
	iFileNames.ResetAndDestroy();
	}
	
CInstallationPlanner::CFilesFromPlannedController* CInstallationPlanner::CFilesFromPlannedController::NewLC(TUid aUid, TUid aParentUid, Sis::TInstallType aInstallType, Sis::CString* aName)
	{
	CFilesFromPlannedController* self = new (ELeave) CFilesFromPlannedController(aUid, aParentUid, aInstallType, aName);
	CleanupStack::PushL(self);
	return self;
	}

void CInstallationPlanner::CFilesFromPlannedController::AppendL(HBufC* aFileName)
	{
	iFileNames.AppendL(aFileName);
	++iCount;
	}

TUid CInstallationPlanner::CFilesFromPlannedController::Uid()
	{
	return iUid;
	}

TUid CInstallationPlanner::CFilesFromPlannedController::ParentUid()
	{
	return iParentUid;
	}
	
TInt CInstallationPlanner::CFilesFromPlannedController::Count()
	{
	return iCount;
	}
	
TChar CInstallationPlanner::CFilesFromPlannedController::Drive()
	{
	return iDrive;
	}
	
void CInstallationPlanner::CFilesFromPlannedController::SetDrive(TChar& aDrive)
	{
	iDrive = aDrive;
	}

Sis::CString* CInstallationPlanner::CFilesFromPlannedController::Name()
	{
	return iName;
	}

Sis::TInstallType CInstallationPlanner::CFilesFromPlannedController::InstallType()
	{
	return iInstallType;
	}

TBool CInstallationPlanner::CFilesFromPlannedController::IsMatched(const TDesC& aFileName)
	{
	TBool match = EFalse;
	TInt count = iFileNames.Count();
	for(TInt i=0; i<count; ++i)
		{
		if(!aFileName.CompareF(*iFileNames[i]))
			{
			match = ETrue;
			break;
			}
		}
	return match;
	}

TBool CInstallationPlanner::CFilesFromPlannedController::IsEclipsed(const TDesC& aFileName)
	{
	TBool eclipse = EFalse;
	TInt count = iFileNames.Count();
	for(TInt i=0; i<count; ++i)
		{
		if(aFileName.Mid(1).CompareF((*iFileNames[i]).Mid(1)) == 0)
			{
			eclipse = ETrue;
			break;
			}
		}
	return eclipse;
	}
	
const CController& CInstallationPlanner::CurrentController()
	{
	return iControllersToProcess[iCurrentController].Controller();
	}
	
const Sis::CController& CInstallationPlanner::MainController()
	{
	return iControllersToProcess[0].Controller();
	}

TInt CInstallationPlanner::GetControllerReference(TUid aUid)
	{
	TInt count = iControllersToProcess.Count();
	for(TInt i=0; i<count; ++i)
		{
		if(aUid == iControllersToProcess[i].Controller().Info().Uid().Uid())
			{
			return i;
			}
		}
	return KErrNotFound;
	}

void CInstallationPlanner::PlanCurrentControllerL(RPointerArray<CFileDescription>& aFilesToCapabilityCheck)
	{
	if(!iMainApplication)
		{
		// Plan main controller
		iMainApplication = ProcessControllerL(CurrentController(), 0, aFilesToCapabilityCheck, CurrentController().Info().Uid().Uid()); // Top-level controller has embedded data index of 0
		}
	else
		{
		CApplication* parentApplication = iControllersToProcess[iCurrentController].ParentApplication();
		User::LeaveIfNull(parentApplication);
		
		CApplication* application=ProcessControllerL(CurrentController(), parentApplication->AbsoluteDataIndex(), aFilesToCapabilityCheck, parentApplication->ControllerL().Info().Uid().Uid());
		CleanupStack::PushL(application);
		parentApplication->AddEmbeddedApplicationL(application); // ownership transferred
		CleanupStack::Pop(application);
		}
	}

TBool CInstallationPlanner::GetNextController()
	{
	// Find the next controller in our list of controllers
	if(++iCurrentController < iControllersToProcess.Count())
		{
		return ETrue;
		}
	else
		{
		return EFalse;	
		}
	}

void CInstallationPlanner::DisplayErrorL(TErrorDialog aType, const TDesC& aParam)
	{
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Display the error message only when the planner is not in info collection mode.
	if(!IsInInfoCollectionMode())
		{
		CDisplayError* displayError=CDisplayError::NewLC(Plan().AppInfoL(), aType, aParam);
		UiHandler().ExecuteL(*displayError);
		CleanupStack::PopAndDestroy(displayError);
		}
	#else
	CDisplayError* displayError=CDisplayError::NewLC(Plan().AppInfoL(), aType, aParam);
	UiHandler().ExecuteL(*displayError);
	CleanupStack::PopAndDestroy(displayError);
	#endif	
	}

void CInstallationPlanner::CheckFilesFromPlannedControllersL(const TDesC& aFileName, CFilesFromPlannedController& aPlannedFiles) const
	{
	TBool check = EFalse;
	
	// Check all files from already planned controllers to see if this file
	// will overwrite or eclipse.  Note that the array may contain files from
	// the current controller beyond the last index checked here; a single
	// package is allowed to overwrite or eclipse its own target files.
	TInt count = iFilesFromPlannedControllers.Count();	
	for(TInt i=0; i<count; ++i)
		{
		check = EFalse;
		
		if(aPlannedFiles.Uid() != iFilesFromPlannedControllers[i]->Uid())
			{//This is a different package so we should check files
			check = ETrue;
			}
		else
			{
			//Do not allow that two packages with the same UID are embedded by different base packages
			if(aPlannedFiles.ParentUid() != iFilesFromPlannedControllers[i]->ParentUid())
				{
				User::Leave(KErrAlreadyExists);
				}
			else
				{
				Sis::TInstallType plannedType = iFilesFromPlannedControllers[i]->InstallType();
				Sis::TInstallType currentType = aPlannedFiles.InstallType();
				
				if(currentType != plannedType)
					{
					//Augmentations can't overwrite files from the base package or its PU
					if((currentType == EInstAugmentation) || (plannedType == EInstAugmentation))
						{
						check = ETrue;
						}
					}
				else
					{
					//SA packages delivered within the embedding package can not be SA
					//upgraded from within the same package
					if(currentType == EInstInstallation)
						{
						User::Leave(KErrAlreadyExists);
						}
					//multiple partial upgrades to the base are supported
  					//multiple separate augmentations are supported
					}
				}
			}
			
		if(check)
			{
			if(iFilesFromPlannedControllers[i]->IsMatched(aFileName))
				{
				// File exactly matches one from another controller already
				// processed in this plan.
				User::Leave(KErrAlreadyExists);
				}
			else if(iFilesFromPlannedControllers[i]->IsEclipsed(aFileName))
				{
				// File would eclipse one from another controller already
				// processed in this plan.
				User::Leave(KErrInvalidEclipsing);
				}
			}
		}
	}

TBool CInstallationPlanner::IsAugmentationUpgradeL(const Sis::CController& aController, RSisRegistryEntry& aRegistryEntry,
		RSisRegistrySession& aRegistrySession)
	{
	TBool ret = EFalse;
	if (aController.Info().InstallType() == Sis::EInstAugmentation)
		{
		CSisRegistryPackage* package = CSisRegistryPackage::NewLC(aController.Info().Uid().Uid(),
			aController.Info().Names()[iDisplayLanguageIndex]->Data(),
			aController.Info().UniqueVendorName().Data());
		
		// first check this isn't exactly the same package name...
		CSisRegistryPackage* base = aRegistryEntry.PackageL();
		CleanupStack::PushL(base);
		if ((base->Uid() == package->Uid()) && (base->Name() == package->Name()))
			{
			// cannot upgrade a base package with an augmentation!
			User::Leave(KErrInvalidUpgrade);
			}
		CleanupStack::PopAndDestroy(base);
		
		// See if we can find this package in the augmentations to this entry
		RPointerArray<CSisRegistryPackage> augmentations;
		CleanupResetAndDestroyPushL(augmentations);
		aRegistryEntry.AugmentationsL(augmentations);
	
		TInt count(augmentations.Count());	
		for (TInt i = 0; i < count; ++i)
			{
			if ((package->Uid()  == augmentations[i]->Uid()) &&
				(package->Name()  == augmentations[i]->Name())) 
				{
				// close and reopen the registry entry as that for the augmentation
				aRegistryEntry.Close();
				aRegistryEntry.OpenL(aRegistrySession, *augmentations[i]);
			
				ret = ETrue;
				break;
				}
			}
		CleanupStack::PopAndDestroy(2, package); // augmentations
		}
		
	return ret;
	}	

TBool CInstallationPlanner::IsAugmentationUpgrade(const Sis::CController& aController)
	{
	TBool isUpgrade = EFalse;
	TInt count = iFilesFromPlannedControllers.Count();
	for(TInt i=0; i<count; i++)
		{
		if((aController.Info().Uid().Uid() == iFilesFromPlannedControllers[i]->Uid())
		   && (iFilesFromPlannedControllers[i]->InstallType() == EInstAugmentation))
			{
			//If augmentation has the same name with another augmentation in the list, it's a upgrade
			if(!iFilesFromPlannedControllers[i]->Name()->Data().CompareF(aController.Info().Names()[0]->Data()))
				{
				isUpgrade = ETrue;
				}	
			}
		}
	
	return isUpgrade;
	}
	
//Checks whether an SP/PU package's base package has already been planned		
TBool CInstallationPlanner::IsPlannedControllerL(const Sis::CController& aController, TInt& aIndex, TUid aParentUid)
	{
	TBool inList = EFalse;
	TInt i;
	for(i=0; i<iFilesFromPlannedControllers.Count(); i++)
		{
		if(aController.Info().Uid().Uid() == iFilesFromPlannedControllers[i]->Uid())
			{
			if(aController.Info().InstallType() == EInstAugmentation)
				{
				//If augmentation has the same name with the base package in the list, it's an invalid upgrade
				if(!iFilesFromPlannedControllers[i]->Name()->Data().CompareF(aController.Info().Names()[0]->Data())
				   && (iFilesFromPlannedControllers[i]->InstallType() == EInstInstallation))
					{
					User::Leave(KErrInvalidUpgrade);
					}
				}
			//Augmentations or partial upgrades must be embedded by the same base pacakge				
			if(aParentUid == iFilesFromPlannedControllers[i]->ParentUid())
				{
				inList = ETrue;
				break;	
				}
			}
		}
	aIndex = i;
	return inList;
	}

/** This function takes a fully qualified name and searches the filesystem through for all possible adorned filename matches.
 *  Besides that it calculates the unadorned version of the input filename as well and return it through the corresponding given 
 *  input reference variable.
 *  @param aFs A valid file server session handle
 *  @param aFileName The fully qualified filename (full path and name)
 *  @param aUnadornedName The calulated unadorned name is returned through this variable(full path and unadorned name calculated from aFileName)
 *  @param aAdornedFileNamesFound All the found adorned name matches are returned through this pointer array to descriptors
 */
void CInstallationPlanner::AdornedProcessingOfFileL(RFs& aFs, TDesC& aFileName, TDes& aUnadornedName, RPointerArray<HBufC>& aAdornedFileNamesFound)
	{
    TParsePtrC parsedFileName(aFileName);
	TInt originalNameAndExtLength = parsedFileName.NameAndExt().Length();
	RBuf searchNameWild;
	searchNameWild.CreateL(originalNameAndExtLength+Swi::FileNameUnadornedPartLength); //searchname wild is max 10 chars longer than original filename+ext
	searchNameWild.CleanupClosePushL();
	RBuf unadornedName;
	unadornedName.CreateL(originalNameAndExtLength); //unadorned name is never longer than original name
	unadornedName.CleanupClosePushL();

    Swi::GetUnadornedFileName(parsedFileName.NameAndExt(), unadornedName);
	aUnadornedName = parsedFileName.DriveAndPath();
	aUnadornedName.Append(unadornedName);
	Swi::GenerateSearchNameWildL(parsedFileName.NameAndExt(), searchNameWild);
	Swi::FindAllAdornedVariantsL(aFs, searchNameWild, parsedFileName.Path(), aAdornedFileNamesFound);
	CleanupStack::PopAndDestroy(2, &searchNameWild);
	}

/** This function handles overwriting situation (i.e: we are trying to install c:\somedir\somename.ext however it already exist)
 *  In this case we have two possible scenarios:
 *  1.) We are correctly overwriting the already present file (a PU)
 *  2.) We are incorrectly overwriting the file present (in this case the error will be caught later in CPostrequisitesChecker::WouldOverWriteL)
 *  @param aFileName The fully qualified filename (full path and name)
 *  @param aApplication installation plan
 */
void CInstallationPlanner::HandleFileOverwritingL(TDesC& aFileName, CApplication& aApplication)
	{
	// Case 1.1 is handled here - we'll uninstall the old file if we have the right to do it.
	// Case 1.2 will be handled in CPostrequisitesChecker::WouldOverWriteL, as the error will be caught there


	// sis file allowed to install based on SU Cert Rules and not of SP type 
	// are allowed to overwrite files owned by other packages. Add it to the plan for removal
	// Files are not removed at install time if is a propagation.
	TBool spInstall = (aApplication.ControllerL().Info().InstallType() == Sis::EInstAugmentation);		
	if (iIsValidSystemUpgrade && !spInstall && !iIsPropagated)
		{
		aApplication.RemoveFileL(aFileName);
		Plan().AddUninstallFileForProgress();
		}

	TInt count(iOverwriteableFiles.Count());
	for (TInt i = 0; i < count; i++)
		{
		CSisRegistryFileDescription* description = iOverwriteableFiles[i];
		if (aFileName.CompareF(description->Target()) == 0 && (description->Operation() != EOpNull))
			{
			aApplication.RemoveFileL(*description);
			Plan().AddUninstallFileForProgress();
			}
		}
	}

// store references to supplied Controller and application
TControllerAndParentApplication::TControllerAndParentApplication(
		const CController& aController, CApplication* aParentApplication) :
		iController(aController), iParentApplication(aParentApplication)
			{
			
			}
			
const CController& TControllerAndParentApplication::Controller()
	{
	return iController;
	}

CApplication* TControllerAndParentApplication::ParentApplication()
	{
	return iParentApplication;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
void CInstallationPlanner::SetInInfoCollectionMode(TBool aMode)
	{
	iIsInInfoCollectionMode = aMode;
	}
	
TBool CInstallationPlanner::IsInInfoCollectionMode()
	{
	return iIsInInfoCollectionMode;	
	}
#endif
