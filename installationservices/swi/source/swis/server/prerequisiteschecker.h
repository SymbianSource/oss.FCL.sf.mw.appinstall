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

#ifndef __PREREQUISITESCHECKER_H__
#define __PREREQUISITESCHECKER_H__

#include <e32base.h>
#include <f32file.h>
#include "sisregistrysession.h"
#include "requisiteschecker.h"

namespace Swi 
	{
	class RUiHandler;
	class CSISInstallationResult;	
	class CInstallationResult;
	class CApplication;
	class CContentProvider;

	namespace Sis
		{
		class CController;
		class CDependency;
		class CVersionRange;
		}
 
	/**
	Performs the initial (preplanning) requisite checks, e.g. whether the app 
	is compatible with the actual device.
	*/
	class CPrerequisitesChecker : public CRequisitesChecker
		{
		public:
		/**
		This creates a new CPrerequisitesChecker object, and adds it to cleanup stack
		@param aInstallerUI - An implementation of the Installer UI
		@param aController - The controller
		@param aInstallationResult - Installation result
		*/
		static CPrerequisitesChecker* NewLC(RUiHandler& aInstallerUI, 
										const Sis::CController& aController, 
										CInstallationResult& aInstallationResult,
										const CContentProvider& aProvider,
										const Sis::CController& aMainController);

		/**
		The destructor.
		*/
		~CPrerequisitesChecker();

		private:
		/**
		The constructor.
		@param aInstallerUI - An implementation of the Installer UI
		@param aController - The controller
		@param aInstallationResult - Installation result
		*/

		CPrerequisitesChecker(RUiHandler& aInstallerUI, 
							const Sis::CController& aController,
							CInstallationResult& aInstallationResult,
							const CContentProvider& aProvider,
							const Sis::CController& aMainController);
		
		/**
		The second-phase constructor.
		*/
		void ConstructL();
		
		public:
		/**
		Public function to kick off prerequisites check
		*/
		void CheckPrerequisitesL();

	private:
		/**
		Checks the dependencies of the top-level controller
		*/
		void CheckDependenciesL(void);

		/**
		Checks that the embedding depth is not excessive
		@param aController - the currently scanned controller
		@param aCurrentDepth - the current depth of nesting of embedded SIS files
		*/
		void CheckEmbeddingDepthL(const Sis::CController& aController, TInt aCurrentDepth=0);
		
		/**
		Checks that the application is removable
		*/
		void CheckIfRemovableL(void);

	private:
		/**
		The maximum depth to which SIS files may be embedded
		*/
		static const TInt	KMaximumEmbeddingDepth = 8;
		
		/**
		The top-level controller in the SIS file
		*/
		const Sis::CController&	iController;
		
		/**
		The Main Controller in the SIS file
		*/
		const Sis::CController&	iMainController;
		};

	} // end Swi::

#endif
