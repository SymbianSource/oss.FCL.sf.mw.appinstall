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
*     CGlobalProgressDialogHandler implementation
*
*
*/


#include <avkon.hrh>
#include "ADMGlobalProgressDialogHandler.h"

CGlobalProgressDialogHandler::~CGlobalProgressDialogHandler()
	{
	Cancel();
	}

void CGlobalProgressDialogHandler::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

CGlobalProgressDialogHandler* CGlobalProgressDialogHandler::NewLC(MGlobalProgressDialogObserver& aObserver)
	{
	CGlobalProgressDialogHandler* self = new (ELeave) CGlobalProgressDialogHandler(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CGlobalProgressDialogHandler* CGlobalProgressDialogHandler::NewL(MGlobalProgressDialogObserver& aObserver)
	{
	CGlobalProgressDialogHandler* self = CGlobalProgressDialogHandler::NewLC(aObserver);
	CleanupStack::Pop(self);
	return self;
	}

void CGlobalProgressDialogHandler::DoCancel()
	{
	// Nothing to be done here
	}

void CGlobalProgressDialogHandler::StartHandler()
	{
	SetActive();
	}

void CGlobalProgressDialogHandler::RunL()
	{
	if(iStatus == EAknSoftkeyCancel)
		{
		iObserver.ProcessCancelOptionL();
		}
	else if(iStatus == EAknSoftkeyDone)
		{
		iObserver.ProcessDoneOptionL();
		}
	else
		{
		}
	Cancel();
	}
//EOF
