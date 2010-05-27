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
* Description:   Definition of MNcdPurchaseHistory interface
*
*/


#ifndef M_NCD_PURCHASE_HISTORY_H
#define M_NCD_PURCHASE_HISTORY_H

#include "catalogsbase.h"
#include "ncdinterfaceids.h"

class MNcdPurchaseHistoryFilter;
class MNcdPurchaseDetails;
class CNcdPurchaseDetails;

/**
 *  Purchase history interface.
 *
 *  This interface gives access to the purchase history. Purchase details can
 *  be inserted into the purchase history and list of previously inserted
 *  details can be asked through this interface.
 *
 *  
 */

class MNcdPurchaseHistory : public virtual MCatalogsBase
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase
     * interfaces.
     * 
     */
    enum { KInterfaceUid = ENcdPurchaseHistoryUid };

    /**
     * Save purchase into the purchase history. Purchase details of previous
     * purchase will be overwritten, if Namespace, Entity ID, purchase time
     * and client UID of new details matches to the previous details.
     *
     * 
     * @param aDetails Details of the purchase.
     * @param aSaveIcon ETrue by default, which means that a purchase detail icon 
     * is also saved with other purchase details. If EFalse, a new icon will not be saved 
     * into the purchase history. This is useful, for example, if an icon was not 
     * loaded when PurchaseDetailsL function was used and the old icon should be
     * left into the purchase history when purchase details are saved. 
     */
    virtual void SavePurchaseL( const MNcdPurchaseDetails& aDetails,
                                TBool aSaveIcon = ETrue ) = 0;

    /**
     * Get purchase IDs from purchase history.
     *
     * 
     * @param aFilter Filter used to get certain purchase IDs from the
     *  purchase history.
     * @return Array of purchase IDs. Can be empty.
     * @exception Leave System wide error code.
     */
    virtual RArray<TUint> PurchaseIdsL(
        const MNcdPurchaseHistoryFilter& aFilter ) = 0;

    /**
     * Get purchase details from purchase history.
     *
     * 
     * @param aPurchaseId ID of the purchase. PurchaseIdsL function can be
     * used to get purchase IDs.
     * @param aLoadIcon If EFalse, icon is not loaded from the purchase history. 
     * ETrue by default.
     * @return Purchase details. Ownership is transferred to the caller.
     * @exception KErrNotFound if purchase is not found.
     */
    virtual CNcdPurchaseDetails* PurchaseDetailsL( TUint aPurchaseId, 
        TBool aLoadIcon = ETrue ) = 0;

    /**
     * Remove purchase from purchase history.
     *
     * 
     * @param aPurchaseId ID of the purchase. PurchaseIdsL function can be
     * used to get purchase IDs.
     * @exception KErrNotFound if purchase is not found.
     */
    virtual void RemovePurchaseL( TUint aPurchaseId ) = 0;

    /**
     * Get count of all purchase events.
     *
     * 
     * @return Total purchase event count.
     */
    virtual TUint EventCountL() = 0;


protected: // Destruction

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdPurchaseHistory() {}

    };

#endif // M_NCD_PURCHASE_HISTORY_H
