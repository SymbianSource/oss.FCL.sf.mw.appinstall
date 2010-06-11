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

#ifndef SIFUIINSTALLINDICATORPLUGIN_H
#define SIFUIINSTALLINDICATORPLUGIN_H

#include <QObject>
#include <hbindicatorplugininterface.h>


/**
 * Software install progress indicator plugin for the status indicator area
 * and/or in universal indicator popup.
 */
class SifUiInstallIndicatorPlugin : public QObject, public HbIndicatorPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(HbIndicatorPluginInterface)

public:     // constructor and destructor
     SifUiInstallIndicatorPlugin();
    ~SifUiInstallIndicatorPlugin();

public:     // from HbIndicatorPluginInterface
    QStringList indicatorTypes() const;
    bool accessAllowed(const QString &indicatorType, const QVariantMap &securityInfo) const;
    HbIndicatorInterface* createIndicator(const QString &indicatorType);
    int error() const;

private:    // new functions
    Q_DISABLE_COPY(SifUiInstallIndicatorPlugin)

private:    // data
    int mError;
};

#endif  // SIFUIINSTALLINDICATORPLUGIN_H

