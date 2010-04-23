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
* Description:   Implements CNcdSubscriptionGroupGroup class
*
*/


#include "ncdsubscriptiongroup.h"
#include "ncdsubscriptionimpl.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "catalogsconstants.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncd_pp_subscription.h"
#include "ncdpurchaseoptionimpl.h"
#include "ncdserverpartofsubscription.h"
#include "ncdserverupgrade.h"
#include "ncdnodeidentifier.h"


CNcdSubscriptionGroup::CNcdSubscriptionGroup() : CCatalogsCommunicable()
    {
    }

void CNcdSubscriptionGroup::ConstructL()
    {
    iOriginNodeIdentifier = CNcdNodeIdentifier::NewL();
    iIcon = KNullDesC8().AllocL();
    } 

CNcdSubscriptionGroup* CNcdSubscriptionGroup::NewL()
    {
    CNcdSubscriptionGroup* self =   
        CNcdSubscriptionGroup::NewLC();
    CleanupStack::Pop( self );
    return self;        
    }

CNcdSubscriptionGroup* CNcdSubscriptionGroup::NewLC()
    {
    CNcdSubscriptionGroup* self = 
        new( ELeave ) CNcdSubscriptionGroup();
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdSubscriptionGroup::~CNcdSubscriptionGroup()
    {
    DLTRACEIN((""));

    ResetMemberVariables();

    DLTRACEOUT((""));
    }

void CNcdSubscriptionGroup::InternalizeSubscriptionL(
    const CNcdPurchaseOptionImpl& aData )
    {
    DLTRACEIN((""));

    // get old subscription or if not found create a new one

    CNcdSubscription& subscription = SubscriptionL( aData.Id() );
    
    // We have index of the subscription so use the purchase
    // option on it.
    subscription.InternalizeL( aData );
    DLTRACEOUT((""));
    }


void CNcdSubscriptionGroup::InternalizeSubscriptionL(
    MNcdPreminetProtocolSubscription& aSubscription )
    {
    DLTRACEIN((""));

    // get old subscription or if not found create a new one

    CNcdSubscription& subscription =
        SubscriptionL( aSubscription.PurchaseOptionId() );
    
    // We have index of the subscription so use the protocol
    // entity on it.
    subscription.InternalizeL( aSubscription );
    DLTRACEOUT((""));
    }

        
void CNcdSubscriptionGroup::RemoveSubscriptionL(
    const TDesC& aPurchaseOptionId )
    {
    TInt index = FindSubscriptionL( aPurchaseOptionId );
    
    // Manager has one reference count to all objects
    // so that for one Close is said to the subscription.
    iSubscriptions[index]->Close();
    iSubscriptions.Remove( index );
    }

void CNcdSubscriptionGroup::SetRecentlyUpdatedL(
    TBool aNewState,
    const TDesC& aPurchaseOptionId )
    {
    DLTRACEIN((""));
    // Leave if no subscription found.
    TInt index = FindSubscriptionL( aPurchaseOptionId );
    DLINFO(("subscription found"));
    CNcdSubscription* subscription = iSubscriptions[index];
    subscription->SetRecentlyUpdated( aNewState );
    DLTRACEOUT((""));
    }
   
TBool CNcdSubscriptionGroup::RecentlyUpdatedL(
    const TDesC& aPurchaseOptionId ) const
    {
    DLTRACEIN((""));
    // Leave if no subscription found.
    TInt index = FindSubscriptionL( aPurchaseOptionId );
    CNcdSubscription* subscription = iSubscriptions[index];
    TBool recentlyCreated = subscription->RecentlyUpdated();
    DLTRACEOUT((""));
    return recentlyCreated;
    }

TBool CNcdSubscriptionGroup::RemoveUnmarkedSubscriptionsAndUnmarkL()
    {
    TBool changesMade( EFalse );

    TInt subscriptionIndexer( iSubscriptions.Count() - 1 );
    while ( subscriptionIndexer > -1 )
        {        
        if ( !iSubscriptions[subscriptionIndexer]->RecentlyUpdated() )
            {
            // Manager has one reference count to all objects
            // so that for one Close is said to the subscription.
            iSubscriptions[subscriptionIndexer]->Close();
            iSubscriptions.Remove( subscriptionIndexer );
            changesMade = ETrue;
            }
        else
            {
            // Resetting the flag for later use
            iSubscriptions[subscriptionIndexer]->
                SetRecentlyUpdated( EFalse );
            }                
        --subscriptionIndexer;
        }
    return changesMade;    
    }

    
void CNcdSubscriptionGroup::SetEntityInfoL( const TDesC& aEntityId,
                                            const TDesC& aNamespace,
                                            const TDesC& aServerUri,
                                            const TUid aClientUid )
    {
    DLTRACEIN((""));
    
    delete iOriginNodeIdentifier;
    iOriginNodeIdentifier = NULL;
    iOriginNodeIdentifier = CNcdNodeIdentifier::NewL( aNamespace,
                                                      aEntityId,
                                                      aServerUri,
                                                      aClientUid );
    }

const TDesC8& CNcdSubscriptionGroup::Icon()
    {
    // Probably would be good just to take a pointer here
    // not to create a copy
    DLTRACEIN((""));
    if( iIcon == NULL )
        {
        return KNullDesC8;
        }
        
    return *iIcon;
    }

void CNcdSubscriptionGroup::SetIconL( const TDesC8& aIcon )
    {
    DLTRACEIN((""));
    delete iIcon;
    iIcon = NULL;
    iIcon = aIcon.AllocL();
    }

const TDesC& CNcdSubscriptionGroup::EntityId() const
    {
    return iOriginNodeIdentifier->NodeId();
    }

const TDesC& CNcdSubscriptionGroup::Namespace() const
    {
    return iOriginNodeIdentifier->NodeNameSpace();
    }
    
const TDesC& CNcdSubscriptionGroup::ServerUri() const
    {
    return iOriginNodeIdentifier->ServerUri();
    }

CNcdNodeIdentifier& CNcdSubscriptionGroup::OriginNodeIdentifier() const
    {
    return *iOriginNodeIdentifier;
    }

CNcdSubscription& CNcdSubscriptionGroup::SubscriptionIfExistsL(
    const TDesC& aPurchaseOptionId ) const 
    {
    DLTRACEIN((""));
    TInt index = FindSubscriptionL( aPurchaseOptionId );
    return *iSubscriptions[index];
    }

TInt CNcdSubscriptionGroup::SubscriptionCount() const
    {
    return iSubscriptions.Count();
    }

void CNcdSubscriptionGroup::AppendIncompleteSubscriptionIDsL(
    RPointerArray<CNcdNodeIdentifier>& aNodeIds,
    CDesCArrayFlat& aPurchaseOptionIDs )
    {
    TInt subscriptionIndex( 0 );
    const TInt KSubscriptionCount( iSubscriptions.Count() );
    while( subscriptionIndex < KSubscriptionCount )
        {
        if ( iSubscriptions[subscriptionIndex]->Name() == KNullDesC ||
             iIcon == NULL || *iIcon == KNullDesC8 )
            {
            aNodeIds.AppendL(
                CNcdNodeIdentifier::NewL( OriginNodeIdentifier() ) );
            aPurchaseOptionIDs.AppendL(
                iSubscriptions[subscriptionIndex]->PurchaseOptionId() );
            }
        ++subscriptionIndex;
        }
    }
      
// Internalization from and externalization to the database
    
void CNcdSubscriptionGroup::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    // Set all the membervariable values to the stream. So,
    // that the stream may be used later to create a new
    // object.

    iOriginNodeIdentifier->ExternalizeL( aStream );

    DLTRACE(( _L(" Externalizing subscriptiongroup info, EntityId: %S, Namespace: %S, server uri: %S"),
              &iOriginNodeIdentifier->NodeId(),
              &iOriginNodeIdentifier->NodeNameSpace(), 
              &iOriginNodeIdentifier->ServerUri() ));

    if ( iIcon != NULL )
        {
        ExternalizeDesL( *iIcon, aStream );
        DLTRACE(( "Externalizing subscription info, icon" ));
        }
    else
        {
        ExternalizeDesL( KNullDesC8, aStream );
        DLINFO(( "Externalizing subscription info, no icon found." ));
        }

    TInt32 subscriptionAmount( iSubscriptions.Count() );
    aStream.WriteInt32L( subscriptionAmount );

    DLTRACE(( _L( "Externalizing also %d subscriptions." ),
              subscriptionAmount ));

    
    TInt32 subscriptionIndexer( 0 );
    while ( subscriptionIndexer < subscriptionAmount )
        {        
        iSubscriptions[subscriptionIndexer]->ExternalizeL( aStream );        
        ++subscriptionIndexer;
        }

    DLTRACEOUT((""));
    }

