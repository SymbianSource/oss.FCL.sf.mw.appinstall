/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SW installation settings changer application.
*
*/

#ifndef SWINSTSETTINGSCHANGER_H
#define SWINSTSETTINGSCHANGER_H

#include <hbapplication.h>

class HbMainWindow;
class HbView;
class HbComboBox;
class HbTextEdit;
class HbCheckBox;


class SwInstSettingsChanger : public HbApplication
{
    Q_OBJECT

public:     // constructor and destructor
    SwInstSettingsChanger(int& argc, char* argv[]);
    ~SwInstSettingsChanger();

private slots:  // new functions
    void handleRead();
    void handleSave();

private:    // new functions
    void DoHandleReadL();
    void DoHandleSaveL();

private:    // data
    HbMainWindow *mMainWindow;
    HbView       *mMainView;
    HbComboBox   *mOcspProcedure;
    HbTextEdit   *mDefaultUrl;
    HbCheckBox   *mAllowUntrusted;
};

#endif  // SWINSTSETTINGSCHANGER_H

