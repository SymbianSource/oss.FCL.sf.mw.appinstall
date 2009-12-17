/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ncdnodeseenfolderimpl.h"
#include "catalogsdebug.h"
#include "ncdnodefunctionids.h"
#include "catalogsbasemessage.h"
#include "ncdnodemanager.h"
#include "ncdnodefolder.h"
#include "ncdnodeseenimpl.h"
#include "catalogsutils.h"
#include "ncdnodeidentifier.h"

CNcdNodeSeenFolder* CNcdNodeSeenFolder::NewL( CNcdNodeFolder& aParent ) 
    {
    DLTRACEIN((""));
    CNcdNodeSeenFolder* self = NewLC( aParent );
    CleanupStack::Pop( self );
    return self;
    }


CNcdNodeSeenFolder* CNcdNodeSeenFolder::NewLC( CNcdNodeFolder& aParent ) 
    {
    DLTRACEIN((""));
    CNcdNodeSeenFolder* self = new( ELeave ) CNcdNodeSeenFolder( aParent );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;
    }
    

CNcdNodeSeenFolder::CNcdNodeSeenFolder( CNcdNodeFolder& aParent ) :
    iOwnerNode( aParent ) 
    {
    }
    

void CNcdNodeSeenFolder::ConstructL() 
    {
    }
    

CNcdNodeSeenFolder::~CNcdNodeSeenFolder() 
    {
    DLTRACEIN((""));
    }


void CNcdNodeSeenFolder::ReceiveMessage(
    MCatalogsBaseMessage* aMessage,
    TInt aFunctionNumber )
    {
    DLTRACEIN((""));
    
    DASSERT( aMessage );
        
    TInt trapError( KErrNone );
    
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdNodeSeenFolderSetContentsSeen:
            TRAP( trapError, SetContentsSeenRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdNodeSeenFolderNewCount:
            TRAP( trapError, NewCountRequestL( *aMessage ) );
            break;
            
        case NcdNodeFunctionIds::ENcdNodeSeenFolderNewNodes:
            TRAP( trapError, NewNodesRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdRelease:
            ReleaseRequest( *aMessage );
            break;
            
        default:
            DLERROR(("Unidentified function request"));
            DASSERT( EFalse );
            break;
        }
       
    if ( trapError != KErrNone )
        {
        // Because something went wrong the complete has not been
        // yet called for the message.
        // So, inform the client about the error.
        aMessage->CompleteAndRelease( trapError );
        }
    }
    

void CNcdNodeSeenFolder::CounterPartLost( const MCatalogsSession& /*aSession*/ )
    {
    DLTRACEIN((""));
    }    
    
    
void CNcdNodeSeenFolder::SetContentsSeenRequestL(
    MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));
    SetContentsSeenL();        
    aMessage.CompleteAndRelease( KErrNone );
    }
    
    
void CNcdNodeSeenFolder::NewCountRequestL(
    MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));
    
    // Read the level number.
    RCatalogsMessageReader reader;
    reader.OpenLC( aMessage );
    
    TInt level = reader().ReadInt32L();
    DASSERT( level > 0 );
    
    CleanupStack::PopAndDestroy( &reader );
    
    TInt count = NewChildCountL( level - 1 );
    
    aMessage.CompleteAndReleaseL( count, KErrNone );
    }
    
    
void CNcdNodeSeenFolder::NewNodesRequestL(
    MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));
    
    // Read the level number
    RCatalogsMessageReader reader;
    reader.OpenLC( aMessage );
    
    TInt level = reader().ReadInt32L();
    DASSERT( level > 0 );
    
    CleanupStack::PopAndDestroy( &reader );
    
    RPointerArray<CNcdNodeIdentifier> newNodes;
    CleanupResetAndDestroyPushL( newNodes );
    
    NewChildrenL( level - 1, newNodes );
    TInt newCount = newNodes.Count();
    
    DLINFO(("found %d new nodes", newCount ));
    
    // Write the node identifier to output buffer.
    RCatalogsBufferWriter writer;
    writer.OpenLC();
    writer().WriteInt32L( newCount );
    
    for ( TInt i = 0; i < newCount; i++ ) 
        {
        newNodes[ i ]->ExternalizeL( writer() );
        }
        
    aMessage.CompleteAndReleaseL( writer.PtrL(), KErrNone );
    
    CleanupStack::PopAndDestroy( &writer );
    CleanupStack::PopAndDestroy( &newNodes );
    }
    

void CNcdNodeSeenFolder::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
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


