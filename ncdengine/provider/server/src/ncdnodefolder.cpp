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
* Description:   Implements CNcdNodeFolder class
*
*/


#include "ncdnodefolder.h"
#include "ncdnodefolderlink.h"
#include "ncdnodefoldermetadata.h"
#include "ncdnodemanager.h"
#include "ncdnodeclassids.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "catalogsbasemessage.h"
#include "catalogsutils.h"
#include "ncdchildentity.h"
#include "ncdnodefolderlink.h"
#include "ncdnodeseenfolderimpl.h"
#include "ncdexpirednode.h"

#include "catalogsdebug.h"

CNcdNodeFolder::CNcdNodeFolder( CNcdNodeManager& aNodeManager,
    NcdNodeClassIds::TNcdNodeClassId aNodeClassId, 
    NcdNodeClassIds::TNcdNodeClassId aAcceptedLinkClassId,
    NcdNodeClassIds::TNcdNodeClassId aAcceptedMetaDataClassId )
: CNcdNode( aNodeManager,
            aNodeClassId,
            aAcceptedLinkClassId,
            aAcceptedMetaDataClassId ),
  iPreviousChildCount( KErrNotFound )
    {
    }

void CNcdNodeFolder::ConstructL( const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN(("this: %X", this ));

    CNcdNode::ConstructL( aIdentifier );
    
    iNodeSeenFolder = CNcdNodeSeenFolder::NewL( *this );
        
    DLTRACEOUT((""));
    }


CNcdNodeFolder::~CNcdNodeFolder()
    {
    DLTRACEIN(("this: %X", this));
    for ( TInt i = 0; i < iChildren.Count(); ++i ) 
        {
        DLINFO(( _L("Child %d, ns: %S, id: %S"), i,
                 &iChildren[i]->Identifier().NodeNameSpace(),
                 &iChildren[i]->Identifier().NodeId() ));
        }
    RemoveChildren();
    
    if ( iNodeSeenFolder )
        {
        DLINFO(("Closing node seen folder"));
        iNodeSeenFolder->Close();
        iNodeSeenFolder = NULL;
        }
        
    iPreviousChildren.ResetAndDestroy();

    DLTRACEOUT((""));
    }


