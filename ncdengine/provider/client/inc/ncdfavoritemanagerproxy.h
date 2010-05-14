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


#ifndef NCD_FAVORITE_MANAGER_PROXY_H
#define NCD_FAVORITE_MANAGER_PROXY_H

#include "ncdinterfacebaseproxy.h"
#include "ncdfavoritemanager.h"

class CNcdNodeManagerProxy;
class CNcdNodeIdentifier;
class CNcdNodeDisclaimerProxy;
class CNcdNodeProxy;

/**
 * Proxy side object of favorite manager. Implements the MNcdFavoriteManager
 * interface used by UI. The class contains a list of all the favorite nodes.
 */
class CNcdFavoriteManagerProxy: public CNcdInterfaceBaseProxy,
                                public MNcdFavoriteManager
{
public:
    /**
     * NewL
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aParent Parent whose reference count AddRef of this
     *                object increments.
     * @param aNodeManager This class uses node manager to get the nodes.
     * @return CNcdFavoriteManagerProxy* Pointer to the created object 
     * of this class.
     */     
    static CNcdFavoriteManagerProxy* NewL(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CCatalogsInterfaceBase* aParent,
        CNcdNodeManagerProxy& aNodeManager );
        
    /**
     * NewLC
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aParent Parent whose reference count AddRef of this
     *                object increments.
     * @param aNodeManager This class uses node manager to get the nodes.
     * @return CNcdFavoriteManagerProxy* Pointer to the created object 
     * of this class.
     */     
    static CNcdFavoriteManagerProxy* NewLC(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CCatalogsInterfaceBase* aParent,
        CNcdNodeManagerProxy& aNodeManager );
    
    /**
     * Destructor.
     */    
    virtual ~CNcdFavoriteManagerProxy();
    
    /**
     * Tells whether the given node is a favorite node. The node is
     * favorite if its metadata is favorite.
     *
     * @param aNodeIdentifier The identifier.
     * @return ETrue if favorite, otherwise EFalse.
     */
    TBool IsFavoriteL( const CNcdNodeIdentifier& aNodeIdentifier ) const;
    
    /**
     * Adds the given identifier to favorite list.
     *
     * @param aNodeIdentifier The identifier.
     * @param aRemoveOnDisconnect If true, the node is removed from favorites when
     * the client-server session is closed.
     */
    void AddToFavoritesL(
        const CNcdNodeIdentifier& aNodeIdentifier, TBool aRemoveOnDisconnect );
    
    /**
     * Removes the given identifier from favorites list.
     *
     * @param aNodeIdentifier The identifier.
     */
    void RemoveFromFavoritesL( const CNcdNodeIdentifier& aNodeIdentifier );
    
    /**
     * Sets or removes the disclaimer of the favorite node.
     *
     * @param aNodeIdentifier The favorite node identifier.
     * @param aDisclaimerOwner The owner of the disclaimer, to unset disclaimer, give NULL.
     *                         Ownership is not transferred.
     * @exception Leave KErrArgument If the node is not a favorite node.
     */
    void SetNodeDisclaimerL(
        const CNcdNodeIdentifier& aNodeIdentifier,
        MNcdNode* aDisclaimerOwner );

    /**
     * Returns the disclaimer of the favorite node or NULL if the node
     * has not a disclaimer.
     *
     * @param aNodeIdentifier The node.
     * @return The disclaimer.
     * @exception Leave KErrArgument If the node is not a favorite node.
     */        
    CNcdNodeDisclaimerProxy* NodeDisclaimerL(
        const CNcdNodeIdentifier& aNodeIdentifier );

    /**
     * Returns the favorite node which has the given metadata or NULL
     * if there is no such favorite node.
     *
     * @param aMetaDataIdentifier The metadata identifier.
     * @return The node or NULL. Ownership is NOT transferred.
     */
    CNcdNodeProxy* FavoriteNodeByMetaDataL(
        const CNcdNodeIdentifier& aMetaDataIdentifier ) const;
        

    /**
     * Internalizes the favorite list from server side favorite manager.
     */
    void InternalizeL();    
        
    
public: // From MNcdFavoriteManager

    /**
     * @see MNcdFavoriteManager::FavoriteNodeCount
     */
    virtual TInt FavoriteNodeCount() const;
    
    /**
     * @see MNcdFavoriteManager::FavoriteNodeL
     */
    virtual MNcdNode* FavoriteNodeL( TInt aIndex ) const;
    
    /**
     * @see MNcdFavoriteManager::FavoriteIndexL
     */
    virtual TInt FavoriteIndexL(
        const TDesC& aNamespace, const TDesC& aId ) const;
    
    /**
     * @see MNcdFavoriteManager::ClearFavoritesL
     */
    virtual void ClearFavoritesL();
    
protected:

    /**
     * Constructor.
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aParent Parent whose reference count AddRef of this
     *                object increments.
     * @param aNodeManager This class uses node manager to get the nodes.
     */    
    CNcdFavoriteManagerProxy( 
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CCatalogsInterfaceBase* aParent,
        CNcdNodeManagerProxy& aNodeManager );
        
    /**
     * ConstructL.
     */        
    void ConstructL();
    
    
    /**
     * Internalizes the favorite list from the given stream.
     *
     * @param aStream The stream.
     */
    void InternalizeDataL( RReadStream& aStream );
    
    /**
     * Returns the actual favorite node identifier, which has the same
     * metadata id as the given node. Returns NULL if there is no such
     * favorite node.
     *
     * @param aNodeIdentifier The node identifier.
     * @return The actual favorite node identifier, or NULL. Ownership is NOT
     * transferred.
     */
    CNcdNodeIdentifier* ActualFavoriteIdentifierL(
        const CNcdNodeIdentifier& aNodeIdentifier ) const;
    
    /**
     * Returns the favorite node identifier which has the given metadata identifier.
     * If there is no such favorite node, return NULL.
     *
     * @param aMetaDataIdentifier The metadata identifier.
     * @return The favorite node identifier, or NULL. Ownership is not
     * transferred.
     */
     CNcdNodeIdentifier* FavoriteIdentifierByMetaDataL(
        const CNcdNodeIdentifier& aMetaDataIdentifier ) const;        
    
private:
    // Node manager.
    CNcdNodeManagerProxy& iNodeManager;
    
    // Favorite nodes.
    RPointerArray<CNcdNodeIdentifier> iFavoriteNodes;
};


#endif
