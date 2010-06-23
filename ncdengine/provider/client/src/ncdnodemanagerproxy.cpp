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
* Description:   Implements CNcdNodeManagerProxy class
*
*/


#include <e32err.h>

#include "ncdnodemanagerproxy.h"
#include "ncdnodeproxy.h"
#include "ncdnodefolderproxy.h"
#include "ncdbundlefolderproxy.h"
#include "ncdsearchnodefolderproxy.h"
#include "ncdnodeitemproxy.h"
#include "ncdsearchnodeitemproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "ncdproviderdefines.h"
#include "ncdnodeidentifier.h"
#include "ncdrootnodeproxy.h"
#include "ncdpurchasedetails.h"
#include "catalogsdebug.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdsearchrootnodeproxy.h"
#include "catalogsconstants.h"
#include "catalogsutils.h"
#include "ncdproviderproxy.h"
#include "ncdexpirednode.h"
#include "ncdnodesupplierproxy.h"
#include "ncdnodeidentifiereditor.h"
#include "ncdsearchnodebundleproxy.h"
#include "ncdfavoritemanagerproxy.h"
#include "ncdpanics.h"


CNcdNodeManagerProxy* CNcdNodeManagerProxy::NewL(
    MCatalogsClientServer& aSession, 
    TInt aHandle,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdSubscriptionManagerProxy& aSubscriptionManager,
    CNcdProviderProxy& aProvider )
    {
    CNcdNodeManagerProxy* self = 
        CNcdNodeManagerProxy::NewLC( aSession, 
                                     aHandle,
                                     aOperationManager,
                                     aSubscriptionManager,
                                     aProvider );
    CleanupStack::Pop( self );
    return self;        
    }


CNcdNodeManagerProxy* CNcdNodeManagerProxy::NewLC(
    MCatalogsClientServer& aSession, 
    TInt aHandle,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdSubscriptionManagerProxy& aSubscriptionManager,
    CNcdProviderProxy& aProvider )
    {
    CNcdNodeManagerProxy* self = 
        new( ELeave ) CNcdNodeManagerProxy( aSession,
                                            aHandle,
                                            aOperationManager,
                                            aSubscriptionManager,
                                            aProvider );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;        
    }


CNcdNodeManagerProxy::CNcdNodeManagerProxy(
    MCatalogsClientServer& aSession, 
    TInt aHandle,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdSubscriptionManagerProxy& aSubscriptionManager,
    CNcdProviderProxy& aProvider )
: CNcdBaseProxy( aSession, aHandle ),
  iOperationManager( aOperationManager ),
  iSubscriptionManager( aSubscriptionManager ),
  iFavoriteManager( NULL ),
  iProvider( aProvider )
    {
    }


void CNcdNodeManagerProxy::ConstructL()
    {
    iOperationManager.SetNodeManager( this );

    }


CNcdNodeManagerProxy::~CNcdNodeManagerProxy()
    {
    DLTRACEIN((""));
    
    if( iSearchRootNode )
        {
        // release own reference to 
        iSearchRootNode->InternalRelease();
        }
    
    // Release all nodes without reference count. These nodes have not
    // been used after creation.
    for ( TInt i = iNodeCache.Count() - 1; i >= 0; i-- ) 
        {
        if ( iNodeCache[i]->TotalRefCount() == 0 )
            {
            // During delete, node is removed from the array automatically.
            // Notice that it is safe to call delete here instead of Release,
            // because we checked the total count above. Release implementation 
            // contains DASSERT that would assert if we called Release here.
            delete iNodeCache[i];
            }
        }
    
    // For debugging purposes check if some nodes have been left hanging.
    // All the nodes should be released before manager is deleted.

    DLINFO(("Cache count: %d", iNodeCache.Count()));

    #ifdef CATALOGS_BUILD_CONFIG_DEBUG
        DLINFO(("The following nodes have not been properly released:"));
        for ( TInt i = 0; i < iNodeCache.Count(); i++ ) 
            {
            DLINFO(( _L("MNcdNode: %x, namespace: %S, id: %S, refcount: %d"), 
                static_cast<MNcdNode*>( iNodeCache[i] ),
                &iNodeCache[i]->Namespace(), &iNodeCache[i]->Id(),
                iNodeCache[i]->TotalRefCount() ));
                
            }
        DLINFO(("release check end"));        
    #endif

    DASSERT( iNodeCache.Count() == 0 );           
        
    // Close the cache.
    iNodeCache.Close();

    // Notice that the root node is also included into the cache.
    // So, no need to delete it separately.

    DLTRACEOUT((""));
    }


