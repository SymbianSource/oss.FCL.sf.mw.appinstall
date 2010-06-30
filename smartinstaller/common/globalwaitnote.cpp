/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*     CGlobalWaitNoteObserver Definition.
*
*
*/

#include <aknglobalnote.h>
#include <stringloader.h>
#include <avkon.hrh>
#include <avkon.rsg>

#include "globalwaitnote.h"

CGlobalWaitNote* CGlobalWaitNote::NewLC(MGlobalWaitNoteClient& aOwner, TBool aCancellable)
	{
	CGlobalWaitNote* self = new(ELeave) CGlobalWaitNote(aOwner, aCancellable);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CGlobalWaitNote* CGlobalWaitNote::NewL(MGlobalWaitNoteClient& aOwner, TBool aCancellable)
	{
	CGlobalWaitNote* self = NewLC(aOwner, aCancellable);
	CleanupStack::Pop(self);
	return self;
	}

CGlobalWaitNote::CGlobalWaitNote(MGlobalWaitNoteClient& aOwner, TBool aCancellable) :
	CActive(EPriorityStandard),
	iOwner(aOwner),
	iCancellable(aCancellable)
	{
	CActiveScheduler::Add(this);
	}

void CGlobalWaitNote::ConstructL()
	{
	iWaitNote = CAknGlobalNote::NewL();
	}

CGlobalWaitNote::~CGlobalWaitNote()
	{
	if ( IsActive() )
		{
		Cancel();
		}
	delete iWaitNote;
	iWaitNote = NULL;
	}

void CGlobalWaitNote::ShowNoteL(const TDesC& aText)
	{
	if ( !IsActive() )
		{
		if ( !iCancellable )
			{
			iWaitNote->SetSoftkeys( R_AVKON_SOFTKEYS_EMPTY );
			}
		iWaitNoteId = iWaitNote->ShowNoteL(
			iStatus,
			EAknGlobalWaitNote,
			aText );
		SetActive();
		}
	}

void CGlobalWaitNote::CancelNoteL()
	{
	if ( iWaitNote /* && iWaitNoteId >= 0*/ )
		{
		iWaitNote->CancelNoteL( iWaitNoteId );
		delete iWaitNote;
		iWaitNote = NULL;
		}
	}

void CGlobalWaitNote::RunL()
	{
	// Always cancel i.e. remove the note
	CancelNoteL();
	if ( iCancellable && iStatus.Int() == EAknSoftkeyCancel )
		{
		iOwner.WaitNoteCancelled();
		}
	}

void CGlobalWaitNote::DoCancel()
	{
	}
