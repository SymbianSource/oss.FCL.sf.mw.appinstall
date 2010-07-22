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
* Description: Software install notification plugin class.
*
*/

#include "sifuidialog.h"
#include "sifuidialogtitlewidget.h"
#include "sifuidialogcontentwidget.h"
#include "sifuiinstallindicatorparams.h"
#include <QFile>
#include <hblabel.h>
#include <hbaction.h>
#include <hbindicator.h>
#include <hbtranslator.h>
#include <hbmessagebox.h>
#include <qvaluespacesubscriber.h>
#include <xqappmgr.h>                       // XQApplicationManager

QTM_USE_NAMESPACE

const QString KTranslationsPath = "/resource/qt/translations/";
const QString KSifUiTranslationsFile = "sifuidevicedialogplugin";
const QString KCommonTranslationsFile = "common";
const QString KSwiErrorsFile = "c:\\temp\\swierrors.txt";
const QString KSwiErrorFormat = " (%1)";


// ----------------------------------------------------------------------------
// SifUiDialog::SifUiDialog()
// ----------------------------------------------------------------------------
//
SifUiDialog::SifUiDialog(const QVariantMap &parameters) : HbDialog(),
    mCommonTranslator(0), mSifUITranslator(0), mLastDialogError(KErrNone),
    mShowEventReceived(false), mDialogType(SifUiUnspecifiedDialog),
    mTitle(0), mContent(0), mPrimaryAction(0), mSecondaryAction(0),
    mResultMap(), mIndicator(0), mSubscriber(0)
{
    mCommonTranslator = new HbTranslator(KTranslationsPath, KCommonTranslationsFile);
    mSifUITranslator = new HbTranslator(KTranslationsPath, KSifUiTranslationsFile);
    constructDialog(parameters);
}

// ----------------------------------------------------------------------------
// SifUiDialog::~SifUiDialog()
// ----------------------------------------------------------------------------
//
SifUiDialog::~SifUiDialog()
{
    delete mSubscriber;
    delete mPrimaryAction;
    delete mSecondaryAction;
    delete mSifUITranslator;
    delete mCommonTranslator;
}

// ----------------------------------------------------------------------------
// SifUiDialog::dialogType()
// ----------------------------------------------------------------------------
//
SifUiDeviceDialogType SifUiDialog::dialogType(const QVariantMap &parameters)
{
    SifUiDeviceDialogType type = SifUiUnspecifiedDialog;
    if (parameters.contains(KSifUiDialogType)) {
        type = static_cast<SifUiDeviceDialogType>(parameters.value(KSifUiDialogType).toInt());
        Q_ASSERT(type == SifUiConfirmationQuery || type == SifUiProgressNote ||
            type == SifUiCompleteNote || type == SifUiErrorNote);
    }
    return type;
}

// ----------------------------------------------------------------------------
// SifUiDialog::setDeviceDialogParameters()
// ----------------------------------------------------------------------------
//
bool SifUiDialog::setDeviceDialogParameters(const QVariantMap &parameters)
{
    setVisible(!isInstallIndicatorActive());
    return updateFromParameters(parameters);
}

// ----------------------------------------------------------------------------
// SifUiDialog::deviceDialogError()
// ----------------------------------------------------------------------------
//
int SifUiDialog::deviceDialogError() const
{
    return mLastDialogError;
}

// ----------------------------------------------------------------------------
// SifUiDialog::closeDeviceDialog()
// ----------------------------------------------------------------------------
//
void SifUiDialog::closeDeviceDialog(bool byClient)
{
    Q_UNUSED(byClient);
    close();

    // If show event has been received, close is signalled from hide event.
    // If not, hide event does not come and close is signalled from here.
    if (!mShowEventReceived) {
        emit deviceDialogClosed();
    }
}

// ----------------------------------------------------------------------------
// SifUiDialog::deviceDialogWidget()
// ----------------------------------------------------------------------------
//
HbDialog *SifUiDialog::deviceDialogWidget() const
{
    return const_cast<SifUiDialog*>(this);
}

// ----------------------------------------------------------------------------
// SifUiDialog::hideEvent()
// ----------------------------------------------------------------------------
//
void SifUiDialog::hideEvent(QHideEvent *event)
{
    HbDialog::hideEvent(event);
    emit deviceDialogClosed();
}