CNcdNodeProxy& CNcdNodeManagerProxy::NodeL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((_L("Node ID: %S, %S, %d"), 
                &aNodeIdentifier.NodeId(),
                &aNodeIdentifier.NodeNameSpace(),
                aNodeIdentifier.ClientUid()));

    // Check if the node proxy already exists in the cache.
    // Every node has its own namespace and dataid.
    // If it does, do not create it.
        
    DLTRACE(("Node cache count: %d", iNodeCache.Count() ));

    CNcdNodeProxy* node( NodeExists( aNodeIdentifier ) );
    if ( node != NULL )
        {
        // Node was found. So, return it.
        DLTRACEOUT(("Node found"));
        return *node;
        }
    else if ( aNodeIdentifier.NodeNameSpace() 
                == NcdProviderDefines::KRootNodeNameSpace )
        {
        DLTRACE(("RootNode"));
        // Because the node was not found from the cache,
        // create a new node.
        return RootNodeL();
        }
    else
        {
        DLTRACE(("Create a new node"));
        // Because the node was not found from the cache,
        // create a new node.
        return CreateNodeL( aNodeIdentifier );        
        }
    }


CNcdRootNodeProxy& CNcdNodeManagerProxy::RootNodeL()
    {
    DLTRACEIN((""));

    if( iRootNode == NULL )
        {
        DLTRACE(("Root did not exist. Create it."));
        
        // Root does not exist. So, create it.

        // First get the handle to the node of the server side.
        // The handle is gotten from the server side nodemanager.
        TInt rootHandle( 0 );
        User::LeaveIfError(
                ClientServerSession().
                    SendSync( NcdNodeFunctionIds::ENcdRootNodeHandle,
                              KNullDesC,
                              rootHandle,
                              Handle() ) );

        DLTRACE(("Root handle: %d", rootHandle ));
        
        // Now we have handle. So, create the actual root proxy. 
        // Notice that if the proxy object leaves during the construction
        // the destructor automatically releases the handle from the server side.               
        iRootNode = 
            CNcdRootNodeProxy::NewL( ClientServerSession(), 
                                     rootHandle,
                                     *this,
                                     iOperationManager,
                                     *iFavoriteManager );
                                      
        // Because root node is created now, it is not in the cache yet.
        // Put it into the cache. So, users can find the root then also
        // by using the NodeL function.
        iNodeCache.AppendL( iRootNode );
        }

    DLTRACEOUT((""));
    
    return *iRootNode;
    }
  
CNcdSearchRootNodeProxy& CNcdNodeManagerProxy::SearchRootNodeL()
    {
    DLTRACEIN((""));

    if( iSearchRootNode == NULL )
        {
        DLTRACE(("Search root did not exist. Create it."));
        
        // Search root does not exist. So, create it.

        // First get the handle to the node of the server side.
        // The handle is gotten from the server side nodemanager.
        TInt searchRootHandle( 0 );
        User::LeaveIfError(
                ClientServerSession().
                    SendSync( NcdNodeFunctionIds::ENcdSearchRootNodeHandle,
                              KNullDesC,
                              searchRootHandle,
                              Handle() ) );

        DLTRACE(("Search root handle: %d", searchRootHandle ));
        
        // Now we have handle. So, create the actual search root proxy. 
        // Notice that if the proxy object leaves during the construction
        // the destructor automatically releases the handle from the server side.               
        iSearchRootNode = 
            CNcdSearchRootNodeProxy::NewL( ClientServerSession(), 
                                     searchRootHandle,
                                     *this,
                                     iOperationManager,
                                     *iFavoriteManager );
        // call addref to keep search root alive for search ops (search root 
        // is set as observer for them)
        iSearchRootNode->InternalAddRef();
        // Because search root node is created now, it is not in the cache yet.
        // Put it into the cache. So, users can find the search root then also
        // by using the NodeL function.
        iNodeCache.AppendL( iSearchRootNode );
        }

    DLTRACEOUT((""));
    
    return *iSearchRootNode;
    }


CNcdNodeProxy& CNcdNodeManagerProxy::CreateSchemeNodeL(
    const CNcdNodeIdentifier& aMetadataIdentifier,
    TNcdSchemeNodeType aType,
    TBool aRemoveOnDisconnect,
    TBool aForceCreate )
    {
    DLTRACEIN((""));
    
    CNcdNodeProxy* node( NULL );
    
    // Check the favorites for the given metadata and return the node which has
    // the metadata if one exists.
    node = iFavoriteManager->FavoriteNodeByMetaDataL( aMetadataIdentifier );
    
    if ( node )
        {
        return *node;
        }
        
    // Node was not found from favorites.
    if ( aForceCreate )
        {
        // Creation is forced, create node of correct type.
        switch ( aType )
            {
            case ENcdSchemeItem:
                
                node = &CreateTemporaryNodeItemL( aMetadataIdentifier, EFalse );
                break;
                
            case ENcdSchemeFolder:
                node = &CreateTemporaryNodeFolderL( aMetadataIdentifier, EFalse );
                break;
                
            case ENcdSchemeBundleFolder:
                node = &CreateTemporaryBundleFolderL( aMetadataIdentifier, EFalse );
                break;
            
            default:
                NCD_ASSERT_ALWAYS( EFalse, ENcdPanicInvalidArgument );
                break;
            }
        }
    else 
        {
        // Creation not forced, create temporary node item if metadata exists already.
        // This leaves with KErrNotFound if metadata does not exist, thus the
        // node cannot be created.
        node = &CreateTemporaryNodeIfMetadataExistsL( aMetadataIdentifier );
        }

    DASSERT( node );
    
    // Scheme nodes must be added to favourites.
    node->AddToFavoritesL( aRemoveOnDisconnect );

    DLTRACEOUT((""));
    
    return *node;
    }


