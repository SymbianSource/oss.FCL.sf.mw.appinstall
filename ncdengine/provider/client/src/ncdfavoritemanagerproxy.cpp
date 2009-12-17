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
* Description:   Contains CNcdFavoriteManagerProxy class
*
*/


#include <s32mem.h>

#include "ncdfavoritemanagerproxy.h"
#include "catalogsdebug.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsclientserver.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeidentifier.h"
#include "catalogsconstants.h"
#include "ncdnodemanagerproxy.h"
#include "ncdnodeproxy.h"
#include "catalogsutils.h"
#include "ncdnodedisclaimerproxy.h"
#include "ncdnodeidentifiereditor.h"
#include "ncdnodeidentifierutils.h"

CNcdFavoriteManagerProxy* CNcdFavoriteManagerProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CCatalogsInterfaceBase* aParent,
    CNcdNodeManagerProxy& aNodeManager ) 
    {
    CNcdFavoriteManagerProxy* self = NewLC(
        aSession, aHandle, aParent, aNodeManager );
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdFavoriteManagerProxy* CNcdFavoriteManagerProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CCatalogsInterfaceBase* aParent,
    CNcdNodeManagerProxy& aNodeManager ) 
    {
    CNcdFavoriteManagerProxy* self =
        new( ELeave ) CNcdFavoriteManagerProxy(
            aSession, aHandle, aParent, aNodeManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
CNcdFavoriteManagerProxy::CNcdFavoriteManagerProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CCatalogsInterfaceBase* aParent,
    CNcdNodeManagerProxy& aNodeManager )
    : CNcdInterfaceBaseProxy( aSession, aHandle, aParent ),
      iNodeManager( aNodeManager ) 
    {
    }
    
CNcdFavoriteManagerProxy::~CNcdFavoriteManagerProxy() 
    {
    DLTRACEIN((""));
    iFavoriteNodes.ResetAndDestroy();
    }
    

TBool CNcdFavoriteManagerProxy::IsFavoriteL(
    const CNcdNodeIdentifier& aNodeIdentifier ) const
    {
    DLTRACEIN((""));
    
    // If the metadata is favorite, the node itself is favorite.
    CNcdNodeIdentifier* actualFavId = ActualFavoriteIdentifierL( aNodeIdentifier );
    return actualFavId != NULL;
    }


void CNcdFavoriteManagerProxy::AddToFavoritesL(
    const CNcdNodeIdentifier& aNodeIdentifier, TBool aRemoveOnDisconnect ) 
    {
    DLTRACEIN((""));
    
    // Check if the metadata is already added as favorite.
    CNcdNodeIdentifier* metadataId =
        NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( aNodeIdentifier );
    if ( FavoriteNodeByMetaDataL( *metadataId ) ) 
        {
        // The metadata is already favorite, do nothing.
        CleanupStack::PopAndDestroy( metadataId );
        return;
        }        
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );

    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );
    
    aNodeIdentifier.ExternalizeL( stream );
    stream.WriteInt8L( aRemoveOnDisconnect );
    
    CleanupStack::PopAndDestroy( &stream );
    
    TInt output;
    
    // Send the message to server side.
    User::LeaveIfError(
        ClientServerSession().SendSync(
            NcdNodeFunctionIds::ENcdFavoriteManagerAddFavorite,
            buf->Ptr( 0 ),
            output,
            Handle() ) );
            
    CleanupStack::PopAndDestroy( buf );            
    
    // Add the identifier to proxy side list too.
    CNcdNodeIdentifier* copy = CNcdNodeIdentifier::NewLC( aNodeIdentifier );
    iFavoriteNodes.AppendL( copy );
    CleanupStack::Pop( copy );    
    CleanupStack::PopAndDestroy( metadataId );
    
    // In principle, the other node proxys having the same metadata should
    // be provided with the MNcdNodeFavorite interface as well.
    }
    
    
