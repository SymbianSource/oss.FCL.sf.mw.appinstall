/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __OBJECTEXCHANGECLIENT_H__
#define __OBJECTEXCHANGECLIENT_H__

#include <e32base.h>
#include <obex.h>

class CObjectExchangeServiceSearcher;

/** 
 * @class CObjectExchangeClient
 * @description Class for connecting and sending files to a remote OBEX capable device
 */
class CObjectExchangeClient : public CActive
    {
public:
/**
 * @function NewL
 * 
 * @description Construct a CObjectExchangeClient
 * @return a pointer to the created instance of CObjectExchangeClient
 */
    static CObjectExchangeClient* NewL();

/**
 * @function NewLC
 * 
 * @description Construct a CObjectExchangeClient and put it into the cleanup stack
 * @return a pointer to the created instance of CObjectExchangeClient
 */
    static CObjectExchangeClient* NewLC();

/**
 * @function ~CObjectExchangeClient
 * 
 * @description Destroy the object and release all memory objects. Close any open sockets
 */
    ~CObjectExchangeClient();

/**
 * @function ConnectL
 * 
 * @description Connect to an OBEX service on a remote device
 * pops up 
 */
    void ConnectL(TRequestStatus& aObsRequestStatus);

/**
 * @function DisconnectL
 * 
 * @description Disconnect from the remote device.  Sends an OBEX disconnect, and
 * closes the transport on response from the server.
 **/
    void DisconnectL(TRequestStatus& aObsRequestStatus);

/**
 * @function SendObjectL
 * 
 * @description Send a file to an OBEX service on a remote machine
 * @param aFileName {const TDesc&, in} The full path name of the file to send
 */
    void SendObjectL(const TDesC& aFileName, TRequestStatus& aObsRequestStatus);

/**
 * @function StopL
 * 
 * @description Send the OBEX aborts command to the remote machine 
 */
    void StopL();

/**
 * @function IsConnected
 * 
 * @return ETrue if the client is connected
 */
    TBool IsConnected();

/**
 * @function IsBusy
 * 
 * @return ETrue if the client is performing some operation.
 */
    TBool IsBusy();

protected:    // from CActive
/**
 * @function DoCancel
 * 
 * @description Cancel any outstanding requests
 */
    void DoCancel();

/**
 * @function RunL
 * 
 * @description Respond to an asynhcronous event
 */
    void RunL();

private:
/**
 * @function CObjectExchangeClient
 *
 * @description Construct this object
 */
    CObjectExchangeClient();

/**
 * @function ConstructL
 *
 * @description Perform second phase construction of this object
 */
    void ConstructL();
    
/**
 * @function ConnectToServerL
 * 
 * @description Connect to the server
 */
    void ConnectToServerL();

private:

/**
 * @enumtype TState
 *
 * @description The state of the active object which determines behaviour within
 * the RunL method.
 * @value EWaitingToGetDevice waiting for the user to select a device
 * @value EGettingDevice searching for a device
 * @value EGettingService searching for a service
 * @value EGettingConnection connecting to a service on a remote device
 * @value EWaitingToSend sending a message to the remote device
 * @value EDisconnecting disconnecting from the server
 */
  
    
    enum TState 
    {
        EWaitingToGetDevice,
            EGettingDevice,
            EGettingService,
            EGettingConnection,
            EWaitingToSend,
            EDisconnecting
            
    };


/* iState the state of the active object, determines behaviour within the RunL method. */
    TState iState;

/* iServiceSearcher searches for service this client can connect to */
    CObjectExchangeServiceSearcher* iServiceSearcher;

/* iClient manages the OBEX client connection */
    CObexClient* iClient;

/* iCurrObject the OBEX object to transfer */
    CObexBaseObject* iCurrObject;

/* @var iObserverStatus pointer to the observer's TRequestStatus */
    TRequestStatus* iObserverStatus; 

	TParse iParse;

    };



#endif // __OBJECTEXCHANGECLIENT_H__