CNcdNodeProxy& CNcdNodeManagerProxy::CreateTemporaryNodeL( 
    const MNcdPurchaseDetails& aDetails )
    {
    DLTRACEIN((""));

    CNcdNodeProxy* node( NULL );

    CNcdNodeIdentifier* identifier( 
        CNcdNodeIdentifier::NewLC( aDetails.Namespace(), 
                                   aDetails.EntityId(), 
                                   aDetails.ServerUri(),
                                   aDetails.ClientUid() ) );

    if ( aDetails.ItemType() == MNcdPurchaseDetails::EItem )
        {
        DLINFO(("Item from purchase history"));
        node = &CreateTemporaryNodeItemL( *identifier, ETrue );
        }
    else if ( aDetails.ItemType() == MNcdPurchaseDetails::EFolder )
        {
        DLINFO(("Folder from purchase history"));
        node = &CreateTemporaryNodeFolderL( *identifier, ETrue );
        }
    else
        {
        DLINFO(("Unknown from purchase history"));
        node = &CreateTemporaryOrSupplierNodeL( *identifier );
        }

    CleanupStack::PopAndDestroy( identifier );

    DLTRACEOUT((""));

    return *node;
    }
    
CNcdNodeProxy& CNcdNodeManagerProxy::CreateTemporaryNodeFolderL( 
    const CNcdNodeIdentifier& aMetadataIdentifier, TBool aCreateMetaData )
    {
    DLTRACEIN((_L("Metadata ID: %S, %S, %S, %d"), 
                &aMetadataIdentifier.NodeId(),
                &aMetadataIdentifier.NodeNameSpace(),
                &aMetadataIdentifier.ServerUri(),
                aMetadataIdentifier.ClientUid()));        

    // Check if the node proxy already exists in the cache.
    // Every node has its own namespace and dataid.
    // If it does exist, do not create it.

    // At first, try to find the node from cache by creating the node identifier
    // from the metadata identifier.
    CNcdNodeIdentifier* tempNodeIdentifier =
        NcdNodeIdentifierEditor::CreateTemporaryNodeIdentifierLC( aMetadataIdentifier );
    CNcdNodeProxy* node( NodeExists( *tempNodeIdentifier ) );
    
    if ( node ) 
        {
        // Node was found. So, return it.
        CleanupStack::PopAndDestroy( tempNodeIdentifier );
        DLTRACEOUT(("Node found in cache"));
        return *node;
        }

    // Notice that the actual node identifier is given here, instead of the
    // metadata identifier.
    HBufC8* nodeIdentifierData( tempNodeIdentifier->NodeIdentifierDataL() );
    CleanupStack::PopAndDestroy( tempNodeIdentifier );
    CleanupStack::PushL( nodeIdentifierData );
        
    DLINFO(("Get node handle"));
    // First get the handle to the node of the server side.
    // The handle is gotten from the server side nodemanager.
    
    // NOTE: The identifier may contain NULL UID here, but it will
    // be replaced by the family id in the server side. And then
    // the node proxy will contain the right UID when the created node is
    // internalized during its creation.
    TInt function = aCreateMetaData ? 
        NcdNodeFunctionIds::ENcdTemporaryNodeFolderWithMetaDataHandle : 
        NcdNodeFunctionIds::ENcdTemporaryNodeFolderHandle;
    
    TInt nodeHandle( 0 );
    User::LeaveIfError(
            ClientServerSession().
                SendSync( function,
                          *nodeIdentifierData,
                          nodeHandle,
                          Handle() ) );

    DLTRACE(("Node handle: %d", nodeHandle ));

    CleanupStack::PopAndDestroy( nodeIdentifierData );

    // No need to check the class id here, because we requested an item
    // node above.

    node = 
        CNcdNodeFolderProxy::NewLC( ClientServerSession(),
                                    nodeHandle,
                                    *this,
                                    iOperationManager,
                                    *iFavoriteManager );

    // Because we creted a new node, it should be added to the cache.
    iNodeCache.AppendL( node );

    CleanupStack::Pop( node );

    DLTRACEOUT((""));
    
    return *node;
    }
    
