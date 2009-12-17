/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdatePurchaseOperation 
*
*/



#ifndef IA_UPDATE_PURCHASE_OPERATION_H
#define IA_UPDATE_PURCHASE_OPERATION_H


#include <ncdpurchaseoperationobserver.h>

#include "iaupdatenormalcontentoperation.h"

class CIAUpdateNode;


/**
 * CIAUpdatePurchaseOperation can handle purchasing of hidden
 * dependency nodes when one operation is requested
 * for one visible node.
 */
class CIAUpdatePurchaseOperation : public CIAUpdateNormalContentOperation,
                                   public MNcdPurchaseOperationObserver
    {

public:

    /**
     * @see CIAUpdatePurchaseOperation::CIAUpdatePurchaseOperation
     * @return CIAUpdatePurchaseOperation* Created object.
     * Ownership is transferred.
     */
    static CIAUpdatePurchaseOperation* NewL(
        CIAUpdateNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );
    
    /**
     * @see CIAUpdatePurchaseOperation::NewL
     */ 
    static CIAUpdatePurchaseOperation* NewLC(
        CIAUpdateNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );

    /**
     * Destructor
     */
    virtual ~CIAUpdatePurchaseOperation();


public: // MNcdPurchaseOperationObserver
    
    /**
     * @see MNcdPurchaseOperationObserver::PurchaseProgress    
     */
    virtual void PurchaseProgress( 
        MNcdPurchaseOperation& aOperation, TNcdProgress aProgress );
        
    /**
     * @see MNcdPurchaseOperationObserver::QueryReceived
     */
    virtual void QueryReceived( 
        MNcdPurchaseOperation& aOperation, MNcdQuery* aQuery );

    /**
     * @see MNcdPurchaseOperationObserver::OperationComplete
     */
    virtual void OperationComplete( 
        MNcdPurchaseOperation& aOperation, TInt aError );    
    
    
protected: // CIAUpdateNormalContentOperation

    /**
     * @see CIAUpdateContentOperation::HandleContentL
     */ 
    virtual MNcdOperation* HandleContentL();


private:
    
    // Prevent these if not implemented
    CIAUpdatePurchaseOperation( 
        const CIAUpdatePurchaseOperation& aObject );
    CIAUpdatePurchaseOperation& operator =( 
        const CIAUpdatePurchaseOperation& aObject );


    /**
     * @see CIAUpdateNormalContentOperation::CIAUpdateNormalContentOperation
     */
    CIAUpdatePurchaseOperation( 
        CIAUpdateNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );

    /**
     * ConstructL
     */
    virtual void ConstructL();


    /** 
     * Starts the purchase operation for given node.
     *
     * @return MNcdOperation* Operation that is started.
     */
    MNcdOperation* PurchaseL();
    

private: // data

    };

#endif // IA_UPDATE_PURCHASE_OPERATION_H

