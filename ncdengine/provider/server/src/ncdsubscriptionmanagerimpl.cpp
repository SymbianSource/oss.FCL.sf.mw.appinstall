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
* Description:   Implements CNcdSubscriptionManager class
*
*/


#include "ncdsubscriptionmanagerimpl.h"

#include <s32mem.h>
#include <bamdesca.h> // MDesCArray

#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsconstants.h"
#include "ncd_pp_dataentity.h"
#include "ncd_pp_subscription.h"
#include "ncd_cp_query.h"
#include "catalogsutils.h"
#include "ncdstoragedataitem.h"
#include "ncdsubscriptionssourceidentifier.h"
#include "ncdserverpartofsubscription.h"
#include "ncdclientssubscriptions.h"
#include "ncdsubscriptiongroup.h"
#include "catalogscontext.h"
#include "ncddatabasestorage.h"
#include "ncdstorage.h"
#include "ncdproviderdefines.h"
#include "ncdstorageitem.h"
#include "ncdstoragemanagerimpl.h"
#include "ncdpurchaseoptionimpl.h"
#include "ncdnodeidentifier.h"
#include "ncdnodemanager.h"
#include "ncdrootnode.h"
#include "ncdsubscriptiongroup.h"
#include "ncdutils.h"
#include "ncdnodeiconimpl.h"

// This is for the contentsourcemap
#include "ncdloadrootnodeoperationimpl.h"

#include "ncdsubscriptionmanagerobserver.h"
#include "ncdsubscriptiondatacompleter.h"

#include "catalogsdebug.h"

CNcdSubscriptionManager::CNcdSubscriptionManager(
    CNcdStorageManager& aStorageManager,
    CNcdNodeManager& aNodeManager )
    : CCatalogsCommunicable(),
      iStorageManager( aStorageManager ),
      iNodeManager( aNodeManager )
    {
    DLTRACEIN((""));

    DLTRACEOUT((""));
    }

void CNcdSubscriptionManager::ConstructL()
    {
    }

