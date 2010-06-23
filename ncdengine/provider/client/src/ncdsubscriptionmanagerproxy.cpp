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
* Description:   Contains CNcdSubscriptionManagerProxy class implementation
*
*/


#include "ncdsubscriptionmanagerproxy.h"
#include "ncdsubscriptiongroupproxy.h"
#include "ncdsubscriptionproxy.h"
#include "ncdsubscriptionoperationproxy.h"
#include "ncdoperationimpl.h"
#include "ncddownloadoperationproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdoperationdatatypes.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "ncdutils.h"
#include "catalogsconstants.h"
#include "ncdsubscriptionmanagerlistener.h"

#include "catalogsdebug.h"

// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdSubscriptionManagerProxy::CNcdSubscriptionManagerProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CCatalogsInterfaceBase* aParent,
    CNcdOperationManagerProxy& aOperationManager )
    : CNcdInterfaceBaseProxy( aSession, aHandle, aParent ),
      iOperationManager( aOperationManager )
    {
    }


void CNcdSubscriptionManagerProxy::ConstructL()
    {
    DLTRACEIN((""));
    
    // Register the interface
    MNcdSubscriptionManager* interface( this );
    AddInterfaceL( CCatalogsInterfaceIdentifier::NewL( 
                        interface, this,
                        MNcdSubscriptionManager::KInterfaceUid ) );
                        
    // Create listener.
    iListener = CNcdSubscriptionManagerListener::NewL( *this );
    
    iInputBuf.CreateL( 1 );
    iOutputBuf.CreateL( 1 );
    
    
    // Is it ok if internalization fails here?
    // Earlier comment: Do not let the internalization leave here.
    //                  This object may be reinternalized later.
    InternalizeL();
    
    // Send an asynchronous message to server side subscription manager to be notified
    // when subscription states are changed in server side.
    ClientServerSession().SendAsync(
        NcdNodeFunctionIds::ENcdListenerEnrollment,
        iOutputBuf,
        iInputBuf,
        Handle(),
        iListener->iStatus );
            
    iListener->Activate();
        
    DLTRACEOUT((""));
    }


CNcdSubscriptionManagerProxy* CNcdSubscriptionManagerProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CCatalogsInterfaceBase* aParent,
    CNcdOperationManagerProxy& aOperationManager )
    {
    CNcdSubscriptionManagerProxy* self = 
        CNcdSubscriptionManagerProxy::NewLC( aSession,
                                             aHandle,
                                             aParent,
                                             aOperationManager );
    CleanupStack::Pop( self );
    return self;
    }

CNcdSubscriptionManagerProxy* CNcdSubscriptionManagerProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CCatalogsInterfaceBase* aParent,
    CNcdOperationManagerProxy& aOperationManager )
    {
    CNcdSubscriptionManagerProxy* self = 
        new( ELeave ) CNcdSubscriptionManagerProxy( aSession,
                                                    aHandle,
                                                    aParent,
                                                    aOperationManager );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdSubscriptionManagerProxy::~CNcdSubscriptionManagerProxy()
    {
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdSubscriptionManager::KInterfaceUid );
    
    iSubscriptionGroups.ResetAndDestroy();
    
    delete iListener;
    iInputBuf.Close();
    iOutputBuf.Close();
    }


void CNcdSubscriptionManagerProxy::SetNodeManager(
    CNcdNodeManagerProxy* aManager )
    {
    // Not owned so no deletion needed for possible previous object
    iNodeManager = aManager;
    }

