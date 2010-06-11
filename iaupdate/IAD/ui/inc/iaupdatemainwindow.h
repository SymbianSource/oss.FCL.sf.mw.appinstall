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
* Description:   This file contains the header file of the IAUpdateMainWindow
*                class 
*
*/

#ifndef IAUPDATEMAINWINDOW_H_
#define IAUPDATEMAINWINDOW_H_

#include <QObject>
#include <hbmainwindow.h>

// Forward declarations
class IAUpdateMainView;
class HbDocumentLoader;
class MIAUpdateNode;
class MIAUpdateFwNode;
class IAUpdateEngine;  
class CIAUpdateSettingDialog;

class IAUpdateMainWindow : public HbMainWindow
{
    Q_OBJECT
    
public:
    IAUpdateMainWindow(IAUpdateEngine *engine);
    virtual ~IAUpdateMainWindow();
    
    IAUpdateMainView*  GetMainView();
    CIAUpdateSettingDialog* GetSettingView();

public slots:
    void toMainView();
    void refreshMainView(const RPointerArray<MIAUpdateNode>& nodes,
                         const RPointerArray<MIAUpdateFwNode>& fwNodes,
                         int error);
    void toSettingView();

private:
    void addMainView(IAUpdateEngine *engine);
    void addSettingView();
    
private:
    IAUpdateMainView *mMainView;
    CIAUpdateSettingDialog *mSettingView;
    
};

#endif /* IAUPDATEMAINWINDOW */