CNcdNodeProxy& CNcdNodeManagerProxy::CreateTemporaryBundleFolderL(
    const CNcdNodeIdentifier& aMetadataIdentifier, TBool aCreateMetaData )
    {
    DLTRACEIN((_L("Metadata ID: %S, %S, %S, %d"), 
                &aMetadataIdentifier.NodeId(),
                &aMetadataIdentifier.NodeNameSpace(),
                &aMetadataIdentifier.ServerUri(),
                aMetadataIdentifier.ClientUid()));        

    // Check if the node proxy already exists in the cache.
    // Every node has its own namespace and dataid.
    // If it does exist, do not create it.

    // At first, try to find the node from cache by creating the node identifier
    // from the metadata identifier.
    CNcdNodeIdentifier* tempNodeIdentifier =
        NcdNodeIdentifierEditor::CreateTemporaryNodeIdentifierLC( aMetadataIdentifier );
    CNcdNodeProxy* node( NodeExists( *tempNodeIdentifier ) );
    
    if ( node ) 
        {
        // Node was found. So, return it.
        CleanupStack::PopAndDestroy( tempNodeIdentifier );
        DLTRACEOUT(("Node found in cache"));
        return *node;
        }

    // Notice that the actual node identifier is given here, instead of the
    // metadata identifier.
    HBufC8* nodeIdentifierData( tempNodeIdentifier->NodeIdentifierDataL() );
    CleanupStack::PopAndDestroy( tempNodeIdentifier );
    CleanupStack::PushL( nodeIdentifierData );
        
    DLINFO(("Get node handle"));
    // First get the handle to the node of the server side.
    // The handle is gotten from the server side nodemanager.
    
    // NOTE: The identifier may contain NULL UID here, but it will
    // be replaced by the family id in the server side. And then
    // the node proxy will contain the right UID when the created node is
    // internalized during its creation.
    TInt function = aCreateMetaData ?
        NcdNodeFunctionIds::ENcdTemporaryBundleFolderWithMetaDataHandle :
        NcdNodeFunctionIds::ENcdTemporaryBundleFolderHandle;
    
    TInt nodeHandle( 0 );
    User::LeaveIfError(
            ClientServerSession().
                SendSync( function,
                          *nodeIdentifierData,
                          nodeHandle,
                          Handle() ) );

    DLTRACE(("Node handle: %d", nodeHandle ));

    CleanupStack::PopAndDestroy( nodeIdentifierData );

    // No need to check the class id here, because we requested a bundle folder
    // node above.
    node = 
        CNcdBundleFolderProxy::NewLC( ClientServerSession(),
                                      nodeHandle,
                                      *this,
                                      iOperationManager,
                                      *iFavoriteManager );

    // Because we creted a new node, it should be added to the cache.
    iNodeCache.AppendL( node );

    CleanupStack::Pop( node );

    DLTRACEOUT((""));
    
    return *node;
    }

    
CNcdNodeProxy& CNcdNodeManagerProxy::CreateTemporaryNodeItemL(
    const CNcdNodeIdentifier& aMetadataIdentifier,
    TBool aCreateMetaData )
    {
    DLTRACEIN((_L("Metadata ID: %S, %S, %S, %d"), 
                &aMetadataIdentifier.NodeId(),
                &aMetadataIdentifier.NodeNameSpace(),
                &aMetadataIdentifier.ServerUri(),
                aMetadataIdentifier.ClientUid()));        

    // Check if the node proxy already exists in the cache.
    // Every node has its own namespace and dataid.
    // If it does exist, do not create it.

    // At first, try to find the node from cache by creating the node identifier
    // from the metadata identifier.
    CNcdNodeIdentifier* tempNodeIdentifier =
        NcdNodeIdentifierEditor::CreateTemporaryNodeIdentifierLC( aMetadataIdentifier );
    CNcdNodeProxy* node( NodeExists( *tempNodeIdentifier ) );
    
    if ( node ) 
        {
        // Node was found. So, return it.
        CleanupStack::PopAndDestroy( tempNodeIdentifier );
        DLTRACEOUT(("Node found in cache"));
        return *node;
        }

    // Notice that the actual node identifier is given here, instead of the
    // metadata identifier.
    HBufC8* nodeIdentifierData( tempNodeIdentifier->NodeIdentifierDataL() );
    CleanupStack::PopAndDestroy( tempNodeIdentifier );
    CleanupStack::PushL( nodeIdentifierData );
        
    DLINFO(("Get node handle"));
    // First get the handle to the node of the server side.
    // The handle is gotten from the server side nodemanager.
    
    // NOTE: The identifier may contain NULL UID here, but it will
    // be replaced by the family id in the server side. And then
    // the node proxy will contain the right UID when the created node is
    // internalized during its creation.
    TInt function = aCreateMetaData ?
        NcdNodeFunctionIds::ENcdTemporaryNodeItemWithMetaDataHandle :
        NcdNodeFunctionIds::ENcdTemporaryNodeItemHandle;
    
    TInt nodeHandle( 0 );
    User::LeaveIfError(
            ClientServerSession().
                SendSync( function,
                          *nodeIdentifierData,
                          nodeHandle,
                          Handle() ) );

    DLTRACE(("Node handle: %d", nodeHandle ));

    CleanupStack::PopAndDestroy( nodeIdentifierData );

    // No need to check the class id here, because we requested an item
    // node above.

    node = 
        CNcdNodeItemProxy::NewLC( ClientServerSession(),
                                  nodeHandle,
                                  *this,
                                  iOperationManager,
                                  *iFavoriteManager );

    // Because we creted a new node, it should be added to the cache.
    iNodeCache.AppendL( node );

    CleanupStack::Pop( node );

    DLTRACEOUT((""));
    
    return *node;
    }
    
    
