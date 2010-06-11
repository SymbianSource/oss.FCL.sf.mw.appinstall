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
* Description:   Contains CNcdNodeFolder class
*
*/


#ifndef NCD_NODE_FOLDER_H
#define NCD_NODE_FOLDER_H


#include <e32cmn.h>

#include "ncdnodeimpl.h"

class CNcdNodeIdentifier;
class CNcdChildEntity;
class CNcdNodeFolderLink;
class CNcdNodeSeenFolder;


/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeFolder : public CNcdNode
    {
    
public:

    /**
     * NewL
     *
     * @return CNcdNodeFolder* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeFolder* NewL( CNcdNodeManager& aNodeManager,
                                 const CNcdNodeIdentifier& aIdentifier );
    
    /**
     * NewLC
     *
     * @return CNcdNodeFolder* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeFolder* NewLC( CNcdNodeManager& aNodeManager,
                                  const CNcdNodeIdentifier& aIdentifier );


    /**
     * Destructor
     */
    virtual ~CNcdNodeFolder();


    /** 
     * Getter for child count.
     * 
     * @note This is the count of children in the child array, not the expected
     * child count that comes from server!
     */
    virtual TInt ChildCount() const;

    /**
     * Returns the child node by the given index.
     *
     * @note Returns the available children, can be indexed against ChildCount
     */
    virtual const CNcdNodeIdentifier& ChildL( TInt aIndex ) const;
    
    /**
     * Returns the whole child array.
     */
    const RPointerArray<CNcdChildEntity>& ChildArray() const;
    
    /** 
     * Getter for server child count.
     * 
     * Uses CNcdNodeFolderLink::ExpectedChildrenCount
     *
     * @exception KErrNotFound if link not set.
     */
    virtual TInt ServerChildCountL() const;
    
    /** 
     * Getter for child identifier by server child index.
     *
     * Indexed against ServerChildCountL. Some children may be unavailable
     * because of paging, so this function should be used with care.
     *
     * @param aIndex The index of the child (i.e. index on server).
     * @return Child identifier for the given index.
     * @exception KErrNotFound if the child entity is not found.
     */
    virtual const CNcdNodeIdentifier& ChildByServerIndexL( TInt aIndex ) const;
    
    /** 
     * Getter for child entity by server child index.
     *
     * Indexed against ServerChildCountL. Some children may be unavailable
     * because of paging, so this function should be used with care.
     *
     * @param aIndex The index of the child (i.e. index on server).
     * @return Child entity for the given index.
     * @exception KErrNotFound if the child entity is not found.
     */
    virtual const CNcdChildEntity& ChildEntityByServerIndexL( TInt aIndex ) const;
    
    
    
    /**
     * Adds a child to this folder if not present already.
     *
     * @param aNodeIdentifier Identifier of the node.
     * @param aIndex Index of the child node (i.e. index of the actual node on server)
     * @param aTransparent Defines whether this child is transparent.
     * @param aNodeType Type of the child node (folder, item etc.).
     * @return ETrue if the child was added. Else EFalse (children
     * are inserted only once).
     */
    virtual TBool ReplaceChildL( 
        const CNcdNodeIdentifier& aNodeIdentifier,
        TInt aIndex,
        TBool aTransparent,
        CNcdNodeFactory::TNcdNodeType aNodeType );
    
    virtual TBool InsertChildL( 
        const CNcdNodeIdentifier& aNodeIdentifier,
        TInt aIndex,
        TBool aTransparent,
        CNcdNodeFactory::TNcdNodeType aNodeType );
        
    virtual TBool AppendChildL( 
        const CNcdNodeIdentifier& aNodeIdentifier,
        TBool aTransparent,
        CNcdNodeFactory::TNcdNodeType aNodeType );
        
    CNcdNodeFolderLink& FolderLinkL() const;

    CNcdNodeSeenFolder& NodeSeenFolder() const;

    void RemoveChild( const CNcdNodeIdentifier& aNodeIdentifier );
    
    /**
     * Removes children from the child list and database
     */
    void RemoveChildrenL();
    
    /**
     * Removes children only from the child list
     */
    void RemoveChildren();
    
    /**
     * Stores current children to previous child list and sets the
     * previous child count.
     */
    void StoreChildrenToPreviousListL();
    
    /**
     * Stores the given child list as previous child list and sets the
     * previous child count.
     *
     * @param aPreviousChildren The previous children.
     * @param aPreviousChildCount.
     */
    void StoreChildrenToPreviousListL(
        const RPointerArray<CNcdChildEntity>& aPreviousChildren,
        TInt aPreviousChildCount );
    
    /**
     * Checks that have this folder's children been loaded previously.
     * This is needed to differentiate first time loading of the folder from
     * consequent loads.
     *
     * @note Does not indicate that the children are in cache or
     * anything else other than what is described above.
     *
     * @return ETrue if children have been loaded previously, EFalse otherwise.
     */
    TBool ChildrenPreviouslyLoaded();
    
    /**
     * Setter for the children previously loaded flag.
     *
     * @param aChildrenPreviouslyLoaded Default value is ETrue.
     * @note This flag should be unset only in special occaisions
     * such as unsetting for root node on cache clear.
     */
    void SetChildrenPreviouslyLoaded( TBool aChildrenPreviouslyLoaded = ETrue );
    
    /**
     * Getter for previous child count.
     *
     * @note Not persistently stored to db.
     * @return The previous child count or KErrNotFound if not set.
     */
    TInt PreviousChildCount();
    
    /**
     * Getter for previous child array.
     *
     * @note The array is not persistently stored to db.
     * @return An array containing previous children.
     */
    const RPointerArray<CNcdChildEntity>& PreviousChildArray();
    
    
    /**
     * Sets folder's children as expired and removes them from the child array
     * and deletes their metadatas from disk cache
     */
    void ExpireAndRemoveChildrenL();
    

public: // CNcdNode

    /**
     * @see CNcdNode::ReceiveMessage
     */
    virtual void ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                 TInt aFunctionNumber );


    /**
     * @see CNcdNode::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see CNcdNode::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );



protected:

    /**
     * @see CNcdNode::CNcdNode
     */
    CNcdNodeFolder( CNcdNodeManager& aNodeManager,
        NcdNodeClassIds::TNcdNodeClassId aNodeClassId = NcdNodeClassIds::ENcdFolderNodeClassId, 
        NcdNodeClassIds::TNcdNodeClassId aAcceptedLinkClassId = NcdNodeClassIds::ENcdFolderNodeLinkClassId,
        NcdNodeClassIds::TNcdNodeClassId aAcceptedMetaDataClassId = NcdNodeClassIds::ENcdFolderNodeMetaDataClassId );
    
    /**
     * @see CNcdNode::ConstructL
     */
    virtual void ConstructL( const CNcdNodeIdentifier& aIdentifier );


    /**
     * @see CNcdNode::CreateLinkL
     */
    virtual CNcdNodeLink* CreateLinkL();


    /** 
     * @see CNcdNode::ExternalizeDataForRequestL
     */
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream ) const;    


    /**
     * @param aStream that will contain the child array data.
     */
    void ExternalizeChildArrayL( RWriteStream& aStream ) const;

    /**
     * This function is required because the proxy may require different
     * child identifiers into its array. The child classes of this class
     * may provide its own implementation to create child arrays for proxy.
     * This implementation uses ExternalizeChildArrayL.
     *
     * @param aStream that will contain the child array data.
     */
    virtual void ExternalizeChildArrayForRequestL( RWriteStream& aStream ) const;
    
    void NodeSeenFolderHandleRequestL( MCatalogsBaseMessage& aMessage ) const;  
    
    
