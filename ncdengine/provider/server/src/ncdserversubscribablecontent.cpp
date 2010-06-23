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


#include "ncdserversubscribablecontent.h"
#include "ncd_pp_dataentitycontent.h"
#include "catalogsutils.h"

CNcdServerSubscribableContent::CNcdServerSubscribableContent()
    {
    }

void CNcdServerSubscribableContent::ConstructL()
    {
    ResetMemberVariables();
    }

CNcdServerSubscribableContent* CNcdServerSubscribableContent::NewL()
    {
    CNcdServerSubscribableContent* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CNcdServerSubscribableContent* CNcdServerSubscribableContent::NewLC()
    {
    CNcdServerSubscribableContent* self =
        new (ELeave) CNcdServerSubscribableContent;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CNcdServerSubscribableContent::~CNcdServerSubscribableContent()
    {
    ResetMemberVariables();
    }


void CNcdServerSubscribableContent::InternalizeL(
    const MNcdPreminetProtocolDataEntityContent& aData )
    {
    // Current time saved in the beginning for later use.
    // Done as soon as possible so it won't be too long
    // after message is received from the server and delta
    // values are hopefully still valid.
    TTime now;
    now.HomeTime();

    ResetMemberVariables();


    TInt validUntilDelta = aData.ValidUntilDelta();
    // If validuntil is zero, it is determined to mean
    // that the validuntil was not received and is not in use
    if ( validUntilDelta > 0 )
        {
        iValidUntilSet = ETrue;
        }
    
    
    // validUntilDelta is in minutes so we use TTimeIntervalMinutes
    // to reprsent the interval
    TTimeIntervalMinutes validUntilInterval( validUntilDelta );        
    iValidUntil = now + validUntilInterval;

    
    TNcdSubscriptionType tmpType = aData.SubscriptionType();
    DASSERT( tmpType != ENotSubscribable );
    
    switch( tmpType )
        {
        case EPeriodic:
            iSubscriptionType = MNcdSubscription::EPeriodic;
            break;

        case EAutomaticContinous:
            iSubscriptionType = MNcdSubscription::EAutomaticContinous;
            break;

        default:
            DASSERT( false ); // Should not end up here
            break;
        }

    
    iChildSeparatelyPurchasable = aData.ChildSeparatelyPurchasable();
    }

void CNcdServerSubscribableContent::ExternalizeL( RWriteStream& aStream )
    {
    const TInt64& intValidUntil = iValidUntil.Int64();
    aStream << intValidUntil;
    aStream.WriteInt32L( iValidUntilSet );

    aStream.WriteInt32L( iSubscriptionType );    
    aStream.WriteInt32L( iChildSeparatelyPurchasable );
    }

void CNcdServerSubscribableContent::InternalizeL( RReadStream& aStream )
    {
    ResetMemberVariables();
    
    TInt64 intValidUntil( 0 );
    aStream >> intValidUntil;
    iValidUntil = intValidUntil;
    iValidUntilSet = aStream.ReadInt32L();    
    
    iSubscriptionType = 
        static_cast<MNcdSubscription::TType>(aStream.ReadInt32L());
        
    iChildSeparatelyPurchasable = aStream.ReadInt32L();
    }


MNcdSubscription::TType
    CNcdServerSubscribableContent::SubscriptionType() const
    {
    return iSubscriptionType;
    }

    
void CNcdServerSubscribableContent::ResetMemberVariables()
    {
    iValidUntil = 0;
    iValidUntilSet = EFalse;
    
    // It does not matter so much which type of the subscription is used
    // as the initial value. Probably if it is of any type that is incorrect
    // things are not looking great.
    
    iChildSeparatelyPurchasable = ETrue;    
    }