CNcdNodeFolder* CNcdNodeFolder::NewL( CNcdNodeManager& aNodeManager,
                                      const CNcdNodeIdentifier& aIdentifier  )
    {
    CNcdNodeFolder* self = 
        CNcdNodeFolder::NewLC( aNodeManager, aIdentifier );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeFolder* CNcdNodeFolder::NewLC( CNcdNodeManager& aNodeManager,
                                       const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdNodeFolder* self = 
        new( ELeave ) CNcdNodeFolder( aNodeManager );
    CleanupClosePushL( *self );
    self->ConstructL( aIdentifier );
    return self;        
    }
                                  

TInt CNcdNodeFolder::ChildCount() const
    {
    DLTRACEIN(( "this: %X, ChildCount: %d", this, iChildren.Count() ));
    return iChildren.Count();
    }


const CNcdNodeIdentifier& CNcdNodeFolder::ChildL( TInt aIndex ) const
    {
    DLTRACEIN(( "" ));
    if( aIndex < 0 || aIndex > iChildren.Count() )
        {
        User::Leave( KErrArgument );
        }
    return iChildren[aIndex]->Identifier();
    }
    
TInt CNcdNodeFolder::ServerChildCountL() const
    {
    DLTRACEIN((""));
    TInt serverChildCount = FolderLinkL().ExpectedChildrenCount();
    DLTRACEOUT(("this: %X, ServerChildCount: %d", this, serverChildCount ));
    return serverChildCount;
    }

const CNcdNodeIdentifier& CNcdNodeFolder::ChildByServerIndexL( TInt aIndex ) const
    {
    DLTRACEIN((""));
    return ChildEntityByServerIndexL( aIndex ).Identifier();
    }

const CNcdChildEntity& CNcdNodeFolder::ChildEntityByServerIndexL( TInt aIndex ) const
    {
    DLTRACEIN(( "aIndex: %d", aIndex )); 
           
    // search for a child with given index
    const CNcdChildEntity* child = NULL;
    for( TInt i = 0 ; i < iChildren.Count() ; i++ )
        {
        if ( iChildren[i]->Index() == aIndex )
            {
            child = iChildren[i];
            break;
            }
        else if ( iChildren[i]->Index() > aIndex )
            {
            // no sense in searching further
            break;
            }
        }
       
    if ( child == NULL )
        {
        // Leave because the child is set NULL.
        // NULL can not be returned as a reference value.
        DLINFO(("Child node identifier was NULL. So, leave with KErrNotFound"))
        User::Leave( KErrNotFound );
        }
    
    DLTRACEOUT((""));

    return *child;
    }

const RPointerArray<CNcdChildEntity>& CNcdNodeFolder::ChildArray() const
    {
    return iChildren;
    }


TBool CNcdNodeFolder::ReplaceChildL( 
    const CNcdNodeIdentifier& aNodeIdentifier,
    TInt aIndex,
    TBool aTransparent,
    CNcdNodeFactory::TNcdNodeType aNodeType )
    {
    DLTRACEIN(( _L("this: %X, Parent ns, id: %S, %S, Child ns, id: %S, %S, aIndex: %d childCount: %d"),
        this, &Identifier().NodeNameSpace(), &Identifier().NodeId(),
        &aNodeIdentifier.NodeNameSpace(), &aNodeIdentifier.NodeId(),
        aIndex, iChildren.Count() ));

    TInt expectedChildCount = FolderLinkL().ExpectedChildrenCount();
    if ( aIndex < 0 /*|| aIndex >= expectedChildCount*/ )
        {
        // For debugging purposes
        DLERROR(("Wrong child index"));
        DASSERT( EFalse );
        
        User::Leave( KErrArgument );   
        }
        
    
    TBool childFound = EFalse;
    TInt i = 0;
    for ( ; i < iChildren.Count() ; i++ )
        {
        if( iChildren[i]->Index() == aIndex )
            {
            DLTRACE(("Child with the same index found."));
            if( ! (iChildren[i]->Identifier().Equals( aNodeIdentifier  ) ) )
                {
                DLTRACE(("Not the same child, replace."));
                iChildren[i]->SetIdentifierL( aNodeIdentifier );
                iChildren[i]->SetTransparent( aTransparent );
                iChildren[i]->SetNodeType( aNodeType );
                // Should the old child be expired via nodemgr?
                }
            else
                {
                iChildren[i]->SetTransparent( aTransparent );
                iChildren[i]->SetNodeType( aNodeType );
                // exactly the same child, do nothing
                }
            childFound = ETrue;
            break;
            }
        else if ( iChildren[i]->Index() > aIndex )
            {
            // no sense in searching further
            break;
            }
        }
        
    if ( !childFound )
        {
        DLTRACE(("Child not found, create new"))
        CNcdChildEntity* childEntity = CNcdChildEntity::NewLC( 
            aIndex,
            aNodeIdentifier,
            aTransparent,
            aNodeType );
            
        if ( i == iChildren.Count() )
            {
            // last child, append
            DLTRACE(("Appending"));
            iChildren.AppendL( childEntity );
            }
        else
            {
            DLTRACE(("Inserting"));
            iChildren.InsertL( childEntity, i );
            }
        CleanupStack::Pop( childEntity );
        }
        
    #ifdef CATALOGS_BUILD_CONFIG_DEBUG
    for( TInt j = 0 ; j < iChildren.Count() ; j++ )
        {
        CNcdChildEntity* child = iChildren[j];
        DLINFO((_L("Child id: %S, array index: %d, real index: %d"),
            &child->Identifier().NodeId(), j, child->Index() ));
        }
    #endif
    
    return !childFound;
    }
        
        
TBool CNcdNodeFolder::InsertChildL( 
    const CNcdNodeIdentifier& aNodeIdentifier,
    TInt aIndex,
    TBool aTransparent,
    CNcdNodeFactory::TNcdNodeType aNodeType )
    {
    DLTRACEIN(( _L("this: %X, Parent ns, id: %S, %S, Child ns, id: %S, %S, aIndex: %d, childCount: %d"),
        this, &Identifier().NodeNameSpace(), &Identifier().NodeId(),
        &aNodeIdentifier.NodeNameSpace(), &aNodeIdentifier.NodeId(),
        aIndex, iChildren.Count() ));
               
    DPROFILING_BEGIN( x );
    if ( aIndex < 0 || aIndex > iChildren.Count() )
        {
        // For debugging purposes
        DLERROR(("Wrong child index"));
        DASSERT( EFalse );
        
        User::Leave( KErrArgument );
        }
    
    CNcdChildEntity* childEntity = CNcdChildEntity::NewLC( 
        aIndex,
        aNodeIdentifier,
        aTransparent,
        aNodeType );
    if( aIndex == iChildren.Count() )
        {
        // last index, append
        DLTRACE(("Appending"));
        iChildren.AppendL( childEntity );
        }
    else
        {
        DLTRACE(("Inserting"));
        iChildren.InsertL( childEntity, aIndex );
        }    
    CleanupStack::Pop( childEntity );    

    #ifdef CATALOGS_BUILD_CONFIG_DEBUG
    for( TInt i = 0 ; i < iChildren.Count() ; i++ )
        {
        DLINFO(( _L("Child: index: %d, real index: %d, id: %S, ns: %S "),
            i, iChildren[i]->Index(), &iChildren[i]->Identifier().NodeId(),
            &iChildren[i]->Identifier().NodeNameSpace() ));
        }
    #endif

    DPROFILING_END( x );
    return ETrue;
    }
    
    
TBool CNcdNodeFolder::AppendChildL( 
    const CNcdNodeIdentifier& aNodeIdentifier,
    TBool aTransparent,
    CNcdNodeFactory::TNcdNodeType aNodeType ) 
    {
    DLTRACEIN((""));
    return InsertChildL( aNodeIdentifier, iChildren.Count(), aTransparent,
        aNodeType );
    }
    
CNcdNodeFolderLink& CNcdNodeFolder::FolderLinkL() const
    {
    // safe to cast because folders always have folder links
    return static_cast<CNcdNodeFolderLink&>( NodeLinkL() );
    }
    

CNcdNodeSeenFolder& CNcdNodeFolder::NodeSeenFolder() const 
    {
    return *iNodeSeenFolder;
    }                  
    
    
void CNcdNodeFolder::RemoveChild( const CNcdNodeIdentifier& aNodeIdentifier ) 
    {
    DLTRACEIN(( _L("this: %X, Parent ns, id: %S, %S"), this,
                &Identifier().NodeNameSpace(), &Identifier().NodeId() ));
    DLINFO(( _L("child to remove ns, id: %S, %S"),
             &aNodeIdentifier.NodeNameSpace(), &aNodeIdentifier.NodeId() ));
    DPROFILING_BEGIN( x );
    for ( TInt i = 0 ; i < iChildren.Count() ; i++ )
        {
        if( iChildren[i]->Identifier().Equals( aNodeIdentifier ) )
            {
            DeleteFromArray( iChildren, i );
            DPROFILING_END( x );
            return;
            }
        }
    
    DPROFILING_END( x );
    DLWARNING(("Unable to remove nonexisting child"));
    }
    

void CNcdNodeFolder::RemoveChildrenL()
    {
    DLTRACEIN((""));
    DPROFILING_BEGIN( x );
    NodeManager().RemoveChildrenL( *this );
    DPROFILING_END( x );
    }
    

void CNcdNodeFolder::RemoveChildren()
    {
    DLTRACEIN((""));
    iChildren.ResetAndDestroy();
    // Also set children previously loaded flag to false,
    // because child list is no longer valid
    iChildrenPreviouslyLoaded = EFalse;
    DLTRACE((_L("node id: %S"), &Identifier().NodeId() ));
    DLTRACE(("iChildrenPreviouslyLoaded: %d",iChildrenPreviouslyLoaded ));
    }
    
void CNcdNodeFolder::StoreChildrenToPreviousListL()
    {
    DLTRACEIN((""));
    StoreChildrenToPreviousListL( iChildren,
        FolderLinkL().ExpectedChildrenCount() );
    }

void CNcdNodeFolder::StoreChildrenToPreviousListL(
    const RPointerArray<CNcdChildEntity>& aPreviousChildren,
    TInt aPreviousChildCount )
    {
    DLTRACEIN((""));
    // First clear previous list.
    iPreviousChildren.ResetAndDestroy();
    for( TInt i = 0 ; i < aPreviousChildren.Count() ; i++ )
        {
        CNcdChildEntity* childEntity = CNcdChildEntity::NewLC(
            *aPreviousChildren[i] );
        DLTRACE((_L("Adding child entity, identifier: %S, index: %d"),
             &childEntity->Identifier().NodeId(), childEntity->Index() ));
        iPreviousChildren.AppendL( childEntity );
        CleanupStack::Pop( childEntity );
        }
    DLTRACE(("Setting previous child count: %d", aPreviousChildCount ));
    iPreviousChildCount = aPreviousChildCount;
    }

TBool CNcdNodeFolder::ChildrenPreviouslyLoaded()
    {
    DLTRACEIN((""));
    DLINFO(("ret: %d",iChildrenPreviouslyLoaded));
    return iChildrenPreviouslyLoaded;
    }

void CNcdNodeFolder::SetChildrenPreviouslyLoaded( TBool aChildrenPreviouslyLoaded )
    {
    DLTRACEIN((""));
    iChildrenPreviouslyLoaded = aChildrenPreviouslyLoaded;
    }

TInt CNcdNodeFolder::PreviousChildCount()
    {
    DLTRACEIN((""));
    return iPreviousChildCount;
    }

const RPointerArray<CNcdChildEntity>& CNcdNodeFolder::PreviousChildArray()
    {
    DLTRACEIN((""));
    return iPreviousChildren;
    }
    
    
void CNcdNodeFolder::ExpireAndRemoveChildrenL()
    {
    DLTRACEIN((""));
    TInt count = iChildren.Count();
    
    CNcdNode* node = NULL;
    
    RPointerArray<CNcdExpiredNode> expiredNodes;
    CleanupResetAndDestroyPushL( expiredNodes );
    expiredNodes.ReserveL( count );
    
    while( count-- ) 
        {        
        node = NodeManager().NodePtrL( iChildren[ count ]->Identifier() );
        if ( node )
            {
            NodeManager().SetNodeExpiredL( *node, EFalse, EFalse, expiredNodes );
             
            }
        }
    CleanupStack::PopAndDestroy( &expiredNodes );
    // Delete child metadatas from disk cache
    NodeManager().RemoveChildrenMetadataL( *this );
    
    // Empty children lists
    RemoveChildren();    
    }
    
    
void CNcdNodeFolder::ReceiveMessage(
    MCatalogsBaseMessage* aMessage,
    TInt aFunctionNumber )
    {
    DLTRACEIN(("this-ptr: %x", this));
    
    DASSERT( aMessage );
    
    // Now, we can be sure that rest of the time iMessage exists.
    // This member variable is set for the CounterPartLost function.
    iMessage = aMessage;
        
    TInt trapError( KErrNone );
    
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdNodeSeenFolderHandle:
            TRAP( trapError, NodeSeenFolderHandleRequestL( *aMessage ) );            
            break;
            
        default:
            // Let base class handle this
            iMessage = NULL;
            CNcdNode::ReceiveMessage( aMessage, aFunctionNumber );
            return;
        }
        
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
    }
            
                

