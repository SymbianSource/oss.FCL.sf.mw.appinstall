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


#include "ncdclientsubscription.h"




// ======== MEMBER FUNCTIONS ========


CNcdClientSubscription::CNcdClientSubscription()
    {
    }

void CNcdClientSubscription::ConstructL()
    {
    }


CNcdClientSubscription* CNcdClientSubscription::NewL()
    {
    CNcdClientSubscription* self = 
        CNcdClientSubscription::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


CNcdClientSubscription* CNcdClientSubscription::NewLC()
    {
    CNcdClientSubscription* self =
        new( ELeave ) CNcdClientSubscription();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdClientSubscription::~CNcdClientSubscription()
    {
    delete iAmountOfCreditsCurrency;
    }
    





void CNcdClientSubscription::SetValidityDelta( 
    TInt aValidityDelta )
    {
    iValidityDelta = aValidityDelta;
    }
    
void CNcdClientSubscription::SetValidityAutoUpdate( 
    TBool aValidityAutoUpdate )
    {
    iValidityAutoUpdate = aValidityAutoUpdate;
    }
    
void CNcdClientSubscription::SetAmountOfCredits( 
    TReal32 aAmountOfCredits )
    {
    iAmountOfCredits = aAmountOfCredits;
    }
    
void CNcdClientSubscription::SetAmountOfCreditsCurrency( 
    HBufC* aAmountOfCreditsCurrency )
    {
    delete iAmountOfCreditsCurrency;
    iAmountOfCreditsCurrency = aAmountOfCreditsCurrency;
    }
    
void CNcdClientSubscription::SetNumberOfDownloads(
    TInt aNumberOfDownloads )
    {
    iNumberOfDownloads = aNumberOfDownloads;
    }
