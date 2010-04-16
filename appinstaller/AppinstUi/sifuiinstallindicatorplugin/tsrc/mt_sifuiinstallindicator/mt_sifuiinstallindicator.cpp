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

#include "mt_sifuiinstallindicator.h"
#include "sifuiinstallindicatorparams.h"
#include <hbindicator.h>
#include <QTest>


// ---------------------------------------------------------------------------
// TestSifUiInstallIndicator::activateWithoutParams()
// ---------------------------------------------------------------------------
//
void TestSifUiInstallIndicator::activateWithoutParams()
{
    HbIndicator *indicator = new HbIndicator();
    QVERIFY(indicator != 0);
    QVERIFY(indicator->error() == 0);
    QVERIFY(indicator->activate(KSifUiIndicatorPlugin));
    QTest::qWait(3000);
    QVERIFY(indicator->deactivate(KSifUiIndicatorPlugin));
    delete indicator;
}

// ---------------------------------------------------------------------------
// TestSifUiInstallIndicator::activateWithAppName()
// ---------------------------------------------------------------------------
//
void TestSifUiInstallIndicator::activateWithAppName()
{
    HbIndicator *indicator = new HbIndicator();
    QVERIFY(indicator != 0);
    QVERIFY(indicator->error() == 0);

    QVariantMap map;
    map.insert(KSifUiIndicatorApplicationName, QString("Application name"));
    QVariant params(map);
    QVERIFY(indicator->activate(KSifUiIndicatorPlugin, params));
    QTest::qWait(3000);
    QVERIFY(indicator->deactivate(KSifUiIndicatorPlugin));
    delete indicator;
}

// ---------------------------------------------------------------------------
// TestSifUiInstallIndicator::activateWithInvalidParam()
// ---------------------------------------------------------------------------
//
void TestSifUiInstallIndicator::activateWithInvalidParam()
{
    HbIndicator *indicator = new HbIndicator();
    QVERIFY(indicator != 0);
    QVERIFY(indicator->error() == 0);

    QVariant params(QByteArray("test"));
    QVERIFY(indicator->activate(KSifUiIndicatorPlugin, params));
    QTest::qWait(3000);
    QVERIFY(indicator->deactivate(KSifUiIndicatorPlugin));
    delete indicator;
}

// ---------------------------------------------------------------------------
// Main QTestLib definitions
// ---------------------------------------------------------------------------
//
QTEST_MAIN(TestSifUiInstallIndicator)
#include "moc_mt_sifuiinstallindicator.cpp"

