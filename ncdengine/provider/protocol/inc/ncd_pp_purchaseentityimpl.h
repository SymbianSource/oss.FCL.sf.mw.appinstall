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


#ifndef NCD_PROTOCOL_ELEMENT_PURCHASEENTITY_IMPL_H
#define NCD_PROTOCOL_ELEMENT_PURCHASEENTITY_IMPL_H

#include "ncd_pp_purchase.h"
#include "ncdprotocoltypes.h"

class CNcdPreminetProtocolPurchaseEntityImpl 
: public CBase, public MNcdPreminetProtocolPurchaseEntity
    {
    
public:

    static CNcdPreminetProtocolPurchaseEntityImpl* NewL();
    static CNcdPreminetProtocolPurchaseEntityImpl* NewLC();

    virtual ~CNcdPreminetProtocolPurchaseEntityImpl();
    
    void ConstructL();

    virtual const TDesC& Ticket() const;
    
    virtual const TDesC& Id() const;
    
    virtual const RArray<TNcdDeliveryMethod>& 
        DeliveryMethods() const;
    
    virtual const MNcdPreminetProtocolDownload&
        DownloadDetailsL(TInt aIndex) const;
    virtual TInt DownloadDetailsCount() const;
    
    virtual const TDesC& Price() const;
    virtual const TDesC& PriceCurrency() const;
    virtual const TDesC& PriceText() const;

private:
    CNcdPreminetProtocolPurchaseEntityImpl();

public:

    HBufC* iTicket;
    HBufC* iId;

    RArray<TNcdDeliveryMethod> iDeliveryMethods;
    RPointerArray<MNcdPreminetProtocolDownload> iDownloadDetails;

    HBufC* iPrice;
    HBufC* iPriceCurrency;
    HBufC* iPriceText;
    
    };

#endif // NCD_PROTOCOL_ELEMENT_PURCHASEENTITY_IMPL_H