void CNcdSubscriptionManagerProxy::InternalizeL()
    {
    DLTRACEIN((""));
    
    // Request the subscription group identifiers from server.
    RPointerArray<CNcdKeyValuePair> groupIds = SubscriptionGroupIdentifiersL();    
    CleanupResetAndDestroyPushL( groupIds );
    
    DLINFO(("Amount of ids received: %d", groupIds.Count() ));
    
    // Release the subscription groups that don't exist in server side.
    DeleteMissingSubscriptionGroups( groupIds );
    
    // Reinternalize existing subscriptiongroup.
    InternalizeSubscriptionGroupsL();
    
    // Remove the subscription groups from the array which are up to date currently.
    for ( TInt i = groupIds.Count() - 1; i >= 0; i-- ) 
        {
        if ( SubscriptionGroup( groupIds[i]->Value(), groupIds[i]->Key() ) != NULL ) 
            {
            delete groupIds[i];
            groupIds.Remove( i );
            }
        }
               
    // Request handles to the new subscription groups
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream writeStream( *buf );
    CleanupClosePushL( writeStream );
    
    writeStream.WriteInt32L( groupIds.Count() );
    for ( TInt i = 0; i < groupIds.Count(); i++ ) 
        {
        groupIds[i]->ExternalizeL( writeStream );
        }
        
    CleanupStack::PopAndDestroy( &writeStream );
            
    
    HBufC8* dataToSend = HBufC8::NewL( buf->Size() );
    dataToSend->Des().Copy( buf->Ptr( 0 ) );
    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PushL( dataToSend );

    HBufC8* data( NULL );
    
    DLINFO(("Request buffer: %S", dataToSend ));
        
    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    // Get all the data that is necessary to internalize this object
    // from the server side.
    User::LeaveIfError(
        ClientServerSession().
        SendSyncAlloc( NcdNodeFunctionIds::ENcdInternalize,
                       *dataToSend,
                       data,
                       Handle(),
                       0 ) );

    if ( data == NULL )
        {
        DLERROR((""));
        User::Leave(  KErrNotFound );
        }
        
    CleanupStack::PopAndDestroy( dataToSend );
    CleanupStack::PushL( data );

    // Read the data from the stream and insert it to the memeber variables
    RDesReadStream stream( *data );
    CleanupClosePushL( stream );
    
    TRAPD( internalizeError, InternalizeDataL( stream ) );
    if ( internalizeError != KErrNone )
        {
        // Should check that no handles are left to the
        // stream. If there are, release messages should
        // be sent to the server side with those handles.
        iSubscriptionGroups.ResetAndDestroy();
        User::Leave( internalizeError );
        }
    
    // Closes the stream
    CleanupStack::PopAndDestroy( &stream ); 
    CleanupStack::PopAndDestroy( data );
    CleanupStack::PopAndDestroy( &groupIds );

    DLTRACEOUT((""));    
    }


TBool CNcdSubscriptionManagerProxy::ActiveSubscriptionExists(
    const TDesC& aEntityId,
    const TDesC& aNamespace,
    const TDesC& aPurchaseOptionId )
    {
    DLTRACEIN((""));
    CNcdSubscriptionProxy* searchedSubscription( NULL );
    searchedSubscription = Subscription( aEntityId,
                                         aNamespace,
                                         aPurchaseOptionId );

    if ( searchedSubscription != NULL )
        {
        if ( searchedSubscription->SubscriptionStatus() ==
                 MNcdSubscription::ESubscriptionActive )
            {
            DLTRACEOUT(("Return ETrue"));
            return ETrue;
            }
        }    
    DLTRACEOUT(("Return EFalse"));
    return EFalse;
    }

CNcdSubscriptionProxy* CNcdSubscriptionManagerProxy::Subscription(
    const TDesC& aEntityId,
    const TDesC& aNamespace,
    const TDesC& aPurchaseOptionId )
    {
    DLTRACEIN((""));    
    CNcdSubscriptionProxy* resultSubscription( NULL );
    
    // First find the subscription group...
    CNcdSubscriptionGroupProxy* group( NULL );
    group = SubscriptionGroup( aEntityId, aNamespace );
    
    if ( group != NULL )
        {        
        // then subscription...
        resultSubscription = group->Subscription( aPurchaseOptionId );
        }
    
    DLTRACEOUT((""));
    return resultSubscription;
    }

