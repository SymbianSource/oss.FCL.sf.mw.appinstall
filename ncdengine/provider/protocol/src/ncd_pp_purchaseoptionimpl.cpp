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
* Description:   CNcdEntityRefParser implementation
*
*/


#include "ncd_pp_purchaseoptionimpl.h"
#include "ncd_pp_subscriptiondetailsimpl.h"
#include "ncd_pp_download.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

CNcdPreminetProtocolPurchaseOptionImpl* 
CNcdPreminetProtocolPurchaseOptionImpl::NewL() 
    {
    CNcdPreminetProtocolPurchaseOptionImpl* self =
        new (ELeave) CNcdPreminetProtocolPurchaseOptionImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolPurchaseOptionImpl* 
CNcdPreminetProtocolPurchaseOptionImpl::NewLC() 
    {
    CNcdPreminetProtocolPurchaseOptionImpl* self =
        new (ELeave) CNcdPreminetProtocolPurchaseOptionImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CNcdPreminetProtocolPurchaseOptionImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iName );
    NcdProtocolUtils::AssignEmptyDesL( iId );
    NcdProtocolUtils::AssignEmptyDesL( iDependencyId );
    NcdProtocolUtils::AssignEmptyDesL( iDescription );
    NcdProtocolUtils::AssignEmptyDesL( iPriceText );
    NcdProtocolUtils::AssignEmptyDesL( iPriceCurrency );
    NcdProtocolUtils::AssignEmptyDesL( iParentSubscriptionEntityId );
    NcdProtocolUtils::AssignEmptyDesL( iParentSubscriptionPurchaseOptionId );

    iSubscriptionDetails = CNcdPreminetProtocolSubscriptionDetailsImpl::NewL();
    
    iRequirePurchaseProcess = EFalse;
    }

CNcdPreminetProtocolPurchaseOptionImpl::CNcdPreminetProtocolPurchaseOptionImpl()
: CBase(), iPurchase(ETypeNotSet), iFree(EFalse), iRequirePurchaseProcess(EFalse), 
  iCanBeGift(ETrue), iPrice(0), iCreditPrice(0)
    {
    }
    
CNcdPreminetProtocolPurchaseOptionImpl::~CNcdPreminetProtocolPurchaseOptionImpl()
    {
    delete iName;
    delete iId;
    delete iDependencyId;
    delete iDescription;
    delete iPriceText;
    delete iPriceCurrency;
    iDownloadDetails.ResetAndDestroy();
    delete iParentSubscriptionEntityId;
    delete iParentSubscriptionPurchaseOptionId;
    delete iSubscriptionDetails;
    }

const TDesC& CNcdPreminetProtocolPurchaseOptionImpl::Name() const
    {
    return *iName;    
    }

const TDesC& CNcdPreminetProtocolPurchaseOptionImpl::Id() const
    {
    return *iId;
    }

const TDesC& CNcdPreminetProtocolPurchaseOptionImpl::DependencyId() const
    {
    return *iDependencyId;
    }

TNcdPurchaseType CNcdPreminetProtocolPurchaseOptionImpl::Purchase() const
    {
    return iPurchase;
    }

TBool CNcdPreminetProtocolPurchaseOptionImpl::IsFree() const
    {
    return iFree;
    }

TBool CNcdPreminetProtocolPurchaseOptionImpl::RequirePurchaseProcess() const
    {
    return iRequirePurchaseProcess;
    }

TBool CNcdPreminetProtocolPurchaseOptionImpl::CanBeGift() const
    {
    return iCanBeGift;
    }

const TDesC& CNcdPreminetProtocolPurchaseOptionImpl::Description() const
    {
    return *iDescription;
    }
    
const MNcdPreminetProtocolDownload& 
    CNcdPreminetProtocolPurchaseOptionImpl::DownloadDetailsL(TInt aIndex) const
    {
    DASSERT( aIndex >= 0 && aIndex < iDownloadDetails.Count() );
    if ( aIndex < 0 || aIndex >= iDownloadDetails.Count() ) 
        {
        User::Leave(KErrArgument);
        }
    return *iDownloadDetails[aIndex];
    }

TInt CNcdPreminetProtocolPurchaseOptionImpl::DownloadDetailsCount() const
    {
    return iDownloadDetails.Count();
    }

// subscription details
const MNcdPreminetProtocolSubscriptionDetails* 
CNcdPreminetProtocolPurchaseOptionImpl::SubscriptionDetails() const
    {
    return iSubscriptionDetails;
    }

const TDesC& 
CNcdPreminetProtocolPurchaseOptionImpl::ParentSubscriptionEntityId() const
    {
    return *iParentSubscriptionEntityId;
    }

const TDesC& 
CNcdPreminetProtocolPurchaseOptionImpl::ParentSubscriptionPurchaseOptionId() const
    {
    return *iParentSubscriptionPurchaseOptionId;
    }


const TDesC& CNcdPreminetProtocolPurchaseOptionImpl::PriceText() const
    {
    return *iPriceText;
    }

TReal32 CNcdPreminetProtocolPurchaseOptionImpl::Price() const
    {
    return iPrice;
    }

const TDesC& CNcdPreminetProtocolPurchaseOptionImpl::PriceCurrency() const
    {
    return *iPriceCurrency;
    }

TReal32 CNcdPreminetProtocolPurchaseOptionImpl::CreditPrice() const
    {
    return iCreditPrice;
    }