CNcdSubscriptionManager* CNcdSubscriptionManager::NewL(
    CNcdStorageManager& aStorageManager,
    CNcdNodeManager& aNodeManager )
    {
    CNcdSubscriptionManager* self =
        CNcdSubscriptionManager::NewLC( aStorageManager, aNodeManager );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdSubscriptionManager* CNcdSubscriptionManager::NewLC(
    CNcdStorageManager& aStorageManager,
    CNcdNodeManager& aNodeManager )
    {
    CNcdSubscriptionManager* self = 
        new( ELeave ) CNcdSubscriptionManager( aStorageManager, aNodeManager );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdSubscriptionManager::~CNcdSubscriptionManager()
    {
    DLTRACEIN((""));

    iSubscriptionDataCompleters.ResetAndDestroy();

    // All subscriptions should be saved into database if they
    // are new or changed. So there is no need to save all of them
    // here.

    const TInt clientsCount( iClientSubscriptions.Count() );
    TInt indexer( 0 );
    
    while( indexer < clientsCount )
        {
        delete iClientSubscriptions[indexer];
        
        ++indexer;
        }
    iClientSubscriptions.Reset();
    
    // Complete the pending messages
    NotifyAllListeners( KErrServerTerminated );
    iPendingMessages.Close();

    DLTRACEOUT((""));
    }        

void CNcdSubscriptionManager::SetOperationManager(
    CNcdOperationManager* aManager )
    {
    iOperationManager = aManager;
    }




RPointerArray<CNcdSubscriptionsSourceIdentifier>
    CNcdSubscriptionManager::SubscriptionsSourcesL( TUid aClientUid ) const
    {    
    DLTRACEIN((""));
    
    // First read possibly existing subscriptions sources from the
    // subscriptions database
    TInt clientIndex( -1 );
    RPointerArray<CNcdSubscriptionsSourceIdentifier> sources;    
    TRAP_IGNORE( clientIndex = IsInCacheL( aClientUid ) );
    if ( clientIndex > -1 )
        {
        TRAP_IGNORE( sources = SubscriptionsSourcesL( clientIndex ) );
        }


    DLINFO(( "Getting subscriptions sources from the root node." ));

    // Get content sources from root node.
    CNcdRootNode& rootNode = 
        iNodeManager.CreateRootL( aClientUid );
    CNcdContentSourceMap& csMap = rootNode.ContentSourceMap();
    DLTRACE(("Content sources: %d", csMap.ContentSourceCount() ));
    

    TIdentityRelation<CNcdSubscriptionsSourceIdentifier> match( 
            &CNcdSubscriptionsSourceIdentifier::CompareIdentifiers );
    
    for ( TInt i = 0; i < csMap.ContentSourceCount(); i++ ) 
        {
        CNcdContentSource& contentSource = csMap.ContentSource( i );
        // When we get the source identifier from the root node, we don't know
        // if it supports subscriptions so that has to be checked when sending
        // requests -> aRequireCapabilityCheck (last param) is set ETrue
        CNcdSubscriptionsSourceIdentifier* sourceId =
            CNcdSubscriptionsSourceIdentifier::NewLC(
                contentSource.Uri(), contentSource.NameSpace(), ETrue );
        
        // Do not add the source identifier if the same exists already.
        if ( sources.Find( sourceId, match ) == KErrNotFound )
            {
            sources.AppendL( sourceId );
            DLINFO(( "Subscriptions source appended into the array." ));
            CleanupStack::Pop( sourceId );
            }
        else
            {
            DLINFO(( "Subscriptions source already found from the array." ));
            CleanupStack::PopAndDestroy( sourceId );
            }
        
        DLINFO(( _L( "Source namespace: %S."), &contentSource.NameSpace() ));
        }

    // Handle bundle content sources. This actually helps only after the
    // user has browsed inside a bundle because we won't know the capabilities
    // of the bundle's servers before that
    TInt bundleCount = csMap.BundleFolderCount();
    DLINFO(("Bundle folders: %d", bundleCount ));
    for ( TInt i = 0; i < bundleCount; i++ ) 
        {
        const CNcdNodeIdentifier& bundleId = csMap.BundleFolder( i );
        TInt folderIndex = csMap.FindFolder( bundleId );
        CNcdFolderContent& folderContent = csMap.FolderContent( folderIndex );
        
        DLTRACE(("Bundle has %d content sources", 
            folderContent.ContentSourceCount() ));
            
        for ( TInt index = 0; index < folderContent.ContentSourceCount(); ++index ) 
            {            
            CNcdContentSource& contentSource = folderContent.ContentSource( index );
            // When we get the source identifier from the root node, we don't know
            // if it supports subscriptions so that has to be checked when sending
            // requests -> aRequireCapabilityCheck (last param) is set ETrue
            CNcdSubscriptionsSourceIdentifier* sourceId =
                CNcdSubscriptionsSourceIdentifier::NewLC(
                    contentSource.Uri(), contentSource.NameSpace(), ETrue );
            
            // Do not add the source identifier if the same exists already.
            if ( sources.Find( sourceId, match ) == KErrNotFound )
                {
                sources.AppendL( sourceId );
                CleanupStack::Pop( sourceId );
                }
            else
                {
                CleanupStack::PopAndDestroy( sourceId );
                }
            DLINFO(( _L( "Source namespace: %S."), &contentSource.NameSpace() ));
            }
        }

    return sources;    
    }

void CNcdSubscriptionManager::InternalizeSubscriptionsFromServerL(
    TUid aClientUid,
    const TDesC& aUri,
    RPointerArray<MNcdPreminetProtocolSubscription>& aServersSubscriptions,
    MCatalogsContext* aContext,
    MNcdSubscriptionManagerObserver* aObserver )
    {
    DLTRACEIN((""));
    
    // Should do leave handling
    
    // First we create objects resembling subscriptions
    // and mark them as recently created.
    // This also writes subscriptions to database.
    InternalizeAndMarkSubscriptionsL( aClientUid,
                                      aUri,
                                      aServersSubscriptions );
    
    // Get the subscriptions related to given server
    // Here we should get also the recently created ones
    RPointerArray<CNcdSubscriptionGroup> serversGroups =
        ServersGroupsL( aClientUid, aUri );
        
    // In the end we remove subscriptions that were
    // not received in the aServersSubscriptions array and reset
    // the flags of subscriptions that were received. (Subscriptions that
    // were not received from the server in the aServersSubscriptions array
    // and are marked as received from the server (earlier) are removed
    // from the database, from the cache and from the serversGroups
    // array.)
    TRAP_IGNORE( RemoveUnmarkedSubscriptionsL( aClientUid, serversGroups ) );

    // Check that server's current subscriptions have all needed data and
    // if not get them from the corresponding nodes. Do also the needed
    // callback of observer and notifying of subscription manager's listeners
    // Also database write is done after completion. The earlier write
    // was done to ensure that some entry is found from the db if this
    // operation does not end before shutdown or some possible error situation
    // occurs.
    TRAP_IGNORE( CompleteSubscriptionsDataL( aClientUid,
                                             serversGroups,
                                             aContext,
                                             aObserver ) );

    // As the subscriptions are stored in member variable,
    // they should not be deleted. Just close for the array.
    serversGroups.Close();    
    }    


void CNcdSubscriptionManager::InternalizeSubscriptionL( 
    TUid aClientUid,
    const TDesC& aUri,
    MNcdPreminetProtocolSubscription& aSubscription,
    MCatalogsContext* aContext,
    MNcdSubscriptionManagerObserver* aObserver )
    {
    DLTRACEIN((""));

    // Get or create subscription group

    CNcdSubscriptionGroup& groupForSubscription = ClientsSubscriptionGroupL(
        aClientUid,
        aSubscription.EntityId(),
        aSubscription.Namespace(),
        aUri );


    groupForSubscription.InternalizeSubscriptionL( aSubscription );

    DLTRACE(( "Subscription internalized." ));

    // Database should always have updated info so at this point we have
    // to write updates there.
    
    // Because there is a database for each client the writing has
    // to be done here where we know which group was updated.

    SaveGroupIntoDatabaseL( aClientUid,
                            groupForSubscription );

    DLTRACE(( "Subscription group saved into database." ));


    // Check that current subscription (and its group) has all needed
    // data and if not, get them from the corresponding nodes. Do also
    // the needed callback of observer and notifying of subscription
    // manager's listeners. Also database write is done after completion.
    // The earlier write was done to ensure that some entry is found
    // from the db if this operation does not end before shutdown or
    // some possible error situation occurs.
    RPointerArray<CNcdSubscriptionGroup> groups;
    groups.AppendL( &groupForSubscription );
    TRAP_IGNORE( CompleteSubscriptionsDataL( aClientUid,
                                             groups,
                                             aContext,
                                             aObserver ) );

    // As the subscriptions are stored in member variable,
    // they should not be deleted. Just close for the array.
    groups.Close();

    DLTRACEOUT((""));
    }


void CNcdSubscriptionManager::RemoveSubscriptionL(
    const CNcdNodeIdentifier& aData,
    const TDesC& aPurchaseOptionId)
    {
    
    CNcdSubscriptionGroup& group = FindGroupL( aData.ClientUid(),
                                               aData.NodeId(),
                                               aData.NodeNameSpace() );

    group.RemoveSubscriptionL( aPurchaseOptionId );
    
    if ( group.SubscriptionCount() == 0 )
        {
        TInt index = IsInCacheL( aData.ClientUid() );
        CNcdClientsSubscriptions* clientsSubscriptions =
        iClientSubscriptions[index];
        
        clientsSubscriptions->RemoveGroup( &group );
        
        RemoveGroupFromDatabaseL( aData.ClientUid(),
                                  group );
        }    
    else
        {
        SaveGroupIntoDatabaseL( aData.ClientUid(), group );
        }

    // As the clientsSubscriptions only exists in the cache,
    // it is not checked that if groups still exist after this
    // group removal and whether the clientsSubscriptions
    // should be removed.    

    // Notify the listeners.
    NotifyListeners( aData.ClientUid() );
    }

CNcdSubscription& CNcdSubscriptionManager::SubscriptionL(
    const TDesC& aNodeId,
    const TDesC& aNodeNameSpace,
    const TDesC& aPurchaseOptionId,
    const TUid& aClientUid ) 
    {
    DLTRACEIN((""));
    
    TInt index = IsInCacheL( aClientUid );
    CNcdClientsSubscriptions* subscriptions = iClientSubscriptions[index];
    RPointerArray<CNcdSubscriptionGroup>& groups = subscriptions->Groups();
    TInt groupIndex = FindGroupL( groups, aNodeId, aNodeNameSpace );
    CNcdSubscriptionGroup* group = groups[groupIndex];
    return group->SubscriptionIfExistsL( aPurchaseOptionId );
    }
    
    
void CNcdSubscriptionManager::ClearSubscriptionDbL( const MCatalogsContext& aContext ) 
    {
    DLTRACEIN((""));
    iStorageManager.RemoveStorageL(
        aContext.FamilyId().Name(), NcdProviderDefines::KSubscriptionNamespace );
    
    // Remove from RAM cache too.
    for ( TInt i = 0; i < iClientSubscriptions.Count(); i++ ) 
        {
        if ( iClientSubscriptions[i]->ClientId() == aContext.FamilyId() ) 
            {
            delete iClientSubscriptions[i];
            iClientSubscriptions.Remove( i );
            break;
            }
        }    
    }
    

void CNcdSubscriptionManager::NotifyListeners( TUid aClientUid ) 
    {
    DLTRACEIN((""));
    for ( TInt i = iPendingMessages.Count() - 1; i >= 0; i-- ) 
        {
        if ( iPendingMessages[i]->Session().Context().FamilyId() == 
             aClientUid ) 
            {            
            iPendingMessages[i]->CompleteAndRelease( KErrNone );
            iPendingMessages.Remove( i );                        
            }
        }
    }

void CNcdSubscriptionManager::SubscriptionCompleted(
        CNcdSubscriptionDataCompleter* aFinishedCompleter )
    {
    const TInt KCompleterAmount( iSubscriptionDataCompleters.Count() );
    TInt completerIndexer( 0 );
    
    CNcdSubscriptionDataCompleter* tempCompleter( NULL );
    while ( completerIndexer < KCompleterAmount )
        {
        tempCompleter = iSubscriptionDataCompleters[ completerIndexer ];
        if ( tempCompleter == aFinishedCompleter )
            {
            iSubscriptionDataCompleters.Remove( completerIndexer );
            delete aFinishedCompleter;
            return;
            }        
        ++completerIndexer;
        }
    }

void CNcdSubscriptionManager::SaveGroupIntoDatabaseL(
    TUid aClientUid,
    CNcdSubscriptionGroup& aGroup )
    {
    DLTRACEIN((""));
    
    // Databases are created for each client. (for each uid)
    // As a namespace we use a constant namespace (KSubscriptionNameSpace)
    // so we know in the future where from to load all data.

    HBufC* dataId = GenerateGroupDataIdL( aGroup );
    CleanupStack::PushL( dataId );

    DLTRACE(( _L("DataId for subscription group database item: %S"),
              dataId ));


    SaveDataIntoDatabaseL( aClientUid,
                           NcdProviderDefines::KSubscriptionNamespace,
                           *dataId,
                           aGroup,
                           NcdNodeClassIds::ENcdSubscriptionsData );
                           
    CleanupStack::PopAndDestroy( dataId );
    
    DLTRACEOUT((""));
    }


void CNcdSubscriptionManager::ReceiveMessage( 
    MCatalogsBaseMessage* aMessage,
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
        case NcdNodeFunctionIds::ENcdSubscriptionGroupIdentifiers:
            // Subscription group identifiers requested from proxy side.
            TRAP( trapError, SubscriptionGroupIdentifiersRequestL( *aMessage ) );
            break;
            
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
            
        case NcdNodeFunctionIds::ENcdListenerEnrollment:
            // The proxy wants to enroll as listener.
            TRAP( trapError, ListenerEnrolledL( *aMessage ) );
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


void CNcdSubscriptionManager::CounterPartLost( 
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
        
    for ( TInt i = 0; i < iPendingMessages.Count(); i++ ) 
        {
        iPendingMessages[i]->CounterPartLost( aSession );
        }
    }
 
                    

void CNcdSubscriptionManager::ReleaseRequest( 
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );

    DLTRACE(("Calling counterpart lost for messages"));
    for ( TInt i = 0; i < iPendingMessages.Count(); i++ ) 
        {
        iPendingMessages[i]->CounterPartLost( requestSession );
        }


    // Send complete information back to proxy.
    aMessage.CompleteAndRelease( KErrNone );
        
    // Remove this object from the session.
    requestSession.RemoveObject( handle );

    DLTRACEOUT((""));
    }




void CNcdSubscriptionManager::InternalizeRequestL( 
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    // Parse the input message and create an array containing
    // subscription group identifiers which are requested.
    RPointerArray<CNcdKeyValuePair> groupIds;
    CleanupResetAndDestroyPushL( groupIds );
    
    HBufC8* input = HBufC8::NewLC( aMessage.InputLength() );
    TPtr8 inputPtr = input->Des();
    aMessage.ReadInput( inputPtr );
    RDesReadStream inputStream( *input );
    CleanupClosePushL( inputStream );
    
    TInt32 groupCount = inputStream.ReadInt32L();
    groupIds.ReserveL( groupCount );
    for ( TInt i = 0; i < groupCount; i++ ) 
        {
        CNcdKeyValuePair* pair = CNcdKeyValuePair::NewL( inputStream );
        groupIds.Append( pair );
        }
 
    CleanupStack::PopAndDestroy( &inputStream );
    CleanupStack::PopAndDestroy( input );
           
           
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );


    // We need session info in the functions for example to
    // check the client id (there can be several clients)
    // and to register needed objects to the session.
    MCatalogsSession& session = aMessage.Session();
    TUid clientUid = session.Context().FamilyId();
    
    ExternalizeClientsSubscriptionsL( session, groupIds, stream );
    
    // Commits data to the stream when closing.
    CleanupStack::PopAndDestroy( &stream );


    // If this leaves, ReceiveMessage will complete the message.
    // NOTE: that here we expect that the buffer contains at least
    // some data. So, make sure that ExternalizeDataForRequestL inserts
    // something to the buffer.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );        
        
    
    DLTRACE(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( &groupIds );
        
    DLTRACEOUT((""));
    }
    
    
void CNcdSubscriptionManager::SubscriptionGroupIdentifiersRequestL(
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );
    
    TUid clientUid = aMessage.Session().Context().FamilyId();
    TInt clientIndex( -1 );
    TRAPD( err, clientIndex = IsInCacheL( clientUid ) );
    if ( err == KErrNotFound )
        {
        // Check also storage.
        // Needed for example when the engine has not retrieved
        // the info from db after last restart.
        TRAP_IGNORE( LoadClientsInfoFromStorageL( clientUid ) );
                          
        TRAP_IGNORE( clientIndex = IsInCacheL( clientUid ) );
        }
        
    if ( clientIndex < 0 ) 
        {
        stream.WriteInt32L( 0 );
        }
    else 
        {
        CNcdClientsSubscriptions* subscriptions =
            iClientSubscriptions[clientIndex];
        RPointerArray<CNcdSubscriptionGroup>& groups =
            subscriptions->Groups();
        stream.WriteInt32L( groups.Count() );
        for ( TInt i = 0; i < groups.Count(); i++ ) 
            {
            CNcdKeyValuePair* pair = CNcdKeyValuePair::NewLC(
                groups[i]->Namespace(), groups[i]->EntityId() );
            pair->ExternalizeL( stream );
            CleanupStack::PopAndDestroy( pair );
            }
        }
        
    CleanupStack::PopAndDestroy( &stream );
    
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );
    
    CleanupStack::PopAndDestroy( buf );
    }