CNcdNodeProxy& CNcdNodeManagerProxy::CreateTemporaryNodeIfMetadataExistsL(
    const CNcdNodeIdentifier& aMetadataIdentifier )
    {
    DLTRACEIN((_L("Metadata ID: %S, %S, %S, %d"), 
                &aMetadataIdentifier.NodeId(),
                &aMetadataIdentifier.NodeNameSpace(),
                &aMetadataIdentifier.ServerUri(),
                aMetadataIdentifier.ClientUid()));
                
    CNcdNodeIdentifier* tempNodeIdentifier =
        NcdNodeIdentifierEditor::CreateTemporaryNodeIdentifierLC( aMetadataIdentifier );
                           
    // Check if the node proxy already exists in the cache.
    // Every node has its own namespace and dataid.
    // If it does exist, do not create it.
    CNcdNodeProxy* node( NodeExists( *tempNodeIdentifier ) );    
    if ( node ) 
        {
        // Node was found. So, return it.
        CleanupStack::PopAndDestroy( tempNodeIdentifier );
        DLTRACEOUT(("Node found in cache"));
        return *node;
        }
        
    // Node was not found from the cache. So, new node proxy has to be created.
                    
    // Notice that the actual node identifier is given here for the server, instead of the
    // metadata identifier.
    HBufC8* nodeIdentifierData( tempNodeIdentifier->NodeIdentifierDataL() );
    CleanupStack::PushL( nodeIdentifierData );
        
    DLINFO(("Create temporary if metadata exists"));

    // This is the output value from SendSync. It tells whether the node was created in server side.
    // It will be ETrue also when the node existed there already.
    TBool isCreated( EFalse );
    
    // NOTE: The identifier may contain NULL UID here, but it will
    // be replaced by the family id in the server side. And then
    // the node proxy will contain the right UID when the created node is
    // internalized during its creation.
    // This will create the temporary node in server side if the metadata exists there already.
    // So, it can be requested next when the node proxy will be created.
    User::LeaveIfError(
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdCreateTemporaryNodeIfMetadataExists,
                          *nodeIdentifierData,
                          isCreated,
                          Handle() ) );

    CleanupStack::PopAndDestroy( nodeIdentifierData );
    
    if ( isCreated ) 
        {
        // The node was created above in the server side if it did not
        // already exist. So, we can be sure that we get the node below.
        // NodeL knows how to create the correct node and inserts it to the cache. 
        // So, use that function here.
        // Notice, that there is no need to use ReplaceCacheNodeL here because the
        // supplier node can use it itself when the correct node data has been loaded
        // from the web.
        node = &NodeL( *tempNodeIdentifier );
        CleanupStack::PopAndDestroy( tempNodeIdentifier );
        }
    else 
        {
        User::Leave( KErrNotFound );
        }

    DLTRACEOUT(("metadata was found and temporary node created"));
    
    return *node;
    }
    
    
CNcdNodeProxy& CNcdNodeManagerProxy::CreateTemporaryOrSupplierNodeL(
    const CNcdNodeIdentifier& aMetadataIdentifier )
    {
    DLTRACEIN((_L("Metadata ID: %S, %S, %S, %d"), 
                &aMetadataIdentifier.NodeId(),
                &aMetadataIdentifier.NodeNameSpace(),
                &aMetadataIdentifier.ServerUri(),
                aMetadataIdentifier.ClientUid()));        

    // Notice that the supplier node and the temporary node use same identifiers
    // but only one version should exist at a time.

    CNcdNodeIdentifier* tempNodeIdentifier =
        NcdNodeIdentifierEditor::CreateTemporaryNodeIdentifierLC( aMetadataIdentifier );

    // Check if the node proxy already exists in the cache.
    // Every node has its own namespace and dataid.
    // If it does exist, do not create it.
    CNcdNodeProxy* node( NodeExists( *tempNodeIdentifier ) );    
    if ( node ) 
        {
        // Node was found. So, return it.
        CleanupStack::PopAndDestroy( tempNodeIdentifier );
        DLTRACEOUT(("Node found in cache"));
        return *node;
        }

    // Node was not found from the cache. So, new node proxy has to be created.
    
    // Notice that the actual node identifier is given here for the server, instead of the
    // metadata identifier.
    HBufC8* nodeIdentifierData( tempNodeIdentifier->NodeIdentifierDataL() );
    CleanupStack::PushL( nodeIdentifierData );
        
    DLINFO(("Create temporary or supplier node proxy"));

    // tmpNum is not used for anything, but give the SendSync at least some parameter.    
    TInt tmpNum( 0 );
    
    // NOTE: The identifier may contain NULL UID here, but it will
    // be replaced by the family id in the server side. And then
    // the node proxy will contain the right UID when the created node is
    // internalized during its creation.
    // This will create the supplier or temporary node in server side. So, it can be
    // requested next when the node proxy will be created.
    User::LeaveIfError(
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdCreateTemporaryOrSupplierNode,
                          *nodeIdentifierData,
                          tmpNum,
                          Handle() ) );

    CleanupStack::PopAndDestroy( nodeIdentifierData );

    // The node was created above in the server side if it did not
    // already exist. So, we can be sure that we get the node below.
    // NodeL knows how to create the correct node and inserts it to the cache. 
    // So, use that function here.
    // Notice, that there is no need to use ReplaceCacheNodeL here because the
    // supplier node can use it itself when the correct node data has been loaded
    // from the web.
    node = &NodeL( *tempNodeIdentifier );
    
    CleanupStack::PopAndDestroy( tempNodeIdentifier );

    DLTRACEOUT((""));
    
    return *node;
    }


