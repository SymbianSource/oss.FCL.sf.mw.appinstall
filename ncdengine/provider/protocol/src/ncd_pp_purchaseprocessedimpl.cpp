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
* Description:   CNcdPreminetProtocolPurchaseProcessedImpl implementation
*
*/


#include "ncd_pp_purchaseprocessedimpl.h"
#include "ncd_pp_purchase.h"
#include "ncd_cp_query.h"
#include "ncdprotocolutils.h"

CNcdPreminetProtocolPurchaseProcessedImpl* 
CNcdPreminetProtocolPurchaseProcessedImpl::NewL() 
    {
    CNcdPreminetProtocolPurchaseProcessedImpl* self =
        new (ELeave) CNcdPreminetProtocolPurchaseProcessedImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolPurchaseProcessedImpl* 
CNcdPreminetProtocolPurchaseProcessedImpl::NewLC() 
    {
    CNcdPreminetProtocolPurchaseProcessedImpl* self =
        new (ELeave) CNcdPreminetProtocolPurchaseProcessedImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CNcdPreminetProtocolPurchaseProcessedImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iTransactionId );
    NcdProtocolUtils::AssignEmptyDesL( iQueryId );
    }

CNcdPreminetProtocolPurchaseProcessedImpl::CNcdPreminetProtocolPurchaseProcessedImpl()
: CBase()
    {
    iInformation = 0;
    }

CNcdPreminetProtocolPurchaseProcessedImpl::~CNcdPreminetProtocolPurchaseProcessedImpl()
    {
    DLTRACEIN((""));
    delete iInformation;
    delete iTransactionId;
    delete iQueryId;
    iEntities.ResetAndDestroy();
    DLTRACEOUT((""));
    }

TInt CNcdPreminetProtocolPurchaseProcessedImpl::ResultCode() const
    {
    return iResultCode;    
    }


const MNcdPreminetProtocolPurchaseEntity&
CNcdPreminetProtocolPurchaseProcessedImpl::EntityL(TInt aIndex) const
    {
    if ( aIndex < 0 || aIndex >= iEntities.Count() )
        {
        User::Leave(KErrArgument);
        }
    return *iEntities[aIndex];
    }

TInt CNcdPreminetProtocolPurchaseProcessedImpl::EntityCount() const 
    {
    return iEntities.Count();
    }

const MNcdConfigurationProtocolQuery* 
CNcdPreminetProtocolPurchaseProcessedImpl::Information() const
    {
    return iInformation;
    }

const TDesC& CNcdPreminetProtocolPurchaseProcessedImpl::TransactionId() const
    {
    return *iTransactionId;    
    }

const TDesC& CNcdPreminetProtocolPurchaseProcessedImpl::QueryId() const
    {
    return *iQueryId;
    }

