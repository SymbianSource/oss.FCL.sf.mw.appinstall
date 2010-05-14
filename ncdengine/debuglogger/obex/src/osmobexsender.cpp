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
* Description:   ?Description
*
*/


// Class for testing the asynchronous api of CObjectExchange client

#include "OsmObexSender.h"
#include "ObjectExchangeClient.h"


COsmObexSender* COsmObexSender::NewL( MOsmObexSenderObserver& aObserver )
{
    COsmObexSender* self = NewLC( aObserver );
    CleanupStack::Pop(self);
    return self;
}

COsmObexSender* COsmObexSender::NewLC( MOsmObexSenderObserver& aObserver )
{
    COsmObexSender* self = new (ELeave) COsmObexSender( aObserver );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

COsmObexSender::COsmObexSender( MOsmObexSenderObserver& aObserver ) : 
    CActive(CActive::EPriorityStandard),
    iState(EWaitingToConnect),  // Set initial state. Waiting for StartL function call
    iObserver( aObserver )
{
    CActiveScheduler::Add(this);
}

void COsmObexSender::ConstructL()
{
    iObexClient = CObjectExchangeClient::NewL();
}


COsmObexSender::~COsmObexSender()
{
    Cancel();
   
    delete iObexClient;
}

void COsmObexSender::DoCancel()
{
    if (iState != EWaitingToConnect && iObexClient)
    {
        iObexClient->Cancel(); // NOTE: following could do this?
    }
	iState = EWaitingToConnect;  // Return to initial state
}


void COsmObexSender::RunL()
{
    // add proper implementations for these:
    if( iStatus != KErrNone )
    {
        switch( iState )
        {
        case EWaitingToConnect:          
            iObserver.ObexDisconnected( iStatus.Int(), EWaitingToConnect );
            iState = EWaitingToConnect;
            break;
        case EConnecting: 
            iObserver.ObexDisconnected( iStatus.Int(), EConnecting );
            iState = EWaitingToConnect;
            break;
        case ESending:
            iObserver.ObexDisconnected( iStatus.Int(), ESending );
            iState = EWaitingToConnect;
            break;
        case EDisconnecting:
            iObserver.ObexDisconnected( iStatus.Int(), EDisconnecting );
            iState = EWaitingToConnect;
            break;
        default:
            // should panic here?
            break;
        }
    }
    else 
    {
        switch( iState )
        {
        case EWaitingToConnect: 
            break;
        case EConnecting: 
            iState = ESending;
            iObexClient->SendObjectL( iFileName, iStatus );
            SetActive(); 
            break;        
        case ESending:
            iState = EDisconnecting;
            iObexClient->DisconnectL( iStatus );
            SetActive();
            break;
        case EDisconnecting:
            iState = EWaitingToConnect;
            iObserver.ObexFileSent();
            // Sent successfully
            break;
        default:
            // should panic here?
            break;
        };
    }
}



void COsmObexSender::SendFileL( TOsmConnectionType aConnectionType, const TDesC& aFileName )
{
    iFileName.Copy( aFileName );

    if (iState == EWaitingToConnect)
    {
        iState = EConnecting;
        if( aConnectionType == EOsmConnectionBT )
        {
            iObexClient->ConnectL( iStatus );
        }
        else
        {
//            iObexClient->ConnectIrL( iStatus );
        }

        SetActive();
    }
}


void COsmObexSender::StopL()
{
    iObexClient->StopL();

    /* Implementation for this?? */
    /*
    if (iClient && iClient->IsConnected())
    {
        iClient->Abort();
        iState = EWaitingToGetDevice;
    }
    */
}



