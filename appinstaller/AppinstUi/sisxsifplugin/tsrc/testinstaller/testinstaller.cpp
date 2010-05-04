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
#include <xqappmgr.h>                       // XQApplicationManager
#include <usif/scr/scr.h>                   // RSoftwareComponentRegistry

using namespace Usif;

#define INSTALLS_PATH_1 "C:\\Data\\Installs\\"
#define INSTALLS_PATH_2 "E:\\Installs\\"
#define INSTALLS_PATH_3 "F:\\Installs\\"
#define INSTALLS_PATH_4 "C:\\"
#define INSTALLS_PATH_5 "E:\\"
#define INSTALLS_PATH_6 "F:\\"


TestInstaller::TestInstaller(int& argc, char* argv[]) : HbApplication(argc, argv),
    mMainWindow(0), mMainView(0), mUseSilentInstall(false),
    mInstallDirectories(0), mInstallableFiles(0), mRemovableApps(0),
    mCurrentDirPath(), mCurrentFile(), mRunner(0)
{
    mMainWindow = new HbMainWindow();
    mMainView = new HbView();
    mMainView->setTitle(tr("Test Installer"));

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);

    HbCheckBox *silentInstallCheckBox = new HbCheckBox;
    silentInstallCheckBox->setText(tr("Silent install/uninstall"));
    connect(silentInstallCheckBox, SIGNAL(stateChanged(int)),
        this, SLOT(silentCheckChanged(int)));
    layout->addItem(silentInstallCheckBox);
    layout->addStretch();

    HbLabel *installTitle = new HbLabel(tr("Install:"));
    layout->addItem(installTitle);

    mInstallDirectories = new HbComboBox;
    mInstallDirectories->setEditable(false);
    QStringList dirList;
    getInstallDirs(dirList);
    mInstallDirectories->setItems(dirList);
    connect(mInstallDirectories, SIGNAL(currentIndexChanged(int)),
        this, SLOT(installableDirChanged(int)));
    layout->addItem(mInstallDirectories);

    mInstallableFiles = new HbComboBox;
    mInstallableFiles->setEditable(false);
    connect(mInstallableFiles, SIGNAL(currentIndexChanged(int)),
            this, SLOT(installableFileChanged(int)));
    layout->addItem(mInstallableFiles);

    HbPushButton *installNew = new HbPushButton(tr("Install using new API"));
    layout->addItem(installNew);
    HbPushButton *installOld = new HbPushButton(tr("Install using old API"));
    layout->addItem(installOld);
    HbPushButton *launchApp = new HbPushButton(tr("Install by opening file"));
    layout->addItem(launchApp);
    connect(installNew, SIGNAL(clicked()), this, SLOT(installUsingNewApi()));
    connect(installOld, SIGNAL(clicked()), this, SLOT(installUsingOldApi()));
    connect(launchApp, SIGNAL(clicked()), this, SLOT(installByOpeningFile()));
    layout->addStretch();

    HbLabel *uninstallTitle = new HbLabel(tr("Uninstall:"));
    layout->addItem(uninstallTitle);
    mRemovableApps = new HbComboBox;
    mRemovableApps->setEditable(false);
    layout->addItem(mRemovableApps);

    HbPushButton *removeNew = new HbPushButton(tr("Remove using new API"));
    layout->addItem(removeNew);
    HbPushButton *removeOld = new HbPushButton(tr("Remove using old API"));
    layout->addItem(removeOld);
    connect(removeNew, SIGNAL(clicked()), this, SLOT(removeUsingNewApi()));
    connect(removeOld, SIGNAL(clicked()), this, SLOT(removeUsingOldApi()));
    layout->addStretch();

    HbPushButton *quit = new HbPushButton(tr("Exit"));
    layout->addItem(quit);

    connect(quit, SIGNAL(clicked()), this, SLOT(closeApp()));

    mMainView->setLayout(layout);
    mMainWindow->addView(mMainView);
    mMainWindow->show();

    changeDir(mInstallDirectories->currentText());
    getRemovableApps();
}