void CNcdSubscriptionManagerProxy::SubscriptionsChangedL() 
    {
    DLTRACEIN((""));
    
    // Send new listener enrollment message to server side.
    ClientServerSession().SendAsync(
        NcdNodeFunctionIds::ENcdListenerEnrollment,
        iOutputBuf,
        iInputBuf,
        Handle(),
        iListener->iStatus );
    iListener->Activate();
        
    // Internalize the data from server.
    InternalizeL();
    }
        


// MNcdSubscriptionManager functions
    


// ---------------------------------------------------------------------------
// Subscriptions are now under subscription groups so they are not
// kept in one large array.
// Benefit from the one large array would be that the returning of
// all subscriptions in this function would be easy but because
// the array would have to be updated or remade after every
// update from server (which is always before this function) it seems
// that keeping up the large array does not benefit us. Atleast
// not in this function.
// ---------------------------------------------------------------------------
//
RCatalogsArray<MNcdSubscription> 
    CNcdSubscriptionManagerProxy::SubscriptionsL() const
    {
    DLTRACEIN(( "" ));
    
    // When ui asks for subscriptions it does it in the
    // management view so the ui should update the subscriptions
    // from the server before this function. Update is not done here.

    // Let's make one array out of the subscriptions
    RCatalogsArray<MNcdSubscription> subscriptions;
    
    // in case of leave, destroy its contents (release)
    CleanupResetAndDestroyPushL( subscriptions );

    
    TInt groupCount( iSubscriptionGroups.Count() );
    TInt groupIndexer( 0 );
    
    DLINFO(( "Coming to while" ));
    while ( groupIndexer < groupCount )
        {
        // through all groups
        const RPointerArray<CNcdSubscriptionProxy>& subscriptionsOfGroup
            = iSubscriptionGroups[groupIndexer]->Subscriptions();
        
        TInt subscriptionCount( subscriptionsOfGroup.Count() );
        TInt subscriptionIndexer( 0 );
        
        DLINFO(( "Coming to while2" ));
        while ( subscriptionIndexer < subscriptionCount )
            {
            // through all subscriptions of group
            subscriptions.AppendL(
                subscriptionsOfGroup[subscriptionIndexer] );
            subscriptionsOfGroup[subscriptionIndexer]->AddRef();        
            ++subscriptionIndexer;
            }
        
        ++groupIndexer;
        }
    CleanupStack::Pop( &subscriptions );
    
    DLTRACEOUT(( "" ));
    return subscriptions;
    }


MNcdSubscriptionOperation* 
    CNcdSubscriptionManagerProxy::RefreshSubscriptionsL(
        MNcdSubscriptionOperationObserver& aObserver )
    {
    DLTRACEIN((""));

    CNcdSubscriptionOperationProxy* operation( NULL );

    operation =
        iOperationManager.CreateSubscriptionRefreshOperationL( aObserver );

    DLTRACEOUT((""));

    return operation;
    }



// Other functions

CNcdOperationManagerProxy&
    CNcdSubscriptionManagerProxy::OperationManager() const
    {
    return iOperationManager;
    }



void CNcdSubscriptionManagerProxy::InternalizeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));    

    TInt handleAmount( 0 );
    handleAmount = aStream.ReadInt32L();
    
    DLTRACE(( "Amount of subscriptiongroup handles received: %d",
              handleAmount ));

    TInt tmpProxyHandle( -1 ); // handle of a proxy read from stream
    
    // temporary pointer to subscription group that is going to be
    // added to subscriptiongroups-array
    CNcdSubscriptionGroupProxy* tmpSubscriptionGroup( NULL );
    

    // In error handling, objects with received handles
    // should be released from the server side session
    // if proxies for them cannot be created.

    TInt handleIndex( 0 );
    while ( handleIndex < handleAmount )
        {
        tmpProxyHandle = aStream.ReadInt32L();
        
        DLTRACE(( "Received subscriptiongroup handle: %i",
                  tmpProxyHandle ));
                      
        tmpSubscriptionGroup = CNcdSubscriptionGroupProxy::NewL( 
                                ClientServerSession(),
                                tmpProxyHandle,
                                OperationManager(),
                                *iNodeManager );                

        TRAPD( addError, iSubscriptionGroups.AppendL( tmpSubscriptionGroup ) );
        if ( addError != KErrNone )
            {
            delete tmpSubscriptionGroup;
            User::Leave( addError );
            }

        ++handleIndex;
        }

    DLTRACEOUT((""));
    }

