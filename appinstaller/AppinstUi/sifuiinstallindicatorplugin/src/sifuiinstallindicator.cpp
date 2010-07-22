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

const char KSifUiDefaultApplicationIcon[] = "qtg_large_application";
const char KSifUiErrorIcon[] = "qtg_large_warning";
const QString KSifUiPathSeparator = "/";


// ======== LOCAL FUNCTIONS =========

// ---------------------------------------------------------------------------
// getIntValue()
// ---------------------------------------------------------------------------
//
void getIntValue(const QVariant &variant, int &value)
{
    bool ok = false;
    int temp = variant.toInt(&ok);
    if (ok) {
        value = temp;
    }
}


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// SifUiInstallIndicator::SifUiInstallIndicator()
// ---------------------------------------------------------------------------
//
SifUiInstallIndicator::SifUiInstallIndicator(const QString &indicatorType) :
    HbIndicatorInterface(indicatorType, HbIndicatorInterface::ProgressCategory,
            InteractionActivated), mAppName(), mProgress(0), mPublisher(0),
            mIsActive(false), mPhase(Installing), mIsComplete(false), mErrorCode(0)
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
            if (mIsComplete && mErrorCode) {
                data = QString(KSifUiErrorIcon);
            } else {
                // TODO: how to set application specific icon if defined?
                data = QString(KSifUiDefaultApplicationIcon);
            }
            break;

        case PrimaryTextRole:
            if (mIsComplete) {
                if (mErrorCode) {
                    //: Indicates that application installation failed.
                    // TODO: localized UI string needed
                    data = tr("Installation failed");
                } else {
                    //: Indicates that application installation is completed.
                    // TODO: localized UI string needed
                    data = tr("Installed");
                }
            } else {
                switch (mPhase) {
                    case Installing:
                        //: Indicates that application installation is ongoing.
                        // TODO: localized UI string needed
                        data = tr("Installing");
                        break;
                    case Downloading:
                        //: Indicates that download is ongoing.
                        // TODO: localized UI string needed
                        data = tr("Downloading");
                        break;
                    case CheckingCerts:
                        //: Indicates that OCSP check is ongoing.
                        // TODO: localized UI string needed
                        data = tr("Checking certificates");
                        break;
                    default:
                        break;
                }
            }
            break;

        case SecondaryTextRole:
            if (mIsComplete) {
                data = mAppName;
            } else {
                switch (mPhase) {
                    case Installing:
                    case Downloading:
                        if (!mAppName.isEmpty()) {
                            //: Application name %1 followed by installation progress %L2
                            // TODO: localized UI string needed
                            data = tr("%1 (%L2 %)").arg(mAppName).arg(mProgress);
                        }
                        break;
                    case CheckingCerts:
                    default:
                        data = mAppName;
                        break;
                }
            }
            break;

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
    if (parameter.isValid()) {
        if (parameter.type() == QVariant::String) {
            mAppName = parameter.toString();
        } else if (parameter.type() == QVariant::Int) {
            getIntValue(parameter, mProgress);
        } else if (parameter.type() == QVariant::Map) {
            QVariantMap map = parameter.toMap();
            QMapIterator<QString,QVariant> iter(map);
            while (iter.hasNext()) {
                iter.next();
                if (iter.key() == KSifUiInstallIndicatorAppName) {
                    mAppName = iter.value().toString();
                } else if (iter.key() == KSifUiInstallIndicatorPhase) {
                    int value = Installing;
                    getIntValue(iter.value(), value);
                    mPhase = static_cast<Phase>(value);
                } else if (iter.key() == KSifUiInstallIndicatorProgress) {
                    getIntValue(iter.value(), mProgress);
                } else if (iter.key() == KSifUiInstallIndicatorComplete) {
                    mIsComplete = true;
                    mErrorCode = KErrNone;
                    getIntValue(iter.value(), mErrorCode);
                } else if (iter.key() == KSifUiInstallIndicatorIcon) {
                    // TODO: icon?
                } else {
                    // ignore other types
                }
            }
        } else {
            // ignore other types
        }
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

