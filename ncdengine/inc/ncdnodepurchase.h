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
* Description:   Contains MNcdNodePurchase interface
*
*/


#ifndef M_NCD_NODE_PURCHASE_H
#define M_NCD_NODE_PURCHASE_H


#include <e32std.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"
#include "catalogsarray.h"

class MNcdPurchaseOption;
class MNcdPurchaseOperation;
class MNcdPurchaseOperationObserver;


/**
 *  Provides functions for the purchase process of the node content.
 *  There may be multiple purchase options for the content. And every
 *  purchase option may have multiple payment methods.
 *
 *  After the node has been purchased the content has to be downloaded.
 *  Downloading may be done by using the download interface functions.
 *
 *  
 */
class MNcdNodePurchase : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodePurchaseUid };


    /**
     * Gives the available purchase options for the node.
     *
     * @note This function should be called to get current and valid list of
     *       purchaseoptions after node's state changes.
     *
     * @return Array of pointers to available purchase options for the node.
     *         Counted, Release() must be called for each after use, e.g. with
     *         a call to RCatalogsArray::ResetAndDestroy()
     *         
     * @exception Leave System wide error codes.
     */
    virtual RCatalogsArray< MNcdPurchaseOption > PurchaseOptionsL() const = 0;


    /**
     * Gives a purchase option whose id matches given purchase option id.
     *
     * @return Purchase option whose id matches given purchase option id.
     *         NULL if no purchase option with given id is found from the
     *         purchase options available for the node.
     *         Counted, Release() must be called after use.
     *         
     * @exception Leave System wide error codes.
     */
    virtual MNcdPurchaseOption* PurchaseOptionL(
        const TDesC& aPurchaseOptionId ) const = 0;


    /**
     * Starts the purchase operation for the given purchase option.
     *
     * @note The reference count of the operation object is increased by one. 
     * So, Release function of the operation should be called when operation 
     * is not needed anymore.
     *
     * @param aPurchaseOption The purchase option that is used for purchasing.
     * @param aObserver Observer for the operation.
     * @return Pointer to an operation that can 
     *         be used to check the progressing of the purchase operation. Counted,
     *         Release() must be called after use.
     * @exception Leave with KNcdErrorObsolete if the given purchase option
     *            is obsolete.
     *            Leave with KNcdErrorSubscriptionNotSubscribableAnymore
     *            if the validity time, which indicates that the subscription
     *            is subscribable, has already passed.
     *            Leave with KNcdErrorParallelOperationNotAllowed if a parallel
     *            client is running an operation for the same metadata. See
     *            MNcdOperation for full explanation.
     *            Else leave with system wide error code.
     */
    virtual MNcdPurchaseOperation* PurchaseL(
        MNcdPurchaseOption& aPurchaseOption,
        MNcdPurchaseOperationObserver& aObserver ) = 0;


    /**
     * Tells if the node is already purchased.
     *
     * 
     * @return ETrue if purchased, EFalse otherwise.
     */
    virtual TBool IsPurchased() const = 0;

    
    /**
     * Returns the purchased purchase option.
     *
     * @note Valid only when IsPurchased() returns ETrue.
     *
     * @return Purchase option of the last successful purchase. NULL if
     *  node has not been purchased. Counted, Release() must be called
     *  after use.
     * @exception Leave System wide error code.
     */
    virtual MNcdPurchaseOption* PurchasedOptionL() const = 0;


    /**
     * Gives the time of the purchase if the node has been purchased.
     * Time given is in universal time.
     *
     * @note Valid only when IsPurchased() returns ETrue.
     *
     * @return Time of the purchase in universal time.
     * @exception KErrNotFound If the node has not been purchased.
     * @exception Leave System wide error codes.
     */
    virtual TTime TimeOfPurchaseL() const = 0;


    /**
     * Gives the final price paid when the node was purchased.
     *
     * @note Valid only when IsPurchased() returns ETrue.
     *
     * @return Final price of the purchase.
     * @exception KErrNotFound If the node has not been purchased.
     * @exception Leave System wide error codes.
     */
    virtual const TDesC& PurchasedPriceL() const = 0;


protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdNodePurchase() {}

    };


#endif // M_NCD_NODE_PURCHASE_H
