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
#include <QGraphicsLinearLayout>
#include <hblabel.h>
#include <hbpushbutton.h>

const char KSifUiCertTrusted[]    = "qtg_small_secure.svg";
// TODO: enable when available, take also in use
//const char KSifUiCertNotTrusted[] = "qtg_small_untrusted.svg";


// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::SifUiDialogTitleWidget()
// ----------------------------------------------------------------------------
//
SifUiDialogTitleWidget::SifUiDialogTitleWidget(QGraphicsItem *parent,
        Qt::WindowFlags flags): HbWidget(parent, flags),
        mLayout(0), mTitle(0), mCertButton(0)
{
}

// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::~SifUiDialogTitleWidget()
// ----------------------------------------------------------------------------
//
SifUiDialogTitleWidget::~SifUiDialogTitleWidget()
{
}

// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::constructFromParameters()
// ----------------------------------------------------------------------------
//
void SifUiDialogTitleWidget::constructFromParameters(const QVariantMap &parameters)
{
    mLayout = new QGraphicsLinearLayout(Qt::Horizontal);

    Q_ASSERT(mTitle == 0);
    HbLabel *titleLabel = 0;
    if (parameters.contains(KSifUiDialogTitle)) {
        QString titleText = parameters.value(KSifUiDialogTitle).toString();
        titleLabel = new HbLabel(titleText);
    } else {
        SifUiDeviceDialogType type = SifUiDialog::dialogType(parameters);
        titleLabel = new HbLabel(defaultTitle(type));
    }
    titleLabel->setFontSpec(HbFontSpec(HbFontSpec::Title));
    mLayout->addItem(titleLabel);
    mTitle = titleLabel;

    createCertButton();

    setLayout(mLayout);
}

// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::updateFromParameters()
// ----------------------------------------------------------------------------
//
void SifUiDialogTitleWidget::updateFromParameters(const QVariantMap &parameters)
{
    Q_ASSERT(mTitle != 0);

    QString titleText;
    if (parameters.contains(KSifUiDialogTitle)) {
        titleText = parameters.value(KSifUiDialogTitle).toString();
    } else {
        SifUiDeviceDialogType type = SifUiDialog::dialogType(parameters);
        titleText = defaultTitle(type);
    }
    if (titleText != mTitle->plainText()) {
        mTitle->setPlainText(titleText);
    }
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
// SifUiDialogTitleWidget::createCertButton()
// ----------------------------------------------------------------------------
//
void SifUiDialogTitleWidget::createCertButton()
{
    if (!mCertButton) {
        HbPushButton *certButton = new HbPushButton;
        // TODO: show KSifUiCertNotTrusted when needed (and when available)
        certButton->setIcon(HbIcon(KSifUiCertTrusted));
        connect(certButton,SIGNAL(clicked()),this,SIGNAL(certificatesClicked()));
        mLayout->addStretch();
        mLayout->addItem(certButton);
        mLayout->setAlignment(certButton, Qt::AlignRight|Qt::AlignVCenter);
        mCertButton = certButton;
    }
}

// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::removeCertButton()
// ----------------------------------------------------------------------------
//
void SifUiDialogTitleWidget::removeCertButton()
{
    if (mCertButton && mLayout) {
        mLayout->removeItem(mCertButton);
        delete mCertButton;
        mCertButton = 0;
    }
}