CNcdNodeProxy& CNcdNodeManagerProxy::ReplaceCacheNodeL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""))
    
    // Remove the node that has the given identifier from the cache.
    // Notice, that the node is now left hanging. And, it will be just deleted
    // when the user releases it. Also, notice that when the node is deleted,
    // CNcdNodeManager::NodeDeleted() will be called but it does not matter because the
    // comparisons in that function are done by using pointers, not identifiers.
    for ( TInt i = 0; i < iNodeCache.Count(); ++i )
        {
        // Because the UID may be set NULL for an identifier and because the
        // UID should always be same for all the nodes in this application,
        // the UID is not included when comparing node identifiers here.
        if ( iNodeCache[ i ]->NodeIdentifier().
                Equals( aNodeIdentifier, ETrue, ETrue, EFalse, EFalse ) ) 
            {
            iNodeCache.Remove( i );
            }
        }        

    DLTRACEOUT((""));

    // This will also insert the new node into the cache.
    // Let the node call leave if something goes wrong. Actually, 
    // at least some node should be found if one version of the node with the
    // given identifier alreay exists in the server side.
    // This call may also leave if we are out of memory. In that case just leave.
    // Do not try to set the removed node back to the array. When memory is released.
    // It can be requested from the server side normally later.
    return NodeL( aNodeIdentifier );
    }


CNcdNodeProxy* CNcdNodeManagerProxy::NodeExists( const CNcdNodeIdentifier& aNodeIdentifier ) const
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iNodeCache.Count(); ++i )
        {
        // Because the UID may be set NULL for an identifier and because the
        // UID should always be same for all the nodes in this application,
        // the UID is not included when comparing node identifiers here.
        if ( iNodeCache[ i ]->NodeIdentifier().
                Equals( aNodeIdentifier, ETrue, ETrue, EFalse, EFalse ) ) 
            {
            // The node has already been created 
            // because the link ids matched.
            // Return the old node.
            DLTRACE(("node found"));
            return iNodeCache[ i ];
            }
        }
    DLTRACE(("node not found"));
    return NULL;
    }


void CNcdNodeManagerProxy::NodeDeleted( CNcdNodeProxy* aNode )
    {
    DLTRACEIN(("Node Deleted"));

    CNcdNodeProxy* root( iRootNode );
    CNcdNodeProxy* searchRoot( iSearchRootNode );
    if( root == aNode )
        {
        DLTRACE(("Deleted root node"));
        // Because the root node is deleted. Set the pointer value to null.
        iRootNode = NULL;        
        }
    else if( searchRoot == aNode )
        {
        DLTRACE(("Deleted search node"));
        // Because the search root node is deleted. Set the pointer value to
        // null.
        iSearchRootNode = NULL;
        }

    // Remove node from the cache because it will be or has been deleted.
    for ( TInt i = 0; i < iNodeCache.Count(); ++i )
        {
        if( aNode == iNodeCache[ i ] )
            {
            DLTRACE(("Removing from cache"));
            // Remove node from the array.
            iNodeCache.Remove( i );
            break;
            }
        }
    }


void CNcdNodeManagerProxy::InternalizeRelatedNodesL( CNcdNodeProxy& aNode ) const
    {
    DLTRACEIN((""));
    
    // First internalize the given node
    aNode.InternalizeL();
    
    // Then check if there is some other nodes to internalize.
    
    // Get the metadata identifier of the given node.
    CNcdNodeIdentifier* paramMetaIdentifier( 
        NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( 
            aNode.NodeIdentifier() ) );
    CNcdNodeIdentifier* metaIdentifier( NULL );
    CNcdNodeProxy* node( NULL );
        
    // Check if related nodes are found from the cache. 
    for ( TInt i = 0; i < iNodeCache.Count(); ++i )
        {
        node = iNodeCache[ i ];
        metaIdentifier = 
            NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( 
                node->NodeIdentifier() );
            
        // Because given node has already been internalized, there is no need to redo it
        // here. But, internalize all related nodes.
        // When identifiers are compared, compare only id and namespace. Omit comparison
        // of URI and UID. UID may be NULL if the node has not been internalized before.
        if ( &aNode != node
             && paramMetaIdentifier->Equals( *metaIdentifier, 
                                             ETrue, ETrue, EFalse, EFalse ) )
            {
            DLINFO(("Related node found. Internalize."));
            node->InternalizeL();
            }
             
        CleanupStack::PopAndDestroy( metaIdentifier );
        metaIdentifier = NULL;
        }

    CleanupStack::PopAndDestroy( paramMetaIdentifier );    
    DLTRACEOUT((""));
    }


CNcdSubscriptionManagerProxy&
    CNcdNodeManagerProxy::SubscriptionManager() const
    {
    return iSubscriptionManager;
    }
    
void CNcdNodeManagerProxy::SetFavoriteManager(
    CNcdFavoriteManagerProxy& aFavoriteManager ) 
    {
    iFavoriteManager = &aFavoriteManager;
    }
    
