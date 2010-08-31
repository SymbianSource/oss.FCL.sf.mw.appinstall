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
 * Description:
 *
 */

#include <QtPlugin>
#include <QVariant>
#include "sisxsilentinstallindicatorplugin.h"
#include "sisxsilentinstallindicator.h"

Q_EXPORT_PLUGIN(SisxSilentInstallIndicatorPlugin)

// ----------------------------------------------------------------------------
// SisxSilentInstallIndicatorPlugin::SisxSilentInstallIndicatorPlugin
// @see sisxsilentinstallindicatorplugin.h
// ----------------------------------------------------------------------------
SisxSilentInstallIndicatorPlugin::SisxSilentInstallIndicatorPlugin() : mError(0)
{
   
}

// ----------------------------------------------------------------------------
// SisxSilentInstallIndicatorPlugin::~SisxSilentInstallIndicatorPlugin
// @see sisxsilentinstallindicatorplugin.h
// ----------------------------------------------------------------------------
SisxSilentInstallIndicatorPlugin::~SisxSilentInstallIndicatorPlugin()
{
}

// ----------------------------------------------------------------------------
// SisxSilentInstallIndicatorPlugin::indicatorTypes
// Return notification types this plugin implements
// @see sisxsilentinstallindicatorplugin.h
// ----------------------------------------------------------------------------
QStringList SisxSilentInstallIndicatorPlugin::indicatorTypes() const
{
  QStringList types; 
  types << "com.nokia.sisxsilentinstall.indicatorplugin/1.0";
  return types;
}

// ----------------------------------------------------------------------------
// SisxSilentInstallIndicatorPlugin::accessAllowed
// Check if client is allowed to use notification widget
// @see sisxsilentinstallindicatorplugin.h
// ----------------------------------------------------------------------------
bool SisxSilentInstallIndicatorPlugin::accessAllowed(
		const QString &indicatorType,
    const QVariantMap &securityInfo) const
{
    Q_UNUSED(indicatorType)
    Q_UNUSED(securityInfo)

    // This plugin doesn't perform operations that may compromise security.
    // All clients are allowed to use.
    return true;
}

// ----------------------------------------------------------------------------
// SisxSilentInstallIndicatorPlugin::createIndicator
// @see sisxsilentinstallindicatorplugin.h
// ----------------------------------------------------------------------------
HbIndicatorInterface* SisxSilentInstallIndicatorPlugin::createIndicator(
        const QString &indicatorType)
{
    HbIndicatorInterface *indicator = 
            new SisxSilentInstallIndicator(indicatorType);
    return indicator;
}

// ----------------------------------------------------------------------------
// SisxSilentInstallIndicatorPlugin::error
// @see sisxsilentinstallindicatorplugin.h
// ----------------------------------------------------------------------------
int SisxSilentInstallIndicatorPlugin::error() const
{
    return mError;
}

//EOF
