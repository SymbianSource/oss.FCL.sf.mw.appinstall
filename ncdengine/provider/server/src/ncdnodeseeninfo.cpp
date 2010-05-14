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


#include "ncdnodeseeninfo.h"

#include "ncdnodeidentifierutils.h"
#include "ncdnodeidentifier.h"
#include "ncdstorage.h"
#include "ncdstoragemanager.h"
#include "ncdproviderdefines.h"
#include "ncddatabasestorage.h"
#include "ncdstorageitem.h"
#include "ncdrootnode.h"
#include "ncdnodeidentifiereditor.h"
#include "catalogsutils.h"
#include "ncdchildentity.h"
#include "ncdchildentitymap.h"
#include "ncdnodefolderlink.h"
#include "ncdgeneralmanager.h"

#include "catalogsdebug.h"

CNcdNodeSeenInfo* CNcdNodeSeenInfo::NewL( CNcdGeneralManager& aGeneralManager )
    {
    CNcdNodeSeenInfo* self = NewLC( aGeneralManager );
    CleanupStack::Pop( self );
    return self;
    }
    

CNcdNodeSeenInfo* CNcdNodeSeenInfo::NewLC( CNcdGeneralManager& aGeneralManager )
    {
    CNcdNodeSeenInfo* self = new( ELeave ) CNcdNodeSeenInfo( aGeneralManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeSeenInfo::CNcdNodeSeenInfo( CNcdGeneralManager& aGeneralManager ) :
    iGeneralManager( aGeneralManager ),
    iStorageManager( aGeneralManager.StorageManager() ), 
    iNodeManager( aGeneralManager.NodeManager() )
    {	
    }    


void CNcdNodeSeenInfo::ConstructL()
    {
    }    


CNcdNodeSeenInfo::~CNcdNodeSeenInfo()
    {
    DLTRACEIN((""));
    iClientSeenInfos.ResetAndDestroy();
    }
    
void CNcdNodeSeenInfo::AddNewIdL( const CNcdChildEntity& aChildEntity )
    {
    DLTRACEIN((""));
    AddNewIdL( aChildEntity.Identifier(), ENcdStatusNew, aChildEntity.NodeType() );
    }

void CNcdNodeSeenInfo::AddNewIdL( const CNcdNodeIdentifier& aNodeIdentifier,
    TNcdNewStatus aNewStatus, CNcdNodeFactory::TNcdNodeType aNodeType )
    {
    DLTRACEIN((""));
        
    const TUid& clientUid = aNodeIdentifier.ClientUid();
            
    if ( !IsClientInfoLoaded( clientUid ) ) 
        {
        DbLoadClientInfoL( clientUid );
        }
    
    CClientSeenInfo* info = ClientInfo( clientUid );
    DASSERT( info );

#ifdef CATALOGS_BUILD_CONFIG_DEBUG
    DLINFO(("New status array:"));
    for( TInt i = 0 ; i < info->iNodeNewStatusArray.Count() ; i++ )
        {
        DLINFO((_L("index: %d, id: %S"),i, &info->iNodeNewStatusArray[i]->Identifier().NodeId() ));
        }
#endif

    // Latest "new" nodes will be added to the top of the list and older "new" nodes will
    // drop out when max size is exceeded.
    TInt index = IndexInNewStatusArray( aNodeIdentifier );
    if( index == KErrNotFound )
        {
        DLTRACE(("Not found, create new."));
        // Create new status.
        CNcdNodeNewStatus* nodeNewStatus = CNcdNodeNewStatus::NewLC(
            aNodeIdentifier,
            aNewStatus,
            aNodeType );
        // Insert to the top of the list.
        info->iNodeNewStatusArray.InsertL( nodeNewStatus, 0 );
        CleanupStack::Pop( nodeNewStatus );
        
        }
    else
        {
        DLTRACE(("Found, update."));
        // Node is already in the new list -> move it to the top of the list.
        CNcdNodeNewStatus* nodeNewStatus = info->iNodeNewStatusArray[index];
        info->iNodeNewStatusArray.InsertL( nodeNewStatus, 0 );
        info->iNodeNewStatusArray.Remove( index + 1 );
        // Remove seen status if any
        RemoveFromSeenListL( aNodeIdentifier );
        // Update new status.
        nodeNewStatus->SetNewStatus( TNcdNewStatus(nodeNewStatus->NewStatus() | aNewStatus) );
        }
        
    // Set parent new as well.
    if( !NcdNodeIdentifierEditor::IdentifiesSomeRoot( aNodeIdentifier ) )
        {
        CNcdNodeIdentifier* parent = NcdNodeIdentifierEditor::ParentOfLC( aNodeIdentifier );
        AddNewIdL( *parent, ENcdStatusNewNodesInside, CNcdNodeFactory::ENcdNodeFolder );
        CleanupStack::PopAndDestroy( parent );
        }
    
    // Remove identifiers from the bottom if max size is exceeded.
    while( info->iNodeNewStatusArray.Count() > KNewListMaxSize )
        {
        delete info->iNodeNewStatusArray[info->iNodeNewStatusArray.Count() - 1];
        info->iNodeNewStatusArray.Remove(
            info->iNodeNewStatusArray.Count() - 1 );
        }
        
#ifdef CATALOGS_BUILD_CONFIG_DEBUG
    DLINFO(("New status array:"));
    for( TInt i = 0 ; i < info->iNodeNewStatusArray.Count() ; i++ )
        {
        DLINFO((_L("index: %d, id: %S"),i, &info->iNodeNewStatusArray[i]->Identifier().NodeId() ));
        }
#endif
    }
    

TBool CNcdNodeSeenInfo::IsSeenL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((_L("ns: %S, id: %S"), &aNodeIdentifier.NodeNameSpace(), &aNodeIdentifier.NodeId()));

    const TUid& clientUid = aNodeIdentifier.ClientUid();
    
    if ( !IsClientInfoLoaded( clientUid ) )
        {
        DbLoadClientInfoL( clientUid );
        }

    CClientSeenInfo* clientInfo = ClientInfo( aNodeIdentifier.ClientUid() );
    DASSERT( clientInfo );
    DLINFO((("clientInfo-ptr: %x"), clientInfo ));    
    
    DLINFO(("new count: %d", clientInfo->iNodeNewStatusArray.Count() ));
    TInt index = IndexInNewStatusArray( aNodeIdentifier );
    DLINFO(("index in new status array: %d", index ));
    return index == KErrNotFound;
    }
    

TBool CNcdNodeSeenInfo::ChildExistsInArrayL( const CNcdChildEntity& aChildEntity,
        const RPointerArray<CNcdChildEntity>& aChildArray,
        TBool& aNodeWithSameIndexFound )
    {
    DLTRACEIN((""));
    TBool childFound = EFalse;
    aNodeWithSameIndexFound = EFalse;
    for( TInt i = 0 ; i < aChildArray.Count() ; i++ )
        {
        if( aChildArray[i]->Identifier().Equals( aChildEntity.Identifier() ) )
            {
            childFound = ETrue;
            }
        if( aChildArray[i]->Index() == aChildEntity.Index() )
            {
            aNodeWithSameIndexFound = ETrue;
            }
        }
    DLINFO(("childFound %d, aNodeWithSameIndexFound: %d", childFound,
        aNodeWithSameIndexFound));
    return childFound;
    }

void CNcdNodeSeenInfo::RefreshFolderSeenStatusL( 
    const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((_L("aNodeIdentifier: %S"), &aNodeIdentifier.NodeId() ));
    const TUid& clientUid = aNodeIdentifier.ClientUid();
            
    if ( !IsClientInfoLoaded( clientUid ) ) 
        {
        DbLoadClientInfoL( clientUid );
        }
    
    CClientSeenInfo* info = ClientInfo( clientUid );
    DASSERT( info );
    
    TInt index = IndexInNewStatusArray( aNodeIdentifier );
    if( index == KErrNotFound )
        {
        DLTRACE(("Not in new status array"));
        return;
        }
    
    if( !FolderHasNewChildrenL( aNodeIdentifier ) )
        {
        CNcdNodeNewStatus* newStatus = info->iNodeNewStatusArray[index];
        // Don't touch folders that are really new, only folders that have just
        // new items inside.
        if( !( newStatus->NewStatus() & ENcdStatusNew ||
               newStatus->NewStatus() & ENcdStatusNewButSeen ) )
            {
            // Special case: all new children have been removed, immediately remove folder's new status
            // (better user experience when compared to just setting it seen)
            RemoveFromNewListL( aNodeIdentifier );
            RemoveFromSeenListL( aNodeIdentifier );
            // Check parent as well.
            if( !NcdNodeIdentifierEditor::IdentifiesSomeRoot( aNodeIdentifier ) )
                {
                CNcdNodeIdentifier* parent = NcdNodeIdentifierEditor::ParentOfLC( aNodeIdentifier );
                RefreshFolderSeenStatusL( *parent );
                CleanupStack::PopAndDestroy( parent );
                }
            }
        }
    }
    
void CNcdNodeSeenInfo::SetFolderSeenIfNeededL( 
    const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));
    
    TInt index = IndexInNewStatusArray( aNodeIdentifier );
    if( index == KErrNotFound || IsAddedAsSeen( aNodeIdentifier ) )
        {
        DLTRACE(("Not in new status array, or already set seen"));
        return;
        }
    
    if( !FolderHasNewUnseenChildrenL( aNodeIdentifier ) )
        {
        // Add to seen list.
        AddSeenIdL( aNodeIdentifier );
        // Check parent as well.
        if( !NcdNodeIdentifierEditor::IdentifiesSomeRoot( aNodeIdentifier ) )
            {
            CNcdNodeIdentifier* parent = NcdNodeIdentifierEditor::ParentOfLC( aNodeIdentifier );
            SetFolderSeenIfNeededL( *parent );
            CleanupStack::PopAndDestroy( parent );
            }
        }
    }
    
