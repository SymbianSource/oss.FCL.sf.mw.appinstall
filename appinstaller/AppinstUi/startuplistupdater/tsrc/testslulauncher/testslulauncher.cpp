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

#include "testslulauncher.h"
#include <hbmainwindow.h>
#include <hbview.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbmessagebox.h>
#include <QGraphicsLinearLayout>
#ifdef Q_OS_SYMBIAN
#include <e32std.h>
#endif  // Q_OS_SYMBIAN


// ======== LOCAL FUNCTIONS ========

#ifdef Q_OS_SYMBIAN
_LIT( KStartupListUpdater, "z:\\sys\\bin\\startuplistupdater.exe" );

// ---------------------------------------------------------------------------
// DoLaunchStartupListUpdater()
// ---------------------------------------------------------------------------
//
TInt DoLaunchStartupListUpdater()
{
    RProcess process;
    TInt result = process.Create( KStartupListUpdater, KNullDesC );

    if (result == KErrNone) {
        TRequestStatus rendezvousStatus;
        process.Rendezvous(rendezvousStatus);

        // start process and wait until it is started
        process.Resume();
        User::WaitForRequest(rendezvousStatus);

        if (rendezvousStatus.Int() == KErrNone) {
            TRequestStatus logonStatus;
            process.Logon(logonStatus);

            // waits until process is finished
            User::WaitForRequest(logonStatus);

            result = logonStatus.Int();
        } else {
            result = rendezvousStatus.Int();
        }

        process.Close();
    }

    return result;
}
#endif  // Q_OS_SYMBIAN


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TestStartupListUpdateLauncher::TestStartupListUpdateLauncher
// ---------------------------------------------------------------------------
//
TestStartupListUpdateLauncher::TestStartupListUpdateLauncher(int& argc, char* argv[])
        : HbApplication(argc, argv), mMainWindow(0), mMainView(0)
{
    mMainWindow = new HbMainWindow();
    mMainView = new HbView();
    mMainView->setTitle(tr("Test SLU Launcher"));

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);

    HbPushButton *updateStartupList = new HbPushButton(tr("Update Startup List"));
    layout->addItem(updateStartupList);
    connect(updateStartupList, SIGNAL(clicked()), this, SLOT(updateStartupList()));

    HbPushButton *exitApp = new HbPushButton(tr("Exit"));
    layout->addItem(exitApp);
    connect(exitApp, SIGNAL(clicked()), this, SLOT(closeApp()));

    mMainView->setLayout(layout);
    mMainWindow->addView(mMainView);
    mMainWindow->show();
}

// ---------------------------------------------------------------------------
// TestStartupListUpdateLauncher::~TestStartupListUpdateLauncher
// ---------------------------------------------------------------------------
//
TestStartupListUpdateLauncher::~TestStartupListUpdateLauncher()
{
    delete mMainView;
    delete mMainWindow;
}

// ---------------------------------------------------------------------------
// TestStartupListUpdateLauncher::updateStartupList()
// ---------------------------------------------------------------------------
//
void TestStartupListUpdateLauncher::updateStartupList()
{
#ifdef Q_OS_SYMBIAN
    int err = DoLaunchStartupListUpdater();
    if( !err ) {
        HbMessageBox::information(tr("Ok"));
    } else {
        HbMessageBox::information(tr("Error %L1").arg(err));
    }
#endif  // Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
// TestStartupListUpdateLauncher::closeApp()
// ---------------------------------------------------------------------------
//
void TestStartupListUpdateLauncher::closeApp()
{
    qApp->exit();
}

