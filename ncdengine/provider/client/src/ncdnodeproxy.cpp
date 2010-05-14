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
* Description:   Contains CNcdNodeProxy class implementation
*
*/


#include "ncdnodeproxy.h"
#include "ncdrootnodeproxy.h"
#include "ncdnodecontainer.h"
#include "ncdnodefolderproxy.h"
#include "ncdnodemetadataproxy.h"
#include "ncdnodeuserdataproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdloadnodeoperationproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsdebug.h"
#include "catalogsutils.h"
#include "ncdnodemanagerproxy.h"
#include "ncdfavoritemanagerproxy.h"
#include "ncdquery.h"
#include "ncdnodedisclaimerproxy.h"
#include "ncdnodefavorite.h"
#include "ncdnodeidentifiereditor.h"
#include "ncdnodeseenproxy.h"


CNcdNodeProxy::CNcdNodeProxy( MCatalogsClientServer& aSession,
                              TInt aHandle, 
                              CNcdNodeManagerProxy& aNodeManager,
                              CNcdOperationManagerProxy& aOperationManager,
                              CNcdFavoriteManagerProxy& aFavoriteManager ) 
: CNcdInterfaceBaseProxy( aSession, aHandle, NULL ),
  iNodeManager( aNodeManager ),
  iOperationManager( aOperationManager ),
  iFavoriteManager( aFavoriteManager )
    {
    }


void CNcdNodeProxy::ConstructL( )
    {
    DLTRACEIN(("this-ptr as MNcdNode: %X", static_cast<MNcdNode*>( this )));

    // Register the interfaces of this object
    MNcdNode* node( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( node, this, MNcdNode::KInterfaceUid ) );

    // Make sure that at least some values are inserted to the link descriptors.
    // So, the references returned from functions of this class will contain at
    // least something.
    iTimestamp = KNullDesC().AllocL();
    iCatalogSourceName = KNullDesC().AllocL(); 
    iExpiredTime = 0;

    // Get the data from the server side.
    // It may be possible that actually not much is received
    // because the node may be in uninitialized state.
    // But at least the namespace and id information should be gotten.
    TRAPD( trapError, InternalizeL() );    

    // This node can not exist if the identifier data is not set.
    // So, if node identifier is not set this should leave and
    // prevent the creation of the node.
    if ( iNodeIdentifier == NULL )
        {
        User::LeaveIfError( trapError );
        }
    
    // Also make sure that parent identifier is set at least to empty value
    if ( iParentIdentifier == NULL )
        {
        // If we have to set the parent identifier here, it means that
        // the node link was not set yet. 
        // If node link was set, then the parent id was also set to correct value. 
        // And, then this value should not be set here.
        iParentIdentifier = CNcdNodeIdentifier::NewL();        
        }
        
    if ( iRealParentIdentifier == NULL ) 
        {
        iRealParentIdentifier = CNcdNodeIdentifier::NewL();
        }

    DLTRACEOUT((""));
    }


CNcdNodeProxy::~CNcdNodeProxy()
    {
    DLTRACEIN(("this-ptr: %x", this));
    // Because this object is deleted. Remove the interfaces from the
    // interface list.
    RemoveInterface( MNcdNode::KInterfaceUid );
    RemoveInterface( MNcdNodeChildOfTransparent::KInterfaceUid );
    RemoveInterface( MNcdNodeFavorite::KInterfaceUid );
    
    // Inform node manager that this node is under deletion.    
    iNodeManager.NodeDeleted( this );

    delete iNodeIdentifier;
    iNodeIdentifier = NULL;

    // Delete link data

    delete iCatalogSourceName;
    iCatalogSourceName = NULL;

    delete iParentIdentifier;
    iParentIdentifier = NULL;
    
    delete iRealParentIdentifier;
    iRealParentIdentifier = NULL;

    delete iTimestamp;
    iTimestamp = NULL;


    // Delete the objects that are owned by this class and provide functionality
    // through the api.
    // Notice that the api-objects are Released (not deleted) from the UI side.
    // The node owns the data and the reference counter of the node keeps
    // track of the api objects it owns. When the reference counter of the node
    // reaches zero, it means that nobody is using the node or the classes owned
    // by the node. If somebody is using the object owned by the node, the reference
    // counter can not be zero until everything is released.
    // Thus, the node may delete the data here.

    delete iMetadata;
    iMetadata = NULL;
    
    delete iNodeSeen;
    iNodeSeen = NULL;
    DLTRACEOUT(("this-ptr: %x", this));
    }



