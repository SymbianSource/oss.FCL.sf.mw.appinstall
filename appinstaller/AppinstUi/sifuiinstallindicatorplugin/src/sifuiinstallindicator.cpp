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


// ---------------------------------------------------------------------------
// SifUiInstallIndicator::SifUiInstallIndicator()
// ---------------------------------------------------------------------------
//
SifUiInstallIndicator::SifUiInstallIndicator(const QString &indicatorType) :
    HbIndicatorInterface(indicatorType, HbIndicatorInterface::GroupPriorityHigh,
            InteractionActivated), mAppName(), mPublisher(0)
{
}

// ---------------------------------------------------------------------------
// SifUiInstallIndicator::~SifUiInstallIndicator()
// ---------------------------------------------------------------------------
//
SifUiInstallIndicator::~SifUiInstallIndicator()
{
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
        case IconNameRole:
        case DecorationNameRole:
            // TODO: use proper icon when available
            data = QString("note_info.svg");
            break;
        case PrimaryTextRole:
            //: Primary text for application installation progress displayed in
            //: universal indicator menu. Secondary text is the application name.
            // TODO: use localised UI string when available
            data = tr("Installing...");
            break;
        case SecondaryTextRole:
            data = mAppName;
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
    if (parameter.isValid() && (parameter.type() == QVariant::Map)) {
        QVariantMap map = parameter.toMap();
        mAppName = map.value(KSifUiIndicatorApplicationName).toString();
    }
}

// ---------------------------------------------------------------------------
// SifUiInstallIndicator::publishActivityStatus()
// ---------------------------------------------------------------------------
//
void SifUiInstallIndicator::publishActivityStatus(bool status)
{
    if (!mPublisher) {
        mPublisher = new QTM_PREPEND_NAMESPACE(QValueSpacePublisher(KSifUiIndicatorPath));
    }
    mPublisher->setValue(KSifUiIndicatorActive, QVariant(status));
}

