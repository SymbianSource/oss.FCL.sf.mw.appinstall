/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include "progressdialog.h"
#include <aknglobalprogressdialog.h>
CProgressDialog* CProgressDialog::NewL(MProgressDialogClient& aOwner)
{
	CProgressDialog* self = new(ELeave) CProgressDialog(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CProgressDialog::CProgressDialog(MProgressDialogClient& aOwner) :
	CActive(EPriorityStandard), iOwner(aOwner)
{
}

void CProgressDialog::ConstructL()
{
	iProgress = CAknGlobalProgressDialog::NewL();
	CActiveScheduler::Add(this);
}

void CProgressDialog::StartL(const TDesC& aPrompt, TInt aFinal)
{
	if(!IsActive()) {
		iProgress->ShowProgressDialogL(iStatus, aPrompt, 0, aFinal);
		SetActive();
	}
	iCurrentValue = 0;
	iFinalValue = aFinal;
}

void CProgressDialog::Update(TInt aProgress, TInt aFinal)
{
	iCurrentValue = aProgress;
	iFinalValue = aFinal;
	iProgress->UpdateProgressDialog(aProgress, iFinalValue);
}

void CProgressDialog::Add(TInt aProgress)
{
	iCurrentValue += aProgress;
	iProgress->UpdateProgressDialog(iCurrentValue, iFinalValue);
}

void CProgressDialog::Stop()
{
	iProgress->ProcessFinished();
}

void CProgressDialog::RunL()
{
	if(iStatus.Int() == EAknSoftkeyCancel)
		iOwner.ProgressDialogCancelled();
}

CProgressDialog::~CProgressDialog()
{
	Cancel();
	delete iProgress;
}

void CProgressDialog::DoCancel()
{
	iProgress->CancelProgressDialog();
}
