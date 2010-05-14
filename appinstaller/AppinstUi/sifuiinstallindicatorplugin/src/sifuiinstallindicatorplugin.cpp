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

#include "sifuiinstallindicatorplugin.h"
#include "sifuiinstallindicator.h"
#include "sifuiinstallindicatorparams.h"

Q_EXPORT_PLUGIN(SifUiInstallIndicatorPlugin)


// ---------------------------------------------------------------------------
// SifUiInstallIndicatorPlugin::SifUiInstallIndicatorPlugin
// ---------------------------------------------------------------------------
//
SifUiInstallIndicatorPlugin::SifUiInstallIndicatorPlugin() : mError(0)
{
}

// ---------------------------------------------------------------------------
// SifUiInstallIndicatorPlugin::~SifUiInstallIndicatorPlugin
// ---------------------------------------------------------------------------
//
SifUiInstallIndicatorPlugin::~SifUiInstallIndicatorPlugin()
{
}

// ---------------------------------------------------------------------------
// SifUiInstallIndicatorPlugin::indicatorTypes
// ---------------------------------------------------------------------------
//
QStringList SifUiInstallIndicatorPlugin::indicatorTypes() const
{
    QStringList types;
    types << KSifUiInstallIndicatorType;
    return types;
}

// ---------------------------------------------------------------------------
// SifUiInstallIndicatorPlugin::accessAllowed
// ---------------------------------------------------------------------------
//
bool SifUiInstallIndicatorPlugin::accessAllowed(const QString &indicatorType,
    const QVariantMap &securityInfo) const
{
    Q_UNUSED(indicatorType)
    Q_UNUSED(securityInfo)

    return true;    // All clients are allowed to use.
}

// ---------------------------------------------------------------------------
// SifUiInstallIndicatorPlugin::createIndicator
// ---------------------------------------------------------------------------
//
HbIndicatorInterface* SifUiInstallIndicatorPlugin::createIndicator(
        const QString &indicatorType)
{
    HbIndicatorInterface *indicator = new SifUiInstallIndicator(indicatorType);
    return indicator;
}

// ---------------------------------------------------------------------------
// SifUiInstallIndicatorPlugin::error
// ---------------------------------------------------------------------------
//
int SifUiInstallIndicatorPlugin::error() const
{
    return mError;
}