TInt CNcdNodeSeenFolder::NewChildCountL(
    TInt aRecursionLevel ) const 
    {
    DLTRACEIN((""));
    DASSERT( aRecursionLevel >= 0 );
    
    CNcdNodeManager& nodeManager = iOwnerNode.NodeManager();
    
    TInt newCount( 0 );

    for ( TInt i = 0; i < iOwnerNode.ChildCount(); i++ ) 
        {
        const CNcdNodeIdentifier& childIdentifier =
            iOwnerNode.ChildL( i );
        CNcdNode* child = nodeManager.NodePtrL( childIdentifier );
        if ( child ) 
            {
            // Increase the count if the node is not seen, but skip transparent nodes since
            // they are not visible in proxy side anyway.
            if ( !child->NodeSeen().IsSeenL() &&
                 child->ClassId() != NcdNodeClassIds::ENcdTransparentFolderNodeClassId ) 
                {
                newCount++;
                }
            else if ( child->ClassId() == NcdNodeClassIds::ENcdTransparentFolderNodeClassId
                      && aRecursionLevel == 0 ) 
                {
                // Transparent child, add the children of the folder to child count.
                CNcdNodeFolder* transparent = static_cast<CNcdNodeFolder*>( child );
                newCount += transparent->NodeSeenFolder().NewChildCountL( aRecursionLevel );
                }
                                  
            if ( aRecursionLevel > 0 ) 
                {
                // If recursion level is greater than 0 and the
                // the node is a folder, add the child count of
                // the folder too.
                if ( CNcdNodeFactory::NodeTypeL( *child ) == CNcdNodeFactory::ENcdNodeFolder )
                    {
                    CNcdNodeFolder* folder = static_cast<CNcdNodeFolder*>( child );
                    
                    // If the folder is transparent folder, the recursion level must not
                    // be decreased.
                    TInt nextRecursionLevel( aRecursionLevel - 1 );
                    if ( folder->ClassId() == NcdNodeClassIds::ENcdTransparentFolderNodeClassId ) 
                        {
                        nextRecursionLevel++;
                        }
                        
                    newCount += folder->NodeSeenFolder().NewChildCountL(
                        nextRecursionLevel );
                    }
                }
            }
        }
        
    return newCount;
    }
    
    
void CNcdNodeSeenFolder::NewChildrenL(
    TInt aRecursionLevel, RPointerArray<CNcdNodeIdentifier>& aNodes ) const
    {
    DLTRACEIN(("level %d", aRecursionLevel));
    DASSERT( aRecursionLevel >= 0 );
    
    CNcdNodeManager& nodeManager = iOwnerNode.NodeManager();
    TInt childCount = iOwnerNode.ChildCount();
    
    for ( TInt i = 0; i < childCount; i++ ) 
        {
        const CNcdNodeIdentifier& childId = iOwnerNode.ChildL( i );
        CNcdNode* child = nodeManager.NodePtrL( childId );
        if ( !child ) 
            {
            continue;
            }
        
        // If the child is transparent folder, don't add it to the array.
        if ( child->ClassId() != NcdNodeClassIds::ENcdTransparentFolderNodeClassId && !child->NodeSeen().IsSeenL() ) 
            {
            // Create copy of the identifier and add it to the array.
            CNcdNodeIdentifier* copy = CNcdNodeIdentifier::NewLC( childId );
            aNodes.AppendL( copy );
            CleanupStack::Pop( copy );
            }
        else if ( child->ClassId() == NcdNodeClassIds::ENcdTransparentFolderNodeClassId
                  && aRecursionLevel == 0 ) 
            {
            // Transparent child, and this is the last recursion level. The children of the
            // transparent folder must be added to the array.
            CNcdNodeFolder* transparent = static_cast<CNcdNodeFolder*>( child );
            transparent->NodeSeenFolder().NewChildrenL( aRecursionLevel, aNodes );
            }            
                          
        if ( aRecursionLevel > 0 ) 
            {
            // If the node is a folder, go further in recursion.
            CNcdNodeFactory::TNcdNodeType nodeType = CNcdNodeFactory::NodeTypeL( *child );
            if ( nodeType == CNcdNodeFactory::ENcdNodeFolder || 
                 nodeType == CNcdNodeFactory::ENcdNodeRoot )
                {
                CNcdNodeFolder* folder = static_cast<CNcdNodeFolder*>( child );
                                
                // If the folder is transparent folder, the recursion level must not
                // be decreased.
                TInt nextRecursionLevel( aRecursionLevel - 1 );
                if ( folder->ClassId() == NcdNodeClassIds::ENcdTransparentFolderNodeClassId ) 
                    {
                    nextRecursionLevel++;
                    }
                folder->NodeSeenFolder().NewChildrenL( nextRecursionLevel, aNodes );
                }
            }
        }
    }
    

void CNcdNodeSeenFolder::SetContentsSeenL() const
    {
    DLTRACEIN((""));

    CNcdNodeManager& nodeManager = iOwnerNode.NodeManager();
    
    // Set the children of the folder as seen.
    for ( TInt i = 0; i < iOwnerNode.ChildCount(); i++ ) 
        {
        const CNcdNodeIdentifier& childIdentifier =
            iOwnerNode.ChildL( i );
        CNcdNode* child = nodeManager.NodePtrL( childIdentifier );
        if ( child ) 
            {
            // If the child is transparent folder, set its contents as seen since in
            // UI they are immediate children of this folder.
            if ( child->ClassId() == NcdNodeClassIds::ENcdTransparentFolderNodeClassId ) 
                {
                CNcdNodeFolder* transparent = static_cast<CNcdNodeFolder*>( child );
                transparent->NodeSeenFolder().SetContentsSeenL();
                }
            else 
                {
                child->NodeSeen().SetSeenL();
                }
            }
        }
    }
