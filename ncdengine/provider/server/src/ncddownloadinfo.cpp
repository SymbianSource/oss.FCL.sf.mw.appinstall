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
* Description:   Implementation of CNcdDownloadInfo
*
*/


#include "ncddownloadinfo.h"
#include "ncd_pp_entitydependency.h"
#include "ncd_pp_download.h"
#include "ncd_pp_descriptor.h"
#include "ncd_pp_rights.h"
#include "catalogsutils.h"
#include "ncdpurchasedownloadinfo.h"


CNcdDownloadInfo* CNcdDownloadInfo::NewL()
    {
    CNcdDownloadInfo* self =
         CNcdDownloadInfo::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CNcdDownloadInfo* CNcdDownloadInfo::NewLC()
    {
    CNcdDownloadInfo* self = 
        new (ELeave) CNcdDownloadInfo();
    CleanupStack::PushL( self );
    self->CNcdPurchaseDownloadInfo::ConstructL();
    return self;
    }


CNcdDownloadInfo* CNcdDownloadInfo::NewL(
    const MNcdPreminetProtocolEntityDependency& aProtocolDependency )
    {
    CNcdDownloadInfo* self = 
        CNcdDownloadInfo::NewLC( aProtocolDependency );
    CleanupStack::Pop( self );
    return self;
    }

CNcdDownloadInfo* CNcdDownloadInfo::NewLC( 
    const MNcdPreminetProtocolEntityDependency& aProtocolDependency  )
    {    
    CNcdDownloadInfo* self = 
        new (ELeave) CNcdDownloadInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aProtocolDependency );
    return self;
    }


CNcdDownloadInfo* CNcdDownloadInfo::NewLC( 
    const MNcdPreminetProtocolDownload& aProtocolDownload )
    {
    CNcdDownloadInfo* self = 
        new (ELeave) CNcdDownloadInfo();
    CleanupStack::PushL( self );    
    self->ConstructL( aProtocolDownload );
    return self;    
    }

    
CNcdDownloadInfo* CNcdDownloadInfo::NewLC( 
    const MNcdPurchaseDownloadInfo& aDownload,
    TUid aUid,
    const TDesC& aVersion )
    {
    CNcdDownloadInfo* self = 
        new (ELeave) CNcdDownloadInfo();
    CleanupStack::PushL( self );    
    self->ConstructL( aDownload, aUid, aVersion );
    return self;    
    }
    


void CNcdDownloadInfo::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    CNcdPurchaseDownloadInfo::InternalizeL( aStream );
    iUid.iUid = aStream.ReadInt32L();
    InternalizeDesL( iVersion, aStream );    
    InternalizeEnumL( iDependencyState, aStream );
    DLTRACEOUT(("Internalization successful"));
    }


void CNcdDownloadInfo::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));
    CNcdPurchaseDownloadInfo::ExternalizeL( aStream );
    aStream.WriteInt32L( iUid.iUid );
    ExternalizeDesL( *iVersion, aStream );
    ExternalizeEnumL( iDependencyState, aStream );
    DLTRACEOUT(("Externalization successful"));
    }



const TDesC& CNcdDownloadInfo::ContentName() const
    {
    DLTRACEIN((""));
    const TDesC* name = NULL;
    
    // Errors ignore, AttributeStringL leaves with KErrNotFound if
    // the attribute is not set but that doesn't matter
    TRAP_IGNORE( name = &AttributeStringL( 
        MNcdPurchaseDownloadInfo::EDownloadAttributeDependencyName ) );
    if ( name ) 
        {
        return *name;
        }
    return KNullDesC;
    }


TUid CNcdDownloadInfo::ContentId() const
    {
    return iUid;
    }


const TDesC& CNcdDownloadInfo::ContentVersion() const
    {
    DASSERT( iVersion );
    return *iVersion;
    }


TNcdDependencyState CNcdDownloadInfo::DependencyState() const
    {
    return iDependencyState;
    }


void CNcdDownloadInfo::SetDependencyState( TNcdDependencyState aState )
    {
    iDependencyState = aState;
    }


CNcdDownloadInfo::CNcdDownloadInfo()
    : CNcdPurchaseDownloadInfo(), 
    iDependencyState( ENcdDependencyMissing )
    {
    }


