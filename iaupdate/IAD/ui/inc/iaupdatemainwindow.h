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
* Description:   This file contains the header file of the IAUpdateMainWindow
*                class 
*
*/

#ifndef IAUPDATEMAINWINDOW_H_
#define IAUPDATEMAINWINDOW_H_

#include <qobject>
#include <hbmainwindow.h>

// Forward declarations
class IAUpdateMainView;
class IAUpdateHistoryView;
class HbDocumentLoader;
class MIAUpdateNode;
class MIAUpdateFwNode;
class IAUpdateServiceProvider;  //temp

class IAUpdateMainWindow : public HbMainWindow
    {
    Q_OBJECT
    
public:
    IAUpdateMainWindow();
    virtual ~IAUpdateMainWindow();

public slots:
    void toMainView();
    void refreshMainView(const RPointerArray<MIAUpdateNode>& nodes,
                         const RPointerArray<MIAUpdateFwNode>& fwNodes,
                         int error);
    void toHistoryView();

private:
    void addMainView();
    void addHistoryView();
    
private:
    IAUpdateMainView *mMainView;
    IAUpdateHistoryView *mHistoryView;
    
    };

#endif /* IAUPDATEMAINWINDOW */
