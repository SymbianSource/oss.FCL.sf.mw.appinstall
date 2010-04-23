/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  BTObjectExchange application panic codes
*
*/


#ifndef __BTOBJECTEXCHANGE_PAN__
#define __BTOBJECTEXCHANGE_PAN__

#include <e32std.h>

/** BTObjectExchange application panic codes */
enum TBTObjectExchangePanics 
    {
    EBTObjectExchangeBasicUi = 1,
    EBTObjectExchangeReceiverInvalidState,
    EBTObjectExchangeSenderExists,
    EBTObjectExchangeReceiverExists,
    EBTObjectExchangeSenderInvalidState,
    EBTObjectExchangeNoSender,
    EBTObjectExchangeAddMessage,
    EBTObjectExchangeNextRecordRequestComplete,
    EBTObjectExchangeAttributeRequestResult,
    EBTObjectExchangeAttributeRequestComplete,
    EBTObjectExchangeInvalidControlIndex,
    EBTObjectExchangeProtocolRead,
    EBTObjectExchangeAttributeRequest,
    EBTObjectExchangeSdpRecordDelete,
    EBTObjectExchangeUnexpectedState,
    EBTObjectExchangeUnexpectedLogicState
    };


/** Panic Category */
_LIT(KPanicBTOBEX, "BTOBEX");


inline void Panic(TBTObjectExchangePanics aReason)
    { 
    User::Panic(KPanicBTOBEX, aReason);
    }

#endif // __BTOBJECTEXCHANGE_PAN__
