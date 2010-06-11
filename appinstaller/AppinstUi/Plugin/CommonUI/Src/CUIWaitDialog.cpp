/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CCUIWaitDialog
*                class member functions.
*
*/


// INCLUDE FILES
#include "CUIWaitDialog.h"
#include "CUIDialogs.h"

using namespace SwiUI::CommonUI;

// ========================= MEMBER FUNCTIONS ================================

// -----------------------------------------------------------------------------
// CCUIDialogs::CCUIDialogs
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCUIWaitDialog::CCUIWaitDialog( CEikDialog** aSelfPtr, 
                                TBool aVisibilityDelayOff )
    :CAknWaitDialog( aSelfPtr, aVisibilityDelayOff )
    {
    }

// Destructor
CCUIWaitDialog::~CCUIWaitDialog()
    {
    if (iSelfPtr) 
        {
        *iSelfPtr = NULL;
        iSelfPtr = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CCUIWaitDialog::OkToExitL()
// called by framework when the Softkey is pressed. 
// On cancel either stop installation or exit 
// ---------------------------------------------------------------------------
//
TBool CCUIWaitDialog::OkToExitL( TInt aButtonId )
    {
    TBool result( ETrue );
    
    if ( iCallback )
        {
        result = iCallback->HandleDialogExitL( aButtonId );
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CCUIWaitDialog::SetCallback
// ---------------------------------------------------------------------------
//
void CCUIWaitDialog::SetCallback( MCUIWaitDialogCallback* aCallback )
    {
    iCallback = aCallback;
    }

// End of File