private:
    
    // Prevent these two if they are not implemented
    CNcdNodeFolder( const CNcdNodeFolder& aObject );
    CNcdNodeFolder& operator =( const CNcdNodeFolder& aObject );


private: // data

    // This array contains the identifying info of child nodes.
    // The information can be used to get the actual node from
    // the node manager. The identityinfo is used here instead of
    // actual references to the nodes because the manager will
    // handle the destruction of the nodes. Now, there is not
    // danger for NULL pointer errors.
    RPointerArray<CNcdChildEntity> iChildren;
    
    // Determines whether the previous child list has been populated.
    // NOTE: This is not stored to db currently.
    //TBool iPreviousChildrenStored;
    
    // Previous child list is stored here when refreshing folder.
    // Nodes not found from this list are considered new.
    // NOTE: This array is not stored to db currently.
    RPointerArray<CNcdChildEntity> iPreviousChildren;
    
    // Previous child count (i.e. the server child count)
    // KErrNotFound if not set;
    // NOTE: This is not stored to db currently. 
    TInt iPreviousChildCount;
    
    // Defines whether children of this folder have been loaded previously.
    // This is needed to differentiate first time loading of the folder from
    // consequent loads. This flag is persistently stored to db.
    // NOTE: This flag does not indicate that the children are in cache or
    // anything else other than what is described above.
    TBool iChildrenPreviouslyLoaded;
    
    // Implements the seen interface of folders.
    CNcdNodeSeenFolder* iNodeSeenFolder;
    
    };


#endif // NCD_NODE_FOLDER_H
