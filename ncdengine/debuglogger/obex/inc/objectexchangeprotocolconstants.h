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
* Description:  
*
*/


#ifndef __CBTOBEX_SDP_PROTOCOL__
#define __CBTOBEX_SDP_PROTOCOL__

#include <e32def.h>
#include <bt_sock.h>

const TUid KUidBTObjectExchangeApp = {0x10005b8e};

// See entry for OBEX in table 4.3 "Protocols"
// at the url http://www.bluetooth.org/assigned-numbers/sdp.htm
const TUint KBtProtocolIdOBEX = 0x0008;

const TUint KRfcommChannel = 1;

// See entry for OBEXObjectPush in table 4.4 "Service Classes"
// at the url http://www.bluetooth.org/assigned-numbers/sdp.htm
const TUint KServiceClass = 0x1105; 

_LIT(KServiceName,"OBEX");
_LIT(KServiceDescription,"Object Push");
_LIT(KServerTransportName,"RFCOMM");

#endif //__CBTOBEX_SDP_PROTOCOL__