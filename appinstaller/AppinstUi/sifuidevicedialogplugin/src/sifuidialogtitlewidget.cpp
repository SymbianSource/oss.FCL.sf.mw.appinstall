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

// TODO: change these to mono icons when available, see graphics request TLIS-855ECE
const char KSifUiCertTrusted[]    = "qtg_small_secure";
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
QString SifUiDialogTitleWidget::defaultTitle(SifUiDeviceDialogType type)
{
    QString title;
    switch (type) {
        case SifUiConfirmationQuery:
            //: Install confirmation query title. Installation starts if
            //: the user accepts the query.
            // TODO: enable when translations ready
            //title = hbTrId("txt_sisxui_install_conf_head");
            title = tr("Install?");
            break;
        case SifUiProgressNote:
            //: Progress note title. Installation is going on and progress
            //: bar shows how it proceeds.
            // TODO: enable when translations ready
            //title = hbTrId("txt_sisxui_installing_progress_head");
            title = tr("Installing");
            break;
        case SifUiCompleteNote:
            //: Installation complete note title. Indicates that installation
            //: was succesfully completed. User has option to launch AppLib
            // TODO: enable when translations ready
            //title = hbTrId("txt_sisxui_install_complete_head");
            title = tr("Installed");
            break;
        case SifUiErrorNote:
            //: Installation failed note title. Indicates that installation failed.
            // TODO: enable when translations ready
            //title = hbTrId("txt_sisxui_install_failed_head");
            title = tr("Install failed");
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
        titleText = defaultTitle(type);
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
        HbMessageBox::warning(tr("Application is not certified. It's origin and authenticity cannot be proved."));
    }
}
