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

// See definitions in sifuidevicedialogplugin.qrc
const char KSifUiDialogIconCertificates[] = ":/cert_indi_icon.svg";


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
        SifUiDeviceDialogMode mode = SifUiDialog::dialogMode(parameters);
        titleLabel = new HbLabel(defaultTitle(type, mode));
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
        SifUiDeviceDialogMode mode = SifUiDialog::dialogMode(parameters);
        titleText = defaultTitle(type, mode);
    }
    if (titleText != mTitle->plainText()) {
        mTitle->setPlainText(titleText);
    }

    if (parameters.contains(KSifUiCertificates)) {
        createCertButton();
    } else {
        removeCertButton();
    }
}

// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::defaultTitle()
// ----------------------------------------------------------------------------
//
QString SifUiDialogTitleWidget::defaultTitle(SifUiDeviceDialogType type,
    SifUiDeviceDialogMode mode)
{
    QString title;
    switch (mode) {
        case SifUiInstalling:
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
            break;
        case SifUiUninstalling:
            switch (type) {
                case SifUiConfirmationQuery:
                    //: Uninstall confirmation query title. Asks permission to
                    //: remove selected application/other content.
                    // TODO: enable when translations ready
                    //title = hbTrId("txt_sisxui_uninstall_conf_head");
                    title = tr("Remove?");
                    break;
                case SifUiProgressNote:
                    //: Progress note title. Uninstallation is going on and progress
                    //: bar shows how it proceeds.
                    // TODO: enable when translations ready
                    //title = hbTrId("txt_sisxui_uninstalling_progress_head");
                    title = tr("Removing");
                    break;
                case SifUiCompleteNote:
                    //: Uninstallation complete note title. Indicates that application
                    //: was succesfully removed.
                    // TODO: enable when translations ready
                    //title = hbTrId("txt_sisxui_uninstall_complete_head");
                    title = tr("Removed");
                    break;
                case SifUiErrorNote:
                    //: Uninstallation failed note title. Indicates that uninstallation failed.
                    // TODO: enable when translations ready
                    //title = hbTrId("txt_sisxui_uninstall_failed_head");
                    title = tr("Uninstall failed");
                    break;
                case SifUiUnspecifiedDialog:
                default:
                    break;
            }
            break;
        case SifUiUnspecified:
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
        certButton->setIcon(HbIcon(KSifUiDialogIconCertificates));
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

