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
* Description:   Payment method interface definition.
*
*/


#ifndef M_NCD_PAYMENT_METHOD_H
#define M_NCD_PAYMENT_METHOD_H

#include "ncdinterfaceids.h"


/**
 *  Describes a payment method.
 *
 *  Each purchasable item may have one or more of these.
 *
 *  @note namespace named as MNcdPaymentMethod for compatibility with current code
 *  and to make it possible to convert this to an actual interface in the future
 */
namespace MNcdPaymentMethod
    {
    
    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdPaymentMethodUid };


    /** 
     * TNcdNodePaymentMethod defines the ways that can be used 
     * to pay the node items.
     *
     * 
     */
    enum TNcdPaymentMethodType
        {
        /** Payment method is credit card. */
        EPaymentCreditCard,

        /** Payment method is direct. */        
        EPaymentDirect,

        /** Payment method is SMS. */
        EPaymentSms,

        /**  Payment method is custom. */        
        EPaymentCustom
        };

    }
	
	
#endif //  M_NCD_PAYMENT_METHOD_H