TBool CNcdNodeSeenInfo::FolderHasNewUnseenChildrenL(
    const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));    
    CNcdNodeFolder& folder = iNodeManager.FolderL( aNodeIdentifier );
    const RPointerArray<CNcdChildEntity>& childArray = folder.ChildArray();
    for( TInt i = 0 ; i < childArray.Count() ; i++ )
        {
        // NOTE: Children that are set seen are not considered new here.
        CNcdNodeNewStatus* nodeNewStatus = NodeNewStatus( childArray[i]->Identifier() );
        if( nodeNewStatus && !IsAddedAsSeen( childArray[i]->Identifier() ) )
            {
            DLTRACE(("New child found"));
            return ETrue;
            }
        }
    return EFalse;
    }
    
 TBool CNcdNodeSeenInfo::FolderHasNewChildrenL(
    const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));    
    CNcdNodeFolder& folder = iNodeManager.FolderL( aNodeIdentifier );
    const RPointerArray<CNcdChildEntity>& childArray = folder.ChildArray();
    for( TInt i = 0 ; i < childArray.Count() ; i++ )
        {
        // NOTE: Children that are set seen are considered new here.
        CNcdNodeNewStatus* nodeNewStatus = NodeNewStatus( childArray[i]->Identifier() );
        if( nodeNewStatus )
            {
            DLTRACE(("New child found"));
            return ETrue;
            }
        }
    return EFalse;
    }

