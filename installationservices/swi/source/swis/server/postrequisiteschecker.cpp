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
* Definition of the CPostrequisitesChecker
*
*/


/**
 @file
*/


#include "postrequisiteschecker.h"

#include "sisprerequisites.h"
#include "siscontroller.h"
#include "sisfiledescription.h"
#include "sisstring.h"
#include "sisdependency.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "siscontroller.h"
#include "sisinstallblock.h"
#include "sisversionrange.h"
#include "siscontentprovider.h"

#include "sisregistryentry.h"
#include "sisinstallerrors.h"

#include <swi/msisuihandlers.h>

#include "plan.h"
#include "application.h"
#include "userselections.h"

#include "sisregistryfiledescription.h"
#include "sisregistrypackage.h"

#include "cleanuputils.h"
#include "log.h"

#include "sisuihandler.h"
#include "securitycheckutil.h"
#include "securitypolicy.h"

using namespace Swi;
using namespace Swi::Sis;

/*static*/ CPostrequisitesChecker* CPostrequisitesChecker::NewLC(
	RUiHandler& aInstallerUI, const CPlan& aPlan,
	CInstallationResult& aInstallationResult, const CContentProvider& aProvider)
	{
	CSecurityPolicy* securityPolicy=CSecurityPolicy::GetSecurityPolicyL();
		
	CPostrequisitesChecker* self =
		new(ELeave) CPostrequisitesChecker(aInstallerUI, aPlan,
										   aInstallationResult,
										   aProvider,
										   securityPolicy->AllowOrphanedOverwrite(),
										   securityPolicy->AllowProtectedOrphanOverwrite());
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CPostrequisitesChecker::CPostrequisitesChecker(
	RUiHandler& aInstallerUI, const CPlan& aPlan,
	CInstallationResult& aInstallationResult, const CContentProvider& aProvider,
	TBool aAllowOrphanedOverwrite, TBool aAllowPrivateOrphanOverwrite)
	: CRequisitesChecker(aInstallerUI, aInstallationResult, aProvider), iPlan(aPlan)
	, iAllowOrphanedOverwrite(aAllowOrphanedOverwrite)
	, iAllowPrivateOrphanOverwrite(aAllowPrivateOrphanOverwrite)
	{
	}

CPostrequisitesChecker::~CPostrequisitesChecker()
	{
	}

void CPostrequisitesChecker::ConstructL() 
	{
	CRequisitesChecker::ConstructL();
	}

void CPostrequisitesChecker::CheckPostrequisitesL()
	{
	CheckDependenciesL();
	CheckOverWritesL ();
	}

void CPostrequisitesChecker::CheckDependenciesL()	
	{
	DoCheckDependenciesL(iPlan.ApplicationL());
	}

void CPostrequisitesChecker::DoCheckDependenciesL(const CApplication& aApplication)	
	{
	if (aApplication.IsUninstall())	// If it's an uninstall then we don't want to consider
		{										// this package or any packages below this one when 
		return;								// checking dependencies as they will all be uninstalled.
		}
	
	for (TInt i=0; i<aApplication.EmbeddedApplications().Count(); ++i)
		{
		DoCheckDependenciesEmbeddedAppL(*aApplication.EmbeddedApplications()[i]);
		}

	const CController& controller = aApplication.ControllerL();
	const CPrerequisites& reqs = controller.Prerequisites();

	if(!AllOnDeviceOrPlanL(reqs.Dependencies(), iPlan))
		{
		User::Leave(KErrSISPrerequisitesMissingDependency);
		}
	}

void CPostrequisitesChecker::DoCheckDependenciesEmbeddedAppL(const CApplication& aApplication)	
	{
	if (aApplication.IsUninstall())	// If it's an uninstall then we don't want to consider
		{										// this package or any packages below this one when 
		return;								// checking dependencies as they will all be uninstalled.
		}
	
	for (TInt i=0; i<aApplication.EmbeddedApplications().Count(); ++i)
		{
		DoCheckDependenciesEmbeddedAppL(*aApplication.EmbeddedApplications()[i]);
		}

	const CController& controller = aApplication.ControllerL();
	const CPrerequisites& reqs = controller.Prerequisites();

	
	if (!AnyOnDeviceL(reqs.TargetDevices()))
		{
		User::Leave(KErrSISPrerequisitesMissingDependency);
		}
	
	if(!AllOnDeviceOrPlanL(reqs.Dependencies(), iPlan))
		{
		User::Leave(KErrSISPrerequisitesMissingDependency);
		}
	}

void CPostrequisitesChecker::CheckOverWritesL()
	{
	DoCheckOverWritesL(iPlan.ApplicationL());	
	}

void CPostrequisitesChecker::DoCheckOverWritesL(const CApplication& aApplication)
	{
	TBool ignoreCheck = EFalse;
	for (TInt i = 0; i < aApplication.EmbeddedApplications().Count(); ++i)
		{
		TBool spInstall = (aApplication.ControllerL().Info().InstallType() == Sis::EInstAugmentation);
		if (aApplication.IsInstallSuCertBased() && !spInstall)
			{
			ignoreCheck = ETrue;
			}
		DoCheckOverWritesL(*aApplication.EmbeddedApplications()[i]);
		}

	// For each application to be installed, we check if it will overwrite
	// a file.  If it does, the application UID must match the UID of the
	// application that installed the file
	if (!ignoreCheck && WouldOverWriteL(aApplication))
		{
		User::Leave(KErrSISWouldOverWrite);
		}
	}

TBool CPostrequisitesChecker::FileRemovedInPlanL(TFileName& aFileName)
	{
	return FileRemovedInApplication(iPlan.ApplicationL(), aFileName);
	}
	
TBool CPostrequisitesChecker::FileRemovedInApplication(const CApplication& aApplication, TFileName& aFileName)
	{
	TInt i;
	for (i = 0; i<aApplication.EmbeddedApplications().Count(); ++i)
		{
		if (FileRemovedInApplication(*aApplication.EmbeddedApplications()[i], aFileName))
			{
			return ETrue;
			}
		}

	for (i = 0 ; i < aApplication.FilesToRemove().Count(); ++i)
		{
		if (0 == aFileName.CompareF(aApplication.FilesToRemove()[i]->Target()))
			{
			return ETrue;
			}
		}
	return EFalse;
	}

CSisRegistryPackage* CPostrequisitesChecker::GetOwnerPackageL(const TDesC& aFileName)
	{
	// check all registry entries for the file
	RPointerArray<CSisRegistryPackage> packages;		
	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(packages);
	iSisRegistrySession.InstalledPackagesL(packages);
	
	// for all registered packages...
	for (TInt i=0; i< packages.Count(); ++i)
		{
		RSisRegistryEntry entry;
		entry.OpenL(iSisRegistrySession, *packages[i]);
		CleanupClosePushL(entry);
		
		RPointerArray<HBufC> files;		
		CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(files);
		entry.FilesL(files);

		// ..and each file in each package...
		for (TInt j=0; j< files.Count(); ++j)
			{
			// .. see if our file is owned
			if (files[j]->CompareF(aFileName)==0)
				{
				// get the owning package
				CSisRegistryPackage* aOwner =  CSisRegistryPackage::NewL(*packages[i]);				
				CleanupStack::PopAndDestroy(3, &packages);
				return aOwner; // file is owned
				}
			}

		CleanupStack::PopAndDestroy(2, &entry);		
		}
	
	CleanupStack::PopAndDestroy(&packages);
	return NULL; // file is NOT owned by a package
	}

void CPostrequisitesChecker::GetUnadornedFileName(const CSisRegistryFileDescription& aFileDescription, TFileName* aFilename)
	{
	*aFilename = aFileDescription.Target();
		
	// Check this name is not adorned, if it is remove version no
 	TInt startVersion = aFilename->Find(_L("{"));
 	if (startVersion != KErrNotFound)
 	{ 		
		TInt endVersion = aFilename->Find(_L("}"));
		if (( endVersion != KErrNotFound ) && (endVersion - startVersion == 9))
			{
			// get the name without the version
			TFileName unadorned = aFilename->Left(startVersion);
			unadorned.Append(aFilename->Mid(endVersion+1));
		 	*aFilename = unadorned;
			}
		}
	}


TBool CPostrequisitesChecker::WouldOverWriteL(const CApplication& aApplication)
	{
	TBool result(EFalse);
	TInt fileCount = aApplication.FilesToAdd().Count();
	if (fileCount == 0)
		{
		return EFalse;
		}
	
	if (aApplication.IsUninstall()) // If it's an uninstall then there are no files to check
		{
		return EFalse;
		}

	// create an BIT array of the orphanage state of the files we plan
	// to overwrite...
	RArray<TInt32> orphaned;	
	CleanupClosePushL(orphaned);

	// assume all files are orphaned to start with
	TInt count = (fileCount/32)+1;
	for (TInt i=0; i<count; ++i) orphaned.AppendL(0xFFFFFFFF);

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
	for (TInt k=0; k<fileCount; ++k)
		{
		const CSisRegistryFileDescription& fileDescription = *aApplication.FilesToAdd()[k];
		TInt index = k/32;
		TInt bit = 1<<(k%32);		
		if (fileDescription.Operation() != EOpNull && iSisRegistrySession.IsFileRegisteredL(fileDescription.Target()))
			{
			// found a match so file is NOT an orphan ... clear the orphanage state
			orphaned[index] &= (bit^0xFFFFFFFF);
			}		
		}
#else //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
	// setup the registry access 
	RPointerArray<CSisRegistryPackage> packages;		
	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(packages);
	iSisRegistrySession.InstalledPackagesL(packages);
	
	// for each already installed package....
	for (TInt i=0; i< packages.Count(); ++i)
		{
		RSisRegistryEntry entry;
		entry.OpenL(iSisRegistrySession, *packages[i]);
		CleanupClosePushL(entry);
		
		RPointerArray<HBufC> files;		
		CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(files);
		entry.FilesL(files);
		
		// ...and each file inside each package...
		for (TInt j=0; j< files.Count(); ++j)
			{
			// check against the known set of files we want to (possibly) overwrite
			for (TInt k=0; k<fileCount; ++k)
				{
				TInt index = k/32;
				TInt bit = 1<<(k%32);
				const CSisRegistryFileDescription& fileDescription = *aApplication.FilesToAdd()[k];
				
				// only check if we don't know its already orphaned
				if ((orphaned[index]&bit) != 0 && fileDescription.Operation() != EOpNull)
					{
					if (files[j]->CompareF(fileDescription.Target())==0)
						{
						// found a match so file is NOT an orphan ... clear the orphanage state
						orphaned[index] &= (bit^0xFFFFFFFF);
						}
					}
				}
			}
		CleanupStack::PopAndDestroy(2, &entry);		
		}
	CleanupStack::PopAndDestroy(&packages);
#endif
	
	// Process the files we plan to install to see if any will overwrite a file 
	// owned by an already installed package
	for (TInt i=0; (i<fileCount) && (result==EFalse); ++i)
		{
		// get file's orphanage state
		TInt index = i/32;
		TInt bit = 1<<(i%32);
		TBool isOrphaned = ((orphaned[index]&bit)!=0);
		
		const CSisRegistryFileDescription& fileDescription = *aApplication.FilesToAdd()[i];
			
		TFileName filename = fileDescription.Target();

		if(fileDescription.Operation() == EOpNull)
			continue;
		
		TEntry* fileEntryPtr = new (ELeave) TEntry;
		TInt err = iFs.Entry (filename, *fileEntryPtr);
		delete fileEntryPtr;
		fileEntryPtr = NULL;
		
		
		// if we could open the filename successfully, or it is in use then
		// we might overwrite it!
		if(err == KErrNone || err == KErrInUse)
			{
			TBool fileRemovedInPlan = FileRemovedInPlanL(filename);	

			// Pre-installed apps or pre-installed patches cannot
			// "install" existing files which are owned by another
			// package. Leave with KErrSISInvalidTargetFile in this case
			if (aApplication.IsPreInstalledApp() || aApplication.IsPreInstalledPatch())
				{
				if (!isOrphaned)
					{
					DEBUG_PRINTF(_L8("PostRequisites - Error, preinstalled package attempting to adopt a non-orphaned file!"));
					User::Leave(KErrSISInvalidTargetFile);
					}
				}

			// If we still don't think it's allowed and we're allowed to ask the user about
			// orphaned file overwrites, then check if it's orphaned and then ask the user about it if so
			else if (!fileRemovedInPlan && iAllowOrphanedOverwrite && isOrphaned)
				{
				if (iAllowPrivateOrphanOverwrite || !SecurityCheckUtil::IsProtectedDirectoryL(filename))
					{
					// ask about orphaned files
					CDisplayQuestion* cmd = CDisplayQuestion::NewLC(iPlan.AppInfoL(), EQuestionOverwriteFile, filename);
					iInstallerUI.ExecuteL(*cmd);
					if (!cmd->ReturnResult())
					 	{
					 	result=ETrue; // Just asked so don't want to set allowOverwrite and display dialog again
					 	}
					// Plan orphaned file for removal.  Need to cast away
					// constness to add a file for removal to the application,
					// and to update the final progress bar count for the plan.
					const_cast<CApplication&>(aApplication).RemoveFileL(fileDescription.Target());
					const_cast<CPlan&>(iPlan).AddUninstallFileForProgress();
					CleanupStack::PopAndDestroy(cmd);
					}
				else
					{
					result = ETrue;
					}
				}
			else if (!fileRemovedInPlan)
				{
				// get the owning UID (if any)					
				if (!iAllowOrphanedOverwrite && !isOrphaned)
					{
					CSisRegistryPackage* ownerPackage = GetOwnerPackageL(filename);
					ASSERT(ownerPackage!=NULL);
					CleanupStack::PushL(ownerPackage);
					result = !DisplayProblemFileL(iPlan, filename, ownerPackage);
					CleanupStack::PopAndDestroy(ownerPackage);
					}
				else
					{
					result = !DisplayProblemFileL(iPlan, filename, NULL);						
					}
				}
			}
		else if (err != KErrNotFound && err != KErrPathNotFound)
			{
			User::Leave(err);
			}
		}
		
	CleanupStack::PopAndDestroy(&orphaned);

	return result;
	}
