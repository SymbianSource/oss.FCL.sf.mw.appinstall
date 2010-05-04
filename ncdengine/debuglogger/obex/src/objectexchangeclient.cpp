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


/** Add TRAP harnesses for leaving functions such as iServiceSearcher->ConnectL 
Decide what to do with Copyrighted Nokia code */


#include "ObjectExchangeClient.h"
#include "ObjectExchangeServiceSearcher.h"

#include "obexconstantsinternal.h"
#include "BTObjectExchange.pan"

#include <e32svr.h>

// Define DEBUG trace for debug builds
#ifdef _DEBUG
#define TRACE(a) RDebug::Print a
#else
#define TRACE(a)
#endif

CObjectExchangeClient* CObjectExchangeClient::NewL()
{
    CObjectExchangeClient* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}

CObjectExchangeClient* CObjectExchangeClient::NewLC()
{
    CObjectExchangeClient* self = new (ELeave) CObjectExchangeClient();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CObjectExchangeClient::CObjectExchangeClient()
: CActive(CActive::EPriorityStandard),
iState(EWaitingToGetDevice)  // Go to initial state and wait for ConnectL function call
{
    CActiveScheduler::Add(this);
}

void CObjectExchangeClient::ConstructL()
{
    iServiceSearcher = CObjectExchangeServiceSearcher::NewL(); 
}


CObjectExchangeClient::~CObjectExchangeClient()
{
    Cancel();           // Cancel any outstanding request on this object
    
    delete iCurrObject;
    iCurrObject = NULL;
    
    delete iServiceSearcher; // iServiceSearcher's destructor completes any outstanding request
    iServiceSearcher = NULL;
    
    delete iClient;
    iClient = NULL;

}

void CObjectExchangeClient::DoCancel()
{

    // Add Cancelling for outstanding request caused by 
    // iServiceSearcher->SelectDeviceByDiscoveryL(iStatus)
    // and iServiceSearcher->FindServiceL(iStatus);
    // NOTE: works now if the iServiceSearcher is deleted, but not if cancelled
    // SOLUTION: delete and recreate iServiceSearcher in DoCancel? Or implement
    // Cancel to iServiceSearcer!

    if (iState != EWaitingToGetDevice && iClient)
    {
        iClient->Abort();

	
	delete iClient; // must be deleted so that connection is not left open
					// if sender cancels before receiver accepts connection
	iClient = NULL;
		
    }


	iServiceSearcher->Cancel();

	iStatus = KErrNone;  // At least emulated version does not work without this    

	iState = EWaitingToGetDevice;
	User::RequestComplete( iObserverStatus, KErrCancel );
}


void CObjectExchangeClient::RunL()
{   
   
    if (iStatus != KErrNone) 
    {
        switch (iState) 
        {
        case EGettingDevice: 
        case EGettingService:
        case EGettingConnection:
        case EDisconnecting: 
        case EWaitingToSend:    
            User::RequestComplete(iObserverStatus, iStatus.Int()); // Relay error codes
            iState = EWaitingToGetDevice; // Return always to initial state
            break;
       
        default:            
            Panic(EBTObjectExchangeUnexpectedLogicState); 
            break;
        }
    }    
    else 
    {
        switch (iState)
        {   
        case EGettingDevice:                                 

            iState = EGettingService;   
            iStatus = KRequestPending;  // NOTE: Setting this should be the responsibility 
                                        // of the called function
                                        // move to CBTServiceSearcher::FindServiceL?

            iServiceSearcher->FindServiceL(iStatus); // Find the OBEX service on the target device
            SetActive();
            break;
            
        case EGettingService:
            iState = EGettingConnection;    
            ConnectToServerL();         // Connect to the OBEX service port on the target device
            break;
            
        case EGettingConnection:
            iState = EWaitingToSend;               
            User::RequestComplete(iObserverStatus, KErrNone);  // ConnectL was succesfull                          
            break;
            
        case EWaitingToSend:                 
            User::RequestComplete(iObserverStatus, KErrNone); // SendObjectL was successfull
            break;
            
        case EDisconnecting:         
            iState = EWaitingToGetDevice;            
            User::RequestComplete(iObserverStatus, KErrNone); // DisconnectL succeeded
            break;
            
        default:
            Panic(EBTObjectExchangeSdpRecordDelete);
            break;
        };
    }
}

void CObjectExchangeClient::ConnectL(TRequestStatus& aObsRequestStatus)
{
    aObsRequestStatus = KRequestPending;  // Have the observer wait for this request to complete
    iObserverStatus = &aObsRequestStatus; // Store the observer's TRequestStatus for later use 
    
    if (iState == EWaitingToGetDevice && !IsActive())
    {
        iState = EGettingDevice;
        iServiceSearcher->SelectDeviceByDiscoveryL(iStatus); // Show a device selection dialog
        SetActive();

    }
    else
    {
        // Inform the observer that connection cannot be initiated
        User::RequestComplete(iObserverStatus, KErrInUse );  
        return;
    }
}

void CObjectExchangeClient::ConnectToServerL()
{  

    TObexBluetoothProtocolInfo protocolInfo;
    
    protocolInfo.iTransport.Copy(KServerTransportName);
    protocolInfo.iAddr.SetBTAddr(iServiceSearcher->BTDevAddr());
    protocolInfo.iAddr.SetPort(iServiceSearcher->Port());
    
    if (iClient)
    {
        delete iClient;
        iClient = NULL;
    }
    iClient = CObexClient::NewL(protocolInfo);
    
    iClient->Connect(iStatus);
    SetActive();

}

void CObjectExchangeClient::SendObjectL( const TDesC& aFileName, TRequestStatus& aObsRequestStatus)
{
    aObsRequestStatus = KRequestPending;  // Have the observer wait for this request to complete
    iObserverStatus = &aObsRequestStatus; // Store the observer's TRequestStatus for later use 
    
    if (iState != EWaitingToSend)
    {
        // Inform the user that sending is not possible because there is no connection
        User::RequestComplete(iObserverStatus, KErrDisconnected);  
        return;
     
    }
    else if (IsActive()) 
    {
        // Inform the user that there is already an outstanding request on this object
        User::RequestComplete(iObserverStatus, KErrInUse);              
        return;
    }
    
    delete iCurrObject;
    iCurrObject = NULL;
    
    TRACE((aFileName));

	iParse.Set( aFileName, NULL, NULL );
    
    // Create the OBEX object from the file
    TRAPD(err, iCurrObject = CObexFileObject::NewL(aFileName));
    if (err) 
    {
        TRACE((_L("_DEBUG: Error creating CObexFileObject"))); 

        // Inform the observer that file was not found
        User::RequestComplete(iObserverStatus, err);
        return;
        
    }
    else
    {

        /* Set proper values for the object info ? */

        // Fill in the object info        
        iCurrObject->SetDescriptionL(_L("OBEX file object")); 
        iCurrObject->SetNameL( iParse.NameAndExt());  
        iCurrObject->SetTypeL(_L8("text/plain"));     // NOTE: Sending everything as text/plain
        
         // Update the header mask
        TObexHeaderMask headermask = iCurrObject->HeaderMask(); // get current header mask
        headermask = headermask | KObexHdrName | KObexHdrType | KObexHdrDescription;
        iCurrObject->SetHeaderMask(headermask);    
        
        
    }    

    // Send the object
    iClient->Put(*iCurrObject, iStatus);
    SetActive();
    
}


void CObjectExchangeClient::StopL()
{

    if (iClient && iClient->IsConnected())
    {
        iClient->Abort();
        iState = EWaitingToGetDevice;
    }
}

void CObjectExchangeClient::DisconnectL(TRequestStatus& aObsRequestStatus)
{    
    aObsRequestStatus = KRequestPending;  // Have the observer wait for this request to complete
    iObserverStatus = &aObsRequestStatus; // Store the observer's TRequestStatus for later use 

    if (iState == EWaitingToGetDevice)
    {
        // Not connected, inform the observer
        User::RequestComplete(iObserverStatus, KErrNone);
        return;                           
    }

    if (iState == EWaitingToSend)
    {
        iState = EDisconnecting;
        iClient->Disconnect(iStatus);
        SetActive();

    }
    
    
    else 
    {
        // Busy connecting or disconnecting
        User::RequestComplete(iObserverStatus, KErrInUse);
        return;
        
        
    }
}

TBool CObjectExchangeClient::IsBusy()
{
    return IsActive();
}

TBool CObjectExchangeClient::IsConnected()
{
    return iState == EWaitingToSend;

}

