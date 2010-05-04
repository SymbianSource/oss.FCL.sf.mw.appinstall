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
* Description:   This module contains the implementation of IAUpdateMainWindow 
*                class member functions.
*
*/

#include <hbdocumentloader.h>

#include "iaupdatemainwindow.h"
#include "iaupdateengine.h"
#include "iaupdatemainview.h"
#include "iaupdatehistoryview.h"


// ViewManager Constructor
IAUpdateMainWindow::IAUpdateMainWindow()
{
    // Add the views to the main window
    //addHistoryView();
    addMainView();
    // show the main window (which will display the last view that was added)
    show();   //temp
}

// Destructor
IAUpdateMainWindow::~IAUpdateMainWindow()
{
}

// Methods to add views to the main window
void IAUpdateMainWindow::addMainView()
{
    mMainView = new IAUpdateMainView();
    addView(mMainView);
}

void IAUpdateMainWindow::addHistoryView()
{
    mHistoryView = new IAUpdateHistoryView();
    addView(mHistoryView);
}


// Slots to handle view change
void IAUpdateMainWindow::toMainView()
{
    setCurrentView(mMainView);
}

void IAUpdateMainWindow::refreshMainView(const RPointerArray<MIAUpdateNode>& nodes,
                                         const RPointerArray<MIAUpdateFwNode>& fwNodes,
                                         int error)
{
    mMainView->refresh(nodes,fwNodes,error);  
    show(); 
} 


void IAUpdateMainWindow::toHistoryView()
{
    setCurrentView(mHistoryView);
}



