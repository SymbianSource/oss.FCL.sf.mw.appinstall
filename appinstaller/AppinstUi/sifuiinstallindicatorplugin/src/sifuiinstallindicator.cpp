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

#include "sifuiinstallindicator.h"
#include "sifuiinstallindicatorparams.h"
#include <QVariant>
#include <qvaluespacepublisher.h>

const char KSifUiDefaultApplicationIcon[] = "qtg_large_application.svg";
const QString KSifUiPathSeparator = "/";


// ---------------------------------------------------------------------------
// SifUiInstallIndicator::SifUiInstallIndicator()
// ---------------------------------------------------------------------------
//
SifUiInstallIndicator::SifUiInstallIndicator(const QString &indicatorType) :
    HbIndicatorInterface(indicatorType, HbIndicatorInterface::ProgressCategory,
            InteractionActivated), mAppName(), mPublisher(0), mIsActive(false)
{
    mPublisher = new QTM_PREPEND_NAMESPACE(QValueSpacePublisher(KSifUiInstallIndicatorPath));
}

// ---------------------------------------------------------------------------
// SifUiInstallIndicator::~SifUiInstallIndicator()
// ---------------------------------------------------------------------------
//
SifUiInstallIndicator::~SifUiInstallIndicator()
{
    if (mPublisher && mIsActive) {
        publishActivityStatus(false);
    }
    delete mPublisher;
}

// ---------------------------------------------------------------------------
// SifUiInstallIndicator::handleInteraction()
// ---------------------------------------------------------------------------
//
bool SifUiInstallIndicator::handleInteraction(InteractionType type)
{
    bool handled = false;

    if (type == InteractionActivated) {
        emit deactivate();
        publishActivityStatus(false);
        handled = true;
    }

    return handled;
}

// ---------------------------------------------------------------------------
// SifUiInstallIndicator::indicatorData()
// ---------------------------------------------------------------------------
//
QVariant SifUiInstallIndicator::indicatorData(int role) const
{
    QVariant data;

    switch(role) {
        case DecorationNameRole:
            data = QString(KSifUiDefaultApplicationIcon);
            break;
        case PrimaryTextRole:
            //: Primary text for application installation progress displayed in
            //: universal indicator menu. Secondary text is the application name.
            // TODO: use localised UI string when available
            data = tr("Installing");
            // TODO: text must indicate installation phase, need to support also
            // tr("Downloading") and tr("Doing OCSP checks")
            break;
        case SecondaryTextRole:
            if (!mAppName.isEmpty()) {
                data = tr("%1 (%L2 %)").arg(mAppName).arg(mProgress);
            }
        default:
            break;
    }

    return data;
}

// ---------------------------------------------------------------------------
// SifUiInstallIndicator::prepareDisplayName()
// ---------------------------------------------------------------------------
//
bool SifUiInstallIndicator::handleClientRequest(RequestType type, const QVariant &parameter)
{
    bool indicatorDisplayed = false;

    switch (type) {
        case RequestActivate:
            processParameters(parameter);
            indicatorDisplayed = true;
            emit dataChanged();
            break;
        case RequestDeactivate:
            break;
        default:
            break;
    }

    publishActivityStatus(indicatorDisplayed);
    return indicatorDisplayed;
}

// ---------------------------------------------------------------------------
// SifUiInstallIndicator::processParameters()
// ---------------------------------------------------------------------------
//
void SifUiInstallIndicator::processParameters(const QVariant &parameter)
{
    if (parameter.isValid() && (parameter.type() == QVariant::String)) {
        mAppName = parameter.toString();
        mProgress = 0;

        // TODO: get icon if standard icon needs to be replaced
        // TODO: start listening USIF installation progress
    }
}

// ---------------------------------------------------------------------------
// SifUiInstallIndicator::publishActivityStatus()
// ---------------------------------------------------------------------------
//
void SifUiInstallIndicator::publishActivityStatus(bool status)
{
    if (status != mIsActive) {
        if (mPublisher) {
            int intStatus = status;
            mPublisher->setValue(KSifUiInstallIndicatorStatus, QVariant(intStatus));
            mPublisher->sync();
        }
        mIsActive = status;
    }
}

