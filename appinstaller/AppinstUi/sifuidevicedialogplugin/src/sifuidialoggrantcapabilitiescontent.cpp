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
* Description: Grant capabilities dialog in SifUi
*
*/

#include "sifuidialoggrantcapabilitiescontent.h" // SifUiDialogGrantCapabilitiesContent
#include <QGraphicsLinearLayout>
#include <HbLabel>
#include <HbPushButton>
#include <HbScrollArea>
#include <HbAction>
#include <HbDialog>

const QString KDetailsTextHtmlBeginList = "<html>%1<ul>";
const QString KDetailsTextHtmlListItem = "<li>%1</li>";
const QString KDetailsTextHtmlEndList = "</ul></html>";


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// SifUiDialogGrantCapabilitiesContent::~SifUiDialogGrantCapabilitiesContent()
// ----------------------------------------------------------------------------
//
SifUiDialogGrantCapabilitiesContent::SifUiDialogGrantCapabilitiesContent(
    const QString& applicationName, const QVariant &capabilities, QGraphicsItem *parent)
    : HbWidget(parent), mCapabilities(), mIsCapabilitiesValid(false)
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);

    HbLabel *label = new HbLabel();
    //TODO: localized UI string needed
    //label->setPlainText(hbTrId("txt_").arg(applicationName));
    label->setPlainText(tr("Application \"%1\" uses capabilities it is not signed for.")
        .arg(applicationName));
    label->setTextWrapping(Hb::TextWordWrap);
    layout->addItem(label);
    layout->addStretch();

    //TODO: localized UI string needed
    //HbPushButton *details = new HbPushButton(hbTrId("txt_"));
    HbPushButton *details = new HbPushButton(tr("View details"));
    connect(details, SIGNAL(clicked()), this, SLOT(viewDetails()));
    layout->addItem(details);
    layout->addStretch();

    HbPushButton *accept = new HbPushButton;
    //TODO: localized UI string needed
    //accept->setText(hbTrId("txt_"));
    accept->setText(tr("I trust the app"));
    //TODO: localized UI string needed
    //accept->setAdditionalText(hbTrId("txt_"));
    accept->setAdditionalText(tr("Grant all permissions"));
    connect(accept, SIGNAL(clicked()), this, SIGNAL(accepted()));
    layout->addItem(accept);

    HbPushButton *deny = new HbPushButton;
    //TODO: localized UI string needed
    //deny->setText(hbTrId("txt_"));
    deny->setText(tr("I do not trust"));
    //TODO: localized UI string needed
    //deny->setAdditionalText(hbTrId("txt_"));
    deny->setAdditionalText(tr("Cancel installation"));
    connect(deny, SIGNAL(clicked()), this, SIGNAL(rejected()));
    layout->addItem(deny);

    setLayout(layout);
    setCapabilities(capabilities);
}

// ----------------------------------------------------------------------------
// SifUiDialogGrantCapabilitiesContent::~SifUiDialogGrantCapabilitiesContent()
// ----------------------------------------------------------------------------
//
SifUiDialogGrantCapabilitiesContent::~SifUiDialogGrantCapabilitiesContent()
{
}

// ----------------------------------------------------------------------------
// SifUiDialogGrantCapabilitiesContent::viewDetails()
// ----------------------------------------------------------------------------
//
void SifUiDialogGrantCapabilitiesContent::viewDetails()
{
    HbDialog *detailsDialog= new HbDialog();
    detailsDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    detailsDialog->setTimeout(HbPopup::NoTimeout);
    detailsDialog->setDismissPolicy(HbDialog::NoDismiss);
    detailsDialog->setModal(true);

    // TODO: localised UI string needed
    QString detailsTitle = tr("Permission details");
    detailsDialog->setHeadingWidget(new HbLabel(detailsTitle));

    HbScrollArea *scrollArea = new HbScrollArea(this);

    // TODO: localized UI string needed
    QString detailsText = KDetailsTextHtmlBeginList.arg("The application has requested permissions to:");
    QStringListIterator capaIter(mCapabilities);
    while (capaIter.hasNext()) {
        detailsText.append(KDetailsTextHtmlListItem.arg(capaIter.next()));
    }
    detailsText.append(KDetailsTextHtmlEndList);

    HbLabel *detailsTextLabel = new HbLabel;
    detailsTextLabel->setHtml(detailsText);
    detailsTextLabel->setTextWrapping(Hb::TextWordWrap);

    scrollArea->setContentWidget(detailsTextLabel);
    scrollArea->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAsNeeded);
    detailsDialog->setContentWidget(scrollArea);

    HbAction *closeAction = new HbAction(hbTrId("txt_common_button_ok"));
    detailsDialog->addAction(closeAction);

    detailsDialog->show();
}