void CNcdSubscriptionManager::CompleteSubscriptionsDataL(
    TUid aClientUid,
    RPointerArray<CNcdSubscriptionGroup>& aServersGroups,
    MCatalogsContext* aContext,
    MNcdSubscriptionManagerObserver* aObserver )
    {
    DLTRACEIN((""));
    // Create array of node identifiers to identify the nodes
    // (and subscriptions)
    // and create array of purchase option ids to identify
    // the subscriptions
    RPointerArray<CNcdNodeIdentifier> nodeIds;
    CDesCArrayFlat* purchaseOptionIDs( NULL );

    CNcdSubscriptionDataCompleter* completer( NULL );
    TRAPD( error,
        {
        purchaseOptionIDs = new (ELeave) CDesCArrayFlat( 5 );
        IdentifiersForCompletionL( aServersGroups,
                                   nodeIds,
                                   *purchaseOptionIDs );

        // Create data completer object
        completer =
            CNcdSubscriptionDataCompleter::NewL( *this,
                                                 iNodeManager,
                                                 *iOperationManager );
        iSubscriptionDataCompleters.AppendL( completer );
        
        } );
    if ( error != KErrNone )
        {
        nodeIds.ResetAndDestroy();
        
        if ( purchaseOptionIDs )
            {
            purchaseOptionIDs->Reset();
            }
        User::Leave( error );
        }
    
    // passing ownership of identifiers
    completer->CompleteSubscriptions( aClientUid,
                                      nodeIds,
                                      purchaseOptionIDs,
                                      aContext,
                                      aObserver );
    DLTRACEOUT((""));
    }

