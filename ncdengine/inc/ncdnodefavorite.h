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
* Description:   Contains MNcdNodeFavorite interface
*
*/


#ifndef M_NCD_NODE_FAVORITE_H
#define M_NCD_NODE_FAVORITE_H


#include "catalogsbase.h"
#include "ncdinterfaceids.h"

class MNcdQuery;
class MNcdNode;

/**
 * This interface is provided for favorite nodes. It can be used
 * to set and get a disclaimer of the favorite node, and remove
 * the node from favorites.
 *
 * It can be checked whether a node is a favorite node, by querying
 * this interface.
 *
 *  
 */
class MNcdNodeFavorite : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeFavoriteUid };
    
    /**
     * Removes the node from favorites.
     *
     * After calling this method, this interface will be invalid.
     *
     * @exception Leave System wide error code.
     */
    virtual void RemoveFromFavoritesL() = 0;
    
    /**
     * Sets the disclaimer for the node. To remove disclaimer,
     * give NULL pointer as parameter. 
     *
     * @param aDisclaimerOwner The node which has the disclaimer to set.
     *                         Ownership is not transferred.
     * @exception Leave System wide error code.
     */
    virtual void SetDisclaimerL( MNcdNode* aDisclaimerOwner ) = 0;
    
    /**
     * Get the dislaimer.
     *
     * @return The disclaimer, or NULL if disclaimer is not set. The object is
     * reference counted. Must be released after use.
     * @exception Leave System wide error code.
     */
    virtual MNcdQuery* DisclaimerL() const = 0;
    
    
protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdNodeFavorite() {}
    
    };


#endif // M_NCD_NODE_FAVORITE_H