void CNcdNodeFolder::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN(("this: %X, children: %d", this, iChildren.Count() ));

    // First use the parent to externalize the general data
    CNcdNode::ExternalizeL( aStream );

    ExternalizeChildArrayL( aStream );
    
    aStream.WriteInt8L( iChildrenPreviouslyLoaded );
    DLTRACE((_L("node id: %S"), &Identifier().NodeId() ));
    DLTRACE(("wrote iChildrenPreviouslyLoaded: %d",iChildrenPreviouslyLoaded ));

    DLTRACEOUT((""));
    }
    
void CNcdNodeFolder::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN(("this: %X", this));

    // First use the parent to internalize the general data
    CNcdNode::InternalizeL( aStream );
    
    DLINFO(("Parent class internalized"));
    // Now internalize the data of this specific class

    // Insert child information to iChildren array
    // But first release previous info if it exists.
    iChildren.ResetAndDestroy();
    
    
    TInt childCount( aStream.ReadInt32L() );
    DLINFO(("Children: %d", childCount ));
    NcdNodeClassIds::TNcdNodeClassId classObjectType = 
        NcdNodeClassIds::ENcdNullObjectClassId;
    
    for ( TInt i = 0; i < childCount; ++i )
        {
        // This is safe casting because enum is same as TInt
        classObjectType = 
            static_cast<NcdNodeClassIds::TNcdNodeClassId>(aStream.ReadInt32L());
        if ( NcdNodeClassIds::ENcdChildEntityClassId == classObjectType )
            {
            CNcdChildEntity* childEntity = CNcdChildEntity::NewLC( aStream );
            iChildren.AppendL( childEntity );
            CleanupStack::Pop( childEntity );
            }
        else
            {
            // Wrong kind of class object info
            User::Leave( KErrCorrupt );
            }
        }
    
    iChildrenPreviouslyLoaded = aStream.ReadInt8L();
    DLTRACE((_L("node id: %S"), &Identifier().NodeId() ));
    DLTRACE(("read iChildrenPreviouslyLoaded: %d",iChildrenPreviouslyLoaded ));
    DLTRACEOUT((""));
    }


