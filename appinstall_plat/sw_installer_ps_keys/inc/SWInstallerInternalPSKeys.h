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
* Description:   Internal Publish & Subscribe keys.
*
*/


#ifndef SWINSTALLERINTERNALPSKEYS_H
#define SWINSTALLERINTERNALPSKEYS_H


/**
* Sowftware Installer UI notification
*/
const TUid KPSUidSWInstallerUiNotification = { 0x101FD65A };

/**
* KSWInstallerCancelReq
* Indicates if user has requested cancellation of the current process.
*
* Possible values:
* 0: False
* 1: True
*/
const TUint32 KSWInstallerCancelReq = 0x00000001;

/**
 * KSWInstallerUninstallation
 * Key is used to signal that uninstallation of SW is ongoing. 
 * During uninstallation process key takes value of the package, 
 * which is currently handled. When uninstallation is over package value 
 * equals to zero (0).
 *
 *
 */
const TUint32 KSWInstallerUninstallation = 0x00000002;

/**
 * KSWInstallerDisableDaemonNotes
 * Key is used to disable SWI Daemon's pre install note which is 
 * shown when SWI Daemon start pre install. SWI Daemon will read 
 * this key always before it will show the note. If key is set to 
 * true SWI Daemon does not show the note.
 * 
 * Possible values:
 * 0: False
 * 1: True
 */
const TUint32 KSWInstallerDisableDaemonNotes = 0x00000003;

#endif

// End of file
