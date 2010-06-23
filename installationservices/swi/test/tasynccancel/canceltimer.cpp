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


#include "canceltimer.h"



CCancelTimer::CCancelTimer():CTimer(CActive::EPriorityStandard)
	{
	iInstaller=NULL;
	iStopScheduler=0;
	iWaitWhileCanceling=0;
	}
	
CCancelTimer::~CCancelTimer()
	{
	Cancel();
	delete iInstaller;
	}
	
void CCancelTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

CCancelTimer* CCancelTimer::NewLC(CInstallPrefs* aPrefs)
	{
	CCancelTimer* result= new (ELeave) CCancelTimer();
	CleanupStack::PushL(result);
	result->ConstructL();
	result->iInstaller= CInstaller::NewL(aPrefs);
	
	return result;
	}

CCancelTimer* CCancelTimer::NewL(CInstallPrefs* aPrefs)
	{
	CCancelTimer* result= NewLC(aPrefs);
	CleanupStack::Pop(result);
	
	return result;
	}


void CCancelTimer::RunL()
	{

	iTime++;


	// Waiting for the cancellation (or whole installation) to complete
	// to stop the ActiveScheduler
	
	if (iWaitWhileCanceling)
		{
		if (iInstaller->FinishedCanceling())
			{
			if (iStopScheduler) 
				{
				StopScheduler();
				}
			else 
				{
				iStopScheduler=1;
				// Give some time (1 second) to server so that it doesn't report it's busy
				SetNextTick(1000000);
				}
			}
		else 
			{
			SetNextTick();
			}
		
		return;
		}

	
	// Tick until it's time to cancel installation
	
	if (iTime==iWhenToCancel)
		{
		iWaitWhileCanceling=1;
		iInstaller->CancelInstallation();
		// don't StopScheduler(); now since CancelInstallation also async => need scheduler to be running
		}

	SetNextTick();
	
	}

void CCancelTimer::SetNextTick(TInt aTime)
	{
	After(TTimeIntervalMicroSeconds32(aTime));
	}

void CCancelTimer::StartL(const TDesC& aSisToInstall, TInt aWhenToCancel, TInt aDontCancel)
	{
	iTime=0;	
	iWhenToCancel= aWhenToCancel;
	iStopScheduler=0;
	
	iWaitWhileCanceling=(aDontCancel==0)?0:1;
	
	// Set up installation
	iInstaller->StartL(aSisToInstall);
	
	CActiveScheduler::Add(this);
	SetNextTick();
	}

void CCancelTimer::StopScheduler()
	{
	CActiveScheduler::Stop();
	}

// End of file
