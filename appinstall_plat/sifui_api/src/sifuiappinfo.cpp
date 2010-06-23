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

#include "sifuiappinfo.h"           // CSifUiAppInfo
#include <s32strm.h>                // RWriteStream
#include <apgicnfl.h>               // CApaMaskedBitmap


// in sifuicertificateinfo.cpp:
void WriteBufToStreamL( RWriteStream& aStream, const TDesC& aBuffer );


// ======== LOCAL FUNCTIONS ========

void WriteVersionToStreamL( RWriteStream& aStream, const TVersion& aVersion )
    {
    TPckg<TVersion> buffer( aVersion );
    aStream.WriteL( buffer );
    }

void WriteBitmapsToStreamL( RWriteStream& /*aStream*/, const CApaMaskedBitmap* /*aIcon*/ )
    {
    // TODO: implement
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSifUiAppInfo::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CSifUiAppInfo* CSifUiAppInfo::NewLC( const TDesC& aAppName,
        const TDesC& aAppVendor, const TVersion& aAppVersion, TInt aAppSize,
        const CApaMaskedBitmap* aAppIcon )
    {
    CSifUiAppInfo* self = new ( ELeave ) CSifUiAppInfo;
    CleanupStack::PushL( self );
    self->ConstructL( aAppName, aAppVendor, aAppVersion, aAppSize, aAppIcon );
    return self;
    }

// ---------------------------------------------------------------------------
// CSifUiAppInfo::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CSifUiAppInfo* CSifUiAppInfo::NewL( const TDesC& aAppName,
        const TDesC& aAppVendor, const TVersion& aAppVersion, TInt aAppSize,
        const CApaMaskedBitmap* aAppIcon )
    {
    CSifUiAppInfo* self = CSifUiAppInfo::NewLC( aAppName, aAppVendor,
            aAppVersion, aAppSize, aAppIcon );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSifUiAppInfo::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CSifUiAppInfo* CSifUiAppInfo::NewL( const CSifUiAppInfo& aAppInfo )
    {
    CSifUiAppInfo* self = new ( ELeave ) CSifUiAppInfo;
    CleanupStack::PushL( self );
    self->ConstructL( aAppInfo );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSifUiAppInfo::~CSifUiAppInfo()
// ---------------------------------------------------------------------------
//
CSifUiAppInfo::~CSifUiAppInfo()
    {
    delete iAppName;
    delete iAppVendor;
    delete iAppIcon;
    }

// ---------------------------------------------------------------------------
// CSifUiAppInfo::Name()
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CSifUiAppInfo::Name() const
    {
    if( iAppName )
        {
        return *iAppName;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CSifUiAppInfo::Vendor()
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CSifUiAppInfo::Vendor() const
    {
    if( iAppVendor )
        {
        return *iAppVendor;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CSifUiAppInfo::Version()
// ---------------------------------------------------------------------------
//
EXPORT_C const TVersion& CSifUiAppInfo::Version() const
    {
    return iAppVersion;
    }

// ---------------------------------------------------------------------------
// CSifUiAppInfo::Size()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSifUiAppInfo::Size() const
    {
    return iAppSize;
    }

// ---------------------------------------------------------------------------
// CSifUiAppInfo::Bitmaps()
// ---------------------------------------------------------------------------
//
EXPORT_C const CApaMaskedBitmap* CSifUiAppInfo::Bitmaps() const
    {
    return iAppIcon;
    }

// ---------------------------------------------------------------------------
// CSifUiAppInfo::ExternalizeL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUiAppInfo::ExternalizeL( RWriteStream& aStream ) const
    {
    WriteBufToStreamL( aStream, *iAppName );
    WriteBufToStreamL( aStream, *iAppVendor );
    WriteVersionToStreamL( aStream, iAppVersion );
    aStream.WriteInt32L( iAppSize );
    WriteBitmapsToStreamL( aStream, iAppIcon );
    }

// ---------------------------------------------------------------------------
// CSifUiAppInfo::CSifUiAppInfo()
// ---------------------------------------------------------------------------
//
CSifUiAppInfo::CSifUiAppInfo()
    {
    }

// ---------------------------------------------------------------------------
// CSifUiAppInfo::ConstructL()
// ---------------------------------------------------------------------------
//
void CSifUiAppInfo::ConstructL( const TDesC& aAppName, const TDesC& aAppVendor,
        const TVersion& aAppVersion, TInt aAppSize, const CApaMaskedBitmap* aAppIcon )
    {
    iAppName = aAppName.AllocL();
    iAppVendor = aAppVendor.AllocL();
    iAppVersion = aAppVersion;
    iAppSize = aAppSize;
    if( aAppIcon )
        {
        iAppIcon = CApaMaskedBitmap::NewL( aAppIcon );
        }
    }

// ---------------------------------------------------------------------------
// CSifUiAppInfo::ConstructL()
// ---------------------------------------------------------------------------
//
void CSifUiAppInfo::ConstructL( const CSifUiAppInfo& aAppInfo )
    {
    iAppName = aAppInfo.Name().AllocL();
    iAppVendor = aAppInfo.Vendor().AllocL();
    iAppVersion = aAppInfo.iAppVersion;
    iAppSize = aAppInfo.iAppSize;
    if( aAppInfo.iAppIcon )
        {
        iAppIcon = CApaMaskedBitmap::NewL( aAppInfo.iAppIcon );
        }
    }