RPointerArray<CNcdKeyValuePair> CNcdSubscriptionManagerProxy::SubscriptionGroupIdentifiersL() const 
    {
    DLTRACEIN((""));
    
    HBufC8* data( NULL );

    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    User::LeaveIfError(
        ClientServerSession().
        SendSyncAlloc( NcdNodeFunctionIds::ENcdSubscriptionGroupIdentifiers,
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

    // Read the data from the stream and insert it to the memeber variables
    RDesReadStream stream( *data );
    CleanupClosePushL( stream );
     
    TInt32 identifierCount = stream.ReadInt32L();
    RPointerArray<CNcdKeyValuePair> identifiers;
    CleanupResetAndDestroyPushL( identifiers );
    identifiers.ReserveL( identifierCount );
    for ( TInt i = 0 ; i < identifierCount; i++ ) 
        {
        CNcdKeyValuePair* pair = CNcdKeyValuePair::NewL( stream );
        TInt err = identifiers.Append( pair );
        // Error should not be possible since there should be enough room to append.
        DASSERT( err == KErrNone );
        }
        
    CleanupStack::Pop( &identifiers );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( data );
     
    return identifiers;
    }
    
void CNcdSubscriptionManagerProxy::DeleteMissingSubscriptionGroups(
    const RPointerArray<CNcdKeyValuePair>& aGroupIdentifiers ) 
    {
    DLTRACEIN((""));
    
    for ( TInt i = iSubscriptionGroups.Count() - 1; i >= 0; i-- ) 
        {
        const TDesC& nameSpace = iSubscriptionGroups[i]->Namespace();
        const TDesC& entityId = iSubscriptionGroups[i]->EntityId();
        
        TBool found = EFalse;
        for ( TInt idIndexer = 0; idIndexer < aGroupIdentifiers.Count(); idIndexer++ ) 
            {
            if ( aGroupIdentifiers[idIndexer]->Key() == nameSpace &&
                 aGroupIdentifiers[idIndexer]->Value() == entityId ) 
                {
                found = ETrue;
                break;
                }
            }
        
        if ( !found ) 
            {
            delete iSubscriptionGroups[i];
            iSubscriptionGroups.Remove( i );
            }            
        }
    }        
    
void CNcdSubscriptionManagerProxy::InternalizeSubscriptionGroupsL() const 
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iSubscriptionGroups.Count(); i++ ) 
        {
        iSubscriptionGroups[i]->InternalizeL();
        }
    }

CNcdSubscriptionGroupProxy* CNcdSubscriptionManagerProxy::SubscriptionGroup(
    const TDesC& aEntityId,
    const TDesC& aNamespace )
    {
    DLTRACEIN((""));
    // Search for the subscription group

    TInt groupCount( iSubscriptionGroups.Count() );
    TInt groupIndexer( 0 );
    while ( groupIndexer < groupCount )
        {        
        const TDesC& groupsEntityId =
            iSubscriptionGroups[groupIndexer]->EntityId();
        const TDesC& groupsNamespace =
            iSubscriptionGroups[groupIndexer]->Namespace();

        DLINFO(( _L("Searched entityid: %S, now entityid: %S"),
                 &aEntityId,
                 &groupsEntityId ));        
        
        if ( aEntityId == groupsEntityId )
            {

            DLINFO(( _L("Searched namespace: %S, now namespace: %S"),
                     &aNamespace,
                     &groupsNamespace ));
                 
            if ( aNamespace == groupsNamespace )
                {
                DLTRACEOUT(("Found."));
                return iSubscriptionGroups[groupIndexer];
                }
            
            }
        
        ++groupIndexer;
        }

    DLTRACEOUT(("Not found."));
    return NULL;
    }

