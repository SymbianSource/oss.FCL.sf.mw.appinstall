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
* Description:   ?Description
*
*/
	

#ifndef M_NCD_PURCHASE_OPERATION_H
#define M_NCD_PURCHASE_OPERATION_H

#include <e32cmn.h>

#include "ncdoperation.h"
#include "ncdinterfaceids.h"

class MNcdPurchaseOperationObserver;
class MNcdPurchaseOption;

/**
 *  Purchase operation interface.
 *
 *  Purchase operation handles the purchasing of one purchasable node.
 *
 *  Purchase operations will query the user for information needed to
 *  complete the purchase. For example payment method, credit card number,
 *  -owner, etc will be be queried during a credit card purchase.
 *
 *  
 *  @see MNcdNodePurchase
 *  @see MNcdPurchaseOperationObserver
 */
class MNcdPurchaseOperation : public MNcdOperation	
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdPurchaseOperationUid };


    /**
     * Gets the purchase option selected for this operation.
     *
     * 
     * @return Selected purchase option. Uncounted reference.
     */
    virtual const MNcdPurchaseOption& PurchaseOption() = 0;
	

protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdPurchaseOperation() {}

	};
	
	
#endif //  M_NCD_PURCHASE_OPERATION_H
