/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Common stuff for the Sit Transport library.
*
*/


/**
 @file
 @internalComponent
*/

#ifndef SIFTRANSPORTCOMMON_H
#define SIFTRANSPORTCOMMON_H

#include <e32uid.h>

namespace Usif
	{

		/**
			Operations supported by the SIF Server

			bit 0 reserved for requests that send file name in an asynchronous IPC,
			bit 1 reserved for requests that send file handle in an asynchronous IPC,
			bit 2 reserved for requests that send component info in an asynchronous IPC,
			bit 3 reserved for requests that send component id in an asynchronous IPC,
			bit 4 reserved for requests that send opaque data in an asynchronous IPC,
			bit 5 reserved for requests that send opaque data to a plug-in,
			bit 6 reserved for installation requests
	 	*/
	enum TSifTransportFunction
		{
		EFileNameInIpc								= 0x01,
		EFileHandleInIpc							= 0x02,
		EComponentInfoInIpc							= 0x04,
		EComponentIdInIpc							= 0x08,
		EOpaqueDataInIpc							= 0x10,
		EOpaqueDataToPlugin							= 0x20,
		EInstall									= 0x40,
		EGetComponentInfoByFileName 				= 0x100|EFileNameInIpc|EComponentInfoInIpc,
		EGetComponentInfoByFileHandle				= 0x200|EFileHandleInIpc|EComponentInfoInIpc,
		EInstallByFileName							= 0x300|EInstall|EFileNameInIpc|EOpaqueDataToPlugin,
		EInstallByFileNameWithOpaqueData			= 0x400|EInstall|EFileNameInIpc|EOpaqueDataInIpc|EOpaqueDataToPlugin,
		EInstallByFileHandle						= 0x500|EInstall|EFileHandleInIpc|EOpaqueDataToPlugin,
		EInstallByFileHandleWithOpaqueData			= 0x600|EInstall|EFileHandleInIpc|EOpaqueDataInIpc|EOpaqueDataToPlugin,
		EInstallByFileHandleWithOpaqueDataPreamble	= 0x700,
		EUninstall									= 0x800|EComponentIdInIpc|EOpaqueDataToPlugin,
		EUninstallWithOpaqueData					= 0x900|EComponentIdInIpc|EOpaqueDataInIpc|EOpaqueDataToPlugin,
		EActivate									= 0xa00|EComponentIdInIpc,
		EDeactivate									= 0xb00|EComponentIdInIpc
		};

	} // End of namespace Usif

#endif	// #ifndef SIFSERVERCOMMON_H
