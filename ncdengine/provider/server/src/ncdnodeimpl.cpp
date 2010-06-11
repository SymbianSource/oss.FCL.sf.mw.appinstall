/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements CNcdNode class
*
*/


#include <e32err.h>
#include <e32base.h>

#include "ncdnodeimpl.h"
#include "ncdnodemanager.h"
#include "ncdnodelink.h"
#include "ncdnodeitemlink.h"
#include "ncdnodefolderlink.h"
#include "ncdnodemetadataimpl.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeidentifier.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodeuserdataimpl.h"
#include "ncdutils.h"
#include "catalogsconstants.h"
#include "catalogsdebug.h"
#include "catalogsutils.h"
#include "ncdnodeseenimpl.h"


CNcdNode::CNcdNode(
    CNcdNodeManager& aNodeManager,
    NcdNodeClassIds::TNcdNodeClassId aNodeClassId, 
    NcdNodeClassIds::TNcdNodeClassId aAcceptedLinkClassId,
    NcdNodeClassIds::TNcdNodeClassId aAcceptedMetaDataClassId )
: CCatalogsCommunicable(), 
  iNodeManager( aNodeManager ),
  iNodeClassId( aNodeClassId ),
  iAcceptedLinkClassId( aAcceptedLinkClassId ),
  iAcceptedMetaDataClassId( aAcceptedMetaDataClassId )
    {
    }


void CNcdNode::ConstructL( const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN((_L("ns: %S, id: %S"), &aIdentifier.NodeNameSpace(), &aIdentifier.NodeId()));
    // Set the identifier for this node.
    iNodeIdentifier =
        CNcdNodeIdentifier::NewL( aIdentifier );
    iNodeSeen = CNcdNodeSeen::NewL( iNodeManager.SeenInfo(), *this );
    }


CNcdNode::~CNcdNode()
    {
    DLTRACEIN((""));
    delete iNodeIdentifier;
    
    // Notice that CCatalogsCommunicable object should not be deleted but
    // only closed.
    
    if ( iNodeLink ) 
        {
        DLINFO(("Closing node link"));
        iNodeLink->Close();
        }
        
    if ( iNodeSeen )
        {
        DLINFO(("Closing node seen"));
        iNodeSeen->Close();
        }
    
    // Do not delete iNodeMetaData here.
    // It is managers responsibility
    // Also, do not delete the manager because it is providers job.

    DLTRACEOUT((""));
    }


const CNcdNodeIdentifier& CNcdNode::Identifier() const
    {
    DASSERT( iNodeIdentifier );
    return *iNodeIdentifier;
    }


NcdNodeClassIds::TNcdNodeClassId CNcdNode::ClassId() const
    {
    return iNodeClassId;
    }


CNcdNodeManager& CNcdNode::NodeManager() const
    {
    return iNodeManager;
    }


CNcdNodeLink& CNcdNode::CreateAndSetLinkL()
    {
    iNodeLink = CreateLinkL();
    if ( iNodeMetaData ) 
        {
        // Ensure that link has the correct metadata timestamp
        iNodeLink->SetMetadataTimeStampL( iNodeMetaData->TimeStamp() );
        }
    return *iNodeLink;
    }


CNcdNodeLink& CNcdNode::NodeLinkL() const
    {
    if( iNodeLink == NULL )
        {
        User::Leave( KErrNotFound );
        }         
       
    return *iNodeLink;
    }


CNcdNodeLink* CNcdNode::NodeLink() const
    {       
    return iNodeLink;
    }
    

CNcdNodeSeen& CNcdNode::NodeSeen() const
    {
    DASSERT( iNodeSeen );
    return *iNodeSeen;
    }


void CNcdNode::InternalizeLinkL( const MNcdPreminetProtocolEntityRef& aData,
                                 const CNcdNodeIdentifier& aParentIdentifier,
                                 const CNcdNodeIdentifier& aRequestParentIdentifier,
                                 const TUid& aClientUid )
    {
    DLTRACEIN((""));
    
    TBool linkCreated( EFalse );
    
    if ( iNodeLink == NULL )
        {
        DLINFO(("Create link"));
        
        // Because the node link did not exist create it.
        // Create link should be implemented in child classes. So the right
        // kind of link is set here.
        iNodeLink = CreateLinkL();
        linkCreated = ETrue;
        }
        
    DLINFO(("Internalize link"));

    TRAPD( err, iNodeLink->InternalizeL( 
        aData, aParentIdentifier, aRequestParentIdentifier, aClientUid ) );
    
    if( err != KErrNone )
        {
        DLINFO(("Internalize link error: %d", err));

        if( linkCreated )
            {
            // Because the link was created but the internalization went wrong,
            // delete the link because it does not contain correct data.
            iNodeLink->Close();
            iNodeLink = NULL;
            }
        User::Leave( err );
        }

    DLTRACEOUT(( _L("Link internalized for node: %S, %S"), 
                   &iNodeIdentifier->NodeNameSpace(),
                   &iNodeIdentifier->NodeId()));
    }


