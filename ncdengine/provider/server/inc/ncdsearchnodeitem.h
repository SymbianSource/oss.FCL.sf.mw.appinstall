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
* Description:   Contains CNcdSearchNodeItem class
*
*/


#ifndef NCD_SEARCH_NODE_ITEM_H
#define NCD_SEARCH_NODE_ITEM_H


#include <e32cmn.h>

#include "ncdnodeitem.h"


/**
 * This class provides search item specific functionality for the node.
 */
class CNcdSearchNodeItem : public CNcdNodeItem
    {
    
public:

    /**
     * NewL
     *
     * Note, that node link contains the metadata id.
     * Because node link contains metadata id, set meta data function
     * is not provided in this class.
     * @return CNcdSearchNodeItem* Pointer to the created object 
     * of this class.
     */
    static CNcdSearchNodeItem* NewL( CNcdNodeManager& aNodeManager,
                                     const CNcdNodeIdentifier& aIdentifier );

    /**
     * @see CNcdSearchNodeItem::NewL
     *
     */
    static CNcdSearchNodeItem* NewLC( CNcdNodeManager& aNodeManager,
                                      const CNcdNodeIdentifier& aIdentifier );


    /**
     * Destructor
     */
    virtual ~CNcdSearchNodeItem();


protected:

    /**
     * @see CNcdNodeItem::CNcdNodeItem
     */
    CNcdSearchNodeItem( CNcdNodeManager& aNodeManager );
    
    /**
     * @see CNcdNodeItem::ConstructL
     */
    virtual void ConstructL( const CNcdNodeIdentifier& aIdentifier );

private:
    
    // Prevent these two if they are not implemented
    CNcdSearchNodeItem( const CNcdSearchNodeItem& aObject );
    CNcdSearchNodeItem& operator =( const CNcdSearchNodeItem& aObject );


private: // data
    
    };


#endif // NCD_SEARCH_NODE_ITEM_H