void CNcdNodeSeenInfo::AddSeenIdL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));
    CClientSeenInfo* info = ClientInfo( aNodeIdentifier.ClientUid() );
    DASSERT( info );
    // Add to seen list.
    CNcdNodeIdentifier* copy = CNcdNodeIdentifier::NewLC( aNodeIdentifier );
    info->iSeenStructureIds.AppendL( copy );
    CleanupStack::Pop( copy );
    }

void CNcdNodeSeenInfo::SetSeenL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));
        
    const TUid& clientUid = aNodeIdentifier.ClientUid();
            
    if ( !IsClientInfoLoaded( clientUid ) ) 
        {
        DbLoadClientInfoL( clientUid );
        }
    TInt index = IndexInNewStatusArray( aNodeIdentifier );
    
    if( index == KErrNotFound || IsAddedAsSeen( aNodeIdentifier ) )
        {
        DLTRACE(("Not in new status array, or already set seen"));
        return;
        }
    
    CClientSeenInfo* info = ClientInfo( clientUid );
    DASSERT( info );
    
    CNcdNodeNewStatus* nodeNewStatus = info->iNodeNewStatusArray[index];
    if( nodeNewStatus->NodeType() == CNcdNodeFactory::ENcdNodeItem )
        {
        AddSeenIdL( aNodeIdentifier );
        }
    else if ( nodeNewStatus->NewStatus() & ENcdStatusNew &&
        nodeNewStatus->NewStatus() & ENcdStatusNewNodesInside )
        {
        nodeNewStatus->SetNewStatus(
            TNcdNewStatus( ENcdStatusNewButSeen | ENcdStatusNewNodesInside ) );
        }
    else if ( nodeNewStatus->NewStatus() & ENcdStatusNew )
        {
        AddSeenIdL( aNodeIdentifier );
        }
        
    if( !NcdNodeIdentifierEditor::IdentifiesSomeRoot( aNodeIdentifier ) )
        {
        CNcdNodeIdentifier* parent = NcdNodeIdentifierEditor::ParentOfLC( aNodeIdentifier );
        SetFolderSeenIfNeededL( *parent );
        CleanupStack::PopAndDestroy( parent );
        }
    }
    
void CNcdNodeSeenInfo::CheckFolderNewStatusL( CNcdNodeFolder& aParentFolder )
    {
    DLTRACEIN((""));
    // New checking for children can only be done if the previous
    // child array has been stored. (Not stored e.g. on first time browse and
    // after restart)
    if( aParentFolder.PreviousChildCount() != KErrNotFound )
        {
        DLTRACE(("Previous children set -> do new comparison"));
        const RPointerArray<CNcdChildEntity>& previousChildArray =
            aParentFolder.PreviousChildArray();
        CheckFolderNewStatusL( aParentFolder, previousChildArray );
        }
    }
        
