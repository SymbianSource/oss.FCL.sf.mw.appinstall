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
	

#ifndef M_NCDPURCHASEOPERATIONOBSERVER_H
#define M_NCDPURCHASEOPERATIONOBSERVER_H

#include <e32cmn.h>

#include "catalogsarray.h"

class MNcdPurchaseOperation;
class TNcdProgress;
class MNcdQuery;

/**
 *  Observer interface for purchase operations
 *
 *  Purchase operation users should implement this interface to be able to
 *  receive purchase operation callbacks.
 *
 *  
 *  @see MNcdPurchaseOperation
 */
class MNcdPurchaseOperationObserver
    {
    
public:

    /**
     * Called when a purchase operation has progressed.
     *
     * 
     * @param aOperation The operation that sent the event.
     * @param aProgress Progress of the operation.
     */
    virtual void PurchaseProgress( MNcdPurchaseOperation& aOperation,
        TNcdProgress aProgress ) = 0;


    /**
     * Called when a query has been received for an operation.
     *
     * @note Querys need to be completed with MNcdOperation::CompleteQueryL()
     *       to continue the operation! Alternatively, the operation must be
     *       cancelled.
     *
     * 
     * @param aOperation Uncounted reference to the operation that sent
     *        the event.
     * @param aQuery Pointer to the query object that needs to be
     *        completed. Counted, Release() must be called after use.
     * @see MNcdOperation::CompleteQueryL()
     */
    virtual void QueryReceived( MNcdPurchaseOperation& aOperation,
        MNcdQuery* aQuery ) = 0;
    

    /**
     * Called when an operation has been completed.
     *
     * 
     * @param aOperation The operation that sent the event.
     * @param aError Error code for operation completion. 
     *               KErrNone for successful completion, otherwise a system
     *               wide error code.
     */
    virtual void OperationComplete( MNcdPurchaseOperation& aOperation,
        TInt aError ) = 0;
    };
    
#endif //  M_NCDPURCHASEOPERATIONOBSERVER_H
