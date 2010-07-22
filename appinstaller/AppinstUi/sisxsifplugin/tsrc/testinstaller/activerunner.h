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

#ifndef ACTIVERUNNER_H
#define ACTIVERUNNER_H

#include <QObject>
#include <usif/usifcommon.h>

class ActiveRunnerPrivate;


class ActiveRunner : public QObject
{
    Q_OBJECT

public:     // constructor and destructor
    ActiveRunner(bool useSif);
    ~ActiveRunner();

signals:
    void opCompleted();
    void opFailed(int error);

public:     // new functions
    void install(const QString &fileName, bool silent, bool openfile, bool ocsp);
    void remove(const Usif::TComponentId& aComponentId, bool silent);
    void remove(const TUid& aUid, const TDesC8& aMime, bool silent);

protected:
    void handleCompletion();
    void handleError(int error);

private:    // data
    friend class ActiveRunnerPrivate;
    ActiveRunnerPrivate *d_ptr;
};

#endif  // ACTIVERUNNER_H
