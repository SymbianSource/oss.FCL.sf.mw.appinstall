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
* Description:  Test startup list update launcher application
*
*/

#ifndef TESTSLULAUNCHER_H
#define TESTSLULAUNCHER_H

#include <hbapplication.h>

class HbMainWindow;
class HbView;


class TestStartupListUpdateLauncher : public HbApplication
{
    Q_OBJECT

public:     // constructor and destructor
    TestStartupListUpdateLauncher(int& argc, char* argv[]);
    ~TestStartupListUpdateLauncher();

private slots:
    void updateStartupList();
    void closeApp();

private:    // functions

private:    // data
    HbMainWindow *mMainWindow;
    HbView       *mMainView;
};

#endif  // TESTSLULAUNCHER_H
