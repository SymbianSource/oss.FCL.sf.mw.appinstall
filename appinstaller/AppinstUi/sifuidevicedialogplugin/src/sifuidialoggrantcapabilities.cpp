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

#include "sifuidialoggrantcapabilities.h"   // SifUiDialogGrantCapabilities
#include "sifuidialoggrantcapabilitiescontent.h" // SifUiDialogGrantCapabilitiesContent
#include <HbLabel>


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// SifUiDialogGrantCapabilities::SifUiDialogGrantCapabilities()
// ----------------------------------------------------------------------------
//
SifUiDialogGrantCapabilities::SifUiDialogGrantCapabilities(const QString& applicationName,
    const QVariant &capabilities, QGraphicsItem *parent) : HbDialog(parent)
{
    setTimeout(HbPopup::NoTimeout);
    setDismissPolicy(HbPopup::NoDismiss);
    setModal(true);

    // TODO: localized UI string needed
    //HbLabel *title = new HbLabel(hbTrId("txt_permissions_needed"));
    HbLabel *title = new HbLabel(QString("Permissions needed"));
    setHeadingWidget(title);

    SifUiDialogGrantCapabilitiesContent *content =
        new SifUiDialogGrantCapabilitiesContent(applicationName, capabilities);
    connect(content, SIGNAL(accepted()), this, SLOT(handleAccepted()));
    connect(content, SIGNAL(rejected()), this, SLOT(handleRejected()));
    setContentWidget(content);
}

// ----------------------------------------------------------------------------
// SifUiDialogGrantCapabilities::~SifUiDialogGrantCapabilities()
// ----------------------------------------------------------------------------
//
SifUiDialogGrantCapabilities::~SifUiDialogGrantCapabilities()
{
}

// ----------------------------------------------------------------------------
// SifUiDialogGrantCapabilities::handleAccepted()
// ----------------------------------------------------------------------------
//
void SifUiDialogGrantCapabilities::handleAccepted()
{
    emit accepted();
    close();
}

// ----------------------------------------------------------------------------
// SifUiDialogGrantCapabilities::handleRejected()
// ----------------------------------------------------------------------------
//
void SifUiDialogGrantCapabilities::handleRejected()
{
    emit rejected();
    close();
}

