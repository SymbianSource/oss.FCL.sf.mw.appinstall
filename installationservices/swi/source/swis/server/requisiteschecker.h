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
 @internalComponent
 @released
*/


#ifndef __REQUISITESCHECKER_H__
#define __REQUISITESCHECKER_H__

#include <e32base.h>
#include <e32cmn.h>
#include <f32file.h>
#include "sisregistrysession.h"

namespace Swi 
	{
	class CPlan;
	class CApplication;
	class TCheckedVersion;
	class RUiHandler;
	class CInstallationResult;
	class CContentProvider;
	namespace Sis
		{
		class CController;
		class CDependency;
		class CVersionRange;
		class CInstallBlock;
		}
	/**
	Base class for pre- and post-plan requisites checking
	*/
	class CRequisitesChecker : public CBase
		{
		protected:
		CRequisitesChecker (RUiHandler& aInstallerUI, CInstallationResult& aInstallationResult, const CContentProvider& aProvider);
		void ConstructL();
		~CRequisitesChecker();

		/**
		Indicates if any of a set of dependencies can be satisfied from what is already present on the device
		@param aDependencies - The dependencies to be satisfied
		@return - Whether one of the dependencies can be satisfied
		*/
		TBool AnyOnDeviceL(const RPointerArray<Sis::CDependency>& aDependencies);
		/**
		Indicates if all of a set of dependencies can be satisfied from what is already present on the device
		or is in the candidate SIS file
		@param aDependencies - The dependencies to be satisfied
		@param aController - A controller possibly embedding controllers that may satisfy some of the dependencies
		@return - Whether one of the dependencies can be satisfied
		*/
		TBool AllOnDeviceOrSISL(const RPointerArray<Sis::CDependency>& aDependencies, const Sis::CController& aController);
		/**
		Indicates if all of a set of dependencies can be satisfied from what is already present on the device
		or from what has been selected to be installed from a SIS file
		@param aDependencies - The dependencies to be satisfied
		@param aPlan - The selection
		@return - Whether one of the dependencies can be satisfied
		*/
		TBool AllOnDeviceOrPlanL(const RPointerArray<Sis::CDependency>& aDependencies, const CPlan& aPlan);
		
		/**
		A file being installed already exists, find the application that owns it and inform user using installer UI
		@param aPlan - Application that is trying to install the file
		@param aFileName - The file being overwritten
		@param aOwnerUid The UID of the owning application
		@return - ETrue to continue, EFalse to fail
		*/
		TBool DisplayProblemFileL(const CPlan& aPlan, TFileName& aFilename, CSisRegistryPackage* aPackage);
	private:
		/**
		Indicates if a dependency can be satisfied from what is already present on the device
		@param aDependency - The dependency to be satisfied
		@param aMismatch - Set to true if the failing dependency failed due to incompatible versions
		@param aMismatchedVersion - If aMismatch is set, this is set to the incompatible version
		@return - Whether the dependency can be satisfied
		*/
		TBool OnDeviceL(const Sis::CDependency& aDependency, TBool& aMismatch, TVersion& aMismatchedVersion);
		/**
		Indicates if a dependency can be satisfied from what may be installed by a SIS file
		@param aDependency - The dependency to be satisfied
		@param aController - A controller possibly embedding controllers that satisfy the dependency
		@param aMismatch - Set to true if the failing dependency failed due to incompatible versions
		@param aMismatchedVersion - If aMismatch is set, this is set to the incompatible version
		@param aFoundDependency - Set to true if the dependency is found.
		*/
		void InSIS(const Sis::CDependency& aDependency, const Sis::CController& aController, TBool& aMismatch, TVersion& aMismatchedVersion, TBool& aFoundDependency);
		
		/**
		Indicates if a dependency can be satisfied from the embedded sis file of the install block
		@param aDependency - The dependency to be satisfied
		@param aInstallBlock - A installblock possibly embedding controllers that satisfy the dependency
		@param aMismatch - Set to true if the failing dependency failed due to incompatible versions
		@param aMismatchedVersion - If aMismatch is set, this is set to the incompatible version
		@param aFoundDependency - Set to true if the dependency is found
		*/		
		void InEmbeddedSIS(const Sis::CDependency& aDependency, const Sis::CInstallBlock& aInstallBlock, TBool& aMismatch, TVersion& aMismatchedVersion, TBool& aFoundDependency);
		
		/**
		Indicates if a dependency can be satisfied from what has been selected to be installed from a SIS file
		@param aDependency - The dependency to be satisfied
		@param aPlan - The selection
		@param aMismatch - Set to true if the failing dependency failed due to incompatible versions
		@param aMismatchedVersion - If aMismatch is set, this is set to the incompatible version
		@return - Whether the dependency can be satisfied
		*/
		TBool InPlanL(const Sis::CDependency& aDependency, const CPlan& aPlan, TBool& aMismatch, TVersion& aMismatchedVersion);

		/**
		Checks that a version is in range
		@param aVersion - The version to be checked
		@param aVersionRange - The valid range
		*/
		TBool InRange(TCheckedVersion aVersion, const Sis::CVersionRange* aDependency);

		/**
		Informs the user there is a missing dependency and installation cannot continue
		@param aDependency - The dependency that cannot be satisfied.
		@param aVersionMismatch - ETrue if there is an available incompatible version
		@param aMismatchedVersion - If aVersionMismatch is set, indicates the incompatible version of the
			dependency that is available
		@return - ETrue to continue, EFalse to fail
		*/
		TBool ShowDependencyDialogL(const Sis::CDependency& aDependency, TBool aVersionMismatch, TVersion aMismatchedVersion);

		/**
		Inform the user that the SIS file or plan is incompatible with the device
		@return - ETrue to continue, EFalse to fail
		*/
		TBool DisplayDeviceIncompatibilityL();

	private:

		/**
		Internal function which processes an application in order to indicate if a dependency 
		can be satisfied from what has been selected to be installed from a SIS file. This 
		function processes applications recursively.
		@param aPlan - The complete installation plan.
		@param aApplication - The application to check downwards from.
		@param aDependency - The dependency to be satisfied
		@param aMismatch - Set to true if the failing dependency failed due to incompatible versions
		@param aMismatchedVersion - If aMismatch is set, this is set to the incompatible version
		@return - Whether the dependency can be satisfied
		*/
		TBool DoInPlanL(const CPlan& aPlan, const CApplication& aApplication, const Sis::CDependency& aDependency, TBool& aMismatch, TVersion& aMismatchedVersion);

		/**
		Controllers not in the InstallBlock are optional. This function searches for the
		embedded controller within the specified controller's install block to determine
		if installation	of this controller is optional.
		@param aController - The controller to search within.
		@param aEmbeddedController - The controller to search for.
		@return - Whether the controller is optional
		*/
		TBool ControllerOptional(const Sis::CController& aController, const Sis::CController& aEmbeddedController);
			
		/**
		Installation result
		*/
		CInstallationResult&	iInstallationResult;
		
		const CContentProvider&	iProvider;
		
	protected:

		RFs						iFs;

		/**
		An implementation of the Installer UI
		*/
		RUiHandler&				iInstallerUI;

		/**
		Installed apps registry class used to access the registry.
		*/
		RSisRegistrySession		iSisRegistrySession;
		};
	
	} // end Swi::
	
#endif
