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

#include "testindiapp.h"
#include <hbmainwindow.h>
#include <hbview.h>
#include <QGraphicsLinearLayout>
#include <hbtextitem.h>
#include <hbcheckbox.h>
#include <hbpushbutton.h>
#include <hbindicator.h>

const QString KInstallIndicator = "com.nokia.sifui.indi/1.0";

TestInstallIndicator::TestInstallIndicator(int& argc, char* argv[]) : HbApplication(argc, argv),
    mMainWindow(0), mMainView(0), mIndicator(0)
{
    mMainWindow = new HbMainWindow();
    mMainView = new HbView();
    mMainView->setTitle(tr("TestInstIndi"));

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);

    HbTextItem *infoText = new HbTextItem;
    infoText->setText("Enable/disable SW install progress indicator in status indicator area and/or in universal indicator popup.");
    infoText->setTextWrapping(Hb::TextWordWrap);
    layout->addItem(infoText);

    HbCheckBox *enableCheckBox = new HbCheckBox;
    enableCheckBox->setText("Activate install indicator");
    connect(enableCheckBox, SIGNAL(stateChanged(int)), this, SLOT(activateStateChanged(int)));
    layout->addItem(enableCheckBox);

    HbPushButton *closeButton = new HbPushButton("Close");
    connect(closeButton, SIGNAL(clicked()), qApp, SLOT(quit()));
    layout->addItem(closeButton);

    mMainView->setLayout(layout);
    mMainWindow->addView(mMainView);
    mMainWindow->show();
}

TestInstallIndicator::~TestInstallIndicator()
{
    delete mIndicator;
    delete mMainView;
    delete mMainWindow;
}

void TestInstallIndicator::activateStateChanged(int state)
{
    if (!mIndicator) {
        mIndicator = new HbIndicator;
    }

    Qt::CheckState s = static_cast<Qt::CheckState>(state);
    if (s == Qt::Checked) {
        mIndicator->activate(KInstallIndicator);
    } else {
        mIndicator->deactivate(KInstallIndicator);
    }
}