void CNcdNodeSeenInfo::CheckFolderNewStatusL( CNcdNodeFolder& aParentFolder,
    const RPointerArray<CNcdChildEntity>& aPreviousChildArray )
    {
    const TUid& clientUid = aParentFolder.Identifier().ClientUid();
            
    if ( !IsClientInfoLoaded( clientUid ) ) 
        {
        DbLoadClientInfoL( clientUid );
        }
        
    CClientSeenInfo* info = ClientInfo( clientUid );
    DASSERT( info );
    
    // This flag is used to determine whether this folder contains any new items.
    TBool newChildrenFound = EFalse;
    // Check new array against the previous array
    const RPointerArray<CNcdChildEntity>& childArray =
        aParentFolder.ChildArray();
    CNcdNodeNewStatus* parentNewStatus = NodeNewStatus( aParentFolder.Identifier() );
    TBool parentIsNew = parentNewStatus &&
            ( parentNewStatus->NewStatus() & ENcdStatusNew ||
              parentNewStatus->NewStatus() & ENcdStatusNewButSeen );
    for( TInt i = 0 ; i < childArray.Count() ; i++ )
        {
        // compare with previous list
        TBool nodeWithSameIndexFound = EFalse;
        TBool childFound = ChildExistsInArrayL( *childArray[i],
            aPreviousChildArray,
            nodeWithSameIndexFound );
        
        if( parentIsNew )
            {
            DLTRACE(("Parent folder is new -> all children are new "));
            AddNewIdL( *childArray[i] );
            newChildrenFound = ETrue;
            }
        // if found -> old
        else if( childFound )
            {
            DLTRACE(("Child found -> old"));
            }
        // if not found && index < old child count && no node with this index in previous list
        //      -> old ( cannot know whether this is new or old so default to old, special case! )
        else if( !childFound
            && childArray[i]->Index() < aParentFolder.PreviousChildCount()
            && !nodeWithSameIndexFound )
            {
            DLTRACE(("Child not found and index < previous child count && node with same index not found -> default to old"));
            }
        // if not found && index < old child count && node exists with this index in previous list
        //      -> new ( the "usual" new case )
        else if( !childFound
             && childArray[i]->Index() < aParentFolder.PreviousChildCount()
             && nodeWithSameIndexFound )
            {
            DLTRACE(("New"));
            AddNewIdL( *childArray[i] );
            newChildrenFound = ETrue;
            }
        // if not found && index > old child count -> new 
        else if( !childFound
            && childArray[i]->Index() >= aParentFolder.PreviousChildCount() )
            {
            DLTRACE(("New"));
            AddNewIdL( *childArray[i] );
            newChildrenFound = ETrue;
            }
            
        // If no new children are found before, check that is this node previously new
        if( !newChildrenFound )
            {
            DLTRACE(("Check if already set new"));
            // Must be in new array and not in seen array to be new
            // (internally new, i.e. new even after commit).
            CNcdNodeNewStatus* nodeNewStatus = NodeNewStatus(
                childArray[i]->Identifier() );
            if( nodeNewStatus )
                {
                newChildrenFound = ETrue;
                }
            }
        }
        
    if( !newChildrenFound && parentIsNew )
        {
        // Special case: all new items have been removed, immediately remove parent's new status
        // (better user experience when compared to just setting it seen)
        RemoveFromNewListL( aParentFolder.Identifier() );
        }
    }   

void CNcdNodeSeenInfo::CheckChildNewStatusL( CNcdNodeFolder& aParentFolder,
    TInt aChildIndex )
    {
    DLTRACEIN((""));
    const TUid& clientUid = aParentFolder.Identifier().ClientUid();
            
    if ( !IsClientInfoLoaded( clientUid ) ) 
        {
        DbLoadClientInfoL( clientUid );
        }
    
    CNcdNodeNewStatus* parentNewStatus = NodeNewStatus( aParentFolder.Identifier() );
    TBool parentIsNew = parentNewStatus &&
            ( parentNewStatus->NewStatus() & ENcdStatusNew ||
              parentNewStatus->NewStatus() & ENcdStatusNewButSeen );
    if( parentIsNew )
        {
        DLTRACE(("Parent is new, set child new as well"));
        AddNewIdL( aParentFolder.ChildEntityByServerIndexL( aChildIndex ) );
        }
    else if( aParentFolder.PreviousChildCount() != KErrNotFound )
        {
        DLTRACE(("Previous child list set, do comparison."));
        const CNcdChildEntity& child = aParentFolder.ChildEntityByServerIndexL( aChildIndex );
        // compare with previous list
        TBool nodeWithSameIndexFound = EFalse;
        TBool childFound = ChildExistsInArrayL( child,
            aParentFolder.PreviousChildArray(),
            nodeWithSameIndexFound );

        // if found -> old
        if( childFound )
            {
            DLTRACE(("Child found -> old"));
            }
        // if not found && index < old child count && no node with this index in previous list
        //      -> old ( cannot know whether this is new or old so default to old, special case! )
        else if( !childFound
            && child.Index() < aParentFolder.PreviousChildCount()
            && !nodeWithSameIndexFound )
            {
            DLTRACE(("Child not found and index < previous child count && node with same index not found -> default to old"));
            }
        // if not found && index < old child count && node exists with this index in previous list
        //      -> new ( the "usual" new case )
        else if( !childFound
             && child.Index() < aParentFolder.PreviousChildCount()
             && nodeWithSameIndexFound )
            {
            DLTRACE(("New"));
            AddNewIdL( child );
            }
        // if not found && index > old child count -> new 
        else if( !childFound
            && child.Index() >= aParentFolder.PreviousChildCount() )
            {
            DLTRACE(("New"));
            AddNewIdL( child );
            }
        }
    }
    
