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
* Description:   Implements CNcdSubscription class
*
*/


#include "ncdsubscriptionimpl.h"

#include "ncdpurchaseoptionimpl.h"
#include "ncdserversubscription.h"
#include "ncdserverpartofsubscription.h"
#include "ncdserverupgrade.h"
#include "ncd_pp_subscriptiondetails.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsconstants.h"
#include "ncd_pp_dataentity.h"
#include "ncd_cp_query.h"
#include "ncd_pp_subscription.h"
#include "catalogsutils.h"
#include "ncdserversubscribablecontent.h"
#include "ncdsubscriptiongroup.h"
#include "ncdprotocolutils.h"

#include "catalogsdebug.h"


CNcdSubscription::CNcdSubscription( CNcdSubscriptionGroup& aParentGroup )
    : CCatalogsCommunicable(),
      iParentGroup( aParentGroup )
    {
    }

void CNcdSubscription::ConstructL()
    {
    }


CNcdSubscription* CNcdSubscription::NewL(
    CNcdSubscriptionGroup& aParentGroup )
    {
    CNcdSubscription* self =   
        CNcdSubscription::NewLC( aParentGroup );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdSubscription* CNcdSubscription::NewLC(
    CNcdSubscriptionGroup& aParentGroup )
    {
    CNcdSubscription* self = 
        new( ELeave ) CNcdSubscription( aParentGroup );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }

CNcdSubscription::~CNcdSubscription()
    {
    DLTRACEIN((""));

    ResetMemberVariables();

    DLTRACEOUT((""));
    }        

const TDesC& CNcdSubscription::SubscriptionName() const
    {
    DLTRACEIN((""));
    if ( iName == NULL )
        {
        DLTRACEOUT((""));
        return KNullDesC;
        }
    else
        {
        DLTRACEOUT((""));
        return *iName;
        }      
    }

void CNcdSubscription::SetSubscriptionNameL( const TDesC& aNewName )
    {
    delete iName;
    iName = NULL;
    iName = aNewName.AllocL();
    }

const TDesC& CNcdSubscription::PurchaseOptionId() const
    {
    DLTRACEIN((""));
    if ( iPurchaseOptionId == NULL )
        {
        DLTRACEOUT((""));
        return KNullDesC;
        }
    else
        {
        DLTRACEOUT((""));
        return *iPurchaseOptionId;
        }    
    }

    
CNcdSubscriptionGroup& CNcdSubscription::ParentGroup() const 
    {
    DLTRACEIN((""));
    return iParentGroup;
    }
    
MNcdSubscription::TType CNcdSubscription::SubscriptionType() const 
    {
    DLTRACEIN((""));
    return iSubscriptionType;
    }

void CNcdSubscription::InternalizeL( const CNcdPurchaseOptionImpl& aData )
    {
    DLTRACEIN(("Subscriptionimpl internalizing from purchase option."));
    
    // Current time saved in the beginning for later use.
    // Done as soon as possible so it won't be too long
    // after message is received from the server and delta
    // values are hopefully still valid.
    TTime now;
    now.HomeTime();

    
    ResetMemberVariables();
    
    iName = aData.PurchaseOptionName().AllocL();
    DLTRACE(( _L("Name: %S."), iName ));

    // Cannot get expired on info from the purchaseoption and
    // if this subscription is just bought as it should be, it
    // has not expired.
    // Expired on is left into its initial value.
    
    // The same thing is with iCancelled. It is assumed that as the
    // subscription is just bought, it is not unsubscribed yet.


    // Get subscription type from the subscribable content
    const CNcdServerSubscribableContent* subscribableContent =
        aData.ParentSubscribableContent();
    
    // Protocol makes it possible that although we have a node that has
    // a purchaseoption that can be used to buy a subscription the node
    // does not need to have a subscribable content field.    
    if ( subscribableContent != NULL )
        {
        iSubscriptionType = subscribableContent->SubscriptionType();
        DLINFO(( "Subscriptionimpl internalizing, type: %d",
                 iSubscriptionType ));        
        }
    else
        {
        // We use the default type of subscription
        DLINFO(( "Subscribable content not found, have to use default type: %d",
                 iSubscriptionType ));
        }


    // Next we set total values if they are set in the subscription

    const CNcdServerSubscription* subscriptionInfo =
        aData.SubscriptionInfo();
    if ( subscriptionInfo == NULL )
        {
        User::Leave( KErrArgument );
        }

 
    iTotalValidityDelta = subscriptionInfo->ValidityDelta();
    DLINFO(( "Total validitydelta: %d.", iTotalValidityDelta ));
    if ( iTotalValidityDelta >= 0 )
        {
        iValidityTimeSet = ETrue;
        }
    DLINFO(( "ValidityTimeSet: %d.", iValidityTimeSet ));
    
    iTotalCredits = subscriptionInfo->AmountOfCredits();
    DLINFO(( "Total credits: %f.", iTotalCredits ));
    // Simple >0 is decided to be enough because no complicated operations
    // are done with TReal numbers
    if ( iTotalCredits >= 0 )
        {
        iCreditLimitSet = ETrue;
        }
    
    iTotalDownloads = subscriptionInfo->NumberOfDownloads();
    DLINFO(( "Total downloads: %d.", iTotalDownloads ));
    if ( iTotalDownloads >= 0 )
        {
        iDownloadLimitSet = ETrue;
        }
    
    
    // Because this function should be used just after purchase
    // of the subscription, we set maximum values to the
    // "left" variables

    // validityDelta is in minutes so we use TTimeIntervalMinutes
    // to reprsent the interval
    TTimeIntervalMinutes validityInterval( iTotalValidityDelta );
    iValidUntil = now + validityInterval;
 
    iCreditsLeft = iTotalCredits;
    iDownloadsLeft = iTotalDownloads;

    iPurchaseOptionId = aData.Id().AllocL();
    
    DLTRACEOUT((""));
    }




void CNcdSubscription::InternalizeL(
    MNcdPreminetProtocolSubscription& aData )
    {
    DLTRACEIN(("Subscriptionimpl internalizing from protocol."));
    
    // Current time saved in the beginning for later use.
    // Done as soon as possible so it won't be too long
    // after message is received from the server and delta
    // values are hopefully still valid.
    TTime now;
    now.HomeTime();


    // ResetMemberVariables function is not called here
    // because it deletes all member variables and
    // we cannot recreate all of them from the given protocol object.
    
    // Instead ResetProtocolMemberVariables is called
    ResetProtocolMemberVariables();
    
    
    // Nothing is done to existing name and icon
    // info because hopefully they are received some other way
    // (from purchase option)

    
    iExpiredOn = aData.ExpiredOn().AllocL();
    
    iCancelled = aData.Cancelled();
    
    TNcdSubscriptionType tmpType = aData.Type();
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

    
    // If we have a expired subscription TotalUsageRights is
    // assumed to return usagerights of the expired subscription
    const MNcdPreminetProtocolSubscriptionDetails* totalRights =
        aData.TotalUsageRights();
   
    DASSERT( totalRights != NULL );
        
    if ( totalRights != NULL )
        {
        // We assume here that if any of the original limits or
        // validity negative then that particular limit or validity
        // is not in use.
        
        iTotalValidityDelta = totalRights->ValidityDelta();
        if ( iTotalValidityDelta >= 0 )
            {
            iValidityTimeSet = ETrue;
            }
                    

        iTotalCredits = totalRights->AmountOfCredits();
        // Also here >0 is deemed to be enough because no complicated
        // operations are done on the TReal numbers
        if ( iTotalCredits >= 0 )
            {
            iCreditLimitSet = ETrue;
            }

        iTotalDownloads = totalRights->NumberOfDownloads();
        if ( iTotalDownloads >= 0 )
            {
            iDownloadLimitSet = ETrue;
            }
        }


    const MNcdPreminetProtocolSubscriptionDetails* remainingRights =
        aData.RemainingUsageRights();

    TInt validityDelta( -1 );
    if ( remainingRights != NULL )
        {
        validityDelta = remainingRights->ValidityDelta();
            
        // validityDelta is in minutes so we use TTimeIntervalMinutes
        // to reprsent the interval
        TTimeIntervalMinutes validityInterval( validityDelta );        
        iValidUntil = now + validityInterval;

        iCreditsLeft = remainingRights->AmountOfCredits();
        iDownloadsLeft = remainingRights->NumberOfDownloads();
        }


    // expiredOn is set to iValidUntil so that client can receive
    // some kind of validuntil (from proxyside interface)
    // even if the subscription has already expired.
    // This way iValidUntil is not necessiraly the original validUntil.
    // For example in situation where other constraint has run out first
    // (for example credits), iValidUntil would be this expiration
    // point. Anyway it is close enough.
    // NOTICE: iValidUntil of expired subscription cannot be greater
    //         than "now" because server does not return
    //         remaining usagerights of expired subscriptions
    if ( iValidityTimeSet && validityDelta == -1 && iExpiredOn != NULL &&
         *iExpiredOn != KNullDesC )
        {
        iValidUntil = NcdProtocolUtils::DesToTimeL( *iExpiredOn ); 
        }

    iPurchaseOptionId = aData.PurchaseOptionId().AllocL();
        
    DLTRACEOUT((""));
    }


void CNcdSubscription::SetRecentlyUpdated( TBool aNewState )
    {
    DLTRACEIN((""));
    iRecentlyUpdated = aNewState;
    DLTRACEOUT((""));
    }
   
TBool CNcdSubscription::RecentlyUpdated() const
    {
    DLTRACEIN((""));
    DLTRACEOUT((""));
    return iRecentlyUpdated;
    }



// Internalization from and externalization to the database
    
void CNcdSubscription::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    ExternalizeOwnDataL( aStream );

    DLTRACEOUT((""));
    }


