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


#ifndef C_NCDNODESEENINFO_H
#define C_NCDNODESEENINFO_H

#include <e32base.h>
#include "ncdstoragedataitem.h"
#include "ncdnodefactory.h"

// Defines the maximum amount of "new" idenfifiers. If
// this value is exceeded then older identifiers will be
// removed from the bottom of the "new" list.
const TInt KNewListMaxSize(50);

class CNcdNodeIdentifier;
class MNcdStorageManager;
class CNcdNodeManager;
class CNcdNodeFolder;
class CNcdChildEntity;
class CNcdChildEntityMap;
class CNcdGeneralManager;

enum TNcdNewStatus
    {
    /** Node itself is new */
    ENcdStatusNew = 0x00000001,
    /** Node itself is new */
    ENcdStatusNewButSeen = 0x00000010,
    /** Folder contains new items */
    ENcdStatusNewNodesInside = 0x00000100
    };

/**
 * Stores information about new nodes.
 */
class CNcdNodeSeenInfo : public CBase
    {
public:
    static CNcdNodeSeenInfo* NewL( CNcdGeneralManager& aGeneralManager );
         
    static CNcdNodeSeenInfo* NewLC( CNcdGeneralManager& aGeneralManager );
         
    ~CNcdNodeSeenInfo();
    
    /**
     * Adds a structure identifier to the new list if it does not exist yet.
     * If it does exist, it will be moved to the top of the list.
     *
     * @param aStructureIdentifier The identifier.
     * @param aNewStatus New status of the node.
     */
    void AddNewIdL( const CNcdNodeIdentifier& aStructureIdentifier,
        TNcdNewStatus aNewStatus, CNcdNodeFactory::TNcdNodeType aNodeType );
        
    void AddNewIdL( const CNcdChildEntity& aChildEntity );
    
    /**
     * Tells whether the given structure node is seen.
     *
     * @param aStructureIdentifier The structure node identifier.
     * @return ETrue if the node is seen, otherwise EFalse.
     */
    TBool IsSeenL( const CNcdNodeIdentifier& aStructureIdentifier );
    
    /**
     * Sets the given structure node as seen. Affects only after
     * CommitChangesL.
     *
     * @param aStructureIdentifier The structure node identifier.
     */
    void SetSeenL( const CNcdNodeIdentifier& aStructureIdentifier );
    
    
    /**
     * Checks the new statuses of a folder's children against the previous children.
     * The folder's and it's parent hierarchy's new statuses are set if needed.
     * The previous array is fetched from the folder.
     *
     * @param aParentFolder The parent folder to check.
     */
    void CheckFolderNewStatusL( CNcdNodeFolder& aParentFolder );
    
    /**
     * Checks the new statuses of a folder's children against the previous children.
     * The folder's and it's parent hierarchy's new statuses are set if needed.
     * The previous array is fetched from the folder.
     *
     * @param aParentFolder The parent folder to check.
     * @param aPreviousChildArray The previous children to check against.
     */
    void CheckFolderNewStatusL(  CNcdNodeFolder& aParentFolder,
        const RPointerArray<CNcdChildEntity>& aPreviousChildArray );
    
    /**
     * Checks the new status of a child node against the parent folder's
     * previous children. The parent hierarchy's new statuses are set if needed.
     *
     * @param aParentFolder The parent folder of the node.
     * @param aChildIndex The index of the child node to check.
     */
    void CheckChildNewStatusL( CNcdNodeFolder& aParentFolder, TInt aChildIndex );
    
    
    /**
     * Clears the new list completely.
     *
     * @param aClient Uid of the client whose list will be cleared.
     */
    void ClearInfoL( const TUid& aClientUid );
    
    /**
     * Commits the changes made by SetSeenL (the seen nodes are be removed from the new list)
     * and saves seen info to the database storage.
     *
     * @param aClient Uid of the client whose changes are committed.
     */
    void CommitChangesL( const TUid& aClientUid );
    
     /**
     * Creates lists of previous children for the children of the given folder.
     *
     * @note Only folders with type CNcdNodeFactory::ENcdNodeFolder are handled
     * (e.g. bundles are not handled).
     * @param aParentFolder The folder.
     * @param aChildEntityMaps Will contain a child entity map for each child on return.
     */
    void CreatePreviousListsForChildrenL( CNcdNodeFolder& aParentFolder,
        RPointerArray<CNcdChildEntityMap>& aChildEntityMaps );
    
    /**
     * Stoeres previous child lists for the existing child folders of the given folder.
     *
     * @param aParentFolder The folder.
     * @param aChildEntityMaps Child entity maps containing the previous lists.
     */
    void StorePreviousListsToExistingChildrenL( CNcdNodeFolder& aParentFolder,
        const RPointerArray<CNcdChildEntityMap>& aChildEntityMaps );
    
    /**
     * Does a new status check for the transparent children of the given folder.
     *
     * @param aParentFolder The folder.
     */
    void DoNewCheckForTransparentChildrenL( CNcdNodeFolder& aParentFolder );

    /**
     * Immediately removes new id for the folder if no new children exist.
     */
    void RefreshFolderSeenStatusL( const CNcdNodeIdentifier& aNodeIdentifier );

private:
    class CClientSeenInfo;
    class CNcdNodeNewStatus;

    CNcdNodeSeenInfo( CNcdGeneralManager& aGeneralManager );
    void ConstructL();
    
    /**
     * Returns the client's client info, if one exists.
     *
     * @param aClientUid The uid of the client.
     * @return The client info, or NULL. Ownership is not transferred.
     */
    CClientSeenInfo* ClientInfo( const TUid& aClientUid ) const;
    
    /**
     * Checks that is the given identifier in the new status array.
     *
     * @param aNodeIdentifier The identifier to search for.
     * @return The index of the given identifier in the array if found,
     * otherwise KErrNotFound.
     */
    TInt IndexInNewStatusArray( const CNcdNodeIdentifier& aNodeIdentifier ) const;
    
    /**
     * Getter for node new status.
     *
     * @param aNodeIdentifier The identifier to search for.
     * @return New status for the node, NULL if not found.
     */
    CNcdNodeNewStatus* NodeNewStatus( const CNcdNodeIdentifier& aNodeIdentifier );
    TInt IndexInSeenArray( const CNcdNodeIdentifier& aNodeIdentifier ) const;    
    TBool IsAddedAsSeen( const CNcdNodeIdentifier& aNodeIdentifier ) const;
    
    void RemoveFromSeenListL( const CNcdNodeIdentifier& aNodeIdentifier );
    void RemoveFromNewListL( const CNcdNodeIdentifier& aNodeIdentifier );
    
    TBool IsClientInfoLoaded( const TUid& aClientUid ) const;
    void RemoveClientInfo( const TUid& aClientUid );
    
    void DbLoadClientInfoL( const TUid& aClientUid );
    void DbSaveClientInfoL( const TUid& aClientUid );
  
    /**
     * Removes the given structure identifier from the new list.
     *
     * @param aStructureIdentifier The structure node identifier.
     */    
    void RemoveNewIdL( const CNcdNodeIdentifier& aStructureIdentifier );
    
    /**
     * Checks that does the given child entity exist in the given array.
     * Comparison is done with identifier only.
     *
     * @param aChildEntity The child to search for.
     * @param aChildArray The array to search from.
     * @param aNodeWithSameIndexFound Set to ETrue if there is a
     * node in aChildArray with the same index as aChildEntity has. NOTE:
     * Since this is not needed if node is found the searching will stop and
     * this value may not have been set correctly! Only use if node not found!
     * @return ETrue if a child with the same identifier is found in the array.
     */
    TBool ChildExistsInArrayL( const CNcdChildEntity& aChildEntity,
        const RPointerArray<CNcdChildEntity>& aChildArray,
        TBool& aNodeWithSameIndexFound );
        
    /**
     * Checks whether a folder has new children that are have not been set seen.
     * Check is made against current new list only, no comparison against
     * previous child array is made.
     */
    TBool FolderHasNewUnseenChildrenL( const CNcdNodeIdentifier& aNodeIdentifier );
    
    /**
     * Checks whether a folder has new children. Being set seen has no effect here.
     * Check is made against current new list only, no comparison against
     * previous child array is made.
     */
    TBool FolderHasNewChildrenL( const CNcdNodeIdentifier& aNodeIdentifier );
        
    /**
     * Adds a seen id.
     */
    void AddSeenIdL( const CNcdNodeIdentifier& aNodeIdentifier );
    
    /**
     * Sets a folder seen if it no longer has new child items.
     */
    void SetFolderSeenIfNeededL( const CNcdNodeIdentifier& aNodeIdentifier );
    
    /**
     * Sets all children of the folder new.
     */
    void SetChildrenNewL( CNcdNodeFolder& aParentFolder );
        
private:

    class CClientSeenInfo : public CBase,
                            public MNcdStorageDataItem
        {
    public:
        CClientSeenInfo( const TUid& aClientUid );
        ~CClientSeenInfo();
    
    public: // from MNcdStorageDataItem

        /* 
        * @see MNcdStorageDataItem::ExternalizeL
        */
        virtual void ExternalizeL( RWriteStream& aStream );

        /* 
        * @see MNcdStorageDataItem::InternalizeL
        */
        virtual void InternalizeL( RReadStream& aStream );
            
    public: // member variables
        
        // List of node new statuses.
        RPointerArray<CNcdNodeNewStatus> iNodeNewStatusArray;
        // List of seen nodes.
        RPointerArray<CNcdNodeIdentifier> iSeenStructureIds;
        TUid iClientUid;
        };
    
    /**
     * Node new status.
     */
    class CNcdNodeNewStatus : public CBase
        {
    public:
        static CNcdNodeNewStatus* NewL( const CNcdNodeIdentifier& aNodeIdentifier,
            TNcdNewStatus aNewStatus, CNcdNodeFactory::TNcdNodeType aNodeType );
        static CNcdNodeNewStatus* NewLC( const CNcdNodeIdentifier& aNodeIdentifier,
            TNcdNewStatus aNewStatus, CNcdNodeFactory::TNcdNodeType iNodeType );
        static CNcdNodeNewStatus* NewL( RReadStream& aReadStream );
        static CNcdNodeNewStatus* NewLC( RReadStream& aReadStream );
        ~CNcdNodeNewStatus();
    public:
        const CNcdNodeIdentifier& Identifier() const;
        TNcdNewStatus NewStatus() const;
        void SetNewStatus( TNcdNewStatus aNewStatus );
        CNcdNodeFactory::TNcdNodeType NodeType() const;
        void ExternalizeL( RWriteStream& aStream );
        void InternalizeL( RReadStream& aStream );
        
    private:
        CNcdNodeNewStatus( TNcdNewStatus aNewStatus,
            CNcdNodeFactory::TNcdNodeType aNodeType );
        CNcdNodeNewStatus();
        void ConstructL( const CNcdNodeIdentifier& aNodeIdentifier );
    
    private: //Data
        CNcdNodeIdentifier* iNodeIdentifier;
        TNcdNewStatus iNewStatus;
        CNcdNodeFactory::TNcdNodeType iNodeType;
        };
    
    CNcdGeneralManager& iGeneralManager;
    
    RPointerArray<CClientSeenInfo> iClientSeenInfos;
    
    // Storage manager.
    MNcdStorageManager& iStorageManager;
    
    // Node manager.
    CNcdNodeManager& iNodeManager;       
    
    };

#endif // C_NCDNODESEENINFO_H

