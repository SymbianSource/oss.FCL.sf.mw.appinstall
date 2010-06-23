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
* Description: Software install framework (SIF) device dialog plugin.
*
*/

#ifndef SIFUIDEVICEDIALOGPLUGIN_H
#define SIFUIDEVICEDIALOGPLUGIN_H

#include <hbdevicedialogplugin.h>       // HbDeviceDialogPlugin


/**
 * SIF UI device dialog plugin.
 * Implements device dialog plugin for UI dialogs that can be used
 * in software install framwork (SIF) plugins.
 */
class SifUiDeviceDialogPlugin : public HbDeviceDialogPlugin
{
    Q_OBJECT

public:
    SifUiDeviceDialogPlugin();
    ~SifUiDeviceDialogPlugin();

public:     // from HbDeviceDialogPlugin
    bool accessAllowed(const QString &deviceDialogType, const QVariantMap &parameters,
            const QVariantMap &securityInfo) const;
    HbDeviceDialogInterface *createDeviceDialog(const QString &deviceDialogType,
            const QVariantMap &parameters);
    bool deviceDialogInfo(const QString &deviceDialogType, const QVariantMap &parameters,
            DeviceDialogInfo *info) const;
    QStringList deviceDialogTypes() const;
    PluginFlags pluginFlags() const;
    int error() const;

private:
    Q_DISABLE_COPY(SifUiDeviceDialogPlugin)

    int mError;
};

#endif // SIFUIDEVICEDIALOGPLUGIN_H
