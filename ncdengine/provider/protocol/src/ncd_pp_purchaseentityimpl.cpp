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
* Description:   CNcdPreminetProtocolPurchaseEntityImpl implementation
*
*/


#include "ncd_pp_purchaseentityimpl.h"
#include "ncd_pp_download.h"
#include "ncdprotocolutils.h"

CNcdPreminetProtocolPurchaseEntityImpl* 
CNcdPreminetProtocolPurchaseEntityImpl::NewL() 
    {
    CNcdPreminetProtocolPurchaseEntityImpl* self =
        new (ELeave) CNcdPreminetProtocolPurchaseEntityImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolPurchaseEntityImpl* 
CNcdPreminetProtocolPurchaseEntityImpl::NewLC() 
    {
    CNcdPreminetProtocolPurchaseEntityImpl* self =
        new (ELeave) CNcdPreminetProtocolPurchaseEntityImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
void CNcdPreminetProtocolPurchaseEntityImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iId );
    NcdProtocolUtils::AssignEmptyDesL( iTicket );
    NcdProtocolUtils::AssignEmptyDesL( iPriceText );
    NcdProtocolUtils::AssignEmptyDesL( iPrice );
    NcdProtocolUtils::AssignEmptyDesL( iPriceCurrency );
    }

CNcdPreminetProtocolPurchaseEntityImpl::CNcdPreminetProtocolPurchaseEntityImpl()
: CBase()
    {
    }
    
CNcdPreminetProtocolPurchaseEntityImpl::~CNcdPreminetProtocolPurchaseEntityImpl()
    {
    DLTRACEIN((""));
    delete iId;
    delete iTicket;
    delete iPriceText;
    delete iPrice;
    delete iPriceCurrency;
    iDownloadDetails.ResetAndDestroy();
    iDeliveryMethods.Close();
    DLTRACEOUT((""));
    }

const TDesC& 
CNcdPreminetProtocolPurchaseEntityImpl::Id() const
    {
    return *iId;
    }

const TDesC& 
CNcdPreminetProtocolPurchaseEntityImpl::Ticket() const
    {
    return *iTicket;
    }

const MNcdPreminetProtocolDownload&
CNcdPreminetProtocolPurchaseEntityImpl::DownloadDetailsL(TInt aIndex) const
    {
    DASSERT( aIndex >= 0 && aIndex < iDownloadDetails.Count() );
    if ( aIndex < 0 || aIndex >= iDownloadDetails.Count() ) 
        {
        User::Leave(KErrArgument);
        }
    return *iDownloadDetails[aIndex];
    }

TInt CNcdPreminetProtocolPurchaseEntityImpl::DownloadDetailsCount() const
    {
    return iDownloadDetails.Count();
    }

const TDesC& 
CNcdPreminetProtocolPurchaseEntityImpl::Price() const
    {
    return *iPrice;
    }

const TDesC& 
CNcdPreminetProtocolPurchaseEntityImpl::PriceCurrency() const
    {
    return *iPriceCurrency;
    }

const TDesC& 
CNcdPreminetProtocolPurchaseEntityImpl::PriceText() const
    {
    return *iPriceText;
    }

const RArray<TNcdDeliveryMethod>& 
CNcdPreminetProtocolPurchaseEntityImpl::DeliveryMethods() const
    {
    return iDeliveryMethods;
    }

