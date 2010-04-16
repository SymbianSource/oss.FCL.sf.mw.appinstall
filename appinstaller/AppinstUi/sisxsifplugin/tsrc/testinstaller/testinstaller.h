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
    void selectedFileChanged(int index);
    void silentCheckChanged(int state);
    void installUsingNewApi();
    void installUsingOldApi();
    void handleComplete();
    void handleError(int error);
    void closeApp();

private:    // functions
    void changeDir(const QString& dirPath);
    void createRunner(bool useSif);
    void installSelected();
    void doInstall(const QString &fileName);

private:    // data
    HbMainWindow *mMainWindow;
    HbView       *mMainView;
    QStringList  mFileNames;
    HbComboBox   *mSelectableFiles;
    bool         mUseSilentInstall;
    QString      mDirPath;
    ActiveRunner *mRunner;
};

#endif  // TESTINSTALLER_H
