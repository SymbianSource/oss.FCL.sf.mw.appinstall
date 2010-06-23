/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CSisxUICertificateInfo
*                class member functions.
*
*/


// INCLUDE FILES
#include "SisxUICertificateInfo.h"
#include <swi/msisuihandlers.h>


using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSisxUICertificateInfo::CSisxUICertificateInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSisxUICertificateInfo::CSisxUICertificateInfo( const Swi::CCertificateInfo& aCertInfo )
    : iCertInfo( aCertInfo )
    {  
    }

// -----------------------------------------------------------------------------
// CSisxUICertificateInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSisxUICertificateInfo::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSisxUICertificateInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSisxUICertificateInfo* CSisxUICertificateInfo::NewL( 
    const Swi::CCertificateInfo& aCertInfo )
    {
    CSisxUICertificateInfo* self = new( ELeave ) CSisxUICertificateInfo( aCertInfo );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
// Destructor
CSisxUICertificateInfo::~CSisxUICertificateInfo()
    {
    }

// -----------------------------------------------------------------------------
// CSisxUICertificateInfo::SubjectName
// Getter for subject field.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CSisxUICertificateInfo::SubjectNameL() const
    {
    return iCertInfo.SubjectName();    
    }

// -----------------------------------------------------------------------------
// CSisxUICertificateInfo::IssuerName
// Getter for issuer field.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CSisxUICertificateInfo::IssuerNameL() const
    {
    return iCertInfo.IssuerName();    
    }

// -----------------------------------------------------------------------------
// CSisxUICertificateInfo::ValidFrom
// Getter for valid from field.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDateTime CSisxUICertificateInfo::ValidFromL() const
    {
    return iCertInfo.ValidFrom();    
    }

// -----------------------------------------------------------------------------
// CSisxUICertificateInfo::ValidTo
// Getter for certificate expiration date.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDateTime CSisxUICertificateInfo::ValidToL() const
    {
    return iCertInfo.ValidTo();    
    }

// -----------------------------------------------------------------------------
// CSisxUICertificateInfo::Fingerprint
// Getter for fingerprint of the certificate.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& CSisxUICertificateInfo::FingerprintL() const
    {
    return iCertInfo.Fingerprint();    
    }

// -----------------------------------------------------------------------------
// CSisxUICertificateInfo::SerialNumber
// Getter for serial number of the certificate.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& CSisxUICertificateInfo::SerialNumberL() const
    {
    return iCertInfo.SerialNumber();
    }

// -----------------------------------------------------------------------------
// CSisxUICertificateInfo::IsSelfSignedL
// Indicates wheter this certificate is self signed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUICertificateInfo::IsSelfSignedL() const
    {
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// CSisxUICertificateInfo::EncodingL
// Gets the full encoding of this certificate.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& CSisxUICertificateInfo::EncodingL() const
    {
    return KNullDesC8();    
    }

// -----------------------------------------------------------------------------
// CSisxUICertificateInfo::SignatureL
// Gets the signature of this certificate.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& CSisxUICertificateInfo::SignatureL() const
    {
    return KNullDesC8();    
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::PublicKeyAlgorithmL
// Gets the public key algorithm of the certificate.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAlgorithmId CSisxUICertificateInfo::PublicKeyAlgorithmL() const
    {
    return TAlgorithmId( -1 );    
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::DigestAlgorithmL
// Gets the digest algorithm of the certificate.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAlgorithmId CSisxUICertificateInfo::DigestAlgorithmL() const
    {
    return TAlgorithmId( -1 );    
    }

//  End of File  