void CNcdSubscription::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    ResetMemberVariables();

    InternalizeDesL( iName, aStream );
    InternalizeDesL( iExpiredOn, aStream );
    
    iCancelled = aStream.ReadInt32L();

    iSubscriptionType = 
        static_cast<MNcdSubscription::TType>(aStream.ReadInt32L());

    iValidityTimeSet = aStream.ReadInt32L();
    TInt64 intValidUntil( 0 );
    aStream >> intValidUntil;
    iValidUntil = intValidUntil;
    iTotalValidityDelta = aStream.ReadInt32L();
    
    iCreditLimitSet = aStream.ReadInt32L();
    iCreditsLeft = aStream.ReadReal32L();
    iTotalCredits = aStream.ReadReal32L();

    iDownloadLimitSet = aStream.ReadInt32L();
    iDownloadsLeft = aStream.ReadInt32L();
    iTotalDownloads = aStream.ReadInt32L();

    InternalizeDesL( iPurchaseOptionId, aStream );
    
    DLTRACE(( _L(" Internalized subscription, purchaseoptionid: %S"),
              iPurchaseOptionId ));

    DLTRACEOUT((""));
    }


void CNcdSubscription::ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                  TInt aFunctionNumber )
    {
    DLTRACEIN((""));    

    DASSERT( aMessage );
    
    // Now, we can be sure that rest of the time iMessage exists.
    // This member variable is set for the CounterPartLost function.
    iMessage = aMessage;
    
    TInt trapError( KErrNone );
    
    // Check which function is called by the proxy side object.
    // Function number are located in ncdnodefunctinoids.h file.
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdInternalize:
            // Internalize the proxy side according to the data
            // of this object.
            TRAP( trapError, InternalizeRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdRelease:
            // The proxy does not want to use this object anymore.
            // So, release the handle from the session.
            ReleaseRequest( *aMessage );
            break;
                    
        default:
            break;
        }

    if ( trapError != KErrNone )
        {
        // Because something went wrong, the complete has not been
        // yet called for the message.
        // So, inform the client about the error if the
        // message is still available.
        aMessage->CompleteAndRelease( trapError );
        }

    // Because the message should not be used after this, set it NULL.
    // So, CounterPartLost function will know that no messages are
    // waiting the response at the moment.
    iMessage = NULL;        
    
    DLTRACEOUT((""));
    }

