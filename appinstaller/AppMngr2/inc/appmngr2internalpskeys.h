/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Internal Publish & Subscribe keys
*
*/


#ifndef _APPMNGR2INTERNALPSKEYS_H
#define _APPMNGR2INTERNALPSKEYS_H


/**
* App Manager notification
*/
const TUid KPSUidAppManagerNotification = { 0x101F8512 };

/**
* KAppManagerViewMode
* Indicates what view to be shown in AppManager
*
* Possible values:
* 0: Installation Files view
* 1: Installed view
*/
const TUint32 KAppManagerApplicationMode = 0x00000001;

enum TAppManagerApplicationModeValues {
    EAppMngr2InstallationFilesView = 0,
    EAppMngr2InstalledView = 1
};


#endif  // _APPMNGR2INTERNALPSKEYS_H

