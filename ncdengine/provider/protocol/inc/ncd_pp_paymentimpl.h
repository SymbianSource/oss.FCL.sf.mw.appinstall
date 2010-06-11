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
* Description:   CNcdPreminetProtocolPaymentImpl declaration
*
*/


#ifndef NCD_PROTOCOL_ELEMENT_PAYMENT_IMPL_H
#define NCD_PROTOCOL_ELEMENT_PAYMENT_IMPL_H

#include "ncd_pp_purchase.h"
//#include "ncdprotocoltypes.h"
#include "ncdpaymentmethod.h"
#include "ncdstoragedataitem.h"

class CNcdString;

class CNcdPreminetProtocolPaymentImpl :
    public CBase,
    public MNcdPreminetProtocolPayment,
    public MNcdStorageDataItem
    {
    
public:

    static CNcdPreminetProtocolPaymentImpl* NewL();
    static CNcdPreminetProtocolPaymentImpl* NewLC();

    virtual ~CNcdPreminetProtocolPaymentImpl();
    
    void ConstructL();


    // From base class MNcdPreminetProtocolPurchase
    virtual MNcdPaymentMethod::TNcdPaymentMethodType Method() const;
    
    virtual const TDesC& QueryId() const;
    
    virtual const CNcdString& Name() const;

    virtual const MNcdPreminetProtocolSmsDetails& 
        SmsDetailsL(TInt aIndex) const;
    virtual TInt SmsDetailsCount() const;
    
    virtual CNcdPreminetProtocolPaymentImpl* CloneL() const;

public: // From MNcdStorageDataItem

    void ExternalizeL( RWriteStream& aStream );
    void InternalizeL( RReadStream& aStream );

public:

    MNcdPaymentMethod::TNcdPaymentMethodType iMethod;
    HBufC* iQueryId;
    CNcdString* iName;
    RPointerArray<MNcdPreminetProtocolSmsDetails> iSmsDetails;
    
    };

#endif // NCD_PROTOCOL_ELEMENT_PAYMENT_IMPL_H