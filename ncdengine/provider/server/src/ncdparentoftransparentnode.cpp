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
* Description:   Implements CNcdParentOfTransparentNode class
*
*/


#include <e32err.h>
#include <e32base.h>

#include "ncdparentoftransparentnode.h"
#include "ncdchildentity.h"
#include "ncdnodeidentifier.h"
#include "catalogsutils.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsconstants.h"
#include "ncdsearchnodefolder.h"
#include "ncdnodetransparentfolder.h"



CNcdParentOfTransparentNode::CNcdParentOfTransparentNode( CNcdNodeManager& aNodeManager,
                                                          NcdNodeClassIds::TNcdNodeClassId aNodeClassId ) 
: CNcdNodeFolder( aNodeManager, aNodeClassId ) 
    {
    }

void CNcdParentOfTransparentNode::ConstructL( const CNcdNodeIdentifier& aIdentifier ) 
    {
    CNcdNodeFolder::ConstructL( aIdentifier );
    }
    
CNcdParentOfTransparentNode::~CNcdParentOfTransparentNode() 
    {
    }


void CNcdParentOfTransparentNode::ExternalizeL( RWriteStream& aStream ) 
    {
    DLTRACEIN((""));
    CNcdNodeFolder::ExternalizeL( aStream );
    DLTRACEOUT((""));
    }
    
void CNcdParentOfTransparentNode::InternalizeL( RReadStream& aStream ) 
    {
    DLTRACEIN((""));
    CNcdNodeFolder::InternalizeL( aStream );
    DLTRACEOUT((""));
    }


                                    
void CNcdParentOfTransparentNode::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    DLTRACEIN(("this: %X", this));

    // Just delegate the command forward
    CNcdNodeFolder::ExternalizeDataForRequestL( aStream );
    
    // This will give the correct child count that may be needed in proxy side.
    // Because the child count may differ there if transparent child folders are
    // replaced by their children.
    aStream.WriteInt32L( ChildArray().Count() );

    DLTRACEOUT((""));
    }
    

void CNcdParentOfTransparentNode::ExternalizeChildArrayForRequestL( RWriteStream& aStream ) const
    {
    DLTRACEIN(("this: %X", this));
    DPROFILING_BEGIN( x );
    
    // Fixes PRECLI-1539
    // This method doesn't support paging and the parent implementation doesn't support
    // transparents but since search folders need paging but not transparents we call the
    // base class
    //
    // Note: No sense overriding this method in CNcdSearchNodeFolder because its
    // children need this implementation so we would have to implement overrides also
    // to them
    if ( ClassId() == NcdNodeClassIds::ENcdSearchFolderNodeClassId ) 
        {
        DLTRACE(("No need for transparent support, call CNcdNodeFolder-method"));
        CNcdNodeFolder::ExternalizeChildArrayForRequestL( aStream );
        return;
        }
    
    CNcdNode* childNode( NULL );
    CNcdNodeFolder* transparentFolder( NULL );
    const CNcdNodeIdentifier* childIdentifier( NULL );
    const CNcdNodeIdentifier* transparentChildIdentifier( NULL );

    // This array will contain the identifiers of the children that are set for
    // the proxy side
    RPointerArray<CNcdChildEntity> childEntitys;
    CleanupResetAndDestroyPushL( childEntitys );
    
    // Remember to check if this folder contains any transparent folders.
    // Add all the children into the temporary array that will contain
    // the identifiers of the children for the proxy folder.    
    // child entitys need an index as well as identifier
    
  
    const RPointerArray<CNcdChildEntity>& childArray( ChildArray() );
    TInt childCount = childArray.Count();
    childEntitys.ReserveL( childCount );
    DLTRACE(( "Externalizing at least %d children", childCount ));
    TInt childIndex = 0;    
    for ( TInt i = 0; i < childCount; ++i ) 
        {
        childNode = NULL;
        childIdentifier = &childArray[i]->Identifier();
        DASSERT( childIdentifier );
        if ( childArray[i]->IsTransparent() ) 
            {
            DLTRACE(("child is likely transparent"));
            childNode = NodeManager().NodePtrL( *childIdentifier );

            // This is as a backup
            if ( childNode != NULL 
                && (childNode->ClassId() == NcdNodeClassIds::ENcdTransparentFolderNodeClassId ||
                    (childNode->ClassId() == NcdNodeClassIds::ENcdSearchFolderNodeClassId &&
                    NodeManager().SearchFolderL( *childIdentifier ).IsTransparent() ) ) )
                {
                DLINFO(("Transparent folder found for child array"));
                // Because the transparent folder is replaced,
                // add the the children of the transparent folder here.
                transparentFolder = static_cast<CNcdNodeFolder*>(childNode);
                const RPointerArray<CNcdChildEntity>& children = transparentFolder->ChildArray();
                DLINFO((("Transparent folder has %d children"), children.Count() ));
                for( TInt j = 0; j < children.Count(); ++j )
                    {
                    transparentChildIdentifier = &children[j]->Identifier();

                    DLINFO(("Child of transparent to array."));
                    CNcdChildEntity* childEntity = 
                        CNcdChildEntity::NewLC( 
                            childIndex++,
                            *transparentChildIdentifier,
                            children[j]->IsTransparent(),
                            children[j]->NodeType() );
                    childEntitys.AppendL( childEntity );
                    CleanupStack::Pop( childEntity );
                    }
                }
            else 
                {
                // In case childNode was found but it wasn't really transparent
                childNode = NULL;
                }
            
            }
        
        // Handle non-transparents and nodes that were not found from node manager
        if ( !childNode )        
            {
            DLINFO(("Normal child. Add to the array."));
            DLNODEID(( *childIdentifier ));
            
            // Child was normal child.                
            CNcdChildEntity* childEntity = CNcdChildEntity::NewLC( 
                childIndex++, 
                *childIdentifier, 
                childArray[i]->IsTransparent(),
                childArray[i]->NodeType() );
            childEntitys.AppendL( childEntity );
            CleanupStack::Pop( childEntity );            
            }
        }

    // First insert the size of the child array
    aStream.WriteInt32L( childEntitys.Count() );
    
    // Insert all the child identifier info for the proxy into the stream
    for( TInt i = 0; i < childEntitys.Count(); ++i )
        {
        // Inform that the next class is child entity
        aStream.WriteInt32L( NcdNodeClassIds::ENcdChildEntityClassId );
        childEntitys[ i ]->ExternalizeL( aStream );
        }    

    // Close the array, but do not delete its elements that are owned else where.
    CleanupStack::PopAndDestroy( &childEntitys );
    DPROFILING_END( x );
    DLTRACEOUT((""));
    }
    
    