void CNcdSubscriptionManager::IdentifiersForCompletionL(
    RPointerArray<CNcdSubscriptionGroup>& aGroups,
    RPointerArray<CNcdNodeIdentifier>& aNodeIds,
    CDesCArrayFlat& aPurchaseOptionIDs )
    {
    DLTRACEIN((""));
    TInt groupIndex( 0 );
    const TInt KGroupCount( aGroups.Count() );
    while( groupIndex < KGroupCount )
        {
        DLINFO(( "group nro: %d", groupIndex ));
        aGroups[groupIndex]->AppendIncompleteSubscriptionIDsL(
                                 aNodeIds,
                                 aPurchaseOptionIDs );

        ++groupIndex;
        }
    DLTRACEOUT((""));
    }

TInt CNcdSubscriptionManager::IsInCacheL( TUid aUid ) const
    {
    DLTRACEIN((""));
    
    TInt clientIndex( 0 );
    const TInt KClientCount( iClientSubscriptions.Count() );
    while( clientIndex < KClientCount )
        {
        if ( iClientSubscriptions[clientIndex]->ClientId() == aUid )
            {
            return clientIndex;
            }
        ++clientIndex;
        }
        
    User::Leave( KErrNotFound );
    DLTRACEOUT((""));
    return KErrNotFound;
    }


TInt CNcdSubscriptionManager::ClientsSubscriptionsL( TUid aClientUid )
    {
    DLTRACEIN((""));
    TInt clientIndex( -1 );
    TRAPD( searchError, clientIndex = IsInCacheL( aClientUid ) );
    if ( searchError == KErrNotFound )
        {
        // Let's create new ClientsSubscriptions because it was not
        // found
        CNcdClientsSubscriptions* clientsSubscriptions =
            CNcdClientsSubscriptions::NewLC( aClientUid );        
        iClientSubscriptions.AppendL( clientsSubscriptions );        
        CleanupStack::Pop( clientsSubscriptions );        
        
        // set clientIndex to the appended clients subscriptions
        clientIndex = iClientSubscriptions.Count() - 1;
        }
    else if ( searchError != KErrNone )
        {
        // If leave occurs and it is not KErrNotFound, leave.
        User::Leave( searchError );
        }

    return clientIndex;
    }


