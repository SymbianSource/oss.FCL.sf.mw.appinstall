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


#include <apgcli.h>
#include "daemoninstaller.h"
#include "swispubsubdefs.h"
#include "log.h"

const static TInt KAppListRetryWaitTime = 5000000; // 5 secs
_LIT(KMCSisInstaller,"Daemon-Installer"); // Minor-Component name

namespace Swi
	{
	// Two phased construction

	CSisInstaller* CSisInstaller::NewL(MDaemonInstallBehaviour* aDaemonBehaviour)
		{
		CSisInstaller* self = new (ELeave) CSisInstaller(aDaemonBehaviour);
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop(self);
		return self;
		}
	
	// Install Request constructor
	
	CSisInstaller::CSisInstaller(MDaemonInstallBehaviour* aDaemonBehaviour) : CActive(CActive::EPriorityStandard),
		  iDaemonBehaviour(aDaemonBehaviour),
		  iState(MDaemonInstallBehaviour::EStateVerifyAppCacheList),
		  iFileIndex(0),
		  iTimeStep(KAppListRetryWaitTime)		  
		{
		CActiveScheduler::Add(this);
		}
	
	// Install Request destructor

	CSisInstaller::~CSisInstaller()
		{
		Cancel();
		iTimer.Close();
		iProperty.Close();
		iFilesToInstall.ResetAndDestroy();
		iFilesToInstall.Close();
		}
	
	// 2nd phase construction

	void CSisInstaller::ConstructL()
		{
		User::LeaveIfError(iTimer.CreateLocal());
		User::LeaveIfError(iProperty.Attach(KUidSystemCategory,KUidSoftwareInstallKey));
		}
		
	// Set the location of all sis files and the list of them
	// also take ownership of the pointers to memory
		
	void CSisInstaller::AddFileToInstallL(const TDesC& aFileName)
		{
		DEBUG_PRINTF2(_L("SWI Daemon - Queueing presintalled SIS file '%S' for processing"),
			&aFileName);
		
		HBufC* fileName = aFileName.AllocLC();
		iFilesToInstall.AppendL(fileName);
		CleanupStack::Pop(fileName);
		}
	
	// Start the request to process the Sisx file

	void CSisInstaller::StartInstallingL()
		{
		if(iFilesToInstall.Count() > 0)
			{
			iState = MDaemonInstallBehaviour::EStateVerifyAppCacheList;
			ReRequestL();
			}
		}
		
	// Requesting an asyncronous timer/subscrive event

	void CSisInstaller::ReRequestL()
		{
		switch (iState)
			{
		// Verify the app cache list at time intervals
		case MDaemonInstallBehaviour::EStateVerifyAppCacheList:
			iTimer.Cancel();
			iTimer.After(iStatus,iTimeStep);
			SetActive();
			break;
		
		// Verify software installer property 
		case MDaemonInstallBehaviour::EStateVerifySwisProperty:
			CompleteSelf();
			break;
		
		case MDaemonInstallBehaviour::EStateVerifySwisIdle:
			iProperty.Subscribe(iStatus);
			SetActive();
			break;

		case MDaemonInstallBehaviour::EStateInstall:
			CompleteSelf();
			break;

		default:
			// SisInstaller in an incorrect state
			User::Panic(KMCSisInstaller,KErrNotSupported);
			break;			
			}
		}
		
	// Complete the request manually
		
	void CSisInstaller::CompleteSelf()
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status,KErrNone);
		SetActive();
		}

	// Cancel the active request

	void CSisInstaller::DoCancel()
		{
		iProperty.Cancel();
		iTimer.Cancel();
		}

	void CSisInstaller::Reset()
		{
		Cancel();
		iFileIndex = 0;
		iFilesToInstall.ResetAndDestroy();
		}
	
	// When the software installer has changed state
	// attemp to install a sisx file

	void CSisInstaller::RunL()
		{
		if (iStatus.Int() != KErrNone)
			{
			User::Leave(iStatus.Int()); // Invoke RunError
			}
					
		switch (iState)
			{
		// Check the cached list of applications
		case MDaemonInstallBehaviour::EStateVerifyAppCacheList:
			iState = iDaemonBehaviour->VerifyAppCacheListL();
			ReRequestL();
			break;
			
		case MDaemonInstallBehaviour::EStateVerifySwisProperty:
			iState = iDaemonBehaviour->VerifySwisPropertyL();
			ReRequestL();
			break;
		
		// Check the software installer is not busy
		case MDaemonInstallBehaviour::EStateVerifySwisIdle:
			iState = iDaemonBehaviour->VerifySwisIdleL();
			ReRequestL();
			break;
			
		// Install a file
		case MDaemonInstallBehaviour::EStateInstall:
			{
			iSisFile.Copy(*iFilesToInstall[iFileIndex]);
			iDaemonBehaviour->DoInstallRequestL(iSisFile);
			if (++iFileIndex < iFilesToInstall.Count())
				{
				iState = MDaemonInstallBehaviour::EStateVerifySwisProperty;
				CompleteSelf();
				}
			else
				{
				// All done
				iDaemonBehaviour->DoNotifyMediaProcessingComplete();
				}
			
			break;
			}
		
		// Active object in unknown state
		default:
			User::Panic(KMCSisInstaller,KErrNotSupported);
			break;
			}
		}
	
	// If RunL leaves then ignore errors

	TInt CSisInstaller::RunError(TInt aError)
		{
		DEBUG_PRINTF3(_L8("SWI Daemon - Daemon failed in state %d with code %d"),
			iState, aError);
		
		Reset();
		TInt err(KErrNone);
		if (aError == KErrCancel)
			{
			iState = MDaemonInstallBehaviour::EStateVerifyAppCacheList;
			}
		return err;
		}		
	}
