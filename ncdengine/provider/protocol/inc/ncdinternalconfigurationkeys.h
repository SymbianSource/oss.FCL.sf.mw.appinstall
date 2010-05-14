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


#ifndef NCD_INTERNAL_CONFIGURATION_KEYS_H
#define NCD_INTERNAL_CONFIGURATION_KEYS_H

// Include also normal configuration keys here because they
// may be used also internally.
#include "ncdconfigurationkeys.h"

/**
 * Internal configurations that provide additional keys
 * for NcdConfigurationKeys. These keys are provided only
 * for internal use and are not to be used through the API.
 *
 * Keys mentioned here are mostly used for filling specific
 * elements/attributes in configuration requests.
 *
 * Keys that are not listed here, are always added 
 * to software details in configuration requests.
 *
 * @internal
 */
namespace NcdInternalConfigurationKeys
    {
    /**
     * @note Free disk space is automatically sent by the engine. The drive with the largest amount
     * of free disk space is chosen. This is sent in client's client-element as a hardware detail.
     */
    _LIT( KFreeStorage, "freeStorage" );

    /**
     * @note UID and version number of the installed Flash player is sent automatically by the engine
     * in the client's client-element as a hardware detail
     */
    _LIT( KFlashPlayer, "flash-player" );
    
    /**
     * @note The IMEI code of the device is sent in client's client-element as a hardware detail.
     */
    _LIT( KImei, "imei" );
    }

#endif // NCD_INTERNAL_CONFIGURATION_KEYS_H
