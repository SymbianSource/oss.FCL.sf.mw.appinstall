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


#include "ncdserverpartofsubscription.h"





// ======== MEMBER FUNCTIONS ========


CNcdServerPartOfSubscription::CNcdServerPartOfSubscription()
    {
    }

void CNcdServerPartOfSubscription::ConstructL()
    {
    }


CNcdServerPartOfSubscription* CNcdServerPartOfSubscription::NewL()
    {
    CNcdServerPartOfSubscription* self = 
        CNcdServerPartOfSubscription::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


CNcdServerPartOfSubscription* CNcdServerPartOfSubscription::NewLC()
    {
    CNcdServerPartOfSubscription* self =
        new( ELeave ) CNcdServerPartOfSubscription();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdServerPartOfSubscription::~CNcdServerPartOfSubscription()
    {
    delete iParentEntityId;
    delete iParentPurchaseOptionId;
    }

void CNcdServerPartOfSubscription::SetParentEntityId(
    HBufC* aParentEntityId )
    {
    delete iParentEntityId;
    iParentEntityId = aParentEntityId;
    }
    
void CNcdServerPartOfSubscription::SetParentPurchaseOptionId(
    HBufC* aParentPurchaseOptionId )
    {
    delete iParentPurchaseOptionId;
    iParentPurchaseOptionId = aParentPurchaseOptionId;
    }

void CNcdServerPartOfSubscription::SetCreditPrice( TReal32 aCreditPrice )
    {
    iCreditPrice = aCreditPrice;
    }

const TDesC& CNcdServerPartOfSubscription::ParentEntityId() const
    {
    if ( iParentEntityId == NULL )
        {
        return KNullDesC;
        }
    return *iParentEntityId;
    }
    
const TDesC& CNcdServerPartOfSubscription::ParentPurchaseOptionId() const
    {
    if ( iParentPurchaseOptionId == NULL )
        {
        return KNullDesC;
        }    
    return *iParentPurchaseOptionId;
    }
TReal32 CNcdServerPartOfSubscription::CreditPrice() const
    {
    return iCreditPrice;
    }
