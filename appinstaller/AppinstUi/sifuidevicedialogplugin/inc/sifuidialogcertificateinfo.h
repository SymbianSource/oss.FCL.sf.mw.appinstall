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
* Description: Certificate info class for SIF UI device dialog plugin.
*
*/

#ifndef SIFUIDIALOGCERTIFICATEINFO_H
#define SIFUIDIALOGCERTIFICATEINFO_H

#include <QDateTime>

/**
 * Certificate info class for SifUi device dialogs.
 * This class corresponds to Symbian-side CSifUiCertificateInfo class.
 */
class SifUiDialogCertificateInfo
{
public:     // constructor and destructor
    SifUiDialogCertificateInfo(const QString &subject, const QString &issuer,
        const QByteArray &fingerprint, const QByteArray &serialNumber,
        const QDateTime &validFrom, const QDateTime &validTo);
    virtual ~SifUiDialogCertificateInfo();

public:     // new functions
    const QString subjectName() const;
    const QString issuerName() const;
    const QByteArray fingerprint() const;
    const QString formattedFingerprint() const;
    const QByteArray serialNumber() const;
    const QString formattedSerialNumber() const;
    const QDateTime validFrom() const;
    const QDateTime validTo() const;

private:
    QString mSubjectName;
    QString mIssuerName;
    QByteArray mFingerprint;
    QByteArray mSerialNumber;
    QDateTime mValidFrom;
    QDateTime mValidTo;
};

#endif // SIFUIDIALOGCERTIFICATEINFO_H
