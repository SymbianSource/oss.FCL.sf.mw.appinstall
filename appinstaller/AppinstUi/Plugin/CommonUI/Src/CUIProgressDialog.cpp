/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CCUIProgressDialog
*                class member functions.
*
*/


// INCLUDE FILES
#include "CUIProgressDialog.h"
#include "CUIDialogs.h"

using namespace SwiUI::CommonUI;

// ========================= MEMBER FUNCTIONS ================================

// -----------------------------------------------------------------------------
// CCUIDialogs::CCUIDialogs
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCUIProgressDialog::CCUIProgressDialog( CEikDialog** aSelfPtr,
                                        TBool aVisibilityDelayOff )
    :CAknProgressDialog( aSelfPtr, aVisibilityDelayOff )
    {
    }

// Destructor
CCUIProgressDialog::~CCUIProgressDialog()
    {
    
    if (iSelfPtr) 
        {
        *iSelfPtr = NULL;
        iSelfPtr = NULL;
        }
    
    }

// ---------------------------------------------------------------------------
// CCUIProgressDialog::OkToExitL
// called by framework when the Softkey is pressed.
// ---------------------------------------------------------------------------
//
TBool CCUIProgressDialog::OkToExitL( TInt aButtonId )
    {
    TBool result( ETrue );
    
    if ( iCallback )
        {
        result = iCallback->HandleDialogExitL( aButtonId );
        }
    
    return result;
    }

// ---------------------------------------------------------------------------
// CCUIProgressDialog::SetCallback
// ---------------------------------------------------------------------------
//
void CCUIProgressDialog::SetCallback( MCUIWaitDialogCallback* aCallback )
    {
    iCallback = aCallback;
    }

// ---------------------------------------------------------------------------
// CCUIProgressDialog::PreLayoutDynInitL
// ---------------------------------------------------------------------------
//
void CCUIProgressDialog::PreLayoutDynInitL()
    {
    // prevent background apps getting pointer events when dimmed area tapped
    SetGloballyCapturing( ETrue );
    SetPointerCapture( ETrue );
    }


// End of File
