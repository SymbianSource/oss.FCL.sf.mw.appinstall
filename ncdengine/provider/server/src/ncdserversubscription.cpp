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


#include <e32base.h>

#include "ncdserversubscription.h"

#include "catalogsinterfaceidentifier.h"



// ======== MEMBER FUNCTIONS ========


CNcdServerSubscription::CNcdServerSubscription()
    {
    }

void CNcdServerSubscription::ConstructL()
    {
    }


CNcdServerSubscription* CNcdServerSubscription::NewL()
    {
    CNcdServerSubscription* self = 
        CNcdServerSubscription::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


CNcdServerSubscription* CNcdServerSubscription::NewLC()
    {
    CNcdServerSubscription* self =
        new( ELeave ) CNcdServerSubscription();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdServerSubscription::~CNcdServerSubscription()
    {
    delete iAmountOfCreditsCurrency;
    }

void CNcdServerSubscription::SetValidityDelta( 
    TInt aValidityDelta )
    {
    iValidityDelta = aValidityDelta;
    }
    
void CNcdServerSubscription::SetValidityAutoUpdate( 
    TBool aValidityAutoUpdate )
    {
    iValidityAutoUpdate = aValidityAutoUpdate;
    }
    
void CNcdServerSubscription::SetAmountOfCredits( 
    TReal32 aAmountOfCredits )
    {
    iAmountOfCredits = aAmountOfCredits;
    }
    
void CNcdServerSubscription::SetAmountOfCreditsCurrency( 
    HBufC* aAmountOfCreditsCurrency )
    {
    delete iAmountOfCreditsCurrency;
    iAmountOfCreditsCurrency = aAmountOfCreditsCurrency;
    }
    
void CNcdServerSubscription::SetNumberOfDownloads(
    TInt aNumberOfDownloads )
    {
    iNumberOfDownloads = aNumberOfDownloads;
    }
    
    

TInt CNcdServerSubscription::ValidityDelta() const
    {
    return iValidityDelta;
    }
    
TBool CNcdServerSubscription::ValidityAutoUpdate() const
    {
    return iValidityAutoUpdate;
    }
    
TReal32 CNcdServerSubscription::AmountOfCredits() const
    {
    return iAmountOfCredits;
    }
    
const TDesC& CNcdServerSubscription::AmountOfCreditsCurrency() const
    {
    if ( iAmountOfCreditsCurrency == NULL )
        {
        return KNullDesC;
        }
    return *iAmountOfCreditsCurrency;
    }
    
TInt CNcdServerSubscription::NumberOfDownloads() const
    {
    return iNumberOfDownloads;
    }
