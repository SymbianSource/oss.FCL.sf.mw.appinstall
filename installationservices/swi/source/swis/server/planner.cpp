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

#include "planner.h"
#include "application.h"
#include "plan.h"

#include "siscontentprovider.h"
#include "sisinstallblock.h"
#include "siscontroller.h"
#include "sisstring.h"
#include "sisinstallationresult.h"
#include "sissupportedlanguages.h"
#include "sislanguage.h"

#include <swi/msisuihandlers.h>

#include "sisregistryfiledescription.h"
#include "sisuid.h"
#include "sisinfo.h"
#include "dessisdataprovider.h"

#include "cleanuputils.h"

#include <swi/msisuihandlers.h>

#include "log.h"
#include "securitycheckutil.h"

#include <f32file.h>

#include "securitymanager.h"

#include "sisregistrydependency.h"
#include "uninstallationnode.h"

using namespace Swi;
using namespace Swi::Sis;

/**
 * Utility Function: This function will append the source array into the target array 
 */ 
template<typename T>
void AppendArrayL(RPointerArray<T>& aTarget, RPointerArray<T>& aSource)
	{
	for (TInt i=0; i < aSource.Count(); ++i)
		{
		aTarget.AppendL(aSource[i]); 
		}
	}


/**
 * This creates a new CPlanner object.
 * @param aUiHandler An implementation of the UI
 * @param aResult    Result of uninstallation
 * @return           New object
 */
CPlanner* CPlanner::NewL(RUiHandler& aUiHandler, CInstallationResult& aResult)
	{
	CPlanner* self = CPlanner::NewLC(aUiHandler, aResult);
	CleanupStack::Pop(self);
	return self;
	}

/**
 * This creates a new CPlanner object.
 * @param aUiHandler An implementation of the UI
 * @param aResult    Result of installation
 * @return           New object on the cleanup stack
 */
