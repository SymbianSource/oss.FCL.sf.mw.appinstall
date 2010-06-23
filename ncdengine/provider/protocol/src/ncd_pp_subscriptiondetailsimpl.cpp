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


#include "ncd_pp_subscriptiondetailsimpl.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

CNcdPreminetProtocolSubscriptionDetailsImpl* 
CNcdPreminetProtocolSubscriptionDetailsImpl::NewL() 
    {
    CNcdPreminetProtocolSubscriptionDetailsImpl* self =
        new (ELeave) CNcdPreminetProtocolSubscriptionDetailsImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolSubscriptionDetailsImpl* 
CNcdPreminetProtocolSubscriptionDetailsImpl::NewLC() 
    {
    CNcdPreminetProtocolSubscriptionDetailsImpl* self =
        new (ELeave) CNcdPreminetProtocolSubscriptionDetailsImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CNcdPreminetProtocolSubscriptionDetailsImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iAmountOfCreditsCurrency );
    }

CNcdPreminetProtocolSubscriptionDetailsImpl::CNcdPreminetProtocolSubscriptionDetailsImpl()
: CBase(), iValidityDelta( -1 ), iValidityAutoUpdate( EFalse ), iAmountOfCredits( -1 ),
  iNumberOfDownloads( -1 )
    {
    }
    
CNcdPreminetProtocolSubscriptionDetailsImpl::~CNcdPreminetProtocolSubscriptionDetailsImpl()
    {
    delete iAmountOfCreditsCurrency;
    }


// subscription details
TInt CNcdPreminetProtocolSubscriptionDetailsImpl::ValidityDelta() const
    {
    return iValidityDelta;
    }

TBool CNcdPreminetProtocolSubscriptionDetailsImpl::ValidityAutoUpdate() const
    {
    return iValidityAutoUpdate;
    }

TReal32 CNcdPreminetProtocolSubscriptionDetailsImpl::AmountOfCredits() const
    {
    return iAmountOfCredits;
    }

const TDesC& 
CNcdPreminetProtocolSubscriptionDetailsImpl::AmountOfCreditsCurrency() const
    {
    DASSERT((iAmountOfCreditsCurrency));
    return *iAmountOfCreditsCurrency;
    }

TInt CNcdPreminetProtocolSubscriptionDetailsImpl::NumberOfDownloads() const
    {
    return iNumberOfDownloads;
    }
    

