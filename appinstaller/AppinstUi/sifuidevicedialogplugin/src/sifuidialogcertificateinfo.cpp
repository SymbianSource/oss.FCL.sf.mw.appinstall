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

#include "sifuidialogcertificateinfo.h"

const char *KHexNumberFormatSimple = "%X";
const char *KHexNumberFormatTwoDigitsWithLeadingZeroes = "%02X";
const int KCharsPerBlock = 2;
const char KBlockSeparator = ' ';


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// SifUiDialogCertificateInfo::SifUiDialogCertificateInfo()
// ----------------------------------------------------------------------------
//
SifUiDialogCertificateInfo::SifUiDialogCertificateInfo(const QString &subject,
    const QString &issuer, const QByteArray &fingerprint, const QByteArray &serialNumber,
    const QDateTime &validFrom, const QDateTime &validTo) :
    mSubjectName(subject), mIssuerName(issuer), mFingerprint(fingerprint),
    mSerialNumber(serialNumber), mValidFrom(validFrom), mValidTo(validTo)
{
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateInfo::~SifUiDialogCertificateInfo()
// ----------------------------------------------------------------------------
//
SifUiDialogCertificateInfo::~SifUiDialogCertificateInfo()
{
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateInfo::subjectName()
// ----------------------------------------------------------------------------
//
const QString SifUiDialogCertificateInfo::subjectName() const
{
    return mSubjectName;
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateInfo::issuerName()
// ----------------------------------------------------------------------------
//
const QString SifUiDialogCertificateInfo::issuerName() const
{
    return mIssuerName;
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateInfo::fingerprint()
// ----------------------------------------------------------------------------
//
const QByteArray SifUiDialogCertificateInfo::fingerprint() const
{
    return mFingerprint;
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateInfo::formattedFingerprint()
// ----------------------------------------------------------------------------
//
const QString SifUiDialogCertificateInfo::formattedFingerprint() const
{
    QString formatted;

    QString number;
    int blockIndex = 0;
    int count = mFingerprint.count();
    for (int index = 0; index < count; ++index) {
        if (blockIndex == KCharsPerBlock) {
            formatted.append(KBlockSeparator);
            blockIndex = 0;
        }
        number.sprintf(KHexNumberFormatTwoDigitsWithLeadingZeroes,
            static_cast<unsigned char>(mFingerprint.at(index)));
        formatted.append(number);
        ++blockIndex;
    }

    return formatted;
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateInfo::serialNumber()
// ----------------------------------------------------------------------------
//
const QByteArray SifUiDialogCertificateInfo::serialNumber() const
{
    return mSerialNumber;
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateInfo::formattedSerialNumber()
// ----------------------------------------------------------------------------
//
const QString SifUiDialogCertificateInfo::formattedSerialNumber() const
{
    QString formatted;

    QString number;
    int count = mSerialNumber.count();
    for (int index = 0; index < count; ++index) {
        number.sprintf(KHexNumberFormatSimple,
            static_cast<unsigned char>(mSerialNumber.at(index)));
        formatted.append(number);
    }

    return formatted;
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateInfo::validFrom()
// ----------------------------------------------------------------------------
//
const QDateTime SifUiDialogCertificateInfo::validFrom() const
{
    return mValidFrom;
}

// ----------------------------------------------------------------------------
// SifUiDialogCertificateInfo::validTo()
// ----------------------------------------------------------------------------
//
const QDateTime SifUiDialogCertificateInfo::validTo() const
{
    return mValidTo;
}

