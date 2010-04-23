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
*
*/


/**
 @file
*/

#include "ocspqueryrunner.h"

#include <ocsp.h>
#include "ocspsupport.h"
#include "log.h"
#include <ocsprequestandresponse.h>

namespace Swi
{
/*static*/ COcspQueryRunner* COcspQueryRunner::NewL(MOCSPTransport* aTransport, const RMessage2& aMessage, TPriority aPriority)
	{
	COcspQueryRunner* self=COcspQueryRunner::NewLC(aTransport, aMessage, aPriority);
	CleanupStack::Pop(self);
	return self;
	}
	
/*static*/ COcspQueryRunner* COcspQueryRunner::NewLC(MOCSPTransport* aTransport, const RMessage2& aMessage, TPriority aPriority)
	{
	COcspQueryRunner* self=new(ELeave)COcspQueryRunner(aTransport, aMessage, aPriority);
	CleanupStack::PushL(self);
	self->StartL();
	return self;
	}

COcspQueryRunner::COcspQueryRunner(MOCSPTransport* aTransport, const RMessage2& aMessage, TPriority aPriority)
	: CActive(aPriority), iMessage(aMessage), iTransport(aTransport)
	{
	CActiveScheduler::Add(this);
	}

COcspQueryRunner::~COcspQueryRunner()
	{
	Deque();
	delete iTransport;
	delete iUri;
	delete iRequest;
	}

void COcspQueryRunner::StartL()
	{
	delete iUri;
	iUri=0;

	iUri=HBufC8::NewL(iMessage.GetDesLengthL(0));
	TPtr8 pUri(iUri->Des());
	iMessage.ReadL(0, pUri);

	delete iRequest;
	iRequest=0;
	iRequest=HBufC8::NewL(iMessage.GetDesLengthL(1));
	TPtr8 pRequest(iRequest->Des());
	iMessage.ReadL(1, pRequest);
	
	TInt timeout = iMessage.Int2();

	SendRequest(*iUri, *iRequest, timeout);
	}

void COcspQueryRunner::RunL()
	{
	DEBUG_PRINTF2(_L8("OCSP Support - OCSP request completed with status %d."), iStatus.Int());
	// Request has finished
	iMessage.Complete(iStatus.Int());
	}
	
void COcspQueryRunner::SendRequest (const TDesC8 &aUri, const TDesC8 &aRequest, const TInt aTimeout)
	{
	DEBUG_PRINTF2(_L8("OCSP Support - Sending OCSP request to URL: %S."), &aUri);
	
	iStatus=KRequestPending;
	SetActive();
	iTransport->SendRequest(aUri, aRequest, aTimeout, iStatus);	
	}

void COcspQueryRunner::DoCancel()
	{
	DEBUG_PRINTF(_L8("OCSP Support - Cancelling OCSP request."));
	
	iTransport->CancelRequest();
	iMessage.Complete(KErrCancel);
	}

TPtrC8 COcspQueryRunner::GetResponse () const
	{
	return iTransport->GetResponse();	
	}

}