void CNcdSubscription::CounterPartLost( const MCatalogsSession& aSession )
    {
    // This function may be called whenever -- when the message is waiting
    // response or when the message does not exist.
    // iMessage may be NULL here, because in the end of the
    // ReceiveMessage it is set to NULL. The life time of the message
    // ends shortly after CompleteAndRelease is called.
    if ( iMessage != NULL )
        {
        iMessage->CounterPartLost( aSession );
        }
    }
                

void CNcdSubscription::InternalizeRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );


    // Include all the necessary node data to the stream
    ExternalizeDataForRequestL( stream );     
    
    
    // Commits data to the stream when closing.
    CleanupStack::PopAndDestroy( &stream );


    // If this leaves, ReceiveMessage will complete the message.
    // NOTE: that here we expect that the buffer contains at least
    // some data. So, make sure that ExternalizeDataForRequestL inserts
    // something to the buffer.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );        
        
    
    DLTRACE(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
        
    DLTRACEOUT((""));
    }
    

void CNcdSubscription::ExternalizeDataForRequestL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    ExternalizeOwnDataL( aStream );
        
    DLTRACEOUT((""));
    }

void CNcdSubscription::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );

    // Send complete information back to proxy.
    aMessage.CompleteAndRelease( KErrNone );
        
    // Remove this object from the session.
    requestSession.RemoveObject( handle );
        
    DLTRACEOUT((""));
    }