void CNcdNodeManagerProxy::ClearSearchResultsL()
    {
    DLTRACEIN((""));
    TInt err = KErrNone;
    User::LeaveIfError( ClientServerSession().SendSync(
        NcdNodeFunctionIds::ENcdClearSearchResults,
        KNullDesC,
        err,
        Handle() ) );
    DLTRACEOUT(( "err = %d", err ));
    }


TBool CNcdNodeManagerProxy::IsCapabilitySupportedL( const CNcdNodeIdentifier& aNode, 
    const TDesC& aCapability )
    {
    DLTRACEIN((""));
    CBufBase* sendBuffer = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( sendBuffer );
    RBufWriteStream stream( *sendBuffer );
    CleanupClosePushL( stream );
    
    aNode.ExternalizeL( stream );
    ExternalizeDesL( aCapability, stream );
    
    TPtrC8 sendPtr( sendBuffer->Ptr( 0 ) );
    
    CleanupStack::PopAndDestroy( &stream );
    TBool isCapabilitySupported = EFalse;
    
    User::LeaveIfError( ClientServerSession().SendSync(
        NcdNodeFunctionIds::ENcdIsCapabilitySupported,
        sendPtr,
        isCapabilitySupported,
        Handle() ) );
    
    CleanupStack::PopAndDestroy( sendBuffer );
    return isCapabilitySupported;
    }


void CNcdNodeManagerProxy::HandleExpiredNodesL(
    RPointerArray< CNcdExpiredNode >& aExpiredNodes )
    {
    DLTRACEIN((""));
    RCatalogsArray<MNcdNode> expiredNodeArray;
    CleanupClosePushL( expiredNodeArray );
    for( TInt i = 0 ; i < aExpiredNodes.Count() ; i++ )
        {
        CNcdNodeProxy* node = NodeExists( aExpiredNodes[i]->NodeIdentifier() );
        if( node )
            {
            DLTRACE(("Node existed"));
            // Perhaps errors should not be ignored?
            TRAP_IGNORE( node->InternalizeL() );
            if ( aExpiredNodes[i]->ForceUpdate() )
                {
                DLTRACE(("Force update set, add to array"));
                node->AddRef();
                expiredNodeArray.AppendL( node );
                }
            }
        }
    if( expiredNodeArray.Count() > 0 )
        {
        DLTRACE(("Send force update nodes to provider"));
        iProvider.ExpirationCallback( expiredNodeArray );
        }
    CleanupStack::PopAndDestroy( &expiredNodeArray );
    }


CNcdProviderProxy& CNcdNodeManagerProxy::Provider() const
    {
    return iProvider;
    }

