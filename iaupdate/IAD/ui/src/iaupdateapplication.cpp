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
    mEngine (new IAUpdateEngine)
{
    connect(&(*mEngine), SIGNAL(createUI()),
                &(*this), SLOT(createUI()));
}

IAUpdateApplication::~IAUpdateApplication()
{
    delete mEngine;    
}

void IAUpdateApplication::createUI()
{ 
    if (mMainWindow.isNull())
    {    
        mMainWindow = (QSharedPointer<IAUpdateMainWindow>) new IAUpdateMainWindow(mEngine); 
        // get mainview 
        IAUpdateMainView* mainView = mMainWindow->GetMainView();
        
        // get setting view
        CIAUpdateSettingDialog* settingView = mMainWindow->GetSettingView();
        
        // Connect view change signals to the view change slots
        connect(&(*settingView), SIGNAL(toMainView()), &(*mMainWindow), SLOT(toMainView()));
        connect(&(*mainView), SIGNAL(toSettingView()), &(*mMainWindow), SLOT(toSettingView()));
        connect(&(*mEngine), SIGNAL(refresh(const RPointerArray<MIAUpdateNode>&, const RPointerArray<MIAUpdateFwNode>&,int)),
                &(*mMainWindow), SLOT(refreshMainView(const RPointerArray<MIAUpdateNode>&, const RPointerArray<MIAUpdateFwNode>&,int)));
        connect(&(*mEngine), SIGNAL(refreshProgress()),
                &(*mMainWindow), SLOT(refreshMainViewProgress()));
        connect(&(*mEngine), SIGNAL(setUpdatesRefreshing(bool)),
                &(*mMainWindow), SLOT(setRefreshingAnimation(bool)));
        connect(&(*mEngine), SIGNAL(updateCompleted()),
                &(*mMainWindow), SLOT(updateCompleted()));
    }
}