// ----------------------------------------------------------------------------
// SifUiDialog::showEvent()
// ----------------------------------------------------------------------------
//
void SifUiDialog::showEvent(QShowEvent *event)
{
    HbDialog::showEvent(event);
    mShowEventReceived = true;
}

// ----------------------------------------------------------------------------
// SifUiDialog::isInstallIndicatorActive()
// ----------------------------------------------------------------------------
//
bool SifUiDialog::isInstallIndicatorActive()
{
    bool isActive = false;
    if (!mSubscriber) {
        mSubscriber = new QTM_PREPEND_NAMESPACE(QValueSpaceSubscriber(
            KSifUiInstallIndicatorStatusPath));
    }
    QVariant variant = mSubscriber->value();
    bool valueOk = false;
    int intValue = variant.toInt(&valueOk);
    if (valueOk && intValue) {
        isActive = true;
    }
    return isActive;
}

// ----------------------------------------------------------------------------
// SifUiDialog::constructDialog()
// ----------------------------------------------------------------------------
//
bool SifUiDialog::constructDialog(const QVariantMap &parameters)
    {
    setTimeout(HbPopup::NoTimeout);
    setDismissPolicy(HbPopup::NoDismiss);
    setModal(true);

    mDialogType = dialogType(parameters);

    Q_ASSERT(mTitle == 0);
    mTitle = new SifUiDialogTitleWidget(this);
    mTitle->constructFromParameters(parameters);
    setHeadingWidget(mTitle);

    Q_ASSERT(mContent == 0);
    mContent = new SifUiDialogContentWidget(this);
    connect(mContent, SIGNAL(memorySelectionChanged(const QChar &)),
            this, SLOT(handleMemorySelectionChanged(const QChar &)));
    mContent->constructFromParameters(parameters);
    setContentWidget(mContent);

    updateButtons(parameters);

    return true;
}

// ----------------------------------------------------------------------------
// SifUiDialog::updateFromParameters()
// ----------------------------------------------------------------------------
//
bool SifUiDialog::updateFromParameters(const QVariantMap &parameters)
{
    SifUiDeviceDialogType prevDialogType = mDialogType;
    mDialogType = dialogType(parameters);
    if (mTitle)
        {
        mTitle->updateFromParameters(parameters);
        }
    if (mContent)
        {
        mContent->updateFromParameters(parameters);
        }
    if (prevDialogType != mDialogType) {
        updateButtons(parameters);
    }
    if (parameters.contains(KSifUiErrorCode)) {
        bool ok = false;
        int errorCode = parameters.value(KSifUiErrorCode).toInt(&ok);
        if (ok) {
            mInstallError = errorCode;
        }
    }
    return true;
}

// ----------------------------------------------------------------------------
// SifUiDialog::updateButtons()
// ----------------------------------------------------------------------------
//
void SifUiDialog::updateButtons(const QVariantMap &parameters)
{
    if (mPrimaryAction) {
        removeAction(mPrimaryAction);
        delete mPrimaryAction;
        mPrimaryAction = 0;
    }
    switch (mDialogType) {
        case SifUiConfirmationQuery:
            //: Accepts the SW install confirmation query and starts installation.
            mPrimaryAction = new HbAction(hbTrId("txt_common_button_ok"));
            addAction(mPrimaryAction);
            disconnect(mPrimaryAction, SIGNAL(triggered()), this, SLOT(close()));
            connect(mPrimaryAction, SIGNAL(triggered()), this, SLOT(handleAccepted()));
            break;
        case SifUiProgressNote:
            if (!parameters.contains(KSifUiProgressNoteIsHideButtonHidden)) {
                //: Hides the progress dialog. Progress note moves into universal indicator.
                mPrimaryAction = new HbAction(hbTrId("txt_common_button_hide"));
                addAction(mPrimaryAction);
                disconnect(mPrimaryAction, SIGNAL(triggered()), this, SLOT(close()));
                connect(mPrimaryAction, SIGNAL(triggered()), this, SLOT(handleHidePressed()));
            }
            break;
        case SifUiCompleteNote:
            if (!parameters.contains(KSifUiCompleteNoteIsShowButtonHidden)) {
                //: Opens Application Library to view the installed application.
                mPrimaryAction = new HbAction(hbTrId("txt_installer_button_show"));
                addAction(mPrimaryAction);
                disconnect(mPrimaryAction, SIGNAL(triggered()), this, SLOT(close()));
                connect(mPrimaryAction, SIGNAL(triggered()), this, SLOT(handleShowInstalled()));
            }
            break;
        case SifUiErrorNote:
            if (!parameters.contains(KSifUiErrorNoteIsDetailsButtonHidden)) {
                //: Shows a dialog with further info about the failure (i.e. why installation failed).
                mPrimaryAction = new HbAction(hbTrId("txt_installer_button_details"));
                addAction(mPrimaryAction);
                disconnect(mPrimaryAction, SIGNAL(triggered()), this, SLOT(close()));
                connect(mPrimaryAction, SIGNAL(triggered()), this, SLOT(handleErrorDetails()));
            }
            break;
        default:
            break;
    }

    if (mSecondaryAction) {
        removeAction(mSecondaryAction);
        delete mSecondaryAction;
        mSecondaryAction = 0;
    }
    switch (mDialogType) {
        case SifUiConfirmationQuery:
        case SifUiProgressNote:
            if (!parameters.contains(KSifUiProgressNoteIsCancelButtonHidden)) {
                //: Cancels the SW install confirmation query and closes the dialog.
                mSecondaryAction = new HbAction(hbTrId("txt_common_button_cancel"));
                addAction(mSecondaryAction);
                disconnect(mSecondaryAction, SIGNAL(triggered()), this, SLOT(close()));
                connect(mSecondaryAction, SIGNAL(triggered()), this, SLOT(handleCancelled()));
            }
            break;
        case SifUiCompleteNote:
        case SifUiErrorNote:
            //: Closes the dialog. Control returns back to where the installation was started.
            mSecondaryAction = new HbAction(hbTrId("txt_common_button_close"));
            addAction(mSecondaryAction);
            break;
        default:
            break;
    }
}

