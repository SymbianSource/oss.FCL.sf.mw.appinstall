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


#ifndef __OBJECTEXCHANGETESTOBJECT_H__
#define __OBJECTEXCHANGETESTOBJECT_H__

#include <e32base.h>

class CObjectExchangeClient;

class MOsmObexSenderObserver
{
public:
    virtual void ObexFileSent() = 0;
    virtual void ObexDisconnected( TInt aError, TInt aState ) = 0;
};

/*! 
  @class COsmObexSender
  
  @discussion Class for testing the asynchronous call API of CObjectExchangeClient
  */
class COsmObexSender : public CActive
    {
public:

    enum TOsmConnectionType
    {
        EOsmConnectionBT,
        EOsmConnectionIrDA
    };

    COsmObexSender( MOsmObexSenderObserver& aObserver );


/*!
  @function NewL
  
  @discussion Construct a COsmObexSender
  @result a pointer to the created instance of COsmObexSender
  */
    static COsmObexSender* NewL( MOsmObexSenderObserver& aObserver );

/*!
  @function NewLC
  
  @discussion Construct a COsmObexSender
  @result a pointer to the created instance of COsmObexSender
  */
    static COsmObexSender* NewLC( MOsmObexSenderObserver& aObserver );

/*!
  @function ~COsmObexSender
  
  @discussion Destroy the object and release all memory objects. Close any open sockets
  */
    ~COsmObexSender();

/*!
  @function StartL
  
  @discussion Start the test procedure
  */
    void SendFileL( TOsmConnectionType aConnectionType, const TDesC& aFileName );

/*!
  @function StopL
  
  @discussion Stop the test procedure
  */
    void StopL();


protected:    // from CActive
/*!
  @function DoCancel
  
  @discussion Cancel any outstanding requests
  */
    void DoCancel();

/*!
  @function RunL
  
  @discussion Respond to an event
  */
    void RunL();

private:
/*!
  @function CMessageClient

  @discussion Construct this object
  */
    COsmObexSender();

/*!
  @function ConstructL

  @discussion Perform second phase construction of this object
  */
    void ConstructL();

private:

    /*!
      @enum TState
  
      @discussion The state of the active object, determines behaviour within
      the RunL method.
      @value EWaitingToConnect initial state
      @value EConnecting trying to connect to the server 
      @value ESending Sending data
      @value EDisconnecting Disconnecting from the remote
      */

    enum TState 
        {
        EWaitingToConnect,
        EConnecting,
        ESending,       
        EDisconnecting
        };

    /*! @var iState the state of the active object, determines behaviour within the RunL method. */
    TState iState;

    /*! @var iObexClient Active object that does file transfer */
    CObjectExchangeClient* iObexClient;

    MOsmObexSenderObserver& iObserver;

    TFileName   iFileName;

    };



#endif // __OBJECTEXCHANGETESTOBJECT_H__

