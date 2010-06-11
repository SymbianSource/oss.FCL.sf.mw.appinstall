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
* Description:   MNcdPreminetProtocolPurchaseOption declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_SUBSCRIPTIONDETAILS_H
#define NCD_PREMINET_PROTOCOL_SUBSCRIPTIONDETAILS_H

#include <e32base.h>
#include "ncdprotocoltypes.h"

class MNcdPreminetProtocolSubscriptionDetails
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolSubscriptionDetails() {}

    /**
     * Validity delta
     * @return Validity time
     */
    virtual TInt ValidityDelta() const = 0;

    /**
     * Is the subscription auto-updateable
     * @return True or false
     */
    virtual TBool ValidityAutoUpdate() const = 0;

    /**
     * Subscription credit value
     * @return Credit value
     */
    virtual TReal32 AmountOfCredits() const = 0;

    /**
     * Subscription credit currency
     * @return Credit currency
     */
    virtual const TDesC& AmountOfCreditsCurrency() const = 0;

    /**
     * Amount of downloads in this subscription
     * @return Amount of downloads
     */
    virtual TInt NumberOfDownloads() const = 0;

    };


#endif //NCD_PREMINET_PROTOCOL_SUBSCRIPTIONDETAILS_H
