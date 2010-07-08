/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* SisRegistry - shared client/server definitions
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include <e32std.h>

_LIT(KSisLauncherServerName,"!SisLauncherServer");
_LIT(KSisLauncherServerImg,"SisLauncherServer");		// DLL/EXE name
const TUid KServerUid3={0x1020473F};

const TInt KMaxSisLauncherMessage=100;

enum TSisLauncherMessages
	{
	ERunExecutable,
	EStartDocument,
	EStartDocumentByHandle,
	EStartByMime,
	EStartByMimeByHandle,
	EShutdown,
	EShutdownAll,
	ECheckApplicationInUse,
	ENotifyNewApps,
	EQueueRunExecutable,
	EQueueStartDocumentByHandle,
	EQueueStartByMimeByHandle,
	EExecuteQueue,
	EKillQueue,
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	//Function Id's to be accessed by Installserver
	EParseSwTypeRegFile = 30,
	ERegisterSifLauncherMimeTypes,
	ENotifyNewAppsData,                                                     //Notify Apparc about application data
	//Function Id's to be accessed by SisRegistryServer
	EUnregisterSifLauncherMimeTypes = 60,
	//Function Id's to be accessed by Installserver and SisRegistryServer
	ENotifyApparcForApps = 90,                                              //Notify Apparc about updated applications
	//Function Id's to be accessed by all clients
    EAsyncParseResourceFileSize = 120,                                      //Parse registration resource file, Async Function
    EAsyncParseResourceFileData,                                            //Return parsed resource data
    #endif
	ESeparatorEndAll	
   
	};
