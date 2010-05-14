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
* Device information fetching active object
* @internalComponent
* @released
*
*/


#ifndef __DEVINFOFETCHER_H__
#define __DEVINFOFETCHER_H__

#include <e32base.h>
#include <etel.h>
#include <etelmm.h>
#include <cdblen.h>

class RMessagePtr2;

namespace Swi
{

class CDeviceIdList;

/** 
Interface that clients should implement to receive device Id's
once the fetching active object has finished running.
*/
class MDeviceInfoReceiver
	{
public:
	/**
	Clients who implement this method must take ownership of the
	device id list.
	@param aDeviceIdList the device id list being handed over.
	*/
	virtual void AcceptDeviceIdsL(CDeviceIdList* aDeviceIdList) = 0;

	/**
	When the fetching of device ids fails this method will be called
	to indicate to the client that the request has failed.
	@param aErrCode the code that caused the failure.
	*/
	virtual void HandleDeviceIdErrorL(TInt aErrCode) = 0;
	};

/**
Active object class that makes asynchronous calls to the
RTelServer/RMobilePhone API's to fetch the device information and
store it locally.
*/
class CDeviceInfoFetcher : public CActive
	{
public:
	/**
	Construct an instance and begin fetching device information.
	@param aClient client to which the device information is passed once it has been retrieved.
	@param aPriority Priority with which the active object should run - defaults to CActive::EPriorityStandard
	@return new instance of CDeviceInfoFetcher class
	*/
	static CDeviceInfoFetcher* NewL(MDeviceInfoReceiver& aClient, TPriority aPriority = CActive::EPriorityStandard);

	/**
	Construct an instance and begin fetching device information. Place the result on the cleanup stack.
	@param aClient client to which the device information is passed once it has been retrieved.
	@param aPriority Priority with which the active object should run - defaults to CActive::EPriorityStandard
	@return new instance of CDeviceInfoFetcher class
	*/
	static CDeviceInfoFetcher* NewLC(MDeviceInfoReceiver& aClient, TPriority aPriority = CActive::EPriorityStandard);

	/**
    Cleans up the resources used by the instance.
	*/
	~CDeviceInfoFetcher();
private:
	/**
	Construct an instance.
	@param aClient client to which the device information is passed once it has been retrieved.
	@param aPriority Priority with which the active object should run - defaults to CActive::EPriorityStandard
	*/
	CDeviceInfoFetcher(MDeviceInfoReceiver& aClient, TPriority aPriority);

	/**
	Starts the active object to get the device Ids.
	*/
	void StartL();

	/**
	Connect to the etel server and load the default TSY.
	*/
	void ConnectToEtelL();

	/**
	Get the default TSY name from Commsdb/CommsDat.
	*/
	void GetTsyNameL();

	/**
	Get the next device id from the RMobilePhone class asynchronously.
	*/
	void GetNextDeviceIdL();

private:
    // from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt);
private:

	/**
	Client to which the device id list will be passed upon completion.
	*/
	MDeviceInfoReceiver& iClient;
	/**
	Telephony server connection.
	*/
	RTelServer iTelServer;
	/**
	Telephony phone connection.
	*/
	RMobilePhone iMobilePhone;
	/**
	Telephony device id (and other) information.
	*/
	RMobilePhone::TMobilePhoneIdentityV1 iMobilePhoneId;

	/**
	Total number of phones obtained by calling
	RTelServer::EnumeratePhones()
	*/
	TInt iNumPhones;
	/**
	Index into the current RMobilePhone (for use by RTelServer
	GetTsyName() and GetPhoneInfo() methods.
	*/
	TInt iCurrentPhoneIndex;

	/**
	Name of the default tsy used.
	*/
	TBuf<KCommsDbSvrMaxFieldLength> iTsyName;

	/**
	Array of device id's being built.  This is what is passed to the
	MDeviceInfoReceiver interface upon successful completion.
	*/
	CDeviceIdList* iDeviceIdList;
	};
}

#endif // __DEVINFOFETCHER_H__


