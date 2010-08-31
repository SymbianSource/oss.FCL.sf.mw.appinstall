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
* Definition of the CPlanner
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#ifndef __PLANNER_H__
#define __PLANNER_H__

#include <e32std.h>

#include "sisuihandler.h"
#include "sisinstallationresult.h"
#include "application.h"
#include "sisregistrywritableentry.h"
#include "sisregistrywritablesession.h"
#include "sisregistrypackage.h"
#include "secutils.h"

namespace Swi 
{
class RUiHandler;            // UISS handle
class CAppInfo;            
class CInstallationPlan;
class RSisRegistryEntry;
class CSisRegistryPackage;
class CPlan;
class CUninstallationNode;
class CApplication;

namespace Sis 
{
class COptions;             // SISX options
}

/**
 * This class plans the uninstallation. It asks the user all the required 
 * questions (regarding dependencies and files/apps in use) and builds a list 
 * of application and files that need to be uninstalled, without actually 
 * changing the state of the device.
 *
 * @released
 * @internalTechnology
 */
class CPlanner : public CBase
	{
public:
	static CPlanner* NewL(RUiHandler& aUiHandler, CInstallationResult& aResult);
	static CPlanner* NewLC(RUiHandler& aUiHandler, CInstallationResult& aResult);

	virtual ~CPlanner(void);

public:

	CPlan* TransferPlanOwnership();
	CApplication* UninstallPackageL(CSisRegistryPackage& aPackage, TBool aTopLevelPackage);
	
protected:
	CPlanner(RUiHandler& aInstallerUI, CInstallationResult& aInstallationResult);

	void ConstructL();
	
protected:
	void AbortL();
	void DisplayDependentPackagesDialogL(const RPointerArray<CSisRegistryPackage>& aDependentPackages) const;
	void SetProgressBarFinalValueL(TInt aFinalValue);
	void SetupApplicationInformationL(RSisRegistryEntry& aRegistryEntry);

	inline RUiHandler& UiHandler();
	inline CPlan& Plan();
	inline const CPlan& Plan() const;

private:
	TBool IsInPlannedPackages(RPointerArray<CSisRegistryPackage>& aPlannedPackages, const CSisRegistryPackage& aPackage) const;
	void CreateUninstallationTreeL(CUninstallationNode& aRootNode);
	void CreateChildNodesL(CUninstallationNode& aParentNode, RPointerArray<CSisRegistryPackage>& aPlannedPackages);
	void CreateNewNodesL(RPointerArray<CSisRegistryPackage>& aProcessPackages, CUninstallationNode& aParentNode, RPointerArray<CSisRegistryPackage>& aPlannedPackages);
	TBool ValidateUninstallationCandidateL(CUninstallationNode& aAppToProcess, RPointerArray<CSisRegistryPackage>& aPlannedPackages);
	CApplication* CreatePlannedApplicationL(CUninstallationNode& aRootNode);
	void ConfirmForUninstallL(CUninstallationNode& aNode, RPointerArray<CSisRegistryPackage>& aPlannedPackages);
	void UpdateAppForUninstallL(CApplication& aApplication, CUninstallationNode& aNode);
	
protected:
	/// The drive on which to store device integrity data (hashes, registry etc) 
	TChar iSystemDriveChar;		
	/// To store package uids which are getting processed 
	/////An array whose first element holds the number of uids in the subsequent indices of the array, followed by the the list of uids that are being published.
	TUid iUidList[KMaxUidCount];
	
private:
	/// Handle to UISS
	RUiHandler& iUiHandler;

	/// Result of uninstallation
	CInstallationResult& iResult;

	/// The uninstallation plan that will be populated by this class
	CPlan* iPlan;

	/**
	 * Information about the application being installed or uninstalled.
	 * This is passed to the UI callbacks.
	 */
	CAppInfo* iAppInfo;

	// Registry Session
	RSisRegistryWritableSession iRegistrySession;
	};

// inline functions from CPlanner

/**
 * @return Returns the UI Handler.
 */
inline RUiHandler& CPlanner::UiHandler()
	{
	return iUiHandler;
	}

/**
 * @return Returns a reference to the plan.
 */
inline CPlan& CPlanner::Plan()
	{
	return *iPlan;
	}

/**
 * @return Returns a const reference to the plan.
 */
inline const CPlan& CPlanner::Plan() const
	{
	return *iPlan;
	}

} // namespace Swi 

#endif