void CNcdSubscriptionGroup::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    ResetMemberVariables();

    if ( !iOriginNodeIdentifier )
        {
        iOriginNodeIdentifier = CNcdNodeIdentifier::NewL();
        }        
    iOriginNodeIdentifier->InternalizeL( aStream );

    DLTRACE(( _L(" Internalizing subscriptiongroup info, EntityId: %S, Namespace: %S, server uri: %S"),
              &iOriginNodeIdentifier->NodeId(),
              &iOriginNodeIdentifier->NodeNameSpace(), 
              &iOriginNodeIdentifier->ServerUri() ));

    InternalizeDesL( iIcon, aStream );

    TInt32 subscriptionAmount( aStream.ReadInt32L() );

    DLTRACE(( _L( "Internalizing also %d subscriptions." ),
              subscriptionAmount ));
    
    TInt32 subscriptionIndexer( 0 );
    while ( subscriptionIndexer < subscriptionAmount )
        {        
        CNcdSubscription* tempSubscription = CNcdSubscription::NewLC( *this );
        iSubscriptions.AppendL( tempSubscription );        
        CleanupStack::Pop( tempSubscription );
        
        tempSubscription->InternalizeL( aStream );
        
        ++subscriptionIndexer;
        }

    DLTRACEOUT((""));
    }



