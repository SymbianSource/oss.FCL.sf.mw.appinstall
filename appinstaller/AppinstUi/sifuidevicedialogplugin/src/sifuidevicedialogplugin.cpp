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

#include "sifuidevicedialogplugin.h"
#include "sifuidialogdefinitions.h"
#include "sifuidialog.h"

// This plugin implements one device dialog, SIF UI device dialog.
static const char* KSifUiDeviceDialog = "com.nokia.sifui/1.0";
static const struct {
    const char *mTypeString;
} dialogInfos[] = {
    {KSifUiDeviceDialog}
};


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// SifUiDeviceDialogPlugin::SifUiDeviceDialogPlugin()
// ----------------------------------------------------------------------------
//
SifUiDeviceDialogPlugin::SifUiDeviceDialogPlugin() : mError(KErrNone)
{
}

// ----------------------------------------------------------------------------
// SifUiDeviceDialogPlugin::~SifUiDeviceDialogPlugin()
// ----------------------------------------------------------------------------
//
SifUiDeviceDialogPlugin::~SifUiDeviceDialogPlugin()
{
}

// ----------------------------------------------------------------------------
// SifUiDeviceDialogPlugin::accessAllowed()
// ----------------------------------------------------------------------------
//
bool SifUiDeviceDialogPlugin::accessAllowed(const QString &deviceDialogType,
    const QVariantMap &parameters, const QVariantMap &securityInfo) const
{
    Q_UNUSED(deviceDialogType)
    Q_UNUSED(parameters)
    Q_UNUSED(securityInfo)

    // All clients are allowed to use.
    // TODO: should access be limited to certain clients?
    return true;
}

// ----------------------------------------------------------------------------
// SifUiDeviceDialogPlugin::createDeviceDialog()
// ----------------------------------------------------------------------------
//
HbDeviceDialogInterface *SifUiDeviceDialogPlugin::createDeviceDialog(
    const QString &deviceDialogType, const QVariantMap &parameters)
{
    //  Create device dialog widget
    Q_UNUSED(deviceDialogType)

    SifUiDialog *deviceDialog = new SifUiDialog(parameters);
    mError = deviceDialog->deviceDialogError();
    if (mError != KErrNone) {
        delete deviceDialog;
        deviceDialog = 0;
    }

    return deviceDialog;
}

// ----------------------------------------------------------------------------
// SifUiDeviceDialogPlugin::deviceDialogInfo()
// ----------------------------------------------------------------------------
//
bool SifUiDeviceDialogPlugin::deviceDialogInfo( const QString &deviceDialogType,
        const QVariantMap &parameters, DeviceDialogInfo *info) const
{
    // Return device dialog flags
    Q_UNUSED(deviceDialogType);
    Q_UNUSED(parameters);

    info->group = GenericDeviceDialogGroup;
    info->flags = NoDeviceDialogFlags;
    info->priority = DefaultPriority;

    return true;
}

// ----------------------------------------------------------------------------
// SifUiDeviceDialogPlugin::deviceDialogTypes()
// ----------------------------------------------------------------------------
//
QStringList SifUiDeviceDialogPlugin::deviceDialogTypes() const
{
    // Return device dialog types this plugin implements

    QStringList types;
    const int numTypes = sizeof(dialogInfos) / sizeof(dialogInfos[0]);
    for(int i = 0; i < numTypes; ++i) {
        types.append(dialogInfos[i].mTypeString);
    }

    return types;
}

// ----------------------------------------------------------------------------
// SifUiDeviceDialogPlugin::pluginFlags()
// ----------------------------------------------------------------------------
//
HbDeviceDialogPlugin::PluginFlags SifUiDeviceDialogPlugin::pluginFlags() const
{
    // Return plugin flags
    return NoPluginFlags;
}

// ----------------------------------------------------------------------------
// SifUiDeviceDialogPlugin::error()
// ----------------------------------------------------------------------------
//
int SifUiDeviceDialogPlugin::error() const
{
    // Return last error
    return mError;
}

Q_EXPORT_PLUGIN2(SifUiDeviceDialogPlugin,SifUiDeviceDialogPlugin)

