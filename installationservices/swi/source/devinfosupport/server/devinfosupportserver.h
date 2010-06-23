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
* Device information server
* @internalComponent
* @released
*
*/


#ifndef __DEVINFOSUPPORTSERVER_H__
#define __DEVINFOSUPPORTSERVER_H__

#include <e32base.h>
#include "generictimer.h"
#include "devinfofetcher.h"

namespace Swi
{

/**
Server class to return device information to the client.  When
instantiated this class immediately begins to fetch the device
information via the CDeviceInfoFetcher class.  This class implements
the MDeviceInfoReceiver interface to receive this information once it
has been retrieved.  It also implementeds the MGenericTimerClient
interface to receive notification of when the server should be
shutdown after its alloted idle time.  Since device information
requests are stateless, the session classes delegate the job of
completing client requests to this server.  If the information has not
been retrieved at the point the client request is received, the client
request is queued until the information has been received at which
point the client requests are completed.
*/
class CDeviceInfoServer : public CPolicyServer, public MGenericTimerClient, public MDeviceInfoReceiver
	{
public:

	/**
	Create an instance of the server at the given priority.
	@param aPriority priority at which the server active object should run.  Defaults to CActive::EPriorityStandard.
	@return the new server instance.
	*/
	IMPORT_C static CDeviceInfoServer* NewL(CActive::TPriority aPriority = CActive::EPriorityStandard);
	/**
	Create an instance of the server at the given priority.  Place the result on the cleanup stack.
	@param aPriority priority at which the server active object should run.  Defaults to CActive::EPriorityStandard.
	@return the new server instance.
	*/
	IMPORT_C static CDeviceInfoServer* NewLC(CActive::TPriority aPriority = CActive::EPriorityStandard);

	/**
	Clean up resources used by the server.
	*/
	~CDeviceInfoServer();
private:
	/**
	Private Constructor.
	*/
	CDeviceInfoServer(CActive::TPriority aPriority);

	/**
	Create a new session.
	*/
	CSession2* NewSessionL(const TVersion& aClientVersion, const RMessage2& aMessage) const;

	/**
	Second phase constructor.
	*/
	void ConstructL();

public:
// API used only by CDeviceInfoSession

	/**
	Notify the server that a session is active.
	*/
	void AddSession();
	/**
	Notify the server that a session has been destroyed.
	*/
	void DropSession();
	/**
	Delegation of the session method to handle a request to get the
	device ids from the device.  If the information is available this
	method will complete the message.  If not then the message is
	queued until the information is available.
	@param aMessage message to be handled.
	*/
	void HandleGetDeviceIdsL(const RMessagePtr2& aMessage);

private:
	/**
	Implementation of MGenericTimerClient handler.  Stops the active
	scheduler, thus terminating the server.
	*/
	void HandleTimeout();

private:
	// from MDeviceInfoReceiver handler
	/**
	Implementation of MDeviceInfoReceiver method to accept the list of
	device ids once it has been retrieved.  Ownership of the list is
	transferred by this method.
	*/
	void AcceptDeviceIdsL(CDeviceIdList* aDeviceIdList);
	/**
	Implementation of MDeviceInfoReceiver method to signal an error
	whilst fetching the list of device ids.
	*/
	void HandleDeviceIdErrorL(TInt aErrCode);

private:

	/**
	Method to complete a client request to retrieve device
	information.  Handles marshalling and unmarshalling of data across
	the IPC boundary.
	*/
	void CompleteDeviceIdRequestL(const RMessagePtr2& aMessage);
	/**
	Method to complete any outstanding client requests to retrieve device
	information.
	*/
	void CompleteOutstandingRequestsL();

	/**
	The number of sessions that are connected to the server. When this
	number reaches 0, the server will (after an idle period) initiate
	its shutdown.
	*/
	TInt iSessionCount;

	/**
	This is used to shutdown the server after a delay.
	*/
	CGenericTimer* iTimer;

	/**
	Active object used to retrieve the device information.
	*/
	CDeviceInfoFetcher* iDeviceInfoFetcher;
	/**
	List of device ids returned to clients.
	*/
	CDeviceIdList* iDeviceIdList;
	/**
	Error returned to clients in the event that device information
	could not be retrieved.
	*/
	TInt iDeviceIdFetchError;
	/**
	List of device ids packed into a descriptor ready for return
	across the IPC boundary.  This is a copy of the data in
	iDeviceIdList (in a packed form).
	*/
	HBufC8* iPackedDeviceIdList;


	/**
	Array of outstanding device id requests are completed once the
	device ids have finished being retrieved.
	*/
	RArray<RMessagePtr2> iOutstandingDeviceIdRequests;

	// Variables used by the server policy
	static const TUint iRangeCount=2;
	static const TInt iRanges[iRangeCount];
	static const TUint8 iElementsIndex[iRangeCount];
	static const CPolicyServer::TPolicyElement iPolicyElements[1];
	static const CPolicyServer::TPolicy iPolicy;
	};


/**
Method to start up the server
*/
void StartServerL();

} // namespace Swi

/**
Main entry point to the server.
*/
GLDEF_C TInt E32Main();

#endif // __DEVINFOSUPPORTSERVER_H__
