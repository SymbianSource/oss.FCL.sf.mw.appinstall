/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent
*/


#ifndef INSTALLER_H
#define INSTALLER_H

#include "common.h"
#include "asyncc-tui.h"


/**
 * This class defines the object that is used to start and cancel
 * the installation process
 */
class CInstaller : public CActive
	{

	public:
	
		CInstaller();
		static CInstaller* NewLC(CInstallPrefs* aPrefs=NULL);
		static CInstaller* NewL(CInstallPrefs* aPrefs=NULL);
		~CInstaller();

		/**
 		 * Sets up the installation (but without starting the Active Scheduler)
 		 * @param aSisToInstall		Full path to sis file to be installed
 		 */
		void StartL(const TDesC& aSisToInstall);
		void CancelInstallation();
		
		TInt CancelationSuccess()
			{
			return iCancelationSuccess;
			}
			
		TInt FinishedInstallation()
			{
			return iFinishedInstallation;
			}

		TInt FinishedCanceling()
			{
			return iFinishedCanceling;
			}
		
		TInt Status()
			{
			return iEndStatus;
			}
		
	private:
	
		// Executed when cancelation finishes
		void RunL();
		void DoCancel();
		void ConstructL(CInstallPrefs* aPrefs=NULL);
		
		CAsyncLauncher* iLauncher;
		CInstallPrefs* iPrefs;
		CUIScriptAdaptor* iUiHandler;

		TInt iCancelationSuccess;
		TInt iFinishedInstallation;
		TInt iFinishedCanceling;
		
		TInt iDeletePrefs;
		
		TInt iEndStatus;
	};

#endif
// End of file
