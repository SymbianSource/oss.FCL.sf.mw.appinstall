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
* Description:   This file contains the header file of the CSisxUICertificateInfo 
*                class.
*
*                This class implements the CCUICertificateInfo for Sisx 
*                implementation.
*
*/


#ifndef SISXUICERTIFICATEINFO_H
#define SISXUICERTIFICATEINFO_H

//  INCLUDES
#include <e32base.h>
#include <CUIDetailsDialog.h>

//  FORWARD DECLARATIONS
namespace Swi
{
class CCertificateInfo;
}

namespace SwiUI
{

//  FORWARD DECLARATIONS

/**
* This class implements the CCUICertificateInfo for Sisx implementation.
*
* @lib swinstcommonui.lib
* @since 3.0
*/
class CSisxUICertificateInfo : public CommonUI::CCUICertificateInfo
    {
    public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        */
        static CSisxUICertificateInfo* NewL( const Swi::CCertificateInfo& aCertInfo );
    
        /**
        * Destructor.
        */
        virtual ~CSisxUICertificateInfo();

    public:  // Functions from base classes

        /**
        * From CCUICertificateInfo, Gets the subject name of the certificate.
        * @since 3.0
        * @return The subject name of the certificate.
        */
	const TDesC& SubjectNameL() const;

	/**
        * From CCUICertificateInfo, Gets the issuer name of the certificate.
        * @since 3.0
        * @return The issuer name of the certificate.
        */
	const TDesC& IssuerNameL() const;

	/**
        * From CCUICertificateInfo, Gets the date the certificate is valid from.
        * @since 3.0
        * @return The date the certificate is valid from.
        */
	TDateTime ValidFromL() const;

	/**
        * From CCUICertificateInfo, Gets the date the certificate is valid until.
        * @since 3.0
        * @return The date the certificate is valid until.
        */
	TDateTime ValidToL() const;

	/**
        * From CCUICertificateInfo, Gets the subject name of the certificate.
        * @since 3.0
        * @return The subject name of the certificate.
        */
	const TDesC8& FingerprintL() const;

	/**
        * From CCUICertificateInfo, Gets the subject name of the certificate.
        * @since 3.0
        * @return The subject name of the certificate.
        */
	const TDesC8& SerialNumberL() const;

        /**
        * From CCUICertificateInfo, Indicates wheter this certificate is self signed.
        * @since 3.0
        * @return ETrue if is self signed.
        */
        TBool IsSelfSignedL() const;      

        /**
        * From CCUICertificateInfo, Gets the full encoding of this certificate.
        * @since 3.0
        * @return Encoding.
        */
        const TDesC8& EncodingL() const;

        /**
        * From CCUICertificateInfo, Gets the signature of this certificate.
        * @since 3.0
        * @return Signature.
        */
        const TDesC8& SignatureL() const;

        /**
        * From CCUICertificateInfo, Gets the public key algorithm of the certificate.
        * @since 3.0
        * @return Public key algorithm.
        */
        TAlgorithmId PublicKeyAlgorithmL() const;

        /**
        * From CCUICertificateInfo, Gets the digest algorithm of the certificate.
        * @since 3.0
        * @return digest algorithm.
        */
        TAlgorithmId DigestAlgorithmL() const;
        
    protected:
        
        /**
        * C++ default constructor.
        */
        CSisxUICertificateInfo( const Swi::CCertificateInfo& aCertInfo );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private:  // Data

        const Swi::CCertificateInfo& iCertInfo;
    };
}

#endif      // SISXUICERTIFICATEINFO_H
            
// End of File
