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
* Description:  CSifUiCertificateInfo for certificate details
*
*/

#ifndef C_SIFUICERTIFICATEINFO_H
#define C_SIFUICERTIFICATEINFO_H

#include <e32base.h>                            // CBase

namespace Swi {
    class CCertificateInfo;
}
class RWriteStream;


/**
 * CSifUiCertificateInfo transfers certificate details from a SIF plugin
 * to UI process in device dialog server. See also Swi::CCertificateInfo.
 *
 * @lib sifui.lib
 * @since 10.1
 */
class CSifUiCertificateInfo : public CBase
    {
    public:     // constructors and destructor
        /**
         * Creates new CSifUiCertificateInfo object and pushes it to cleanup stack.
         * @returns CSifUiCertificateInfo* -- new CSifUiCertificateInfo object
         */
        IMPORT_C static CSifUiCertificateInfo* NewLC( Swi::CCertificateInfo& aCertInfo );

        /**
         * Creates new CSifUiCertificateInfo object.
         * @returns CSifUiCertificateInfo* -- new CSifUiCertificateInfo object
         */
        IMPORT_C static CSifUiCertificateInfo* NewL( Swi::CCertificateInfo& aCertInfo );

        /**
         * Destructor.
         */
        ~CSifUiCertificateInfo();

    public:     // new functions
        /**
         * Externalizes this object to the specified write stream.
         * @param aStream   Stream to which the contents is written.
         */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

    private:    // new functions
        CSifUiCertificateInfo();
        void ConstructL( Swi::CCertificateInfo& aCertInfo );
        const TDesC& SubjectName() const;
        const TDesC& IssuerName() const;
        const TDesC8& Fingerprint() const;
        const TDesC8& SerialNumber() const;

    private:    // data
        HBufC* iSubjectName;
        HBufC* iIssuerName;
        HBufC8* iFingerprint;
        HBufC8* iSerialNumber;
        TDateTime iValidFrom;
        TDateTime iValidTo;
    };

#endif  // C_SIFUICERTIFICATEINFO_H

