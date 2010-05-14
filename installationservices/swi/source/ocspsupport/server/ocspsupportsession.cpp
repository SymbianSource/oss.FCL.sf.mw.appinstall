/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ocspsupportsession.h"
#include "ocspqueryrunner.h"
#include "ocspsupportserver.h"
#include "log.h"

// OCSP Support Server Commands


namespace Swi
{

COcspSupportSession* COcspSupportSession::NewL(COcspSupportServer& aServer)
	{
	COcspSupportSession* self=new (ELeave) COcspSupportSession(aServer);
	return self;
	}

COcspSupportSession* COcspSupportSession::NewLC(COcspSupportServer& aServer)
	{
	COcspSupportSession* self=new (ELeave) COcspSupportSession(aServer);
	CleanupStack::PushL(self);
	return self;
	}

COcspSupportSession::COcspSupportSession(COcspSupportServer& aServer)
	: iServer(aServer)
	{
	iServer.AddSession();
	}
	
COcspSupportSession::~COcspSupportSession()
	{
	CancelRequest();
	delete iOcspQueryRunner;
	iServer.DropSession();
	}

void COcspSupportSession::ServiceL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF2(_L8("OCSP Support - Servicing Message Type %d."), aMessage.Function());
	
	switch (aMessage.Function())
		{
	case KMessageCancelRequest:
		HandleCancelRequest(aMessage);
		break;
		
	case KMessageGetResponse:
		HandleGetResponseL(aMessage);
		break;
		
	case KMessageSendRequest:
		HandleSendRequestL(aMessage);
		break;
		}
	}

void COcspSupportSession::HandleCancelRequest(const RMessage2& aMessage)
	{
	CancelRequest();
	aMessage.Complete(KErrNone);
	}

void COcspSupportSession::HandleSendRequestL(const RMessage2& aMessage)
	{
	iIap = aMessage.Int3();
	CancelRequest();

	iOcspQueryRunner=COcspQueryRunner::NewL(iServer.CreateTransportL(iIap), aMessage);	
	}

void COcspSupportSession::HandleGetResponseL(const RMessage2& aMessage)
	{
	if (!iOcspQueryRunner)
		{
		DEBUG_PRINTF(_L8("OCSP Support - Error. Attempt to get response before response recieved."));
		aMessage.Complete(KErrNotReady);
		return;
		}

	TPtrC8 response=iOcspQueryRunner->GetResponse();
	TPckgC<TUint32> pkg(iIap);
	
	if (aMessage.GetDesMaxLengthL(0) < response.Length() )
		{
		if (aMessage.GetDesMaxLengthL(0) < static_cast<TInt>(sizeof(TUint)))
			{
			aMessage.Complete(KErrArgument);
			return;
			}
		
		DEBUG_PRINTF(_L8("OCSP Support - OCSP Respone too large for IPC descriptor. Must Re-request."));
		TUint responseSize=response.Length();
		TPckgC<TUint> responseSizePackage(responseSize);
		aMessage.WriteL(0, responseSizePackage);
		aMessage.Complete(KErrOverflow);
		}
	else
		{
		DEBUG_PRINTF(_L8("OCSP Support - Sending OCSP response to client."));
		aMessage.WriteL(0, response);
		aMessage.WriteL(1, pkg);
		aMessage.Complete(KErrNone);
		}
	}

void COcspSupportSession::CancelRequest()
	{
	if (iOcspQueryRunner)
		{
		iOcspQueryRunner->Cancel();
		delete iOcspQueryRunner;
		iOcspQueryRunner = NULL;
		}
	}

} // namespace Swi