CNcdNodeMetaData& CNcdNode::NodeMetaDataL() const
    {
    if( iNodeMetaData == NULL )
        {
        User::Leave( KErrNotFound );
        }        
    
    return *iNodeMetaData;
    }
  
    
CNcdNodeMetaData* CNcdNode::NodeMetaData() const
    {    
    return iNodeMetaData;
    }


void CNcdNode::SetNodeMetaDataL( CNcdNodeMetaData& aMetaData )
    {
    DLTRACEIN((("this-ptr: %x"), this));

    if( iAcceptedMetaDataClassId == aMetaData.ClassId() )
        {
        // The metadata is of the right type, so it is safe to 
        // insert as member variable.
        // Do not delete metadata because manager owns it.
        iNodeMetaData = &aMetaData;
        
        // Temp nodes don't necessarily have a link set at this point   
        if ( iNodeLink ) 
            {            
            iNodeLink->SetMetadataTimeStampL( iNodeMetaData->TimeStamp() );
            }
        DLINFO(("Metadata inserted"));
        }
    else
        {
        DLINFO(( "Metadata was of the wrong type: %d, %d", 
                 iAcceptedMetaDataClassId, aMetaData.ClassId() ));
        DASSERT( EFalse );
        // Because the metadata was of the wrong type we do not set
        // metadata for this object
        User::Leave( KErrArgument );
        }
        
    DLTRACEOUT(( _L("MetaData set for: %S, %S"), 
                    &iNodeIdentifier->NodeNameSpace(),
                    &iNodeIdentifier->NodeId() ));
    }
    

MNcdNode::TState CNcdNode::State() const
    {
    DLTRACEIN((""));
    
    CNcdNodeLink* link = NodeLink();    
    if ( link == NULL )
        {
        DLTRACEOUT(("Link NULL"));
        
        // Because link did not exist. The node is not initialized
        return MNcdNode::EStateNotInitialized;
        }

    CNcdNodeMetaData* meta = NodeMetaData();    
    if ( meta == NULL )
        {
        DLTRACEOUT(("Meta NULL"));

        // Because metadata did not exist. 
        // Then the state can not be initialized.
        return MNcdNode::EStateNotInitialized;
        }

    // Because link and meta both existed the node has required data
    // but check if the node is expired.
    if ( link->IsExpired() )
        {
        DLTRACEOUT(("Link was expired"));
        
        return MNcdNode::EStateExpired;
        }
    
    DLTRACEOUT(("State was initalized"));
    
    // Everything was set. So, the node state is initialized
    return MNcdNode::EStateInitialized;
    }


void CNcdNode::ReceiveMessage( MCatalogsBaseMessage* aMessage,
                               TInt aFunctionNumber )
    {
    DLTRACEIN(("handle: %d, function: %d", aMessage->Handle(), 
        aFunctionNumber));

    DASSERT( aMessage );
    
    // Now, we can be sure that rest of the time iMessage exists.
    // This member variable is set for the CounterPartLost function.
    iMessage = aMessage;
                
    TRAPD( trapError, DoReceiveMessageL( *aMessage, aFunctionNumber ) );
    if ( trapError != KErrNone )
        {
        // Because something went wrong the complete has not been
        // yet called for the message.
        // So, inform the client about the error.
        DLTRACEIN(("Complete with error message"));
        aMessage->CompleteAndRelease( trapError );
        }

    // Because the message should not be used after this, set it NULL.
    // So, CounterPartLost function will know that no messages are
    // waiting the response at the moment.
    iMessage = NULL;

    if ( aFunctionNumber == NcdNodeFunctionIds::ENcdRelease )
        {
        // Because release was called for this object it may be time to
        // delete this object. Inform manager about the release so it may
        // close this object and clear the cache if needed.
        // Notice that if the manager closes this object then this object will
        // be deleted. It is safe to do here because no memeber variables are
        // needed here after the call.
        NodeManager().NodeReleased( *this );       
        }
            
    DLTRACEOUT((""));
    }


void CNcdNode::DoReceiveMessageL( 
    MCatalogsBaseMessage& aMessage,
    TInt aFunctionNumber )
    {
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdInternalize:
            InternalizeRequestL( aMessage );
            break;
            
        case NcdNodeFunctionIds::ENcdLinkHandle:
            LinkHandleRequestL( aMessage );
            break;

        case NcdNodeFunctionIds::ENcdMetadataHandle:
            MetadataHandleRequestL( aMessage );
            break;
            
        case NcdNodeFunctionIds::ENcdNodeSeenHandle:
            NodeSeenHandleRequestL( aMessage );
            break;

        case NcdNodeFunctionIds::ENcdClassId:
            ClassIdRequestL( aMessage );
            break;

        case NcdNodeFunctionIds::ENcdRelease:
            ReleaseRequest( aMessage );
            break;

        default:
            DLERROR(("Unidentified function request"));
            User::Leave( KErrNotSupported );
            break;
        }    
    }