void CNcdNodeSeenInfo::ClearInfoL( const TUid& aClientUid )
    {
    DLTRACEIN((""));
    if ( !IsClientInfoLoaded( aClientUid ) ) 
        {
        DbLoadClientInfoL( aClientUid );
        }
    CClientSeenInfo* info = ClientInfo( aClientUid );
    DASSERT( info );
    info->iNodeNewStatusArray.ResetAndDestroy();
    info->iSeenStructureIds.ResetAndDestroy();
    }

void CNcdNodeSeenInfo::CommitChangesL( const TUid& aClientUid ) 
    {
    DLTRACEIN((""));
    
    CClientSeenInfo* info = ClientInfo( aClientUid );
    if ( !info )
        {
        // No changes made, return;
        return;
        }
    
    for( TInt i = info->iNodeNewStatusArray.Count() - 1 ; i >= 0 ; i-- )
        {
        CNcdNodeNewStatus* nodeNewStatus = info->iNodeNewStatusArray[i];
        if( nodeNewStatus->NewStatus() & ENcdStatusNewButSeen )
            {
            nodeNewStatus->SetNewStatus( 
                TNcdNewStatus(nodeNewStatus->NewStatus() ^ ENcdStatusNewButSeen ) );
            }
        }
    
    TInt count = info->iSeenStructureIds.Count();
    // Remove the seen identifiers from the new list.
    while ( count-- ) 
        {
        RemoveFromNewListL( *info->iSeenStructureIds[ count ] );
        delete info->iSeenStructureIds[ count];
        // Removing here in case RemoveFromNewListL leaves
        info->iSeenStructureIds.Remove( count );
        }
        
    DASSERT( info->iSeenStructureIds.Count() == 0 );
    // Save the changes to db.
    DbSaveClientInfoL( aClientUid );
    }

 void CNcdNodeSeenInfo::CreatePreviousListsForChildrenL(
    CNcdNodeFolder& aParentFolder,
    RPointerArray<CNcdChildEntityMap>& aChildEntityMaps )
    {
    DLTRACEIN((""));
    for( TInt i = 0 ; i < aParentFolder.ChildArray().Count() ; i++ )
        {
        if( aParentFolder.ChildArray()[i]->NodeType() ==
            CNcdNodeFactory::ENcdNodeFolder )
            {
            CNcdNodeFolder* child = NULL;
            TRAPD(err, child = &iNodeManager.FolderL(
                aParentFolder.ChildArray()[i]->Identifier() ) ); //TRAPD
            // Special handling for bundles
            if( err != KErrNone )
                {
                DLTRACE(("Child not found from cache or db."));
                continue;
                }
            TInt childCount = 0;
            if( CNcdNodeFactory::NodePurposeL( *child ) ==
                CNcdNodeFactory::ENcdBundleNode && 
                child->ChildrenPreviouslyLoaded() )
                {
                DLTRACE((""))
                CreatePreviousListsForChildrenL( *child, aChildEntityMaps );
                childCount = child->ChildCount();
                }
            else
                {
                childCount = child->FolderLinkL().ExpectedChildrenCount();
                }
            // Store only if previously loaded.
            if( child->ChildrenPreviouslyLoaded() )
                {
                DLTRACEIN((_L("Creating previous list for: %S"),
                    &child->Identifier().NodeId() ));
                CNcdChildEntityMap* childEntityMap =
                    CNcdChildEntityMap::NewLC(
                        child->Identifier(),
                        child->ChildArray(),
                        childCount );
                aChildEntityMaps.AppendL( childEntityMap );
                CleanupStack::Pop( childEntityMap );
                }
            }
        }
    }

void CNcdNodeSeenInfo::StorePreviousListsToExistingChildrenL(
    CNcdNodeFolder& aParentFolder,
    const RPointerArray<CNcdChildEntityMap>& aChildEntityMaps )
    {
    DLTRACEIN((""));
    const RPointerArray<CNcdChildEntity>& children = aParentFolder.ChildArray();
    for( TInt i = 0 ; i < children.Count() ; i++ )
        {
        CNcdNodeFolder* folder = NULL;
        TRAPD(err, folder =
            &iNodeManager.FolderL( children[i]->Identifier() ) ); //TRAPD
        if( err != KErrNone )
            {
            DLTRACE(("Child not found from cache or db."));
            continue;
            }
        if( CNcdNodeFactory::NodePurposeL( 
            *folder ) == CNcdNodeFactory::ENcdBundleNode )
            {
            DLTRACE(("Child is bundle -> store it's children's previous lists."));
            StorePreviousListsToExistingChildrenL( *folder, aChildEntityMaps );
            }
            
        for( TInt j = 0 ; j < aChildEntityMaps.Count() ; j++ )
            {
            // Should remove child entity map when matching child is found.
            const CNcdChildEntityMap* previousChildEntityMap = aChildEntityMaps[j];
            if( previousChildEntityMap->ParentIdentifier().Equals( 
                folder->Identifier() ) )
                {
                DLTRACE(("Matching child found, store it's previous list."));
                folder->StoreChildrenToPreviousListL( 
                    previousChildEntityMap->ChildArray(),
                    previousChildEntityMap->ChildCount() );
                break;
                }
            }
        }
    }

