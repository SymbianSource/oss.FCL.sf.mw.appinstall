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
* Definition of the CUninstallationNode. This class represents a node in the un-installation tree.
* It holds the information required by Plan to uninstallation the application. This class creates
* an instance of CApplication which is used by the Plan to uninstallation.
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#ifndef __CUNINSTALLATIONNODE_H__
#define __CUNINSTALLATIONNODE_H__

#include <e32base.h>

namespace Swi
{

// Forward declarations
class RSisRegistryEntry;
class CSisRegistryPackage;
class CApplication;

/**
 * This class represents a node in an uninstallation tree. 
 */ 
class CUninstallationNode: public CBase
	{
public:
	static CUninstallationNode* NewL(RSisRegistryEntry& aRegistryEntry, CSisRegistryPackage& aPackage);
	static CUninstallationNode* NewLC(RSisRegistryEntry& aRegistryEntry, CSisRegistryPackage& aPackage);
	
	virtual ~CUninstallationNode();
	
	inline TBool IsRomUpgrade() const;
	inline void SetIsRomUpgrade(TBool aNewValue);
	inline TBool IsPlanned() const;
	inline void SetIsPlanned(TBool aValue);
	inline const RPointerArray<CSisRegistryPackage>& DependentPackages() const;
	inline RPointerArray<CUninstallationNode>& ChildNodes();
	inline const CSisRegistryPackage& PackageL() const;	
	
	void AddNodeAsChildL(CUninstallationNode* aNode);
	void SetRemoveOnEmbeddedL();
	TBool AllDependantsCovered(RPointerArray<CSisRegistryPackage>& aPackagesUninstalling);

private:
	CUninstallationNode();
	void ConstructL(RSisRegistryEntry& aRegistryEntry, CSisRegistryPackage& aPackage);
	TBool IsAugmentationL(CSisRegistryPackage& aPackage);
	
private:
	// Ownership is with parent CApplication.
	CUninstallationNode*				iParent;
	RPointerArray<CUninstallationNode>	iChildNodes;
	TBool 								iIsRomUpgrade;
	/*
	 * Used during uninstallation planning.
	 * ETrue	: Application's dependencies are fulfilled and is planned for un-installation.
	 * EFalse	: Application's dependencies are not fulfilled yet therefore it is not planned for un-installation.
	 */  
	TBool 								iIsPlanned;	
	RPointerArray<CSisRegistryPackage> 	iEmbeddedPackages;
	RPointerArray<CSisRegistryPackage> 	iDependentPackages;
	RPointerArray<CSisRegistryPackage> 	iAugmentations;
	
	CSisRegistryPackage* iPackage;
	};

// Inline Member Function Declarations

/**
 * Checks whether this application is a ROM upgrade or not.
 */
inline TBool CUninstallationNode::IsRomUpgrade() const
	{
	return iIsRomUpgrade;
	}

/**
 * Marks the current application as ROM upgrade or not
 * @param aNewValue ETrue to mark application/package as ROM upgrade.
 */
inline void CUninstallationNode::SetIsRomUpgrade(TBool aNewValue)
	{
	iIsRomUpgrade = aNewValue;
	}

/**
 * Checks whether the application is planned for un-installation or not.
 * 
 * @param ETrue if the node is planned for un-installation else EFalse 
 */
inline TBool CUninstallationNode::IsPlanned() const
	{
	return iIsPlanned;
	}

/**
 * Change the state of the app as planned or not planned for un-installation.
 * 
 * @param aValue change the application's state. ETrue if Application's dependencies are fulfilled and is planned for un-installation
 *				 else EFalse.
 */
inline void CUninstallationNode::SetIsPlanned(TBool aValue)
	{
	iIsPlanned = aValue;
	}

/**
 * @return array of dependent packages.
 */
inline const RPointerArray<CSisRegistryPackage>& CUninstallationNode::DependentPackages() const
	{
	return iDependentPackages;
	}

/**
 * This function returns all the child nodes in for this node in the dependecy tree.
 * 
 * @return array of CUninstallationNode.
 */
inline RPointerArray<CUninstallationNode>& CUninstallationNode::ChildNodes()
	{
	return iChildNodes;
	}

/**
 * This function returns Package related to this node.
 * @return  CSisRegistryPackage reference.
 */
inline const CSisRegistryPackage& CUninstallationNode::PackageL() const
	{
	return *iPackage;
	}

} // swi namespace

#endif //__CUNINSTALLATIONNODE_H__


