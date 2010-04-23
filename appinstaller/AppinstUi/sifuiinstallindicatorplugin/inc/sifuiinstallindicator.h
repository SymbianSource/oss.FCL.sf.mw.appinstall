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
* Description:  Software install progress indicator
*
*/

#ifndef SIFUIINSTALLINDICATOR_H
#define SIFUIINSTALLINDICATOR_H

#include <hbindicatorinterface.h>
#include <qmobilityglobal.h>            // QTM namespace macros

QTM_BEGIN_NAMESPACE
class QValueSpacePublisher;
QTM_END_NAMESPACE


/**
 * Software install progress indicator for the status indicator area
 * and/or in universal indicator popup.
 */
class SifUiInstallIndicator : public HbIndicatorInterface
{
    Q_OBJECT

public:     // constructor and destructor
    SifUiInstallIndicator(const QString &indicatorType);
    virtual ~SifUiInstallIndicator();

public:     // from HbIndicatorInterface
    virtual bool handleInteraction(InteractionType type);
    virtual QVariant indicatorData(int role) const;

protected:  // from HbIndicatorInterface
    bool handleClientRequest(RequestType type, const QVariant &parameter);

private:    // new functions
    void processParameters(const QVariant &parameter);
    void publishActivityStatus(bool status);

private:    // data
    QString mAppName;
    QTM_PREPEND_NAMESPACE(QValueSpacePublisher) *mPublisher;
};

#endif  // SIFUIINSTALLINDICATOR_H

