/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CCUICertificateInfo
*                class member functions.
*
*/


// INCLUDE FILES
#include <X509CertNameParser.h>
#include <x509cert.h>
#include "CUIDetailsDialog.h"

using namespace SwiUI::CommonUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCUICertificateInfo::CCUICertificateInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCUICertificateInfo::CCUICertificateInfo()  
    {  
    }

// -----------------------------------------------------------------------------
// CCUICertificateDetailsDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCUICertificateInfo::BaseConstructL( const CX509Certificate& aCertificate  )
    {
    iCertificate = CX509Certificate::NewL( aCertificate ); 
    X509CertNameParser::SubjectFullNameL( *iCertificate, iSubject );
    X509CertNameParser::IssuerFullNameL( *iCertificate, iIssuer ); 
    iSerialNumber = iCertificate->SerialNumber().AllocL();    
    iFingerprint = iCertificate->Fingerprint().AllocL();    
    iEncoding = iCertificate->Encoding().AllocL(); 
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCUICertificateInfo* CCUICertificateInfo::NewL( const CX509Certificate& aCertificate )     
    {
    CCUICertificateInfo* self = new ( ELeave ) CCUICertificateInfo();
    CleanupStack::PushL( self );
    self->BaseConstructL( aCertificate );
    CleanupStack::Pop( self );
    return self; 
    }
    
// Destructor
EXPORT_C CCUICertificateInfo::~CCUICertificateInfo()
    {    
    delete iCertificate;    
    delete iIssuer;
    delete iSubject;    
    delete iSerialNumber;
    delete iFingerprint;    
    delete iEncoding; 
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::X509Certificate
// Returns pointer to X509Certificate where this info is based on.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CX509Certificate* CCUICertificateInfo::X509Certificate()
    {
    return iCertificate;    
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::SubjectName
// Getter for subject field.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CCUICertificateInfo::SubjectNameL() const
    {
    return *iSubject;
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::IssuerName
// Getter for issuer field.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CCUICertificateInfo::IssuerNameL() const
    {
    return *iIssuer;    
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::ValidFrom
// Getter for valid from field.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDateTime CCUICertificateInfo::ValidFromL() const
    {
    const CValidityPeriod& validityPeriod = iCertificate->ValidityPeriod();
    return validityPeriod.Start().DateTime();
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::ValidTo
// Getter for certificate expiration date.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDateTime CCUICertificateInfo::ValidToL() const
    {
    const CValidityPeriod& validityPeriod = iCertificate->ValidityPeriod();
    return validityPeriod.Finish().DateTime();
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::Fingerprint
// Getter for fingerprint of the certificate.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& CCUICertificateInfo::FingerprintL() const
    {
    return *iFingerprint;    
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::SerialNumber
// Getter for serial number of the certificate.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& CCUICertificateInfo::SerialNumberL() const
    {
    return *iSerialNumber;    
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::IsSelfSignedL
// Indicates wheter this certificate is self signed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCUICertificateInfo::IsSelfSignedL() const
    {
    return iCertificate->IsSelfSignedL();    
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::EncodingL
// Gets the full encoding of this certificate.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& CCUICertificateInfo::EncodingL() const
    {
    return *iEncoding;    
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::SignatureL
// Gets the signature of this certificate.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& CCUICertificateInfo::SignatureL() const
    {
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::PublicKeyAlgorithmL
// Gets the public key algorithm of the certificate.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAlgorithmId CCUICertificateInfo::PublicKeyAlgorithmL() const
    {
    return iCertificate->SigningAlgorithm().AsymmetricAlgorithm().Algorithm();
    }

// -----------------------------------------------------------------------------
// CCUICertificateInfo::DigestAlgorithmL
// Gets the digest algorithm of the certificate.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAlgorithmId CCUICertificateInfo::DigestAlgorithmL() const
    {
    return iCertificate->SigningAlgorithm().DigestAlgorithm().Algorithm();
    }

//  End of File  
