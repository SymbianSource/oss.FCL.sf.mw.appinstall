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
* Description: Unit test definitions for SIF UI device dialog.
*
*/

#ifndef UT_SIFUIDEVICEDIALOG_H
#define UT_SIFUIDEVICEDIALOG_H

#include <QtTest/QtTest>

class TestSifUiDeviceDialog : public QObject
{
    Q_OBJECT

private slots:
    // Called first before first test case
    void initTestCase();
    // Called before each test case
    void init();
    // Called after every test case
    void cleanup();
    // Called after the last test case
    void cleanupTestCase();

    // Test cases
    void showInfoNote();

private:
    

};


#endif  //  UT_SIFUIDEVICEDIALOG_H

