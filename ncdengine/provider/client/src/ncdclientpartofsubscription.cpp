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
* Description:  
*
*/


#include "ncdclientpartofsubscription.h"





// ======== MEMBER FUNCTIONS ========


CNcdClientPartOfSubscription::CNcdClientPartOfSubscription()
    {
    }

void CNcdClientPartOfSubscription::ConstructL()
    {
    }


CNcdClientPartOfSubscription* CNcdClientPartOfSubscription::NewL()
    {
    CNcdClientPartOfSubscription* self = 
        CNcdClientPartOfSubscription::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


CNcdClientPartOfSubscription* CNcdClientPartOfSubscription::NewLC()
    {
    CNcdClientPartOfSubscription* self =
        new( ELeave ) CNcdClientPartOfSubscription();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdClientPartOfSubscription::~CNcdClientPartOfSubscription()
    {
    delete iParentEntityId;
    delete iParentPurchaseOptionId;
    }

void CNcdClientPartOfSubscription::SetParentEntityId(
    HBufC* aParentEntityId )
    {
    delete iParentEntityId;
    iParentEntityId = aParentEntityId;
    }
    
void CNcdClientPartOfSubscription::SetParentPurchaseOptionId(
    HBufC* aParentPurchaseOptionId )
    {
    delete iParentPurchaseOptionId;
    iParentPurchaseOptionId = aParentPurchaseOptionId;
    }
    
void CNcdClientPartOfSubscription::SetCreditPrice( TReal32 aCreditPrice )
    {
    iCreditPrice = aCreditPrice;
    }

const TDesC& CNcdClientPartOfSubscription::ParentEntityId() const
    {
    if ( iParentEntityId == NULL )
        {
        return KNullDesC;
        }
    return *iParentEntityId;
    }
    
const TDesC& CNcdClientPartOfSubscription::ParentPurchaseOptionId() const
    {
    if ( iParentPurchaseOptionId == NULL )
        {
        return KNullDesC;
        }    
    return *iParentPurchaseOptionId;
    }
    
TReal32 CNcdClientPartOfSubscription::CreditPrice() const
    {
    return iCreditPrice;
    }
