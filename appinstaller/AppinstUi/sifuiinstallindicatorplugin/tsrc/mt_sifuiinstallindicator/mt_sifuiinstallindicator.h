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
* Description:  Module tests for SW install progress indicator.
*
*/


#ifndef MT_SIFUIINSTALLINDICATOR_H
#define MT_SIFUIINSTALLINDICATOR_H

#include <QtTest/QtTest>

class TestSifUiInstallIndicator: public QObject
{
    Q_OBJECT

private slots:  // test functions
    void activateWithoutParams();
    void activateWithAppName();
    void activateWithInvalidParam();
};

#endif  // MT_SIFUIINSTALLINDICATOR_H