TInt CNcdSubscriptionManager::FindGroupL(
    const RPointerArray<CNcdSubscriptionGroup>& aGroups,
    const TDesC& aEntityId,
    const TDesC& aNamespace ) const
    {
    DLTRACEIN((""));
    
    TInt groupIndex( 0 );
    const TInt KGroupCount( aGroups.Count() );
    while( groupIndex < KGroupCount )
        {
        if ( aGroups[groupIndex]->EntityId() == aEntityId &&
             aGroups[groupIndex]->Namespace() == aNamespace )
            {
            return groupIndex;
            }
        ++groupIndex;
        }
        
    User::Leave( KErrNotFound );
    DLTRACEOUT((""));    
    return KErrNotFound;
    }

CNcdSubscriptionGroup& CNcdSubscriptionManager::FindGroupL(
    TUid aClientUid,
    const TDesC& aEntityId,
    const TDesC& aNamespace ) const
    {
    DLTRACEIN((""));
    TInt index = IsInCacheL( aClientUid );
    CNcdClientsSubscriptions* subscriptions = iClientSubscriptions[index];
    RPointerArray<CNcdSubscriptionGroup>& groups = subscriptions->Groups();
    TInt groupIndex = FindGroupL( groups, aEntityId, aNamespace );    
    return *groups[groupIndex];    
    }

CNcdSubscriptionGroup& CNcdSubscriptionManager::ClientsSubscriptionGroupL(
    const TDesC& aUri,
    const CNcdPurchaseOptionImpl& aData )
    {
    DLTRACEIN((""));
    
    const CNcdNodeIdentifier& nodeIdentifier = aData.ParentMetaIdentifier();
    const TDesC& nameSpace = nodeIdentifier.NodeNameSpace();
    const TUid& clientUid = nodeIdentifier.ClientUid();
    const TDesC& entityId = nodeIdentifier.NodeId();

    CNcdSubscriptionGroup& groupForSubscription = ClientsSubscriptionGroupL(
        clientUid,
        entityId,
        nameSpace,
        aUri );
 
    // Icon is always set here to be sure that it is set. So it does
    // not have to be retrieved later on.
    // Icon for the subscription is at the moment thought to be icon
    // of the node where from the subscription is bought
    const CNcdNodeIcon& metaDataIcon = aData.ParentIconL();
    HBufC8* icon = metaDataIcon.IconDataL();
    CleanupStack::PushL( icon );
    groupForSubscription.SetIconL( *icon );
    CleanupStack::PopAndDestroy( icon );

    return groupForSubscription;
    }

CNcdSubscriptionGroup& CNcdSubscriptionManager::ClientsSubscriptionGroupL(
    TUid aClientUid,
    const TDesC& aEntityId,
    const TDesC& aNamespace,
    const TDesC& aUri )
    {
    DLTRACEIN((""));
    // Get or create object for clients subscription info    
    TInt clientIndex( ClientsSubscriptionsL( aClientUid ) );
    
    RPointerArray<CNcdSubscriptionGroup>& groupsOfClient =
        iClientSubscriptions[clientIndex]->Groups();
    
    // Go through groups resembling subscriptions and if a group
    // is from the same dataentity as the given
    // protocol-subscription, use it.

    TInt groupIndex( -1 );    
    TRAPD( searchError, groupIndex = FindGroupL( groupsOfClient,
                                                 aEntityId,
                                                 aNamespace ) );    
    if ( searchError == KErrNotFound )
        {
        // Let's create new group because it was not
        // found
        CNcdSubscriptionGroup* group =
            CNcdSubscriptionGroup::NewLC();
        groupsOfClient.AppendL( group );        
        CleanupStack::Pop( group );
        
        group->SetEntityInfoL( aEntityId,
                               aNamespace,
                               aUri,
                               aClientUid );
        
        // set clientIndex to the appended clients subscriptions
        groupIndex = groupsOfClient.Count() - 1;
        }
    else if ( searchError != KErrNone )
        {
        // If leave occurs and it is not KErrNotFound, leave.
        User::Leave( searchError );
        }
    
    return *(groupsOfClient[groupIndex]);
    }

RPointerArray<CNcdSubscriptionGroup> CNcdSubscriptionManager::ServersGroupsL(
    TUid aClientUid,
    const TDesC& aUri ) const
    {
    DLTRACEIN((""));
    // Into this array we gather all groups that are got from the given
    // server
    RPointerArray<CNcdSubscriptionGroup> returnArray;
    CleanupClosePushL( returnArray );

    TInt index = IsInCacheL( aClientUid );
    CNcdClientsSubscriptions* subscriptions = iClientSubscriptions[index];
    // All groups (also groups from other servers)
    RPointerArray<CNcdSubscriptionGroup>& groups = subscriptions->Groups();

    // And now, take only groups that are from the given server
    TInt groupCount( groups.Count() );
    TInt groupIndex( 0 );

    while ( groupIndex < groupCount )
        {
        if ( aUri == groups[groupIndex]->ServerUri() )
            {
            returnArray.AppendL( groups[groupIndex] );
            }
        ++groupIndex;
        }

    CleanupStack::Pop( &returnArray );
    return returnArray;
    }

void CNcdSubscriptionManager::InternalizeAndMarkSubscriptionsL(
    TUid aClientUid,
    const TDesC& aUri,
    RPointerArray<MNcdPreminetProtocolSubscription>&
        aServersSubscriptions )
    {
    DLTRACEIN((""));
    TInt count( aServersSubscriptions.Count() );
    
    for ( TInt i = 0; i < count; i++ )
        {
        DLINFO(( "index: %d of count: %d", i, count ));
        // Write all that can be updated. If some are not
        // successfull, continue to next one.
        TRAP_IGNORE( InternalizeAndMarkSubscriptionL(
                         aClientUid,
                         aUri,
                         *aServersSubscriptions[i] ) );
        }
    }

