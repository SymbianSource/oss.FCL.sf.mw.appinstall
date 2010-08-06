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

#ifndef SIFUIDIALOGCERTIFICATEDETAILS_H
#define SIFUIDIALOGCERTIFICATEDETAILS_H

#include <hbdialog.h>                   // HbDialog

class SifUiDialogCertificateInfo;


/**
 * SifUi certificate details dialog. Implements selection list and
 * certificate details dialogs. Selection list is shown first if there
 * are more than one certificate to be displayed.
 */
class SifUiDialogCertificateDetails : public HbDialog
{
    Q_OBJECT

public:     // constructor and destructor
    SifUiDialogCertificateDetails(const QList<SifUiDialogCertificateInfo*> &certificates);
    virtual ~SifUiDialogCertificateDetails();

public:     // new functions
    void showDetails();

private:    // new functions
    QString certificateDetails(const SifUiDialogCertificateInfo& certificate);
    void showList();

private slots:
    void certificateSelected();
    void showCertificate(int index);

private:    // data
    const QList<SifUiDialogCertificateInfo*> &mCertificates;
};

#endif // SIFUIDIALOGCERTIFICATEDETAILS_H
