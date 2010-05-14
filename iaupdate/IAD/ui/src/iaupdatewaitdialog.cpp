/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of IAUpdateWaitDialog
*                class member functions.
*
*/



// INCLUDE FILES
#include <hbprogressdialog.h>

#include "iaupdatewaitdialog.h"
#include "iaupdatewaitdialogobserver.h"



// ========================= MEMBER FUNCTIONS ================================

// -----------------------------------------------------------------------------
// IAUpdateWaitDialog::IAUpdateWaitDialog
// -----------------------------------------------------------------------------
//
IAUpdateWaitDialog::IAUpdateWaitDialog() 
{
    mCallback = NULL;
    mWaitDialog = NULL;
}

// -----------------------------------------------------------------------------
// IAUpdateWaitDialog::~IAUpdateWaitDialog
// Destructor
// -----------------------------------------------------------------------------
//
IAUpdateWaitDialog::~IAUpdateWaitDialog()
{
    if (mWaitDialog)
    {
        //mWaitDialog->close();  //TODO: is there need to close/delete progressdialog, it's selfdeleted in close()
    }
}

// ---------------------------------------------------------------------------
// IAUpdateWaitDialog::showDialog()
// ---------------------------------------------------------------------------
//
int IAUpdateWaitDialog::showDialog(const QString& text)
{
    if (!mWaitDialog)
    {
        mWaitDialog = new HbProgressDialog(HbProgressDialog::WaitDialog);
        connect(mWaitDialog, SIGNAL(cancelled()), this, SLOT(dialogCancelled()));
    }
    mWaitDialog->setText(text);
    mWaitDialog->setTimeout(HbPopup::NoTimeout);
    mWaitDialog->show();
    return 0;
}
// ---------------------------------------------------------------------------
// IAUpdateWaitDialog::SetCallback
// ---------------------------------------------------------------------------
//
void IAUpdateWaitDialog::SetCallback(MIAUpdateWaitDialogObserver* callback)
{
    mCallback = callback;
}

void IAUpdateWaitDialog::close()
{
    if (mWaitDialog)  
    {
       mWaitDialog->close();     
    }
}

void IAUpdateWaitDialog::dialogCancelled()
{
    if (mCallback)
    {
        mCallback->HandleWaitDialogCancel();        
    }
}
// End of File
