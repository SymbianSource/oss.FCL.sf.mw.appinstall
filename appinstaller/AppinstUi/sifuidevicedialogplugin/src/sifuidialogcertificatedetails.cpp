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
* Description: Certificate details dialog in SifUi
*
*/

#include "sifuidialogcertificatedetails.h"      // SifUiCertificateDetails
#include "sifuidialogcertificateinfo.h"         // SifUiDialogCertificateInfo
#include <hblabel.h>                            // HbLabel
#include <hbtextedit.h>                         // HbTextEdit
#include <hbscrollbar.h>                        // HbScrollBar
#include <hblistwidget.h>                       // HbListWidget
#include <hblistwidgetitem.h>                   // HbListWidgetItem
#include <hbaction.h>                           // HbAction
#include <QTextStream>


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// SifUiDialogCertificateDetails::SifUiDialogCertificateDetails()
// ----------------------------------------------------------------------------
//
SifUiDialogCertificateDetails::SifUiDialogCertificateDetails(
    const QList<SifUiDialogCertificateInfo*> &certificates) : HbDialog(),
    mCertificates(certificates)
{
    setDismissPolicy(HbPopup::NoDismiss);
    setTimeout(HbPopup::NoTimeout);
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateDetails::showDetails()
// ----------------------------------------------------------------------------
//
void SifUiDialogCertificateDetails::showDetails()
{
    int count = mCertificates.count();
    Q_ASSERT(count != 0);
    if (count == 1) {
        showCertificate(0);
    } else {
        showList();
    }
    open();
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateDetails::~SifUiDialogCertificateDetails()
// ----------------------------------------------------------------------------
//
SifUiDialogCertificateDetails::~SifUiDialogCertificateDetails()
{
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateDetails::certificateDetails()
// ----------------------------------------------------------------------------
//
QString SifUiDialogCertificateDetails::certificateDetails(
    const SifUiDialogCertificateInfo& certificate)
{
    // TODO: change to properly localizable string
    QString details;
    QTextStream stream(&details);
    stream << tr("Issuer:\n")
        << certificate.issuerName()
        << endl << endl;
    stream << tr("Subject:\n")
        << certificate.subjectName()
        << endl << endl;
    stream << tr("Valid from:\n")
        << certificate.validFrom().toString()
        << endl << endl;
    stream << tr("Valid until:\n")
        << certificate.validTo().toString()
        << endl << endl;
    stream << tr("Serial number:\n")
        << certificate.formattedSerialNumber()
        << endl << endl;
    stream << tr("Fingerprint (SHA1):\n")
        << certificate.formattedFingerprint()
        << endl;
    return details;
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateDetails::showList()
// ----------------------------------------------------------------------------
//
void SifUiDialogCertificateDetails::showList()
{
    int count = mCertificates.count();
    Q_ASSERT(count > 1);

    // TODO: localisation
    HbLabel *title = new HbLabel(tr("Select certificate"));
    setHeadingWidget(title);

    HbListWidget *listWidget = new HbListWidget;
    for (int index = 0; index < count; ++index ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setData(QVariant(mCertificates.at(index)->subjectName()));
        listWidget->addItem(item);
    }
    listWidget->setScrollDirections(Qt::Vertical);
    listWidget->setFrictionEnabled(true);
    listWidget->setScrollingStyle(HbScrollArea::PanWithFollowOn);
    listWidget->verticalScrollBar()->setInteractive(true);
    connect(listWidget, SIGNAL(activated(HbListWidgetItem*)),
        this, SLOT(certificateSelected()));
    setContentWidget(listWidget);

    addAction(new HbAction(hbTrId("txt_common_button_close"), this));
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateDetails::certificateSelected()
// ----------------------------------------------------------------------------
//
void SifUiDialogCertificateDetails::certificateSelected()
{
    QGraphicsWidget *content = contentWidget();
    if (content->type() == Hb::ItemType_ListView) {
        HbListWidget *listWidget = reinterpret_cast<HbListWidget*>(content);
        int index = listWidget->currentRow();
        if (index >= 0 && index < mCertificates.count()) {
            showCertificate(index);
        }
    }
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateDetails::showCertificate()
// ----------------------------------------------------------------------------
//
void SifUiDialogCertificateDetails::showCertificate(int index)
{
    // TODO: localisation
    HbLabel *title = new HbLabel(tr("Certificate details"));
    setHeadingWidget(title);

    HbTextEdit *textEdit = new HbTextEdit;
    Q_ASSERT(index >= 0 && index < mCertificates.count());
    QString details = certificateDetails(*mCertificates[index]);
    textEdit->setPlainText(details);
    textEdit->setReadOnly(true);
    textEdit->setCursorVisibility(Hb::TextCursorHidden);
    // TODO: how to display scroll bar and not to show blue border(focus)?
    setContentWidget(textEdit);

    //: Closes the dialog. Control returns back to where the installation was started.
    addAction(new HbAction(hbTrId("txt_common_button_close"), this));
}

