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
* Description:   Definition of purchase history filter interface and
*                implementation class.
*
*/


#ifndef M_NCD_PURCHASE_HISTORY_FILTER_H
#define M_NCD_PURCHASE_HISTORY_FILTER_H

#include <e32base.h>

#include "catalogsbase.h"

/**
 *  Purchase history filter interface.
 *
 *  Allows setting of client uid(s), entity namespace and id, and event id
 *  for filtering purchase history information.
 *
 *  
 *  @see MNcdPurchaseHistory::PurchaseIdsL()
 */

class MNcdPurchaseHistoryFilter
    {

public:
    
    /**
     * Get client UIDs used in this filter. Purchase events have client UID to
     * indicate the creator of the event.
     *
     * 
     * @return Array of client UIDs. If empty, all client UIDs match.
     */
    virtual const TArray< TUid > ClientUids() const = 0;


    /**
     * Get the namespace used in this filter.
     *
     * 
     * @return Namespace. If empty string, all namespaces match.
     */
    virtual const TDesC& Namespace() const = 0;


    /**
     * Get entity ID used in this filter.
     *
     * 
     * @return Entity ID. If empty string, all entity ids match.
     */
    virtual const TDesC& EntityId() const = 0;

    /**
     * Get event ID used in this filter.
     *
     * 
     * @return Event ID. Only purchases with a larger event id will match. 
     */
    virtual TUint EventId() const = 0;

    };


#endif // M_NCD_PURCHASE_HISTORY_FILTER_H
