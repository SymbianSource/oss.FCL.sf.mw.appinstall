/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    This file contains the constant definitions of the Central
 *                Repository keys and the Central Repository UID.
 *
*/




#ifndef IAUPDATESDKCRKEYS_H
#define IAUPDATESDKCRKEYS_H


//  INCLUDES
#include <e32std.h>

#include "iaupdateuids.h"

// CONSTANTS

/**
 * The UID of the Central Repository file containing the settings. Should be
 * given as a parameter in CRepository::NewL() call.
 */
const TUid KCRUidIAUpdateSettings = { KIAUpdateUiUid };

/**
* The id of the currently selected access point, integer value.
**/
const TUint32 KIAUpdateAccessPoint = 0x0;

/**
* Automatic update check, integer value.
*
* Possible values:
*
* 0 = No automatiuc update check
* 1 = Automatiuc update check enabled when not roaming
* 2 = Automatic update enabled
**/
const TUint32 KIAUpdateAutoUpdateCheck = 0x1;


/**
* Checking frequency. 
*
* Possible values:
* By default it will be 28 days.
**/
const TUint32 KIAUpdateCheckingFrequency = 0x3;


/**
* Indicator of IAP overwritten
*
* Possible values:
* By default it will be EFalse
**/
const TBool KIAUpdateFWDefaultIAPIsOverwritten = 0x4;


/**
* The Id of default IAP
*
* Possible values:
* By default it will be EFalse
**/
const TUint32 KIAUpdateFWDefaultIAPId = 0x5; 


/**
* The Id of reminder interval for available updates
*
* Possible values:
* By default it will be 7 days
**/
const TUint32 KIAUpdateUpdateAvailableReminderInterval = 0x6; 


/**
* The Id of first time dialog delay
*
* Possible values:
* By default it will be 2 days
**/
const TUint32 KIAUpdateFirstTimeDialogDelay = 0x7; 


/**
* The Id of reminder interval of first time dialog
*
* Possible values:
* By default it will be 28 days
**/
const TUint32 KIAUpdateFirstTimeDialogReminderInterval = 0x8; 


/**
* The Id of refresh delta hours from grid
*
* Possible values:
* By default it will be 24 hours
**/
const TUint32 KIAUpdateGridRefreshDeltaHours = 0x9;
    

/**
 * The Id of application update interval
 *
 * Possible values:
 * By default it will be 168 hours
 **/
 const TUint32 KIAUpdateQueryHistoryDelayHours = 0xA;

/**
 * The Id of enabling firmware upgrade feature
 *
 * Possible values:
 * By default it will be true
 **/
 const TUint32 KIAUpdateEnableFWFeature = 0xB;


/**
 * The Id of disabling first time dialog
 *
 * Possible values:
 * By default it will be false
 **/
 const TUint32 KIAUpdateFirstTimeDialogDisabled = 0xC;


#endif      // IAUPDATESDKCRKEYS_H

// End of File