void CNcdNodeSeenInfo::DoNewCheckForTransparentChildrenL(
    CNcdNodeFolder& aParentFolder )
    {
    DLTRACEIN((""));
    const RPointerArray<CNcdChildEntity>& children = aParentFolder.ChildArray();
    for( TInt i = 0 ; i < children.Count() ; i++ )
        {
        if( children[i]->NodeType() == CNcdNodeFactory::ENcdNodeFolder
            && children[i]->IsTransparent() )
            {
            CNcdNodeFolder* folder = NULL;
            TRAPD(err, folder =
                &iNodeManager.FolderL( children[i]->Identifier() ) ); //TRAPD
            if( err == KErrNone )
                {
                CNcdNodeNewStatus* parentNewStatus = NodeNewStatus( children[i]->Identifier() );
                TBool parentIsNew = parentNewStatus &&
                    ( parentNewStatus->NewStatus() & ENcdStatusNew ||
                      parentNewStatus->NewStatus() & ENcdStatusNewButSeen );
                if( parentIsNew )
                    {
                    SetChildrenNewL( *folder );
                    }
                else
                    {
                    CheckFolderNewStatusL( *folder );
                    }
                }
            }
        }
    }

CNcdNodeSeenInfo::CClientSeenInfo* CNcdNodeSeenInfo::ClientInfo(
    const TUid& aClientUid ) const 
    {
    for ( TInt i = 0; i < iClientSeenInfos.Count(); i++ ) 
        {
        if ( iClientSeenInfos[ i ]->iClientUid == aClientUid ) 
            {
            return iClientSeenInfos[ i ];
            }
        }
    return NULL;
    }
    

TInt CNcdNodeSeenInfo::IndexInSeenArray( const CNcdNodeIdentifier& aNodeIdentifier ) const
    {
    DLTRACEIN((""));
    CClientSeenInfo* info = ClientInfo( aNodeIdentifier.ClientUid() );
    DASSERT( info );
    return NcdNodeIdentifierUtils::IdentifierIndex(
        aNodeIdentifier, info->iSeenStructureIds );
    }
    
TBool CNcdNodeSeenInfo::IsAddedAsSeen( const CNcdNodeIdentifier& aNodeIdentifier ) const
    {
    DLTRACEIN((""));
    return IndexInSeenArray( aNodeIdentifier ) != KErrNotFound;
    }
    
TInt CNcdNodeSeenInfo::IndexInNewStatusArray( const CNcdNodeIdentifier& aNodeIdentifier ) const
    {
    DLTRACEIN((""));
    CClientSeenInfo* info = ClientInfo( aNodeIdentifier.ClientUid() );
    DASSERT( info );
    TInt index = KErrNotFound;
    for( TInt i = 0 ; i < info->iNodeNewStatusArray.Count() ; i++ )
        {
        if( info->iNodeNewStatusArray[i]->Identifier().Equals( aNodeIdentifier ) )
            {
            index = i;
            break;
            }
        }
    return index;
    }
    
CNcdNodeSeenInfo::CNcdNodeNewStatus* CNcdNodeSeenInfo::NodeNewStatus( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));
    CNcdNodeNewStatus* nodeNewStatus = NULL;
    TInt index = IndexInNewStatusArray( aNodeIdentifier );
    if( index != KErrNotFound )
        {
        CClientSeenInfo* info = ClientInfo( aNodeIdentifier.ClientUid() );
        DASSERT( info );
        nodeNewStatus = info->iNodeNewStatusArray[index];
        }
    return nodeNewStatus;
    }
    
void CNcdNodeSeenInfo::RemoveFromSeenListL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));
    const TUid& clientUid = aNodeIdentifier.ClientUid();
            
    if ( !IsClientInfoLoaded( clientUid ) ) 
        {
        DbLoadClientInfoL( clientUid );
        }
        
    CClientSeenInfo* info = ClientInfo( clientUid );
    DASSERT( info );
    
    TInt index = NcdNodeIdentifierUtils::IdentifierIndex(
        aNodeIdentifier, info->iSeenStructureIds );
    if( index != KErrNotFound )
        {
        delete info->iSeenStructureIds[index];
        info->iSeenStructureIds.Remove( index );
        }
    }

void CNcdNodeSeenInfo::RemoveFromNewListL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));
    const TUid& clientUid = aNodeIdentifier.ClientUid();
            
    if ( !IsClientInfoLoaded( clientUid ) ) 
        {
        DbLoadClientInfoL( clientUid );
        }
        
    CClientSeenInfo* info = ClientInfo( clientUid );
    DASSERT( info );
    
    TInt index = IndexInNewStatusArray( aNodeIdentifier );
    if( index != KErrNotFound )
        {
        DLTRACE((_L("Removing from new status array, id: %S"), &aNodeIdentifier.NodeId() ));
        delete info->iNodeNewStatusArray[index];
        info->iNodeNewStatusArray.Remove( index );
        }
    }

