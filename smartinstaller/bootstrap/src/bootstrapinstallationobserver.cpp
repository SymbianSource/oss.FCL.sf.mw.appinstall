/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*     CInstallObserver class implementation.
*
*
*/


#include <e32base.h>
#include <swinstdefs.h>
#include "bootstrapinstallationobserver.h"
#include "bootstrapstatemachine.h"

CInstallObserver::CInstallObserver(CStateMachine* aStateMachineObj) :
	CActive(CActive::EPriorityStandard),
	iStateMachineObj(aStateMachineObj)
	{
	CActiveScheduler::Add(this);
	}

CInstallObserver::~CInstallObserver()
	{
	Cancel();
	}

void CInstallObserver::RunL()
	{
	// Invoke the handler function installation complete,
	// passing the status
	iStateMachineObj->HandleInstallCompleteL(iStatus.Int());
	}

TInt CInstallObserver::RunError(TInt aError)
	{
	iStateMachineObj->HandleFailure(aError);
	return KErrNone;
	}

void CInstallObserver::DoCancel()
	{
	}

void CInstallObserver::Start()
	{
	SetActive();
	}