void CNcdSubscriptionManager::InternalizeAndMarkSubscriptionL(
    TUid aClientUid,
    const TDesC& aUri,
    MNcdPreminetProtocolSubscription& aSubscription )
    {
    DLTRACEIN((""));
    // First internalize and write to db
    InternalizeSubscriptionL( aClientUid,
                              aUri,
                              aSubscription );
    DLINFO((""));
    // Search for group and then mark it as updated so that
    // all that were not updated can be removed later on.
    // If this fails, then this subscription will probably be
    // removed later on.

    CNcdSubscriptionGroup& group = FindGroupL( aClientUid,
                                               aSubscription.EntityId(),
                                               aSubscription.Namespace() );

    SaveGroupIntoDatabaseL( aClientUid, group );
    DLTRACE(( "Subscription group saved into database." ));                                           
    
    // With this flag we later determine if this subscription was
    // received from the server and should not be removed.
    group.SetRecentlyUpdatedL( ETrue,
                               aSubscription.PurchaseOptionId() );
    DLINFO(("subscription set as recently updated"));
    DLTRACEOUT((""));
    }

void CNcdSubscriptionManager::InternalizeSubscriptionL( 
    TUid aClientUid,
    const TDesC& aUri,
    MNcdPreminetProtocolSubscription& aSubscription )
    {
    DLTRACEIN((""));

    // Get or create subscription group

    CNcdSubscriptionGroup& groupForSubscription = ClientsSubscriptionGroupL(
        aClientUid,
        aSubscription.EntityId(),
        aSubscription.Namespace(),
        aUri );


    groupForSubscription.InternalizeSubscriptionL( aSubscription );

    DLTRACE(( "Subscription internalized." ));

    // Database should always have updated info so at this point we have
    // to write updates there.
    
    // Because there is a database for each client the writing has
    // to be done here where we know which group was updated.

    SaveGroupIntoDatabaseL( aClientUid,
                            groupForSubscription );

    DLTRACE(( "Subscription group saved into database." ));
    
    DLTRACEOUT((""));
    }
    
void CNcdSubscriptionManager::RemoveUnmarkedSubscriptionsL(
    TUid aClientUid,
    RPointerArray<CNcdSubscriptionGroup>& aGroups )
    {
    DLTRACEIN((""));
    // Client reference is only needed if a whole group has to be removed
    TInt index = IsInCacheL( aClientUid );
    CNcdClientsSubscriptions* clientsSubscriptions =
        iClientSubscriptions[index];


    TInt groupIndex( aGroups.Count() - 1 );
    
    while ( groupIndex > -1 )
        {
        TBool changesMade = 
            aGroups[groupIndex]->RemoveUnmarkedSubscriptionsAndUnmarkL();
        
        if ( changesMade )
            {
            if ( aGroups[groupIndex]->SubscriptionCount() == 0 )
                {
                // No subscriptions left in the group, remove the group
                
                // from cache
                clientsSubscriptions->RemoveGroup( aGroups[groupIndex] );
                // from db
                RemoveGroupFromDatabaseL( aClientUid,
                                          *aGroups[groupIndex] );
                
                // Remove the group also from the array given array
                // so the array is up to date and can be used after
                // completion of this function
                aGroups.Remove( groupIndex );
                }
            else
                {
                // Subscriptions left in the group, write current
                // status of the group into db
                SaveGroupIntoDatabaseL( aClientUid, *aGroups[groupIndex] );
                }                            
            }
        --groupIndex;
        }

    // As the clientsSubscriptions only exists in the cache,
    // it is not checked that if groups still exist after this
    // group removal and whether the clientsSubscriptions
    // should be removed.
    }

RPointerArray<CNcdSubscriptionsSourceIdentifier> 
    CNcdSubscriptionManager::SubscriptionsSourcesL( TInt aClientIndex ) const
    {
    DLTRACEIN(( "Getting subscriptions sources from subscriptions db." ));
    RPointerArray<CNcdSubscriptionGroup>& clientsGroups =
        iClientSubscriptions[aClientIndex]->Groups();
    
    TInt groupCount( clientsGroups.Count() );
    if ( groupCount < 1 )
        {
        User::Leave( KErrNotFound );
        }
        
    RPointerArray<CNcdSubscriptionsSourceIdentifier> returnArray;
    CleanupResetAndDestroyPushL( returnArray );
    
    // Let's take the first subscriptions source info always into
    // the array because nothing is yet in the array.
    
    // Notice that in this function we set all source identifiers not
    // to require capability check. This is because we assume that
    // if the server has once supported subscriptions, it will support
    // them also later.

    TInt groupIndex( 0 );
    
    CNcdSubscriptionsSourceIdentifier* tmpIdentifier =
        CNcdSubscriptionsSourceIdentifier::NewLC(
            clientsGroups[groupIndex]->ServerUri(),
            clientsGroups[groupIndex]->Namespace(),
            EFalse );
            
    returnArray.AppendL( tmpIdentifier );
    CleanupStack::Pop( tmpIdentifier );
    tmpIdentifier = NULL;
    
    ++groupIndex;
    
    // Then go through others one at a time and check that such info
    // is not already found from the array.
    
    // Notice that only uri and namespace are compared here,
    // capability requirement is not checked
    TIdentityRelation<CNcdSubscriptionsSourceIdentifier> match( 
            &CNcdSubscriptionsSourceIdentifier::CompareIdentifiers );
    
    while ( groupIndex < groupCount )
        {
        tmpIdentifier = CNcdSubscriptionsSourceIdentifier::NewLC(
            clientsGroups[groupIndex]->ServerUri(),
            clientsGroups[groupIndex]->Namespace(),
            EFalse );

        if ( returnArray.Find( tmpIdentifier, match ) == KErrNotFound )
            {
            returnArray.AppendL( tmpIdentifier );
            CleanupStack::Pop( tmpIdentifier );
            }
        else
            {
            CleanupStack::PopAndDestroy( tmpIdentifier );
            }
        
        tmpIdentifier = NULL;
    
        ++groupIndex;
        }
    
    CleanupStack::Pop( &returnArray );
    
    return returnArray;    
    }


