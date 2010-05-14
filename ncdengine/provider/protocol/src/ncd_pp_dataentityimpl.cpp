/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdPreminetProtocolDataEntityImpl implementation
*
*/


#include "ncd_pp_purchaseoption.h"
#include "ncd_pp_dataentityimpl.h"
#include "ncd_pp_dataentitycontent.h"
#include "ncd_pp_skin.h"
#include "ncd_pp_icon.h"
#include "ncd_pp_download.h"
#include "ncd_cp_query.h"
#include "ncdprotocolutils.h"
#include "catalogsdebug.h"
#include "ncd_cp_detail.h"

CNcdPreminetProtocolDataEntityImpl* 
CNcdPreminetProtocolDataEntityImpl::NewL() 
    {
    CNcdPreminetProtocolDataEntityImpl* self =
        new (ELeave) CNcdPreminetProtocolDataEntityImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolDataEntityImpl* 
CNcdPreminetProtocolDataEntityImpl::NewLC() 
    {
    CNcdPreminetProtocolDataEntityImpl* self =
        new (ELeave) CNcdPreminetProtocolDataEntityImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
void CNcdPreminetProtocolDataEntityImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iId );
    NcdProtocolUtils::AssignEmptyDesL( iTimestamp );
    NcdProtocolUtils::AssignEmptyDesL( iNamespace );
    NcdProtocolUtils::AssignEmptyDesL( iServerUri );
    NcdProtocolUtils::AssignEmptyDesL( iName );
    NcdProtocolUtils::AssignEmptyDesL( iDescription );
    NcdProtocolUtils::AssignEmptyDesL( iLayoutType );
    
    }

CNcdPreminetProtocolDataEntityImpl::~CNcdPreminetProtocolDataEntityImpl()
    {
    DLTRACEIN((""));
    delete iId;
    delete iTimestamp;
    delete iNamespace;
    delete iServerUri;
    delete iName;
    delete iDescription;
    delete iDisclaimer;
    delete iIcon;
    
    iPurchaseOptions.ResetAndDestroy();    
    iScreenshots.ResetAndDestroy();
    iPreviews.ResetAndDestroy();
    
    delete iDownloadableContent;
    delete iSubscribableContent;
    
    if( iDetails )
        {
        iDetails->ResetAndDestroy();
        }
    delete iDetails;    
    delete iLayoutType;
    delete iSkin;
    delete iMoreInfo;
    }

TNcdDataEntityType CNcdPreminetProtocolDataEntityImpl::Type() const
    {
    return iType;
    }
    
const TDesC& CNcdPreminetProtocolDataEntityImpl::Id() const
    {
    DASSERT( iId );
    return *iId;
    }

const TDesC& CNcdPreminetProtocolDataEntityImpl::Timestamp() const
    {
    DASSERT( iTimestamp );
    return *iTimestamp;
    }

const TDesC& CNcdPreminetProtocolDataEntityImpl::Namespace() const
    {
    DASSERT( iNamespace );
    return *iNamespace;
    }

const TDesC& CNcdPreminetProtocolDataEntityImpl::ServerUri() const
    {
    DASSERT( iServerUri );
    return *iServerUri;
    }

const TDesC& CNcdPreminetProtocolDataEntityImpl::Name() const
    {
    DASSERT( iName );
    return *iName;    
    }

const TDesC& CNcdPreminetProtocolDataEntityImpl::Description() const
    {
    DASSERT( iDescription );
    return *iDescription;
    }
    
const MNcdConfigurationProtocolQuery* 
CNcdPreminetProtocolDataEntityImpl::Disclaimer() const
    {
    return iDisclaimer;
    }
    
const MNcdPreminetProtocolDownload* 
CNcdPreminetProtocolDataEntityImpl::HelpInformation() const
    {
    return 0;
    //return iHelpInformation;
    }
    
const MNcdPreminetProtocolDataEntityContent* 
CNcdPreminetProtocolDataEntityImpl::DownloadableContent() const 
    {
    return iDownloadableContent;
    }

const MNcdPreminetProtocolDataEntityContent* 
CNcdPreminetProtocolDataEntityImpl::SubscribableContent() const 
    {
    return iSubscribableContent;
    }

    
const MNcdPreminetProtocolIcon* 
CNcdPreminetProtocolDataEntityImpl::Icon() const
    {    
    return iIcon;
    }

TInt CNcdPreminetProtocolDataEntityImpl::PurchaseOptionCount() const 
    {
    return iPurchaseOptions.Count();
    }
    
const MNcdPreminetProtocolPurchaseOption&
CNcdPreminetProtocolDataEntityImpl::PurchaseOptionL(TInt aIndex) const 
    {
    DASSERT( aIndex >= 0 && aIndex < iPurchaseOptions.Count() );
    if (aIndex < 0 || aIndex >= iPurchaseOptions.Count()) 
        {
        User::Leave(KErrArgument);
        }
    return *iPurchaseOptions[aIndex];
    }
    
const MNcdPreminetProtocolSkin* 
CNcdPreminetProtocolDataEntityImpl::Skin() const 
    {
    return iSkin;
    }
    
const TDesC& CNcdPreminetProtocolDataEntityImpl::LayoutType() const
    {
    return *iLayoutType;
    }

TInt CNcdPreminetProtocolDataEntityImpl::ScreenshotCount() const 
    {
    return iScreenshots.Count();
    }

const MNcdPreminetProtocolDownload&
CNcdPreminetProtocolDataEntityImpl::ScreenshotL(TInt aIndex) const
    {
    if ( aIndex < 0 || aIndex >= iScreenshots.Count() ) 
        {
        User::Leave(KErrArgument);
        }
    return *iScreenshots[aIndex];
    }

TInt CNcdPreminetProtocolDataEntityImpl::PreviewCount() const 
    {
    return iPreviews.Count();
    }

const MNcdPreminetProtocolDownload&
CNcdPreminetProtocolDataEntityImpl::PreviewL(TInt aIndex) const
    {
    if ( aIndex < 0 || aIndex >= iPreviews.Count() ) 
        {
        User::Leave(KErrArgument);
        }
    return *iPreviews[aIndex];
    }

const MNcdConfigurationProtocolQuery* 
CNcdPreminetProtocolDataEntityImpl::MoreInfo() const
    {
    return iMoreInfo;
    }

TInt CNcdPreminetProtocolDataEntityImpl::DetailCount() const
    {
    if( iDetails )
        {
        return iDetails->Count();
        }
    else
        {
        return 0;
        }
    }

const MNcdConfigurationProtocolDetail&
CNcdPreminetProtocolDataEntityImpl::DetailL( TInt aIndex ) const
    {
    if( aIndex < 0 || !iDetails || aIndex >= iDetails->Count() )
        {
        User::Leave( KErrArgument );
        }
    return *iDetails->At( aIndex );
    }