// ----------------------------------------------------------------------------
// SifUiDialogGrantCapabilitiesContent::setCapabilities()
// ----------------------------------------------------------------------------
//
void SifUiDialogGrantCapabilitiesContent::setCapabilities(const QVariant &capabilities)
{
    QByteArray byteArray = capabilities.toByteArray();

#ifdef Q_OS_SYMBIAN
    const TPtrC8 ptr(reinterpret_cast<const TText8*>(byteArray.constData()), byteArray.length());
    QT_TRAP_THROWING(setCapabilitiesSymbianL(ptr));
#endif  // Q_OS_SYMBIAN

    if (!mIsCapabilitiesValid) {
        emit rejected();
    }
}

#ifdef Q_OS_SYMBIAN
// ----------------------------------------------------------------------------
// SifUiDialogGrantCapabilitiesContent::setCapabilitiesSymbianL()
// ----------------------------------------------------------------------------
//
void SifUiDialogGrantCapabilitiesContent::setCapabilitiesSymbianL( const TDesC8& aBuf )
{
    const TPckgC<TCapabilitySet>& capabilitySetPckg( reinterpret_cast< const TPckgC<TCapabilitySet>& >( aBuf ) );
    const TCapabilitySet& capabilitySet( capabilitySetPckg() );

    mIsCapabilitiesValid = true;
    for( TInt i = 0; i < ECapability_HardLimit && mIsCapabilitiesValid; i++ ) {
        const TCapability capability( static_cast<TCapability>(i) );
        if( capabilitySet.HasCapability(capability) ) {
            QT_TRYCATCH_LEAVING( mCapabilities.append(capabilityName(capability)) );
        }
    }
}

// ----------------------------------------------------------------------------
// SifUiDialogGrantCapabilitiesContent::capabilityName()
// ----------------------------------------------------------------------------
//
QString SifUiDialogGrantCapabilitiesContent::capabilityName( const TCapability aCapability )
{
    QString name;

    switch( aCapability ) {
    case ECapabilityPowerMgmt:
        // TODO: is this user-grantable?
        name = tr("ECapabilityPowerMgmt");
        break;
    case ECapabilityReadDeviceData:
        // TODO: is this user-grantable?
        name = tr("ECapabilityReadDeviceData");
        break;
    case ECapabilityWriteDeviceData:
        // TODO: is this user-grantable?
        name = tr("ECapabilityWriteDeviceData");
        break;
    case ECapabilityTrustedUI:
        // TODO: is this user-grantable?
        name = tr("ECapabilityTrustedUI");
        break;
    case ECapabilityProtServ:
        // TODO: is this user-grantable?
        name = tr("ECapabilityProtServ");
        break;
    case ECapabilitySwEvent:
        // TODO: is this user-grantable?
        name = tr("ECapabilitySwEvent");
        break;
    case ECapabilityNetworkServices:
        // TODO: localized UI string needed
        name = tr("Make network connections or phone calls");
        break;
    case ECapabilityLocalServices:
        // TODO: localized UI string needed
        name = tr("Connect using local connectivity");
        break;
    case ECapabilityReadUserData:
        // TODO: localized UI string needed
        name = tr("Read user's personal data");
        break;
    case ECapabilityWriteUserData:
        // TODO: localized UI string needed
        name = tr("Modify user's personal data");
        break;
    case ECapabilityLocation:
        // TODO: localized UI string needed
        name = tr("Access the location information");
        break;
    case ECapabilitySurroundingsDD:
        // TODO: is this user-grantable?
        name = tr("ECapabilitySurroundingsDD");
        break;
    case ECapabilityUserEnvironment:
        // TODO: localized UI string needed
        name = tr("Access user environment");
        break;

    // Other capabilities are not user-grantable
    default:
        mIsCapabilitiesValid = false;
        break;
    }

    return name;
}
#endif  // Q_OS_SYMBIAN

