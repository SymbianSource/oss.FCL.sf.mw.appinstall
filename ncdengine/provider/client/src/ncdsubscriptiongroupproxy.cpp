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
* Description:   Contains CNcdSubscriptionGroupProxy class implementation
*
*/


#include "ncdsubscriptiongroupproxy.h"

#include "ncdsubscriptionproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsutils.h"
#include "catalogsconstants.h"
#include "ncdnodeidentifier.h"

#include "catalogsdebug.h"

// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdSubscriptionGroupProxy::CNcdSubscriptionGroupProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdNodeManagerProxy& aNodeManager )
    : CNcdBaseProxy( aSession, aHandle ),
      iOperationManager( aOperationManager ),
      iNodeManager( aNodeManager )
    {
    }


void CNcdSubscriptionGroupProxy::ConstructL()
    {
    // Is it ok if internalization fails here?
    // Earlier comment: Do not let the internalization leave here.
    //                  This object may be reinternalized later.
    InternalizeL();
    }


CNcdSubscriptionGroupProxy* CNcdSubscriptionGroupProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdNodeManagerProxy& aNodeManager )
    {
    CNcdSubscriptionGroupProxy* self = 
        CNcdSubscriptionGroupProxy::NewLC( aSession,
                                           aHandle,
                                           aOperationManager,
                                           aNodeManager );
    CleanupStack::Pop( self );
    return self;   
    }

