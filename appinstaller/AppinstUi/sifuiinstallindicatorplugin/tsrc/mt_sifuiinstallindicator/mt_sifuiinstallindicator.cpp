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
#include <qvaluespacesubscriber.h>
#include <QTest>

QTM_USE_NAMESPACE


// ---------------------------------------------------------------------------
// TestSifUiInstallIndicator::activateWithoutParams()
// ---------------------------------------------------------------------------
//
void TestSifUiInstallIndicator::activateWithoutParams()
{
    QValueSpaceSubscriber *subscriber = 0;
    subscriber = new QValueSpaceSubscriber(KSifUiInstallIndicatorStatusPath);
    QSignalSpy indicatorStatusSpy(subscriber, SIGNAL(contentsChanged()));

    HbIndicator *indicator = new HbIndicator();
    QVERIFY(indicator != 0);
    QVERIFY(indicator->error() == 0);

    QVERIFY(indicator->activate(KSifUiInstallIndicatorType));
    QTest::qWait(3000);
    QCOMPARE(indicatorStatusSpy.count(), 1);
    QVERIFY(indicator->deactivate(KSifUiInstallIndicatorType));
    delete indicator;

    QCOMPARE(indicatorStatusSpy.count(), 2);
    delete subscriber;
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

    QValueSpaceSubscriber *subscriber = 0;
    subscriber = new QValueSpaceSubscriber(KSifUiInstallIndicatorStatusPath);
    QSignalSpy indicatorStatusSpy(subscriber, SIGNAL(contentsChanged()));

    QVariant params(QString("Test application"));
    QVERIFY(indicator->activate(KSifUiInstallIndicatorType, params));
    QTest::qWait(3000);
    QCOMPARE(indicatorStatusSpy.count(), 1);
    QVERIFY(indicator->deactivate(KSifUiInstallIndicatorType));
    delete indicator;

    QCOMPARE(indicatorStatusSpy.count(), 2);
    delete subscriber;
}

// ---------------------------------------------------------------------------
// TestSifUiInstallIndicator::activateWithInvalidParam()
// ---------------------------------------------------------------------------
//
void TestSifUiInstallIndicator::activateWithInvalidParam()
{
    QValueSpaceSubscriber *subscriber = 0;
    subscriber = new QValueSpaceSubscriber(KSifUiInstallIndicatorStatusPath);
    QSignalSpy indicatorStatusSpy(subscriber, SIGNAL(contentsChanged()));

    HbIndicator *indicator = new HbIndicator();
    QVERIFY(indicator != 0);
    QVERIFY(indicator->error() == 0);

    QVariant params(QByteArray("test"));
    QVERIFY(indicator->activate(KSifUiInstallIndicatorType, params));
    QTest::qWait(3000);
    QCOMPARE(indicatorStatusSpy.count(), 1);
    QVERIFY(indicator->deactivate(KSifUiInstallIndicatorType));
    delete indicator;

    QCOMPARE(indicatorStatusSpy.count(), 2);
    delete subscriber;
}

// ---------------------------------------------------------------------------
// Main QTestLib definitions
// ---------------------------------------------------------------------------
//
QTEST_MAIN(TestSifUiInstallIndicator)
#include "moc_mt_sifuiinstallindicator.cpp"

