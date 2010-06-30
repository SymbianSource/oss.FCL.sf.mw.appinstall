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
*     CGlobalWaitNoteObserver class implementation.
*
*
*/


#include "bootstrapglobalwaitnoteobserver.h"
#include "bootstrapappui.h"

CGlobalWaitNoteObserver::CGlobalWaitNoteObserver(CBootstrapAppUi* aBsAppUiObj) :
	CActive(CActive::EPriorityStandard),
	iBsAppUiObj(aBsAppUiObj)
	{
	CActiveScheduler::Add(this);
	}

CGlobalWaitNoteObserver::~CGlobalWaitNoteObserver()
	{
	Cancel();
	}

void CGlobalWaitNoteObserver::RunL()
	{
	// Invoke the handler function for cancel
	iBsAppUiObj->HandleGlobalWaitNoteCancel(iStatus.Int());
	}

TInt CGlobalWaitNoteObserver::RunError(TInt aError)
	{
	return KErrNone;
	}

void CGlobalWaitNoteObserver::DoCancel()
	{
	}

void CGlobalWaitNoteObserver::Start()
	{
	SetActive();
	}
