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
* Description:   CNcdPreminetProtocolPurchaseInformationImpl implementation
*
*/


#include "ncd_pp_purchaseinformationimpl.h"
#include "ncd_pp_purchase.h"
#include "ncd_cp_queryimpl.h"
#include "ncdprotocolutils.h"

CNcdPreminetProtocolPurchaseInformationImpl* 
CNcdPreminetProtocolPurchaseInformationImpl::NewL() 
    {
    CNcdPreminetProtocolPurchaseInformationImpl* self =
        new (ELeave) CNcdPreminetProtocolPurchaseInformationImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolPurchaseInformationImpl* 
CNcdPreminetProtocolPurchaseInformationImpl::NewLC() 
    {
    CNcdPreminetProtocolPurchaseInformationImpl* self =
        new (ELeave) CNcdPreminetProtocolPurchaseInformationImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


void CNcdPreminetProtocolPurchaseInformationImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iUri );
    NcdProtocolUtils::AssignEmptyDesL( iNamespace );
    NcdProtocolUtils::AssignEmptyDesL( iTotalPrice );
    NcdProtocolUtils::AssignEmptyDesL( iTotalPriceCurrency );
    NcdProtocolUtils::AssignEmptyDesL( iTransactionId );
    NcdProtocolUtils::AssignEmptyDesL( iQueryId );
    // set to true by default, because a new session is assumed, even
    // though CGW doesn't send this in the response
    iInitiateSession = ETrue;
    }

CNcdPreminetProtocolPurchaseInformationImpl::~CNcdPreminetProtocolPurchaseInformationImpl()
    {
    iPayments.ResetAndDestroy();

    delete iEntity;
    
    delete iUri;
    delete iNamespace;
    delete iDisclaimer;
    delete iTotalPrice;
    delete iTotalPriceCurrency;
    delete iTransactionId;
    delete iQueryId;
    }

const TDesC& 
CNcdPreminetProtocolPurchaseInformationImpl::Uri() const
    {
    return *iUri;
    }

const TDesC& 
CNcdPreminetProtocolPurchaseInformationImpl::Namespace() const
    {
    return *iNamespace;
    }

TBool CNcdPreminetProtocolPurchaseInformationImpl::InitiateSession() const
    {
    return iInitiateSession;
    }

const MNcdPreminetProtocolPayment&
CNcdPreminetProtocolPurchaseInformationImpl::PaymentL(TInt aIndex) const
    {
    DASSERT( aIndex >= 0 && aIndex < iPayments.Count() );
    if ( aIndex < 0 || aIndex >= iPayments.Count() ) 
        {
        User::Leave(KErrArgument);
        }
    return *iPayments[aIndex];
    }

TInt CNcdPreminetProtocolPurchaseInformationImpl::PaymentCount() const
    {
    return iPayments.Count();
    }

const MNcdConfigurationProtocolQuery* 
CNcdPreminetProtocolPurchaseInformationImpl::Disclaimer() const
    {
    return iDisclaimer;
    }

const TDesC& 
CNcdPreminetProtocolPurchaseInformationImpl::TotalPrice() const
    {
    return *iTotalPrice;
    }

const TDesC& 
CNcdPreminetProtocolPurchaseInformationImpl::TotalPriceCurrency() const
    {
    return *iTotalPriceCurrency;
    }

const MNcdPreminetProtocolPurchaseEntity&
CNcdPreminetProtocolPurchaseInformationImpl::EntityL(TInt /*aIndex*/) const
    {
    //  remove comments when enabling support for multiple entities
    //if ( aIndex < 0 || aIndex >= iEntities.Count() )
    //    User::Leave(KErrArgument);
    // return iEntities[aIndex];
    if ( !iEntity )
        User::Leave(KErrArgument);
    return *iEntity;
    }

TInt CNcdPreminetProtocolPurchaseInformationImpl::EntityCount() const
    {
    //  remove comments when enabling support for multiple entities
    //return iEntities.Count();
    if ( !iEntity )
        return 0;
    return 1;
    }
/*
const RArray<TNcdPurchaseEntityRedirected>& 
CNcdPreminetProtocolPurchaseInformationImpl::RedirectedEntities() const
    {
    }
*/

const TDesC& CNcdPreminetProtocolPurchaseInformationImpl::TransactionId() const
    {
    return *iTransactionId;    
    }

const TDesC& CNcdPreminetProtocolPurchaseInformationImpl::QueryId() const
    {
    return *iQueryId;
    }