TBool CNcdNodeSeenInfo::IsClientInfoLoaded( const TUid& aClientUid ) const
    {
    DLTRACEIN((""));
    return ClientInfo( aClientUid ) != NULL;
    }
    

void CNcdNodeSeenInfo::RemoveClientInfo( const TUid& aClientUid )
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iClientSeenInfos.Count(); i++ ) 
        {
        if ( iClientSeenInfos[ i ]->iClientUid == aClientUid ) 
            {
            delete iClientSeenInfos[ i ];
            iClientSeenInfos.Remove( i );
            break;
            }
        }
    }       
    

void CNcdNodeSeenInfo::DbLoadClientInfoL( const TUid& aClientUid )
    {
    DLTRACEIN((""));
    
    RemoveClientInfo( aClientUid );
    
    MNcdStorage& providerStorage = iStorageManager.ProviderStorageL(
        iGeneralManager.FamilyName() );
    MNcdDatabaseStorage& database =
        providerStorage.DatabaseStorageL( NcdProviderDefines::KDefaultDatabaseUid );
        
    if( !database.ItemExistsInStorageL( aClientUid.Name(), 
        NcdProviderDefines::ENcdNodeSeenInfo ) ) 
        {
        // Create empty client info object.
        CClientSeenInfo* empty = new( ELeave ) CClientSeenInfo( aClientUid );
        CleanupStack::PushL( empty );
        iClientSeenInfos.AppendL( empty );
        CleanupStack::Pop( empty );
        }
    else 
        {
        // Database contains the data, load it.
        // Get the storage item from which the data is loaded
        // Note: database has the ownership of the item
        MNcdStorageItem* item = database.StorageItemL(
            aClientUid.Name(), NcdProviderDefines::ENcdNodeSeenInfo );    
        
        // Get data from database by using CClientSeenInfo as the target so that 
        // internalize will be called for it
        CClientSeenInfo* data = new( ELeave ) CClientSeenInfo( aClientUid );
        CleanupStack::PushL( data );
        item->SetDataItem( data );
    
        // Read data -> calls CClientSeenInfo::InternalizeL
        item->ReadDataL();
        
        iClientSeenInfos.AppendL( data );
        CleanupStack::Pop( data );
        }
    }
    
    
void CNcdNodeSeenInfo::DbSaveClientInfoL( const TUid& aClientUid )
    {
    DLTRACEIN((""));
    
    CClientSeenInfo* info = ClientInfo( aClientUid );
    if ( !info ) 
        {
        return;
        }
        
    MNcdStorage& providerStorage = iStorageManager.ProviderStorageL(
        iGeneralManager.FamilyName() );
    MNcdDatabaseStorage& database =
        providerStorage.DatabaseStorageL( NcdProviderDefines::KDefaultDatabaseUid );
        
    // Get the storage item to which the client seen info is stored
    // Note: database has the ownership of the item
    MNcdStorageItem* item = database.StorageItemL(
        aClientUid.Name(), NcdProviderDefines::ENcdNodeSeenInfo );
    item->SetDataItem( info );
    item->OpenL();
        
    // Calls ExternalizeL for this
    item->WriteDataL();
    item->SaveL();
    }
    
void CNcdNodeSeenInfo::SetChildrenNewL( CNcdNodeFolder& aParentFolder )
    {
    DLTRACEIN((""));
    const RPointerArray<CNcdChildEntity>& children = aParentFolder.ChildArray();
    for( TInt i = 0 ; i < children.Count() ; i++ )
        {
        AddNewIdL( *children[i] );
        RemoveFromSeenListL( children[i]->Identifier() );
        }
    }
        
    
// ----------------------------------------------------------------------------------------------
// CNcdNodeSeenInfo::CClientInfo
// ----------------------------------------------------------------------------------------------
//

CNcdNodeSeenInfo::CClientSeenInfo::CClientSeenInfo( const TUid& aClientUid ) :
    iClientUid( aClientUid ) 
    {
    }
    
CNcdNodeSeenInfo::CClientSeenInfo::~CClientSeenInfo()
    {
    iNodeNewStatusArray.ResetAndDestroy();
    iSeenStructureIds.ResetAndDestroy();
    }
    

// From MNcdStorageDataItem

void CNcdNodeSeenInfo::CClientSeenInfo::ExternalizeL( RWriteStream& aStream ) 
    {
    DLTRACEIN((""));
    
    // Write uid.
    aStream.WriteInt32L( iClientUid.iUid );

    // Write new structure ids.
    TInt count = iNodeNewStatusArray.Count();
    aStream.WriteInt32L( count );
    for ( TInt i = 0; i < count; i++ ) 
        {
        iNodeNewStatusArray[ i ]->ExternalizeL( aStream );
        }
    }


