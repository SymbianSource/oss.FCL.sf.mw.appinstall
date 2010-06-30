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
*     Declaration of CGlobalWaitNoteObserver class.
*
*
*/

#ifndef __GLOBALWAITNOTE_H__
#define __GLOBALWAITNOTE_H__

#include <e32base.h>

class CGlobalWaitNote : public CActive
	{
public:
	class MGlobalWaitNoteClient
	{
	public:
		virtual void WaitNoteCancelled() = 0;
	};
	static CGlobalWaitNote* NewL (MGlobalWaitNoteClient& aOwner, TBool aCancellable = ETrue);
	static CGlobalWaitNote* NewLC(MGlobalWaitNoteClient& aOwner, TBool aCancellable = ETrue);

public:
	virtual ~CGlobalWaitNote();
	void ShowNoteL(const TDesC& aText);
	void CancelNoteL();

private:
	void ConstructL();
	void RunL();
	void DoCancel();

private:
   CGlobalWaitNote(MGlobalWaitNoteClient& aOwner, TBool aCancellable);

private:
	MGlobalWaitNoteClient&  iOwner;
	CAknGlobalNote*         iWaitNote;
	TInt                    iWaitNoteId;
	TBool                   iCancellable;
	TRequestStatus          iStatus;
	};

#endif
