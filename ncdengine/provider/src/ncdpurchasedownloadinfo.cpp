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
* Description:   Implementation of CNcdPurchaseDownloadInfo
*
*/


#include <s32strm.h>

#include "ncdutils.h"
#include "catalogsutils.h"
#include "ncdattributes.h"

EXPORT_C CNcdPurchaseDownloadInfo* CNcdPurchaseDownloadInfo::NewL()
    {
    CNcdPurchaseDownloadInfo* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CNcdPurchaseDownloadInfo* CNcdPurchaseDownloadInfo::NewLC()
    {
    CNcdPurchaseDownloadInfo* self = new (ELeave) CNcdPurchaseDownloadInfo;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

EXPORT_C CNcdPurchaseDownloadInfo* CNcdPurchaseDownloadInfo::NewL(
    const MNcdPurchaseDownloadInfo& aSource )
    {
    CNcdPurchaseDownloadInfo* self = NewLC( aSource );
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CNcdPurchaseDownloadInfo* CNcdPurchaseDownloadInfo::NewLC(
    const MNcdPurchaseDownloadInfo& aSource )
    {
    CNcdPurchaseDownloadInfo* self = new (ELeave) CNcdPurchaseDownloadInfo;
    CleanupStack::PushL( self );
    self->ConstructL( aSource );
    return self;
    }


EXPORT_C CNcdPurchaseDownloadInfo::CNcdPurchaseDownloadInfo()
: CBase()
    {
    }


EXPORT_C void CNcdPurchaseDownloadInfo::ConstructL()
    {
    SetContentUsage( MNcdPurchaseDownloadInfo::EDownloadable );
    SetContentUriL( KNullDesC );
    SetContentValidityDelta( -1 );
    SetContentMimeTypeL( KNullDesC );
    SetContentSize( 0 );
    SetLaunchable( EFalse );
    SetDescriptorTypeL( KNullDesC );
    SetDescriptorNameL( KNullDesC );
    SetDescriptorUriL( KNullDesC );
    SetDescriptorDataL( KNullDesC8 );
    SetRightsUriL( KNullDesC );
    SetRightsTypeL( KNullDesC );
    SetActivationKeyL( KNullDesC );
    SetInstallNotificationUriL( KNullDesC );
    }

EXPORT_C void CNcdPurchaseDownloadInfo::ConstructL(
    const MNcdPurchaseDownloadInfo& aSource )
    {
    SetContentUsage( aSource.ContentUsage() );
    SetContentUriL( aSource.ContentUri() );
    SetContentValidityDelta( aSource.ContentValidityDelta() );
    SetContentMimeTypeL( aSource.ContentMimeType() );
    SetContentSize( aSource.ContentSize() );
    SetLaunchable( aSource.IsLaunchable() );
    SetDescriptorTypeL( aSource.DescriptorType() );
    SetDescriptorNameL( aSource.DescriptorName() );
    SetDescriptorUriL( aSource.DescriptorUri() );
    SetDescriptorDataL( aSource.DescriptorData() );
    SetRightsUriL( aSource.RightsUri() );
    SetRightsTypeL( aSource.RightsType() );
    SetActivationKeyL( aSource.ActivationKey() );
    SetInstallNotificationUriL( aSource.InstallNotificationUri() );
    
    // Some ugly stuff for copying the attributes
    // This WILL NOT work for other MNcdPurchaseDownloadInfo-implementations
    // if there's going to be any
    const CNcdPurchaseDownloadInfo& info = 
        static_cast<const CNcdPurchaseDownloadInfo&>( aSource );
    
    if ( info.iAttributes ) 
        {
        iAttributes = CNcdAttributes::NewL( *info.iAttributes );
        }
    }

            
EXPORT_C CNcdPurchaseDownloadInfo::~CNcdPurchaseDownloadInfo()
    {
    delete iContentUri;
    delete iContentMimeType;
    delete iDescriptorType;
    delete iDescriptorName;
    delete iDescriptorUri;
    delete iDescriptorData;
    delete iRightsUri;
    delete iRightsType;
    delete iActivationKey;
    delete iInstallNotificationUri;
    delete iAttributes;
    }


EXPORT_C MNcdPurchaseDownloadInfo::TContentUsage
    CNcdPurchaseDownloadInfo::ContentUsage() const
    {
    return iContentUsage;
    }

EXPORT_C const TDesC& CNcdPurchaseDownloadInfo::ContentUri() const
    {
    return *iContentUri;
    }

EXPORT_C TInt CNcdPurchaseDownloadInfo::ContentValidityDelta() const
    {
    return iValidityDelta;
    }

EXPORT_C const TDesC& CNcdPurchaseDownloadInfo::ContentMimeType() const
    {
    return *iContentMimeType;
    }

EXPORT_C TInt CNcdPurchaseDownloadInfo::ContentSize() const
    {
    return iContentSize;
    }

EXPORT_C TBool CNcdPurchaseDownloadInfo::IsLaunchable() const
    {
    return iLaunchable;
    }

EXPORT_C const TDesC& CNcdPurchaseDownloadInfo::DescriptorType() const
    {
    return *iDescriptorType;
    }

EXPORT_C const TDesC& CNcdPurchaseDownloadInfo::DescriptorName() const
    {
    return *iDescriptorName;
    }

EXPORT_C const TDesC& CNcdPurchaseDownloadInfo::DescriptorUri() const
    {
    return *iDescriptorUri;
    }

EXPORT_C const TDesC8& CNcdPurchaseDownloadInfo::DescriptorData() const
    {
    return *iDescriptorData;
    }

EXPORT_C const TDesC& CNcdPurchaseDownloadInfo::RightsUri() const
    {
    return *iRightsUri;
    }

EXPORT_C const TDesC& CNcdPurchaseDownloadInfo::RightsType() const
    {
    return *iRightsType;
    }

EXPORT_C const TDesC& CNcdPurchaseDownloadInfo::ActivationKey() const
    {
    return *iActivationKey;
    }
    
EXPORT_C const TDesC& CNcdPurchaseDownloadInfo::InstallNotificationUri() const
    {
    return *iInstallNotificationUri;
    }
    
    
EXPORT_C const TDesC& CNcdPurchaseDownloadInfo::AttributeStringL( 
    TDownloadAttribute aAttribute ) const
    {
    DLTRACEIN((""));
    if ( !iAttributes || 
         iAttributes->AttributeType( aAttribute ) == 
            CNcdAttributes::EAttributeTypeUndefined )
        {
        DLERROR(("Attribute not set yet"));
        User::Leave( KErrNotFound );
        }
    return iAttributes->AttributeString16( aAttribute );    
    }


EXPORT_C TInt32 CNcdPurchaseDownloadInfo::AttributeInt32L( 
    TDownloadAttribute aAttribute ) const
    {
    if ( !iAttributes || 
         iAttributes->AttributeType( aAttribute ) == 
            CNcdAttributes::EAttributeTypeUndefined )
        {
        User::Leave( KErrNotFound );
        }
    return iAttributes->AttributeInt32( aAttribute );    
    }
    

EXPORT_C void CNcdPurchaseDownloadInfo::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt16L( iContentUsage );
    ExternalizeDesL( *iContentUri, aStream );
    aStream.WriteInt32L( iValidityDelta );
    ExternalizeDesL( *iContentMimeType, aStream );
    aStream.WriteInt32L( iContentSize );
    aStream.WriteInt8L( iLaunchable );
    ExternalizeDesL( *iDescriptorType, aStream );
    ExternalizeDesL( *iDescriptorName, aStream );
    ExternalizeDesL( *iDescriptorUri, aStream );
    ExternalizeDesL( *iDescriptorData, aStream );
    ExternalizeDesL( *iRightsUri, aStream );
    ExternalizeDesL( *iRightsType, aStream );
    ExternalizeDesL( *iActivationKey, aStream );
    ExternalizeDesL( *iInstallNotificationUri, aStream );
    
    if ( iAttributes ) 
        {
        aStream.WriteInt8L( 1 );
        iAttributes->ExternalizeL( aStream );
        }
    else 
        {
        aStream.WriteInt8L( 0 );
        }
    }

EXPORT_C void CNcdPurchaseDownloadInfo::InternalizeL( RReadStream& aStream )
    {
    iContentUsage = (TContentUsage)aStream.ReadInt16L();
    InternalizeDesL( iContentUri, aStream );
    iValidityDelta = aStream.ReadInt32L();
    InternalizeDesL( iContentMimeType, aStream );
    iContentSize = aStream.ReadInt32L();
    iLaunchable = aStream.ReadInt8L();
    InternalizeDesL( iDescriptorType, aStream );
    InternalizeDesL( iDescriptorName, aStream );
    InternalizeDesL( iDescriptorUri, aStream );
    InternalizeDesL( iDescriptorData, aStream );
    InternalizeDesL( iRightsUri, aStream );
    InternalizeDesL( iRightsType, aStream );
    InternalizeDesL( iActivationKey, aStream );
    InternalizeDesL( iInstallNotificationUri, aStream );
    
    delete iAttributes;
    iAttributes = NULL;
    
    TInt8 attribsExist = aStream.ReadInt8L();
    if ( attribsExist ) 
        {
        
        iAttributes = CNcdAttributes::NewL( 
            aStream,
            MNcdPurchaseDownloadInfo::EDownloadAttributeInternal );
        }
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetContentUsage(
    TContentUsage aUsage )
    {
    DLTRACEIN(("Content usage: %d", aUsage));
    iContentUsage = aUsage;
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetContentUriL( const TDesC& aUri )
    {
    DLTRACEIN(( _L("URI: %S"), &aUri ));
    AssignDesL( iContentUri, aUri );
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetContentValidityDelta(
    TInt aValidityDelta )
    {
    iValidityDelta = aValidityDelta;
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetContentMimeTypeL(
    const TDesC& aMimeType )
    {
    DLTRACEIN(( _L("Mime: %S"), &aMimeType ));
    AssignDesL( iContentMimeType, aMimeType );
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetContentSize( TInt aSize )
    {
    iContentSize = aSize;
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetLaunchable( TBool aLaunchable )
    {
    DLTRACEIN(("Launchable: %d", aLaunchable));
    iLaunchable = aLaunchable;
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetDescriptorTypeL(
    const TDesC& aType )
    {
    DLTRACEIN(( _L("Descriptor type: %S"), &aType ));
    AssignDesL( iDescriptorType, aType );
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetDescriptorNameL(
    const TDesC& aName )
    {
    DLTRACEIN(( _L("Descriptor name: %S"), &aName ));
    AssignDesL( iDescriptorName, aName );
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetDescriptorUriL( const TDesC& aUri )
    {
    DLTRACEIN(( _L("Descriptor URI: %S"), &aUri ));
    AssignDesL( iDescriptorUri, aUri );
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetDescriptorDataL(
    const TDesC8& aData )
    {    
    AssignDesL( iDescriptorData, aData );
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetRightsUriL( const TDesC& aUri )
    {
    AssignDesL( iRightsUri, aUri );
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetRightsTypeL( const TDesC& aType )
    {
    DLTRACEIN(( _L("Rights type: %S"), &aType ));
    AssignDesL( iRightsType, aType );
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetActivationKeyL(
    const TDesC& aActivationKey )
    {
    AssignDesL( iActivationKey, aActivationKey );
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetInstallNotificationUriL(
    const TDesC& aInstallNotificationUri )
    {
    AssignDesL( iInstallNotificationUri, aInstallNotificationUri );
    }


EXPORT_C void CNcdPurchaseDownloadInfo::SetAttributeL( 
    TDownloadAttribute aAttribute, TInt32 aValue )
    {
    CreateAttributesL();
    iAttributes->SetAttributeL( aAttribute, aValue );
    }

EXPORT_C void CNcdPurchaseDownloadInfo::SetAttributeL( 
    TDownloadAttribute aAttribute, const TDesC& aValue )
    {
    CreateAttributesL();
    iAttributes->SetAttributeL( aAttribute, aValue );
    }

EXPORT_C void CNcdPurchaseDownloadInfo::CreateAttributesL()
    {
    DLTRACEIN((""));
    if ( !iAttributes ) 
        {
        iAttributes = CNcdAttributes::NewL( 
            MNcdPurchaseDownloadInfo::EDownloadAttributeInternal );
        }
    }