void CNcdDownloadInfo::ConstructL(
    const MNcdPreminetProtocolEntityDependency& aProtocolDependency )
    {
    DLTRACEIN((""));
    // Just to be sure that everything will be initialized.
    // Notice, that this initializes everything. So,
    // most of the data is most likely unnecessarily created 
    // here because they will be replaced later in this function.
    CNcdPurchaseDownloadInfo::ConstructL();

    if ( aProtocolDependency.DownloadDetails() ) 
        {
        ConstructDownloadL( *aProtocolDependency.DownloadDetails() );
        }

    if ( aProtocolDependency.ContentId().Length() )
        {
        TInt uid = 0;
        DLTRACE(( _L("Content ID: %S"), &aProtocolDependency.ContentId() ));
        
        TRAPD( err, uid = DesHexToIntL( aProtocolDependency.ContentId() ) );
        if( err != KErrNone )
            {
            DLERROR(( "DownloadableContent()->Id() was not valid hex" ));                      
            iUid.iUid = 0;
            }
        else
            {
            iUid.iUid = uid;
            }
        }

    iVersion = aProtocolDependency.ContentVersion().AllocL();
    
    
    SetAttributeL( MNcdPurchaseDownloadInfo::EDownloadAttributeDependencyName, 
        aProtocolDependency.Name() );
    
    DLTRACEOUT(( _L("Content version: %S"), iVersion ));
    }
    

void CNcdDownloadInfo::ConstructL( 
    const MNcdPreminetProtocolDownload& aProtocolDownload )
    {
    DLTRACEIN((""));
    CNcdPurchaseDownloadInfo::ConstructL();
    iVersion = KNullDesC().AllocL();

    SetAttributeL( MNcdPurchaseDownloadInfo::EDownloadAttributeDependencyName, 
        KNullDesC );

    ConstructDownloadL( aProtocolDownload );
    }
    

void CNcdDownloadInfo::ConstructDownloadL( 
    const MNcdPreminetProtocolDownload& aProtocolDownload )
    {
    DLTRACEIN((""));

    
    SetContentUriL( aProtocolDownload.Uri() );
    SetLaunchable( aProtocolDownload.Launchable() );
    SetContentMimeTypeL( aProtocolDownload.Mime() );

    SetContentSize( aProtocolDownload.Size() );
    
    SetInstallNotificationUriL( aProtocolDownload.InstallNotificationUri() );
    
    // Set content usage
    if ( aProtocolDownload.Target() == EDownloadTargetConsumable )
        {                
        SetContentUsage( MNcdPurchaseDownloadInfo::EConsumable );
        }
    else
        {
        DLINFO(("Setting as downloadable, was actually: %d", 
                aProtocolDownload.Target()));
        SetContentUsage( MNcdPurchaseDownloadInfo::EDownloadable );
        }
    
    if( aProtocolDownload.Descriptor() != NULL )
        {
        SetDescriptorDataL( aProtocolDownload.Descriptor()->Data() );
        SetDescriptorNameL( aProtocolDownload.Descriptor()->Name() );
        SetDescriptorTypeL( aProtocolDownload.Descriptor()->Type() );
        SetDescriptorUriL( aProtocolDownload.Descriptor()->Uri() );
        }

    if( aProtocolDownload.Rights() )
        {
        SetRightsTypeL( aProtocolDownload.Rights()->Type() );
        SetRightsUriL( aProtocolDownload.Rights()->Uri() );
        SetActivationKeyL( aProtocolDownload.Rights()->ActivationKey());
        }

    SetContentValidityDelta( aProtocolDownload.ValidityDelta() );
    
    DLTRACEOUT((""));
    }


void CNcdDownloadInfo::ConstructL( 
    const MNcdPurchaseDownloadInfo& aDownload,
    TUid aUid,
    const TDesC& aVersion )
    {
    DLTRACEIN((""));
    CNcdPurchaseDownloadInfo::ConstructL( aDownload );
    iVersion = aVersion.AllocL();
    iUid = aUid;        
    }


CNcdDownloadInfo::~CNcdDownloadInfo()
    {
    delete iVersion;
    }
