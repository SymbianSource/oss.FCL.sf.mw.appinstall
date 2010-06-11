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
* Description: SIF UI dialog title widget (SymbianOS specific part).
*
*/

#include "sifuidialogtitlewidget.h"
#include "sifuidialogcertificateinfo.h"     // SifUiDialogCertificateInfo
#include <s32mem.h>                         // RDesReadStream


// ======== LOCAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// readStringFromStreamL()
// ----------------------------------------------------------------------------
//
QString readStringFromStreamL( RDesReadStream& aReadStream )
{
    TInt length = aReadStream.ReadInt32L();
    HBufC* buf = HBufC::NewLC( length );
    TPtr ptr( buf->Des() );
    aReadStream.ReadL( ptr, length );
    QString string = QString::fromUtf16( ptr.Ptr(), length );
    CleanupStack::PopAndDestroy( buf );
    return string;
}

// ----------------------------------------------------------------------------
// readByteArrayFromStreamL()
// ----------------------------------------------------------------------------
//
QByteArray readByteArrayFromStreamL( RDesReadStream& aReadStream )
{
    TInt length = aReadStream.ReadInt32L();
    HBufC8* buf = HBufC8::NewLC( length );
    TPtr8 ptr( buf->Des() );
    aReadStream.ReadL( ptr, length );
    QByteArray byteArray( reinterpret_cast< const char * >( ptr.Ptr() ), length );
    CleanupStack::PopAndDestroy( buf );
    return byteArray;
}

// ----------------------------------------------------------------------------
// readDateFromStreamL()
// ----------------------------------------------------------------------------
//
void readDateFromStreamL( RDesReadStream& aReadStream, QDateTime& aDateTime )
{
    TDateTime dateTimeSymbian;
    TPckg<TDateTime> dateTimePckg( dateTimeSymbian );
    aReadStream.ReadL( dateTimePckg );
    QDate date( dateTimeSymbian.Year(), dateTimeSymbian.Month()+1, dateTimeSymbian.Day()+1 );
    aDateTime.setDate( date );
    QTime time( dateTimeSymbian.Hour(), dateTimeSymbian.Minute(), dateTimeSymbian.Second() );
    aDateTime.setTime( time );
}

// ----------------------------------------------------------------------------
// doGetCertificatesL()
// ----------------------------------------------------------------------------
//
QList<SifUiDialogCertificateInfo*> *doGetCertificatesL( QByteArray aArray )
{
    QList<SifUiDialogCertificateInfo*> *list = new QList<SifUiDialogCertificateInfo*>;

    const TPtrC8 ptr8( reinterpret_cast< const TText8* >( aArray.constData() ), aArray.size() );
    RDesReadStream readStream;
    CleanupClosePushL( readStream );
    readStream.Open( ptr8 );

    TInt certCount = readStream.ReadInt32L();
    for( TInt index = 0; index < certCount; ++index )
        {
        QString subjectName = readStringFromStreamL( readStream );
        QString issuerName = readStringFromStreamL( readStream );
        QByteArray fingerprint = readByteArrayFromStreamL( readStream );
        QByteArray serialNumber = readByteArrayFromStreamL( readStream );
        QDateTime validFrom;
        readDateFromStreamL( readStream, validFrom );
        QDateTime validTo;
        readDateFromStreamL( readStream, validTo );

        SifUiDialogCertificateInfo *certInfo = new SifUiDialogCertificateInfo(
            subjectName, issuerName, fingerprint, serialNumber, validFrom, validTo );
        list->append( certInfo );
        }

    CleanupStack::PopAndDestroy( &readStream );
    return list;
}


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// SifUiDialogTitleWidget::SifUiDialogTitleWidget()
// ----------------------------------------------------------------------------
//
QList<SifUiDialogCertificateInfo*> *SifUiDialogTitleWidget::getCertificates(
    QByteArray array)
{
    QList<SifUiDialogCertificateInfo*> *list = 0;
    QT_TRAP_THROWING(list = doGetCertificatesL(array));
    return list;
}

