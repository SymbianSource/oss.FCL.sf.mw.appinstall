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
* Description:   Interface definition for numeric value query item.
*
*/
	

#ifndef M_NCD_QUERY_NUMERIC_ITEM_H
#define M_NCD_QUERY_NUMERIC_ITEM_H

#include <e32cmn.h>

#include "ncdqueryitem.h"
#include "ncdinterfaceids.h"


/**
 *  Describes a numeric query item.
 */
class MNcdQueryNumericItem : public virtual MNcdQueryItem
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdQueryNumericItemUid };

    /**
     * Setter for queried numeric value.
     *
     * @param aValue Value to set.
     */ 
    virtual void SetValueL( const TDesC& aValue ) = 0;

protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdQueryNumericItem() {}

    };
	
	
#endif //  M_NCD_QUERY_NUMERIC_ITEM_H