CNcdNodeIdentifier& CNcdNodeProxy::NodeIdentifier() const
    {
    return *iNodeIdentifier;    
    }


CNcdNodeManagerProxy& CNcdNodeProxy::NodeManager() const
    {
    return iNodeManager;
    }


CNcdOperationManagerProxy& CNcdNodeProxy::OperationManager() const
    {
    return iOperationManager;    
    }


CNcdNodeMetadataProxy* CNcdNodeProxy::Metadata() const
    {
    //DASSERT( iMetadata );
    return iMetadata;
    }


void CNcdNodeProxy::InternalizeL()
    {
    DLTRACEIN(("this-ptr: %x", this));
    DPROFILING_BEGIN( x );
    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data id, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    User::LeaveIfError(
            ClientServerSession().
                SendSyncAlloc( NcdNodeFunctionIds::ENcdInternalize,
                               KNullDesC8,
                               data,
                               Handle(),
                               0 ) );

    if ( data == NULL )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PushL( data );

    DLINFO(( "Ncd node internalize data length: %d", data->Length() ));

    // Read the data from the stream and insert it to the memeber variables
    RDesReadStream stream( *data );
    CleanupClosePushL( stream );
    
    // Let this leave. If the node can not internalize itself, then
    // something is really badly wrong. In this case this node should
    // even be created. So, if this is called from ContstructL it should
    // most likely to leave also.
    InternalizeNodeDataL( stream );

    // Closes the stream
    CleanupStack::PopAndDestroy( &stream ); 
    CleanupStack::PopAndDestroy( data );

        
    // The node data was internalized.
        
    // Next, internalize the link data if it exists.    
    DLINFO(("Internalize node proxy link"));
    InternalizeLinkL();    


    // TRAP the functions after this. So, all the existing new interfaces
    // may be added to the interface list.

    // Also, metadata may be ready for initialization.
    DLINFO(("Internalize node proxy metadata"));
    TRAP_IGNORE( InternalizeMetadataL() );
    
    // Internalize node seen proxy.
    DLINFO(("Internalize node seen"));
    InternalizeNodeSeenL();
    
    // Install the MNcdNodeFavorite interface if the same metadata is favorite already.
    if ( iFavoriteManager.IsFavoriteL( *iNodeIdentifier ) ) 
        {
        MNcdNodeFavorite* node( this );
        AddInterfaceL( 
            CCatalogsInterfaceIdentifier::NewL( node, this, MNcdNodeFavorite::KInterfaceUid ) );
        }
    DPROFILING_END( x );
    DLTRACEOUT((""));
    }
    
    
void CNcdNodeProxy::AddToFavoritesL( TBool aRemoveOnDisconnect ) 
    {
    DLTRACEIN(("aRemoveOnDisconnect: %d", aRemoveOnDisconnect));
    if ( !iFavoriteManager.IsFavoriteL( *iNodeIdentifier ) ) 
        {
        iFavoriteManager.AddToFavoritesL( *iNodeIdentifier, aRemoveOnDisconnect );        
        }

    // Register the MNcdNodeFavorite interface.
    MNcdNodeFavorite* node( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( node, this, MNcdNodeFavorite::KInterfaceUid ) );        
    }
    


// MNcdNode functions


MNcdNode::TState CNcdNodeProxy::State() const
    {
    DLTRACEIN((_L("Node namespace=%S, id=%S"), &Namespace(), &Id() ));

    // Check if the link handle has been set, which means that also
    // link data has been internalized. Also, check if the metadata 
    // exists, which means that metadata has also been internalized.
    if ( LinkHandleSet() 
         && iMetadata != NULL )
        {
        DLINFO(("State was initialized"));
        // If state was initialized we have to check if the state
        // has acutally expired already 
        TTime now;
        now.HomeTime();

        DLINFO(("now time: %d", now.Int64() ));
        DLINFO(("expired time: %d", ExpiredTime().Int64() ));

        // We can just compare the times here. Server side
        // inserts the maximum value for the expired time if the
        // protocol has set never expire value for the validity delta.
        if ( now > ExpiredTime() )
            {
            DLTRACEOUT(("Expired"));
            return MNcdNode::EStateExpired;
            }
            
        DLTRACEOUT(("Initialized"));
        return MNcdNode::EStateInitialized;
        }
     else
        {
        // Node has not been initialized.
        DLTRACEOUT(("Not initialized"));
        return MNcdNode::EStateNotInitialized;
        }
    }


