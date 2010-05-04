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
* Description: Device dialog plugin for SIF UI dialogs.
*
*/

#ifndef SIFUIDIALOG_H
#define SIFUIDIALOG_H

#include <hbdialog.h>                   // HbDialog
#include <hbdevicedialoginterface.h>    // HbDeviceDialogInterface
#include "sifuidialogdefinitions.h"     // enum SifUiDeviceDialogType
#include <qmobilityglobal.h>            // QTM namespace macros

class QTranslator;
class SifUiDialogTitleWidget;
class SifUiDialogContentWidget;
class HbIndicator;

QTM_BEGIN_NAMESPACE
class QValueSpaceSubscriber;
QTM_END_NAMESPACE


/**
 * SW Install notification widget class.
 */
class SifUiDialog : public HbDialog, public HbDeviceDialogInterface
{
    Q_OBJECT

public:     // constructor and destructor
    SifUiDialog(const QVariantMap &parameters);
    virtual ~SifUiDialog();

public:     // new functions
    static SifUiDeviceDialogType dialogType(const QVariantMap &parameters);

public:     // from HbDeviceDialogInterface
    bool setDeviceDialogParameters(const QVariantMap &parameters);
    int deviceDialogError() const;
    void closeDeviceDialog(bool byClient);
    HbDialog *deviceDialogWidget() const;

protected:  // from HbDialog
    void closeEvent(QCloseEvent *event);

signals:
    void deviceDialogClosed();
    void deviceDialogData(const QVariantMap &data);

protected:  // from HbPopup (via HbDialog)
    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);

private:    // new functions
    void installTranslator();
    void removeTranslator();
    bool constructDialog(const QVariantMap &parameters);
    bool updateFromParameters(const QVariantMap &parameters);
    void updateButtons();
    void sendResult(bool accepted);
    void monitorIndicatorActivity();

private slots:
    void handleAccepted();
    void handleCancelled();
    void handleMemorySelectionChanged(const QChar &driveLetter);
    void handleDisplayCertificateDetails();
    void handleHidePressed();
    void handleIndicatorActivityChanged();
    void handleShowInstalled();
    void handleErrorDetails();

private:
    Q_DISABLE_COPY(SifUiDialog)

    QTranslator *mTranslator;
    int mLastDialogError;
    bool mShowEventReceived;
    SifUiDeviceDialogType mDialogType;
    SifUiDialogTitleWidget *mTitle;
    SifUiDialogContentWidget *mContent;
    int mInstallError;
    QVariantMap mResultMap;
    HbAction *mIgnoreCloseAction;
    HbIndicator *mIndicator;

    QTM_PREPEND_NAMESPACE(QValueSpaceSubscriber) *mSubscriber;
};

#endif // SIFUIDIALOG_H