TestInstaller::~TestInstaller()
{
    delete mRunner;
    delete mMainView;
    delete mMainWindow;
}

void TestInstaller::silentCheckChanged(int state)
{
    Qt::CheckState s = static_cast<Qt::CheckState>(state);
    mUseSilentInstall = (s == Qt::Checked);
}

void TestInstaller::installableDirChanged(int /*index*/)
{
    if (mInstallDirectories) {
        changeDir(mInstallDirectories->currentText());
    }
}

void TestInstaller::installableFileChanged(int /*index*/)
{
    if (mInstallableFiles) {
        mCurrentFile = mCurrentDirPath;
        mCurrentFile.append(mInstallableFiles->currentText());
    }
}

void TestInstaller::installUsingNewApi()
{
    if (isFileSelected() && createRunner(true)) {
        doInstall(mCurrentFile);
    }
}

void TestInstaller::installUsingOldApi()
{
    if (isFileSelected() && createRunner(false)) {
        doInstall(mCurrentFile);
    }
}

void TestInstaller::installByOpeningFile()
{
    if (mInstallableFiles) {
        doOpenFile(mCurrentFile);
    }
}

void TestInstaller::removeUsingNewApi()
{
    if (isFileSelected() && createRunner(true)) {
        removeSelectedUsingNewApi();
    }
}

void TestInstaller::removeUsingOldApi()
{
    if (isFileSelected() && createRunner(false)) {
        removeSelectedUsingOldApi();
    }
}