const TDesC& CNcdNodeProxy::Id() const
    {
    DLTRACEIN((""));
    return iNodeIdentifier->NodeId(); 
    }

const TDesC& CNcdNodeProxy::Namespace() const
    {
    DLTRACEIN((""));
    return iNodeIdentifier->NodeNameSpace();
    }

const TDesC& CNcdNodeProxy::CatalogSourceName() const
    {
    DLTRACEIN((""));
    return *iCatalogSourceName;
    }    

MNcdNodeContainer* CNcdNodeProxy::ParentL() const
    {
    DLTRACEIN((""));

    if( iParentIdentifier == NULL
        || iParentIdentifier->ContainsEmptyFields() )
        {
        DLTRACEOUT(("Parent identifier was not set. Return NULL"));
        return NULL;
        }
    else
        {
        DLINFO(("Parent identifier was set. Get the parent"));

        // This always gives reference to the node
        // We use pointer instead of reference afterwards.
        // NodeL leaves if the node was not found.
        MNcdNode* parent = &iNodeManager.NodeL( *iParentIdentifier );

        DLINFO(("Parent was gotten"));
        
        // This should always give something because parent is always container.
        // Notice, that the query interface increases the reference count automatically.
        MNcdNodeContainer* container = parent->QueryInterfaceL<MNcdNodeContainer>();

        DLTRACEOUT(( _L("Return parent container interface: %X"), container ));
        
        return container;
        }
    }

MNcdLoadNodeOperation* CNcdNodeProxy::LoadL( MNcdLoadNodeOperationObserver& aObserver )
    {
    DLTRACEIN((""));

    CNcdLoadNodeOperationProxy* operation( NULL );

    operation =
        iOperationManager.CreateLoadNodeOperationL( *this );

    operation->AddObserverL( this );
    operation->AddObserverL( &aObserver );

    DLTRACEOUT((""));

    return operation;
    }

RCatalogsArray<MNcdOperation> CNcdNodeProxy::OperationsL() const
    {
    DLTRACEIN(("this: %x, iMetadata: %x", this, iMetadata ));
    RCatalogsArray<MNcdOperation> operations;
    CleanupClosePushL( operations );

    if ( iMetadata ) 
        {
        // Get the original array and insert its content to catalogs array.
        // Also, increase the reference counter for the items.
        const RPointerArray<MNcdOperation>& origArray = iOperationManager.Operations();
        MNcdOperation* oper( NULL );
        MNcdNode* node( NULL );
        
        CNcdNodeMetadataProxy* metadata( NULL );
        DLTRACE(("Getting metaidentifier"));
        const CNcdNodeIdentifier& metadataId( iMetadata->Identifier() );
        
        DLTRACE(("Origarray.count: %d", origArray.Count() ));
        for ( TInt i = 0; i < origArray.Count(); ++i )
            {            
            oper = origArray[ i ];
            DLTRACE(("oper: %x", oper));
            // Notice that node ref count is increased. So, release it when done.
            node = oper->Node();
            if ( node ) 
                {
                CleanupReleasePushL( *node );
    
                // Compare metadatas
                metadata = static_cast<CNcdNodeProxy*>( node )->Metadata();
                DLTRACE(("Metadata: %x", metadata));
                if ( metadata && metadata->Identifier().Equals( metadataId ) )
                    {
                    DLTRACE(("Appending to ops array"));
                    operations.AppendL( oper );
                    oper->AddRef();
                    }
    
                CleanupStack::PopAndDestroy( node );
                }
            }
        }

    CleanupStack::Pop( &operations );
    
    DLTRACEOUT(( "" ));
    return operations;
    }
    

