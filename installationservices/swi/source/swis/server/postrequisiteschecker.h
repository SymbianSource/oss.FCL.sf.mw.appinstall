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

#ifndef __POSTREQUISITESCHECKER_H__
#define __POSTREQUISITESCHECKER_H__

#include <e32base.h>
#include <f32file.h>
#include "sisregistrysession.h"
#include "requisiteschecker.h"
#include "sisregistryfiledescription.h"

namespace Swi 
	{
	class RUiHandler;
	class CSISInstallationResult;	
	class CInstallationResult;
	class CApplication;
	class CContentProvider;
	class CPlan;
	
	namespace Sis
		{
		class CController;
		class CDependency;
		class CVersionRange;
		}
 
	/**
	Performs the initial (postplanning) requisite checks, e.g. whether the app 
	is compatible with the actual device.
	*/
	class CPostrequisitesChecker : public CRequisitesChecker
		{
		public:
		/**
		This creates a new CPostrequisitesChecker object, and adds it to cleanup stack
		@param aInstallerUI - An implementation of the Installer UI
		@param aPlan - The planned installation
		@param aInstallationResult - Installation result
		@param aProvider - The content provider
		*/
		static CPostrequisitesChecker* NewLC(RUiHandler& aInstallerUI, 
										const CPlan& aPlan, 
										CInstallationResult& aInstallationResult,
										const CContentProvider& aProvider);

		/**
		The destructor.
		*/
		~CPostrequisitesChecker();

		private:
		/**
		The constructor.
		@param aInstallerUI - An implementation of the Installer UI
		@param aPlan - The planned installation
		@param aInstallationResult - Installation result
		@param aProvider - The content provider
		@param aAllowOrphanedOverwrite - A flag indicating whether to ask the user about overwriting orphaned files
		@param aAllowPrivateOrphanOverwrite - if false then always protect orphans in private import directories
		*/
		CPostrequisitesChecker(RUiHandler& aInstallerUI, 
							const CPlan& aPlan, 
							CInstallationResult& aInstallationResult,
							const CContentProvider& aProvider,
							TBool aAllowOrphanedOverwrite,
							TBool aAllowPrivateOrphanOverwrite);
		
		/**
		The second-phase constructor.
		*/
		void ConstructL();
		
		public:
		/**
		Public function to kick off postrequisites check
		*/
		void CheckPostrequisitesL();

	private:
		/**
		Checks the dependencies of the proposed plan
		*/
		void CheckDependenciesL(void);

		/**
		Helper function to check the dependencies for an application and call a 
		function which checks the dependencies for Embedded applications.
		@param aApplication - The application to check the depencies for.
		*/
		void DoCheckDependenciesL(const CApplication& aApplication);
		
		/**
		Checks that proposed plan does not unduly overwrite files
		*/
		void CheckOverWritesL(void);

		/**
		Helper function to check file overwrites for an application and iterate through
		the embedded applications.
		@param aApplication - The application to check for overwrites.
		*/
		void DoCheckOverWritesL(const CApplication& aApplication);

		/**
		Indicates if a file already on the device would be overwritten when an upgrade was not in progress
		@param aApplication - the application to consider
		@return - ETrue if there would be an unsanctioned overwrite
		*/
		TBool WouldOverWriteL(const CApplication& aApplication);

		/**
		Indicates whether a file is removed anywhere in this plan.
		@param aFileName - the filename to check
		@return - ETrue if the filename is present in the plan to remove.
		*/
		TBool FileRemovedInPlanL(TFileName& aFileName);

		/**
		Indicates whether a file is removed in this application.
		@param aApplication - the application to check within.
		@param aFileName - the filename to check.
		@return - ETrue if the filename is present in the plan to remove.
		*/
		TBool FileRemovedInApplication(const CApplication& aApplication, TFileName& aFileName);

		/**
		Returns the owning package of the specified file
		@param aFileName - The file name to check for
		@return - pointer to package which owns this file.
		*/
		CSisRegistryPackage* GetOwnerPackageL(const TDesC& aFileName);

		/**
		Function to check the dependencies for an Embedded application and iterate through
		the embedded applications.
		@param aApplication - The application to check the depencies for.
		*/
		void DoCheckDependenciesEmbeddedAppL(const CApplication& aApplication);	

		/**
		Function to extract the unadorned filename from a file descriptor
		@param aFileDescriptor - file descriptor to get filename from
		@param aFilename - returned filename
		*/
		void GetUnadornedFileName(const CSisRegistryFileDescription& aFileDescription, TFileName* aFilename);

	private:
		/**
		The planned installation
		*/
		const CPlan&	iPlan; 

		/**
		Whether we allow the overwrite of orphaned files. This originates from the security policy.
		*/
		TBool iAllowOrphanedOverwrite;
		/**
		If we allow the user to overwrite orphaned files, should we also allow
		orphaned files in private import directories to be overwritten. This
		originates from the security policy and is only meaningful if
		iAllowOrphanedOverwrite is true.
		*/
		TBool iAllowPrivateOrphanOverwrite;
		};

	} // end Swi::

#endif