CNcdSubscriptionGroupProxy* CNcdSubscriptionGroupProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdNodeManagerProxy& aNodeManager )
    {
    CNcdSubscriptionGroupProxy* self = 
        new( ELeave ) CNcdSubscriptionGroupProxy( aSession,
                                                  aHandle,
                                                  aOperationManager,
                                                  aNodeManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;  
    }


CNcdSubscriptionGroupProxy::~CNcdSubscriptionGroupProxy()
    {
    DeleteSubscriptions();
    
    delete iOriginNodeIdentifier;
    }


void CNcdSubscriptionGroupProxy::InternalizeL()
    {
    DLTRACEIN((""));
    
    // Request all the purchase option ids from server side.
    CDesCArray* poIds = RequestPurchaseOptionIdsL();
    CleanupStack::PushL( poIds );
    
    // Delete the subscriptions that don't exist in the server side.
    DeleteMissingSubscriptions( *poIds );
    
    TRAPD( err, 
        {
        // Reinternalize the existing subscriptions.
        for ( TInt i = 0; i < iSubscriptions.Count(); i++ ) 
            {
            iSubscriptions[i]->InternalizeL();
            }
    
        // Remove the existing subscriptions from the array.
        for ( TInt i = poIds->Count() - 1 ; i >= 0; i-- ) 
            {
            if ( Subscription( (*poIds)[i] ) != NULL ) 
                {
                poIds->Delete( i );
                }
            }
        
        // Internalize the group with the new subscriptions.
        InternalizeL( *poIds );
        });
        
    CleanupStack::PopAndDestroy( poIds );

    if ( err != KErrNone ) 
        {
        DLINFO((( "Internalize error: %d" ), err ));
        DeleteSubscriptions();
        User::Leave( err );
        }
    
    DLTRACEOUT((""));    
    }

const RPointerArray<CNcdSubscriptionProxy>&
    CNcdSubscriptionGroupProxy::Subscriptions()
    {
    return iSubscriptions;
    }

CNcdSubscriptionProxy* CNcdSubscriptionGroupProxy::Subscription(
    const TDesC& aPurchaseOptionId )
    {
    DLTRACEIN((""));
    // Search for the subscriptions

    TInt subscriptionCount( iSubscriptions.Count() );
    TInt subscriptionIndexer( 0 );
    while ( subscriptionIndexer < subscriptionCount )
        {
        
        TDesC& subscriptionPurchaseOptionId = 
            iSubscriptions[subscriptionIndexer]->PurchaseOptionId();
            
        if (  aPurchaseOptionId == subscriptionPurchaseOptionId )
            {
            DLTRACEOUT(("Found."));
            return iSubscriptions[subscriptionIndexer];
            }
        
        ++subscriptionIndexer;
        }

    DLTRACEOUT(("Did not find."));
    return NULL;    
    }

const CNcdNodeIdentifier& CNcdSubscriptionGroupProxy::Identifier() const
    {
    return *iOriginNodeIdentifier;
    }

const TDesC& CNcdSubscriptionGroupProxy::EntityId() const
    {
    return iOriginNodeIdentifier->NodeId();
    }

const TDesC& CNcdSubscriptionGroupProxy::Namespace() const
    {
    return iOriginNodeIdentifier->NodeNameSpace();
    }

const TDesC& CNcdSubscriptionGroupProxy::ServerUri() const
    {
    return iOriginNodeIdentifier->ServerUri();
    }

// ---------------------------------------------------------------------------
// Icon data is not stored on the proxies. It is retrieved from the server
// side every time the data is requested.
// ---------------------------------------------------------------------------
//
HBufC8* CNcdSubscriptionGroupProxy::IconL() const
    {
    HBufC8* icon( RequestIconDataL() );
    return icon;
    }

CNcdOperationManagerProxy& 
    CNcdSubscriptionGroupProxy::OperationManager() const
    {
    return iOperationManager;
    }


// Other functions


void CNcdSubscriptionGroupProxy::InternalizeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    if ( !iOriginNodeIdentifier )
        {
        iOriginNodeIdentifier = CNcdNodeIdentifier::NewL();
        }        
    iOriginNodeIdentifier->InternalizeL( aStream );

    DLTRACE(( _L(" Internalizing subscriptiongroup info in proxy, EntityId: %S, Namespace: %S, server uri: %S"),
              &iOriginNodeIdentifier->NodeId(),
              &iOriginNodeIdentifier->NodeNameSpace(), 
              &iOriginNodeIdentifier->ServerUri() ));

    TInt handleAmount( 0 );
    handleAmount = aStream.ReadInt32L();
    
    DLTRACE(( "Amount of subscription handles received: %d",
              handleAmount ));

    TInt tmpProxyHandle( -1 ); // handle of a proxy read from stream
    
    // temporary pointer to subscription that is going to be
    // added to subscriptions-array
    CNcdSubscriptionProxy* tmpSubscription( NULL );

    // In error handling, objects with received handles
    // should be released from the server side session
    // if proxies for them cannot be created.
    
    TInt handleIndex( 0 );
    while ( handleIndex < handleAmount )
        {
        tmpProxyHandle = aStream.ReadInt32L();
        
        DLTRACE(( "Received subscription handle: %i",
                  tmpProxyHandle ));
                      
        tmpSubscription = CNcdSubscriptionProxy::NewL( 
                              ClientServerSession(),
                              tmpProxyHandle,
                              OperationManager(),
                              iNodeManager,
                              *this );                


        TRAPD( addError, iSubscriptions.AppendL( tmpSubscription ) );
        if ( addError != KErrNone )
            {
            delete tmpSubscription;
            User::Leave( addError );
            }
        
        tmpSubscription->InternalAddRef();

        ++handleIndex;
        }

    DLTRACEOUT((""));
    }

void CNcdSubscriptionGroupProxy::DeleteSubscriptions()
    {
    
    // If references to subscriptions are left, set it obsolete.

    TInt subscriptionsCount( iSubscriptions.Count() );
    TInt subscriptionsIndexer( 0 );
    while ( subscriptionsIndexer < subscriptionsCount )
        {
        DeleteSubscription( iSubscriptions[subscriptionsIndexer] );
        ++subscriptionsIndexer;
        }

    iSubscriptions.Reset();
    }
    
