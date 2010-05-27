/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of CNcdPurchaseInstallInfo
*
*/


#include <s32strm.h>

#include "ncdutils.h"
#include "catalogsutils.h"

EXPORT_C CNcdPurchaseInstallInfo* CNcdPurchaseInstallInfo::NewL()
    {
    CNcdPurchaseInstallInfo* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CNcdPurchaseInstallInfo* CNcdPurchaseInstallInfo::NewLC()
    {
    CNcdPurchaseInstallInfo* self =
        new (ELeave) CNcdPurchaseInstallInfo;
    CleanupStack::PushL( self );
    self->BaseConstructL();
    return self;
    }

EXPORT_C CNcdPurchaseInstallInfo* CNcdPurchaseInstallInfo::NewL( 
    const MNcdPurchaseInstallInfo& aSource )
    {
    CNcdPurchaseInstallInfo* self = NewLC( aSource );
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CNcdPurchaseInstallInfo* CNcdPurchaseInstallInfo::NewLC(
    const MNcdPurchaseInstallInfo& aSource )
    {
    CNcdPurchaseInstallInfo* self =
         new (ELeave) CNcdPurchaseInstallInfo;
    CleanupStack::PushL( self );
    self->BaseConstructL( aSource );
    return self;
    }

EXPORT_C CNcdPurchaseInstallInfo::~CNcdPurchaseInstallInfo()
    {
    delete iFilename;
    delete iApplicationVersion;
    delete iThemeName;
    }

EXPORT_C void CNcdPurchaseInstallInfo::ExternalizeL( RWriteStream& aStream ) const
    {
    ExternalizeDesL( *iFilename, aStream );
    aStream.WriteInt32L( iApplicationUid.iUid );
    ExternalizeDesL( *iApplicationVersion, aStream );
    ExternalizeDesL( *iThemeName, aStream );
    }

EXPORT_C void CNcdPurchaseInstallInfo::InternalizeL( RReadStream& aStream )
    {
    InternalizeDesL( iFilename, aStream );
    iApplicationUid = TUid::Uid( aStream.ReadInt32L() );
    InternalizeDesL( iApplicationVersion, aStream );
    InternalizeDesL( iThemeName, aStream );
    }

EXPORT_C const TDesC& CNcdPurchaseInstallInfo::Filename() const
    {
    return *iFilename;
    }
    
EXPORT_C void CNcdPurchaseInstallInfo::SetFilenameL( const TDesC& aFilename )
    {
    AssignDesL( iFilename, aFilename );
    }
    
EXPORT_C TUid CNcdPurchaseInstallInfo::ApplicationUid() const
    {
    return iApplicationUid;
    }
    
EXPORT_C void CNcdPurchaseInstallInfo::SetApplicationUid( 
    const TUid aApplicationUid )
    {
    iApplicationUid = aApplicationUid;
    }

EXPORT_C const TDesC& CNcdPurchaseInstallInfo::ApplicationVersion() const
    {
    return *iApplicationVersion;
    }
    
EXPORT_C void CNcdPurchaseInstallInfo::SetApplicationVersionL( 
    const TDesC& aApplicationVersion )
    {
    AssignDesL( iApplicationVersion, aApplicationVersion );
    }
    
EXPORT_C const TDesC& CNcdPurchaseInstallInfo::ThemeName() const
    {
    return *iThemeName;
    }
    
EXPORT_C void CNcdPurchaseInstallInfo::SetThemeNameL( 
    const TDesC& aThemeName )
    {
    AssignDesL( iThemeName, aThemeName );
    }

EXPORT_C void CNcdPurchaseInstallInfo::BaseConstructL()
    {
    iFilename = KNullDesC().AllocL();
    iApplicationUid = TUid::Uid( 0 );
    iApplicationVersion = KNullDesC().AllocL();
    iThemeName = KNullDesC().AllocL();
    }


EXPORT_C void CNcdPurchaseInstallInfo::BaseConstructL(
    const MNcdPurchaseInstallInfo& aSource )
    {
    SetFilenameL( aSource.Filename() );
    SetApplicationUid( aSource.ApplicationUid() );
    SetApplicationVersionL( aSource.ApplicationVersion() );
    SetThemeNameL( aSource.ThemeName() );    
    }

EXPORT_C CNcdPurchaseInstallInfo::CNcdPurchaseInstallInfo()
    {
    }