void CNcdNodeProxy::AddToFavoritesL() 
    {
    AddToFavoritesL( EFalse );
    }
    

// MNcdLoadNodeOperationObserver


void CNcdNodeProxy::NodesUpdated( MNcdLoadNodeOperation& /*aOperation*/,
                                  RCatalogsArray< MNcdNode >& aNodes )
    {
    
    for ( TInt i = 0 ; i < aNodes.Count() ; i++ )    
        {
        // Should the node provide its own function instead of API interface
        // that can be called when node/nodes should be updated. Operations know
        // the node proxy object so they could use the function directly instead
        // of using more restricted API interfaces. Then, no casting would be required
        // here...        
        CNcdNodeProxy* node = static_cast<CNcdNodeProxy*>(aNodes[i]);
        TRAP_IGNORE( node->InternalizeL() );
        /*// make sure that this node is a child of this node
        TIdentityRelation<CNcdNodeIdentifier> relation(CNcdNodeIdentifier::Equals);        
        if ( iChildren.Find( node->NodeIdentifier(), relation ) == KErrNotFound )
            {
            
            }*/
        }    
    
    // This function of the observer interface does not need to do anythin here.
    }
    
void CNcdNodeProxy::QueryReceived( MNcdLoadNodeOperation& /*aOperation*/,
                                   MNcdQuery* aQuery )
    {
    // This function of the observer interface does not need to do anythin here.
    aQuery->Release();
    }

void CNcdNodeProxy::OperationComplete( MNcdLoadNodeOperation& /*aOperation*/,
                                       TInt /*aError*/ )
    {
    DLTRACEIN((""));
    DLINFO(("Nothing done."));
    DLTRACEOUT((""));
    }

    
// MNcdNodeChildOfTransparent    

MNcdNode* CNcdNodeProxy::TransparentParentL() const 
    {
    DLTRACEIN((""));
    DASSERT( iRealParentIdentifier );
    DASSERT( !iRealParentIdentifier->ContainsEmptyFields() );
    
    MNcdNode* parent( NULL );
    TRAPD( err, parent = &NodeManager().NodeL( *iRealParentIdentifier ) );    

    if ( err == KErrNotFound ) 
        {
        return NULL;
        }
        
    User::LeaveIfError( err );
    
    parent->AddRef();
    return parent;
    }


// MNcdNodeFavorite

void CNcdNodeProxy::RemoveFromFavoritesL() 
    {
    DLTRACEIN((""));
    iFavoriteManager.RemoveFromFavoritesL( *iNodeIdentifier );
    RemoveInterface( MNcdNodeFavorite::KInterfaceUid );
    }
    
    
void CNcdNodeProxy::SetDisclaimerL( MNcdNode* aDisclaimerOwner ) 
    {
    DLTRACEIN((""));
    iFavoriteManager.SetNodeDisclaimerL( *iNodeIdentifier, aDisclaimerOwner );
    }
    

MNcdQuery* CNcdNodeProxy::DisclaimerL() const 
    {
    DLTRACEIN((""));
    MNcdQuery* disclaimer = iFavoriteManager.NodeDisclaimerL( *iNodeIdentifier );
    if ( disclaimer ) 
        {
        disclaimer->AddRef();
        }
    return disclaimer;
    }


// Other functions

TBool CNcdNodeProxy::LinkHandleSet() const
    {
    return iLinkHandleSet;
    }
    
TInt CNcdNodeProxy::LinkHandleL() const
    {
    if( !iLinkHandleSet )
        {
        // Link handle has not been set
        User::Leave( KErrNotReady );
        }
        
    return iLinkHandle;
    }
    
void CNcdNodeProxy::SetLinkHandle( TInt aHandle )
    {
    iLinkHandle = aHandle;
    iLinkHandleSet = ETrue;
    }

TTime CNcdNodeProxy::ExpiredTime() const
    {
    return iExpiredTime;
    }
    
CNcdNodeIdentifier* CNcdNodeProxy::ParentIdentifier() const
    {
    return iParentIdentifier;
    }

TBool CNcdNodeProxy::IsRemote() const
    {
    return iRemoteFlag;
    }    