void CNcdSubscriptionGroup::ReceiveMessage( MCatalogsBaseMessage* aMessage,
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
        case NcdNodeFunctionIds::ENcdPurchaseOptionIds:
            // Purchase option ids of subscriptions requested from proxy side.
            TRAP( trapError, PurchaseOptionIdsRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdInternalize:
            // Internalize the proxy side according to the data
            // of this object.
            TRAP( trapError, InternalizeRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdSubscriptionIconData:
            // Icon data of this subscription group
            TRAP( trapError, IconDataRequestL( *aMessage ) );
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

void CNcdSubscriptionGroup::CounterPartLost(
    const MCatalogsSession& aSession )
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


void CNcdSubscriptionGroup::PurchaseOptionIdsRequestL(
    MCatalogsBaseMessage& aMessage ) const 
    {
    DLTRACEIN((""));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
   
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );
    
    stream.WriteInt32L( iSubscriptions.Count() );
    for ( TInt i = 0; i < iSubscriptions.Count(); i++ ) 
        {
        ExternalizeDesL( iSubscriptions[i]->PurchaseOptionId(), stream );
        }
        
    CleanupStack::PopAndDestroy( &stream );
    
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );
    CleanupStack::PopAndDestroy( buf );
    }


void CNcdSubscriptionGroup::InternalizeRequestL(
    MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));
    

    // Read the purchase option ids from the message obtained.
    CDesCArray* poIds = new( ELeave ) CDesCArrayFlat( 5 );
    CleanupStack::PushL( poIds );
    RBuf8 inputMsg;
    CleanupClosePushL( inputMsg );
    inputMsg.CreateL( aMessage.InputLength() );
    User::LeaveIfError( aMessage.ReadInput( inputMsg ) );
    
    RDesReadStream inputStream( inputMsg );
    CleanupClosePushL( inputStream );
    
    TInt subscriptionCount = inputStream.ReadInt32L();
    for ( TInt i = 0; i < subscriptionCount; i++ ) 
        {
        HBufC* poId( NULL );
        InternalizeDesL( poId, inputStream );
        CleanupStack::PushL( poId );
        poIds->AppendL( *poId );
        CleanupStack::PopAndDestroy( poId );
        }
        
    CleanupStack::PopAndDestroy( &inputStream );
    CleanupStack::PopAndDestroy( &inputMsg );    
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    // Session info has to be passed to be able
    // to register objects as receivers into the session
    MCatalogsSession& session = aMessage.Session();
    
    // Include all the necessary node data to the stream
    ExternalizeDataForRequestL( session, *poIds, stream );     
    
    
    // Commits data to the stream when closing.
    CleanupStack::PopAndDestroy( &stream );


    // If this leaves, ReceiveMessage will complete the message.
    // NOTE: that here we expect that the buffer contains at least
    // some data. So, make sure that ExternalizeDataForRequestL inserts
    // something to the buffer.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );        
        
    
    DLTRACE(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( poIds );
        
    DLTRACEOUT((""));
    }
    
