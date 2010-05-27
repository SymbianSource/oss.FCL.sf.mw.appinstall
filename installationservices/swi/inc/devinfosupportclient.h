/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @file
* Device information client interface
* @internalComponent
* @released
*
*/


#ifndef __DEVINFOSUPPORTCLIENT_H__
#define __DEVINFOSUPPORTCLIENT_H__

#include <e32std.h>

namespace Swi
{

class CDeviceIdList;

/**
A handle to a server session which calls commdb and etelmm to
retrieve the devices id(s)
@internalComponent
@released
*/
class RDeviceInfo : public RSessionBase
	{
public:
	/**
	Construct the handle.
	*/
	IMPORT_C RDeviceInfo();

	/**
	Connect to the server.
	@return KErrNone on success - error code on failure.
	*/
	IMPORT_C TInt Connect();
	
	/**
	Close the connection to the server
	*/
	IMPORT_C void Close();
	
	/**
	Get the list of device ids.  Ownership is _not_ transferred.
	The pointer array and strings contained therein will be
	deallocated when the Close() method is called.  The data returned
	will be valid until either the Close() or DeviceIdsL()
	methods are called.
	@return Reference to an array of device id heap descriptors.
	*/
	IMPORT_C const RPointerArray<HBufC>& DeviceIdsL();

private:

	/**
	Starts the server which carries out the requests
	*/
	TInt StartServer();

	/**
	List of device ids unpacked from the server request.
	*/
	CDeviceIdList *iDeviceIdList;
	};

} // namespace Swi

#endif // __DEVINFOSUPPORTCLIENT_H__
