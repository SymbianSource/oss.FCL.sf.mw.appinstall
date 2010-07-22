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

#ifndef SIFUIDIALOGTITLEWIDGET_H
#define SIFUIDIALOGTITLEWIDGET_H

#include <hbwidget.h>
#include "sifuidialogdefinitions.h"         // SifUiDeviceDialogType, SifUiDeviceDialogMode
#include "sifuidialogcertificateinfo.h"     // SifUiDialogCertificateInfo

class QGraphicsLinearLayout;
class HbLabel;
class HbPushButton;
class SifUiDialogCertificateDetails;


class SifUiDialogTitleWidget : public HbWidget
{
    Q_OBJECT

public:
    SifUiDialogTitleWidget(QGraphicsItem *parent=0, Qt::WindowFlags flags=0);
    virtual ~SifUiDialogTitleWidget();

    void constructFromParameters(const QVariantMap &parameters);
    void updateFromParameters(const QVariantMap &parameters);

private:
    Q_DISABLE_COPY(SifUiDialogTitleWidget)
    QString defaultTitle(SifUiDeviceDialogType type, SifUiInstallingPhase phase);
    void updateTitle(const QVariantMap &parameters);
    void updateCertificates(const QVariantMap &parameters);
    QList<SifUiDialogCertificateInfo*> *getCertificates(QByteArray array);

private slots:
    void certificatesClicked();

private:    // data
    QGraphicsLinearLayout *mLayout;
    HbLabel *mTitle;
    HbPushButton *mCertButton;
    QList<SifUiDialogCertificateInfo*> *mCertificates;
    SifUiDialogCertificateDetails *mDetailsDialog;
};

#endif // SIFUIDIALOGTITLEWIDGET_H
