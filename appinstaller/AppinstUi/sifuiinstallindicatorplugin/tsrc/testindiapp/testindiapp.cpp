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
* Description:  Test application for SW install indicator plugin
*
*/

#include "testindiapp.h"
#include <hbmainwindow.h>
#include <hbview.h>
#include <QGraphicsLinearLayout>
#include <hbtextitem.h>
#include <hbpushbutton.h>
#include <hbindicator.h>
#include <hbmessagebox.h>
#include <qvaluespacesubscriber.h>
#include "../../inc/sifuiinstallindicatorparams.h"


TestInstallIndicator::TestInstallIndicator(int& argc, char* argv[]) : HbApplication(argc, argv),
    mMainWindow(0), mMainView(0), mIndicator(0), mSubscriber(0)
{
    mMainWindow = new HbMainWindow();
    mMainView = new HbView();
    mMainView->setTitle(tr("TestInstIndi"));

    mIndicator = new HbIndicator;

    mSubscriber = new QTM_PREPEND_NAMESPACE(QValueSpaceSubscriber(KSifUiInstallIndicatorStatusPath));
    connect(mSubscriber, SIGNAL(contentsChanged()), this, SLOT(handleIndicatorActivity()));

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);

    HbTextItem *infoText = new HbTextItem;
    infoText->setText("Activate/deactivate SW install indicator in universal indicator popup.");
    infoText->setTextWrapping(Hb::TextWordWrap);
    layout->addItem(infoText);

    HbPushButton *activateButton = new HbPushButton("Activate install indicator");
    connect(activateButton, SIGNAL(clicked()), this, SLOT(activatePressed()));
    layout->addItem(activateButton);

    HbPushButton *deactivateButton = new HbPushButton("Deactivate install indicator");
    connect(deactivateButton, SIGNAL(clicked()), this, SLOT(deactivatePressed()));
    layout->addItem(deactivateButton);

    HbPushButton *closeButton = new HbPushButton("Close");
    connect(closeButton, SIGNAL(clicked()), qApp, SLOT(quit()));
    layout->addItem(closeButton);

    mMainView->setLayout(layout);
    mMainWindow->addView(mMainView);
    mMainWindow->show();
}

TestInstallIndicator::~TestInstallIndicator()
{
    delete mSubscriber;
    delete mIndicator;
    delete mMainView;
    delete mMainWindow;
}

bool TestInstallIndicator::isIndicatorActive()
{
    bool isActive = false;
    if (mSubscriber) {
        QVariant variant = mSubscriber->value();
        bool valueOk = false;
        int intValue = variant.toInt(&valueOk);
        if (valueOk && intValue) {
            isActive = true;
        }
    }
    return isActive;
}

void TestInstallIndicator::handleIndicatorActivity()
{
    if (isIndicatorActive()) {
        HbMessageBox::information("Indicator activated");
    } else {
        HbMessageBox::information("Indicator deactivated");
    }
}

void TestInstallIndicator::activatePressed()
{
    if (mIndicator) {
        if (isIndicatorActive()) {
            HbMessageBox::information("Already active");
        } else {
            QVariant parameter(QString("Application"));
            mIndicator->activate(KSifUiInstallIndicatorType, parameter);
        }
    }
}

void TestInstallIndicator::deactivatePressed()
{
    if (mIndicator) {
        if (isIndicatorActive()) {
            mIndicator->deactivate(KSifUiInstallIndicatorType);
        } else {
            HbMessageBox::information("Already deactive");
        }
    }
}

