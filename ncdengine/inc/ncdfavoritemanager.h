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
* Description:   Definition of MNcdFavoriteManager interface
*
*/


#ifndef M_NCDFAVORITEMANAGER_H
#define M_NCDFAVORITEMANAGER_H

#include "catalogsbase.h"
#include "ncdinterfaceids.h"

class MNcdNode;

/**
 *  Favorite manager interface.
 *
 *  This interface describes a favorite manager. Favorite manager
 *  provides methods for favorite node management.
 *
 *  
 */
class MNcdFavoriteManager : public virtual MCatalogsBase
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdFavoriteManagerUid };
    
    /**
     * Get the number of favorite nodes.
     *
     * 
     * @return The number of favorite nodes.
     */
    virtual TInt FavoriteNodeCount() const = 0;
    
    /**
     * Get a favorite node by the given index.
     *
     * 
     * @param aIndex The node index.
     * @return The favorite node.
     * @leave KErrNotFound If the index is out of range.
     */
    virtual MNcdNode* FavoriteNodeL( TInt aIndex ) const = 0;
    
    /**
     * Get the index of a favorite node in the favorite list.
     *
     * Note that the actual favorite node may be different node but
     * it has the same metadata. That is, asking a favorite node
     * with FavoriteNodeL with the index returned by this method may
     * return a node with different id and namespace.
     *
     * 
     * @param aNamespace Namespace of the node.
     * @param aId Id of the node.
     * @leave KErrNotFound If the node is not favorite, in other error
     * cases system wide error code.
     */
    virtual TInt FavoriteIndexL(
        const TDesC& aNamespace, const TDesC& aId ) const = 0;
    
    /**
     * Removes all the favorite nodes.
     *
     * 
     * @exception Leave System wide error code.
     */
    virtual void ClearFavoritesL() = 0;

protected: // Destruction

    /**
     * Destructor.
     *
     * @see MCatalogsBase::~MCatalogsBase
     */
    virtual ~MNcdFavoriteManager() {}
    };

#endif // M_NCDFAVORITEMANAGER_H
