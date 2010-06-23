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
* Device information server session
* @internalComponent
* @released
*
*/


#ifndef __DEVINFOSUPPORTSESSION_H__
#define __DEVINFOSUPPORTSESSION_H__

#include <e32base.h>

namespace Swi
{
/**
The name of the Device Info Support Server
*/
_LIT(KDeviceInfoServerName,"!DEVINFOSUPPORTSERVER");

/**
The name of the Device Info Support Executable
*/
_LIT(KDeviceInfoServerExeName,"DEVINFOSUPPORT");

/**
The name of the Device Info Support Thread
*/
_LIT(KDeviceInfoServerThreadName,"DEVINFOSUPPORTSERVER-THREAD");

//The current version of the Device Info Support Server
const TInt KDeviceInfoServerVersionMajor=0;
const TInt KDeviceInfoServerVersionMinor=1;
const TInt KDeviceInfoServerVersionBuild=0;

class CDeviceInfoServer;

/**
Session used by the Device Info support Server.
*/
class CDeviceInfoSession : public CSession2
	{
public:
	/**
	Create a new session for the given server.
	@param aServer server with which this session is associated.
	*/
	static CDeviceInfoSession* NewL(CDeviceInfoServer& aServer);
	/**
	Create a new session for the given server, and place the result on the cleanup stack.
	@param aServer server with which this session is associated.
	*/
	static CDeviceInfoSession* NewLC(CDeviceInfoServer& aServer);

	/**
	Messages passed to the server.
	*/
	enum TMessage
	{
		KMessageGetDeviceIds,

		KMaxMessage,
	};

	/**
	Clean up resources used by the session.
	*/
	~CDeviceInfoSession();	

	/**
	Service a message from the client.
	*/
	void ServiceL(const RMessage2& aMessage);

private:
	/**
	Construct a session.
	*/
	CDeviceInfoSession(CDeviceInfoServer& aServer);


	// Command handling functions
	
	/**
	Handle KMessageGetDeviceIds Message.  Delegate all requests of
	this kidn to the server.
	*/
	void HandleGetDeviceIdsL(const RMessagePtr2& aMessage);

private:
	/**
	The server with which this session is associated.
	*/
	CDeviceInfoServer& iServer;
	};

} // namespace Swi

#endif // __DEVINFOSUPPORTSESSION_H__