/*
CNcdNodeProxy& CNcdNodeManagerProxy::CreateSchemeNodeL( 
    const CNcdNodeIdentifier& aMetadataIdentifier,
    TInt aFunctionId )
    {
    DLTRACEIN((_L("ID: %S, %S, %d"), 
                &aMetadataIdentifier.NodeId(),
                &aMetadataIdentifier.NodeNameSpace(),
                aMetadataIdentifier.ClientUid()));        

    // Check that the root is not in an unitialized state.
    // Scheme should be created only if the root has been initialized before.
    // Because the root is only used here internally, there is no need to increase
    // the reference count.
    CNcdRootNodeProxy& root( RootNodeL() );
    if ( root.State() == MNcdNode::EStateNotInitialized )
        {
        DLERROR(("Root was not initialized before creation of scheme!"));
        DASSERT( EFalse );
        User::Leave( KErrNotReady );
        }

    CNcdNodeIdentifier* nodeIdentifier =
        NcdNodeIdentifierEditor::CreateNodeIdentifierLC(
            RootNodeL().NodeIdentifier(),
            aMetadataIdentifier );

    // Check if the node proxy already exists in the cache.
    // Every node has its own namespace and dataid.
    // If it does exist, do not create it.
        
    CNcdNodeProxy* node( NodeExists( *nodeIdentifier ) );
    if ( node != NULL )
        {
        // Node was found. So, return it.
        CleanupStack::PopAndDestroy( nodeIdentifier );
        DLTRACEOUT(("Node found"));
        return *node;
        }

    HBufC8* nodeIdentifierData( nodeIdentifier->NodeIdentifierDataL() );    
    CleanupStack::PushL( nodeIdentifierData );
        
    // NOTE: The identifier may contain NULL UID here, but it will
    // be replaced by the family id in the server side. And then
    // the node proxy will contain the right UID when the created node is
    // internalized during its creation.

    // tmpNum will not be used for anything. It is just given as a dummy parameter for
    // the session request.
    TInt tmpNum( 0 );
    User::LeaveIfError(
            ClientServerSession().
                SendSync( aFunctionId,
                          *nodeIdentifierData,
                          tmpNum,
                          Handle() ) );

    CleanupStack::PopAndDestroy( nodeIdentifierData );

    // The node was created above in the server side if it did not
    // already exist. So, we can be sure that we get the node below.
    // NodeL knows how to create the correct node and inserts it to the cache. 
    // So, use that function here.
    node = &NodeL( *nodeIdentifier );

    CleanupStack::PopAndDestroy( nodeIdentifier );
    
    // Update root node by calling internalization because scheme nodes will
    // be added to the root node also.
    RootNodeL().InternalizeL();

    DLTRACEOUT((""));
    
    return *node;
    }
*/

 CNcdNodeProxy& CNcdNodeManagerProxy::CreateNodeL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));

    CNcdNodeProxy* node( NULL );

    HBufC8* nodeIdentifierData( aNodeIdentifier.NodeIdentifierDataL() );
    CleanupStack::PushL( nodeIdentifierData );
        
    DLINFO(("Get node handle"));
    // First get the handle to the node of the server side.
    // The handle is gotten from the server side nodemanager.
    
    // NOTE: The identifier may contain NULL UID here, but it will
    // be replaced by the family id in the server side. And then
    // the node proxy will contain the right UID when the created node is
    // internalized during its creation.
    
    TInt nodeHandle( 0 );
    User::LeaveIfError(
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdNodeHandle,
                          *nodeIdentifierData,
                          nodeHandle,
                          Handle() ) );

    DLTRACE(("Node handle: %d", nodeHandle ));

    CleanupStack::PopAndDestroy( nodeIdentifierData );


    // Next, check the type of the node. So, we can create
    // right type of the proxy.
    TInt classId( 0 );    
    TInt classIdError = 
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdClassId,
                          KNullDesC,
                          classId,
                          nodeHandle );
    if ( classIdError != KErrNone )
        {
        // Because we can not create the proxy object for some reason,
        // release the handle.
        TInt tmpNum( 0 );
        ClientServerSession().
            SendSync( NcdNodeFunctionIds::ENcdRelease,
                      KNullDesC,
                      tmpNum,
                      Handle() );
        
        // For debuggin reasons.
        DLERROR(("Class id error"));
        DASSERT( EFalse );
        
        User::Leave( classIdError );
        }

    // Create the node according to the class id
    // Notice that if the proxy object leaves during the construction
    // the destructor automatically releases the handle from the server side.               
    switch( classId )
        {
        case NcdNodeClassIds::ENcdFolderNodeClassId:
            node = 
                CNcdNodeFolderProxy::NewLC( ClientServerSession(), 
                                            nodeHandle,
                                            *this,
                                            iOperationManager,
                                            *iFavoriteManager );
            break;

        case NcdNodeClassIds::ENcdTransparentFolderNodeClassId:
            // The transparent folder is handled in the proxy
            // side as a normal folder.
            node = 
                CNcdNodeFolderProxy::NewLC( ClientServerSession(), 
                                            nodeHandle,
                                            *this,
                                            iOperationManager,
                                            *iFavoriteManager );
            break;
            
        case NcdNodeClassIds::ENcdBundleFolderNodeClassId:
            node =
                CNcdBundleFolderProxy::NewLC( ClientServerSession(),
                                              nodeHandle,
                                              *this,
                                              iOperationManager,
                                              *iFavoriteManager );
            break;
        
        case NcdNodeClassIds::ENcdSearchFolderNodeClassId:
            node = 
                CNcdSearchNodeFolderProxy::NewLC( ClientServerSession(), 
                                            nodeHandle,
                                            *this,
                                            iOperationManager,
                                            *iFavoriteManager );
            break;

        case NcdNodeClassIds::ENcdItemNodeClassId:
            node = 
                CNcdNodeItemProxy::NewLC( ClientServerSession(),
                                          nodeHandle,
                                          *this,
                                          iOperationManager,
                                          *iFavoriteManager );
            break;
        
        case NcdNodeClassIds::ENcdSearchItemNodeClassId:
            node = 
                CNcdSearchNodeItemProxy::NewLC( ClientServerSession(),
                                          nodeHandle,
                                          *this,
                                          iOperationManager,
                                          *iFavoriteManager );
            break;
            
        case NcdNodeClassIds::ENcdSearchRootNodeClassId:
            node = 
                CNcdSearchRootNodeProxy::NewLC( ClientServerSession(),
                                          nodeHandle,
                                          *this,
                                          iOperationManager,
                                          *iFavoriteManager );
            break;
            

        case NcdNodeClassIds::ENcdSupplierNodeClassId:
            {
            node = CNcdNodeSupplierProxy::NewLC( ClientServerSession(),
                                                 nodeHandle,
                                                 *this,
                                                 iOperationManager,
                                                 *iFavoriteManager );

            break;
            }
            
        case NcdNodeClassIds::ENcdSearchBundleNodeClassId:
            {
            node = 
                CNcdSearchNodeBundleProxy::NewLC( ClientServerSession(), 
                                            nodeHandle,
                                            *this,
                                            iOperationManager,
                                            *iFavoriteManager );
            break;
            }            
            
        default:
            // classId is not recognized
            // So, node is left NULL
            DLERROR(("Node class type was not recognized!"));
            // For testing purposes assert here
            DASSERT( EFalse );
            break;
        }

    // Because we creted a new node, it should be added to the cache.
    iNodeCache.AppendL( node );

    CleanupStack::Pop( node );
    DLTRACEOUT((""));
    return *node;
    }
