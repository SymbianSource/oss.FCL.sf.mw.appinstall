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


#include "ncd_pp_subscriptionimpl.h"
#include "ncd_pp_subscriptiondetails.h"
#include "ncd_pp_purchase.h"
#include "ncd_cp_query.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

CNcdPreminetProtocolSubscriptionImpl* 
CNcdPreminetProtocolSubscriptionImpl::NewL() 
    {
    CNcdPreminetProtocolSubscriptionImpl* self =
        new (ELeave) CNcdPreminetProtocolSubscriptionImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolSubscriptionImpl* 
CNcdPreminetProtocolSubscriptionImpl::NewLC() 
    {
    CNcdPreminetProtocolSubscriptionImpl* self =
        new (ELeave) CNcdPreminetProtocolSubscriptionImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CNcdPreminetProtocolSubscriptionImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iNamespace );
    NcdProtocolUtils::AssignEmptyDesL( iEntityId );
    NcdProtocolUtils::AssignEmptyDesL( iPurchaseOptionId );
    NcdProtocolUtils::AssignEmptyDesL( iTimestamp );
    NcdProtocolUtils::AssignEmptyDesL( iExpiredOn );
    }

CNcdPreminetProtocolSubscriptionImpl::CNcdPreminetProtocolSubscriptionImpl()
: CBase(), iType(ENotSubscribable), iCancelled(EFalse)
    {
    }
    
CNcdPreminetProtocolSubscriptionImpl::~CNcdPreminetProtocolSubscriptionImpl()
    {
    delete iNamespace;
    delete iEntityId;
    delete iPurchaseOptionId;
    delete iTimestamp;
    delete iExpiredOn;
    delete iTotalUsageRights;
    delete iRemainingUsageRights;
    }

const TDesC& CNcdPreminetProtocolSubscriptionImpl::Namespace() const 
    {
    DASSERT((iNamespace));
    return *iNamespace;
    }
    
const TDesC& CNcdPreminetProtocolSubscriptionImpl::EntityId() const
    {
    DASSERT((iEntityId));
    return *iEntityId;
    }

const TDesC& CNcdPreminetProtocolSubscriptionImpl::PurchaseOptionId() const
    {
    DASSERT((iPurchaseOptionId));
    return *iPurchaseOptionId;
    }

const TDesC& CNcdPreminetProtocolSubscriptionImpl::Timestamp() const
    {
    DASSERT((iTimestamp));
    return *iTimestamp;
    }

TNcdSubscriptionType CNcdPreminetProtocolSubscriptionImpl::Type() const
    {
    return iType;
    }

TBool CNcdPreminetProtocolSubscriptionImpl::Cancelled() const
    {
    return iCancelled;
    }

const MNcdPreminetProtocolSubscriptionDetails*
CNcdPreminetProtocolSubscriptionImpl::TotalUsageRights() const 
    {
    return iTotalUsageRights;
    }

const MNcdPreminetProtocolSubscriptionDetails*
CNcdPreminetProtocolSubscriptionImpl::RemainingUsageRights() const 
    {
    return iRemainingUsageRights;
    }


const TDesC& CNcdPreminetProtocolSubscriptionImpl::ExpiredOn() const 
    {
    DASSERT((iExpiredOn));
    return *iExpiredOn;
    }

const MNcdPreminetProtocolSubscriptionDetails*
CNcdPreminetProtocolSubscriptionImpl::UsageRights() const 
    {
    return iTotalUsageRights;
    }
