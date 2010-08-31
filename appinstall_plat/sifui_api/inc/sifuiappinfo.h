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
* Description:  CSifUiAppInfo for application details
*
*/

#ifndef C_SIFUIAPPINFO_H
#define C_SIFUIAPPINFO_H

#include <e32base.h>                            // CBase

class RWriteStream;
class CApaMaskedBitmap;


/**
 * CSifUiAppInfo transfers application details from a SIF plugin
 * to UI process in device dialog server. See also Swi::CAppInfo.
 *
 * @lib sifui.lib
 * @since 10.1
 */
class CSifUiAppInfo : public CBase
    {
    public:     // constructors and destructor
        /**
         * Creates new CSifUiAppInfo object and pushes it to cleanup stack.
         * Does not take the ownership of aAppIcon parameter.
         * @param aAppName - application name
         * @param aAppVendor - vendor name
         * @param aAppVersion - application version
         * @param aAppSize - application size in bytes
         * @param aAppIcon - application icon (or NULL if no icon)
         * @returns CSifUiAppInfo* -- new CSifUiAppInfo object
         */
        IMPORT_C static CSifUiAppInfo* NewLC( const TDesC& aAppName,
                const TDesC& aAppVendor, const TVersion& aAppVersion, TInt aAppSize,
                const CApaMaskedBitmap* aAppIcon );

        /**
         * Creates new CSifUiAppInfo object.
         * Does not take the ownership of aAppIcon parameter.
         * @param aAppName - application name
         * @param aAppVendor - vendor name
         * @param aAppVersion - application version
         * @param aAppSize - application size in bytes
         * @param aAppIcon - application icon (or NULL if no icon)
         * @returns CSifUiAppInfo* -- new CSifUiAppInfo object
         */
        IMPORT_C static CSifUiAppInfo* NewL( const TDesC& aAppName,
                const TDesC& aAppVendor, const TVersion& aAppVersion, TInt aAppSize,
                const CApaMaskedBitmap* aAppIcon );

        /**
         * Creates new CSifUiAppInfo object by copying existing object.
         * @param aAppInfo - application info to copy
         * @returns CSifUiAppInfo* -- new CSifUiAppInfo object
         */
        IMPORT_C static CSifUiAppInfo* NewL( const CSifUiAppInfo& aAppInfo );

        /**
         * Destructor.
         */
        ~CSifUiAppInfo();

    public:     // new functions

        /**
         * Application name.
         * @returns TDesC - application name
         */
        IMPORT_C const TDesC& Name() const;

        /**
         * Application vendor.
         * @returns TDesC - vendor name
         */
        IMPORT_C const TDesC& Vendor() const;

        /**
         * Application version.
         * @returns TVersion - application version
         */
        IMPORT_C const TVersion& Version() const;

        /**
         * Application size.
         * @returns TInt - application size
         */
        IMPORT_C TInt Size() const;

        /**
         * Application icon and mask bitmaps. Does not transfer ownership.
         * @returns CApaMaskedBitmap - icon and mask bitmaps, NULL if no bitmaps
         */
        IMPORT_C const CApaMaskedBitmap* Bitmaps() const;

        /**
         * Externalizes this object to the specified write stream.
         * @param aStream   Stream to which the contents is written.
         */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

    private:    // new functions
        CSifUiAppInfo();
        void ConstructL( const TDesC& aAppName, const TDesC& aAppVendor,
                const TVersion& aAppVersion, TInt aAppSize,
                const CApaMaskedBitmap* aAppIcon );
        void ConstructL( const CSifUiAppInfo& aAppInfo );

    private:    // data
        HBufC* iAppName;
        HBufC* iAppVendor;
        TVersion iAppVersion;
        TInt iAppSize;
        CApaMaskedBitmap* iAppIcon;
    };

#endif  // C_SIFUIAPPINFO_H