void CNcdNodeSeenInfo::CClientSeenInfo::InternalizeL( RReadStream& aStream ) 
    {
    DLTRACEIN((""));
    iNodeNewStatusArray.ResetAndDestroy();
    
    // Read uid.
    iClientUid = TUid::Uid( aStream.ReadInt32L() );
    
    // Read new structure ids.
    TInt count = aStream.ReadInt32L();
    for ( TInt i = 0; i < count; i++ ) 
        {
        CNcdNodeNewStatus* identifier = CNcdNodeNewStatus::NewLC( aStream );
        iNodeNewStatusArray.AppendL( identifier );
        CleanupStack::Pop( identifier );
        }
    }

CNcdNodeSeenInfo::CNcdNodeNewStatus* CNcdNodeSeenInfo::CNcdNodeNewStatus::NewL(
    const CNcdNodeIdentifier& aNodeIdentifier,
    TNcdNewStatus aNewStatus,
    CNcdNodeFactory::TNcdNodeType aNodeType )
    {
    DLTRACEIN((""));
    CNcdNodeNewStatus* self = CNcdNodeNewStatus::NewLC( aNodeIdentifier,
        aNewStatus,
        aNodeType );
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdNodeSeenInfo::CNcdNodeNewStatus* CNcdNodeSeenInfo::CNcdNodeNewStatus::NewLC(
    const CNcdNodeIdentifier& aNodeIdentifier,
    TNcdNewStatus aNewStatus,
    CNcdNodeFactory::TNcdNodeType aNodeType )
    {
    DLTRACEIN((""));
    CNcdNodeNewStatus* self = new ( ELeave ) CNcdNodeNewStatus( aNewStatus,
        aNodeType );
    CleanupStack::PushL( self );
    self->ConstructL( aNodeIdentifier );
    return self;
    }
    
CNcdNodeSeenInfo::CNcdNodeNewStatus* CNcdNodeSeenInfo::CNcdNodeNewStatus::NewL(
    RReadStream& aReadStream )
    {
    DLTRACEIN((""));
    CNcdNodeNewStatus* self = CNcdNodeNewStatus::NewLC( aReadStream );
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdNodeSeenInfo::CNcdNodeNewStatus* CNcdNodeSeenInfo::CNcdNodeNewStatus::NewLC(
    RReadStream& aReadStream )
    {
    DLTRACEIN((""));
    CNcdNodeNewStatus* self = new ( ELeave ) CNcdNodeNewStatus();
    CleanupStack::PushL( self );
    self->InternalizeL( aReadStream );
    return self;
    }
    
CNcdNodeSeenInfo::CNcdNodeNewStatus::~CNcdNodeNewStatus()
    {
    DLTRACEIN((""));
    delete iNodeIdentifier;
    }

const CNcdNodeIdentifier& CNcdNodeSeenInfo::CNcdNodeNewStatus::Identifier() const
    {
    return *iNodeIdentifier;
    }

TNcdNewStatus CNcdNodeSeenInfo::CNcdNodeNewStatus::NewStatus() const
    {
    return iNewStatus;
    }
    
void CNcdNodeSeenInfo::CNcdNodeNewStatus::SetNewStatus( TNcdNewStatus aNewStatus )
    {
    DLTRACEIN((""));
    iNewStatus = aNewStatus;
    }

CNcdNodeFactory::TNcdNodeType CNcdNodeSeenInfo::CNcdNodeNewStatus::NodeType() const
    {
    return iNodeType;
    }

void CNcdNodeSeenInfo::CNcdNodeNewStatus::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));
    iNodeIdentifier->ExternalizeL( aStream );
    aStream.WriteInt32L( iNewStatus );
    aStream.WriteInt32L( iNodeType );
    }


void CNcdNodeSeenInfo::CNcdNodeNewStatus::InternalizeL( RReadStream& aStream ) 
    {
    DLTRACEIN((""));
    CNcdNodeIdentifier* identifier = CNcdNodeIdentifier::NewL( aStream );
    delete iNodeIdentifier;
    iNodeIdentifier = identifier;
    iNewStatus = static_cast<TNcdNewStatus>( aStream.ReadInt32L() );
    iNodeType = static_cast<CNcdNodeFactory::TNcdNodeType>( aStream.ReadInt32L() );
    }

CNcdNodeSeenInfo::CNcdNodeNewStatus::CNcdNodeNewStatus( TNcdNewStatus aNewStatus,
    CNcdNodeFactory::TNcdNodeType aNodeType )
    : iNewStatus( aNewStatus ), iNodeType( aNodeType )
    {
    }
    
CNcdNodeSeenInfo::CNcdNodeNewStatus::CNcdNodeNewStatus()
    {
    }
 
void CNcdNodeSeenInfo::CNcdNodeNewStatus::ConstructL(
    const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));
    iNodeIdentifier = CNcdNodeIdentifier::NewL( aNodeIdentifier );
    }
