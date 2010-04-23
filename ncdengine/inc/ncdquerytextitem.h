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
* Description:   Interface definition for text query item.
*
*/
	

#ifndef M_NCD_QUERY_TEXT_ITEM_H
#define M_NCD_QUERY_TEXT_ITEM_H

#include <e32cmn.h>

#include "ncdqueryitem.h"
#include "ncdinterfaceids.h"

/**
 *  Describes a text query item.
 *
 * 
 */
class MNcdQueryTextItem : public virtual MNcdQueryItem
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdQueryTextItemUid };

    /**
     * Setter for query text.
     *
     * 
     * @param aText Text to set.
     * @exception Leave System wide error code.
     */ 
    virtual void SetTextL( const TDesC& aText ) = 0;
    
protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdQueryTextItem() {}

    };
	
	
#endif //  M_NCD_QUERY_TEXT_ITEM_H
