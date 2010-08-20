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
* Description: Language selection dialog in SifUi
*
*/

#include "sifuidialogselectlanguage.h"  // SifUiDialogSelectLanguage
#include <HbLabel>
#include <HbRadioButtonList>
#include <HbAction>


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// SifUiDialogSelectLanguage::SifUiDialogSelectLanguage()
// ----------------------------------------------------------------------------
//
SifUiDialogSelectLanguage::SifUiDialogSelectLanguage(const QVariant &languages,
        QGraphicsItem *parent) : HbDialog(parent), mRadioButtonList(0), mOkAction(0)
{
    setTimeout(HbPopup::NoTimeout);
    setDismissPolicy(HbPopup::NoDismiss);
    setModal(true);

    HbLabel *title = new HbLabel(hbTrId("txt_installer_title_select_language"));
    setHeadingWidget(title);

    Q_ASSERT( mRadioButtonList == 0 );
    mRadioButtonList = new HbRadioButtonList(this);
    mRadioButtonList->setItems(languageNames(languages));
    setContentWidget(mRadioButtonList);

    HbAction *okAction = new HbAction(QString(hbTrId("txt_common_button_ok")), this);
    disconnect(okAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(okAction, SIGNAL(triggered()), this, SLOT(handleAccepted()));
    addAction(okAction);

    HbAction *cancelAction = new HbAction(QString(hbTrId("txt_common_button_cancel")), this);
    disconnect(cancelAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(cancelAction, SIGNAL(triggered()), this, SLOT(handleRejected()));
    addAction(cancelAction);
}

// ----------------------------------------------------------------------------
// SifUiDialogSelectLanguage::~SifUiDialogSelectLanguage()
// ----------------------------------------------------------------------------
//
SifUiDialogSelectLanguage::~SifUiDialogSelectLanguage()
{
}

// ----------------------------------------------------------------------------
// SifUiDialogSelectLanguage::selectedLanguageIndex()
// ----------------------------------------------------------------------------
//
int SifUiDialogSelectLanguage::selectedLanguageIndex() const
{
    if (mRadioButtonList) {
        return mRadioButtonList->selected();
    }
    return -1;
}

// ----------------------------------------------------------------------------
// SifUiDialogSelectLanguage::handleAccepted()
// ----------------------------------------------------------------------------
//
void SifUiDialogSelectLanguage::handleAccepted()
{
    int index = selectedLanguageIndex();
    emit languageSelected(index);
    close();
}

// ----------------------------------------------------------------------------
// SifUiDialogSelectLanguage::handleRejected()
// ----------------------------------------------------------------------------
//
void SifUiDialogSelectLanguage::handleRejected()
{
    emit languageSelectionCancelled();
    close();
}

