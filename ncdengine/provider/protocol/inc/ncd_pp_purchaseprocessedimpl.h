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
* Description:   CNcdPreminetProtocolPurchaseImpl declaration
*
*/


#ifndef NCD_PROTOCOL_ELEMENT_PURCHASEPROCESSED_IMPL_H
#define NCD_PROTOCOL_ELEMENT_PURCHASEPROCESSED_IMPL_H

#include "ncd_pp_purchase.h"
#include "ncdprotocoltypes.h"

class MNcdConfigurationProtocolQuery;

class CNcdPreminetProtocolPurchaseProcessedImpl 
: public CBase, public MNcdPreminetProtocolPurchaseProcessed
    {
    
public:

    static CNcdPreminetProtocolPurchaseProcessedImpl* NewL();
    static CNcdPreminetProtocolPurchaseProcessedImpl* NewLC();

    virtual ~CNcdPreminetProtocolPurchaseProcessedImpl();
    
    void ConstructL();


    // From base class MNcdPreminetProtocolPurchaseProcessed
    /**
     * Returns the ID of this entity.
     * @return Id
     */
    virtual TInt ResultCode() const;

    /**
     * Returns the dependency ID of this entity.
     * @return Id
     */
    virtual const MNcdPreminetProtocolPurchaseEntity&
        EntityL(TInt aIndex) const;
    virtual TInt EntityCount() const;

    virtual const MNcdConfigurationProtocolQuery*
        Information() const;

    virtual const TDesC& TransactionId() const;
    virtual const TDesC& QueryId() const;

private:
    CNcdPreminetProtocolPurchaseProcessedImpl();

public:

    TInt iResultCode;
    RPointerArray<MNcdPreminetProtocolPurchaseEntity> iEntities;
    MNcdConfigurationProtocolQuery* iInformation;

    HBufC* iTransactionId;
    HBufC* iQueryId;
    
    };

#endif // NCD_PROTOCOL_ELEMENT_PURCHASEPROCESSED_IMPL_H
