/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef IAUPDATE_FW_CONST_H
#define IAUPDATE_FW_CONST_H


/**
* FOTA server UID
*/
const TUid KFOTAUid = {0x102072C4};

const TUint KMaxProfileNameLength = 100; 

//include the const defined in DM UI NSmlDMdef.h

#define KNSmlMaxProfileNameLength 100
#define KNSmlMaxItemLength 150  // The item max length.

const TInt KBufSize     = 128;
const TInt KBufSize32   = 32;
const TInt KBufSize255  = 255;
const TInt KBufSize256  = 256;

const TInt KDefaultHttpsPort    = 443;
const TInt KDefaultHttpPort     = 80;
const TInt KHttpUsed  = 1;
const TInt KHttpNotUsed  = 0;

const TInt KDMErr                   = -8144;

const TUid KUidNSmlMediumTypeInternet  = { 0x101F99F0 };
const TUid KUidNSmlMediumTypeBluetooth = { 0x101F99F1 };
const TUid KUidNSmlMediumTypeUSB       = { 0x101F99F2 };

_LIT( KSlash,       "/" );
_LIT( KHttpHeader,  "http://" );
_LIT( KHttpsHeader, "https://" );
_LIT( KColon,       ":" );


enum TNSmlSyncBearerType
    {
    EAspBearerInternet = 0,
    EAspBearerBlueTooth = 1,
    EAspBearerUsb = 2,
    EAspBearerIrda = 3,
    EAspBearerLast = 4
    };

enum TNSmlSyncTransportProperties
    {
    EPropertyIntenetAccessPoint = 0,
    EPropertyHttpUsed = 5,
    EPropertyHttpUserName = 6,
    EPropertyHttpPassword = 7
    };

enum TNSmlSyncState
    {
    ESASyncStateEnable,
    ESASyncStateDisable,
    ESASyncStateConfirm
    };

enum TNSmlStatus
    {
    ENSmlSyncComplete,
    ENSmlRefreshMainView
    };

#endif //IAUPDATE_FW_CONST_H