CPlanner* CPlanner::NewLC(RUiHandler& aUiHandler, CInstallationResult& aResult)
	{
	CPlanner* self = new(ELeave) CPlanner(aUiHandler, aResult);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
 * The destructor.
 */
CPlanner::~CPlanner()
	{
	delete iPlan;
	iRegistrySession.Close();
	}

/**
 * The constructor.
 * @param aUiHandler A UI implementation
 * @param aResult    Result of installation
 */
CPlanner::CPlanner(RUiHandler& aUiHandler, CInstallationResult& aResult) 
	:iSystemDriveChar(RFs::GetSystemDriveChar()),
	iUiHandler(aUiHandler), 
	iResult(aResult)
	{

	}

/** 
 * Second phase constructor.
 * Builds the CAppInfo structure.
 */
void CPlanner::ConstructL()
	{
	iPlan = CPlan::NewL();
	User::LeaveIfError(iRegistrySession.Connect());
	}

/**
 * Aborts the installation or uninstallation.
 */
void CPlanner::AbortL()
	{
	// Revised design NEVER generates aborted events.
	// CHandleInstallEvent* installEventCmd = CHandleInstallEvent::NewLC(iPlan->AppInfoL() , EEventAbortedUnInstall, 0, KNullDesC);
	// iUiHandler.ExecuteL(*installEventCmd);
	User::Leave(KErrCancel);
	}

/**
 * Displays the dialog warning that there are packages still on the device
 * which depend on the package being uninstalled and so may no longer work if this
 * package is removed.
 * @param aDependentPackages dependent packages.
 */
void CPlanner::DisplayDependentPackagesDialogL(const RPointerArray<CSisRegistryPackage>& aDependentPackages) const
	{
	if (aDependentPackages.Count())
		{
		RPointerArray<TDesC> dependencyNames;
		CleanupClosePushL(dependencyNames);

		for (TInt i=0; i < aDependentPackages.Count(); ++i)
			{
			User::LeaveIfError(dependencyNames.Append(&aDependentPackages[i]->Name()));
			}

		CDisplayDependencyBreak* dialog=CDisplayDependencyBreak::NewLC(iPlan->AppInfoL(), dependencyNames);
		iUiHandler.ExecuteL(*dialog);
		if (!dialog->ReturnResult())
			{
			User::Leave(KErrCancel); // Installation cancelled
			}

		CleanupStack::PopAndDestroy(2, &dependencyNames);
		}
	}

/**
 * Sets the final value of the progress bar.
 *
 * @param aFinalValue The value to use as the final value of the bar.
 */
void CPlanner::SetProgressBarFinalValueL(TInt aFinalValue)
	{
	// Send an event to set the size of the progress bar
	CHandleInstallEvent* event=CHandleInstallEvent::NewLC(iPlan->AppInfoL(), EEventSetProgressBarFinalValue, aFinalValue, KNullDesC);
	iUiHandler.ExecuteL(*event);
	if (!event->ReturnResult())
		{
		User::Leave(KErrCancel);
		}
	CleanupStack::PopAndDestroy(event);
	}

/**
 * Sets up the application information based on a registry entry.
 *
 * @param aRegistryEntry The registry entry to use for the information.
 */
void CPlanner::SetupApplicationInformationL(RSisRegistryEntry& aRegistryEntry)
	{
	// Setup the main AppInfo for giving information about the app to the ui.
	HBufC* vendorName=aRegistryEntry.LocalizedVendorNameL();
	CleanupStack::PushL(vendorName);
	
	HBufC* applicationName=aRegistryEntry.PackageNameL();
	CleanupStack::PushL(applicationName);

	iPlan->SetApplicationInformationL(*applicationName, *vendorName, aRegistryEntry.VersionL());
	CleanupStack::PopAndDestroy(2, vendorName); 
	}

/**
 * Transfers ownership of the plan from this object.
 */
CPlan* CPlanner::TransferPlanOwnership()
	{
	CPlan* plan=iPlan;
	iPlan=0;
	return plan;
	}

/**
 * This function manages the creation of uninstallation tree where each node 
 * corresponds to a package which may be uninstalled. 
 * 
 * @param aRootNode Root package to be uninstalled.
 */
void CPlanner::CreateUninstallationTreeL(CUninstallationNode& aRootNode)
	{
	RPointerArray<CUninstallationNode> unprocessedTreeNodes;
	CleanupClosePushL(unprocessedTreeNodes);
	
	CUninstallationNode* currentNode = &aRootNode;
	
	// PlannedPackages is used to avoid duplicate node in the uninstallation
	// tree.
	RPointerArray<CSisRegistryPackage> plannedPackages;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(plannedPackages);

	// Add the root node in the planned packages, so that it is not added again (as a node in the tree)
	// in case of a cyclic dependency.	
	CSisRegistryPackage* rootPackage = CSisRegistryPackage::NewL(currentNode->PackageL());
	plannedPackages.AppendL(rootPackage);
	
	while(ETrue)
		{
		// plannedPackages is used to avoid creation of duplicate nodes in the tree
		CreateChildNodesL(*currentNode, plannedPackages);
		// After processing currentNode its child nodes should be processed.
		// Therefore append child nodes of currentNode to unprocessedTreeNodes
		AppendArrayL(unprocessedTreeNodes, currentNode->ChildNodes());
		TInt index = unprocessedTreeNodes.Count() - 1;
		if(index < 0)
			{
			break;
			}
		// Here we are processing the array from the end to make it a "Depth First Search"
		currentNode = unprocessedTreeNodes[index];
		unprocessedTreeNodes.Remove(index);
		}
	CleanupStack::PopAndDestroy(&plannedPackages);	
	CleanupStack::PopAndDestroy(&unprocessedTreeNodes);
	}

/**
 * Packages that are embedded by, augmentations to or are depended on by the 
 * package argument passed in (aParentNode) are all added as child nodes to 
 * aParentNode.
 * 
 * @param aAppToProcess Package which has its associated packages investigated.
 * @param aPlannedPackages List of planned packages. This variable is used to avoid creation of duplicate 
 * 			nodes in the tree. 
 */
void CPlanner::CreateChildNodesL(CUninstallationNode& aParentNode, RPointerArray<CSisRegistryPackage>& aPlannedPackages)
	{
	RSisRegistryWritableEntry registryEntry;
	User::LeaveIfError(registryEntry.OpenL(iRegistrySession, aParentNode.PackageL()));
	CleanupClosePushL(registryEntry);
	
	
	RPointerArray<CSisRegistryPackage> processPackages;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(processPackages);

	// Go through embedded packages, add them to this aParentNode and append their CUninstallationNode to aResultsArray.  
	// Set them to "Not Planned", and do minimal setup
	// During uninstallation of embedding package embedded packages should also be removed unless there is some
	// dependency.
	registryEntry.EmbeddedPackagesL(processPackages);
	CreateNewNodesL(processPackages, aParentNode, aPlannedPackages);
	processPackages.ResetAndDestroy();
	
	// If the aParentNode represents a base package then grab all augmentations and
	// add them to aParentNode also append their CUninstallationNode to aResultsArray
	TBool isBasePackage = (registryEntry.InstallTypeL() == Sis::EInstInstallation) || 
						  (registryEntry.InstallTypeL() == Sis::EInstPartialUpgrade) || 
						  (registryEntry.InstallTypeL() == Sis::EInstPreInstalledApp);
	if (isBasePackage && !aParentNode.IsRomUpgrade())
		{
		registryEntry.AugmentationsL(processPackages);
		CreateNewNodesL(processPackages, aParentNode, aPlannedPackages);
		processPackages.ResetAndDestroy();
		}
	
	//Go through all packages we are dependent on and add them to aParentNode also append their CUninstallationNode to aResultsArray
	CSecurityPolicy* secPolicy = CSecurityPolicy::GetSecurityPolicyL();
	// If RemoveOnlyWithLastDependent is false then we should not check for dependencies.
	if(secPolicy->RemoveOnlyWithLastDependent() 
			&& ((registryEntry.InstallTypeL() == Sis::EInstInstallation) 
			|| (registryEntry.InstallTypeL() == Sis::EInstPartialUpgrade)
			|| (registryEntry.InstallTypeL() == Sis::EInstAugmentation)))
		{
		RPointerArray<CSisRegistryDependency> dependencyList;
		CleanupResetAndDestroy<RPointerArray<CSisRegistryDependency> >::PushL(dependencyList);
		
		// Retrieve all the dependencies for this package.
		// If B is in A's dependency list then this call for A will retrieve B.
		registryEntry.DependenciesL(dependencyList);

		RSisRegistryWritableEntry depRegistryEntry;
		CSisRegistryPackage *tempPackage = NULL;
		
		for(TInt i= 0; i < dependencyList.Count(); ++i)
			{	
			TInt err = depRegistryEntry.Open(iRegistrySession, dependencyList[i]->Uid());
			if(KErrNone != err)
				{
				continue;
				}
			CleanupClosePushL(depRegistryEntry);

			tempPackage = depRegistryEntry.PackageL();
			
			CleanupStack::PushL(tempPackage);
			processPackages.AppendL(tempPackage);
			CleanupStack::Pop(tempPackage);
			
			CleanupStack::PopAndDestroy(&depRegistryEntry);
			}
		CleanupStack::PopAndDestroy(&dependencyList);
		
		CreateNewNodesL(processPackages, aParentNode, aPlannedPackages);
		processPackages.ResetAndDestroy();
		}
	
	CleanupStack::Pop(&processPackages);
	CleanupStack::PopAndDestroy(&registryEntry);
	}


/**
 * Takes a collection of package details and instantiates CUninstallationNode objects
 * (uninstallation node) in a pending state, adding them to the uninstallation tree.
 * 
 * @param aProcessPackages Collection of package details to be added to the tree structure for planning.
 * @param aParentNode Takes ownership of the resulting CUninstallationNode objects.
 * @param aPlannedPackages List of planned packages. This variable is used to avoid creation of duplicate 
 * 			nodes in the tree. 
 */
void CPlanner::CreateNewNodesL(RPointerArray<CSisRegistryPackage>& aProcessPackages, CUninstallationNode& aParentNode, RPointerArray<CSisRegistryPackage>& aPlannedPackages)
	{
	// We are removing items from array (aProcessPackages)and thus require index 
	// adjustment. But if loop run in reverse order there is no need to adjust the index
	for (TInt i = aProcessPackages.Count() - 1; i >= 0; --i)
		{
		// Ignore already added package
		if (IsInPlannedPackages(aPlannedPackages, *aProcessPackages[i]))
			{
			continue;
			}
		RSisRegistryWritableEntry registryEntry;
		TRAPD(retValue, retValue = registryEntry.OpenL(iRegistrySession, *aProcessPackages[i]));
		if( (KErrNotFound == retValue) || (KErrPathNotFound == retValue))
			{
			// The package has already been removed.
			continue;
			}
		User::LeaveIfError(retValue);
		CleanupClosePushL(registryEntry);
	
		if (registryEntry.IsInRomL())
			{
			DEBUG_PRINTF2(_L8("Failed to plan this package for uninstall. Is in ROM; Uid: '0x%08x'"),aProcessPackages[i]->Uid().iUid);
			CleanupStack::PopAndDestroy(&registryEntry);
			continue;
			}
				
		// Don't remove non-removable packages except:-
		// non-removable patches (SP+NR) where the base is is the root package being uninstalled 
		if (!registryEntry.RemovableL() && 
			!(aParentNode.PackageL().Uid() == registryEntry.UidL() && registryEntry.IsAugmentationL()))
			{
			DEBUG_PRINTF2(_L("Cannot uninstall non-removable application; Uid: '0x%08x'"),aProcessPackages[i]->Uid().iUid);
			CleanupStack::PopAndDestroy(&registryEntry);
			continue;	
			}
	
		CUninstallationNode* newNode = CUninstallationNode::NewLC(registryEntry, *aProcessPackages[i]);
		aParentNode.AddNodeAsChildL(newNode);
		CleanupStack::Pop(newNode);
		aPlannedPackages.AppendL(aProcessPackages[i]);
		// Ownership is transfered from aProcessPackages to aPlannedPackages
		aProcessPackages.Remove(i);
		CleanupStack::PopAndDestroy(&registryEntry);
		}
	}

/**
 * This functions determines whether it is now okay to confirm the removal of the package(Node in the uninstallation tree).  
 * 
 * @param aAppToProcess Package which has its associated packages investigated.
 * @return ETrue If the package is marked for uninstallation else EFalse.
 */
TBool CPlanner::ValidateUninstallationCandidateL(CUninstallationNode& aCandidate, RPointerArray<CSisRegistryPackage>& aPlannedPackages)
	{
	DEBUG_PRINTF4(_L("Checking whether application can be planned for uninstallation or not; Uid: '0x%08x', Name: '%S', Vendor: '%S'"),
						aCandidate.PackageL().Uid(),
						&(aCandidate.PackageL().Name()),
						&(aCandidate.PackageL().Vendor()));
	
	
	RSisRegistryWritableEntry registryEntry;
	User::LeaveIfError(registryEntry.OpenL(iRegistrySession, aCandidate.PackageL()));
	CleanupClosePushL(registryEntry);
	
	TBool result = ETrue;
	
	CSecurityPolicy* secPolicy = CSecurityPolicy::GetSecurityPolicyL();
	
	// RemoveOnlyWithLastDependent 
	//
	// If true, when uninstalling a package which embeds another package, 
	// the embedded package is not removed if a third package is dependent on it.
	//
	// If false, the embedded package is removed along with the embedding 
	// package, even if another package depends on it.
	// So we need not check for any depandants for this package and can be 
	// planned for removal
	if (!secPolicy->RemoveOnlyWithLastDependent())
		{
		// When secPolicy->RemoveOnlyWithLastDependent() is false then packages to be removed
		// will not contain any dependant package so we can remove all the packages.
		if (!aCandidate.IsRomUpgrade()) 
			{
			DisplayDependentPackagesDialogL(aCandidate.DependentPackages());
			}
		ConfirmForUninstallL(aCandidate, aPlannedPackages);
		}
	else
		{
		if ((registryEntry.InstallTypeL() != Sis::EInstInstallation) && (registryEntry.InstallTypeL() != Sis::EInstPartialUpgrade))
			{
			// Augmentations will always be removed
			ConfirmForUninstallL(aCandidate, aPlannedPackages); 	
			}
		else
			{
			// RemoveWithLastDependentL will return true only when base package of this package is already uninstalled
			// or it is in the current planned package list (aPlannedPackages).
			// This shows that this embedded package was not removed when the base package waremoved because
			// some packages which depends on this package is not uninstalled or not present in the current 
			// planned package list.
			// If RemoveWithLastDependentL returns true then this package can be confirmed for uninstallation only when
			// all its dependants are already uninstalled or are present in the planned package list.
			if (registryEntry.RemoveWithLastDependentL() && aCandidate.AllDependantsCovered(aPlannedPackages))
				{
				aCandidate.SetRemoveOnEmbeddedL();
				ConfirmForUninstallL(aCandidate, aPlannedPackages); 
				}
			else
				{
				result = EFalse;
				}
			}
		}
	
	CleanupStack::PopAndDestroy(&registryEntry);
	return result;
	}

/**
 * This is a recursive function which will create CApplication tree.
 * It will walk through the entire uninstallation tree and create
 * tree of CApplication for all the planned nodes.
 * 
 * @param aRootNode Root node of uninstallation tree.
 * @return CApplication pointer corresponding to the aRootNode.
 */
CApplication* CPlanner::CreatePlannedApplicationL(CUninstallationNode& aRootNode)
	{
	CApplication* rootApplication = CApplication::NewLC();
	
	RPointerArray<CUninstallationNode>& nodes = aRootNode.ChildNodes();
	for(TInt i = 0; i < nodes.Count(); ++i)
		{
		if(!nodes[i]->IsPlanned())
			{
			continue;
			}
		CApplication* child = CreatePlannedApplicationL(*nodes[i]);
		CleanupStack::PushL(child);
		rootApplication->AddEmbeddedApplicationL(child);
		CleanupStack::Pop(child);
		}
	
	UpdateAppForUninstallL(*rootApplication, aRootNode);
	if(!(Swi::SecUtils::IsPackageUidPresent(aRootNode.PackageL().Uid(), iUidList)))
	    {
	    TInt err = SecUtils::PublishPackageUid(aRootNode.PackageL().Uid(), iUidList);
		if(err==KErrNone)
            {
            DEBUG_PRINTF2(_L("CPlanner::CreatePlannedApplicationL published Uid is %x."),aRootNode.PackageL().Uid());
		    }
		else if(err == KErrOverflow)
		    {
            DEBUG_PRINTF2(_L("CPlanner::CreatePlannedApplicationL failed to publish Uid %x as the array, holding the uids, exceeded its upper limit."),aRootNode.PackageL().Uid());
		    }
        else if(err == KErrNotFound)
            {
            DEBUG_PRINTF2(_L("CPlanner::CreatePlannedApplicationL failed to publish Uid %x as the property is not defined."),aRootNode.PackageL().Uid());
            }
		else
		    {
            DEBUG_PRINTF3(_L("CPlanner::CreatePlannedApplicationL failed to publish Uid %x with error."),aRootNode.PackageL().Uid(), err);
            User::Leave(err);
		    }
		}
	CleanupStack::Pop(rootApplication);
	
	return rootApplication;
	}

/**
 * Mark the node as planned and update planned package list. 
 * object.
 * 
 * @param aNode The CApplication object to populate.
 * @param aRegistryEntry The relevant SIS Registry entry for the package.
 */
void CPlanner::ConfirmForUninstallL(CUninstallationNode& aNode, RPointerArray<CSisRegistryPackage>& aPlannedPackages)
	{
	aNode.SetIsPlanned(ETrue);

	// aNode owns package therefore to transfer the ownership
	// a copy of CSisRegistryPackage is made.
	CSisRegistryPackage* package = CSisRegistryPackage::NewLC(aNode.PackageL());
	aPlannedPackages.AppendL(package);
	CleanupStack::Pop(package);
	}

/**
 * This function is called after the node/package is marked for uninstallation.
 * So it will fully populate the details of CApplication object.
 * 
 * @param aApplication The CApplication object to populate.
 * @param aNode The uninstallation node which is planned for uninstallation.
 */
void CPlanner::UpdateAppForUninstallL(CApplication& aApplication, CUninstallationNode& aNode)
	{
	RSisRegistryWritableEntry registryEntry;
	User::LeaveIfError(registryEntry.OpenL(iRegistrySession, aNode.PackageL()));
	CleanupClosePushL(registryEntry);
	
	aApplication.SetUninstallL(aNode.PackageL());
	aApplication.SetShutdownAllApps(registryEntry.ShutdownAllAppsL());

	if(registryEntry.IsInRomL())
		{
		aApplication.SetInROM();
		}
	
	if (registryEntry.InstallTypeL() == EInstPreInstalledApp)
 		{
 		aApplication.SetPreInstalledApp();
 		}
 	else if(registryEntry.InstallTypeL() == EInstPreInstalledPatch)
 		{
 		aApplication.SetPreInstalledPatch();
 		}

	if (registryEntry.IsDeletablePreInstalledL())
		{
		aApplication.SetDeletablePreinstalled(ETrue);
		}
	// Add files from this package to the list to remove
	RPointerArray<CSisRegistryFileDescription> fileDescriptions;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryFileDescription> >::PushL(fileDescriptions);
	registryEntry.FileDescriptionsL(fileDescriptions);

	TInt fileRemovalCount = fileDescriptions.Count();
	for (TInt i = 0; i < fileRemovalCount; ++i)
		{
		// Check this file description is not corrupt, it is, ignore it
		// and move on to the next
		if (!SecurityCheckUtil::CheckFileName(fileDescriptions[i]->Target(), iSystemDriveChar))
			{
			User::Leave(KErrSISInvalidTargetFile);
			}
		
		TParsePtrC targetPath(fileDescriptions[i]->Target());

		// Set list of files to remove - note that for preinstalled apps or
		// patches only files in sys\bin are added to the list, and only the
		// hashes will actually be removed, unless the app is marked as
		// deletable.
		
		TBool isPreInstalled =  aApplication.IsPreInstalledApp() ||	aApplication.IsPreInstalledPatch() ;
		TBool isDeletablePreinstalled = aApplication.IsDeletablePreinstalled();

		if (!isPreInstalled || isDeletablePreinstalled || (targetPath.Path().CompareF(KBinPath) == 0))
			{
			DEBUG_PRINTF2(_L("Adding file '%S' to plan to remove"),
							&(fileDescriptions[i]->Target()));
			aApplication.RemoveFileL(*fileDescriptions[i]);
			Plan().AddUninstallFileForProgress();
			}
			
		//Add files having RBS option to run.
		if ( fileDescriptions[i]->Operation() == EOpRun &&
	 	fileDescriptions[i]->OperationOptions() & EInstFileRunOptionBeforeShutdown)
			{
			//this array will contain all the files from all the pkges with RBS option.
			iPlan->RunFilesBeforeShutdownL(*fileDescriptions[i]);
			}

		//Add to files to run if we need to
		if (fileDescriptions[i]->Operation() == EOpRun &&
		 fileDescriptions[i]->OperationOptions() & EInstFileRunOptionUninstall)
			{
			aApplication.RunFileOnUninstallL(*fileDescriptions[i]);
			}
		// Files to display
		else if (fileDescriptions[i]->Operation() == EOpText)
			{
			aApplication.DisplayFileOnUninstallL(*fileDescriptions[i]);
			}
		}
	// cleanup 
	CleanupStack::PopAndDestroy(&fileDescriptions);
	CleanupStack::PopAndDestroy(&registryEntry);
	}

/**
 * This function is called from uninstallation/installation planner. 
 * This function have the logistics to decide which all dependent packages 
 * to be uninstalled along with the main package (aPackage).
 * 
 * @param aPackage Main package to be uninstalled.
 * @param aTopLevelPackage ETrue if this is the main package to be uninstalled. EFalse if
 * 						this package is uninstalled due to installation or uninstallation of
 * 						some other package.
 * @return CApplication of aPackage passed.
 */
CApplication* CPlanner::UninstallPackageL(CSisRegistryPackage& aPackage, TBool aTopLevelPackage)
	{
	DEBUG_PRINTF4(_L("Planning Application For Uninstall; Uid: '0x%08x', Name: '%S', Vendor: '%S'"),
						aPackage.Uid(),
						&(aPackage.Name()),
						&(aPackage.Vendor()));
	
	RSisRegistryWritableEntry registryEntry;
	User::LeaveIfError(registryEntry.OpenL(iRegistrySession, aPackage));
	CleanupClosePushL(registryEntry);
	
	// Packages installed on ROM should not be uninstalled.
	if (registryEntry.IsInRomL())
		{
		DEBUG_PRINTF2(_L8("Failed to plan this package for uninstall. Is in ROM; Uid: '0x%08x'"),aPackage.Uid().iUid);
		User::Leave(KErrNotSupported); // cannot remove rom packages
		}
	
	if (!registryEntry.RemovableL())
		{
		DEBUG_PRINTF2(_L("Cannot uninstall non-removable application; Uid: '0x%08x'"),aPackage.Uid().iUid);
		User::Leave(KErrNotSupported); // cannot remove non-removable (NR) packages	
		}

	// Create the root node in the uninstallation tree. 
	CUninstallationNode* result = CUninstallationNode::NewLC(registryEntry, aPackage);

	// Check whether the package to be removed is a rom-upgrade or not.
	// We already checked that package is not in ROM. And if the package 
	// with same UID exists in ROM then it is a ROM upgrade.
	TBool romUpgrade = iRegistrySession.PackageExistsInRomL(aPackage.Uid());
	result->SetIsRomUpgrade(romUpgrade);
	
	// Perform initial setup.
	if (aTopLevelPackage)
		{
		SetupApplicationInformationL(registryEntry);
		}
	
	// Create a uninstallation tree of all the packages which might be uninstalled due to 
	// uninstallation of this packageaPackage (i.e. the root node).
	// A package is only added into the uninstallation tree at most once.
	CreateUninstallationTreeL(*result);
	
	// PlannedPackages is used to hold packages which are plannded for uninstallation.
	// This variable is used by AllDependantsCovered function to check whether all
	// the dependant packages of a node is also planned for uninstall.
	RPointerArray<CSisRegistryPackage> plannedPackages;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(plannedPackages);
	
	TBool removeOnlyWithLastDependent = CSecurityPolicy::GetSecurityPolicyL()->RemoveOnlyWithLastDependent(); 
	if (removeOnlyWithLastDependent && ((registryEntry.InstallTypeL() == Sis::EInstInstallation) || (registryEntry.InstallTypeL() == Sis::EInstPartialUpgrade)))
		{
		if(aTopLevelPackage)
			{
			DisplayDependentPackagesDialogL(result->DependentPackages());
			result->SetRemoveOnEmbeddedL();
			}
		ConfirmForUninstallL(*result, plannedPackages);
		}

	RPointerArray<CUninstallationNode> workingList;
	CleanupClosePushL(workingList);
	
	// Repeatedly iterate over the tree structure checking if we can confirm any packages for removal. 
	// Continue to loop while new packages are found as this may clear the dependency lists for other packages.
	TBool anotherPass = ETrue;
	
	while (anotherPass)
		{
		anotherPass = EFalse;
		
		// Add the root node in the working list
		workingList.AppendL(result);
		
		TInt index = workingList.Count() - 1;
		// Internal loop traverse the entire uninstallation tree to find if 
		// any node can be uninstalled. Uninstallation of any node can clear 
		// dependency of any other package.
		//
		// e.g. Lets consider the following scenario.
		// A embeds B and C
		// B depends on C.
		// So during uninstallation of A if C is processed before B 
		// then C cannot be uninstalled because B depends on C and B 
		// is not marked for uninstallation. So it will skip the C and move on 
		// to B. Now B can be uninstalled. And when B is uninstalled it
		// will clear the dependency of C so C should be processed again
		// which can be done by traversing the uninstallation tree again.
		// Which is done with the help of anotherPass variable and the outer loop. 
		while (index >= 0)
			{
			// remove the node from the working list so that it can be processed.
			CUninstallationNode* tempApp = workingList[index];
			workingList.Remove(index);

			// If the node is not planned then check if it can be uninstalled or not.
			if(!tempApp->IsPlanned())
				{
				anotherPass |= ValidateUninstallationCandidateL(*tempApp, plannedPackages);
				// If tempApp is planned for uninstall then the above call will return ETrue.
				}

			// tempApp might be planned after the call to ValidateUninstallationCandidateL
			if(tempApp->IsPlanned())
				{
				// If an application node is planned then get the child nodes of this node to
				// do further processing.
				// If a node is not planned for uninstallation then non of its child node
				// will be checked for uninstallation.
				AppendArrayL(workingList, tempApp->ChildNodes());
				}
			index = workingList.Count() - 1;
			}
		
		// Since RemoveOnlyWithLastDependent flag is false then we neednot check if
		// removal of this package fullfills any other dependency.
		if(!removeOnlyWithLastDependent)
			{
			break;
			}
		}
	CleanupStack::PopAndDestroy(&workingList);
	CleanupStack::PopAndDestroy(&plannedPackages);
	
	// Now create CApplication class which will represent the root node of 
	// corresponding uninstallation tree (whose each node is a planned CApplication).  
	// This newly formed uninstallation tree is later used to uninstall the packages. 
	// So all the packages which are represented by the nodes of this tree will be
	// uninstalled from the device.
	CApplication* application = CreatePlannedApplicationL(*result);
	
	//Cleanup and return the result.
	CleanupStack::PopAndDestroy(result);
	CleanupStack::PopAndDestroy(&registryEntry);
	
	return application;	
	}

/**
 * Returns if a package is in the uninstallation list
 *
 * @param aPackage Sis registry package
 * @return ETrue if the package is in the list, otherwise EFalse
 */
TBool CPlanner::IsInPlannedPackages(RPointerArray<CSisRegistryPackage>& aPlannedPackages, const CSisRegistryPackage& aPackage) const
	{
	for(TInt i = 0; i < aPlannedPackages.Count(); ++i)
		{
		if((*aPlannedPackages[i] == aPackage) && 
		   (aPlannedPackages[i]->Index() == aPackage.Index()))
			{
			return ETrue;
			}
		}		
	return EFalse;
	}