void CNcdSubscriptionManager::ExternalizeClientsSubscriptionsL(
    MCatalogsSession& aSession,
    const RPointerArray<CNcdKeyValuePair>& aGroupIdentifiers,
    RWriteStream& aStream )
    {
    DLTRACEIN((""));

    if ( aGroupIdentifiers.Count() == 0) 
        {
        aStream.WriteInt32L( 0 );
        return;
        }

    // Because one subscription manager serves all clients, we
    // check here which client uid the client has and pass it
    // forward
    MCatalogsContext& context = aSession.Context();
    TUid familyId = context.FamilyId();

    // check if clients info is in cache
    TInt index( -1 );
    index = IsInCacheL( familyId );

    ExternalizeDataForRequestL( index, aSession, aGroupIdentifiers, aStream );
    DLTRACEOUT((""));
    }

void CNcdSubscriptionManager::LoadClientsInfoFromStorageL( TUid aUid )
    {
    DLTRACEIN((""));

    DLTRACE(( "Loading subscription info from storage for client: %d",
              aUid.iUid ));

    // Loading from the current client
    MNcdStorage& storage = *StorageL( aUid, 
        NcdProviderDefines::KSubscriptionNamespace );
    
    // NOTE: this creates the database if it does not already exist
    MNcdDatabaseStorage& database = 
        storage.DatabaseStorageL( NcdProviderDefines::KDefaultDatabaseUid );
    
    CNcdClientsSubscriptions* clientsSubscriptions =
        CNcdClientsSubscriptions::NewLC( aUid );
    
    iClientSubscriptions.AppendL( clientsSubscriptions );
    
    CleanupStack::Pop( clientsSubscriptions );
    
    
    RPointerArray<MNcdStorageItem> items;
    CleanupClosePushL( items );
    database.StorageItemsL( items );
    
    const TInt foudItemCount( items.Count() );

    DLTRACE(( "Found %d subscription storage items for client %d.",
              foudItemCount,
              aUid.iUid ));

    if ( items.Count() < 1 )
        {
        // Nothing is found for given client, but the
        // ClientsSubscriptions-object is created for possible later
        // use
        CleanupStack::PopAndDestroy( &items );
        return;
        }    
    
    TInt itemIndex( 0 );
    while( itemIndex < foudItemCount )
        {
        CNcdSubscriptionGroup* group = CNcdSubscriptionGroup::NewLC();
        
        clientsSubscriptions->AddGroupL( group );
        
        CleanupStack::Pop( group );
        
        // Note: database has the ownership of the item
        MNcdStorageItem* item = items[itemIndex];

        // Get data from database by using aDataItem as the target so that 
        // internalize will be called for it
        item->SetDataItem( group );
        
        // Read node data -> calls InternalizeL of aDataItem
        item->ReadDataL();
        
        ++itemIndex;
        }
    CleanupStack::PopAndDestroy( &items );
    DLTRACEOUT((""));    
    }


void CNcdSubscriptionManager::ExternalizeDataForRequestL(
    TInt aIndex,
    MCatalogsSession& aSession,
    const RPointerArray<CNcdKeyValuePair>& aGroupIdentifiers,
    RWriteStream& aStream )
    {
    DLTRACEIN((""));
    
    if ( aIndex < 0 )
        {
        // No entries with given client uid. If there was one, then its
        // Externalize (which is called later) has to write the amount
        // of groups as its first entry. This way we have entry amount
        // always as the first entry in the stream.
        aStream.WriteInt32L( 0 );
        return;
        }
    
    CNcdClientsSubscriptions* clientsSubscriptions = 
        iClientSubscriptions[aIndex];

    RPointerArray<CNcdSubscriptionGroup>& groups =
        clientsSubscriptions->Groups();
    
    aStream.WriteInt32L( aGroupIdentifiers.Count() );
    for ( TInt i = 0; i < aGroupIdentifiers.Count(); i++ ) 
        {
        const TDesC& nameSpace = aGroupIdentifiers[i]->Key();
        const TDesC& entityId = aGroupIdentifiers[i]->Value();
    
        TBool found = EFalse;
        for ( TInt i = 0; i < groups.Count(); i++ ) 
            {
            CNcdSubscriptionGroup* group = groups[i];
            if ( group->Namespace() == nameSpace &&
                 group->EntityId() == entityId ) 
                {
                TInt tmpHandle( aSession.AddObjectL( group ) );
                DLTRACE(( "Sending subscriptiongroup handle: %i",
                    tmpHandle ));
                TRAPD( addError, aStream.WriteInt32L( tmpHandle ));
                if ( addError != KErrNone ) 
                    {
                    // Should all other added objects be removed from
                    // the session also?
                    aSession.RemoveObject( tmpHandle );
                    User::Leave( addError );
                    }
                found = ETrue;
                break;
                }
            }
        if ( !found ) 
            {
            User::Leave( KErrNotFound );
            }
        }
                
    DLTRACEOUT((""));
    }


HBufC* CNcdSubscriptionManager::GenerateGroupDataIdL(
    CNcdSubscriptionGroup& aGroup )
    {
    // Items written to database are identified by classtype (integer)
    // and dataid (string). Here we use a constant classtype so the
    // dataid has to be unique. Because group contains subscriptions
    // available from one entity, we use entityid and namespace to
    // identify the item. The format is as follows:
    // namespace<separator>entityid
    // This kind of format does not promise full uniqueness but it is
    // hopefully close enough.

    const TDesC& entityNamespace = aGroup.Namespace();
    TInt namespaceLength( entityNamespace.Length() );
    
    const TDesC& entityId = aGroup.EntityId();
    TInt entityIdLength( entityId.Length() );
    
    _LIT( KIdSeparator, "|" );
    
    HBufC* dataId = HBufC::NewL( namespaceLength + entityIdLength + 1 );
    TPtr modifiableDataId = dataId->Des();
    
    modifiableDataId.Append( entityNamespace );
    modifiableDataId.Append( KIdSeparator );
    modifiableDataId.Append( entityId );

    return dataId;
    }

