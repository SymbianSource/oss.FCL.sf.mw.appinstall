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
* Description:   This module contains the implementation of IAUpdateApplication 
*                class member functions.
*
*/

#include "iaupdateapplication.h"
#include "iaupdatemainwindow.h"
#include "iaupdateengine.h"
#include "iaupdatemainview.h"
#include "iaupdatesettingdialog.h"

IAUpdateApplication::IAUpdateApplication(  int argc, char* argv[] ) :
    HbApplication( argc, argv ),
    mEngine (new IAUpdateEngine),
    mMainWindow (new IAUpdateMainWindow(mEngine))
    {
    
    // get mainview 
    IAUpdateMainView* mainView = mMainWindow->GetMainView();
    
    // get settig view
    CIAUpdateSettingDialog* settingView = mMainWindow->GetSettingView();
    
    // Connect view change signals to the view change slots
    //connect(&(*mEngine), SIGNAL(toMainView()), &(*mMainWindow), SLOT(toMainView()));
    connect(&(*settingView), SIGNAL(toMainView()), &(*mMainWindow), SLOT(toMainView()));
    connect(&(*mainView), SIGNAL(toSettingView()), &(*mMainWindow), SLOT(toSettingView()));
    connect(&(*mEngine), SIGNAL(refresh(const RPointerArray<MIAUpdateNode>&, const RPointerArray<MIAUpdateFwNode>&,int)),
            &(*mMainWindow), SLOT(refreshMainView(const RPointerArray<MIAUpdateNode>&, const RPointerArray<MIAUpdateFwNode>&,int)));
    }

IAUpdateApplication::~IAUpdateApplication()
{
    delete mEngine;    
}
