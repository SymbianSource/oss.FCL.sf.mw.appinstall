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
* Definition of the CUninstallationPlanner
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#ifndef __UNINSTALLATIONPLANNER_H__
#define __UNINSTALLATIONPLANNER_H__

#include <e32std.h>

#include "planner.h"

namespace Swi 
{
class RUiHandler;
class CInstallationResult;
class CPlan;


/**
 * This class plans the uninstallation. It asks the user all the required 
 * questions (regarding dependencies and files/apps in use) and builds a list 
 * of application and files that need to be uninstalled, without actually 
 * changing the state of the device.
 *
 * @released
 * @internalTechnology
 */
class CUninstallationPlanner : public CPlanner
	{
public:
	/**
	 * This creates a new CUninstallationPlanner object.
	 * @param aUiHandler An implementation of the UI
	 * @param aResult    Result of uninstallation
	 * @return           New object
	 */
	static CUninstallationPlanner* NewL(RUiHandler& aUiHandler, CInstallationResult& aResult);

	/**
	 * This creates a new CUninstallationPlanner object.
	 * @param aUiHandler An implementation of the UI
	 * @param aResult    Result of installation
	 * @return           New object on the cleanup stack
	 */
	static CUninstallationPlanner* NewLC(RUiHandler& aUiHandler, CInstallationResult& aResult);

	/**
	 * The destructor.
	 */
	virtual ~CUninstallationPlanner(void);

public:
	/**
	 * Kicks off the uninstallation planning
	 *
	 * @param aPackage The package to uninstall.
	 */
	void PlanUninstallationL(CSisRegistryPackage& aPackage);

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * @return The final progress bar value from the plan.
	 */
	TInt32 FinalProgressBarValue() const;
#endif
	
	/**
	 * Produces an uninstallation plan
	 * @return Uninstallation plan object
	 */
//	CUninstallationPlan* GetPlanL();
	
private:
	/**
	 * The constructor.
	 * @param aInstallerUI         UID of installation
	 * @param aInstallationResult  Result of installation
	 */
	CUninstallationPlanner(RUiHandler& aInstallerUI, CInstallationResult& aInstallationResult);

	/** 
	 * Second phase constructor.
	 * Builds the CAppInfo structure.
	 */
	void ConstructL();
	
private:
	/**
	 * Uninstalls a package, and packages which are embedded by this package.
	 *
	 * @param aUid UID of the package to uninstall.
	 * @param aTopLevelPackage ETrue if this is the top level package in the embedding
	 *									chain, EFalse otherwise.
	 */
//	void UninstallPackageL(TUid aUid, TBool aTopLevelPackage, TInt& aFileRemovalCount);

private:

	/// The uninstallation plan that will be populated by this class
	CPlan* iPlan;
	};

} // namespace Swi 

#endif