void CNcdNode::CounterPartLost( const MCatalogsSession& aSession )
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



void CNcdNode::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    // Set all the membervariable values to the stream. So,
    // that the stream may be used later to create a new
    // object.

    // First insert data that node manager will use to
    // create this class object
    aStream.WriteInt32L( iNodeClassId );

    // This object acts as a wrapper. So, externalize all the objects that
    // it owns. Note, that metadata is not owned by the node. So, it is not
    // handled here. Metadata is managers responsibility because same metadata
    // may be used by multiple nodes.
    
    // Node Link
    ExternalizeLinkL( aStream );
        
    DLTRACEOUT((""));
    }
    
    
void CNcdNode::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // NOTICE that this internalize function supposes that
    // classid and identifier info that are
    // inserted during externalization, are already read from
    // the stream before calling this function.
    

    // No need to read additional membervariable values from the stream 
    // because all the necessary info has been set during the creation of this
    // object. All other information belongs to the objects that this node
    // wraps.

    InternalizeLinkL( aStream );
    
    // Check: if additional objects will be added for this node. Then their internalization
    // should be called here. 
    
        
    DLTRACEOUT((""));
    }

 

void CNcdNode::InternalizeRequestL( MCatalogsBaseMessage& aMessage ) const
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

    if ( buf->Size() > 0 ) 
        {
        DLINFO(( "Completing the message, buf len: %d", buf->Ptr(0).Length() ));
        }
        
    // If this leaves ReceiveMessage function will complete the message.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );
        
    
    DLINFO(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
        
    DLTRACEOUT((""));
    }


void CNcdNode::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));

    // Insert node class id just in case somebody wants to check that the data
    // is of the right type
    aStream.WriteInt32L( iNodeClassId );

    Identifier().ExternalizeL( aStream );
        
    DLTRACEOUT((""));
    }


void CNcdNode::LinkHandleRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));    
    
    if( iNodeLink == NULL )
        {
        DLINFO(("Node link NULL"));
        User::Leave( KErrNotFound );
        }

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the link to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iNodeLink ) );

    DLINFO(("Link handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );

    DLTRACEOUT((""));    
    }
  
    
void CNcdNode::MetadataHandleRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((("this-ptr: %x"), this));    
    
    if( iNodeMetaData == NULL )
        {
        DLINFO(("Node metadata NULL"));
        User::Leave( KErrNotFound );
        }

    // Get the session that will contain the handle of the node
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the metadata to the session and get the handle.
    // If the node already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iNodeMetaData ) );

    DLINFO(("Metadata handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );

    DLTRACEOUT((""));            
    }
    

void CNcdNode::NodeSeenHandleRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((("this-ptr: %x"), this));
    DASSERT( iNodeSeen );
    
    // Get the session that will contain the handle of the node seen object
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the node seen object to the session and get the handle.
    // If the object already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iNodeSeen ) );

    DLINFO(("Node seen handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );
    }


void CNcdNode::ClassIdRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( ClassId(), KErrNone );
        
    DLTRACEOUT((""));
    }


void CNcdNode::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );
    aMessage.CompleteAndRelease( KErrNone );
    requestSession.RemoveObject( handle );

    DLTRACEOUT((""));
    }


void CNcdNode::ExternalizeLinkL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    if ( iNodeLink != NULL )
        {
        iNodeLink->ExternalizeL( aStream );
        }
    else 
        {
        DLINFO(("No link"));
        aStream.WriteInt32L( NcdNodeClassIds::ENcdNullObjectClassId );
        } 
    
    // Should handle other objects like nodelink above when they are created
        
    DLTRACEOUT((""));
    }


void CNcdNode::InternalizeLinkL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    TInt classId( aStream.ReadInt32L() );
    
    if ( classId == NcdNodeClassIds::ENcdNullObjectClassId )
        {
        // The link was NULL when the data was externalized. So, do not do anything.
        DLTRACEIN(("Link object was null. No need to internalize it")); 
        return;           
        }

    TBool linkCreated( EFalse );
    
    if ( iNodeLink == NULL )
        {
        // Because the node link did not exist create it.
        // Create link should be implemented in child classes. So the right
        // kind of link is set here.
        iNodeLink = CreateLinkL();
        linkCreated = ETrue;
        }
        
    TRAPD( err, iNodeLink->InternalizeL( aStream ) );
    
    if( err != KErrNone )
        {
        if( linkCreated )
            {
            // Because the link was created but the internalization went wrong,
            // delete the link because it does not contain correct data.            
            iNodeLink->Close();
            iNodeLink = NULL;
            }
        User::Leave( err );
        }
        
    DLTRACEOUT((""));
    }