void CNcdParentOfTransparentNode::ReceiveMessage( MCatalogsBaseMessage* aMessage,
                               TInt aFunctionNumber )
    {
    DLTRACEIN(("handle: %d, function: %d", aMessage->Handle(), 
        aFunctionNumber));

    DASSERT( aMessage );
    
    // Now, we can be sure that rest of the time iMessage exists.
    // This member variable is set for the CounterPartLost function.
    iMessage = aMessage;
        
    TInt trapError( KErrNone );
    
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdIsTransparentChildExpired:
            TRAP( trapError, IsTransparentChildExpiredL( *aMessage ) );
            break;
            
        default:
            // Let base class handle this message
            iMessage = NULL;
            CNcdNodeFolder::ReceiveMessage( aMessage, aFunctionNumber );
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

    DLTRACEOUT((""));
    }

void CNcdParentOfTransparentNode::IsTransparentChildExpiredL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    // Write the value to the stream
    stream.WriteInt32L( IsTransparentChildExpiredL() );
    
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
    
TBool CNcdParentOfTransparentNode::IsTransparentChildExpiredL() const
    {
    DLTRACEIN((""));
    TBool isTransparentChildExpired = EFalse;
    for( TInt i = 0 ; i < ServerChildCountL() ; i++ )
        {
        CNcdNode* child = NULL;
        TRAPD( err, child = &NodeManager().NodeL( ChildByServerIndexL(i) ) );
        if( err == KErrNotFound || 
            ( err == KErrNone  &&
                child->ClassId() == NcdNodeClassIds::ENcdTransparentFolderNodeClassId &&
                child->State() != MNcdNode::EStateInitialized ) )
            {
            DLTRACE(("At least one child was expired/uninitialized/not found."));
            isTransparentChildExpired = ETrue;
            break;
            }
        else if ( err == KErrNone &&
            child->ClassId() == NcdNodeClassIds::ENcdTransparentFolderNodeClassId &&
            child->State() == MNcdNode::EStateInitialized )
            {
            DLTRACE(("Transparent child folder, check it's children"));
            CNcdNodeTransparentFolder* transpFolder =
                static_cast<CNcdNodeTransparentFolder*>(child);
            if ( transpFolder->HasExpiredOrMissingChildrenL() )
                {
                DLTRACE(("Transparent child folder's child expired or missing."));
                isTransparentChildExpired = ETrue;
                break;
                }
            }
        else if ( err != KErrNone && err != KErrNotFound )
            {
            DLTRACE(("Error: %d", err));
            User::Leave( err );
            }
        }
    return isTransparentChildExpired;
    }
