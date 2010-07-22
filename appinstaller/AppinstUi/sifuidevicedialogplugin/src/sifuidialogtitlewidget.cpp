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
* Description: SIF UI dialog title widget.
*
*/

#include "sifuidialogtitlewidget.h"
#include "sifuidialog.h"                    // dialogType, dialogMode
#include "sifuidialogcertificatedetails.h"  // SifUiDialogCertificateDetails
#include <QGraphicsLinearLayout>
#include <hblabel.h>
#include <hbpushbutton.h>
#include <hbmessagebox.h>

const char KSifUiCertTrusted[]    = "qtg_small_lock";
const char KSifUiCertNotTrusted[] = "qtg_small_untrusted";


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::SifUiDialogTitleWidget()
// ----------------------------------------------------------------------------
//
SifUiDialogTitleWidget::SifUiDialogTitleWidget(QGraphicsItem *parent,
        Qt::WindowFlags flags): HbWidget(parent, flags),
        mLayout(0), mTitle(0), mCertButton(0), mCertificates(0), mDetailsDialog(0)
{
}

// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::~SifUiDialogTitleWidget()
// ----------------------------------------------------------------------------
//
SifUiDialogTitleWidget::~SifUiDialogTitleWidget()
{
    delete mDetailsDialog;
    delete mCertificates;
}

// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::constructFromParameters()
// ----------------------------------------------------------------------------
//
void SifUiDialogTitleWidget::constructFromParameters(const QVariantMap &parameters)
{
    mLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    updateTitle(parameters);
    updateCertificates(parameters);
    setLayout(mLayout);
}

// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::updateFromParameters()
// ----------------------------------------------------------------------------
//
void SifUiDialogTitleWidget::updateFromParameters(const QVariantMap &parameters)
{
    updateTitle(parameters);
    updateCertificates(parameters);
}

// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::defaultTitle()
// ----------------------------------------------------------------------------
//
QString SifUiDialogTitleWidget::defaultTitle(SifUiDeviceDialogType type,
        SifUiInstallingPhase phase)
{
    QString title;
    switch (type) {
        case SifUiConfirmationQuery:
            //: Custom layout ID parent. Install confirmation query title.
            //: Installation of an application starts if the user accepts the query.
            title = hbTrId("txt_installer_title_install");
            break;
        case SifUiProgressNote:
            switch (phase) {
                case SifUiCheckingCerts:
                    //: Progress note title. Installer validates certificates online.
                    //: This may take some time.
                    //TODO: localised UI string needed
                    title = hbTrId("Checking certificate validity");
                    break;
                case SifUiInstalling:
                case SifUiDownloading:
                default:
                    //: Progress note title. Installation is going on and progress bar
                    //: shows how it proceeds.
                    title = hbTrId("txt_installer_title_installing");
                    break;
            }
            break;
        case SifUiCompleteNote:
            //: Installation complete note title. Indicates that installation
            //: was succesfully completed. User has option to launch AppLib.
            title = hbTrId("txt_installer_title_installed");
            break;
        case SifUiErrorNote:
            //: Installation failed note title. Indicates that installation failed.
            title = hbTrId("txt_installer_info_installation_failed");
            break;
        case SifUiUnspecifiedDialog:
        default:
            break;
    }
    return title;
}

// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::updateTitle()
// ----------------------------------------------------------------------------
//
void SifUiDialogTitleWidget::updateTitle(const QVariantMap &parameters)
{
    QString titleText;
    if (parameters.contains(KSifUiDialogTitle)) {
        titleText = parameters.value(KSifUiDialogTitle).toString();
    } else {
        SifUiDeviceDialogType type = SifUiDialog::dialogType(parameters);
        SifUiInstallingPhase progressPhase = SifUiInstalling;
        if (parameters.contains(KSifUiProgressNotePhase)) {
            bool ok = false;
            int i = parameters.value(KSifUiProgressNotePhase).toInt(&ok);
            if (ok) {
                progressPhase = static_cast<SifUiInstallingPhase>(i);
            }
        }
        titleText = defaultTitle(type, progressPhase);
    }
    if (mTitle) {
        if (titleText != mTitle->plainText()) {
            mTitle->setPlainText(titleText);
        }
    } else {
        mTitle = new HbLabel(titleText);
        mTitle->setFontSpec(HbFontSpec(HbFontSpec::Title));
        mLayout->addItem(mTitle);
    }
}

// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::updateCertificates()
// ----------------------------------------------------------------------------
//
void SifUiDialogTitleWidget::updateCertificates(const QVariantMap &parameters)
{
    if (!mCertButton) {
        mCertButton = new HbPushButton;
        connect(mCertButton, SIGNAL(clicked()), this, SLOT(certificatesClicked()));

        if (parameters.contains(KSifUiCertificates)) {
            Q_ASSERT(mCertificates == 0);
            mCertificates = getCertificates(parameters.value(KSifUiCertificates).toByteArray());
            mCertButton->setIcon(HbIcon(KSifUiCertTrusted));
        } else {
            mCertButton->setIcon(HbIcon(KSifUiCertNotTrusted));
        }

        mLayout->addStretch();
        mLayout->addItem(mCertButton);
        mLayout->setAlignment(mCertButton, Qt::AlignRight|Qt::AlignVCenter);
    } else {
        if (mCertificates == 0 && parameters.contains(KSifUiCertificates)) {
            mCertificates = getCertificates(parameters.value(KSifUiCertificates).toByteArray());
            mCertButton->setIcon(HbIcon(KSifUiCertTrusted));
        }
    }
}

// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::certificatesClicked()
// ----------------------------------------------------------------------------
//
void SifUiDialogTitleWidget::certificatesClicked()
{
    if (mCertificates) {
        if (mDetailsDialog) {
            delete mDetailsDialog;
            mDetailsDialog = 0;
        }
        mDetailsDialog = new SifUiDialogCertificateDetails(*mCertificates);
        mDetailsDialog->showDetails();
    } else {
        // TODO: localize
        HbMessageBox::warning(tr("Application is not certified. It's origin and authenticity cannot be proved."));
    }
}
