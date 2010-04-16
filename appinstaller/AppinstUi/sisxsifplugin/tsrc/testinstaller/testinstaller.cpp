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
* Description:  Test installer that uses Usif::RSoftwareInstall API.
*
*/

#include "testinstaller.h"
#include "activerunner.h"
#include <hbmainwindow.h>
#include <hbview.h>
#include <hbpushbutton.h>
#include <hbcheckbox.h>
#include <hbcombobox.h>
#include <hblabel.h>
#include <hbmessagebox.h>
#include <QGraphicsLinearLayout>
#include <QDir>

#define INSTALLS_PATH "C:\\Data\\Installs\\"


TestInstaller::TestInstaller(int& argc, char* argv[]) : HbApplication(argc, argv),
    mMainWindow(0), mMainView(0), mFileNames(), mSelectableFiles(0),
    mUseSilentInstall(false), mRunner(0)
{
    mMainWindow = new HbMainWindow();
    mMainView = new HbView();
    mMainView->setTitle(tr("Test Installer"));

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);

    mSelectableFiles = new HbComboBox;
    mSelectableFiles->setEditable(false);
    connect(mSelectableFiles, SIGNAL(currentIndexChanged(int)),
            this, SLOT(selectedFileChanged(int)));
    layout->addItem(mSelectableFiles);

    HbCheckBox *silentInstallCheckBox = new HbCheckBox;
    silentInstallCheckBox->setText(tr("Silent install"));
    connect(silentInstallCheckBox, SIGNAL(stateChanged(int)), this, SLOT(silentCheckChanged(int)));
    layout->addItem(silentInstallCheckBox);

    HbPushButton *installNew = new HbPushButton(tr("Install using new API"));
    layout->addItem(installNew);
    HbPushButton *installOld = new HbPushButton(tr("Install using old API"));
    layout->addItem(installOld);
    HbPushButton *quit = new HbPushButton(tr("Exit"));
    layout->addItem(quit);

    connect(installNew, SIGNAL(clicked()), this, SLOT(installUsingNewApi()));
    connect(installOld, SIGNAL(clicked()), this, SLOT(installUsingOldApi()));
    connect(quit, SIGNAL(clicked()), this, SLOT(closeApp()));

    mMainView->setLayout(layout);
    mMainWindow->addView(mMainView);
    mMainWindow->show();

    changeDir(INSTALLS_PATH);
}

TestInstaller::~TestInstaller()
{
    delete mRunner;
    delete mMainView;
    delete mMainWindow;
}

void TestInstaller::selectedFileChanged(int /*index*/)
{
    if (mSelectableFiles) {
        mFileNames.clear();
        QString selectedFile(mDirPath);
        selectedFile.append(mSelectableFiles->currentText());
        mFileNames.append(selectedFile);
    }
}

void TestInstaller::silentCheckChanged(int state)
{
    Qt::CheckState s = static_cast<Qt::CheckState>(state);
    mUseSilentInstall = (s == Qt::Checked);
}

void TestInstaller::installUsingNewApi()
{
    createRunner(true);
}

void TestInstaller::installUsingOldApi()
{
    createRunner(false);
}

void TestInstaller::handleComplete()
{
    HbMessageBox::information(tr("Installed"));

    delete mRunner;
    mRunner = 0;
}

void TestInstaller::handleError(int error)
{
    QString messageText;
    if (error == KErrCancel) {
        messageText = tr("Cancelled");
    } else {
        messageText = tr("Error %1").arg(error);
    }
    HbMessageBox::warning(messageText);

    delete mRunner;
    mRunner = 0;
}

void TestInstaller::closeApp()
{
    qApp->exit();
}

void TestInstaller::changeDir(const QString& dirPath)
{
    bool filesFound = false;

    QDir dir(dirPath);
    if (dir.exists()) {
        mDirPath = dirPath;
        mSelectableFiles->clear();

        QFileInfoList list = dir.entryInfoList(QDir::Files);
        QListIterator<QFileInfo> iter(list);
        while (iter.hasNext()) {
            const QFileInfo &info(iter.next());
            mSelectableFiles->addItem(info.fileName());
        }
        filesFound = (mSelectableFiles->count() > 0);

        mFileNames.clear();
        if (filesFound) {
            QString fileSelectedByDefault(dirPath);
            fileSelectedByDefault.append(mSelectableFiles->currentText());
            mFileNames.append(fileSelectedByDefault);
        }
    }

    if (!filesFound) {
        HbMessageBox::warning(tr("No files in '%1'").arg(dirPath));
    }
}

void TestInstaller::createRunner(bool useSif)
{
    if (!mFileNames.count()) {
        HbMessageBox::warning(tr("No files selected"));
        changeDir(INSTALLS_PATH);
    } else {
        if (!mRunner) {
            mRunner = new ActiveRunner(useSif);
            connect(mRunner, SIGNAL(opCompleted()), this, SLOT(handleComplete()));
            connect(mRunner, SIGNAL(opFailed(int)), this, SLOT(handleError(int)));
            installSelected();
        } else {
            HbMessageBox::warning(tr("Already running"));
        }
    }
}

void TestInstaller::installSelected()
{
    if (mFileNames.count()) {
        QStringListIterator fileNamesIterator(mFileNames);
        while (fileNamesIterator.hasNext()) {
            doInstall(fileNamesIterator.next());
        }
    }
}

void TestInstaller::doInstall(const QString &fileName)
{
    if (mRunner) {
        mRunner->install(fileName, mUseSilentInstall);
    }
}