// ----------------------------------------------------------------------------
// SifUiDialog::sendResult()
// ----------------------------------------------------------------------------
//
void SifUiDialog::sendResult(SifUiDeviceDialogReturnValue value)
{
    mResultMap[KSifUiQueryReturnValue] = QVariant(value);
    emit deviceDialogData(mResultMap);
}

// ----------------------------------------------------------------------------
// SifUiDialog::handleAccepted()
// ----------------------------------------------------------------------------
//
void SifUiDialog::handleAccepted()
{
    mContent->changeType(SifUiProgressNote);
    sendResult(SifUiContinue);
}

// ----------------------------------------------------------------------------
// SifUiDialog::handleCancelled()
// ----------------------------------------------------------------------------
//
void SifUiDialog::handleCancelled()
{
    sendResult(SifUiCancel);
    close();
}

// ----------------------------------------------------------------------------
// SifUiDialog::handleMemorySelectionChanged()
// ----------------------------------------------------------------------------
//
void SifUiDialog::handleMemorySelectionChanged(const QChar &driveLetter)
{
    mResultMap[KSifUiSelectedMemory] = QVariant(driveLetter);
    emit deviceDialogData(mResultMap);
}

// ----------------------------------------------------------------------------
// SifUiDialog::handleHidePressed()
// ----------------------------------------------------------------------------
//
void SifUiDialog::handleHidePressed()
{
    sendResult(SifUiIndicator);
    hide();
}

// ----------------------------------------------------------------------------
// SifUiDialog::handleShowInstalled()
// ----------------------------------------------------------------------------
//
void SifUiDialog::handleShowInstalled()
{
    QUrl openRecentView("appto://20022F35?activityname=AppLibRecentView");

    XQApplicationManager applicationManager;
    XQAiwRequest *request = applicationManager.create(openRecentView);
    if (request) {
        bool result = request->send();
        if (result) {
            closeDeviceDialog(false);
        } else {
            // TODO: proper error handling
            int error = request->lastError();
            QString messageText = tr("Unable to open AppLib. Error %1").arg(error);
            HbMessageBox::warning(messageText);
        }
        delete request;
    }

    close();
}

// ----------------------------------------------------------------------------
// SifUiDialog::handleErrorDetails()
// ----------------------------------------------------------------------------
//
void SifUiDialog::handleErrorDetails()
{
    // TODO: show proper error details dialog
    QString messageText;
    messageText = tr("Error code %1").arg(mInstallError);

    if (QFile::exists(KSwiErrorsFile)) {
        messageText.append(KSwiErrorFormat.arg(mInstallError));
    }
    HbMessageBox::warning(messageText);
}

