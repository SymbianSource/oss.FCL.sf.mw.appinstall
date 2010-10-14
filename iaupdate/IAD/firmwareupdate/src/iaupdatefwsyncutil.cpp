/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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




// INCLUDES
#include <SyncMLTransportProperties.h>
#include <collate.h>


#include "iaupdatefwsyncutil.h"
#include "iaupdatefwdebug.h"
#include "iaupdatefwconst.h"

/*****************************************************************************
 * class TUtil
 *****************************************************************************/


// ----------------------------------------------------------------------------
// Panic
// ----------------------------------------------------------------------------
//
 void TUtil::Panic(TInt aReason)
    {
	_LIT(KPanicCategory,"IAUpdateFWSyncUtil");

	User::Panic( KPanicCategory, aReason ); 
    }

// ----------------------------------------------------------------------------
// StrCopy
// String copy with lenght check.
// ----------------------------------------------------------------------------
//
 void TUtil::StrCopy( TDes8& aTarget, const TDesC& aSource )
    {
    aTarget.Copy( aSource.Left(aTarget.MaxLength() ) );
    }

// ----------------------------------------------------------------------------
// StrCopy
// String copy with lenght check.
// ----------------------------------------------------------------------------
//
 void TUtil::StrCopy( TDes& aTarget, const TDesC8& aSource )
    {
	aTarget.Copy( aSource.Left(aTarget.MaxLength() ) );
    }

// ----------------------------------------------------------------------------
// StrCopy
// String copy with lenght check.
// ----------------------------------------------------------------------------
//
 void TUtil::StrCopy( TDes& aTarget, const TDesC& aSource )
    {
	aTarget.Copy( aSource.Left( aTarget.MaxLength() ) );
    }


// ----------------------------------------------------------------------------
// StrToInt
// ----------------------------------------------------------------------------
//
TInt TUtil::StrToInt( const TDesC& aText, TInt& aNum )
	{
    TLex lex( aText );
    TInt err = lex.Val( aNum ); 
	return err;
	}





//*****************************************************************************
//* class CIAUpdateFWActiveCaller
//*****************************************************************************

// ----------------------------------------------------------------------------
// CIAUpdateFWActiveCaller::NewL
// ----------------------------------------------------------------------------
//
  CIAUpdateFWActiveCaller* CIAUpdateFWActiveCaller::NewL( 
                                       MIAUpdateFWActiveCallerObserver* aObserver )
    {
    CIAUpdateFWActiveCaller* self = new(ELeave) CIAUpdateFWActiveCaller( aObserver );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CIAUpdateFWActiveCaller::~CIAUpdateFWActiveCaller()
    {
	Cancel();
	iTimer.Close();
    }

// ----------------------------------------------------------------------------
// CIAUpdateFWActiveCaller::CIAUpdateFWActiveCaller
// ----------------------------------------------------------------------------
//
CIAUpdateFWActiveCaller::CIAUpdateFWActiveCaller( 
                                        MIAUpdateFWActiveCallerObserver* aObserver )
                                      : CActive(CActive::EPriorityStandard )
    {
	iObserver = aObserver;
    }

// ----------------------------------------------------------------------------
// CIAUpdateFWActiveCaller::ConstructL
// ----------------------------------------------------------------------------
//
void CIAUpdateFWActiveCaller::ConstructL()
    {
	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------------------------
// CIAUpdateFWActiveCaller::DoCancel
// ----------------------------------------------------------------------------
//
void CIAUpdateFWActiveCaller::DoCancel()
    {
	iTimer.Cancel();
    }

// ----------------------------------------------------------------------------
// CIAUpdateFWActiveCaller::RunL
// ----------------------------------------------------------------------------
//
void CIAUpdateFWActiveCaller::RunL()
    {
    iObserver->HandleActiveCallL( );
    }

// ----------------------------------------------------------------------------
// CIAUpdateFWActiveCaller::RunError
// ----------------------------------------------------------------------------
//
TInt CIAUpdateFWActiveCaller::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CIAUpdateFWActiveCaller::CompleteSelf
//
// This function calls this class RunL.
// ----------------------------------------------------------------------------
//
void CIAUpdateFWActiveCaller::CompleteSelf()
    {
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
    }

// ----------------------------------------------------------------------------
// CIAUpdateFWActiveCaller::Start
// ----------------------------------------------------------------------------
//
  void CIAUpdateFWActiveCaller::Start( TInt aCallId, TInt aMilliseconds )
    {
	if (IsActive())
		{
		return;
		}
	
	iCallId = aCallId;

	if (aMilliseconds <= 0)
		{
		CompleteSelf();  // no delay - complete right away
		}
	else
		{
	    iTimer.After( iStatus, aMilliseconds*1000 );
	    SetActive();
		}
    }

// End of File
