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
* Description:   HTTP headers used in NCD
*
*/


/**
 * HTTP headers used in NCD provider
 */
namespace NcdHttpHeaders
    {
    // 8-bit since SSID is stored as 8-bit
    _LIT8( KSsidHeader, "ncdSsid" );
    
    // 16-bit since their values are handled in 16-bit format
    _LIT( KClientIdHeader, "ncdClientId" );    
    _LIT( KSessionIdHeader, "ncdSessionId" );
    }
