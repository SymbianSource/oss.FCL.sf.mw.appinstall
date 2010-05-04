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
#include <hblabel.h>
#include <hbaction.h>
#include <hbindicator.h>
#include <QTranslator>
#include <QApplication>                     // qApp()
#include <QFile>
#include <hbmessagebox.h>
#include <qvaluespacesubscriber.h>

const QString KTranslationsPath = "resource/qt/translations/";
const QString KTranslationsFile = "sifuidevicedialogplugin";
const QString KSwiErrorsFile = "c:\\temp\\swierrors.txt";
const QString KSwiErrorFormat = " (%1)";


// ----------------------------------------------------------------------------
// SifUiDialog::SifUiDialog()
// ----------------------------------------------------------------------------
//
SifUiDialog::SifUiDialog(const QVariantMap &parameters) : HbDialog(),
    mLastDialogError(KErrNone), mShowEventReceived(false),
    mDialogType(SifUiUnspecifiedDialog),
    mTitle(0), mContent(0), mResultMap(),
    mIgnoreCloseAction(0), mIndicator(0), mSubscriber(0)
{
    // TODO: enable when translations ready
    //installTranslator();
    constructDialog(parameters);
}

// ----------------------------------------------------------------------------
// SifUiDialog::~SifUiDialog()
// ----------------------------------------------------------------------------
//
SifUiDialog::~SifUiDialog()
{
    delete mSubscriber;

    // TODO: enable when translations ready
    //removeTranslator();
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
// SifUiDialog::closeEvent()
// ----------------------------------------------------------------------------
//
void SifUiDialog::closeEvent(QCloseEvent *event)
{
    if (mIgnoreCloseAction) {
        HbAction *closingAction = qobject_cast<HbAction *>(sender());
        if (closingAction == mIgnoreCloseAction) {
            // Prevents the dialog from begin closed when "Ok" pressed
            event->ignore();
            return;
        }
    }
    HbDialog::closeEvent(event);
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
// SifUiDialog::installTranslator()
// ----------------------------------------------------------------------------
//
void SifUiDialog::installTranslator()
{
    mTranslator = new QTranslator(this);
    QString lang = QLocale::system().name();
    mTranslator->load(KTranslationsFile + lang, KTranslationsPath);
    qApp->installTranslator(mTranslator);
}

// ----------------------------------------------------------------------------
// SifUiDialog::removeTranslator()
// ----------------------------------------------------------------------------
//
void SifUiDialog::removeTranslator()
{
    QApplication *app = qApp;
    if (mTranslator && app) {
        app->removeTranslator(mTranslator);
        mTranslator = 0;
    }
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

    // Title
    Q_ASSERT(mTitle == 0);
    mTitle = new SifUiDialogTitleWidget(this);
    mTitle->constructFromParameters(parameters);
    connect(mTitle, SIGNAL(certificatesClicked()),
            this, SLOT(handleDisplayCertificateDetails()));
    setHeadingWidget(mTitle);

    // Content
    Q_ASSERT(mContent == 0);
    mContent = new SifUiDialogContentWidget(this);
    mContent->constructFromParameters(parameters);
    connect(mContent, SIGNAL(memorySelectionChanged(const QChar &)),
            this, SLOT(handleMemorySelectionChanged(const QChar &)));
    setContentWidget(mContent);

    // Buttons
    updateButtons();

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
        updateButtons();
    }
    if (parameters.contains(KSifUiErrorCode)) {
        mInstallError = parameters.value(KSifUiErrorCode).toInt();
    }
    return true;
}

// ----------------------------------------------------------------------------
// SifUiDialog::updateButtons()
// ----------------------------------------------------------------------------
//
void SifUiDialog::updateButtons()
{
    mIgnoreCloseAction = 0;

    HbAction *primaryAction = 0;
    switch (mDialogType) {
        case SifUiConfirmationQuery:
            //: Accepts the SW install confirmation query and starts installation.
            // TODO: enable when translations ready
            //primaryAction = new HbAction(hbTrId("txt_sisxui_install_ok"), this);
            primaryAction = new HbAction(tr("Ok"), this);
            connect(primaryAction, SIGNAL(triggered()), this, SLOT(handleAccepted()));
            mIgnoreCloseAction = primaryAction;
            break;
        case SifUiProgressNote:
            //: Hides the progress dialog. Progress note moves into universal indicator.
            // TODO: enable when translations ready
            //primaryAction = new HbAction(hbTrId("txt_sisxui_install_hide"), this);
            primaryAction = new HbAction(tr("Hide"), this);
            connect(primaryAction, SIGNAL(triggered()), this, SLOT(handleHide()));
            mIgnoreCloseAction = primaryAction;
            break;
        case SifUiCompleteNote:
            //: Opens Application Library to view the installed application.
            // TODO: enable when translations ready
            //primaryAction = new HbAction(hbTrId("txt_sisxui_install_show"), this);
            primaryAction = new HbAction(tr("Show"), this);
            connect(primaryAction, SIGNAL(triggered()), this, SLOT(handleShowInstalled()));
            mIgnoreCloseAction = primaryAction;
            break;
        case SifUiErrorNote:
            //: Shows a dialog with further info about the failure (i.e. why installation failed).
            // TODO: enable when translations ready
            //primaryAction = new HbAction(hbTrId("txt_sisxui_install_error_details"), this);
            primaryAction = new HbAction(tr("Details"), this);
            connect(primaryAction, SIGNAL(triggered()), this, SLOT(handleErrorDetails()));
            mIgnoreCloseAction = primaryAction;
            break;
        default:
            break;
    }
    setPrimaryAction(primaryAction);

    HbAction *secondaryAction = 0;
    switch (mDialogType) {
        case SifUiConfirmationQuery:
        case SifUiProgressNote:
            //: Cancels the SW install confirmation query and closes the dialog.
            // TODO: enable when translations ready
            //secondaryAction = new HbAction(hbTrId("txt_sisxui_install_cancel"), this);
            secondaryAction = new HbAction(tr("Cancel"), this);
            connect(secondaryAction, SIGNAL(triggered()), this, SLOT(handleCancelled()));
            break;
        case SifUiCompleteNote:
        case SifUiErrorNote:
            //: Closes the dialog. Control returns back to where the installation was started.
            // TODO: enable when translations ready
            //secondaryAction = new HbAction(hbTrId("txt_sisxui_install_close"), this);
            secondaryAction = new HbAction(tr("Close"), this);
            connect(secondaryAction, SIGNAL(triggered()), this, SLOT(close()));
            break;
        default:
            break;
    }
    setSecondaryAction(secondaryAction);
}

// ----------------------------------------------------------------------------
// SifUiDialog::sendResult()
// ----------------------------------------------------------------------------
//
void SifUiDialog::sendResult(bool accepted)
{
    QVariant acceptedValue(accepted);
    mResultMap.insert(KSifUiQueryAccepted, acceptedValue);
    emit deviceDialogData(mResultMap);
}

// ----------------------------------------------------------------------------
// SifUiDialog::monitorIndicatorActivity()
// ----------------------------------------------------------------------------
//
void SifUiDialog::monitorIndicatorActivity()
{
    if (!mSubscriber) {
        mSubscriber = new QTM_PREPEND_NAMESPACE(QValueSpaceSubscriber(KSifUiIndicatorPath));
        connect(mSubscriber, SIGNAL(contentsChanged()),
            this, SLOT(handleIndicatorActivityChanged()));
    }
}

// ----------------------------------------------------------------------------
// SifUiDialog::handleAccepted()
// ----------------------------------------------------------------------------
//
void SifUiDialog::handleAccepted()
{
    mContent->changeType(SifUiProgressNote);
    sendResult(true);
}

// ----------------------------------------------------------------------------
// SifUiDialog::handleCancelled()
// ----------------------------------------------------------------------------
//
void SifUiDialog::handleCancelled()
{
    sendResult(false);
}

// ----------------------------------------------------------------------------
// SifUiDialog::handleMemorySelectionChanged()
// ----------------------------------------------------------------------------
//
void SifUiDialog::handleMemorySelectionChanged(const QChar &driveLetter)
{
    QVariant memorySelection(driveLetter);
    mResultMap.insert(KSifUiSelectedMemory, memorySelection);
}

// ----------------------------------------------------------------------------
// SifUiDialog::handleDisplayCertificateDetails()
// ----------------------------------------------------------------------------
//
void SifUiDialog::handleDisplayCertificateDetails()
{
    // TODO: display certificate details, or the following note:
    HbMessageBox::warning(tr("Application is not certified. It's origin and authenticity cannot be proved."));
}

// ----------------------------------------------------------------------------
// SifUiDialog::handleHidePressed()
// ----------------------------------------------------------------------------
//
void SifUiDialog::handleHidePressed()
{
    if (!mIndicator) {
        mIndicator = new HbIndicator(this);

        QVariantMap variantMap;
        QVariant applicationNameVariant(mContent->applicationName());
        variantMap.insert(KSifUiIndicatorApplicationName, applicationNameVariant);

        if (!mContent->isDefaultIconUsed()) {
            int iconHandle = 0;
            int maskHandle = 0;
            mContent->iconHandles(iconHandle, maskHandle);
            QVariant iconHandleVariant(iconHandle);
            variantMap.insert(KSifUiIndicatorAppIconHandle, iconHandleVariant);
            QVariant maskHandleVariant(maskHandle);
            variantMap.insert(KSifUiIndicatorAppIconMaskHandle, maskHandleVariant);
        }

        int finalValue = 0;
        int progressValue = 0;
        mContent->progressInfo(finalValue, progressValue);
        QVariant finalValueVariant(finalValue);
        variantMap.insert(KSifUiIndicatorProgressFinal, finalValueVariant);
        QVariant progressValueVariant(progressValue);
        variantMap.insert(KSifUiIndicatorProgressValue, progressValueVariant);

        QVariant parameter(variantMap);
        if (mIndicator->activate(KSifUiIndicatorPlugin, parameter)) {
            hide();
            monitorIndicatorActivity();
        }
    }
}

// ----------------------------------------------------------------------------
// SifUiDialog::handleIndicatorActivityChanged()
// ----------------------------------------------------------------------------
//
void SifUiDialog::handleIndicatorActivityChanged()
{
    QVariant variant = mSubscriber->value(KSifUiIndicatorActive);
    if (variant.isValid() && (variant.type() == QVariant::Int)) {
        bool valueOk = false;
        int intValue = variant.toInt(&valueOk);
        if (valueOk && intValue) {
            show();
        }
    }
}

// ----------------------------------------------------------------------------
// SifUiDialog::handleShowInstalled()
// ----------------------------------------------------------------------------
//
void SifUiDialog::handleShowInstalled()
{
    // TODO: launch applib
    HbMessageBox::warning(tr("Not implemented yet"));
}

// ----------------------------------------------------------------------------
// SifUiDialog::handleErrorDetails()
// ----------------------------------------------------------------------------
//
void SifUiDialog::handleErrorDetails()
{
    // TODO: show proper error details dialog
    QString messageText;
    messageText = tr("Not implemented yet.\n\nError code %1").arg(mInstallError);

    if (QFile::exists(KSwiErrorsFile)) {
        messageText.append(KSwiErrorFormat.arg(mInstallError));
    }
    HbMessageBox::warning(messageText);
}

