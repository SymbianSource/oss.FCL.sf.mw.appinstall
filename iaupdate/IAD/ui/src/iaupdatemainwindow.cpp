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
* Description:   This module contains the implementation of IAUpdateMainWindow 
*                class member functions.
*
*/

#include <hbdocumentloader.h>

#include "iaupdatemainwindow.h"
#include "iaupdateengine.h"
#include "iaupdatemainview.h"
#include "iaupdatesettingdialog.h"


// ViewManager Constructor
IAUpdateMainWindow::IAUpdateMainWindow(IAUpdateEngine *engine)
{
    // Add the views to the main window
    addMainView(engine);
    addSettingView();
    
    // show the main window (which will display the last view that was added)
    show();   //temp
}

// Destructor
IAUpdateMainWindow::~IAUpdateMainWindow()
{
}

// Methods to add views to the main window
void IAUpdateMainWindow::addMainView(IAUpdateEngine *engine)
{
    mMainView = new IAUpdateMainView(engine);
    addView(mMainView);
}


void IAUpdateMainWindow::addSettingView()
{
    
    mSettingView = new CIAUpdateSettingDialog(mMainView);
    addView(mSettingView);
}

// Slots to handle view change
void IAUpdateMainWindow::toMainView()
{
    setCurrentView(mMainView);
}

void IAUpdateMainWindow::toSettingView()
{
    setCurrentView(mSettingView);
}
void IAUpdateMainWindow::refreshMainView(const RPointerArray<MIAUpdateNode>& nodes,
                                         const RPointerArray<MIAUpdateFwNode>& fwNodes,
                                         int error)
{
    mMainView->refresh(nodes,fwNodes,error);  
    show(); 
} 




IAUpdateMainView* IAUpdateMainWindow::GetMainView()
{
    return mMainView;    
}
   
CIAUpdateSettingDialog* IAUpdateMainWindow::GetSettingView()
{
    return mSettingView;}


