/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES

#include "iaupdatefwsyncstate.h"
#include "iaupdatefwsyncutil.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
  CIAUpdateFWSyncState* CIAUpdateFWSyncState::NewL()
    {
    CIAUpdateFWSyncState* self = new(ELeave) CIAUpdateFWSyncState();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// ~CIAUpdateFWSyncState
//
// Destructor.
// -----------------------------------------------------------------------------
//
  CIAUpdateFWSyncState::~CIAUpdateFWSyncState()
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncState
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateFWSyncState::CIAUpdateFWSyncState()
    {
    }

// -----------------------------------------------------------------------------
// ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncState::ConstructL()
    {
	Reset();
    }

// -----------------------------------------------------------------------------
// Reset
//
// Resets all state variables.
// -----------------------------------------------------------------------------
//
  void CIAUpdateFWSyncState::Reset()
    {
	iSyncPhase = EPhaseNone;
    }


// -----------------------------------------------------------------------------
// SetSyncPhase
//
// -----------------------------------------------------------------------------
//
  void CIAUpdateFWSyncState::SetSyncPhase(TInt aPhase)
	{
	iSyncPhase = aPhase;
	}


// -----------------------------------------------------------------------------
// SyncPhase
//
// -----------------------------------------------------------------------------
//
  TInt CIAUpdateFWSyncState::SyncPhase()
	{
	return iSyncPhase;
	}


// -----------------------------------------------------------------------------
// SetProgress
//
// -----------------------------------------------------------------------------
//
  void CIAUpdateFWSyncState::SetProgress(TInt aCount)
	{
	iProgressCount = aCount;
	}

// -----------------------------------------------------------------------------
// IncreaseProgress
//
// -----------------------------------------------------------------------------
//
  void CIAUpdateFWSyncState::IncreaseProgress()
	{
	if (iProgressCount < iTotalProgressCount)
		{
		iProgressCount++;
		}
	}

// -----------------------------------------------------------------------------
// Progress
//
// -----------------------------------------------------------------------------
//
  TInt CIAUpdateFWSyncState::Progress()
	{
	return iProgressCount;
	}

// -----------------------------------------------------------------------------
// SetTotalProgress
//
// -----------------------------------------------------------------------------
//
  void CIAUpdateFWSyncState::SetTotalProgress(TInt aCount)
	{
	iTotalProgressCount = aCount;
	}

// -----------------------------------------------------------------------------
// TotalProgress
//
// -----------------------------------------------------------------------------
//
  TInt CIAUpdateFWSyncState::TotalProgress()
	{
	return iTotalProgressCount;
	}

// -----------------------------------------------------------------------------
// SetContent
//
// -----------------------------------------------------------------------------
//
  void CIAUpdateFWSyncState::SetContent(const TDesC& aContent)
	{
	TUtil::StrCopy(iSyncContent, aContent);
	}

// -----------------------------------------------------------------------------
// Content
//
// -----------------------------------------------------------------------------
//
  const TDesC& CIAUpdateFWSyncState::Content()
	{
	return iSyncContent;
	}

// -----------------------------------------------------------------------------
// ResetProgress
//
// -----------------------------------------------------------------------------
//
  void CIAUpdateFWSyncState::ResetProgress()
	{
	iProgressCount = 0;
	iTotalProgressCount = 0;
	iSyncContent = KNullDesC;
	}

// -----------------------------------------------------------------------------
// ProgressKnown
//
// -----------------------------------------------------------------------------
//
  TBool CIAUpdateFWSyncState::ProgressKnown()
	{
	if (iTotalProgressCount == -1 )
		{
		return EFalse;
		}

	return ETrue;
	}


