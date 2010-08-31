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
* Description:  CSifUiErrorInfo for installation error details
*
*/

#ifndef C_SIFUIERRORINFO_H
#define C_SIFUIERRORINFO_H

#include <e32base.h>                            // CBase
#include <usif/usiferror.h>                     // Usif::TErrorCategory


/**
 * CSifUiErrorInfo is a container class for installation error details.
 *
 * @lib sifui.lib
 * @since 10.1
 */
class CSifUiErrorInfo : public CBase
    {
    public:     // constructors and destructor
        /**
         * Creates new CSifUiErrorInfo object and pushes it to cleanup stack.
         * @param aErrorCategory - error category
         * @param aErrorCode - error code
         * @param aExtendedErrorCode - extended error code
         * @param aErrorMessage - error message
         * @param aErrorMessageDetails - detailed error message
         * @returns CSifUiErrorInfo* -- new CSifUiErrorInfo object
         */
        IMPORT_C static CSifUiErrorInfo* NewLC( Usif::TErrorCategory aErrorCategory,
                TInt aErrorCode, TInt aExtendedErrorCode,
                const TDesC& aErrorMessage, const TDesC& aErrorMessageDetails );

        /**
         * Creates new CSifUiErrorInfo object.
         * @param aErrorCategory - error category
         * @param aErrorCode - error code
         * @param aExtendedErrorCode - extended error code
         * @param aErrorMessage - error message
         * @param aErrorMessageDetails - detailed error message
         * @returns CSifUiErrorInfo* -- new CSifUiErrorInfo object
         */
        IMPORT_C static CSifUiErrorInfo* NewL( Usif::TErrorCategory aErrorCategory,
                TInt aErrorCode, TInt aExtendedErrorCode,
                const TDesC& aErrorMessage, const TDesC& aErrorMessageDetails );

        /**
         * Destructor.
         */
        ~CSifUiErrorInfo();

    public:     // new functions

        /**
         * Error category.
         * @returns Usif::TErrorCategory - error category
         */
        inline Usif::TErrorCategory ErrorCategory() const;

        /**
         * Error code.
         * @returns TInt - error code
         */
        inline TInt ErrorCode() const;

        /**
         * Extended error code.
         * @returns TInt - extended error code
         */
        inline TInt ExtendedErrorCode() const;

        /**
         * Error message.
         * @returns const TDesC& - error message
         */
        inline const TDesC& ErrorMessage() const;

        /**
         * Error message details.
         * @returns const TDesC& - additional error message details
         */
        inline const TDesC& ErrorMessageDetails() const;

    private:    // new functions
        CSifUiErrorInfo();
        void ConstructL( Usif::TErrorCategory aErrorCategory,
                TInt aErrorCode, TInt aExtendedErrorCode,
                const TDesC& aErrorMessage, const TDesC& aErrorMessageDetails );

    private:    // data
        Usif::TErrorCategory iErrorCategory;
        TInt iErrorCode;
        TInt iExtendedErrorCode;
        HBufC* iErrorMessage;
        HBufC* iErrorMessageDetails;
    };


#include "sifuierrorinfo.inl"

#endif  // C_SIFUIERRORINFO_H