// ======== FUNCTIONS FOR NODE INTERNALIZATION ========


void CNcdNodeProxy::InternalizeLinkL()
    {
    DLTRACEIN((""));

    // Get the handle for the link data
    if ( !LinkHandleSet() )
        {
        DLINFO(("Link handle was not set. Set it now."));
        TInt linkHandle( 0 );
        
        // Leave for example by using KErrNotFound if the link did not exist.
        User::LeaveIfError(
                ClientServerSession().
                    SendSync( NcdNodeFunctionIds::ENcdLinkHandle,
                              KNullDesC,
                              linkHandle,
                              Handle() ) );    
        
        // Link existed. So, we got the handle.                      
        DLINFO(("Handle: %i", linkHandle));
        SetLinkHandle( linkHandle );        
        }

    HBufC8* data( NULL );
    // Because we do not know the exact size of the data id, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    User::LeaveIfError(
            ClientServerSession().
                SendSyncAlloc( NcdNodeFunctionIds::ENcdInternalize,
                               KNullDesC8,
                               data,
                               LinkHandleL(),
                               0 ) );

    if ( data == NULL )
        {
        DLINFO(("Received link data was NULL"));
        User::Leave( KErrNotFound );
        }

    CleanupStack::PushL( data );

    DLINFO(( "Received link data length: %d", data->Length() ));

    // Read the data from the stream and insert it to the memeber variables
    RDesReadStream stream( *data );
    CleanupClosePushL( stream );
    
    InternalizeNodeLinkDataL( stream );

    // Closes the stream
    CleanupStack::PopAndDestroy( &stream ); 
    CleanupStack::PopAndDestroy( data );

    DLTRACEOUT((""));
    }


void CNcdNodeProxy::InternalizeNodeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // Read the class id of the node
    // (not actually needed here at the moment but may be used to check if the
    // data is of the right type...)
    aStream.ReadInt32L();

    // No need to put the newIdentifier into the cleanup stack because this
    // function will not leave before inserting the value to the member variable.
    CNcdNodeIdentifier* newIdentifier = CNcdNodeIdentifier::NewL( aStream );
    delete iNodeIdentifier;
    iNodeIdentifier = newIdentifier;
    
    DLTRACEOUT((""));
    }

void CNcdNodeProxy::InternalizeNodeLinkDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    HBufC* tmpTimeStamp( NULL );
    HBufC* tmpCatalogSourceName( NULL );
    TBool tmpRemoteFlag( EFalse );
    TInt64 tmpExpiredTime( 0 );
    CNcdNodeIdentifier* tmpParentIdentifier( NULL );
    CNcdNodeIdentifier* tmpRealParentIdentifier( NULL );
        
    // Read the class id of the link
    TInt tmpInt = aStream.ReadInt32L();
    DLINFO(("Classid: %d", tmpInt));
      
    if ( tmpInt != NcdNodeClassIds::ENcdNullObjectClassId )
        {

        InternalizeDesL( tmpTimeStamp, aStream );
        CleanupStack::PushL( tmpTimeStamp );
        DLINFO(( _L("timestamp: %S"), tmpTimeStamp ));
        
        InternalizeDesL( tmpCatalogSourceName, aStream );
        CleanupStack::PushL( tmpCatalogSourceName );
        DLINFO(( _L("catalogsource: %S"), tmpCatalogSourceName ));

        tmpRemoteFlag = aStream.ReadInt32L();
        DLINFO((_L("remote flag: %d"), tmpRemoteFlag));
        
        aStream >> tmpExpiredTime;        
        DLINFO(("expired time: %d", tmpExpiredTime ));

        tmpParentIdentifier = CNcdNodeIdentifier::NewLC( aStream );
        tmpRealParentIdentifier = CNcdNodeIdentifier::NewLC( aStream );
        }
    else
        {
        tmpTimeStamp = KNullDesC().AllocLC();
        tmpCatalogSourceName = KNullDesC().AllocLC();
        }


    // Now we can be sure that this function will not leave.
    // So it is safe to set the values for the member variables.
    delete iRealParentIdentifier;
    iRealParentIdentifier = tmpRealParentIdentifier;
    if ( tmpRealParentIdentifier ) 
        {
        CleanupStack::Pop( tmpRealParentIdentifier );
        }

    delete iParentIdentifier;
    iParentIdentifier = tmpParentIdentifier;
    if( tmpParentIdentifier )
        {
        CleanupStack::Pop( tmpParentIdentifier );
        if ( !iRealParentIdentifier->Equals( *iParentIdentifier ) ) 
            {
            // Must be child of transparent node since real parent identifier is
            // different. Register the MNcdNodeChildOfTransparent interface.
            DLINFO(("child of transparent"));
            MNcdNodeChildOfTransparent* interface( this );
            AddInterfaceL(
                CCatalogsInterfaceIdentifier::NewL(
                    interface, this, MNcdNodeChildOfTransparent::KInterfaceUid ) );
            }
        }        

    if ( !iRealParentIdentifier || iRealParentIdentifier->Equals( *iParentIdentifier ) ) 
        {
        // Real parent in engine is same as in proxy -> parent is not transparent ->
        // remove the MNcdNodeChildOfTransparent interface.
        RemoveInterface( MNcdNodeChildOfTransparent::KInterfaceUid );
        }
            
    iExpiredTime = tmpExpiredTime;

    iRemoteFlag = tmpRemoteFlag;

    delete iCatalogSourceName;
    iCatalogSourceName = tmpCatalogSourceName;
    CleanupStack::Pop( tmpCatalogSourceName );

    delete iTimestamp;
    iTimestamp = tmpTimeStamp;
    CleanupStack::Pop( tmpTimeStamp );

    DLTRACEOUT((""));    
    }


