/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSisxSifPluginErrorHandler collects information
*               about errors. Error details are then passed to
*               framework (USIF).
*
*/

#ifndef C_SISXSIFPLUGINERRORHANDLER_H
#define C_SISXSIFPLUGINERRORHANDLER_H

#include <e32base.h>                    // CBase
#include <usif/usiferror.h>             // TErrorCategory

namespace Usif
{
    class COpaqueNamedParams;


    /**
     *  Native SISX installation error class.
     */
    class CSisxSifPluginErrorHandler : public CBase
        {
        public:     // constructor and destructor
            static CSisxSifPluginErrorHandler* NewL();
            ~CSisxSifPluginErrorHandler();

        public:     // new functions
            TErrorCategory  ErrorCategory() const;
            void FillOutputParamsL( COpaqueNamedParams& aOutputParams ) const;

            TInt ErrorCode() const;
            void SetErrorCode( TInt aErrorCode );
            TInt ExtendedErrorCode() const;
            void SetExtendedErrorCode( TInt aExtendedErrorCode );
            const TDesC& ErrorMessage() const;
            void SetErrorMessage( const TDesC& aErrorMessage );
            const TDesC& ErrorMessageDetails() const;
            void SetErrorMessageDetails( const TDesC& aErrorMessageDetails );

        private:    // new functions
            CSisxSifPluginErrorHandler();
            void ConstructL();

        private:    // data
            TInt iErrorCode;
            TInt iExtendedErrorCode;
            HBufC* iErrorMessage;
            HBufC* iErrorMessageDetails;
        };

}   // namespace Usif

#endif // C_SISXSIFPLUGINERRORHANDLER_H
