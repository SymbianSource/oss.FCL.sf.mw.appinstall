/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains common internal declarations for both the 
*                Installer server and client.
*
*/


#ifndef SWINSTCOMMON_H
#define SWINSTCOMMON_H

//  INCLUDES
#include <e32std.h>
#include "SWInstPrivateUid.h"

namespace SwiUI
{

// CONSTANTS
_LIT( KSWInstSvrName, "SWInstSvr" );
const TUid KUidSWInstSvr = { KSWInstSvrUid };

// DATA TYPES

/**
* Reasons for client panic.
*
* @since 3.0
*/
enum TSWInstPanic
    {
    ESWInstPanicBadRequest,
    ESWInstPanicBadDescriptor,
    ESWInstPanicBadHandle
    };

const TUint KFileHandleIpcSlot = 0;
const TUint KFileSrvSessionIpcSlot = 1;
const TUint KFileNameIpcSlot = 0;
const TUint KMimeIpcSlot = 1;
const TUint KParamsIpcSlot = 2;
const TUint KOptionsIpcSlot = 3;
const TUint KUidIpcSlot = 0;
const TUint KRequestIpcSlot = 3;
const TUint KCommonParamsIpcSlot = 2;
const TUint KOperationIpcSlot = 0;

}


#endif // SWINSTCOMMON_H