void CNcdFavoriteManagerProxy::RemoveFromFavoritesL(
    const CNcdNodeIdentifier& aNodeIdentifier ) 
    {
    DLTRACEIN((""));
    
    // The actual node to remove may have different id but same metadata,
    // get the actual favorite node.
    CNcdNodeIdentifier* actualIdentifier = ActualFavoriteIdentifierL( aNodeIdentifier );
    if ( !actualIdentifier ) 
        {
        // Nothing to do, the node is not favourite.
        return;
        }
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );

    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );
    
    actualIdentifier->ExternalizeL( stream );
    
    CleanupStack::PopAndDestroy( &stream );
    
    TInt output;
    
    // Send the message to server side.
    User::LeaveIfError(
        ClientServerSession().SendSync(
            NcdNodeFunctionIds::ENcdFavoriteManagerRemoveFavorite,
            buf->Ptr( 0 ),
            output,
            Handle() ) );
            
    CleanupStack::PopAndDestroy( buf );
    
    // Remove the identifier from proxy side list too.
    TInt nodeCount = iFavoriteNodes.Count();
    for ( TInt i = 0; i < nodeCount; i++ ) 
        {
        if ( iFavoriteNodes[i]->Equals( *actualIdentifier ) ) 
            {
            delete iFavoriteNodes[i];
            iFavoriteNodes.Remove( i );
            break;
            }
        }
    
    // In principle, the other node proxys having the same metadata should
    // not have the MNcdNodeFavorite interface after this, so it should be removed.
    }
    
    
void CNcdFavoriteManagerProxy::SetNodeDisclaimerL(
    const CNcdNodeIdentifier& aFavoriteNodeIdentifier,
    MNcdNode* aDisclaimerOwner ) 
    {
    DLTRACEIN((""));
    
    // If the node is not favorite, leave
    if ( !IsFavoriteL( aFavoriteNodeIdentifier ) ) 
        {
        User::Leave( KErrArgument );
        }
        
    CNcdNodeIdentifier* actualFavIdentifier = 
        ActualFavoriteIdentifierL( aFavoriteNodeIdentifier );
    DASSERT( actualFavIdentifier );
    
    if ( aDisclaimerOwner ) 
        {
        CNcdNodeIdentifier* disclaimerOwnerId =
            CNcdNodeIdentifier::NewLC(
                aDisclaimerOwner->Namespace(), aDisclaimerOwner->Id(),
                aFavoriteNodeIdentifier.ClientUid() );
                
        RCatalogsBufferWriter writer;
        writer.OpenLC();
        actualFavIdentifier->ExternalizeL( writer() );
        writer().WriteInt32L( ETrue );        
        disclaimerOwnerId->ExternalizeL( writer() );

        TInt output;
                        
        // Send the message to server side.
        User::LeaveIfError(
            ClientServerSession().SendSync(
                NcdNodeFunctionIds::ENcdFavoriteManagerSetDisclaimer,
                writer.PtrL(),
                output,
                Handle() ) );
                
        CleanupStack::PopAndDestroy( &writer );
        CleanupStack::PopAndDestroy( disclaimerOwnerId );
        }
    else 
        {
        // NULL node was given, remove the disclaimer.

        RCatalogsBufferWriter writer;
        writer.OpenLC();
        actualFavIdentifier->ExternalizeL( writer() );
        writer().WriteInt32L( EFalse );        

        TInt output;
        
        // Send the message to server side.
        User::LeaveIfError(
            ClientServerSession().SendSync(
                NcdNodeFunctionIds::ENcdFavoriteManagerSetDisclaimer,
                writer.PtrL(),
                output,
                Handle() ) );
                
        CleanupStack::PopAndDestroy( &writer );                
        }
    }
    

