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
* Description:  Test applicaiton for SW install indicator plugin
*
*/

#ifndef TESTINDICAPPLICATION_H
#define TESTINDICAPPLICATION_H

#include <hbapplication.h>

class HbMainWindow;
class HbView;
class HbIndicator;


class TestInstallIndicator : public HbApplication
{
    Q_OBJECT

public:     // constructor and destructor
    TestInstallIndicator(int& argc, char* argv[]);
    ~TestInstallIndicator();

private slots:  // new functions
    void activateStateChanged(int state);

private:    // data
    HbMainWindow *mMainWindow;
    HbView       *mMainView;
    HbIndicator  *mIndicator;
};

#endif  // TESTINDICAPPLICATION_H

