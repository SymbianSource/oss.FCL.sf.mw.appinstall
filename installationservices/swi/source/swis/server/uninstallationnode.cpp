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
* Definition of the CUninstallationNode. This class acts a node in the uninstallation tree. 
*
*/


/**
 @file
*/

#include <e32std.h>

#include "uninstallationnode.h"
#include "sisregistrywritableentry.h"
#include "sisregistrywritablesession.h"
#include "sisregistrypackage.h"
#include "log.h"
#include "application.h"

using namespace Swi;

CUninstallationNode* CUninstallationNode::NewL(RSisRegistryEntry& aRegistryEntry, CSisRegistryPackage& aPackage)
	{
	CUninstallationNode* self = CUninstallationNode::NewLC(aRegistryEntry, aPackage);
	CleanupStack::Pop(self);
	return self;
	}

CUninstallationNode* CUninstallationNode::NewLC(RSisRegistryEntry& aRegistryEntry, CSisRegistryPackage& aPackage)
	{
	CUninstallationNode* self = new (ELeave) CUninstallationNode();
	CleanupStack::PushL(self);
	self->ConstructL(aRegistryEntry, aPackage);
	return self;
	}

CUninstallationNode::CUninstallationNode()
	{
	}

CUninstallationNode::~CUninstallationNode()
	{
	iDependentPackages.ResetAndDestroy();
	iEmbeddedPackages.ResetAndDestroy();
	iAugmentations.ResetAndDestroy();
	iChildNodes.ResetAndDestroy();
	delete iPackage;
	}

/**
 * Second phase constructor.
 * @param aRegistryEntry registry entry of the package represented by this Node.
 * @param aPackage package represented by this Node.
 */
void CUninstallationNode::ConstructL(RSisRegistryEntry& aRegistryEntry, CSisRegistryPackage& aPackage)
	{
	iPackage = CSisRegistryPackage::NewL(aPackage);

	aRegistryEntry.EmbeddedPackagesL(iEmbeddedPackages);
	aRegistryEntry.DependentPackagesL(iDependentPackages);
	aRegistryEntry.AugmentationsL(iAugmentations);
	}

/**
 * Add aNode to the list of embedded applications.
 * @param aNode Node to be added as child to the current Node.
 */
void CUninstallationNode::AddNodeAsChildL(CUninstallationNode* aNode)
	{
	// Set the parent link
	aNode->iParent = this;
	// Set the child link
	iChildNodes.AppendL(aNode);
	}

/**
 * Call SetRemoveWithLastDependentL() on packages embedded by this one.
 * Used when the RemoveWithLastDependency flag is True in the security policy
 * and this package is being removed.
 */
void CUninstallationNode::SetRemoveOnEmbeddedL()
	{
	RSisRegistryWritableSession registrySession;
	User::LeaveIfError(registrySession.Connect());
	CleanupClosePushL(registrySession);

	RSisRegistryWritableEntry registryEntry;
	CleanupClosePushL(registryEntry);
	
	for (TInt i=0; i < iEmbeddedPackages.Count(); ++i)
		{
		TRAPD(err, err = registryEntry.OpenL(registrySession, *iEmbeddedPackages[i]));
		// Do not process already uninstalled embedded package.
		if(KErrNotFound == err || KErrPathNotFound == err)
			{
			continue;
			}
		User::LeaveIfError(err);
		registryEntry.SetRemoveWithLastDependentL(iEmbeddedPackages[i]->Uid());
		}
	CleanupStack::PopAndDestroy(2, &registrySession);
	}


/**
 * Checks whether the all the dependants on this Node(package) is present in the list
 * of packages to be un-installed.
 * 
 * @param aPackagesUninstalling List of packages which will be un-installed.
 * @return ETrue if all the dependants is present in the uninstallation list.
 */
TBool CUninstallationNode::AllDependantsCovered(RPointerArray<CSisRegistryPackage>& aPackagesUninstalling)
	{
	//Quick optimisation check.  Obviously if the set of packages being uninstalled 
	//is smaller than the set of packages dependent on us then it is not okay to 
	//uninstall the package.
	if (aPackagesUninstalling.Count() < iDependentPackages.Count())
		{
		return EFalse;
		}
	
	// Iterate through each package dependent on us and ensure it is in the list
	// of packaged currently confirmed for removal.
	// iDependentPackages also contains augmentations. For checking dependants 
	// we should not check for augmentaions. 
	// So skip all augmentations.
	for (TInt i = 0; i < iDependentPackages.Count(); ++i)
		{
		// Skip all the augmentations.
		if(IsAugmentationL(*iDependentPackages[i]))
			{
			continue;
			}
	
		TBool dependencyFound = EFalse;
		for (TInt j = 0;  j < aPackagesUninstalling.Count(); ++j)
			{
			if (*iDependentPackages[i] == *aPackagesUninstalling[j])
				{
				dependencyFound = ETrue;
				break;
				}
			}
		if (!dependencyFound)
			{
			return EFalse;
			}
		}
	
	return ETrue;
	}

/**
 * This function checks that whether the passed package is an Augmentation
 * of this node (Application represented by this node) or not.
 * 
 * @param aPackage Package which needs to be checked
 * @return ETrue if the package is an Augmentation else EFalse.
 */
TBool CUninstallationNode::IsAugmentationL(CSisRegistryPackage& aPackage)
	{
	TBool isAugmentation = EFalse;
	for(TInt i = 0; i < iAugmentations.Count(); ++i)
		{
		if( *iAugmentations[i] == aPackage)
			{
			isAugmentation = ETrue;
			break;
			}
		}
	return isAugmentation;
	}

