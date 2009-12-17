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


#ifndef NCD_PROTOCOL_ELEMENT_PURCHASE_IMPL_H
#define NCD_PROTOCOL_ELEMENT_PURCHASE_IMPL_H

#include "ncd_pp_purchase.h"
#include "ncdprotocoltypes.h"


class CNcdPreminetProtocolPurchaseImpl 
: public CBase, public MNcdPreminetProtocolPurchase
    {
    
public:

    static CNcdPreminetProtocolPurchaseImpl* NewL();
    static CNcdPreminetProtocolPurchaseImpl* NewLC();

    virtual ~CNcdPreminetProtocolPurchaseImpl();
    
    void ConstructL();

public:

    };

#endif // NCD_PROTOCOL_ELEMENT_PURCHASE_IMPL_H