void CNcdSubscription::UseL( const CNcdPurchaseOptionImpl& aData )
    {
    DLTRACEIN((""));
    // If subscription is used, then the purchaseoption
    // has to have a partOfSubscription component.
    
    const CNcdServerPartOfSubscription* partOfSubscriptionInfo =
        aData.PartOfSubscriptionInfo();
        
    if ( partOfSubscriptionInfo == NULL )
        {
        User::Leave( KErrArgument );
        }    

    if ( aData.IsFree() )
        {
        // Free purchase from subscription
        // Do nothing
        return;
        }

    TReal32 creditPrice( partOfSubscriptionInfo->CreditPrice() );
    // Only simple floating point operations are done so simple
    // checking (>0) is also decided to be enough.
    if ( creditPrice >= 0 )
        {
        //DASSERT( iCreditLimitSet );        
        iCreditsLeft = iCreditsLeft - creditPrice;
        }

    if ( iDownloadLimitSet )
        {
        --iDownloadsLeft;        
        }

    if ( !(creditPrice >= 0) && !iDownloadLimitSet )
        {
        // This would be quite weird situation where purchase is not
        // free but no cost information is provided
        DLINFO(("Purchase with subscription is not free but it contains no cost information!"));
        }

    DLTRACEOUT((""));
    }



