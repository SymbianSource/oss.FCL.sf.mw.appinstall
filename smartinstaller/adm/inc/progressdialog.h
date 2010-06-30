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


#ifndef PROGRESSDIALOG_H_
#define PROGRESSDIALOG_H_

#include <e32base.h>

class CAknGlobalProgressDialog;
class CProgressDialog : public CActive
{
public:
	class MProgressDialogClient
	{
	public:
		virtual void ProgressDialogCancelled() = 0;
	};
	static CProgressDialog* NewL(MProgressDialogClient& aOwner);
	~CProgressDialog();
	void StartL(const TDesC& aPrompt, TInt aFinal=0);
	void Add(TInt aProgress);
	void Update(TInt aProgress, TInt aFinal=-1);
	void Stop();
private:
	CProgressDialog(MProgressDialogClient& aOwner);
	void RunL();
	void DoCancel();
	void ConstructL();

private:
	CAknGlobalProgressDialog* iProgress;
	MProgressDialogClient& iOwner;
	TInt iCurrentValue;
	TInt iFinalValue;
};

#endif /* PROGRESSDIALOG_H_ */