CNcdNodeDisclaimerProxy* CNcdFavoriteManagerProxy::NodeDisclaimerL(
    const CNcdNodeIdentifier& aNodeIdentifier ) 
    {
    DLTRACEIN((""));
    
    // If the node is not favorite, leave
    if ( !IsFavoriteL( aNodeIdentifier ) ) 
        {
        User::Leave( KErrArgument );
        }
        
    CNcdNodeIdentifier* actualFavIdentifier =
        ActualFavoriteIdentifierL( aNodeIdentifier );
    DASSERT( actualFavIdentifier );
        
    // Get the disclaimer handle at first.
    
    RCatalogsBufferWriter writer;
    writer.OpenLC();
    actualFavIdentifier->ExternalizeL( writer() );
    
    TInt disclaimerHandle;
    TInt handleError =
        ClientServerSession().SendSync(
            NcdNodeFunctionIds::ENcdFavoriteManagerDisclaimerHandle,
            writer.PtrL(),
            disclaimerHandle,
            Handle() );
            
    CleanupStack::PopAndDestroy( &writer );
    
    if ( handleError == KErrNotFound ) 
        {
        // Disclaimer did not exist in server side, return NULL.
        return NULL;
        }
    
    User::LeaveIfError( handleError );
    
    CNcdNodeDisclaimerProxy* disclaimerProxy =
        CNcdNodeDisclaimerProxy::NewL(
            ClientServerSession(), disclaimerHandle, NULL );            
        
    return disclaimerProxy;
    }
    

CNcdNodeProxy* CNcdFavoriteManagerProxy::FavoriteNodeByMetaDataL(
    const CNcdNodeIdentifier& aMetaDataIdentifier ) const 
    {
    DLTRACEIN((""));
    TInt count = iFavoriteNodes.Count();
    for ( TInt i = 0; i < count; i++ ) 
        {
        CNcdNodeIdentifier* metaDataId = 
            NcdNodeIdentifierEditor::CreateMetaDataIdentifierL( *iFavoriteNodes[ i ] );
        if ( metaDataId->Equals( aMetaDataIdentifier ) )
            {
            delete metaDataId;
            return &iNodeManager.NodeL( *iFavoriteNodes[ i ] );
            }
        delete metaDataId;
        }
        
    // Node was not found, return NULL.
    return NULL;
    }
            
        
        

TInt CNcdFavoriteManagerProxy::FavoriteNodeCount() const
    {
    DLTRACEIN((""));
    return iFavoriteNodes.Count();
    }
    

MNcdNode* CNcdFavoriteManagerProxy::FavoriteNodeL( TInt aIndex ) const
    {
    DLTRACEIN((""));
    
    if ( aIndex < 0 || aIndex >= iFavoriteNodes.Count() ) 
        {
        User::Leave( KErrArgument );
        }
        
    // The favorite list is in order where the previously added item is last.
    // Return the nodes in the opposite order.
    
    TInt realIndex = iFavoriteNodes.Count() - 1 - aIndex;
    CNcdNodeIdentifier* nodeId = iFavoriteNodes[ realIndex ];
    MNcdNode* node = &iNodeManager.NodeL( *nodeId );
    
    // Increase the reference count by one.
    node->AddRef();
    
    return node;
    }
    

TInt CNcdFavoriteManagerProxy::FavoriteIndexL(
    const TDesC& aNamespace, const TDesC& aId ) const 
    {
    DLTRACEIN((""));
    
    if ( iFavoriteNodes.Count() == 0 )
        {
        User::Leave( KErrNotFound );
        }
            
    CNcdNodeIdentifier* nodeId = CNcdNodeIdentifier::NewLC(
        aNamespace, aId, iFavoriteNodes[ 0 ]->ClientUid() );

    // Get the actual favorite node identifier. It may be different, since
    // there is only one node per metadata as favorite node.
    // Note, the ownership of the returned object is not transferred.
    CNcdNodeIdentifier* actualId = ActualFavoriteIdentifierL( *nodeId );
    
    if ( actualId == NULL ) 
        {
        User::Leave( KErrNotFound );
        }

    TInt index = NcdNodeIdentifierUtils::IdentifierIndex(
        *actualId, iFavoriteNodes );
    DASSERT( index != KErrNotFound );
    
    CleanupStack::PopAndDestroy( nodeId );
    
    // Since the list is in opposite order, the index must be converted.
    TInt correctIndex = iFavoriteNodes.Count() - 1 - index;
    DLINFO(("returning index: %d", correctIndex ));
    return correctIndex;
    }
    