void CNcdSubscriptionManager::RemoveGroupFromDatabaseL(
    TUid aClientUid,
    CNcdSubscriptionGroup& aGroup )
    {
    DLTRACEIN((""));
    
    // Databases are created for each client. (for each uid)
    // As a namespace we use a constant namespace (KSubscriptionNameSpace)
    // so we know in the future where from to load all data.

    HBufC* dataId = GenerateGroupDataIdL( aGroup );
    CleanupStack::PushL( dataId );

    DLTRACE(( _L("DataId for subscription group database item: %S"),
              dataId ));


    RemoveDataFromDatabaseL( aClientUid,
                             NcdProviderDefines::KSubscriptionNamespace,
                             *dataId,
                             NcdNodeClassIds::ENcdSubscriptionsData );
                           
    CleanupStack::PopAndDestroy( dataId );
    
    DLTRACEOUT((""));
    }

void CNcdSubscriptionManager::RemoveDataFromDatabaseL( 
    TUid aUid,
    const TDesC& aNamespace,
    const TDesC& aDataId,
    NcdNodeClassIds::TNcdNodeClassType aClassType )
    {
    DLTRACEIN(( _L("%S::%S"), &aNamespace, &aDataId ));

    DLTRACE(( _L("Removing database item with dataid: %S and classtype: %d"),
              &aDataId,
              aClassType ));

    if( aNamespace == KNullDesC || aDataId == KNullDesC )
        {
        DLTRACE(( _L("Empty namespace or id given for storage removing: %S, %S"),
                  &aNamespace, 
                  &aDataId ));
        User::Leave( KErrArgument );
        }
    
    // Removing from the current client and from the namespace in which
    // the metadata is defined
    MNcdStorage* storage = StorageL( aUid, aNamespace );
        
    // NOTE: this creates the database if it does not already exist
    MNcdDatabaseStorage& database = 
        storage->DatabaseStorageL( NcdProviderDefines::KDefaultDatabaseUid );
            
    // Get/create the storage item which is used in removal
    // Note: database has the ownership of the item
    MNcdStorageItem* storageItem = 
        database.StorageItemL( aDataId, aClassType );    
    
    // Remove the item from the storage
    storageItem->RemoveFromStorageL();

    // Make the removing happen.
    database.CommitL();       
    
    DLTRACEOUT((""));    
    }

void CNcdSubscriptionManager::SaveDataIntoDatabaseL( 
    TUid aUid,
    const TDesC& aNamespace,
    const TDesC& aDataId,
    MNcdStorageDataItem& aDataItem,
    NcdNodeClassIds::TNcdNodeClassType aClassType )
    {
    DLTRACEIN(( _L("%S::%S"), &aNamespace, &aDataId ));

    DLTRACE(( _L("Saving to database item with dataid: %S and classtype: %d"),
              &aDataId,
              aClassType ));

    if( aNamespace == KNullDesC || aDataId == KNullDesC )
        {
        DLTRACE(( _L("Empty namespace or id given for storage saving: %S, %S"),
                  &aNamespace, 
                  &aDataId ));
        User::Leave( KErrArgument );
        }
    
    // Storing to the current client and to the namespace in which
    // the metadata is defined
    MNcdStorage* storage = StorageL( aUid, aNamespace );
        
    // NOTE: this creates the database if it does not already exist
    MNcdDatabaseStorage& database = 
        storage->DatabaseStorageL( NcdProviderDefines::KDefaultDatabaseUid );
            
    // Get/create the storage item where the data is saved
    // Note: database has the ownership of the item
    MNcdStorageItem* storageItem = 
        database.StorageItemL( aDataId, aClassType );    
    
    // Here call the storage functions that will handle 
    // the saving of the data
    
    // Save new item to database
    storageItem->SetDataItem( &aDataItem );
    storageItem->OpenL();
    
    // Calls ExternalizeL for data item
    storageItem->WriteDataL();
    
    // Save the data to the database.
    // The data object implements MNcdStorageDataItem interface.
    // So, the externalize function will insert the data to the stream
    // that the database handler will save to the database.
    storageItem->SaveL();        
    
    DLTRACEOUT((""));    
    }
    
    
// ---------------------------------------------------------------------------
// StorageL
// ---------------------------------------------------------------------------
//    
MNcdStorage* CNcdSubscriptionManager::StorageL( 
    TUid aUid,
    const TDesC& aNamespace ) const
    {    
    DLTRACEIN((""));
    MNcdStorage* storage = NULL;
        
    DLTRACE(( _L("Subscriptions storage namespace: %S"), &aNamespace ));

    // No need to clean the name anymore
    HBufC* uid = aUid.Name().AllocLC();

    TRAPD( err, 
        {
        storage = &iStorageManager.StorageL( *uid, 
                                             aNamespace );
        });
    
    if ( err == KErrNotFound ) 
        {
        DLTRACE(("Creating subscription storage for the client"));
        DLTRACE(( _L("Client identifying uid is %S"), uid ));
        storage =
            &iStorageManager.CreateStorageL( *uid, 
                                             aNamespace );
        }
    else if ( err != KErrNone )
        {
        DLTRACE(("Leaving: %i", err));
        User::Leave( err );   
        }
        
    CleanupStack::PopAndDestroy( uid );

    DLTRACEOUT((""));
    return storage;
    }

void CNcdSubscriptionManager::ListenerEnrolledL(
    MCatalogsBaseMessage& aMessage ) 
    {
    DLTRACEIN((""));
    iPendingMessages.AppendL( &aMessage );
    }
    
void CNcdSubscriptionManager::NotifyAllListeners( TInt aError ) 
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iPendingMessages.Count(); i++ ) 
        {
        iPendingMessages[i]->CompleteAndRelease( aError );
        }
    iPendingMessages.Reset();
    }  
