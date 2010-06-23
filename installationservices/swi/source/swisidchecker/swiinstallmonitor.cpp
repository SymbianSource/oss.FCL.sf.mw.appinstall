/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "swiinstallmonitor.h"

//
// CSwiInstallMonitor class
//
CSwiInstallMonitor* CSwiInstallMonitor::NewL(TCallBack aCallBack)
	{
	CSwiInstallMonitor* self = new(ELeave) CSwiInstallMonitor(aCallBack);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSwiInstallMonitor::CSwiInstallMonitor(TCallBack aCallBack) :
		CActive(EPriorityLow),
		iCallBack(aCallBack)
	{
	CActiveScheduler::Add(this);
	}

void CSwiInstallMonitor::ConstructL()
	{
	User::LeaveIfError(iSwisProperty.Attach(KUidSystemCategory, Swi::KUidSoftwareInstallKey));
	}

CSwiInstallMonitor::~CSwiInstallMonitor()
	{
	Cancel();
	iSwisProperty.Close();
	}

void CSwiInstallMonitor::Start()
	{
	DoStart();
	}

void CSwiInstallMonitor::RunL()
	{
	if(iStatus.Int() == KErrNone)
		{
		DoStart();
		// SWI is idle, so we must completed an install, uninstall or
		// restore operation.
		// We do not try and ignore failed/aborted operations because
		// if we key off of the success state we might miss it...
		if (SwisOperation(iSwisState) == Swi::ESwisNone)
			{
			iCallBack.CallBack();
			}
		}
	}

void CSwiInstallMonitor::DoStart()
	{
	iSwisProperty.Subscribe(iStatus);
	SetActive();
	
	TInt err = iSwisProperty.Get(KUidSystemCategory, Swi::KUidSoftwareInstallKey, iSwisState);
	
	if(err != KErrNone)
		{
		iSwisProperty.Cancel();
		}
	}

void CSwiInstallMonitor::DoCancel()
	{
	iSwisProperty.Cancel();
	}

// End of file
