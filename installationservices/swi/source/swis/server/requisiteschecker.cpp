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


#include "requisiteschecker.h"

#include "sisregistryentry.h"
#include "checkedversion.h"
#include "plan.h"
#include "application.h"
#include "msisuihandlers.h"

#include "sisdependency.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "siscontroller.h"
#include "sisinstallblock.h"
#include "sisversionrange.h"
#include "siscontentprovider.h"
#include "securitypolicy.h"
#include "sisregistrypackage.h"
#include "log.h"
#include "siselseif.h"
using namespace Swi;
using namespace Swi::Sis;

CRequisitesChecker::CRequisitesChecker(RUiHandler& aInstallerUI, CInstallationResult& aInstallationResult, const CContentProvider& aProvider)
:	iInstallationResult(aInstallationResult),
	iProvider(aProvider),
	iInstallerUI(aInstallerUI)
	{
	}

void CRequisitesChecker::ConstructL()
	{
	User::LeaveIfError(iSisRegistrySession.Connect());
	User::LeaveIfError(iFs.Connect());
	}

CRequisitesChecker::~CRequisitesChecker()
	{
	iSisRegistrySession.Close();
	iFs.Close();
	}
	
TBool CRequisitesChecker::InRange(TCheckedVersion aVersion, const Sis::CVersionRange* aVersionRange)
	{
	if (!aVersionRange)
		{
		return ETrue;
		}
	TCheckedVersion from(aVersionRange->From());
	if (aVersion >= from)
		{
		if (!aVersionRange->To())
			{
			return ETrue;
			}
		TCheckedVersion to(*aVersionRange->To());
		if (aVersion <= to)
			{
			return ETrue;
			}
		}
	return EFalse;
	}
	
TBool CRequisitesChecker::OnDeviceL(const CDependency& aDependency, TBool& aMismatch, TVersion& aMismatchedVersion)
	{
	TUid uid = aDependency.Uid().Uid();
	
	if (iSisRegistrySession.IsInstalledL(uid))
		{
		// Skip fetching the registry entry if there's nothing to check against
		if (aDependency.VersionRange() == (CVersionRange*)0)
			{
			return ETrue;
			}
		RSisRegistryEntry entry;
		User::LeaveIfError(entry.Open(iSisRegistrySession, uid));
		CleanupClosePushL(entry);
		TCheckedVersion installedVersion(entry.VersionL());
		CleanupStack::PopAndDestroy(&entry);

		if (InRange(installedVersion, aDependency.VersionRange()))
			{
			return ETrue;
			}
		aMismatch = ETrue;
		aMismatchedVersion = installedVersion;
		}
	return EFalse;
	}


void CRequisitesChecker::InSIS(const CDependency& aDependency, const Sis::CController& aController, TBool& aMismatch, TVersion& aMismatchedVersion, TBool& aFoundDependency)
	{
	if (!aFoundDependency)
		{
		// Is the dependency satisfied by this top-level controller
		const CInfo& info = aController.Info();
		if (info.Uid().Uid() == aDependency.Uid().Uid())
			{
			if (InRange(aController.Info().Version(), aDependency.VersionRange()))
				{
				aFoundDependency=ETrue;
				return;
				}
			aMismatch = ETrue;
			aMismatchedVersion.iMajor = info.Version().Major();
			aMismatchedVersion.iMinor = info.Version().Minor();
			aMismatchedVersion.iBuild = info.Version().Build();
			}
		
		// process possible embedded controllers in the expressions
		const CInstallBlock& installBlock=aController.InstallBlock();
		for (TInt i = 0 ; i < installBlock.IfStatements().Count(); ++i)
			{
			Sis::CIf* ifBlock=installBlock.IfStatements()[i];

			if (ifBlock)
				{
				//Handle if block			
				InEmbeddedSIS(aDependency, ifBlock->InstallBlock(), aMismatch, aMismatchedVersion, aFoundDependency);
				
				//Handle else if blocks
				const RPointerArray<CElseIf>& elseIfs=ifBlock->ElseIfs();
				for (TInt j = 0 ; j < elseIfs.Count(); ++j)
					{				
					if (elseIfs[j])
						{
						InEmbeddedSIS(aDependency, elseIfs[j]->InstallBlock(), aMismatch, aMismatchedVersion, aFoundDependency);
						}
					}
				}
			}
			
		// Is the dependency satisfied by a controller immediately embedded by this controller
		InEmbeddedSIS(aDependency, aController.InstallBlock(), aMismatch, aMismatchedVersion, aFoundDependency);
		}
	}

