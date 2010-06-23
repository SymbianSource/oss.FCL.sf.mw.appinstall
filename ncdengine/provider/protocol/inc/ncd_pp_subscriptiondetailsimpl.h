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
* Description:   CNcdPreminetProtocolPurchaseOptionImpl declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_SUBSCRIPTIONDETAILS_IMPL_H
#define NCD_PREMINET_PROTOCOL_SUBSCRIPTIONDETAILS_IMPL_H

#include "ncd_pp_subscriptiondetails.h"
#include "ncdprotocoltypes.h"

class CNcdPreminetProtocolSubscriptionDetailsImpl 
: public CBase, public MNcdPreminetProtocolSubscriptionDetails
    {
    
public:

    static CNcdPreminetProtocolSubscriptionDetailsImpl* NewL();
    static CNcdPreminetProtocolSubscriptionDetailsImpl* NewLC();

    virtual ~CNcdPreminetProtocolSubscriptionDetailsImpl();
    
    void ConstructL();


    // From base class MNcdPreminetProtocolSubscriptionDetails

    /**
     * Subscription details of this purchase.
     */
    virtual TInt ValidityDelta() const;
    virtual TBool ValidityAutoUpdate() const;
    virtual TReal32 AmountOfCredits() const;
    virtual const TDesC& AmountOfCreditsCurrency() const;
    virtual TInt NumberOfDownloads() const;


private:
    CNcdPreminetProtocolSubscriptionDetailsImpl();
    
public:

    // subscription details
    TInt iValidityDelta;
    TBool iValidityAutoUpdate;
    TReal32 iAmountOfCredits; 
    HBufC* iAmountOfCreditsCurrency;
    TInt iNumberOfDownloads;
    };

#endif // NCD_PREMINET_PROTOCOL_SUBSCRIPTIONDETAILS_IMPL_H