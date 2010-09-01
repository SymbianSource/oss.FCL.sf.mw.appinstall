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
* Description:   This file contains the implementation of CIAUpdateProgressDialog
*                class member functions.
*
*/



// INCLUDE FILES
#include <eikprogi.h>

#include "iaupdateprogressdialog.h"
#include "iaupdatewaitdialogobserver.h"



// ========================= MEMBER FUNCTIONS ================================

// -----------------------------------------------------------------------------
// CIAUpdateProgressDialog::CIAUpdateProgressDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateProgressDialog::CIAUpdateProgressDialog( CEikDialog** aSelfPtr, 
                                                  TBool aVisibilityDelayOff )
    :CAknProgressDialog( aSelfPtr, aVisibilityDelayOff )
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateProgressDialog::~CIAUpdateProgressDialog
// Destructor
// -----------------------------------------------------------------------------
//
CIAUpdateProgressDialog::~CIAUpdateProgressDialog()
    {
    }

// ---------------------------------------------------------------------------
// CIAUpdateProgressDialog::OkToExitL()
// called by framework when the Softkey is pressed. 
// On cancel either stop installation or exit 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateProgressDialog::OkToExitL( TInt aButtonId )
    {
    TBool result( ETrue );
    
    if ( iCallback )
        {
        result = iCallback->HandleDialogExitL( aButtonId );
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CIAUpdateProgressDialog::SetCallback
// ---------------------------------------------------------------------------
//
void CIAUpdateProgressDialog::SetCallback( MIAUpdateWaitDialogObserver* aCallback )
    {
    iCallback = aCallback;
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateProgressDialog::SetProgressDialogFinalValueL
// Set final value for the progress bar in progress dialog.
// -----------------------------------------------------------------------------
//
void CIAUpdateProgressDialog::SetProgressDialogFinalValueL( TInt aValue )
    {
    if ( aValue != iMaxProgress )
        {
    	CEikProgressInfo* progressInfo = GetProgressInfoL();
        progressInfo->SetFinalValue( aValue );
        iMaxProgress = aValue;
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateProgressDialog::UpdateProgressDialogValueL
// Increments the progress bar in progress dialog.
// -----------------------------------------------------------------------------
//
void CIAUpdateProgressDialog::UpdateProgressDialogValueL( TInt aValue )
    {
    CEikProgressInfo* progressInfo = GetProgressInfoL();
    progressInfo->SetAndDraw( aValue );
    }

// End of File
