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

#ifndef TESTINSTALLER_H
#define TESTINSTALLER_H

#include <hbapplication.h>
#include <QStringList>
#include <usif/usifcommon.h>

class HbMainWindow;
class HbView;
class HbLabel;
class HbComboBox;
class ActiveRunner;


class TestInstaller : public HbApplication
{
    Q_OBJECT

public:     // constructor and destructor
    TestInstaller(int& argc, char* argv[]);
    ~TestInstaller();

private slots:
    void installViewActivated();
    void removeViewActivated();
    void silentInstallCheckChanged(int state);
    void silentRemoveCheckChanged(int state);
    void rfileCheckChanged(int state);
    void ocspCheckChanged(int state);
    void cancelInstallingChanged(int state);
    void cancelRemovingChanged(int state);
    void installableDirChanged(int index);
    void installableFileChanged(int index);
    void installUsingNewApi();
    void installUsingOldApi();
    void installByOpeningFile();
    void removeUsingNewApi();
    void removeUsingOldApi();
    void handleComplete();
    void handleError(int error);
    void closeApp();
    void fileOpenOk(const QVariant &result);
    void fileOpenFailed(int errorCode, const QString &errorMsg);
    void cancelInstalling();
    void cancelRemoving();

private:    // functions
    void createInstallView();
    void createRemoveView();
    void getInstallDirs(QStringList& dirList);
    void changeDir(const QString& dirPath);
    void getRemovableApps();
    void doGetRemovableAppsL();
    bool isFileSelected();
    bool createRunner(bool useSif);
    void doInstall(const QString &fileName);
    void doOpenFile(const QString &fileName);
    void removeSelectedUsingNewApi();
    void removeSelectedUsingOldApi();

private:    // data
    HbMainWindow *mMainWindow;
    HbView       *mInstallView;
    HbView       *mRemoveView;
    bool         mUseSilentInstall;
    bool         mUseSilentUninstall;
    bool         mUseRFileInstall;
    bool         mOcsp;
    bool         mCancelInstallShortly;
    bool         mCancelRemoveShortly;
    HbComboBox   *mInstallDirectories;
    HbComboBox   *mInstallableFiles;
    HbComboBox   *mRemovableApps;
    QList<Usif::TComponentId> mRemovableComponentIds;
    QList<TUid>  mRemovableUids;
    enum TSoftwareType {
        Unknown,
        Native,
        Java
    };
    QList<TSoftwareType>  mRemovableSoftwareTypes;
    QString      mCurrentDirPath;
    QString      mCurrentFile;
    ActiveRunner *mRunner;
};

#endif  // TESTINSTALLER_H