void CNcdFavoriteManagerProxy::ClearFavoritesL() 
    {
    DLTRACEIN((""));
    while ( iFavoriteNodes.Count() ) 
        {
        MNcdNode* node = FavoriteNodeL( 0 );
        CleanupReleasePushL( *node );
        
        MNcdNodeFavorite* favorite = node->QueryInterfaceLC< MNcdNodeFavorite >();
        DASSERT( favorite != NULL );
        favorite->RemoveFromFavoritesL();
        
        CleanupStack::PopAndDestroy( favorite );
        CleanupStack::PopAndDestroy( node );
        }
    }
    
        
void CNcdFavoriteManagerProxy::ConstructL() 
    {
    DLTRACEIN((""));
    
    // Register the interface
    MNcdFavoriteManager* interface( this );
    AddInterfaceL(
        CCatalogsInterfaceIdentifier::NewL( 
            interface, this, MNcdFavoriteManager::KInterfaceUid ) );
            
    // Internalize the state from server side object
    InternalizeL();
    }
    
void CNcdFavoriteManagerProxy::InternalizeL() 
    {
    DLTRACEIN((""));
    
    HBufC8* data( NULL );
    
    // Internalize the node list from server side object
    User::LeaveIfError(
        ClientServerSession().SendSyncAlloc(
            NcdNodeFunctionIds::ENcdInternalize,
            KNullDesC8(),
            data,
            Handle(),
            0 ) );
        
    CleanupStack::PushL( data );
 
    // Read the data from the stream and insert it to the memeber variables
    RDesReadStream stream( *data );
    CleanupClosePushL( stream );
    
    InternalizeDataL( stream );
    
    // Closes the stream
    CleanupStack::PopAndDestroy( &stream ); 
    CleanupStack::PopAndDestroy( data );
    }
    
void CNcdFavoriteManagerProxy::InternalizeDataL( RReadStream& aStream ) 
    {
    DLTRACEIN((""));
    iFavoriteNodes.ResetAndDestroy();
    
    TInt identifierCount = aStream.ReadInt32L();
    for ( TInt i = 0; i < identifierCount; i++ ) 
        {
        CNcdNodeIdentifier* nodeId = CNcdNodeIdentifier::NewLC( aStream );
        iFavoriteNodes.AppendL( nodeId );
        CleanupStack::Pop( nodeId );
        }
    }
    
CNcdNodeIdentifier* CNcdFavoriteManagerProxy::ActualFavoriteIdentifierL(
    const CNcdNodeIdentifier& aNodeIdentifier ) const 
    {
    DLTRACEIN((""));
    CNcdNodeIdentifier* metadataId =
        NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( aNodeIdentifier );
    CNcdNodeIdentifier* actualId = FavoriteIdentifierByMetaDataL( *metadataId );
    CleanupStack::PopAndDestroy( metadataId );
    return actualId;
    }
    
CNcdNodeIdentifier* CNcdFavoriteManagerProxy::FavoriteIdentifierByMetaDataL(
    const CNcdNodeIdentifier& aMetaDataIdentifier ) const 
    {
    DLTRACEIN((""));
    TInt count = iFavoriteNodes.Count();
    for ( TInt i = 0; i < count; i++ ) 
        {
        CNcdNodeIdentifier* metaId = NcdNodeIdentifierEditor::CreateMetaDataIdentifierL(
            *iFavoriteNodes[ i ] );
        if ( metaId->Equals( aMetaDataIdentifier ) ) 
            {
            delete metaId;
            return iFavoriteNodes[ i ];
            }
        delete metaId;
        }
        
    return NULL;
    }
    
