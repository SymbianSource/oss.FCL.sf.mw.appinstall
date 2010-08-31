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

#ifndef SIFUIDIALOGSELECTLANGUAGE_H
#define SIFUIDIALOGSELECTLANGUAGE_H

#include <HbDialog>

class HbRadioButtonList;


/**
 * SifUi dialog requesting the user to select language. This
 * dialog is displayed only if the languages in the sis-file
 * do not match the languages that are supported by the mobile
 * phone.
 */
class SifUiDialogSelectLanguage : public HbDialog
{
    Q_OBJECT

public:     // constructor and destructor
    SifUiDialogSelectLanguage(const QVariant &languages, QGraphicsItem *parent = 0);
    ~SifUiDialogSelectLanguage();

signals:
    void languageSelected(int index);
    void languageSelectionCancelled();

public:     // new functions
    int selectedLanguageIndex() const;

private:    // new functions
    QStringList languageNames(const QVariant &languages);

private slots:
    void handleAccepted();
    void handleRejected();

private:    // data
    HbRadioButtonList *mRadioButtonList;
    HbAction *mOkAction;
};

#endif // SIFUIDIALOGSELECTLANGUAGE_H