void CNcdSubscription::UpgradeL( const CNcdPurchaseOptionImpl& aData )
    {
    // If subscription is upgraded, then the purchaseoption should
    // have upgrade component.

    const CNcdServerUpgrade* upgradeInfo = aData.UpgradeInfo(); 
    if ( upgradeInfo == NULL )
        {
        User::Leave( KErrArgument );
        }

    // All values received in upgrade are just added to the
    // current values. They have to be added also to total values
    // so that the info can be used for example when renewing.


    TInt validityDelta = upgradeInfo->ValidityDelta();
    // validityDelta is in minutes so we use TTimeIntervalMinutes
    // to represent the interval
    if ( validityDelta >= 0 )
        {
        TTimeIntervalMinutes validityInterval( validityDelta );        
        iValidUntil = iValidUntil + validityInterval;
        iTotalValidityDelta = iTotalValidityDelta + validityDelta;        
        }
    else
        {
        // This constraint is not in use anymore
        iValidityTimeSet = EFalse;
        iValidUntil = -1;
        iTotalValidityDelta = -1;
        }


    TInt creditsLeft( upgradeInfo->AmountOfCredits() );
    if ( iCreditsLeft >= 0 )
        {
        iCreditsLeft = iCreditsLeft + creditsLeft;
        iTotalCredits = iTotalCredits + creditsLeft;        
        }
    else
        {
        // This constraint is not in use anymore
        iCreditLimitSet = EFalse;
        iCreditsLeft = -1;
        iTotalCredits = -1;
        }

    TInt downloadsLeft( upgradeInfo->NumberOfDownloads() );
    if ( downloadsLeft >= 0 )
        {
        iDownloadsLeft = iDownloadsLeft + downloadsLeft;
        iTotalDownloads = iTotalDownloads + downloadsLeft;        
        }
    else
        {
        // This constraint is not in use anymore
        iDownloadLimitSet = EFalse;
        iDownloadsLeft = -1;
        iTotalDownloads = -1;
        }
    }

void CNcdSubscription::ExternalizeOwnDataL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    if ( iName != NULL )
        {
        ExternalizeDesL( *iName, aStream );
        DLTRACE(( _L("Externalizing subscription info, name: %S"),
                  iName ));
        }
    else
        {
        ExternalizeDesL( KNullDesC, aStream );
        DLINFO(( "Externalizing subscription info, no name found." ));
        }

    if ( iExpiredOn != NULL )
        {
        ExternalizeDesL( *iExpiredOn, aStream );
        DLTRACE(( _L("Externalizing subscription info, expired on: %S"),
                  iExpiredOn ));
        }
    else
        {
        ExternalizeDesL( KNullDesC, aStream );
        DLINFO(( "Externalizing subscription info, no expired on info found." ));
        }

    aStream.WriteInt32L( iCancelled );

    // NOTICE: This is a little bit dangerous. If the
    //         subscription type is not found, it is the default
    //         value and that is externalized.
    aStream.WriteInt32L( iSubscriptionType );

    aStream.WriteInt32L( iValidityTimeSet );
    const TInt64& intValidUntil = iValidUntil.Int64();
    aStream << intValidUntil;
    aStream.WriteInt32L( iTotalValidityDelta );
    
    aStream.WriteInt32L( iCreditLimitSet );
    aStream.WriteReal32L( iCreditsLeft );
    aStream.WriteReal32L( iTotalCredits );

    aStream.WriteInt32L( iDownloadLimitSet );
    aStream.WriteInt32L( iDownloadsLeft );
    aStream.WriteInt32L( iTotalDownloads );

    DASSERT( iPurchaseOptionId != NULL );
    ExternalizeDesL( *iPurchaseOptionId, aStream );
    
    DLTRACE(( _L(" Externalized subscription, purchaseoptionid: %S"),
              iPurchaseOptionId ));

    DLTRACEOUT((""));    
    }


void CNcdSubscription::ResetProtocolMemberVariables()
    {
    // In all cases this state variable is in the beginning false
    // This has to be set especially by the user.
    iRecentlyUpdated = EFalse;
    
    delete iPurchaseOptionId;
    iPurchaseOptionId = NULL;

    // Name 
    // cannot be deleted as they are not received
    // from the subscription protocol element.
   
    delete iExpiredOn;
    iExpiredOn = NULL;
    
    iCancelled = EFalse;
    
    iSubscriptionType = MNcdSubscription::EPeriodic;
    
    iValidityTimeSet = EFalse;
    iValidUntil = -1;
    iTotalValidityDelta = -1;

    iCreditLimitSet = EFalse;
    iCreditsLeft = -1;
    iTotalCredits = -1;
    
    iDownloadLimitSet = EFalse;
    iDownloadsLeft = -1;
    iTotalDownloads = -1;    
    }

void CNcdSubscription::ResetMemberVariables()
    {
    delete iName;
    iName = NULL;
    
    ResetProtocolMemberVariables();
    }
