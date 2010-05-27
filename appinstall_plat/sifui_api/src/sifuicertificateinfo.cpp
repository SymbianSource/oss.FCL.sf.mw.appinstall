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
* Description:  CSifUiCertificateInfo for certificate details.
*
*/

#include "sifuicertificateinfo.h"               // CSifUiCertificateInfo
#include <swi/msisuihandlers.h>                 // Swi::CCertificateInfo


// ======== LOCAL FUNCTIONS ========

void WriteBufToStreamL( RWriteStream& aStream, const TDesC& aBuffer )
    {
    aStream.WriteInt32L( aBuffer.Length() );
    aStream.WriteL( aBuffer );
    }

void WriteBufToStreamL( RWriteStream& aStream, const TDesC8& aBuffer )
    {
    aStream.WriteInt32L( aBuffer.Length() );
    aStream.WriteL( aBuffer );
    }

void WriteDateTimeToStreamL( RWriteStream& aStream, const TDateTime& aDateTime )
    {
    TPckg<TDateTime> buffer( aDateTime );
    aStream.WriteL( buffer );
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSifUiCertificateInfo::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CSifUiCertificateInfo* CSifUiCertificateInfo::NewLC(
        Swi::CCertificateInfo& aCertInfo )
    {
    CSifUiCertificateInfo* self = new( ELeave ) CSifUiCertificateInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aCertInfo );
    return self;
    }

// ---------------------------------------------------------------------------
// CSifUiCertificateInfo::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CSifUiCertificateInfo* CSifUiCertificateInfo::NewL(
        Swi::CCertificateInfo& aCertInfo )
    {
    CSifUiCertificateInfo* self = CSifUiCertificateInfo::NewLC( aCertInfo );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSifUiCertificateInfo::~CSifUiCertificateInfo()
// ---------------------------------------------------------------------------
//
CSifUiCertificateInfo::~CSifUiCertificateInfo()
    {
    delete iSubjectName;
    delete iIssuerName;
    delete iFingerprint;
    delete iSerialNumber;
    }

// ---------------------------------------------------------------------------
// CSifUiCertificateInfo::ExternalizeL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUiCertificateInfo::ExternalizeL( RWriteStream& aStream ) const
    {
    WriteBufToStreamL( aStream, SubjectName() );
    WriteBufToStreamL( aStream, IssuerName() );
    WriteBufToStreamL( aStream, Fingerprint() );
    WriteBufToStreamL( aStream, SerialNumber() );
    WriteDateTimeToStreamL( aStream, iValidFrom );
    WriteDateTimeToStreamL( aStream, iValidTo );
    }

// ---------------------------------------------------------------------------
// CSifUiCertificateInfo::CSifUiCertificateInfo()
// ---------------------------------------------------------------------------
//
CSifUiCertificateInfo::CSifUiCertificateInfo()
    {
    }

// ---------------------------------------------------------------------------
// CSifUiCertificateInfo::ConstructL()
// ---------------------------------------------------------------------------
//
void CSifUiCertificateInfo::ConstructL(
        Swi::CCertificateInfo& aCertInfo )
    {
    iSubjectName = aCertInfo.SubjectName().AllocL();
    iIssuerName = aCertInfo.IssuerName().AllocL();
    iFingerprint = aCertInfo.Fingerprint().AllocL();
    iSerialNumber = aCertInfo.SerialNumber().AllocL();
    iValidFrom = aCertInfo.ValidFrom();
    iValidTo = aCertInfo.ValidTo();
    }

// ---------------------------------------------------------------------------
// CSifUiCertificateInfo::SubjectName()
// ---------------------------------------------------------------------------
//
const TDesC& CSifUiCertificateInfo::SubjectName() const
    {
    if( iSubjectName )
        {
        return *iSubjectName;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CSifUiCertificateInfo::IssuerName()
// ---------------------------------------------------------------------------
//
const TDesC& CSifUiCertificateInfo::IssuerName() const
    {
    if( iIssuerName )
        {
        return *iIssuerName;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CSifUiCertificateInfo::Fingerprint()
// ---------------------------------------------------------------------------
//
const TDesC8& CSifUiCertificateInfo::Fingerprint() const
    {
    if( iFingerprint )
        {
        return *iFingerprint;
        }
    return KNullDesC8;
    }

// ---------------------------------------------------------------------------
// CSifUiCertificateInfo::SerialNumber()
// ---------------------------------------------------------------------------
//
const TDesC8& CSifUiCertificateInfo::SerialNumber() const
    {
    if( iSerialNumber )
        {
        return *iSerialNumber;
        }
    return KNullDesC8;
    }

