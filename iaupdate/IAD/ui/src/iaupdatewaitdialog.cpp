/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CIAUpdateWaitDialog
*                class member functions.
*
*/



// INCLUDE FILES
#include "iaupdatewaitdialog.h"
#include "iaupdatewaitdialogobserver.h"



// ========================= MEMBER FUNCTIONS ================================

// -----------------------------------------------------------------------------
// CIAUpdateDialogs::CIAUpdateDialogs
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateWaitDialog::CIAUpdateWaitDialog( CEikDialog** aSelfPtr, 
                                TBool aVisibilityDelayOff )
    :CAknWaitDialog( aSelfPtr, aVisibilityDelayOff )
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateDialogs::~CIAUpdateDialogs
// Destructor
// -----------------------------------------------------------------------------
//
CIAUpdateWaitDialog::~CIAUpdateWaitDialog()
    {
    }

// ---------------------------------------------------------------------------
// CIAUpdateWaitDialog::OkToExitL()
// called by framework when the Softkey is pressed. 
// On cancel either stop installation or exit 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateWaitDialog::OkToExitL( TInt aButtonId )
    {
    TBool result( ETrue );
    
    if ( iCallback )
        {
        result = iCallback->HandleDialogExitL( aButtonId );
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CIAUpdateWaitDialog::SetCallback
// ---------------------------------------------------------------------------
//
void CIAUpdateWaitDialog::SetCallback( MIAUpdateWaitDialogObserver* aCallback )
    {
    iCallback = aCallback;
    }

// End of File
