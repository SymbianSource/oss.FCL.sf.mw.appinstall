/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "bgcindicatorplugin.h"

#include "bgcindicator.h"
#include <QtPlugin>
#include <QVariant>

Q_EXPORT_PLUGIN(BgcIndicatorPlugin)

// ----------------------------------------------------------------------------
// BgcIndicatorPlugin::BgcIndicatorPlugin
// @see bgcindicatorplugin.h
// ----------------------------------------------------------------------------
BgcIndicatorPlugin::BgcIndicatorPlugin() : mError(0)
{
   
}

// ----------------------------------------------------------------------------
// BgcIndicatorPlugin::~BgcIndicatorPlugin
// @see bgcindicatorplugin.h
// ----------------------------------------------------------------------------
BgcIndicatorPlugin::~BgcIndicatorPlugin()
{
}

// ----------------------------------------------------------------------------
// BgcIndicatorPlugin::indicatorTypes
// Return notification types this plugin implements
// @see bgcindicatorplugin.h
// ----------------------------------------------------------------------------
QStringList BgcIndicatorPlugin::indicatorTypes() const
{
  QStringList types; 
  types << "com.nokia.iaupdate.indicatorplugin/1.0";
  return types;
}

// ----------------------------------------------------------------------------
// BgcIndicatorPlugin::accessAllowed
// Check if client is allowed to use notification widget
// @see bgcindicatorplugin.h
// ----------------------------------------------------------------------------
bool BgcIndicatorPlugin::accessAllowed(const QString &indicatorType,
        const QVariantMap &securityInfo) const
{
    Q_UNUSED(indicatorType)
    Q_UNUSED(securityInfo)

    // This plugin doesn't perform operations that may compromise security.
    // All clients are allowed to use.
    return true;
}

// ----------------------------------------------------------------------------
// BgcIndicatorPlugin::createIndicator
// @see bgcindicatorplugin.h
// ----------------------------------------------------------------------------
HbIndicatorInterface* BgcIndicatorPlugin::createIndicator(
        const QString &indicatorType)
{
    HbIndicatorInterface *indicator = new BgcIndicator(indicatorType);

    return indicator;
}

// ----------------------------------------------------------------------------
// BgcIndicatorPlugin::error
// @see bgcindicatorplugin.h
// ----------------------------------------------------------------------------
int BgcIndicatorPlugin::error() const
{
    return mError;
}




