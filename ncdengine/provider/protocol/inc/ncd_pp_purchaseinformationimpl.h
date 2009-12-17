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


#ifndef NCD_PROTOCOL_ELEMENT_PURCHASEINFORMATION_IMPL_H
#define NCD_PROTOCOL_ELEMENT_PURCHASEINFORMATION_IMPL_H

#include "ncd_pp_purchase.h"
#include "ncdprotocoltypes.h"

class MNcdConfigurationProtocolQuery;

class CNcdPreminetProtocolPurchaseInformationImpl 
: public CBase, public MNcdPreminetProtocolPurchaseInformation
    {
    
public:

    static CNcdPreminetProtocolPurchaseInformationImpl* NewL();
    static CNcdPreminetProtocolPurchaseInformationImpl* NewLC();

    virtual ~CNcdPreminetProtocolPurchaseInformationImpl();
    
    void ConstructL();


    // From base class MNcdPreminetProtocolPurchase
    /**
     * Returns the ID of this entity.
     * @return Id
     */
    virtual const TDesC& Uri() const;

    /**
     * Returns the dependency ID of this entity.
     * @return Id
     */
    virtual const TDesC& Namespace() const;

    /**
     * Returns the dependency ID of this entity.
     * @return Id
     */
    virtual TBool InitiateSession() const;

    /**
     * Returns an payment object.
     * @param aIndex Index of payment object (0..count-1)
     * @return Id
     */
    virtual const MNcdPreminetProtocolPayment&
        PaymentL(TInt aIndex) const;

    virtual TInt PaymentCount() const;

    /**
     * Returns the disclaimer. 
     * Ownership is NOT transferred.
     * @return Pointer to disclaimer object. 
     */
    virtual const MNcdConfigurationProtocolQuery* 
        Disclaimer() const;

    /**
     * Returns the dependency ID of this entity.
     * @return Id
     */
    virtual const TDesC& TotalPrice() const;

    /**
     * Returns the dependency ID of this entity.
     * @return Id
     */
    virtual const TDesC& TotalPriceCurrency() const;

    /**
     * Returns the entity. 
     * Ownership is NOT transferred.
     * @return Entity pointer
     */
    // Only one entity supported for now
    virtual const MNcdPreminetProtocolPurchaseEntity&
        EntityL(TInt aIndex) const;

    virtual TInt EntityCount() const;

    /**
     * Returns the dependency ID of this entity.
     * @return Id
     */
    // Not supported?
    //virtual const RArray<TNcdPurchaseEntityRedirected>& 
    //RedirectedEntities() const;

    virtual const TDesC& TransactionId() const;
    virtual const TDesC& QueryId() const;

public:

    HBufC* iUri;
    HBufC* iNamespace;
    TBool iInitiateSession;

    RPointerArray<MNcdPreminetProtocolPayment> iPayments;
    MNcdConfigurationProtocolQuery* iDisclaimer;
    
    HBufC* iTotalPrice;
    HBufC* iTotalPriceCurrency;
    
    MNcdPreminetProtocolPurchaseEntity* iEntity;    

    HBufC* iTransactionId;
    HBufC* iQueryId;
    
    };

#endif // NCD_PROTOCOL_ELEMENT_PURCHASEINFORMATION_IMPL_H