void CRequisitesChecker::InEmbeddedSIS(const CDependency& aDependency, const CInstallBlock& aInstallBlock, TBool& aMismatch, TVersion& aMismatchedVersion, TBool& aFoundDependency)
	{
	const RPointerArray<CController>& embeddeds = aInstallBlock.EmbeddedControllers();
	for (TInt i=0; i<embeddeds.Count(); i++)
		{
		if (!aFoundDependency)
			{
			InSIS(aDependency, *(embeddeds[i]), aMismatch, aMismatchedVersion, aFoundDependency);
			}
		}
	}

TBool CRequisitesChecker::InPlanL(const CDependency& aDependency, const CPlan& aPlan, TBool& aMismatch, TVersion& aMismatchedVersion)
	{
	return DoInPlanL(aPlan, aPlan.ApplicationL(), aDependency, aMismatch, aMismatchedVersion);
	}

TBool CRequisitesChecker::DoInPlanL(const CPlan& aPlan, const CApplication& aApplication, const CDependency& aDependency, TBool& aMismatch, TVersion& aMismatchedVersion)
	{
	if (aApplication.IsUninstall())	// If it's an uninstall then we don't want to consider
		{										// this package or any packages below this one when 
		return EFalse;						// checking dependencies as they will all be uninstalled.
		}

	TInt count = aApplication.EmbeddedApplications().Count();
	for (TInt i=0; i<count; ++i)
		{
		if (DoInPlanL(aPlan, *aApplication.EmbeddedApplications()[i], aDependency, aMismatch, aMismatchedVersion))
			{
			return ETrue;
			}
		}

	if(!ControllerOptional(aPlan.ApplicationL().ControllerL(), aApplication.ControllerL()))
		{
		return ETrue; // will already have been checked by CPrerequisitesChecker
		}

	const CInfo& info = aApplication.ControllerL().Info();
	if (info.Uid().Uid() == aDependency.Uid().Uid())
		{
		if (InRange(info.Version(), aDependency.VersionRange()))
			{
			return ETrue;
			}
		aMismatch = ETrue;
		aMismatchedVersion.iMajor = info.Version().Major();
		aMismatchedVersion.iMinor = info.Version().Minor();
		aMismatchedVersion.iBuild = info.Version().Build();
		}
	return EFalse;
	}

TBool CRequisitesChecker::ControllerOptional(const Sis::CController& aController, const Sis::CController& aEmbeddedController)
	{
	// the top level controller cannot be optional
	if(&aController == &aEmbeddedController)
		{
		return EFalse;
		}
	
	// search controllers immediately embedded by this controller
	if(aController.InstallBlock().EmbeddedControllers().Find(&aEmbeddedController) != KErrNotFound)
		{
		return EFalse;
		}
	
	// search for this controller at the next level of embedding
	TInt count = aController.InstallBlock().EmbeddedControllers().Count();
	for (TInt i = 0; i < count; i++)
		{
		if(!ControllerOptional(*aController.InstallBlock().EmbeddedControllers()[i], aEmbeddedController))
			{
			return EFalse;
			}
		}
	return ETrue;
	}

TBool CRequisitesChecker::AnyOnDeviceL(const RPointerArray<CDependency>& aDependencies)
	{
	TBool mismatch = EFalse;
	TVersion mismatchedVersion;
	TUint dependencyCount = aDependencies.Count();
	if (dependencyCount == 0)
		{
		// Check software install security policy to see if a SIS file should be
		// considered compatible if no target device dependencies are present.
		CSecurityPolicy* secPolicy = CSecurityPolicy::GetSecurityPolicyL();
		TBool emptyTargetListIsCompatible = secPolicy->SISCompatibleIfNoTargetDevices();
		//CleanupStack::PopAndDestroy(&securityPolicy);
		
		if (emptyTargetListIsCompatible)
		    return ETrue;
		}
	else 
		{
		for (TUint i=0; i<dependencyCount; i++)
			{
			if (OnDeviceL(*(aDependencies[i]), mismatch, mismatchedVersion))
				{
				return ETrue;
				}
			}
		}

	return DisplayDeviceIncompatibilityL();
	}

TBool CRequisitesChecker::AllOnDeviceOrSISL(const RPointerArray<CDependency>& aDependencies, const Sis::CController& aController)
	{
	TUint dependencyCount = aDependencies.Count();
	for (TUint i=0; i<dependencyCount; i++)
		{
		const Sis::CDependency& dependency = *(aDependencies[i]);
		TBool mismatch = EFalse;
		TVersion mismatchedVersion;
		if (!OnDeviceL(dependency, mismatch, mismatchedVersion))
			{
			TBool foundDependency=EFalse;
			InSIS(dependency, aController, mismatch, mismatchedVersion, foundDependency);
			if (!foundDependency)
				{
				if (ShowDependencyDialogL(dependency, mismatch, mismatchedVersion)==EFalse)
					{
					return EFalse;
					}
				}
			}
		}
	return ETrue;
	}