CNcdNodeLink* CNcdNodeFolder::CreateLinkL()
    {
    DLTRACEIN((""));
    CNcdNodeLink* link = NodeLink();
        
    if ( link != NULL )
        {
        DLTRACEOUT(("Link already exists"));
        // The link was already created
        return link;
        }
    else
        {
        DLTRACEOUT(("Creating a new link"));
        // Link was not already created.
        // So, create new.
        return CNcdNodeFolderLink::NewL( *this );        
        }
    }

                                    
void CNcdNodeFolder::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    DLTRACEIN(("this: %X", this));

    CNcdNode::ExternalizeDataForRequestL( aStream );

    // Return also, the child information.
    // Use the virtual function here. So, different children may provide their
    // own functionality here. For example. Root folder may work differently than
    // normal folder.
    ExternalizeChildArrayForRequestL( aStream );
        
    DLTRACEOUT((""));
    }


void CNcdNodeFolder::ExternalizeChildArrayL( RWriteStream& aStream ) const
    {
    DLTRACEIN(("this: %X, childcount: %d", this, iChildren.Count() ));
    DPROFILING_BEGIN( x );
    // Now externalize the data of this specific class
    aStream.WriteInt32L( iChildren.Count() );
    
    for( TInt i = 0; i < iChildren.Count(); ++i )
        {
        DASSERT( iChildren[i] );
        aStream.WriteInt32L( NcdNodeClassIds::ENcdChildEntityClassId );
        iChildren[ i ]->ExternalizeL( aStream );        
        /*
        DLINFO((_L("Child id: %S, array index: %d, real index: %d"),
            &iChildren[i]->Identifier().NodeId(), i, iChildren[i]->Index() ));
            */

        }    
    DPROFILING_END( x );
    DLTRACEOUT((""));
    }


void CNcdNodeFolder::ExternalizeChildArrayForRequestL( RWriteStream& aStream ) const
    {
    DLTRACEIN(("this: %X", this));

    // No special functionality needed here.
    ExternalizeChildArrayL( aStream );

    DLTRACEOUT((""));
    }
    
    
void CNcdNodeFolder::NodeSeenFolderHandleRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((("this-ptr: %x"), this));
    DASSERT( iNodeSeenFolder );
    
    // Get the session that will contain the handle of the node seen folder object
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the node seen folder object to the session and get the handle.
    // If the object already existed in the session we will still
    // get a new handle to the same object.
    TInt32 handle( requestSession.AddObjectL( iNodeSeenFolder ) );

    DLINFO(("Node seen folder handle: %d", handle ));
        
    // Send the information to the client side
    // If this leaves, ReceiveMessage will complete the message.
    aMessage.CompleteAndReleaseL( handle, KErrNone );
    }
    
