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

#include "activerunner.h"
#include "activerunner_symbian.h"


ActiveRunner::ActiveRunner(bool useSif)
{
    d_ptr = new ActiveRunnerPrivate(this);
    if (d_ptr) {
        d_ptr->Initialize(useSif);
    }
}

ActiveRunner::~ActiveRunner()
{
    delete d_ptr;
}

void ActiveRunner::install(const QString &fileName, bool silent, bool openfile, bool ocsp)
{
    if (d_ptr) {
        d_ptr->Install(fileName, silent, openfile, ocsp);
    }
}

void ActiveRunner::remove(const Usif::TComponentId& aComponentId, bool silent)
{
    if (d_ptr) {
        d_ptr->Remove(aComponentId, silent);
    }
}

void ActiveRunner::remove(const TUid& aUid, const TDesC8& aMime, bool silent)
{
    if (d_ptr) {
        d_ptr->Remove(aUid, aMime, silent);
    }
}

void ActiveRunner::handleCompletion()
{
    emit opCompleted();
}

void ActiveRunner::handleError(int error)
{
    emit opFailed(error);
}

