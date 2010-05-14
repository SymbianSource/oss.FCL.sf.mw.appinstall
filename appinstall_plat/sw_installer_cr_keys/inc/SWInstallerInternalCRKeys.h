/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SWINSTALLERINTERNALCRKEYS_H
#define SWINSTALLERINTERNALCRKEYS_H

#include <e32std.h>

/**
 * Sowftware Installer settings
 *
 */
const TUid KCRUidSWInstallerSettings = {0x101FD64D};

/**
 * KSWInstallerOcspProcedure
 * Define the policy for the online certificate validation procedure
 * - Off = 0
 * - On = 1
 * - Must be passed = 2 
 * 
 */
const TUint32 KSWInstallerOcspProcedure = 0x00000001;
enum TSWInstallerOcspProcedure
    {
    ESWInstallerOcspProcedureOff = 0,
    ESWInstallerOcspProcedureOn,
    ESWInstallerOcspProcedureMust
    };

/**
 * KSWInstallerOcspDefaultURL
 * Defines the default URL to OCSP server
 */
const TUint32 KSWInstallerOcspDefaultURL = 0x00000002;

/**
 * Defines UI level policy that defines SW Installer behaviour 
 * when user tries to install untrusted SW.
 * - Allow = 1 (Untrusted SW can be installed)
 * - Not Allow = 0 (Untrusted SW cannot be installed)
 * Note: The key does not override swipolicy.ini AllowUnsigned key. 
 */
const TUint32 KSWInstallerAllowUntrusted = 0x00000003;

/**
 * Software Installer Local Variation Keys
 *
 */
const TUid KCRUidSWInstallerLV = {0x101FD64E};

/**
 * KSWInstallerPackageFolder
 * Defines the folder used by the Application Manager UI to search
 * the installation packages, which can be installed on the device.
 * The key is a string value must specify the path. The Application Manager UI
 * searches through all avalible device drives if the key does not specify 
 * the drive:
 *   C:\\S60\\Installs - looks for the packages on C: drive only; 
 *   \\S60\\Installs - looks for the packages on all avalible drives)
 */
const TUint32 KSWInstallerPackageFolder = 0x00000001;

/**
 * This key is used to hide the 'allow installation of untrusted SW' setting 
 * from the UI. If this key has value 1 then the UI setting is not visible. 
 * Even if the setting is hidden from the UI, it affects on the behavior of 
 * Installer UI as defined in KSWInstallerAllowUntrusted.
 * 0 : UI setting is visible.
 * 1 : UI setting is NOT visible.
 *
 */
const TUint32 KSWInstallerHideUntrustedIns = 0x00000004;

#endif
