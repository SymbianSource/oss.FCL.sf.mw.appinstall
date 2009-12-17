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
* Description:   CIAUpdateFwPurchaseOperation 
*
*/



#ifndef IA_UPDATE_FW_PURCHASE_OPERATION_H
#define IA_UPDATE_FW_PURCHASE_OPERATION_H


#include <ncdpurchaseoperationobserver.h>

#include "iaupdatenormalcontentoperation.h"

class CIAUpdateFwNode;


/**
 * CIAUpdateFwPurchaseOperation can handle purchasing
 * of firmware nodes.
 */
class CIAUpdateFwPurchaseOperation : public CIAUpdateNormalContentOperation,
                                     public MNcdPurchaseOperationObserver
    {

public:

    /**
     * @see CIAUpdateFwPurchaseOperation::CIAUpdateFwPurchaseOperation
     * @return CIAUpdateFwPurchaseOperation* Created object.
     * Ownership is transferred.
     */
    static CIAUpdateFwPurchaseOperation* NewL(
        CIAUpdateFwNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );
    
    /**
     * @see CIAUpdateFwPurchaseOperation::NewL
     */ 
    static CIAUpdateFwPurchaseOperation* NewLC(
        CIAUpdateFwNode& aNode,
        MIAUpdateContentOperationObserver& aObserver );

    /**
     * Destructor
     */
    virtual ~CIAUpdateFwPurchaseOperation();


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
    CIAUpdateFwPurchaseOperation( 
        const CIAUpdateFwPurchaseOperation& aObject );
    CIAUpdateFwPurchaseOperation& operator =( 
        const CIAUpdateFwPurchaseOperation& aObject );


    /**
     * @see CIAUpdateNormalContentOperation::CIAUpdateNormalContentOperation
     */
    CIAUpdateFwPurchaseOperation( 
        CIAUpdateFwNode& aNode,
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

#endif // IA_UPDATE_FW_PURCHASE_OPERATION_H

