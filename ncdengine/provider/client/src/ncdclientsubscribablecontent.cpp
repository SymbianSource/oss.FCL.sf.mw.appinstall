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


#include "ncdclientsubscribablecontent.h"
#include "catalogsdebug.h"

CNcdClientSubscribableContent::CNcdClientSubscribableContent()
    {
    }

void CNcdClientSubscribableContent::ConstructL()
    {
    ResetMemberVariables();
    }

CNcdClientSubscribableContent* CNcdClientSubscribableContent::NewL()
    {
    CNcdClientSubscribableContent* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CNcdClientSubscribableContent* CNcdClientSubscribableContent::NewLC()
    {
    CNcdClientSubscribableContent* self =
        new (ELeave) CNcdClientSubscribableContent;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CNcdClientSubscribableContent::~CNcdClientSubscribableContent()
    {
    ResetMemberVariables();
    }


void CNcdClientSubscribableContent::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    
    DLINFO(( "Internalizing subscribable content info." ));
    
    ResetMemberVariables();
    
    TInt64 intValidUntil( 0 );
    aStream >> intValidUntil;
    iValidUntil = intValidUntil;
    DLINFO(( "Valid until as integer: %Ld", iValidUntil.Int64() ));
    iValidUntilSet = aStream.ReadInt32L();

    iSubscriptionType = 
        static_cast<MNcdSubscription::TType>(aStream.ReadInt32L());    
    DLINFO(( "Subscription type: %d", iSubscriptionType ));
        
    iChildSeparatelyPurchasable = aStream.ReadInt32L();
    DLINFO(( "Child separately purchaseable: %d",
             iChildSeparatelyPurchasable ));    
    
    DLTRACEOUT((""));
    }




TTime CNcdClientSubscribableContent::ValidUntil() const
    {
    return iValidUntil;
    }

TBool CNcdClientSubscribableContent::ValidUntilSet() const
    {
    return iValidUntilSet;
    }

TBool CNcdClientSubscribableContent::ChildSeparatelyPurchasable() const
    {
    return iChildSeparatelyPurchasable;
    }

    
void CNcdClientSubscribableContent::ResetMemberVariables()
    {
    iValidUntil = 0;
    iValidUntilSet = EFalse;
    
    // It does not matter so much which type of the subscription is used
    // as the initial value. Probably if it is of any type that is incorrect
    // things are not looking great.
    
    iChildSeparatelyPurchasable = ETrue;    
    }