void TestInstaller::handleComplete()
{
    HbMessageBox::information(tr("Completed"));

    delete mRunner;
    mRunner = 0;

    changeDir(mCurrentDirPath);
    getRemovableApps();
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

void TestInstaller::fileOpenOk(const QVariant &/*result*/)
{
    HbMessageBox::information(tr("Open ok"));
}

void TestInstaller::fileOpenFailed(int errorCode, const QString &errorMsg)
{
    HbMessageBox::warning(tr("Open failed: %1: %2").arg(errorCode).arg(errorMsg));
}

void TestInstaller::getInstallDirs(QStringList& dirList)
{
    QStringList possibleDirs;
    possibleDirs << INSTALLS_PATH_1 << INSTALLS_PATH_2 << INSTALLS_PATH_3
        << INSTALLS_PATH_4 << INSTALLS_PATH_5 << INSTALLS_PATH_6;

    QListIterator<QString> iter(possibleDirs);
    while (iter.hasNext()) {
        QString dirName(iter.next());
        QDir dir(dirName);
        if (dir.exists()) {
            dirList.append(dirName);
        }
    }
}

void TestInstaller::changeDir(const QString& dirPath)
{
    QDir dir(dirPath);
    if (dir.exists()) {
        mCurrentDirPath = dirPath;
        mInstallableFiles->clear();

        QFileInfoList list = dir.entryInfoList(QDir::Files);
        QListIterator<QFileInfo> iter(list);
        while (iter.hasNext()) {
            const QFileInfo &info(iter.next());
            mInstallableFiles->addItem(info.fileName());
        }

        mCurrentFile.clear();
        if (mInstallableFiles->count()) {
            mCurrentFile = mCurrentDirPath;
            mCurrentFile.append(mInstallableFiles->currentText());
        }
    }
}

void TestInstaller::getRemovableApps()
{
    TRAP_IGNORE(doGetRemovableAppsL());
}

void TestInstaller::doGetRemovableAppsL()
{
    mRemovableApps->clear();
    mRemovableComponentIds.clear();
    mRemovableUids.clear();
    mRemovableSoftwareTypes.clear();

    RSoftwareComponentRegistry registry;
    User::LeaveIfError(registry.Connect());
    CleanupClosePushL(registry);

    RArray<TComponentId> componentIdList;
    registry.GetComponentIdsL(componentIdList);
    for (int i = 0; i < componentIdList.Count(); ++i) {
        TComponentId compId = componentIdList[i];
        CComponentEntry *compEntry = CComponentEntry::NewLC();
        if (registry.GetComponentL(compId, *compEntry)) {
            if (compEntry->IsRemovable()) {
                TPtrC compName = compEntry->Name();
                QString name = QString::fromUtf16(compName.Ptr(), compName.Length());
                mRemovableApps->addItem(name);

                mRemovableComponentIds.append(compId);

                _LIT(KCompUid, "CompUid");
                CPropertyEntry *property = registry.GetComponentPropertyL(compId, KCompUid);
                CleanupStack::PushL(property);
                CIntPropertyEntry* intProperty = dynamic_cast<CIntPropertyEntry*>(property);
                mRemovableUids.append(TUid::Uid(intProperty->IntValue()));
                CleanupStack::PopAndDestroy(property);

                TPtrC softwareType = compEntry->SoftwareType();
                if (softwareType == KSoftwareTypeNative) {
                    mRemovableSoftwareTypes.append(Native);
                } else if (softwareType == KSoftwareTypeJava) {
                    mRemovableSoftwareTypes.append(Java);
                } else {
                    mRemovableSoftwareTypes.append(Unknown);
                }
            }
        }
        CleanupStack::PopAndDestroy(compEntry);
    }

    CleanupStack::PopAndDestroy(&registry);
}

bool TestInstaller::isFileSelected()
{
    if (mCurrentFile.isEmpty()) {
        HbMessageBox::warning(tr("No files selected"));
        changeDir(mCurrentDirPath);
        return false;
    }
    return true;
}

bool TestInstaller::createRunner(bool useSif)
{
    if (!mRunner) {
        mRunner = new ActiveRunner(useSif);
        connect(mRunner, SIGNAL(opCompleted()), this, SLOT(handleComplete()));
        connect(mRunner, SIGNAL(opFailed(int)), this, SLOT(handleError(int)));
    } else {
        HbMessageBox::warning(tr("Already running"));
        return false;
    }
    return true;
}

void TestInstaller::doInstall(const QString &fileName)
{
    if (mRunner) {
        mRunner->install(fileName, mUseSilentInstall);
    }
}

void TestInstaller::doOpenFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.exists()) {
        XQApplicationManager appManager;
        XQAiwRequest *request = appManager.create(file);
        if (request) {
            connect(request, SIGNAL(requestOk(const QVariant &)),
                this, SLOT(fileOpenOk(const QVariant &)));
            connect(request, SIGNAL(requestError(int, const QString &)),
                this, SLOT(fileOpenFailed(int, const QString &)));
            QList<QVariant> args;
            args << file.fileName();
            request->setArguments(args);
            if (request->send()) {
                HbMessageBox::information(tr("Opening..."));
            } else {
                HbMessageBox::warning(tr("Cannot open"));
            }
            delete request;
        } else {
            HbMessageBox::warning(tr("No handler for file '%1'").arg(fileName));
        }
    }
}

void TestInstaller::removeSelectedUsingNewApi()
{
    if (mRemovableApps && mRunner) {
        int index = mRemovableApps->currentIndex();
        const TComponentId &compId(mRemovableComponentIds.at(index));
        mRunner->remove(compId, mUseSilentInstall);
    }
}

void TestInstaller::removeSelectedUsingOldApi()
{
    if (mRemovableApps && mRunner) {
        int index = mRemovableApps->currentIndex();
        const TUid &uid(mRemovableUids.at(index));

        if (mRemovableSoftwareTypes.at(index) == Native) {
            _LIT8(KSisxMimeType, "x-epoc/x-sisx-app");
            mRunner->remove(uid, KSisxMimeType, mUseSilentInstall);
        } else if (mRemovableSoftwareTypes.at(index) == Java) {
            _LIT8(KJarMIMEType, "application/java-archive");
            mRunner->remove(uid, KJarMIMEType, mUseSilentInstall);
        } else {
            HbMessageBox::warning(tr("Not supported software type"));
            delete mRunner;
            mRunner = 0;
        }
    }
}

