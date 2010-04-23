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

#ifndef CANCELTIMER_H
#define CANCELTIMER_H

#include "common.h"
#include "installer.h"

/**
 * This class defines the timer that is used to start and cancel
 * the installation process at a given time.
 */
class CCancelTimer : public CTimer
	{
	public:
	
		CCancelTimer();
		void ConstructL();		
		static CCancelTimer* NewLC(CInstallPrefs* aPrefs=NULL);
		static CCancelTimer* NewL(CInstallPrefs* aPrefs=NULL);
		
		virtual ~CCancelTimer();
		
		/**
 		 * Sets up the installation (but without starting the Active Scheduler)
 		 * @param aSisToInstall		Full path to sis file to be installed
 		 * @param aWhenToCancel		Point in time (microseconds) when to cancel the installation
 		 * @param aDontCancel		Set to 1 if the installation is not to be canceled
 		 */
		void StartL(const TDesC& aSisToInstall, TInt aWhenToCancel, TInt aDontCancel=0);
		
		// Used to determine how long did the installation process last
		TInt Time()
			{
			return iTime;
			}
		
		TInt CancelationSuccess()
			{
			return iInstaller->CancelationSuccess();
			}
		
		TInt FinishedInstallation()
			{
			return iInstaller->FinishedInstallation();
			}
			
		TInt InstallerStatus()
			{
			return iInstaller->Status();
			}
		
	private:
	
		// Ticks and cancels the installation at reqested time.
		// Since cancelation takes some time to complete, this function also waits
		// for its completition and than stops the Active Scheduler
		void RunL();

		// Sets the interval after which the next tick will occur
		void SetNextTick(TInt aTime=1);
		
		// Stop the Active Scheduler
		void StopScheduler();
	
		// Keeps the time in order to know when to cancel.
		// Also useful to determine how long did the installation process last
		TInt iTime;
		
		TInt iWhenToCancel;
		
		// Is equal to 1 if ActiveScheduler should be stopped the next time
		// RunL executes
		TInt iStopScheduler;
		// Is equal to 1 if the CancelOperation() was requested and the program
		// is waiting for the cancelation to finish
		TInt iWaitWhileCanceling;
		
		// Runs the installation and cancelation
		CInstaller* iInstaller;
		
	};
	
	
#endif
// End of file