TBool CRequisitesChecker::AllOnDeviceOrPlanL(const RPointerArray<CDependency>& aDependencies, const CPlan& aPlan)
	{
	TUint dependencyCount = aDependencies.Count();
	for (TUint i=0; i<dependencyCount; i++)
		{
		const Sis::CDependency& dependency = *(aDependencies[i]);
		TBool mismatch = EFalse;
		TVersion mismatchedVersion;
		if (!OnDeviceL(dependency, mismatch, mismatchedVersion))
			{
			if (!InPlanL(dependency, aPlan, mismatch, mismatchedVersion))
				{
				if (ShowDependencyDialogL(dependency, mismatch, mismatchedVersion)==EFalse)
					{
					return EFalse;
					}
				}
			}
		}
	return ETrue;
	}



		
TBool CRequisitesChecker::DisplayDeviceIncompatibilityL()
	{
	TAppInfo appInfo(iProvider.DefaultLanguageAppInfoL());
	CDisplayDeviceIncompatibility* cmd = CDisplayDeviceIncompatibility::NewLC(appInfo);
	iInstallerUI.ExecuteL(*cmd);
	TBool retval=cmd->ReturnResult();
	CleanupStack::PopAndDestroy(cmd);
	return retval;
	}



TBool CRequisitesChecker::ShowDependencyDialogL(const Sis::CDependency& aDependency, TBool aVersionMismatch, TVersion aMismatchedVersion)
	{
	TAppInfo appInfo(iProvider.DefaultLanguageAppInfoL());
	TVersion fromVersion;
	TVersion toVersion;
	TBool fromValid(EFalse);
	TBool toValid(EFalse);
	const CVersionRange* range=aDependency.VersionRange();
	if (range)
		{
		fromValid=ETrue;
		fromVersion.iMajor=range->From().Major();
		fromVersion.iMinor=range->From().Minor();
		fromVersion.iBuild=range->From().Build();
		const CVersion* to=range->To();
		if (to)
			{
			toValid=ETrue;
			toVersion.iMajor=to->Major();
			toVersion.iMinor=to->Minor();
			toVersion.iBuild=to->Build();
			}
		}
	
	// Localise dependency name
	TLanguage systemLanguage = User::Language();    // Language of the current locale
    // Check whether the current locale is in the list of languages in the SIS file
    TInt languageIndex = iProvider.AvailableLanguages().Find(systemLanguage);
    if (languageIndex == KErrNotFound) 
    	{
	    languageIndex = 0;
        }	
	const TDesC& name(aDependency.Names()[languageIndex]->Data());
	CDisplayDependencyWarning* cmd = CDisplayDependencyWarning::NewLC(
		appInfo, name,
		fromValid, fromVersion,
		toValid, toVersion,
		aVersionMismatch, aMismatchedVersion);
	iInstallerUI.ExecuteL(*cmd);
	TBool retval=cmd->ReturnResult();
	CleanupStack::PopAndDestroy(cmd);
	return retval;
	}

TBool CRequisitesChecker::DisplayProblemFileL(const CPlan& aPlan, TFileName& aFilename, CSisRegistryPackage* aPackage)
	{
	TBool retval;

	if (aPackage == 0)
		{
		
		// No owning pacakge found, just call the dialog with blank app info
	
		CDisplayCannotOverwriteFile* cmd = CDisplayCannotOverwriteFile::NewLC(
			aPlan.AppInfoL(), TAppInfo(), aFilename);
		iInstallerUI.ExecuteL(*cmd);
		retval=cmd->ReturnResult();
		CleanupStack::PopAndDestroy(cmd);
		
		}
	else
		{
		CleanupStack::PushL((CBase *)aPackage);
		
		// There is an owning application, get its details
		RSisRegistryEntry entry;
		entry.OpenL(iSisRegistrySession, *aPackage);
		CleanupClosePushL(entry);
		
		HBufC* name =  entry.PackageNameL();
		CleanupStack::PushL(name);
		
		HBufC* vendor = entry.UniqueVendorNameL();
		CleanupStack::PushL(vendor);
		
		TVersion version = entry.VersionL();
		
		TAppInfo presentOwner(*name, *vendor, version);
		CDisplayCannotOverwriteFile* cmd = CDisplayCannotOverwriteFile::NewLC(
			aPlan.AppInfoL(), presentOwner, aFilename);
		iInstallerUI.ExecuteL(*cmd);
		retval=cmd->ReturnResult();
		CleanupStack::PopAndDestroy(4, &entry); // name, vendor, cmd
		
		CleanupStack::Pop(aPackage);
		}
		
	return retval;
	}