void CNcdNodeProxy::InternalizeMetadataL()
    {
    DLTRACEIN((""));
    
    if ( iMetadata == NULL )
        {
        DLINFO(("Metadata did not exist"));

        // Check if the metadata exists in the server side and update the information
        TInt metadataHandle( 0 );
            
        TInt handleError =
            ClientServerSession().
                SendSync( NcdNodeFunctionIds::ENcdMetadataHandle,
                          KNullDesC,
                          metadataHandle,
                          Handle() );

        if ( handleError == KErrNotFound )
            {
            DLINFO(("Metadata did not exist in server side"));
            
            // Because metadata did not exist in the server side
            // Nothing to do here.
            return;
            }
        else
            {
            // If error occurred then leave
            User::LeaveIfError( handleError );    
            }

        DLINFO(("Metadata existed"));
                    
        // Now we can create the metadata object
        // Notice that this object will add its own interfaces to the
        // querylist when the object is created.
        // Also, notice that if the proxy object leaves during the construction
        // the destructor automatically releases the handle from the server side.
        iMetadata = CNcdNodeMetadataProxy::NewL( ClientServerSession(), 
                                                 metadataHandle,
                                                 *this );
        // Metadata ref counter is not increased here.
        // So, it will be zero until somebody asks for the metadata.
        // If the Release is called and
        // when the reference counter reaches zero, this node which
        // acts as parent will be deleted. The destructor of this
        // node will delete the metadata because this node owns it.
        }
    else
        {
        // Notice that that if the proxy object was created above,
        // it internalizes itself during construction. So, call
        // the internalize here only if the object was not constructed
        // above.
        // Proxy object existed.
        // So, internalize it with new data.
        iMetadata->InternalizeL();        
        }

    DLTRACEOUT((""));
    }
    

void CNcdNodeProxy::InternalizeNodeSeenL() 
    {
    DLTRACEIN((""));
    if ( iNodeSeen ) 
        {
        // The proxy can internalize its state itself.
        iNodeSeen->InternalizeL();
        }
    else 
        {
        // The proxy object not created yet, create it.
        DLINFO(("Node seen did not exist"));
        TInt nodeSeenHandle( 0 );
        
        User::LeaveIfError(
            ClientServerSession().SendSync(
                NcdNodeFunctionIds::ENcdNodeSeenHandle,
                KNullDesC,
                nodeSeenHandle,
                Handle() ) );

        // Got the handle, create the proxy object. The proxy object internalizes itself
        // in construction.
        iNodeSeen = CNcdNodeSeenProxy::NewL(
            ClientServerSession(), nodeSeenHandle, *this );                    
        }
    }