void CNcdSubscriptionGroup::IconDataRequestL(
    MCatalogsBaseMessage& aMessage ) const 
    {
    DLTRACEIN((""));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
   
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );
    
    ExternalizeDesL( *iIcon, stream );

    CleanupStack::PopAndDestroy( &stream );
    
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );
    CleanupStack::PopAndDestroy( buf );
    }

void CNcdSubscriptionGroup::ExternalizeDataForRequestL(
    MCatalogsSession& aSession,
    const CDesCArray& aPurchaseOptionIds,
    RWriteStream& aStream ) const
    {
    DLTRACEIN((""));


    iOriginNodeIdentifier->ExternalizeL( aStream );


    DLTRACE(( _L(" Externalizing subscriptiongroup info for proxy, EntityId: %S, Namespace: %S, server uri: %S"),
              &iOriginNodeIdentifier->NodeId(),
              &iOriginNodeIdentifier->NodeNameSpace(), 
              &iOriginNodeIdentifier->ServerUri() ));

    // this is the same as handle amount
    TInt subscriptionAmount( aPurchaseOptionIds.Count() );
    aStream.WriteInt32L( subscriptionAmount );

    DLTRACE(( "Amount of subscription handles sending: %d",
              subscriptionAmount ));
    
    for ( TInt i = 0; i < aPurchaseOptionIds.Count(); i++ ) 
        {
        TInt subscriptionIndex = FindSubscriptionL(
            aPurchaseOptionIds[i] );
        CNcdSubscription* tmpSubscription = 
            iSubscriptions[subscriptionIndex];
        
        TInt tmpHandle( aSession.AddObjectL( tmpSubscription ) );

        DLTRACE(( "Sending subscription handle: %i",
                  tmpHandle ));
        
        TRAPD( addError, aStream.WriteInt32L( tmpHandle ) );
        if ( addError != KErrNone )
            {
            // Should all other added objects be also removed from
            // the session?
            aSession.RemoveObject( tmpHandle );
            User::Leave( addError );
            }                
        }   

    DLTRACEOUT((""));
    }

void CNcdSubscriptionGroup::ReleaseRequest(
    MCatalogsBaseMessage& aMessage ) const
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

TInt CNcdSubscriptionGroup::FindSubscriptionL(
    const TDesC& aPurchaseOptionId ) const
    {
    DLTRACEIN((""));
    
    TInt subscriptionIndex( 0 );
    const TInt KSubscriptionCount( iSubscriptions.Count() );
    while( subscriptionIndex < KSubscriptionCount )
        {
        if ( iSubscriptions[subscriptionIndex]->PurchaseOptionId() ==
                 aPurchaseOptionId )
            {
            return subscriptionIndex;
            }
        ++subscriptionIndex;
        }
        
    DLINFO(("Not found"));
    User::Leave( KErrNotFound );
    DLTRACEOUT((""));    
    return KErrNotFound;
    }

CNcdSubscription& CNcdSubscriptionGroup::SubscriptionL(
    const TDesC& aPurchaseOptionId )
    {
    TInt subscriptionIndex( -1 );
    TRAPD( searchError,
           subscriptionIndex =
               FindSubscriptionL( aPurchaseOptionId ) );
    if ( searchError == KErrNotFound )
        {
        // Let's create new subscription because it was not
        // found
        CNcdSubscription* subscription = CNcdSubscription::NewLC( *this );
        iSubscriptions.AppendL( subscription );        
        CleanupStack::Pop( subscription );        
        
        // set clientIndex to the appended clients subscriptions
        subscriptionIndex = iSubscriptions.Count() - 1;
        }
    else if ( searchError != KErrNone )
        {
        // If leave occurs and it is not KErrNotFound, leave.
        User::Leave( searchError );
        }
        
    return *(iSubscriptions[subscriptionIndex]);
    }

void CNcdSubscriptionGroup::ResetMemberVariables()
    {
    delete iOriginNodeIdentifier;
    iOriginNodeIdentifier = NULL;
    
    delete iIcon;
    iIcon = NULL;
    
    ResetAndCloseArray( iSubscriptions );
    }