CDesCArray* CNcdSubscriptionGroupProxy::RequestPurchaseOptionIdsL() const 
    {
    DLTRACEIN((""));
    
    HBufC8* data( NULL );
    
    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    User::LeaveIfError(
        ClientServerSession().
        SendSyncAlloc( NcdNodeFunctionIds::ENcdPurchaseOptionIds,
                       KNullDesC8,
                       data,
                       Handle(),
                       0 ) );

    if ( data == NULL )
        {
        DLERROR((""));
        User::Leave(  KErrNotFound );
        }

     CleanupStack::PushL( data );

     // Read the data from the stream and create the array
     RDesReadStream stream( *data );
     CleanupClosePushL( stream );
     
     CDesCArray* poIds = new( ELeave ) CDesCArrayFlat( 5 );
     CleanupStack::PushL( poIds );
     
     TInt idCount = stream.ReadInt32L();
     for ( TInt i = 0; i < idCount; i++ ) 
        {
        HBufC* id( NULL );
        InternalizeDesL( id, stream );
        CleanupStack::PushL( id );
        poIds->AppendL( *id );
        CleanupStack::PopAndDestroy( id );
        }
        
    CleanupStack::Pop( poIds );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( data );
    
    return poIds;
    }
    
void CNcdSubscriptionGroupProxy::DeleteMissingSubscriptions(
    const CDesCArray& aPurchaseOptionIds ) 
    {
    DLTRACEIN((""));
    
    for ( TInt i = iSubscriptions.Count() - 1; i >= 0; i-- ) 
        {
        TInt pos( 0 );
        if ( aPurchaseOptionIds.Find( iSubscriptions[i]->PurchaseOptionId(), pos )
             != 0 ) 
            {
            DeleteSubscription( iSubscriptions[i] );
            iSubscriptions.Remove( i );
            }
        }
    }
    
void CNcdSubscriptionGroupProxy::DeleteSubscription(
    CNcdSubscriptionProxy* aSubscription ) 
    {
    if ( aSubscription->TotalRefCount() > 1 )
        {
        aSubscription->SetObsolete();
        }
    aSubscription->InternalRelease();
    }

void CNcdSubscriptionGroupProxy::InternalizeL(
    const CDesCArray& aPurchaseOptionIds ) 
    {
    DLTRACEIN((""));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
        
    RBufWriteStream writeStream( *buf );
    CleanupClosePushL( writeStream );
    
    writeStream.WriteInt32L( aPurchaseOptionIds.Count() );
    for ( TInt i = 0; i < aPurchaseOptionIds.Count(); i++ ) 
        {
        ExternalizeDesL( aPurchaseOptionIds[i], writeStream );
        }
        
    HBufC8* outputData = HBufC8::NewL( buf->Size() );
    outputData->Des().Copy( buf->Ptr( 0 ) );
    
    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::PopAndDestroy( buf );
    
    CleanupStack::PushL( outputData );
    
    HBufC8* inputData( NULL );

    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    User::LeaveIfError(
        ClientServerSession().
        SendSyncAlloc( NcdNodeFunctionIds::ENcdInternalize,
                       *outputData,
                       inputData,
                       Handle(),
                       0 ) );

    if ( inputData == NULL )
        {
        DLERROR((""));
        User::Leave(  KErrNotFound );
        }
        
    CleanupStack::PopAndDestroy( outputData );
    CleanupStack::PushL( inputData );
    
    // Read the data from the stream and create the array
    RDesReadStream stream( *inputData );
    CleanupClosePushL( stream );
    
    InternalizeDataL( stream );
    
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( inputData );
    }


HBufC8* CNcdSubscriptionGroupProxy::RequestIconDataL() const
    {
    DLTRACEIN((""));

    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    // Get the icon data from the subscription group implementation
    // on the server side.
    User::LeaveIfError(
        ClientServerSession().
        SendSyncAlloc( NcdNodeFunctionIds::ENcdSubscriptionIconData,
                       KNullDesC8,
                       data,
                       Handle(),
                       0 ) );

    if ( data == NULL )
        {
        DLERROR((""));
        User::Leave( KErrNotFound );
        }

    CleanupStack::PushL( data );

    // Read the data from the stream and insert it to variables
    RDesReadStream stream( *data );
    CleanupClosePushL( stream );

    HBufC8* iconData( NULL );
    InternalizeDesL( iconData, stream );

    // Closes the stream
    CleanupStack::PopAndDestroy( &stream ); 
    CleanupStack::PopAndDestroy( data );
    
    DLTRACEOUT((""));
    return iconData;
    }
